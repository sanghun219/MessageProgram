#include "PckProcessor.h"
#include "DBManager.h"
#include "Singleton.h"
#include "User.h"
#include "ChattingRoom.h"

namespace PacketProc
{
	ERR_PCK_CODE PckProcessor::Process_PCK_MAKE_CHATTING_ROOM_REQ(const Packet& RecvPacket)
	{
		// 룸아이디를 겹치지않게 만들어야함. 자동증가처럼 알아서 증가하도록.
		// 방을 만들면서 참여한 유저들 정보를 넘겨줘야함.

		Packet SendPacket;
		SendPacket.stream = new Stream();

		Singleton<DBManager>::GetInst()->ProcessQuery(QUERY_MAKE_CHATTING_ROOM().c_str());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		while (auto row = mysql_fetch_row(res))
		{
			int roomkey = atoi(row[0]);
		}
		while (mysql_next_result(Singleton<DBManager>::GetInst()->GetConn()) > 0)
		{
		}
		/*mysql_free_result(res);*/
		int numofCreatedRoom = 0;
		*RecvPacket.stream >> &numofCreatedRoom;
		for (int i = 0; i < numofCreatedRoom; i++)
		{
			// 이름들받아서 쿼리에 넣어줄것
			std::string userid;
			*RecvPacket.stream >> &userid;
			Singleton<DBManager>::GetInst()->ProcessQuery(QUERY_ROOMID_TO_USERID_INCHATTINGROOM(to_string(1).c_str(), userid.c_str()).c_str());
		}

		// 방번호 알려주자.

		short pkid = static_cast<short>(PACKET_ID::PCK_MAKE_CHATTING_ROOM_RES);
		*SendPacket.stream << pkid;
		*SendPacket.stream << numofCreatedRoom;

		m_sendpckQueue.push(SendPacket);

		return ERR_PCK_CODE::ERR_NONE;
	}

	ERR_PCK_CODE PckProcessor::Process_PCK_FIND_NICKNAME_REQ(const Packet& RecvPacket)
	{
		std::string UserID;
		*RecvPacket.stream >> &UserID;
		Packet SendPacket;
		SendPacket.stream = new Stream();
		Singleton<DBManager>::GetInst()->ProcessQuery(QUERY_FIND_NICKNAME_FROM_ID(UserID).c_str());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		if (mysql_num_rows(res) != 0)
		{
			bool isExist = true;
			std::string UserNick;
			short pkid = static_cast<short>(PACKET_ID::PCK_FIND_NICK_RES);
			auto row = mysql_fetch_row(res);
			UserNick = row[0];
			*SendPacket.stream << pkid;
			*SendPacket.stream << isExist;
			*SendPacket.stream << UserNick;
			m_sendpckQueue.push(SendPacket);
		}
		else
		{
			bool isExist = false;
			short pkid = static_cast<short>(PACKET_ID::PCK_FIND_NICK_RES);
			*SendPacket.stream << pkid;
			*SendPacket.stream << isExist;
			m_sendpckQueue.push(SendPacket);
		}
		return ERR_PCK_CODE::ERR_NONE;
	}
	ERR_PCK_CODE PckProcessor::Process_PCK_FIND_ID_REQ(const Packet& RecvPacket)
	{
		std::string UserId;
		*RecvPacket.stream >> &UserId;
		Packet SendPacket;
		SendPacket.stream = new Stream();
		Singleton<DBManager>::GetInst()->ProcessQuery(QUERY_FIND_USERID(UserId).c_str());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		if (mysql_num_rows(res) != 0)
		{
			bool isExist = true;
			auto row = mysql_fetch_row(res);
			std::string id = row[0];
			short pkid = static_cast<short>(PACKET_ID::PCK_FIND_ID_RES);
			*SendPacket.stream << pkid;
			*SendPacket.stream << isExist;
			*SendPacket.stream << id;

			m_sendpckQueue.push(SendPacket);
		}
		else
		{
			bool isExist = false;
			short pkid = static_cast<short>(PACKET_ID::PCK_FIND_ID_RES);
			*SendPacket.stream << pkid;
			*SendPacket.stream << isExist;
			m_sendpckQueue.push(SendPacket);
		}

		return ERR_PCK_CODE::ERR_NONE;
	}
	ERR_PCK_CODE PckProcessor::Process_PCK_ADD_ID_REQ(const Packet& RecvPacket)
	{
		std::string UserID, FrID;
		User user;
		Packet SendPacket;
		short pkid = static_cast<short>(PACKET_ID::PCK_ADD_FRIEND_RES);
		*RecvPacket.stream >> &UserID;
		*RecvPacket.stream >> &FrID;
		SendPacket.stream = new Stream();

		*SendPacket.stream << pkid;
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT FriendID FROM friendsinfo WHERE ID ='%s';", UserID.c_str());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		while (auto row = mysql_fetch_row(res))
		{
			if (row[0] == FrID)
			{
				bool isChanged = false;
				*SendPacket.stream << isChanged;
				m_sendpckQueue.push(SendPacket);
				return ERR_PCK_CODE::ERR_NONE;
			}
		}

		Singleton<DBManager>::GetInst()->ProcessQuery(QUERY_ADD_FRIEND(FrID, UserID).c_str());
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT * FROM userinfo WHERE ID = '%s'", FrID.c_str());
		res = Singleton<DBManager>::GetInst()->GetsqlRes();

		while (auto row = mysql_fetch_row(res))
		{
			user.SetUserID(row[0]);
			user.SetUserNick(row[1]);
		}
		bool isChanged = true;
		*SendPacket.stream << isChanged;
		user.Write(*SendPacket.stream, true);
		m_sendpckQueue.push(SendPacket);
		return ERR_PCK_CODE::ERR_NONE;
	}
}