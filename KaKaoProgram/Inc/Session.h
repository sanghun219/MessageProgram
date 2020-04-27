#pragma once
#include "ServLibrary.h"
class SockAddress;
class InputStream;
class OutputStream;
struct Session
{
	bool IsConnect() { return (SOCKET != 0) ? true : false; }

	void Clear()
	{
		idx = 0;
		seq = 0;
		std::free(address);
		SOCKET = 0;
		UserID.clear();
		ZeroMemory(&inStream, sizeof(inStream));
		ZeroMemory(&outStream, sizeof(outStream));
	}

	SESSION_ID idx = 0;
	int seq = 0;
	SOCKET SOCKET = 0;
	SockAddress* address;
	InputStream* inStream = nullptr;
	OutputStream* outStream = nullptr;
	std::string UserID;
};
