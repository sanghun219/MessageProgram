#include "ProcessManager.h"

void ProcessManager::UnpackPacket(RecvPacket * pRcvPck)
{
	if (m_IDtoSession.find(pRcvPck->sessionID) == m_IDtoSession.end())
	{
		// 처음 연결하는 세션에 대한 로직처리
	}
	else
	{
		// 이미 연결된 세션에대한 로직처리
	}

	/*
		packet queue에 packet이 존재한다면 packet을 실행해야함.
		pk_id를 분석해서 어떤 함수를 실행시킬지 결정해야함.
	*/
}

ProcessManager::ProcessManager()
{
}

ProcessManager::~ProcessManager()
{
}