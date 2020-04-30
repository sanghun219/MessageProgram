#pragma once
#include "ServLibrary.h"
class Session;
#ifndef PACKETINFO_H
#define PACKETINFO_H
/*
	패킷 설계
	PACKET_ID / CLASS_ID / DATASIZE / DATA

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
	PCK_FIRST_LOGIN_RES = 8,
	PCK_MAKE_ID_RES = 9,

	PCK_END,
};

enum class PACKET_DIR : short
{
	StoC,
	CtoS,
	END,
};

// RPC 수행위한 enum

#pragma pack(push,1)

struct PacketHeader
{
	PACKET_ID id;
	PACKET_DIR dir;
};

#define PckHeaderSize sizeof(PacketHeader);
struct PacketData
{
	PacketHeader pkHeader;
	int dataSize;
	char data[1500];
};
#define PckDataSize sizeof(PacketData);
struct Packet
{
	Session* session;
	PacketData pckData;
};

#pragma pack(pop)

inline Packet GetPacketSettingParam(const PACKET_ID pk_id, const PACKET_DIR pk_dir,
	const int size, const char* data, Session* session)
{
	Packet pk;
	pk.pckData.pkHeader.dir = pk_dir;
	pk.pckData.pkHeader.id = pk_id;
	pk.pckData.dataSize = size;
	memcpy(pk.pckData.data, data, sizeof(data));
	pk.session = session;
	return pk;
}

#endif