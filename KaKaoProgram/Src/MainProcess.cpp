#include "PckProcessor.h"
#include "DBManager.h"
#include "Singleton.h"
#include "User.h"
#include "ChattingRoom.h"

namespace PacketProc
{// 현재 사용되지 않습니다
	ERR_PCK_CODE PckProcessor::Process_PCK_FIND_USERS_IN_ROOM_REQ(const Packet& RecvPacket)
	{
		return ERR_PCK_CODE::ERR_NONE;
	}

	//	ERR_PCK_CODE PckProcessor::Process_PCK_UPDATE_CLIENT(const Packet& RecvPacket)
	//	{
	//		// UserData  : UserID , Password
	//		std::string UserID;
	//
	//		*RecvPacket.stream >> &UserID;
	//
	//		Packet SendPacket;
	//		SendPacket.stream = new Stream();
	//		SendPacket.pkHeader.SessionIdx = RecvPacket.pkHeader.SessionIdx;
	//
	//		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT * FROM userinfo WHERE ID = '%s'", UserID.data());
	//		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();
	//		MYSQL_ROW row;
	//		unsigned int fieldCount = mysql_num_fields(res);
	//
	//		row = mysql_fetch_row(res);
	//
	//		User* user = new User();
	//		std::string id = row[0];
	//		std::string nickname = row[1];
	//		std::string pass = row[2];
	//		std::vector<ChattingRoom*> chattingRoomList;
	//
	//		user->SetUserID(id);
	//		user->SetUserNick(nickname);
	//		user->SetUserPass(pass);
	//#pragma region 친구목록 조회
	//		auto reterr = Singleton<DBManager>::GetInst()->
	//			ProcessQuery(QUERY_FIND_FRIENDS_INFO(id).c_str());
	//
	//		if (reterr < 0)
	//		{
	//			return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
	//		}
	//
	//		res = Singleton<DBManager>::GetInst()->GetsqlRes();
	//
	//		while ((row = mysql_fetch_row(res)))
	//		{
	//			std::string friendsID = row[0];
	//			std::string friendsNickName = row[1];
	//			User* myFriend = new User(friendsID, friendsNickName);
	//			user->SetFriendList(myFriend);
	//		}
	//#pragma endregion
	//
	//#pragma region 채팅방목록 조회
	//		// 채팅방 ID 찾기
	//		reterr = Singleton<DBManager>::GetInst()->
	//			ProcessQuery(QUERY_FIND_CHATTINGROOM_FROM_USERID(id).c_str());
	//
	//		if (reterr < 0)
	//		{
	//			return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
	//		}
	//
	//		res = Singleton<DBManager>::GetInst()->GetsqlRes();
	//		fieldCount = mysql_num_fields(res);
	//		while (row = mysql_fetch_row(res))
	//		{
	//			ChattingRoom* room = new ChattingRoom();
	//			INT64 RoomID = atoi(row[0]);
	//			// 룸 네임이 없으면
	//			if (row[1] != nullptr)
	//			{
	//				std::string RoomName = row[1];
	//				room->SetRoomName(RoomName);
	//			}
	//			room->SetRoomID(RoomID);
	//			chattingRoomList.push_back(room);
	//		}
	//
	//		for (auto iter : chattingRoomList)
	//		{
	//			reterr = Singleton<DBManager>::GetInst()->
	//				ProcessQuery(QUERY_FIND_JOINNEDUSERS_FROM_ROOMID(iter->GetRoomID()).c_str());
	//
	//			if (reterr < 0)
	//			{
	//				return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
	//			}
	//
	//			res = Singleton<DBManager>::GetInst()->GetsqlRes();
	//			fieldCount = mysql_num_fields(res);
	//
	//			while (row = mysql_fetch_row(res))
	//			{
	//				iter->SetJoinnedUser(row[0]);
	//			}
	//
	//			reterr = Singleton<DBManager>::GetInst()->
	//				ProcessQuery(QUERY_FIND_CHATTINGDATAS_IN_CHATTINGROOM(iter->GetRoomID()).c_str());
	//
	//			if (reterr < 0)
	//			{
	//				return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
	//			}
	//			fieldCount = mysql_num_rows(res);
	//			res = Singleton<DBManager>::GetInst()->GetsqlRes();
	//			if (fieldCount == 0)
	//			{
	//			}
	//			else
	//			{
	//				while (row = mysql_fetch_row(res))
	//				{
	//					ChattingData chatdata;
	//					chatdata.m_RoomID = atoi(row[0]);
	//					chatdata.m_Sequence = atoi(row[1]);
	//					chatdata.m_Senddate = row[2];
	//					chatdata.m_UserID = row[3];
	//					chatdata.m_Nickname = row[4];
	//					chatdata.m_Contents = row[5];
	//					iter->SetChatData(chatdata);
	//				}
	//			}
	//		}
	//
	//#pragma endregion
	//		user->SetChattingRoomList(chattingRoomList);
	//		short pkid = static_cast<short>(PACKET_ID::PCK_UPDATE_CLIENT);
	//		*SendPacket.stream << pkid;
	//
	//		user->Write(*SendPacket.stream);
	//		m_sendpckQueue.push(SendPacket);
	//
	//		return ERR_PCK_CODE::ERR_NONE;
	//	}

	ERR_PCK_CODE PckProcessor::Process_PCK_SEND_DATA_REQ(const Packet& RecvPacket)
	{
		// 클라이언트 2개로 진행해야할듯? 일단 서버에 데이터가 올라가는지 부터 확인한다
		std::lock_guard<std::recursive_mutex> lock(m_rm);
		int roomid = -1;
		std::string senddate;
		std::string id;
		std::string nick;
		std::string contents;
		*RecvPacket.stream >> &roomid;
		*RecvPacket.stream >> &senddate;
		*RecvPacket.stream >> &id;
		*RecvPacket.stream >> &nick;
		*RecvPacket.stream >> &contents;

		Singleton<DBManager>::GetInst()->ProcessQuery("INSERT INTO chattingdata (RoomID,\
			SendDate,ID,Nickname,Contents) SELECT %d ,'%s','%s','%s','%s'", roomid,
			senddate.c_str(), id.c_str(), nick.c_str(), contents.c_str());

		// room에 있는 모든 이에게 데이터를 전송할거임.

		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT UserID FROM userinchatroom WHERE roomID = %d AND Userid <> '%s'", roomid, id.c_str());
		std::vector<std::string> usersInRoom;
		auto res = Singleton<DBManager>::GetInst()->GetsqlRes();

		while (auto row = mysql_fetch_row(res))
		{
			usersInRoom.push_back(row[0]);
		}
		short pkid = static_cast<short>(PACKET_ID::PCK_UPDATE_CHATTING_DATA_RES);

		for (auto iter : usersInRoom)
		{
			std::recursive_mutex test;
			std::unordered_map<std::string, size_t>::const_iterator it = m_UserIDtoSocketIdx.find(iter);
			if (it == m_UserIDtoSocketIdx.end())continue;
			std::lock_guard<std::recursive_mutex> lock(test);
			size_t sessionidx = m_UserIDtoSocketIdx[iter];
			std::cout << "Session IDX : " << sessionidx << std::endl;
			Packet SendPacket;
			ZeroMemory(&SendPacket, sizeof(SendPacket));
			SendPacket.stream = new Stream();
			SendPacket.SessionIdx = new int();
			*SendPacket.SessionIdx = sessionidx;
			SendPacket.pkHeader = new PacketHeader();
			SendPacket.pkHeader->SessionIdx = sessionidx;
			std::cout << "주소 : " << &SendPacket.pkHeader << std::endl;
			std::cout << "SendPacket Sessionidx: " << sessionidx << std::endl;
			*SendPacket.stream << pkid;
			*SendPacket.stream << roomid;
			*SendPacket.stream << senddate;
			*SendPacket.stream << id;
			*SendPacket.stream << nick;
			*SendPacket.stream << contents;
			std::cout << SendPacket.stream->size() << std::endl;
			m_sendpckQueue.push(SendPacket);
		}

		return ERR_PCK_CODE::ERR_NONE;
	}

	ERR_PCK_CODE PckProcessor::Process_PCK_MAKE_CHATTING_ROOM_REQ(const Packet& RecvPacket)
	{
		// 참여한 인원 / 참여 인원 id / 본인 id

		Packet SendPacket;
		SendPacket.stream = new Stream();
		SendPacket.SessionIdx = new int();
		SendPacket.pkHeader->SessionIdx = RecvPacket.pkHeader->SessionIdx;
		*SendPacket.SessionIdx = RecvPacket.pkHeader->SessionIdx;
		Singleton<DBManager>::GetInst()->ProcessQuery(QUERY_MAKE_CHATTING_ROOM().c_str());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		auto row = mysql_fetch_row(res);
		int roomkey = atoi(row[0]);

		while (mysql_next_result(Singleton<DBManager>::GetInst()->GetConn()) > 0)
		{
		}
		/*mysql_free_result(res);*/
		int numofCreatedRoom = -1;
		*RecvPacket.stream >> &numofCreatedRoom;
		for (int i = 0; i < numofCreatedRoom; i++)
		{
			// 이름들받아서 쿼리에 넣어줄것
			std::string userid;
			*RecvPacket.stream >> &userid;
			Singleton<DBManager>::GetInst()->ProcessQuery(QUERY_ROOMID_TO_USERID_INCHATTINGROOM(roomkey, userid).c_str());
		}
		std::string myID;
		*RecvPacket.stream >> &myID;
		Singleton<DBManager>::GetInst()->ProcessQuery(QUERY_ROOMID_TO_USERID_INCHATTINGROOM(roomkey, myID).c_str());

		// 방번호 알려주자.

		short pkid = static_cast<short>(PACKET_ID::PCK_MAKE_CHATTING_ROOM_RES);

		*SendPacket.stream << pkid;
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT UserID FROM userinchatroom WHERE roomID = %d;", roomkey);
		res = Singleton<DBManager>::GetInst()->GetsqlRes();
		*SendPacket.stream << roomkey;
		int rowcount = mysql_num_rows(res);
		*SendPacket.stream << rowcount;

		while (row = mysql_fetch_row(res))
		{
			std::string id;
			id = row[0];
			*SendPacket.stream << id;
		}

		m_sendpckQueue.push(SendPacket);

		return ERR_PCK_CODE::ERR_NONE;
	}

	ERR_PCK_CODE PckProcessor::Process_PCK_FIND_NICKNAME_REQ(const Packet& RecvPacket)
	{
		std::string UserID;
		*RecvPacket.stream >> &UserID;
		Packet SendPacket;
		SendPacket.stream = new Stream();
		SendPacket.SessionIdx = new int();
		*SendPacket.SessionIdx = RecvPacket.pkHeader->SessionIdx;
		SendPacket.pkHeader->SessionIdx = RecvPacket.pkHeader->SessionIdx;
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
		SendPacket.SessionIdx = new int();
		*SendPacket.SessionIdx = RecvPacket.pkHeader->SessionIdx;
		SendPacket.pkHeader->SessionIdx = RecvPacket.pkHeader->SessionIdx;

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

		SendPacket.stream = new Stream();
		SendPacket.SessionIdx = new int();
		*SendPacket.SessionIdx = RecvPacket.pkHeader->SessionIdx;
		SendPacket.pkHeader = new PacketHeader();
		SendPacket.pkHeader->SessionIdx = RecvPacket.pkHeader->SessionIdx;
		short pkid = static_cast<short>(PACKET_ID::PCK_ADD_FRIEND_RES);
		*RecvPacket.stream >> &UserID;
		*RecvPacket.stream >> &FrID;

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