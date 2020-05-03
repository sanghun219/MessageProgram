#pragma once
#include "ServLibrary.h"
#include "PckProcessor.h"
#include "PacketInfo.h"
#include "Session.h"
class SockAddress;
class TCPSocket;
class InputStream;
class OutputStream;

enum class CLOSE_TYPE
{
	FORCING,
	ALL_SESSION,
	END,
};

using namespace PacketProc;
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
	void CloseSession(CLOSE_TYPE type, const int Sessionidx);
	void SndPacket(fd_set& wr);
	ERR_CODE ProcessSendQueue(const Packet& packet);
private:
	Config* m_pConfig;
	fd_set m_Readfds;

	std::deque<int> m_dequeSessionIndex;
	std::deque<Session> m_dequeSession;
	std::queue<Packet> m_queueRecvPacketData;
	std::queue<Packet> m_queueSendPacketData;
	TCPSocket m_tcpSocket;
	std::recursive_mutex m_rm;
	PckProcessor* m_pckProcessor;
public:
	bool InitNetworkLogic(Config* pConfig);
	bool DoRunLoop();
public:
	NetworkLogic();
	~NetworkLogic();
};
