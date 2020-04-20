#pragma once
#include "ServLibrary.h"
class SockAddress;
struct Session
{
	bool IsConnect() { return (SOCKET != 0) ? true : false; }

	void Clear()
	{
		idx = 0;
		seq = 0;
		std::free(address);
		SOCKET = 0;
	}

	SESSION_ID idx = 0;
	int seq = 0;
	unsigned int SOCKET = 0;
	SockAddress* address;

	//TODO : User LoginIDµµ ÇÊ¿ä.
};