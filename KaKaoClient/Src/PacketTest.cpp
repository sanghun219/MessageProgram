#include "PacketTest.h"
using namespace std;
void PacketTest::Send()
{
	auto ret = m_ptrTCPsocket->Connect();

	if (ret == WSAEWOULDBLOCK)
	{
		return;
	}

	isConnected = true;
	char buf[1500];
	ZeroMemory(buf, sizeof(buf));
	cin >> buf;

	OutputStream stream;
	cout << "asdasd" << endl;
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
	clnt_addr.sin_family = AF_INET;
	clnt_addr.sin_port = 32000;

	char IP[32] = { 0, };

	//inet_ntop(AF_INET, &clnt_addr.sin_addr, IP, 32 - 1);
	inet_pton(AF_INET, "192.168.208.72", &clnt_addr.sin_addr);
	SockAddress addr(clnt_addr.sin_addr.S_un.S_addr, clnt_addr.sin_family, clnt_addr.sin_port);
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