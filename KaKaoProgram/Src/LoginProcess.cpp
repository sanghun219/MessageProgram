#include "PckProcessor.h"
#include "DBManager.h"
#include "Singleton.h"
namespace PacketProc
{
	ERR_CODE PckProcessor::Process_LOGIN_REQ(const Packet& packData)
	{
		char userID[128] = { 0, };
		packData.session->inStream->Read(userID, sizeof(userID));
		packData.session->UserID = userID;

		// id찾는 쿼리 없을시 생성, 있을시 데이터 로딩 후 user에 다 집어놓고 해당 데이터와 함께 send
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT * FROM UserData WHERE userID = '%s'", userID);
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		// TODO : 아이디 없으면 아이디 만들라는 RES , 있으면 특정 데이터를 들고 접속하라는 RES
		if (mysql_fetch_row(res) == NULL)
		{
			Packet pck;
			pck = GetPacketSettingParam(PACKET_ID::PCK_MAKE_ID_RES, PACKET_DIR::StoC,
				0, nullptr, packData.session);
			m_sendpckQueue.push(pck);
		}
		else
		{
			/*기존 데이터베이스 자료들고 패킷에 넣은후 들고감.  */
		}

		return ERR_CODE::ERR_NONE;
	}
};