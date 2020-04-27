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
		// TODO : 책에서는 없는경우 다시 만들지만 나는 패킷 ID에 겜오브젝트가 없으니 패스
		LOG("PckProcessor::Process ERR_PCK_NOTFOUNDPCK_IN_HASHMAP!");
		return ERR_CODE::ERR_PCK_NOTFOUNDPCK_IN_HASHMAP;
	}
	else
	{
		(this->*m_PckIDtoFunc[id])(inPacket);
		return ERR_CODE::ERR_NONE;
	}
}

// 클라에서 보내는 함수임.
ERR_CODE PckProcessor::Process_LOGIN_REQ(const Packet& packData)
{
	char userID[128] = { 0, };
	// id찾는 쿼리 없을시 생성, 있을시 데이터 로딩 후 user에 다 집어놓고 해당 데이터와 함께 send
	DBManager::GetInst()->ProcessQuery("SELECT * FROM UserData WHERE userID = '%s'", userID);
	MYSQL_RES* res = DBManager::GetInst()->GetsqlRes();

	// TODO : 아이디 없으면 아이디 만들라는 RES , 있으면 특정 데이터를 들고 접속하라는 RES
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
		아이디가 없다 ? -> 아이디 만들라고 명령하는 패킷을 클라에 보내야함 ->
		클라가 gui를 통해 아이디 비번 입력후 해당 데이터 검증을 서버에 다시보내야함.
	*/

	return ERR_CODE::ERR_NONE;
}