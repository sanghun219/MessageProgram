#pragma once
#include "ServLibrary.h"
class TCPNetwork
{
public:
	void ConnectToServer();
	// 패킷을 받아서 패킷 형태로 가공해서 전달해줌.

	Packet GetPacket();
	// 소켓과 패킷이 섞여 있으니 true false로 간단하게 리턴하자
	bool ReceviePacket();
	bool SendPacket(const Packet& packet);
	void Clear();
private:
	void Update();
	void DisConnected();
private:
	SOCKET m_socket;
	std::unique_ptr<TCPSocket> m_PTCPSocket;
	std::deque<Packet> m_PacketQueue;
	std::recursive_mutex m_rm;
	std::thread m_thread;
	bool m_isConnected = false;
};
