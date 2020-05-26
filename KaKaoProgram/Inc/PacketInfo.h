#pragma once
#include "Stream.h"

enum class PACKET_ID : INT16
{
	PCK_LOGIN_REQ,
	PCK_SIGN_UP_REQ,
	PCK_SIGN_UP_RES,
	PCK_MAKE_ID_RES,
	PCK_LOGIN_RES,
	PCK_LOGIN_WRONG_PASS_RES,

	// 친구리스트
	PCK_FIND_ID_REQ,
	PCK_FIND_ID_RES,
	PCK_ADD_FRIEND_REQ,
	PCK_ADD_FRIEND_RES,
	PCK_FIND_NICK_REQ,
	PCK_FIND_NICK_RES,
	//채팅방
	//PCK_CHAT_ROOM_IS_EMPTY,
	PCK_MAKE_CHATTING_ROOM_REQ,
	PCK_MAKE_CHATTING_ROOM_RES,
	PCK_SEND_CHATTING_DATA_REQ,
	PCK_SEND_CHATTING_DATA_RES,

	PCK_END,
};

#pragma pack(push,1)

struct PacketHeader
{
	short PacketID;
	UINT32 CRC;
};

struct Packet
{
	PacketHeader pkHeader;
	Stream* stream = nullptr;
};
#pragma pack(pop)
