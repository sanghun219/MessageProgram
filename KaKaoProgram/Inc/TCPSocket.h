#pragma once
#include "ServLibrary.h"
#include "SockAddress.h"

class TCPSocket
{
private:
	SOCKET m_Socket;
	SockAddress m_addr;
	int backLog;
public:
	int Send(const char* buf, const int bufSize);
	int Recv(char* buf, const int bufSize);
	ERR_CODE Bind();
	ERR_CODE Listen(int BackLog);
	ERR_CODE Accept();
	ERR_CODE Connect();

	const SockAddress& GetSockAddr()const { return m_addr; }
	const SOCKET& GetSocket()const { return m_Socket; }
	const int GetBackLog()const { return backLog; }
public:
	TCPSocket(SOCKET inSocket, const SockAddress& addr) :
		m_addr(addr.GetAddr().sin_addr.S_un.S_addr,
			addr.GetAddr().sin_family,
			addr.GetAddr().sin_port), m_Socket(inSocket) {
	};
	~TCPSocket();
};

using PtrTCPSocket = std::shared_ptr<TCPSocket>;
