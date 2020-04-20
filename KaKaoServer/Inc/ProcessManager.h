#pragma once
#include "ServLibrary.h"
#include "PacketInfo.h"
/*
	NetworkLogic�� ���߿� �̸��ٲ��ڰ�.
	NetworkLogic�� ���ǰ� ������ �����ϰ� �����͸� ����(���� ��Ŷ���·� �����ؼ� �޾Ƶ���������)

	ProcessManager�� ��Ŷ�� �������� ������ ����ִ� ������
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

public:
	void UnpackPacket(RecvPacket* pRcvPck);
	void DisconnectSession(const SESSION_ID id);
private:
	std::unordered_map<SESSION_ID, Session> m_IDtoSession;

private:
	ProcessManager();
public:
	~ProcessManager();
};
