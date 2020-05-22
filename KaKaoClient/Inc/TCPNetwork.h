#pragma once
#include "ServLibrary.h"
class TCPNetwork
{
public:
	void ConnectToServer();
	// ��Ŷ�� �޾Ƽ� ��Ŷ ���·� �����ؼ� ��������.

	Packet GetPacket();
	// ���ϰ� ��Ŷ�� ���� ������ true false�� �����ϰ� ��������
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
