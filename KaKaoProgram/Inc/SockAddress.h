#pragma once
#include "ServLibrary.h"
enum class SOCKET_TYPE
{
	UDP,
	TCP,
};

class SockAddress
{
private:

	SOCKET_TYPE m_SockType;
	SOCKADDR_IN addr;
public:

	SOCKET_TYPE GetSocketType()const { return m_SockType; }
	SOCKADDR_IN GetAddr()const { return addr; }
	int GetSizeOfAddr()const { return sizeof(addr); }

public:
	SockAddress(ULONG Inaddr, int af, short port)
	{
		ZeroMemory(&addr, sizeof(addr));
		addr.sin_addr.S_un.S_addr = htonl(Inaddr);
		addr.sin_family = af;
		addr.sin_port = htons(port);
	}

	SockAddress& operator = (const SockAddress& s)
	{
		this->addr.sin_addr.S_un.S_addr = s.addr.sin_addr.S_un.S_addr;
		this->addr.sin_family = s.addr.sin_family;
		this->addr.sin_port = s.addr.sin_port;

		return *this;
	}

	bool operator == (const SockAddress& o)
	{
		return ((this->addr.sin_family == AF_INET) &&
			(this->GetAddr().sin_port == o.GetAddr().sin_port) &&
			(this->GetAddr().sin_addr.S_un.S_addr == o.GetAddr().sin_addr.S_un.S_addr));
	}

	size_t GetHash()const
	{
		return (this->GetAddr().sin_addr.S_un.S_addr) |
			((static_cast<ULONG>(this->GetAddr().sin_port)) << 13)
			| this->GetAddr().sin_family;
	}
};

namespace std
{
	template<> struct hash<SockAddress>
	{
		size_t operator()(const SockAddress& inaddr)const
		{
			return inaddr.GetHash();
		}
	};
}