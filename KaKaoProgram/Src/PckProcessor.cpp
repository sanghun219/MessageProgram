#include "PckProcessor.h"
#include "DBManager.h"
#include "Stream.h"

namespace PacketProc
{
	void PckProcessor::InitPckInfo()
	{
		m_PckIDtoFunc[(short)PACKET_ID::PCK_LOGIN_REQ] = &PckProcessor::Process_LOGIN_REQ;
		m_PckIDtoFunc[(short)PACKET_ID::PCK_SIGN_UP_REQ] = &PckProcessor::Process_SIGN_UP_REQ;
		m_PckIDtoFunc[(short)PACKET_ID::PCK_SIGN_UP_RES] = &PckProcessor::Process_SIGN_UP_RES;
	}

	ERR_PCK_CODE PckProcessor::Process(const Packet& inPacket)
	{
		std::lock_guard<std::recursive_mutex> lock(m_rm);
		short id;
		*inPacket.session->ReadStream >> &id;
		if (m_PckIDtoFunc.find(id) == m_PckIDtoFunc.end())
		{
			// TODO : 책에서는 없는경우 다시 만들지만 나는 패킷 ID에 겜오브젝트가 없으니 패스
			LOG("PckProcessor::Process ERR_PCK_NOTFOUNDPCK_IN_HASHMAP!");
			return ERR_PCK_CODE::ERR_PCK_NOTFOUNDPCK_IN_HASHMAP;
		}
		else
		{
			(this->*m_PckIDtoFunc[id])(inPacket);
			return ERR_PCK_CODE::ERR_NONE;
		}
	}
}