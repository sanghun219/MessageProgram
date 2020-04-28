#include "PckProcessor.h"
#include "DBManager.h"
namespace PacketProc
{
	ERR_CODE Process_LOGIN_REQ(const Packet& packData)
	{
		char userID[128] = { 0, };
		packData.session->inStream->Read(userID, sizeof(userID));
		packData.session->UserID = userID;

		// id찾는 쿼리 없을시 생성, 있을시 데이터 로딩 후 user에 다 집어놓고 해당 데이터와 함께 send
		DBManager::GetInst()->ProcessQuery("SELECT * FROM UserData WHERE userID = '%s'", userID);
		MYSQL_RES* res = DBManager::GetInst()->GetsqlRes();

		// TODO : 아이디 없으면 아이디 만들라는 RES , 있으면 특정 데이터를 들고 접속하라는 RES
		if (mysql_fetch_row(res) == NULL)
		{
		}
		else
		{
		}

		return ERR_CODE::ERR_NONE;
	}
};