#include "ProcessManager.h"
#include "TCPSocket.h"
#include "PacketFactory.h"

ProcessManager* ProcessManager::m_pInst = nullptr;

void ProcessManager::UnpackPacket(const RecvPacket& pRcvPck)
{
	std::string userid;
	pRcvPck.session->inStream->Read(userid);

	// 처음 접속하는 경우
	if (m_userIDtoSession.find(userid) == m_userIDtoSession.end())
	{
		// StaticInit에 의해서 여기는 이미 hashmap에 userid가 할당됨.
		// TODO : DB : 중복 ID 검사, 중복 없으면 생성, 있으면 충돌처리 (어떻든간 SEND함수호출됨)

		m_userIDtoSession[userid] = pRcvPck.session;
		m_userIDtoSession[userid]->UserID = userid;
	}
	// 이미 아이디가 있는경우 PacketProcess 호출시킴
	else
	{
		PacketFactory::GetInst()->PacketProcess(pRcvPck);
	}
}

void ProcessManager::StaticInit()
{
	/*
		TODO : DB : DBINFO로 db정보 가져와서 hashmap에 id 다 넣어줘야함.
	*/
}

ProcessManager::ProcessManager()
{
}

ProcessManager::~ProcessManager()
{
	for (auto iter = m_userIDtoSession.begin(); iter != m_userIDtoSession.end(); ++iter)
	{
		iter->second->Clear();
	}
}