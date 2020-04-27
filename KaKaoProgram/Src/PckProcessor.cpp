#include "PckProcessor.h"
#include "DBManager.h"
#include "PacketInnerFunction.h"
#include "Stream.h"
PckProcessor* PckProcessor::m_pInst = nullptr;

void PckProcessor::InitPckInfo()
{
	m_PckIDtoFunc[(short)PACKET_ID::PCK_LOGIN_REQ] = &PckProcessor::Process_LOGIN_REQ;
}

ERR_CODE PckProcessor::Process(const Packet& inPacket)
{
	short id = static_cast<short>(inPacket.pckData.pkHeader.id);

	if (m_PckIDtoFunc.find(id) == m_PckIDtoFunc.end())
	{
		// TODO : å������ ���°�� �ٽ� �������� ���� ��Ŷ ID�� �׿�����Ʈ�� ������ �н�
		LOG("PckProcessor::Process ERR_PCK_NOTFOUNDPCK_IN_HASHMAP!");
		return ERR_CODE::ERR_PCK_NOTFOUNDPCK_IN_HASHMAP;
	}
	else
	{
		(this->*m_PckIDtoFunc[id])(inPacket);
		return ERR_CODE::ERR_NONE;
	}
}

// Ŭ�󿡼� ������ �Լ���.
ERR_CODE PckProcessor::Process_LOGIN_REQ(const Packet& packData)
{
	char userID[128] = { 0, };
	// idã�� ���� ������ ����, ������ ������ �ε� �� user�� �� ������� �ش� �����Ϳ� �Բ� send
	DBManager::GetInst()->ProcessQuery("SELECT * FROM UserData WHERE userID = '%s'", userID);
	MYSQL_RES* res = DBManager::GetInst()->GetsqlRes();

	// TODO : ���̵� ������ ���̵� ������ RES , ������ Ư�� �����͸� ��� �����϶�� RES
	if (mysql_fetch_row(res) == NULL)
	{
		/*

		*/
	}

	return ERR_CODE::ERR_NONE;
}

ERR_CODE PckProcess::FirstLogin(PacketData pckData, MYSQL_RES* res)
{
	/*
		���̵� ���� ? -> ���̵� ������ ����ϴ� ��Ŷ�� Ŭ�� �������� ->
		Ŭ�� gui�� ���� ���̵� ��� �Է��� �ش� ������ ������ ������ �ٽú�������.
	*/

	return ERR_CODE::ERR_NONE;
}