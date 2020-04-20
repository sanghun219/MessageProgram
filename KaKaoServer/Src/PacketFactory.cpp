#include "PacketFactory.h"

void PacketFactory::InitPacketFunc()
{
	for (int i = 0; i < (int)PACKET_ID::PCK_END; i++)
		m_PacketFuncArr[i] = nullptr;

	//m_PacketFuncArr[PACKET_ID::PCK_ACCEPT_FRIEND_RES] = &
	//TODO : 패킷처리 함수들만들기
}