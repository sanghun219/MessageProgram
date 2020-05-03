#pragma once
#include "ServLibrary.h"
class SockAddress;
class InputStream;
class OutputStream;
class TCPSocket;

class Session
{
public:
	void Clear();

	unsigned int idx = 0;
	int seq = 0;
	TCPSocket* fd = nullptr;
	SockAddress* address = nullptr;
	InputStream* inStream = nullptr;
	OutputStream* outStream = nullptr;
	std::string UserID;

	bool IsConnect();
};
