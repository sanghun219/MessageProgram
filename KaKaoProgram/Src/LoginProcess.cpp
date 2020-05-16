#include "PckProcessor.h"
#include "DBManager.h"
#include "Singleton.h"
namespace PacketProc
{
	ERR_PCK_CODE PckProcessor::Process_LOGIN_REQ(const Packet& packData)
	{
		// UserData  : UserID , Password
		std::string UserID;
		std::string UserPass;
		Stream stream = *packData.stream;

		stream >> &UserID;
		stream >> &UserPass;

		// id찾는 쿼리 없을시 생성, 있을시 데이터 로딩 후 user에 다 집어놓고 해당 데이터와 함께 send
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT * FROM userinfo WHERE ID = '%s'", UserID.data());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		// TODO : 아이디 없으면 아이디 만들라는 RES , 있으면 특정 데이터를 들고 접속하라는 RES
		if (mysql_num_fields(res) == 0)
		{
			short pkID = static_cast<short>(PACKET_ID::PCK_MAKE_ID_RES);
			std::string msg = "등록되지 않은 ID입니다. 회원가입을 하시겠습니까?";
			/*packData.session->WriteStream = new Stream();
			*packData.session->WriteStream << pkID;
			*packData.session->WriteStream << msg;*/
			stream << pkID;
			stream << msg;
			m_sendpckQueue.push(packData);
		}
		else
		{
			/*
				ID,
				USERNICKNAME,
				PASS,
				Friends,
				ChattingKey,

			*/

			MYSQL_ROW row;
			unsigned int fieldCount = mysql_num_fields(res);

			row = mysql_fetch_row(res);

			std::string id = row[0];
			std::string nickname = row[1];
			std::string pass = row[2];
			std::string friendskey;
			if (row[3] == nullptr)
			{
				friendskey = "";
			}
			else
			{
				friendskey = row[3];
			}
			int chattingkey;
			if (row[4] == nullptr)
			{
				chattingkey = -1;
			}
			else
			{
				chattingkey = atoi(row[4]);
			}

			short pkid = static_cast<short>(PACKET_ID::PCK_LOGIN_RES);

			*packData.stream << pkid;
			*packData.stream << id;
			*packData.stream << nickname;
			*packData.stream << pass;
			*packData.stream << friendskey;
			*packData.stream << chattingkey;
			m_sendpckQueue.push(packData);
		}

		return ERR_PCK_CODE::ERR_NONE;
	}
	ERR_PCK_CODE PckProcessor::Process_SIGN_UP_RES(const Packet& packData)
	{
		// 정보들이 들어왔다는 가정하에 해당 정보를 db에 입력후 다음 행해야할
		// 패킷 정보를 넘겨준다.

		/*
			ID
			UserNickname
			password
		*/

		std::string ID;
		std::string Nickname;
		std::string Password;
		Stream stream = *packData.stream;
		stream >> &ID;
		stream >> &Nickname;
		stream >> &Password;

		// 중복 ID가 발생한경우 다시 가입하라고 보냄
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT ID FROM userinfo WHERE ID = '%s';", ID);
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();
		if (mysql_num_fields(res))
		{
			// 다시 만들라는 메시지
		}
		else
		{
			// 회원가입 됐다는 메시지
		}

		return ERR_PCK_CODE::ERR_NONE;
	}
	ERR_PCK_CODE PckProcessor::Process_SIGN_UP_REQ(const Packet& packData)
	{
		return ERR_PCK_CODE::ERR_NONE;
	}
};