#include "PckProcessor.h"

PckProcessor* PckProcessor::m_pInst = nullptr;

void PckProcessor::InitPckInfo(SOCKET& socket)
{
	m_PckIDtoFunc[(short)PACKET_ID::PCK_LOGIN_REQ] = &PckProcessor::Process_LOGIN_REQ;
}

ERR_CODE PckProcessor::Process(PacketData inPacket)
{
	std::lock_guard<std::recursive_mutex> _lock(m_rm);
	short id = 0;
	PacketData pckData = inPacket;

	if (m_PckIDtoFunc.find(id) == m_PckIDtoFunc.end())
	{
		// TODO : 책에서는 없는경우 다시 만들지만 나는 패킷 ID에 겜오브젝트가 없으니 패스
		LOG("PckProcessor::Process ERR_PCK_NOTFOUNDPCK_IN_HASHMAP!");
		return ERR_CODE::ERR_PCK_NOTFOUNDPCK_IN_HASHMAP;
	}
	else
	{
		(this->*m_PckIDtoFunc[id])(inPacket);
		return ERR_CODE::ERR_NONE;
	}
}

ERR_CODE PckProcessor::Process_LOGIN_REQ(PacketData packData)
{
	/*
		DataBase와 관련된 사항.
	*/

	return ERR_CODE::ERR_NONE;
}