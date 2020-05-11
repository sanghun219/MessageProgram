#pragma once
#include "ServLibrary.h"
class Session;

/// ��¥ �ܼ��� ��û�� ���� ��Ŷ
enum class PACKET_ID : INT16
{
	PCK_LOGIN_REQ = 0,
	PCK_SIGN_UP_REQ = 1,
	PCK_SIGN_UP_RES = 2,
	PCK_MAKE_ID_RES = 3,

	PCK_END,
};

// RPC �������� enum

#pragma pack(push,1)

#define PckDataSize sizeof(PacketData);
struct Packet
{
	Session* session;
};

struct PacketHeader
{
	PACKET_ID PacketID;
	UINT32 packetSize;
	UCHAR* Contents;
	UINT32 CRC;
};

#pragma pack(pop)
