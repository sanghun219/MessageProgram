#pragma once
#include "ServLibrary.h"
#include "PckProcessor.h"
#include "PacketInfo.h"
#include "Session.h"

class SockAddress;
class TCPSocket;
class Stream;

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
	bool ReadWriteProcess(fd_set& readset, fd_set& writeset);
	bool ReceiveSocket(fd_set& readset, const size_t idx);
	void ReceivePacket(Stream& readStream, const int sessionidx);
	void ProcessRecvQueue();
	void ConnectSessionNClient(SockAddress& addr, TCPSocket& client, const int idx);
	int GetSessionIdx();
	void CreateSessionIdx();
	void CloseSession(CLOSE_TYPE type, const int Sessionidx);
	void SendPacket(fd_set& wr);
	ERR_CODE ProcessSendPacket(const Packet& packet);
private:
	Config* m_pConfig;
	fd_set m_Readfds;
	SOCKET m_servSocket;

	std::deque<int> m_dequeSessionIndex;
	std::deque<Session> m_dequeSession;
	std::queue<Packet> m_queueRecvPacketData;
	std::queue<Packet>* m_queueSendPacketData;
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
