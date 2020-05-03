#pragma once
#include "ServLibrary.h"

class SockAddress
{
public:
	SOCKADDR_IN addr;
public:
	// 우측값참조 떄문에 쓰질 못함.
	// SOCKADDR_IN GetAddr() { return addr; }
	int GetSizeOfAddr()const { return sizeof(addr); }

public:
	SockAddress()
	{
		ZeroMemory(&addr, sizeof(addr));
	}
	SockAddress(ULONG Inaddr, int af, short port)
	{
		ZeroMemory(&addr, sizeof(addr));
		addr.sin_addr.S_un.S_addr = Inaddr;
		addr.sin_family = (ADDRESS_FAMILY)af;
		addr.sin_port = port;
	}
	SockAddress(const SOCKADDR_IN& Inaddr)
	{
		ZeroMemory(&addr, sizeof(addr));
		addr.sin_addr.S_un.S_addr = Inaddr.sin_addr.S_un.S_addr;
		addr.sin_family = (ADDRESS_FAMILY)Inaddr.sin_family;
		addr.sin_port = Inaddr.sin_port;
	}
};
