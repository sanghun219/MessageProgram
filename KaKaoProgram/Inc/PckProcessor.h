#pragma once
#include "ServLibrary.h"
#include "PacketInfo.h"
class TCPSocket;
namespace PacketProc
{
	class PckProcessor
	{
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

		~PckProcessor() {}
	};
}

// TODO : 클라 / 서버의 패킷이 들어오면 ID를 읽고 처리를 해준다.