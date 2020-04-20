#pragma once
#include "ServLibrary.h"
#include "PacketInfo.h"
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
	void PacketProcess();

private:
	using PacketFunc = ERR_CODE(*)();
	PacketFunc m_PacketFuncArr[int(PACKET_ID::PCK_END)];
};
