#include "ProcessManager.h"

void ProcessManager::UnpackPacket(RecvPacket * pRcvPck)
{
	if (m_IDtoSession.find(pRcvPck->sessionID) == m_IDtoSession.end())
	{
		// ó�� �����ϴ� ���ǿ� ���� ����ó��
	}
	else
	{
		// �̹� ����� ���ǿ����� ����ó��
	}

	/*
		packet queue�� packet�� �����Ѵٸ� packet�� �����ؾ���.
		pk_id�� �м��ؼ� � �Լ��� �����ų�� �����ؾ���.
	*/
}

ProcessManager::ProcessManager()
{
}

ProcessManager::~ProcessManager()
{
}