#pragma once
#include "Stream.h"

enum class PACKET_ID : INT16
{
	PCK_LOGIN_REQ = 0,
	PCK_SIGN_UP_REQ = 1,
	PCK_SIGN_UP_RES = 2,
	PCK_MAKE_ID_RES = 3,
	PCK_LOGIN_RES = 4,
	PCK_LOGIN_WRONG_PASS_RES = 5,

	PCK_END,
};

// RPC 수행위한 enum

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
