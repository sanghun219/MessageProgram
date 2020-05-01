#pragma once
#include "ServLibrary.h"

class SockAddress
{
private:
	SOCKADDR_IN addr;
public:

	SOCKADDR_IN GetAddr()const { return addr; }
	int GetSizeOfAddr()const { return sizeof(addr); }

public:
	SockAddress(ULONG Inaddr, int af, short port)
	{
		ZeroMemory(&addr, sizeof(addr));
		addr.sin_addr.S_un.S_addr = htonl(Inaddr);
		addr.sin_family = (ADDRESS_FAMILY)af;
		addr.sin_port = htons(port);
	}
	SockAddress(const SOCKADDR_IN& Inaddr)
	{
		addr.sin_addr.S_un.S_addr = Inaddr.sin_addr.S_un.S_addr;
		addr.sin_family = (ADDRESS_FAMILY)Inaddr.sin_family;
		addr.sin_port = Inaddr.sin_port;
	}
};
