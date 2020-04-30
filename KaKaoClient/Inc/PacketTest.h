#pragma once

#include "ServLibrary.h"
#include "Singleton.h"
class PacketTest
{
	friend Singleton<PacketTest>;
public:
	void Send();
	void Recv();
	void Run();
	void Init();
	void setSockOpt();
public:
	inline void SetIsOver(bool isover) { this->isOver = isover; }
private:
	bool isOver;
	bool isConnected;
	PtrTCPSocket m_ptrTCPsocket;
	SOCKET sock;
};