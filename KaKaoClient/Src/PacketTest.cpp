#include "PacketTest.h"
using namespace std;
void PacketTest::Send()
{
	isConnected = true;
	Stream stream;

	INT16 pkid = (INT16)PACKET_ID::PCK_LOGIN_REQ;
	string userid = "sanghun219";
	string userpass = "jang5229";

	stream << pkid;
	stream << userid;
	stream << userpass;

	int sendSize = m_TCPsocket.Send(stream.data(), stream.size());

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
}

void PacketTest::Recv()
{
	if (isConnected == false)
		return;
	UCHAR copybuf[1500] = { 0, };
	Stream stream(copybuf, sizeof(copybuf));
	int recvsize = m_TCPsocket.Recv(stream.data(), stream.size());

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

	short id = 0;
	std::string ID;
	std::string nick;
	std::string pass;
	std::string friends;
	int chattingkey;
	stream >> &id;
	stream >> &ID;
	stream >> &nick;
	stream >> &pass;
	stream >> &friends;
	stream >> &chattingkey;

	LOG("id : %d", id);
	LOG("user id : %s", ID.data());
	LOG("nickname : %s", nick.data());
	LOG("password : %s", pass.data());
	LOG("friends : %s", friends.data());
	LOG("chattingkey : %d", chattingkey);
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
	Send();
	while (!isOver)
	{
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
	SocketUtil::SetSocketOption(m_TCPsocket.GetSocket());

	LOG("Client Init Complete!");
}

void PacketTest::setSockOpt()
{
	auto opt = 1;
	ioctlsocket(m_TCPsocket.GetSocket(), FIONBIO, (u_long*)&opt);
}