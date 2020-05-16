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
	TCPSocket* fd = nullptr;
	Stream* stream = nullptr;
	//Stream* WriteStream = nullptr;

	bool IsConnect();
};
