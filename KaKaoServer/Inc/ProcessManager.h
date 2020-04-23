#pragma once
#include "ServLibrary.h"
#include "PacketInfo.h"

struct RecvPacket;
class ProcessManager
{
#pragma region SINGLETON
private:
	static ProcessManager* m_pInst;
public:
	static ProcessManager* GetInst()
	{
		if (m_pInst == nullptr)
			m_pInst = new ProcessManager();
		return m_pInst;
	}
#pragma endregion
private:

public:
	void UnpackPacket(const RecvPacket& pRcvPck);
	// TODO : DB : �Ű������� DBINFO ���;���. Server���� �ʱ�ȭ �����ش�.
	void StaticInit();
private:
	std::unordered_map<std::string, Session*> m_userIDtoSession;

private:
	ProcessManager();
public:
	~ProcessManager();
};
