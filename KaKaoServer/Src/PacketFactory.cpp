#include "PacketFactory.h"

PacketFactory* PacketFactory::m_pInst = nullptr;

void PacketFactory::InitPacketFunc()
{
	//m_PacketFuncArr[PACKET_ID::PCK_ACCEPT_FRIEND_RES] = &
	// TODO : ��Ŷó�� �Լ��鸸���
}

void PacketFactory::PacketProcess(const RecvPacket & rcvPacket)
{
}