#pragma once
#include "ServLibrary.h"
class SockAddress;
class Stream;
class TCPSocket;

class Session
{
public:
	void Clear();

	unsigned int idx = 0;
	int seq = 0;
	std::unique_ptr<TCPSocket> fd;
	std::unique_ptr<Stream> stream;

	bool IsConnect();
};
