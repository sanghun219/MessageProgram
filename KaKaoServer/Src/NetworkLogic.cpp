#include "NetworkLogic.h"
#include "TCPSocket.h"
#include "SocketUtil.h"
#include "Session.h"
#include "Logger.h"
#include "PacketInfo.h"
#include "ProcessManager.h"

void NetworkLogic::ReceiveSession()
{
	SOCKET client;
	SockAddress clnt_addr(0, AF_INET, 0);
	TCPSocket clientSocket(client, clnt_addr);

	auto retErr = m_PtcpSocket->Accept();
	if (retErr != ERR_CODE::ERR_NONE)
	{
		if (retErr == ERR_CODE::ERR_WOULDBLOCK)
		{
			SocketUtil::ReportError("NetworkLogic::ERR_WOULDBLOCK");
			return;
		}
		else
		{
			SocketUtil::ReportError("NetworkLogic::ReceiveSession");
			return;
		}
	}

	int ClientIdx = GetSessionIdx();
	if (ClientIdx < 0)
	{
		LOG("�ִ�� ������ �� �ִ� ������ ũ�⸦ �Ѿ���ϴ�!");
		return;
	}
	FD_SET(clientSocket.GetSocket(), &m_Readfds);

	char IP[MAX_IP] = { 0, };
	inet_ntop(AF_INET, (const void*)&clnt_addr.GetAddr(), IP, MAX_IP - 1);
	ConnectSessionNClient(clnt_addr, client, ClientIdx);
}

bool NetworkLogic::ReceivePacket(fd_set& rd, fd_set& wr)
{
	for (int i = 0; i < m_dequeSession.size(); i++)
	{
		auto& session = m_dequeSession[i];
		SOCKET fd = session.SOCKET;
		if (!FD_ISSET(fd, &rd))
			continue;

		session.seq++;

		char Packet[1500] = { 0, };
		const int packetSize = sizeof(Packet);
		InputStream inStream(packetSize * 8, Packet);

		// recv�� buf�� �ѱ�� ���� �����͸� �޾ƿ�.
		auto retSize = m_PtcpSocket->Recv(Packet, 1500);
		if (retSize == SOCKET_ERROR)
		{
			if (retSize == WSAEWOULDBLOCK)
			{
				continue;
			}
			else
			{
				SocketUtil::ReportError("NetworkLogic::ReceivePacket");
				return false;
			}
		}

		pushPakcetInQueue(inStream, session.idx);
	}
	return true;
}

void NetworkLogic::ProcessQueue()
{
	// queue�� �����Ͱ� ������ ó���Ѵ�.

	if (!m_queueRecvPacketData.empty())
	{
		auto packet = m_queueRecvPacketData.front();
		m_queueRecvPacketData.pop();
		ProcessManager::GetInst()->UnpackPacket(packet);
	}
}

void NetworkLogic::ConnectSessionNClient(SockAddress& addr, SOCKET client, const int idx)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	Session session;
	session.address = &addr;
	session.idx = idx;
	session.SOCKET = client;

	m_dequeSession.push_back(session);
	LOG("%d ������ ����Ǿ����ϴ�", session.idx);
}

int NetworkLogic::GetSessionIdx()
{
	if (m_dequeSessionIndex.empty())
	{
		return -1;
	}

	int idx = m_dequeSessionIndex.front();
	m_dequeSessionIndex.pop_front();

	return idx;
}

void NetworkLogic::CreateSessionIdx()
{
	for (int i = 0; i < m_pConfig->maxConnectSession; i++)
	{
		m_dequeSessionIndex.push_back(i);
	}
}

void NetworkLogic::pushPakcetInQueue(InputStream& inStream, const int sessionidx)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	RecvPacket rcvpkt;
	rcvpkt.session = &m_dequeSession[sessionidx];
	rcvpkt.session->inStream = &inStream;
	m_queueRecvPacketData.push(rcvpkt);
}

void NetworkLogic::CloseSession(const int Sessionidx)
{
	auto& clnt = m_dequeSession.at(Sessionidx);

	for (auto iter = m_dequeSession.begin(); iter != m_dequeSession.end(); ++iter)
	{
		if ((*iter).idx == Sessionidx)
		{
			m_dequeSession.erase(iter);
			break;
		}
	}

	m_dequeSessionIndex.push_back(clnt.idx);
	closesocket(clnt.SOCKET);
	clnt.Clear();
}

bool NetworkLogic::InitNetworkLogic(Config * pConfig)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		SocketUtil::ReportError("WSAStartup Error!");
		return false;
	}

	m_pConfig = pConfig;
	SockAddress serv_addr(INADDR_ANY, AF_INET, m_pConfig->port);

	m_PtcpSocket = std::make_shared<TCPSocket>(m_ServSocket, serv_addr);
	auto retErr = m_PtcpSocket->Listen();

	if ((retErr != ERR_CODE::ERR_NONE) && (retErr != ERR_CODE::ERR_WOULDBLOCK))
		return false;
	retErr = m_PtcpSocket->Bind();
	if (retErr != ERR_CODE::ERR_NONE)
		return false;

	FD_ZERO(&m_Readfds);
	FD_SET(m_ServSocket, &m_Readfds);

	SocketUtil::SetSocketOption(m_ServSocket);
	SocketUtil::SetSocketNonblock(m_ServSocket, true);

	if (FD_ISSET(m_ServSocket, &m_Readfds) == false)
	{
		SocketUtil::ReportError("NetworkLogic::InitNetworkLogic::fd_isset");
		return false;
	}

	CreateSessionIdx();

	LOG("InitNetworkLogic Complete!");
	return true;
}

bool NetworkLogic::DoRunLoop()
{
	fd_set fd_read, fd_write;
	timeval tv;
	tv.tv_usec = 1000;
	tv.tv_sec = 1;
	fd_read = m_Readfds;
	fd_write = m_Readfds;

	auto retErr = select(0, &fd_read, &fd_write, nullptr, &tv);
	if (retErr == SOCKET_ERROR)
	{
		SocketUtil::ReportError("NetworkLogic::DoRunLoop::Select");
		return false;
	}

	// receive packet
	// TODO : FD_ISSET�� server ������ fd_read�� ��������Ƿ� ������ true�� ��ȯ�ϴ��� �˾ƺ�����.
	if (FD_ISSET(m_ServSocket, &fd_read))
	{
		ReceiveSession();
	}

	//TODO : ��Ŷ ���� / ��Ŷ ť ó�� / ��Ŷ ���� üũ / (��Ŷ �ۼ��Ž� Ư�� ������ �˻��)

	ReceivePacket(fd_read, fd_write);
	ProcessQueue();
	return true;
}

NetworkLogic::NetworkLogic()
{
}

NetworkLogic::~NetworkLogic()
{
}