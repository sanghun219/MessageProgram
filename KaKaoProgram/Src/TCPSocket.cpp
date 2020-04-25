#include "TCPSocket.h"
#include "SocketUtil.h"
int TCPSocket::Send(const char* buf, const int bufSize)
{
	int sendSize = send(m_Socket, buf, bufSize, 0);

	// WOULD BLOCK과 연결종료 같은것들은 외부에서 처리하자.
	if (sendSize < 0)
	{
		REPORT_ERROR("TCPSocket::Send");
		return -(int)ERR_CODE::ERR_SEND;
	}

	return sendSize;
}
int TCPSocket::Recv(char* buf, const int bufSize)
{
	// WOULD BLOCK과 연결종료 같은것들은 외부에서 처리하자.
	int recvSize = recv(m_Socket, buf, bufSize, 0);
	if (recvSize < 0)
	{
		REPORT_ERROR("TCPSocket::Recv");
		return -(int)ERR_CODE::ERR_RECV;
	}

	return recvSize;
}
ERR_CODE TCPSocket::Bind()
{
	int retErr = bind(m_Socket, (const sockaddr*)&m_addr.GetAddr(), m_addr.GetSizeOfAddr());
	if (retErr == SOCKET_ERROR)
	{
		SocketUtil::ReportError("TCPSocket::Bind");
		return ERR_CODE::ERR_BIND;
	}

	return ERR_CODE::ERR_NONE;
}

ERR_CODE TCPSocket::Listen(int BackLog)
{
	int retErr = listen(m_Socket, backLog);
	if (retErr == SOCKET_ERROR)
	{
		/*if (retErr == EWOULDBLOCK)
		{
			return ERR_CODE::ERR_WOULDBLOCK;
		}*/
		REPORT_ERROR("TCPSocket::Listen");
		return ERR_CODE::ERR_LISTN;
	}
	return ERR_CODE::ERR_NONE;
}

ERR_CODE TCPSocket::Accept()
{
	int clnt_size = m_addr.GetSizeOfAddr();
	m_Socket = accept(m_Socket, (sockaddr*)&m_addr.GetAddr(), &clnt_size);
	if (m_Socket == INVALID_SOCKET)
	{
		REPORT_ERROR("TCPSocket::Accept");
		return ERR_CODE::ERR_INVALID_SOCK;
	}

	return ERR_CODE::ERR_NONE;
}

ERR_CODE TCPSocket::Connect()
{
	int retErr = connect(m_Socket, (const sockaddr*)&m_addr, sizeof(m_addr));
	if (retErr == SOCKET_ERROR)
	{
		REPORT_ERROR("TCPSocket::Connect");
		return ERR_CODE::ERR_CONNECT;
	}
	return ERR_CODE::ERR_NONE;
}

TCPSocket::~TCPSocket()
{
	closesocket(m_Socket);
}