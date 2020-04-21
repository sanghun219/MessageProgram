#include "ProcessManager.h"
#include "TCPSocket.h"

ProcessManager* ProcessManager::m_pInst = nullptr;
ERR_CODE ProcessManager::SendHelloPacket(RecvPacket * pRcvPck)
{
	short pk_id = 0;
	pRcvPck->inputStream->Read(pk_id);
	auto reterr = ERR_CODE::ERR_NONE;
	switch (pk_id)
	{
	case (short)PACKET_ID::PCK_LOGIN_REQ:
		reterr = InvolveUserinServer(pRcvPck);
		break;
	default:
		LOG("�߸��� ��Ŷ ����");
		reterr = ERR_CODE::ERR_PCK_NOTHELLO;
		break;
	}

	return reterr;
}
ERR_CODE ProcessManager::InvolveUserinServer(RecvPacket *pRcvPck)
{
	int dataSize = 0;
	std::string userID;
	pRcvPck->inputStream->Read(dataSize);
	//TODO : ���߿� Ȯ���� ���غ��� USER_ID_MAX ���� -1����
	pRcvPck->inputStream->Read(userID);
	memcpy((void*)&pRcvPck->session->UserID, (void*)&userID, USER_ID_MAX - 1);

	return ERR_CODE();
}
void ProcessManager::UnpackPacket(RecvPacket * pRcvPck)
{
	//TODO : PacketFactory�� Wrapper�Լ��� ����, ��Ŷ�� ��°� ���⼭, ó���� PacketFactory����
	if (m_IDtoSession.find(pRcvPck->session->idx) == m_IDtoSession.end())
	{
		//���⼭ �޾ƿ� �����͵� Ŭ���̾�Ʈ���� ó������ ������ ������
		m_IDtoSession[pRcvPck->session->idx] = (pRcvPck->session);
		SendHelloPacket(pRcvPck);
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