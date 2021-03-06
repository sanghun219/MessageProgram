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

	// 模备府胶飘
	PCK_FIND_ID_REQ,
	PCK_FIND_ID_RES,
	PCK_ADD_FRIEND_REQ,
	PCK_ADD_FRIEND_RES,
	PCK_FIND_NICK_REQ,
	PCK_FIND_NICK_RES,
	//盲泼规
	//PCK_CHAT_ROOM_IS_EMPTY,
	PCK_MAKE_CHATTING_ROOM_REQ,
	PCK_MAKE_CHATTING_ROOM_RES,
	PCK_SEND_CHATTING_DATA_REQ,
	PCK_SEND_CHATTING_DATA_RES,
	PCK_FIND_USERS_IN_CHATTING_ROOM_REQ,
	PCK_FIND_USERS_IN_CHATTING_ROOM_RES,

	// UPDATE 备巩
	PCK_UPDATE_CHATTING_DATA_REQ,
	PCK_UPDATE_CHATTING_DATA_RES,
	PCK_UPDATE_CLIENT,

	PCK_END,
};

struct PacketHeader
{
	short PacketID = -1;
	int SessionIdx = -1;
	UINT32 CRC = -1;
};

struct Packet
{
	PacketHeader* pkHeader = nullptr;
	Stream* stream = nullptr;
};
