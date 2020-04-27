#pragma once
#include "TCPSocket.h"
#include "ServLibrary.h"
#include "PacketInfo.h"
class NetworkLogic
{
private:
	void ReceiveSession();
	bool ReceivePacket(fd_set& rd);
	void ProcessRecvQueue();
	void ConnectSessionNClient(SockAddress& addr, SOCKET client, const int idx);
	int GetSessionIdx();
	void CreateSessionIdx();
	void pushPakcetInQueue(InputStream& inStream, const int sessionidx);
	void CloseSession(const int Sessionidx);
	void SndPacket(fd_set& wr);
private:
	Config* m_pConfig;
	fd_set m_Readfds;
	SOCKET m_ServSocket;
	std::deque<int> m_dequeSessionIndex;
	std::deque<Session> m_dequeSession;
	std::queue<Packet> m_queueRecvPacketData;
	std::queue<Packet> m_queueSendPacketData;
	PtrTCPSocket m_PtcpSocket;
	std::recursive_mutex m_rm;

public:
	bool InitNetworkLogic(Config* pConfig);
	bool DoRunLoop();
public:
	NetworkLogic();
	~NetworkLogic();
};
