#include "NetworkLogic.h"
using namespace std;

ERR_CODE NetworkLogic::ReceiveSession()
{
	SockAddress clnt_addr;
	auto clntSocket = m_tcpSocket.Accept(clnt_addr);
	if (clntSocket == nullptr)
	{
		if (SocketUtil::GetLastError() == WSAEWOULDBLOCK)
		{
			return ERR_CODE::ERR_WOULDBLOCK;
		}
		else
		{
			SocketUtil::ReportError("NetworkLogic::Accept");
			return ERR_CODE::ERR_ACCEPT;
		}
	}
	int ClientIdx = GetSessionIdx();
	if (ClientIdx < 0)
	{
		LOG("최대로 연결할 수 있는 세션의 크기를 넘어섰습니다!");
		return ERR_CODE::ERR_OVER_ACCEPT_CLIENT;
	}

	return ConnectSessionNClient(clnt_addr, *clntSocket, ClientIdx);
}

ERR_CODE NetworkLogic::ReadWriteProcess(fd_set & readset, fd_set & writeset)
{
	for (size_t i = 0; i < m_dequeSession.size(); i++)
	{
		auto& session = m_dequeSession[i];

		if (session.IsConnect() == false)
			continue;

		auto retErr = ReceiveSocket(readset, i);
		if (IsMakeError(retErr))
		{
			CloseSession(CLOSE_TYPE::FORCING, i);
			return retErr;
		}
		else
			ProcessRecvQueue();

		// WRITE PROCESS
		SendPacket(writeset);
	}

	return ERR_CODE::ERR_NONE;
}

ERR_CODE NetworkLogic::ReceiveSocket(fd_set & readset, size_t idx)
{
	auto& session = m_dequeSession[idx];
	if (!FD_ISSET(session.fd->GetSocket(), &readset))
		return ERR_CODE::ERR_NONE;
	session.seq++;

	auto& fd = session.fd;
	UCHAR readPacket[1500];
	ZeroMemory(readPacket, sizeof(readPacket));
	session.ReadStream = new Stream(readPacket, sizeof(readPacket));

	auto retSize = fd->Recv(session.ReadStream->data(), session.ReadStream->size());

	if (retSize < 0)
	{
		if (SocketUtil::GetLastError() == WSAEWOULDBLOCK)
		{
			return ERR_CODE::ERR_WOULDBLOCK;
		}
		else
		{
			SocketUtil::ReportError("NetworkLogic::ReceivePacket");
			return ERR_CODE::ERR_RECV;
		}
	}

	ReceivePacket(*session.ReadStream, session.idx);
	return ERR_CODE::ERR_NONE;
}

void NetworkLogic::ReceivePacket(Stream& readStream, const int sessionidx)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);

	Packet rcvpkt;
	rcvpkt.session = &m_dequeSession[sessionidx];
	rcvpkt.session->ReadStream = &readStream;
	m_queueRecvPacketData.push(rcvpkt);
}

void NetworkLogic::ProcessRecvQueue()
{
	// queue에 데이터가 있으면 처리한다.
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	if (!m_queueRecvPacketData.empty())
	{
		auto packet = m_queueRecvPacketData.front();
		m_queueRecvPacketData.pop();
		m_pckProcessor->Process(packet);
	}
}

ERR_CODE NetworkLogic::ConnectSessionNClient(SockAddress& addr, TCPSocket& client, const int idx)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	Session session;
	session.idx = idx;
	session.fd = new TCPSocket(client.GetSocket(), client.GetSockAddr());

	FD_SET(session.fd->GetSocket(), &m_Readfds);
	if (!FD_ISSET(session.fd->GetSocket(), &m_Readfds))
	{
		SocketUtil::ReportError("NetworkLogic::ConnectSessionNClient!");
		return ERR_CODE::ERR_SESSION_ISNT_CONNECTED;
	}

	m_dequeSession.push_back(session);
	LOG("%d번 세션이 연결되었습니다", session.idx);
	return ERR_CODE::ERR_NONE;
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
	int maxConnection = m_pConfig->maxConnectSession;
	for (int i = 0; i < maxConnection; i++)
	{
		m_dequeSessionIndex.push_back(i);
	}
}

void NetworkLogic::CloseSession(CLOSE_TYPE type, const int Sessionidx)
{
	if (type == CLOSE_TYPE::FORCING)
	{
		closesocket(m_dequeSession[Sessionidx].fd->GetSocket());
		FD_CLR(m_dequeSession[Sessionidx].fd->GetSocket(), &m_Readfds);
		return;
	}
	else if (type == CLOSE_TYPE::ALL_SESSION)
	{
		for (size_t idx = 0; idx < m_dequeSession.size(); idx++)
		{
			auto& clnt = m_dequeSession.at(idx);
			m_dequeSessionIndex.push_back(idx);
			closesocket(clnt.fd->GetSocket());
			clnt.Clear();
		}
	}
	else
	{
		auto& clnt = m_dequeSession.at(Sessionidx);
		m_dequeSessionIndex.push_back(clnt.idx);
		closesocket(clnt.fd->GetSocket());
		clnt.Clear();
	}
}

ERR_CODE NetworkLogic::SendPacket(fd_set& wr)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);

	while (!m_queueSendPacketData->empty())
	{
		auto packet = m_queueSendPacketData->back();
		if (!FD_ISSET(packet.session->fd->GetSocket(), &wr))
			return ERR_CODE::ERR_SESSION_ISNT_CONNECTED;

		m_queueSendPacketData->pop();

		auto retErr = ProcessSendPacket(packet);
		if (IsMakeError(retErr))
		{
			SocketUtil::ReportError("NetworLogic::SndPacket");
			return retErr;
		}
	}
	return ERR_CODE::ERR_NONE;
}

ERR_CODE NetworkLogic::ProcessSendPacket(const Packet& packet)
{
	const auto& fd = packet.session->fd;
	const auto& sendpacket = packet.session->WriteStream->data();
	const size_t sendpacketSize = packet.session->WriteStream->size();

	auto retErr = fd->Send(sendpacket, sendpacketSize);
	if (retErr == SOCKET_ERROR)
	{
		if (retErr == WSAEWOULDBLOCK)
		{
			return ERR_CODE::ERR_WOULDBLOCK;
		}
		else
		{
			SocketUtil::ReportError("NetworkLogic::ProcessSendPacket");
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

	m_servSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_servSocket == INVALID_SOCKET)
	{
		SocketUtil::ReportError("socket Error!");
		return false;
	}

	m_pConfig = pConfig;

	SockAddress serv_sockaddr(INADDR_ANY, AF_INET, htons(m_pConfig->port));

	SocketUtil::SetSocketOption(m_servSocket);
	SocketUtil::SetSocketNonblock(m_servSocket, true);

	m_tcpSocket = TCPSocket(m_servSocket, serv_sockaddr);
	auto retErr = m_tcpSocket.Bind();

	if ((retErr != (int)ERR_CODE::ERR_NONE))
		return false;
	retErr = m_tcpSocket.Listen(m_pConfig->backlog);
	if (retErr != (int)ERR_CODE::ERR_NONE)
		return false;

	FD_ZERO(&m_Readfds);
	FD_SET(m_tcpSocket.GetSocket(), &m_Readfds);

	if (FD_ISSET(m_tcpSocket.GetSocket(), &m_Readfds) == false)
	{
		SocketUtil::ReportError("NetworkLogic::InitNetworkLogic::fd_isset");
		return false;
	}

	CreateSessionIdx();
	m_pckProcessor = new PckProcessor();
	m_pckProcessor->InitPckInfo();

	m_queueSendPacketData = m_pckProcessor->GetPacketQueue();
	LOG("InitNetworkLogic Complete!");
	return true;
}

bool NetworkLogic::DoRunLoop()
{
	auto fd_read = m_Readfds;
	auto fd_write = m_Readfds;

	timeval tv{ 0,1000 };
	auto retErr = select(0, &fd_read, &fd_write, NULL, &tv);

	if (retErr <= 0)
	{
		if (retErr == 0)
		{
			return true;
		}
		SocketUtil::ReportError("NetworkLogic::Select");
		CloseSession(CLOSE_TYPE::FORCING, 0);
		return false;
	}

	// receive packet

	if (FD_ISSET(m_tcpSocket.GetSocket(), &fd_read))
	{
		auto retSession = ReceiveSession();
		if (IsMakeError(retSession))
			return false;
	}

	auto CheckErr = ReadWriteProcess(fd_read, fd_write);
	if (IsMakeError(CheckErr))
		return false;

	return true;
}

NetworkLogic::NetworkLogic()
{
}

NetworkLogic::~NetworkLogic()
{
	CloseSession(CLOSE_TYPE::ALL_SESSION, 0);
	FD_CLR(m_tcpSocket.GetSocket(), &m_Readfds);
}