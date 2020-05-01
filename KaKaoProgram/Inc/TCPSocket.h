#pragma once
#include "ServLibrary.h"
class SockAddress;
class TCPSocket
{
private:
	SOCKET m_Socket;
	SockAddress* m_addr;
	int backLog;
public:
	int Send(const char* buf, const int bufSize);
	int Recv(char* buf, const int bufSize);
	int Bind();
	int Listen(int BackLog);
	int Accept();
	int Connect();

	const SockAddress& GetSockAddr() { return *m_addr; }
	SOCKET GetSocket() { return m_Socket; }
	const int GetBackLog()const { return backLog; }
public:
	TCPSocket(SOCKET& inSocket, SockAddress& addr);
	~TCPSocket();
};

using PtrTCPSocket = std::shared_ptr<TCPSocket>;
