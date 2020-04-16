#pragma once
#include "ServLibrary.h"
#ifndef PACKETINFO_H
#define PACKETINFO_H
/*
	패킷 설계
	PACKET_ID / SESSION_ID / DATASIZE / DATA

*/

/// 진짜 단순한 요청과 응답 패킷
enum class PACKET_ID :short
{
	PCK_LOGIN_REQ = 0,
	PCK_LOGIN_RES = 1,
	PCK_ADD_FRIEND_REQ = 2,
	PCK_ADD_FRIEND_FROM_CLI_REQ = 3,
	PCK_ADD_FRIEND_RES = 4,
	PCK_ACCEPT_FRIEND_RES = 5,
	PCK_CREATE_ROOM_REQ = 6,
	PCK_CREATE_ROOM_RES = 7,
};

// RPC 수행위한 enum

#pragma pack(push,1)

struct PacketHeader
{
	PACKET_ID id;
	int sessionID;
	int dataSize;
};
int PckHeaderSize = sizeof(PacketHeader);

struct PacketData
{
	PacketHeader pkHeader;
	char* pData;
};

struct RecvPacket
{
	int sessionID;
	InputStream* inputStream;
};

#pragma pack(pop)

#endif