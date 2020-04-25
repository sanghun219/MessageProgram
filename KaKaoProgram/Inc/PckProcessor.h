#pragma once
#include "ServLibrary.h"
#include "PacketInfo.h"
#include "TCPSocket.h"
class PckProcessor
{
private:
	static PckProcessor* m_pInst;
	static PckProcessor* GetInst()
	{
		if (m_pInst == nullptr)
			m_pInst = new PckProcessor();
		return m_pInst;
	}

private:
	void InitPckInfo(SOCKET& socket);
	ERR_CODE Process(PacketData inPacket);
	ERR_CODE Process_LOGIN_REQ(PacketData packData);
	std::recursive_mutex m_rm;
private:
	typedef ERR_CODE(PckProcessor::*PckProcessFunction)(PacketData);
	std::unordered_map < short, PckProcessFunction> m_PckIDtoFunc;
	PtrTCPSocket m_pservSocket;

private:
	~PckProcessor() {}
};

// TODO : Ŭ�� / ������ ��Ŷ�� ������ ID�� �а� ó���� ���ش�.