#pragma once
#include "ServLibrary.h"
class SockAddress;
class InputStream;
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
	}

	SESSION_ID idx = 0;
	int seq = 0;
	SOCKET SOCKET = 0;
	SockAddress* address;
	InputStream* inStream = nullptr;
	std::string UserID;
};

// TODO : DB : DB랑 맞춰서 변수 설정하자.
struct UserInfo
{
	std::string userID;
};