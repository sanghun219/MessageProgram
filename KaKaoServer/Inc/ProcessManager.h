#pragma once
#include "ServLibrary.h"
#include "PacketInfo.h"
/*
	NetworkLogic은 나중에 이름바꾸자고.
	NetworkLogic은 세션과 소켓을 연결하고 데이터를 수신(아직 패킷형태로 가공해서 받아들이지않음)

	ProcessManager는 패킷을 가공시켜 로직에 얹어주는 느낌임
*/
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
	ERR_CODE SendHelloPacket(RecvPacket*);
	ERR_CODE InvolveUserinServer(RecvPacket*);
public:
	void UnpackPacket(RecvPacket* pRcvPck);
	void DisconnectSession(const SESSION_ID id);
private:
	std::unordered_map<SESSION_ID, Session*> m_IDtoSession;

private:
	ProcessManager();
public:
	~ProcessManager();
};
