#include "ProcessManager.h"

ProcessManager* ProcessManager::m_pInst = nullptr;
void ProcessManager::UnpackPacket(RecvPacket * pRcvPck)
{
	//TODO : PacketFactory�� Wrapper�Լ��� ����, ��Ŷ�� ��°� ���⼭, ó���� PacketFactory����
	if (m_IDtoSession.find(pRcvPck->session.idx) == m_IDtoSession.end())
	{
		m_IDtoSession[pRcvPck->session.idx] = pRcvPck->session;
	}
	else
	{
		// �̹� ����� ���ǿ����� ����ó�� -
	}
}

void ProcessManager::DisconnectSession(const SESSION_ID id)
{
	if (m_IDtoSession.find(id) != m_IDtoSession.end())
	{
		m_IDtoSession.erase(id);
	}
	else
	{
		LOG("%d ��° �ε����� �ش��ϴ� session�� �������� �ʽ��ϴ�.", id);
	}
}

ProcessManager::ProcessManager()
{
}

ProcessManager::~ProcessManager()
{
}