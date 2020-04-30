#include "PacketTest.h"
using namespace std;
void PacketTest::Send()
{
	auto ret = connect(m_ptrTCPsocket->GetSocket(),
		(const sockaddr*)&m_ptrTCPsocket->GetSockAddr().GetAddr(),
		m_ptrTCPsocket->GetSockAddr().GetSizeOfAddr());
	if (ret == SOCKET_ERROR)
	{
		LOG("%d", WSAGetLastError());
		SocketUtil::ReportError("PacketTest::Send");
		return;
	}
	isConnected = true;
	char buf[1500];
	ZeroMemory(buf, sizeof(buf));

	cin >> buf;

	OutputStream stream;
	stream.Write((short)PACKET_DIR::CtoS);
	stream.Write((short)PACKET_ID::PCK_LOGIN_REQ);
	stream.Write(static_cast<int>(sizeof(buf)));
	stream.Write(buf, sizeof(buf));

	int sendSize = send(m_ptrTCPsocket->GetSocket(), stream.GetBuffer(), stream.GetBufferSize(), 0);

	if (sendSize < 0)
	{
		if (sendSize == WSAEWOULDBLOCK)
		{
			return;
		}
		else
		{
			SocketUtil::ReportError("PacketTest::Write");
			return;
		}
	}

	/*
		client는 순서대로 쓰고 해당 패킷을 보내기만하면됨;
	*/
}

void PacketTest::Recv()
{
	if (isConnected == false)
		return;
	char copybuf[1500] = { 0, };
	int recvsize = recv(m_ptrTCPsocket->GetSocket(), copybuf, sizeof(copybuf), 0);

	if (recvsize < 0)
	{
		if (recvsize == WSAEWOULDBLOCK)
		{
			return;
		}
		else
		{
			SocketUtil::ReportError("PacketTest::Recv");
			return;
		}
	}

	LOG("Client : %s", copybuf);
}

void PacketTest::Run()
{
	while (!isOver)
	{
		Send();
		Recv();
	}
}

void PacketTest::Init()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		SocketUtil::ReportError("PacketTest::Init");
		return;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		SocketUtil::ReportError("PacketTest::Init");
		return;
	}

	SOCKADDR_IN clnt_addr;
	clnt_addr.sin_family = AF_INET6;
	clnt_addr.sin_port = htons(32000);

	inet_pton(AF_INET6, "127.0.0.1", &clnt_addr.sin_addr);
	SockAddress addr(clnt_addr);
	isOver = false;
	isConnected = false;
	m_ptrTCPsocket = make_shared<TCPSocket>(sock, addr);

	setSockOpt();

	LOG("Client Init Complete!");
}

void PacketTest::setSockOpt()
{
	auto opt = 1;
	ioctlsocket(m_ptrTCPsocket->GetSocket(), FIONBIO, (u_long*)&opt);
}