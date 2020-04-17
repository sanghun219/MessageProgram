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
private:
	static ProcessManager* m_pInst;
public:
	static ProcessManager* GetInst()
	{
		if (m_pInst == nullptr)
			m_pInst = new ProcessManager();
		return m_pInst;
	}
public:
	void UnpackPacket(RecvPacket* pRcvPck);
private:
	std::unordered_map<unsigned int, Session> m_IDtoSession;
private:
	ProcessManager();
public:
	~ProcessManager();
};

ProcessManager* ProcessManager::m_pInst = nullptr;