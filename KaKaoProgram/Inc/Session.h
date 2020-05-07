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
	Stream* ReadStream = nullptr;
	Stream* WriteStream = nullptr;
	UCHAR UserID[128] = { 0, };

	bool IsConnect();
};
