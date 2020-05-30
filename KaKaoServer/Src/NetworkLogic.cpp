#include "NetworkLogic.h"
using namespace std;

ERR_CODE NetworkLogic::ReceiveSession()
{
	SockAddress clnt_addr;

	TCPSocket* clntSocket = m_PtcpSocket->Accept(clnt_addr);
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
	for (auto& iter : m_pdequeSession)
	{
		auto session = iter.get();
		if (session == nullptr) continue;
		if (session->IsConnect() == false)
			continue;

		auto retErr = ReceiveSocket(readset, session->idx);
		if (IsMakeError(retErr))
		{
			CloseSession(CLOSE_TYPE::FORCING, session->idx);
			continue;
		}
		else
			ProcessRecvQueue();

		// WRITE PROCESS
		if (FD_ISSET(session->fd.get()->GetSocket(), &writeset))
			SendPacket(writeset, session->idx);
	}

	return ERR_CODE::ERR_NONE;
}

ERR_CODE NetworkLogic::ReceiveSocket(fd_set & readset, size_t idx)
{
	auto session = m_pdequeSession[idx].get();
	if (!FD_ISSET(session->fd->GetSocket(), &readset))
		return ERR_CODE::ERR_NONE;

	auto fd = session->fd.get();
	UCHAR readPacket[BUFSIZE];
	ZeroMemory(readPacket, sizeof(readPacket));
	session->stream = make_unique<Stream>(readPacket, sizeof(readPacket));

	auto retSize = fd->Recv(session->stream->data(), session->stream->size());

	if (retSize < 0)
	{
		if (SocketUtil::GetLastError() == WSAEWOULDBLOCK)
		{
			return ERR_CODE::ERR_WOULDBLOCK;
		}
		else
		{
			return ERR_CODE::ERR_RECV;
		}
	}

	ReceivePacket(session->stream.get(), idx);
	session->seq++;
	return ERR_CODE::ERR_NONE;
}

void NetworkLogic::ReceivePacket(Stream* stream, const size_t Sessionidx)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	Packet rcvpkt;
	ZeroMemory(&rcvpkt, sizeof(rcvpkt));
	rcvpkt.pkHeader = new PacketHeader();
	rcvpkt.pkHeader->SessionIdx = Sessionidx;
	rcvpkt.stream = new Stream(*stream);
	m_queueRecvPacketData.push(rcvpkt);
}

void NetworkLogic::ProcessRecvQueue()
{
	// queue에 데이터가 있으면 처리한다.
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	if (!m_queueRecvPacketData.empty())
	{
		m_queueRecvPacketData.back();
		Packet* packet = &m_queueRecvPacketData.back();
		m_queueRecvPacketData.pop();
		m_pckProcessor->Process(*packet);
	}
}

ERR_CODE NetworkLogic::ConnectSessionNClient(SockAddress& addr, TCPSocket& client, const int idx)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	Session* session;

	session = m_pdequeSession[idx].get();
	session->idx = idx;

	session->fd = std::make_unique<TCPSocket>(client.GetSocket(), client.GetSockAddr());

	FD_SET(session->fd->GetSocket(), &m_Readfds);
	FD_SET(session->fd->GetSocket(), &m_Writefds);
	if (!FD_ISSET(session->fd->GetSocket(), &m_Readfds))
	{
		SocketUtil::ReportError("NetworkLogic::ConnectSessionNClient!");
		return ERR_CODE::ERR_SESSION_ISNT_CONNECTED;
	}
	if (!FD_ISSET(session->fd->GetSocket(), &m_Writefds))
	{
		SocketUtil::ReportError("NetworkLogic::ConnectSessionNClient!");
		return ERR_CODE::ERR_SESSION_ISNT_CONNECTED;
	}

	LOG("%d번 세션이 연결되었습니다", session->idx);

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
		std::unique_ptr<Session> copy = std::make_unique<Session>();
		copy->Clear();
		copy->idx = i;
		m_pdequeSession.push_back(std::move(copy));
		m_dequeSessionIndex.push_back(i);
	}
}

void NetworkLogic::CloseSession(CLOSE_TYPE type, const int Sessionidx)
{
	if (type == CLOSE_TYPE::FORCING)
	{
		FD_CLR(m_pdequeSession[Sessionidx]->fd->GetSocket(), &m_Readfds);
		FD_CLR(m_pdequeSession[Sessionidx]->fd->GetSocket(), &m_Writefds);
		closesocket(m_pdequeSession[Sessionidx]->fd->GetSocket());
		m_pdequeSession[Sessionidx]->Clear();

		// 모든 맵 청소
		for (auto iter = m_pckProcessor->m_SocketIdxTOuserID.begin();
			iter != m_pckProcessor->m_SocketIdxTOuserID.end();)
		{
			if (iter->first == Sessionidx)
				iter = m_pckProcessor->m_SocketIdxTOuserID.erase(iter);
			else
				++iter;
		}
		for (auto iter = m_pckProcessor->m_UserIDtoSocketIdx.begin();
			iter != m_pckProcessor->m_UserIDtoSocketIdx.end();)
		{
			if (iter->second == Sessionidx)
			{
				iter = m_pckProcessor->m_UserIDtoSocketIdx.erase(iter);
				break;
			}
			else
				++iter;
		}

		// 해당 번호
		for (auto iter = m_pdequeSession.begin(); iter != m_pdequeSession.end();)
		{
			if ((*iter)->idx == Sessionidx)
			{
				(*iter)->Clear();
				break;
			}
			else
				++iter;
		}
		// Session index - login id가 묶여있음

		m_dequeSessionIndex.push_front(Sessionidx);

		LOG("%d번 세션의 연결이 해제되었습니다.", Sessionidx);
	}
	else if (type == CLOSE_TYPE::ALL_SESSION)
	{
		for (size_t idx = 0; idx < m_pdequeSession.size(); idx++)
		{
			auto clnt = m_pdequeSession.at(idx).get();
			m_dequeSessionIndex.push_back(idx);
			closesocket(clnt->fd->GetSocket());
			clnt->Clear();
			LOG("%d번 세션의 연결이 해제되었습니다.", idx);
		}
	}
}

ERR_CODE NetworkLogic::SendPacket(fd_set& wr, const int idx)
{
	auto fd = m_pdequeSession[idx]->fd.get();
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	while (!m_queueSendPacketData->empty())
	{
		if (!FD_ISSET(fd->GetSocket(), &wr))
			return ERR_CODE::ERR_SESSION_ISNT_CONNECTED;

		Packet* packet = &m_queueSendPacketData->back();
		std::cout << &packet->pkHeader << std::endl;
		std::cout << "index : " << packet->pkHeader->SessionIdx << std::endl;
		auto retErr = ProcessSendPacket(*packet, packet->pkHeader->SessionIdx);
		if (IsMakeError(retErr))
		{
			SocketUtil::ReportError("NetworLogic::SndPacket");
			return retErr;
		}
		// 비동기라서 바로바로 통과하지만 운영체제 내부에서 패킷을 처리하는중임
		//delete packet;
		m_queueSendPacketData->pop();
	}
	return ERR_CODE::ERR_NONE;
}

ERR_CODE NetworkLogic::ProcessSendPacket(const Packet& packet, const int idx)
{
	const auto fd = m_pdequeSession[idx]->fd.get();

	const auto sendpacket = packet.stream->data();
	const size_t sendpacketSize = packet.stream->size();

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
	std::cout << "보낸 데이터 크기 : " << retErr << std::endl;
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

	m_pConfig = std::make_shared<Config>(*pConfig);

	SockAddress serv_sockaddr(INADDR_ANY, AF_INET, htons(m_pConfig->port));

	SocketUtil::SetSocketOption(m_servSocket);
	SocketUtil::SetSocketNonblock(m_servSocket, true);

	m_PtcpSocket = std::make_unique<TCPSocket>(m_servSocket, serv_sockaddr);
	auto retErr = m_PtcpSocket->Bind();

	if ((retErr != (int)ERR_CODE::ERR_NONE))
		return false;
	retErr = m_PtcpSocket->Listen(m_pConfig->backlog);
	if (retErr != (int)ERR_CODE::ERR_NONE)
		return false;

	FD_ZERO(&m_Readfds);
	FD_SET(m_PtcpSocket->GetSocket(), &m_Readfds);

	if (FD_ISSET(m_PtcpSocket->GetSocket(), &m_Readfds) == false)
	{
		SocketUtil::ReportError("NetworkLogic::InitNetworkLogic::fd_isset");
		return false;
	}

	CreateSessionIdx();
	m_pckProcessor = make_unique<PacketProc::PckProcessor>();
	m_pckProcessor->InitPckInfo();

	m_queueSendPacketData = m_pckProcessor->GetPacketQueue();
	LOG("InitNetworkLogic Complete!");
	return true;
}

bool NetworkLogic::DoRunLoop()
{
	auto fd_read = m_Readfds;
	auto fd_write = m_Writefds;

	timeval tv{ 0,1000 };
	auto retErr = select(0, &fd_read, &fd_write, NULL, &tv);

	if (retErr <= 0)
	{
		if (retErr == 0)
		{
			return true;
		}
		SocketUtil::ReportError("NetworkLogic::Select");
		CloseSession(CLOSE_TYPE::ALL_SESSION, 0);
		return false;
	}

	// receive packet

	if (FD_ISSET(m_PtcpSocket->GetSocket(), &fd_read))
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
	FD_CLR(m_PtcpSocket->GetSocket(), &m_Readfds);
}