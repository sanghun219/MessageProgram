#pragma once
#include "ServLibrary.h"
class SockAddress;
class TCPSocket;
class InputStream;
class OutputStream;
class Session;
struct Packet;
class NetworkLogic
{
private:
	void ReceiveSession();
	bool ReceivePacket(fd_set& rd);
	void ProcessRecvQueue();
	void ConnectSessionNClient(SockAddress& addr, TCPSocket& client, const int idx);
	int GetSessionIdx();
	void CreateSessionIdx();
	void pushPakcetInQueue(InputStream& inStream, const int sessionidx);
	void CloseSession(const int Sessionidx);
	void SndPacket(fd_set& wr);
	ERR_CODE ProcessSendQueue(Session& session);
private:
	Config* m_pConfig;
	fd_set m_Readfds;
	SOCKET m_ServSocket;
	std::deque<int> m_dequeSessionIndex;
	std::deque<Session*> m_dequeSession;
	std::queue<Packet*> m_queueRecvPacketData;
	std::queue<Packet*> m_queueSendPacketData;
	PtrTCPSocket m_PtcpSocket;
	std::recursive_mutex m_rm;

public:
	bool InitNetworkLogic(Config* pConfig);
	bool DoRunLoop();
public:
	NetworkLogic();
	~NetworkLogic();
};
