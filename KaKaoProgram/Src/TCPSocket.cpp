#include "TCPSocket.h"
#include "ServLibrary.h"

int TCPSocket::Send(const char* buf, const int bufSize)
{
	int sendSize = send(m_Socket, buf, static_cast<int>(bufSize), 0);

	// WOULD BLOCK과 연결종료 같은것들은 외부에서 처리하자.
	if (sendSize < 0)
	{
		return -(int)ERR_CODE::ERR_SEND;
	}

	return sendSize;
}
int TCPSocket::Recv(char* buf, const int bufSize)
{
	// WOULD BLOCK과 연결종료 같은것들은 외부에서 처리하자.
	int recvSize = recv(m_Socket, buf, bufSize, 0);
	if (recvSize <= 0)
	{
		return SocketUtil::GetLastError();
	}

	return recvSize;
}
int TCPSocket::Bind()
{
	int retErr = bind(m_Socket, (const sockaddr*)&m_addr->GetAddr(), m_addr->GetSizeOfAddr());
	if (retErr == SOCKET_ERROR)
	{
		SocketUtil::ReportError("TCPSocket::Bind");
		return SocketUtil::GetLastError();
	}

	return static_cast<int>(ERR_CODE::ERR_NONE);
}

int TCPSocket::Listen(int BackLog)
{
	std::cout << BackLog << std::endl;
	int retErr = listen(m_Socket, BackLog);
	if (retErr == SOCKET_ERROR)
	{
		REPORT_ERROR("TCPSocket::Listen");
		return SocketUtil::GetLastError();
	}
	return static_cast<int>(ERR_CODE::ERR_NONE);
}

std::shared_ptr<TCPSocket> TCPSocket::Accept(SockAddress & inAddress)
{
	int addr_len = inAddress.GetSizeOfAddr();
	SOCKET clnt_socket = accept(m_Socket, (struct sockaddr*)&inAddress.GetAddr(), &addr_len);

	if (clnt_socket == INVALID_SOCKET)
	{
		return nullptr;
	}

	TCPSocket clnt_tcp(clnt_socket, inAddress);

	return PtrTCPSocket(new TCPSocket(clnt_tcp));
}

int TCPSocket::Connect()
{
	int retErr = connect(m_Socket, (const sockaddr*)&m_addr->GetAddr(), m_addr->GetSizeOfAddr());
	if (retErr == SOCKET_ERROR)
	{
		return SocketUtil::GetLastError();
	}
	return retErr;
}

TCPSocket::TCPSocket(SOCKET & inSocket, SockAddress& addr)
{
	m_Socket = inSocket;
	m_addr = new SockAddress(addr.GetAddr().sin_addr.S_un.S_addr, addr.GetAddr().sin_family
		, addr.GetAddr().sin_port);
	backLog = SOMAXCONN;
}

TCPSocket::~TCPSocket()
{
	closesocket(m_Socket);
}