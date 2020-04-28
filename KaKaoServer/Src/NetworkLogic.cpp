#include "NetworkLogic.h"
#include "PckProcessor.h"

void NetworkLogic::ReceiveSession()
{
	SOCKET socket = INVALID_SOCKET;
	SockAddress clnt_addr(0, AF_INET, 0);
	TCPSocket clientSocket(socket, clnt_addr);

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
		LOG("최대로 연결할 수 있는 세션의 크기를 넘어섰습니다!");
		return;
	}
	FD_SET(clientSocket.GetSocket(), &m_Readfds);

	char IP[MAX_IP] = { 0, };
	inet_ntop(AF_INET, (const void*)&clnt_addr.GetAddr(), IP, MAX_IP - 1);
	ConnectSessionNClient(clnt_addr, clientSocket, ClientIdx);
}

bool NetworkLogic::ReceivePacket(fd_set& rd)
{
	for (int i = 0; i < m_dequeSession.size(); i++)
	{
		auto& session = m_dequeSession[i];
		SOCKET fd = session->fd->GetSocket();
		if (session->IsConnect() == false)
			continue;

		session->seq++;

		char Packet[1500] = { 0, };
		const int packetSize = sizeof(Packet);
		InputStream inStream(packetSize * 8, Packet);

		// recv에 buf를 넘기는 순간 데이터를 받아옴.
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

		pushPakcetInQueue(inStream, session->idx);
	}
	return true;
}

void NetworkLogic::ProcessRecvQueue()
{
	// queue에 데이터가 있으면 처리한다.
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	if (!m_queueRecvPacketData.empty())
	{
		auto packet = m_queueRecvPacketData.front();
		m_queueRecvPacketData.pop();
		// TODO : 이거 정적 아닌버젼으로 다시만들라.
		//PckProcessor::GetInst()->Process(*packet);
	}
}

void NetworkLogic::ConnectSessionNClient(SockAddress& addr, TCPSocket& client, const int idx)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	Session session;
	session.address = &addr;
	session.idx = idx;
	session.fd = &client;

	m_dequeSession.push_back(&session);
	LOG("%d 세션이 연결되었습니다", session.idx);
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

	// 패킷을 까고 채워넣을건 채워넣자
	PACKET_DIR pkdir = PACKET_DIR::END;
	PACKET_ID pkId = PACKET_ID::PCK_END;
	int64_t pkheadSize = 0;
	int64_t datasize = 0;
	char data[1500] = { 0, };

	PacketData pckData;
	inStream.Read((short)pkdir);
	inStream.Read((short)pkId);

	inStream.Read((int64_t)datasize);
	inStream.Read(data, 1500);

	pckData.pkHeader.dir = pkdir;
	pckData.pkHeader.id = pkId;
	pckData.dataSize = datasize;
	memcpy(&pckData.data[0], &data[0], sizeof(data) - 1);

	Packet rcvpkt;
	rcvpkt.session = m_dequeSession[sessionidx];
	rcvpkt.session->inStream = &inStream;
	rcvpkt.pckData = pckData;
	m_queueRecvPacketData.push(&rcvpkt);
}

void NetworkLogic::CloseSession(const int Sessionidx)
{
	auto& clnt = m_dequeSession.at(Sessionidx);

	for (auto iter = m_dequeSession.begin(); iter != m_dequeSession.end(); ++iter)
	{
		if ((*iter)->idx == Sessionidx)
		{
			m_dequeSession.erase(iter);
			break;
		}
	}

	m_dequeSessionIndex.push_back(clnt->idx);
	closesocket(clnt->fd->GetSocket());
	clnt->Clear();
}

void NetworkLogic::SndPacket(fd_set & wr)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	while (!m_queueSendPacketData.empty())
	{
		auto packet = m_queueSendPacketData.back();
		m_queueSendPacketData.pop();

		if (FD_ISSET(packet->session->fd->GetSocket(), &wr))
		{
			auto err = ProcessSendQueue(*packet->session);
			if (err != ERR_CODE::ERR_NONE)
			{
				SocketUtil::ReportError("NetworLogic::SndPacket");
				return;
			}
		}
	}
}

ERR_CODE NetworkLogic::ProcessSendQueue(Session& session)
{
	if (session.IsConnect() == false)
		return ERR_CODE::ERR_SESSION_ISNT_CONNECTED;

	char* buf = session.outStream->GetBuffer();
	int64_t bufsize = session.outStream->GetBufferSize();

	int64_t sendSize = m_PtcpSocket->Send(buf, bufsize);

	if (sendSize < 0)
	{
		if (sendSize == WSAEWOULDBLOCK)
		{
			SocketUtil::ReportError("NetworkLogic::ProcessSendQueue");
			return ERR_CODE::ERR_WOULDBLOCK;
		}
		else
		{
			return ERR_CODE::ERR_SEND;
		}
	}

	return ERR_CODE::ERR_NONE;
}

bool NetworkLogic::InitNetworkLogic(Config * pConfig)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		SocketUtil::ReportError("WSAStartup Error!");
		return false;
	}

	m_ServSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ServSocket == INVALID_SOCKET)
	{
		SocketUtil::ReportError("socket Error!");
		return false;
	}

	m_pConfig = pConfig;
	SockAddress serv_addr(INADDR_ANY, AF_INET, m_pConfig->port);

	SocketUtil::SetSocketOption(m_ServSocket);
	SocketUtil::SetSocketNonblock(m_ServSocket, true);

	// TODO : 이거 정적 X 버젼으로 다심ㄴ들어라
	/*PckProcessor::GetInst()->InitPckInfo();
	PckProcessor::GetInst()->SetSendPacketQueue(m_queueSendPacketData);*/

	m_PtcpSocket = std::make_shared<TCPSocket>(m_ServSocket, serv_addr);
	auto retErr = m_PtcpSocket->Bind();

	if ((retErr != ERR_CODE::ERR_NONE))
		return false;
	retErr = m_PtcpSocket->Listen(m_pConfig->backlog);
	if (retErr != ERR_CODE::ERR_NONE)
		return false;
	FD_ZERO(&m_Readfds);
	FD_SET(m_PtcpSocket->GetSocket(), &m_Readfds);

	if (FD_ISSET(m_PtcpSocket->GetSocket(), &m_Readfds) == false)
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
	// TODO : FD_ISSET이 server 소켓이 fd_read에 담겨있으므로 무조건 true를 반환하는지 알아봐야함.
	if (FD_ISSET(m_PtcpSocket->GetSocket(), &fd_read))
	{
		ReceiveSession();
		ReceivePacket(fd_read);
		ProcessRecvQueue();
	}

	if (FD_ISSET(m_PtcpSocket->GetSocket(), &fd_write))
	{
		SndPacket(fd_write);
	}

	return true;
}

NetworkLogic::NetworkLogic()
{
}

NetworkLogic::~NetworkLogic()
{
}