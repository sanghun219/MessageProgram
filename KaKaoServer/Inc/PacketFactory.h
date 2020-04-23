#pragma once
#include "ServLibrary.h"
#include "PacketInfo.h"

using PacketFunc = ERR_CODE(*)();
using PacketFuncTable = std::unordered_map<PACKET_ID, PacketFunc>;

class PacketFactory
{
#pragma region SINGLETON
private:
	static PacketFactory* m_pInst;
public:
	static PacketFactory* GetInst()
	{
		if (m_pInst == nullptr)
			m_pInst = new PacketFactory();
		return m_pInst;
	}
#pragma endregion
public:
	void InitPacketFunc();
	void PacketProcess(const RecvPacket& rcvPacket);

private:

	PacketFuncTable m_PacketFuncTable;
};
