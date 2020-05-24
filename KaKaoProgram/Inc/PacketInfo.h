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

	// ģ������Ʈ
	PCK_FIND_ID_REQ,
	PCK_FIND_ID_RES,
	PCK_ADD_FRIEND_REQ,
	PCK_ADD_FRIEND_RES,
	//ä�ù�
	PCK_CHAT_ROOM_IS_EMPTY,

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
