#include "PckProcessor.h"
#include "DBManager.h"
#include "Singleton.h"
namespace PacketProc
{
	ERR_CODE PckProcessor::Process_LOGIN_REQ(const Packet& packData)
	{
		// UserData  : UserID , Password
		std::string UserID;
		std::string UserPass;

		*packData.session->ReadStream >> &UserID;
		*packData.session->ReadStream >> &UserPass;
		std::cout << UserID << std::endl;
		// id찾는 쿼리 없을시 생성, 있을시 데이터 로딩 후 user에 다 집어놓고 해당 데이터와 함께 send
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT * FROM UserData WHERE userID = '%s'", UserID.data());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		// TODO : 아이디 없으면 아이디 만들라는 RES , 있으면 특정 데이터를 들고 접속하라는 RES
		if (mysql_fetch_row(res) == NULL)
		{
			short pkDir = static_cast<short>(PACKET_DIR::StoC);
			short pkID = static_cast<short>(PACKET_ID::PCK_MAKE_ID_RES);
			std::string msg = "등록되지 않은 ID입니다. 새로운 계정을 만들겠습니다!";
			packData.session->WriteStream = new Stream();
			*packData.session->WriteStream << pkDir;
			*packData.session->WriteStream << pkID;
			*packData.session->WriteStream << msg;

			m_sendpckQueue.push(packData);
		}
		else
		{
			/*기존 데이터베이스 자료들고 패킷에 넣은후 들고감.  */
		}

		return ERR_CODE::ERR_NONE;
	}
};