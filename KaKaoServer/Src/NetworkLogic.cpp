#include "NetworkLogic.h"
using namespace std;

void NetworkLogic::ReceiveSession()
{
	SockAddress clnt_addr;
	auto clntSocket = m_tcpSocket.Accept(clnt_addr);

	if (clntSocket == nullptr)
	{
		if (SocketUtil::GetLastError() == WSAEWOULDBLOCK)
		{
			return;
		}
		else
		{
			SocketUtil::ReportError("NetworkLogic::Accept");
			return;
		}
	}
	int ClientIdx = GetSessionIdx();
	if (ClientIdx < 0)
	{
		LOG("최대로 연결할 수 있는 세션의 크기를 넘어섰습니다!");
		return;
	}
	FD_SET(clntSocket->GetSocket(), &m_Readfds);

	ConnectSessionNClient(clnt_addr, *clntSocket, ClientIdx);
}

bool NetworkLogic::ReceivePacket(fd_set& rd)
{
	for (int i = 0; i < m_dequeSession.size(); i++)
	{
		auto& session = m_dequeSession[i];
		auto fd = session.fd;

		if (session.IsConnect() == false)
			continue;

		session.seq++;

		UCHAR buf[1500];
		memset(buf, 0, sizeof(buf));
		session.outStream = new Stream(buf, 1500);

		auto retSize = fd->Recv(session.outStream->data(), session.outStream->size());

		if (retSize < 0)
		{
			if (SocketUtil::GetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
			else
			{
				SocketUtil::ReportError("NetworkLogic::ReceivePacket");
				return false;
			}
		}

		INT16 pkdir = 0;
		INT16 pkid = 0;
		int datasize = 0;
		string data;
		*session.outStream >> &pkdir;
		*session.outStream >> &pkid;
		*session.outStream >> &datasize;
		*session.outStream >> &data;
		cout << pkdir << endl;
		cout << pkid << endl;
		cout << datasize << endl;
		cout << data << endl;
		// TODO : DataSize 넘겨줘야함.
		pushPakcetInQueue(*session.outStream, session.idx);
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

		m_pckProcessor->Process(packet);
	}
}

void NetworkLogic::ConnectSessionNClient(SockAddress& addr, TCPSocket& client, const int idx)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	Session session;
	session.address = &addr;
	session.idx = idx;
	session.fd = new TCPSocket(client.GetSocket(), client.GetSockAddr());

	m_dequeSession.push_back(session);
	//LOG("%d 세션이 연결되었습니다", session.idx);
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

void NetworkLogic::pushPakcetInQueue(Stream& inStream, const int sessionidx)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);

	// 패킷을 까고 채워넣을건 채워넣자
	INT16 pkdir = static_cast<INT16>(PACKET_DIR::END);
	INT16 pkId = static_cast<INT16>(PACKET_ID::PCK_END);
	INT32 pkheadSize = 0;
	INT32 datasize = 0;
	std::string data;

	PacketData pckData;
	inStream >> &pkdir;
	inStream >> &pkId;
	inStream >> &datasize;
	inStream >> &data;

	pckData.pkHeader.dir = (PACKET_DIR)pkdir;
	pckData.pkHeader.id = (PACKET_ID)pkId;
	pckData.dataSize = datasize;
	memcpy(&pckData.data[0], &data[0], sizeof(data));

	cout << "pkdir : " << static_cast<short>(pkdir) << endl;
	cout << "pkid : " << (short)pkId << endl;
	cout << "datasize : " << datasize << endl;

	Packet rcvpkt;
	rcvpkt.session = &m_dequeSession[sessionidx];
	rcvpkt.session->inStream = &inStream;
	rcvpkt.pckData = pckData;
	m_queueRecvPacketData.push(rcvpkt);
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
		for (int idx = 0; idx < m_dequeSession.size(); idx++)
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

void NetworkLogic::SndPacket(fd_set & wr)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	while (!m_queueSendPacketData.empty())
	{
		auto packet = m_queueSendPacketData.back();
		m_queueSendPacketData.pop();

		if (FD_ISSET(packet.session->fd->GetSocket(), &wr))
		{
			auto err = ProcessSendQueue(packet);
			if (err != ERR_CODE::ERR_NONE)
			{
				SocketUtil::ReportError("NetworLogic::SndPacket");
				return;
			}
		}
	}
}

ERR_CODE NetworkLogic::ProcessSendQueue(const Packet& packet)
{
	if (packet.session->IsConnect() == false)
		return ERR_CODE::ERR_SESSION_ISNT_CONNECTED;

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

	SOCKET servSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (servSocket == INVALID_SOCKET)
	{
		SocketUtil::ReportError("socket Error!");
		return false;
	}

	m_pConfig = pConfig;

	SockAddress serv_sockaddr(INADDR_ANY, AF_INET, htons(m_pConfig->port));

	FD_ZERO(&m_Readfds);
	FD_SET(servSocket, &m_Readfds);

	if (FD_ISSET(servSocket, &m_Readfds) == false)
	{
		SocketUtil::ReportError("NetworkLogic::InitNetworkLogic::fd_isset");
		return false;
	}

	SocketUtil::SetSocketOption(servSocket);
	SocketUtil::SetSocketNonblock(servSocket, true);

	m_tcpSocket = TCPSocket(servSocket, serv_sockaddr);
	auto retErr = m_tcpSocket.Bind();

	if ((retErr != (int)ERR_CODE::ERR_NONE))
		return false;
	retErr = m_tcpSocket.Listen(m_pConfig->backlog);
	if (retErr != (int)ERR_CODE::ERR_NONE)
		return false;

	CreateSessionIdx();
	m_pckProcessor = new PckProcessor();
	m_pckProcessor->InitPckInfo();
	m_pckProcessor->SetSendPacketQueue(m_queueSendPacketData);
	LOG("InitNetworkLogic Complete!");
	return true;
}

bool NetworkLogic::DoRunLoop()
{
	fd_set fd_read = m_Readfds;
	fd_set fd_write = m_Readfds;

	timeval tv{ 0,1000 };
	auto retErr = select(0, &fd_read, &fd_write, 0, &tv);

	if (retErr < 0)
	{
		SocketUtil::ReportError("NetworkLogic::Select");
		CloseSession(CLOSE_TYPE::FORCING, 0);
		return false;
	}

	// receive packet
	// TODO : FD_ISSET이 server 소켓이 fd_read에 담겨있으므로 무조건 true를 반환하는지 알아봐야함.

	if (FD_ISSET(m_tcpSocket.GetSocket(), &m_Readfds))
	{
		ReceiveSession();
		ReceivePacket(fd_read);
		ProcessRecvQueue();
	}
	else
		return true;

	SndPacket(fd_write);
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