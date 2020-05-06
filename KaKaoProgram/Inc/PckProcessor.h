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

		ERR_CODE Process(const Packet& inPacket);
		inline std::queue<Packet>* GetPacketQueue() { return &m_sendpckQueue; }
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

// TODO : Ŭ�� / ������ ��Ŷ�� ������ ID�� �а� ó���� ���ش�.