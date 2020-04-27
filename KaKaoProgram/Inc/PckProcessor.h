#pragma once
#include "ServLibrary.h"
#include "PacketInfo.h"
#include "TCPSocket.h"
class PckProcessor
{
private:
	static PckProcessor* m_pInst;
public:
	static PckProcessor* GetInst()
	{
		if (m_pInst == nullptr)
			m_pInst = new PckProcessor();
		return m_pInst;
	}

public:
	void InitPckInfo();

	void SetSendPacketQueue(const std::queue<Packet>& sendpckQueue) { this->m_sendpckQueue = sendpckQueue; }
	ERR_CODE Process(const Packet& inPacket);
private:
	ERR_CODE Process_LOGIN_REQ(const Packet& packData);
	std::recursive_mutex m_rm;
private:
	typedef ERR_CODE(PckProcessor::*PckProcessFunction)(const Packet&);
	std::unordered_map < short, PckProcessFunction> m_PckIDtoFunc;
	std::queue<Packet> m_sendpckQueue;
private:
	~PckProcessor() {}
};

// TODO : Ŭ�� / ������ ��Ŷ�� ������ ID�� �а� ó���� ���ش�.