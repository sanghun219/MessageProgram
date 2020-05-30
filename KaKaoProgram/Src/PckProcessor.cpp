#include "PckProcessor.h"
#include "DBManager.h"
#include "Stream.h"

namespace PacketProc
{
	void PckProcessor::InitPckInfo()
	{
		m_PckIDtoFunc[(short)PACKET_ID::PCK_LOGIN_REQ] = &PckProcessor::Process_LOGIN_REQ;
		m_PckIDtoFunc[(short)PACKET_ID::PCK_SIGN_UP_REQ] = &PckProcessor::Process_SIGN_UP_REQ;
		m_PckIDtoFunc[(short)PACKET_ID::PCK_FIND_ID_REQ] = &PckProcessor::Process_PCK_FIND_ID_REQ;
		m_PckIDtoFunc[(short)PACKET_ID::PCK_ADD_FRIEND_REQ] = &PckProcessor::Process_PCK_ADD_ID_REQ;
		m_PckIDtoFunc[(short)PACKET_ID::PCK_FIND_NICK_REQ] = &PckProcessor::Process_PCK_FIND_NICKNAME_REQ;
		m_PckIDtoFunc[(short)PACKET_ID::PCK_MAKE_CHATTING_ROOM_REQ] = &PckProcessor::Process_PCK_MAKE_CHATTING_ROOM_REQ;
		m_PckIDtoFunc[(short)PACKET_ID::PCK_FIND_USERS_IN_CHATTING_ROOM_REQ] = &PckProcessor::Process_PCK_FIND_USERS_IN_ROOM_REQ;
		m_PckIDtoFunc[(short)PACKET_ID::PCK_SEND_CHATTING_DATA_REQ] = &PckProcessor::Process_PCK_SEND_DATA_REQ;
		//m_PckIDtoFunc[(short)PACKET_ID::PCK_UPDATE_CLIENT] = &PckProcessor::Process_PCK_UPDATE_CLIENT;
	}

	ERR_PCK_CODE PckProcessor::Process(const Packet& inPacket)
	{
		std::lock_guard<std::recursive_mutex> lock(m_rm);
		short id;
		*inPacket.stream >> &id;
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