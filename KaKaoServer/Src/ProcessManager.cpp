#include "ProcessManager.h"

ProcessManager* ProcessManager::m_pInst = nullptr;
void ProcessManager::UnpackPacket(RecvPacket * pRcvPck)
{
	//TODO : PacketFactory의 Wrapper함수들 정의, 패킷을 뜯는건 여기서, 처리는 PacketFactory에서
	if (m_IDtoSession.find(pRcvPck->session.idx) == m_IDtoSession.end())
	{
		m_IDtoSession[pRcvPck->session.idx] = pRcvPck->session;
	}
	else
	{
		// 이미 연결된 세션에대한 로직처리 -
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
		LOG("%d 번째 인덱스에 해당하는 session이 존재하지 않습니다.", id);
	}
}

ProcessManager::ProcessManager()
{
}

ProcessManager::~ProcessManager()
{
}