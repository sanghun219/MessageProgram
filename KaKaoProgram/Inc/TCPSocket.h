#pragma once
#include "ServLibrary.h"
#include "SockAddress.h"

class TCPSocket
{
private:
	SOCKET m_Socket;
	SockAddress m_addr;
public:
	int Send(const char* buf, const int bufSize);
	int Recv(char* buf, const int bufSize);
	ERR_CODE Bind();
	ERR_CODE Listen();
	ERR_CODE Accept();
	ERR_CODE Connect();

	const SockAddress& GetSockAddr()const { return m_addr; }
	const SOCKET& GetSocket()const { return m_Socket; }
public:
	TCPSocket(SOCKET& inSocket, const SockAddress& addr) :m_addr(INADDR_ANY, AF_INET, 0) { m_Socket = inSocket; };
	~TCPSocket();
};

using PtrTCPSocket = std::shared_ptr<TCPSocket>;
