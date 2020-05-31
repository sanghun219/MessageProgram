#include "TCPSocket.h"
#include "ServLibrary.h"

int TCPSocket::Send(const UCHAR* buf, const int bufSize)
{
	int sendSize = send(*m_Socket, (const char*)buf, bufSize, 0);

	if (sendSize < 0)
	{
		return -(int)ERR_CODE::ERR_SEND;
	}

	return sendSize;
}
int TCPSocket::Recv(UCHAR* buf, const int bufSize)
{
	// WOULD BLOCK과 연결종료 같은것들은 외부에서 처리하자.
	int recvSize = recv(*m_Socket, (char*)buf, bufSize, 0);

	return recvSize;
}
int TCPSocket::Bind()
{
	int retErr = bind(*m_Socket, (const sockaddr*)&m_addr->addr, m_addr->GetSizeOfAddr());
	if (retErr == SOCKET_ERROR)
	{
		SocketUtil::ReportError("TCPSocket::Bind");
		return SocketUtil::GetLastError();
	}

	return static_cast<int>(ERR_CODE::ERR_NONE);
}

int TCPSocket::Listen(int BackLog)
{
	int retErr = listen(*m_Socket, BackLog);
	if (retErr == SOCKET_ERROR)
	{
		REPORT_ERROR("TCPSocket::Listen");
		return SocketUtil::GetLastError();
	}
	return static_cast<int>(ERR_CODE::ERR_NONE);
}

TCPSocket* TCPSocket::Accept(SockAddress & inAddress)
{
	int addr_len = inAddress.GetSizeOfAddr();
	SOCKET clnt_socket = accept(*m_Socket, (struct sockaddr*)&inAddress.addr, &addr_len);

	if (clnt_socket == INVALID_SOCKET)
	{
		return nullptr;
	}
	else
	{
		TCPSocket* retSocket = new TCPSocket(clnt_socket, inAddress);
		return retSocket;
	}
}

int TCPSocket::Connect()
{
	int retErr = connect(*m_Socket, (const SOCKADDR*)&m_addr->addr, m_addr->GetSizeOfAddr());
	return retErr;
}

TCPSocket & TCPSocket::operator=(const TCPSocket & s)
{
	if (this == &s)
	{
		return *this;
	}
	this->m_addr.reset(s.m_addr.get());
	this->backLog = s.backLog;
	this->m_Socket.reset(s.m_Socket.get());
	return *this;
}

TCPSocket::TCPSocket(const SOCKET & inSocket, const SockAddress& addr)
{
	m_addr = std::make_unique<SockAddress>(addr);
	m_Socket = std::make_unique<SOCKET>(inSocket);
}

TCPSocket::TCPSocket()
{
}

TCPSocket::~TCPSocket()
{
}