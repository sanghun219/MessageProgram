#include "PacketTest.h"
using namespace std;
void PacketTest::Send()
{
	isConnected = true;
	char buf[1500];
	memset(buf, 0, sizeof(buf));

	cin >> buf;
	OutputStream stream;
	short pkdir = (short)PACKET_DIR::CtoS;
	short pkid = (short)PACKET_ID::PCK_LOGIN_REQ;
	int bufsize = sizeof(buf);
	cout << bufsize << endl;
	cout << (short)pkdir << endl;
	cout << (short)pkid << endl;

	stream.Write(pkdir);
	stream.Write(pkid);
	stream.Write(bufsize);
	stream.Write(buf);
	int sendSize = m_TCPsocket.Send(stream.GetBuffer(), stream.GetBufferSize());

	if (sendSize < 0)
	{
		if (SocketUtil::GetLastError() == WSAEWOULDBLOCK)
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
	int recvsize = recv(m_TCPsocket.GetSocket(), copybuf, sizeof(copybuf), 0);

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
	auto ret = m_TCPsocket.Connect();
	if (ret == SOCKET_ERROR)
	{
		if (SocketUtil::GetLastError() == WSAEWOULDBLOCK)
		{
		}
		else
		{
			SocketUtil::ReportError("PacketTest::test");
			return;
		}
	}
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

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
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
	isOver = false;
	isConnected = false;
	m_TCPsocket = TCPSocket(sock, addr);
	//SocketUtil::SetSocketNonblock(m_TCPsocket.m_Socket, true);
	SocketUtil::SetSocketOption(m_TCPsocket.m_Socket);

	LOG("Client Init Complete!");
}

void PacketTest::setSockOpt()
{
	auto opt = 1;
	ioctlsocket(m_TCPsocket.GetSocket(), FIONBIO, (u_long*)&opt);
}