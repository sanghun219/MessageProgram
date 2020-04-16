#pragma once
#include "ServLibrary.h"
enum class SOCKET_TYPE
{
	UDP,
	TCP,
};

struct EndPoint
{
	char ip[32];
	short port;
};
class SockAddress
{
private:
	EndPoint m_Ep;
	SOCKET_TYPE m_SockType;
	SOCKADDR_IN addr;
	int backlog;
public:
	EndPoint GetEndPoint()const { return m_Ep; }
	SOCKET_TYPE GetSocketType()const { return m_SockType; }
	SOCKADDR_IN GetAddr()const { return addr; }
	int GetSizeOfAddr()const { return sizeof(addr); }
	int GetBackLog()const { return backlog; }
public:
	SockAddress(const char* Inaddr, int af, short port)
	{
		memcpy(m_Ep.ip, Inaddr, sizeof(Inaddr));
		m_Ep.port = port;

		inet_pton(af, Inaddr, &addr.sin_addr);
		addr.sin_family = af;
		addr.sin_port = htons(port);
	}

	SockAddress& operator = (const SockAddress& s)
	{
		memcpy(m_Ep.ip, s.m_Ep.ip, sizeof(s.m_Ep.ip));
		m_Ep.port = s.m_Ep.port;
		addr = s.addr;
	}
};
