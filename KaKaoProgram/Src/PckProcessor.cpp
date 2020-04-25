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
		// TODO : å������ ���°�� �ٽ� �������� ���� ��Ŷ ID�� �׿�����Ʈ�� ������ �н�
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
		DataBase�� ���õ� ����.
	*/

	return ERR_CODE::ERR_NONE;
}