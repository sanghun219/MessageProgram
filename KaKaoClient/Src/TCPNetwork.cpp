#include "TCPNetwork.h"

void TCPNetwork::ConnectToServer()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		SocketUtil::ReportError("PacketTest::Init");
		return;
	}

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		SocketUtil::ReportError("PacketTest::Init");
		return;
	}

	SOCKADDR_IN clnt_addr;
	memset(&clnt_addr, 0, sizeof(SOCKADDR_IN));
	clnt_addr.sin_family = AF_INET;
	clnt_addr.sin_port = htons(32000);

	inet_pton(AF_INET, "127.0.0.1", &clnt_addr.sin_addr);

	SockAddress addr(clnt_addr.sin_addr.S_un.S_addr, clnt_addr.sin_family, clnt_addr.sin_port);

	m_PTCPSocket = std::make_unique<TCPSocket>(m_socket, addr);
	//SocketUtil::SetSocketNonblock(m_TCPsocket.m_Socket, true);
	SocketUtil::SetSocketOption(m_PTCPSocket->GetSocket());
	auto connerr = m_PTCPSocket->Connect();
	if (connerr == SOCKET_ERROR)
	{
		SocketUtil::ReportError("TCPNetwork::InitSocket");
	}
	m_isConnected = true;
	m_thread = std::thread([&]() {Update(); });

	LOG("Client Init Complete!");
}

// 패킷이 있다면 (receive 할) 먼저온 순서대로 return 한다.
Packet TCPNetwork::GetPacket()
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	if (!m_PacketQueue.empty())
	{
		Packet retpacket = m_PacketQueue.back();
		m_PacketQueue.pop_back();
		return retpacket;
	}

	return Packet();
}

// 소켓으로 데이터를 받고 스트림에 채워넣은후 큐에 넣어준다.
bool TCPNetwork::ReceviePacket()
{
	fd_set readset;
	timeval tv{ 0,1000 };

	FD_ZERO(&readset);
	FD_SET(m_PTCPSocket->GetSocket(), &readset);

	auto retsel = select(0, &readset, nullptr, nullptr, &tv);
	if (retsel <= 0)
	{
		if (retsel == 0)
		{
			return true;
		}
		else
		{
			SocketUtil::ReportError("TCPNetwork::ReceivePacket");
			return false;
		}
	}

	if (FD_ISSET(m_PTCPSocket->GetSocket(), &readset))
	{
		UCHAR data[1500];

		ZeroMemory(data, sizeof(data));
		int recvret = m_PTCPSocket->Recv(data, sizeof(data));

		if (recvret <= 0)
		{
			if (SocketUtil::GetLastError() == WSAEWOULDBLOCK)
			{
				return true;
			}
			else
			{
				SocketUtil::ReportError("TCPNetwork::ReceivePacket");
				return false;
			}
		}
		std::lock_guard<std::recursive_mutex> lock(m_rm);
		Packet packet;
		ZeroMemory(&packet, sizeof(packet));
		packet.stream = new Stream(data, recvret);
		m_PacketQueue.push_back(packet);
	}

	return true;
}

// 버튼 동작이 있을때마다 혹은 데이터 입력시 패킷을 넘겨준다.
bool TCPNetwork::SendPacket(const Packet& packet)
{
	int retsend = m_PTCPSocket->Send(packet.stream->data(), packet.stream->size());

	if (retsend <= 0)
	{
		if (retsend == WSAEWOULDBLOCK)
			return true;
		SocketUtil::ReportError("TCPNetwork::SendPacket");
		return false;
	}

	return true;
}

void TCPNetwork::Update()
{
	while (m_isConnected)
	{
		if (!ReceviePacket())
			break;
	}
}

void TCPNetwork::DisConnected()
{
	if (m_isConnected)
	{
		closesocket(m_socket);
		Clear();
	}

	if (m_thread.joinable())
		m_thread.join();
}

void TCPNetwork::Clear()
{
	m_isConnected = false;
	m_PacketQueue.clear();
}