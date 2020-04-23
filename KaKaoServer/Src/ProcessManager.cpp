#include "ProcessManager.h"
#include "TCPSocket.h"
#include "PacketFactory.h"

ProcessManager* ProcessManager::m_pInst = nullptr;

void ProcessManager::UnpackPacket(const RecvPacket& pRcvPck)
{
	std::string userid;
	pRcvPck.session->inStream->Read(userid);

	// ó�� �����ϴ� ���
	if (m_userIDtoSession.find(userid) == m_userIDtoSession.end())
	{
		// StaticInit�� ���ؼ� ����� �̹� hashmap�� userid�� �Ҵ��.
		// TODO : DB : �ߺ� ID �˻�, �ߺ� ������ ����, ������ �浹ó�� (��簣 SEND�Լ�ȣ���)

		m_userIDtoSession[userid] = pRcvPck.session;
		m_userIDtoSession[userid]->UserID = userid;
	}
	// �̹� ���̵� �ִ°�� PacketProcess ȣ���Ŵ
	else
	{
		PacketFactory::GetInst()->PacketProcess(pRcvPck);
	}
}

void ProcessManager::StaticInit()
{
	/*
		TODO : DB : DBINFO�� db���� �����ͼ� hashmap�� id �� �־������.
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