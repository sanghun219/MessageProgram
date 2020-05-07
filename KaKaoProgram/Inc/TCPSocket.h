#pragma once
#include "ServLibrary.h"
class SockAddress;
class TCPSocket
{
private:
	SOCKET* m_Socket;
	SockAddress* m_addr;
	int backLog;
public:
	int Send(const UCHAR* buf, const int bufSize);
	int Recv(UCHAR* buf, const int bufSize);
	int Bind();
	int Listen(int BackLog);
	TCPSocket* Accept(SockAddress& inAddress);
	int Connect();

	inline SockAddress& GetSockAddr() { return *m_addr; }
	inline  SOCKET& GetSocket() { return *m_Socket; }
	inline const int GetBackLog()const { return backLog; }
public:
	TCPSocket& operator = (const TCPSocket& s);

	TCPSocket(const SOCKET& inSocket, const SockAddress& addr);
	TCPSocket();
	~TCPSocket();
};
