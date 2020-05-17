#include "PckProcessor.h"
#include "DBManager.h"
#include "Singleton.h"
#include "User.h"
#include "ChattingRoom.h"
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
			*/
			MYSQL_ROW row;
			unsigned int fieldCount = mysql_num_fields(res);

			row = mysql_fetch_row(res);

			std::string id = row[0];
			std::string nickname = row[1];
			std::string pass = row[2];
			std::vector<User*> friendsIDList;
			std::vector<ChattingRoom*> chattingRoomList;
#pragma region 친구목록 조회
			auto reterr = Singleton<DBManager>::GetInst()->
				ProcessQuery(QUERY_FIND_FRIENDS_INFO(id.c_str()));

			if (reterr < 0)
			{
				return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
			}

			res = Singleton<DBManager>::GetInst()->GetsqlRes();

			fieldCount = mysql_num_fields(res);

			while ((row = mysql_fetch_row(res)))
			{
				std::string friendsid = row[0];
				std::string friendsNickName = row[1];
				User* myFriend = new User(friendsid, friendsNickName);
				friendsIDList.push_back(myFriend);
			}
#pragma endregion

#pragma region 채팅방목록 조회
			// 채팅방 ID 찾기
			reterr = Singleton<DBManager>::GetInst()->
				ProcessQuery(QUERY_FIND_CHATTINGROOM_FROM_USERID(id.c_str()));

			if (reterr < 0)
			{
				return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
			}

			res = Singleton<DBManager>::GetInst()->GetsqlRes();
			fieldCount = mysql_num_fields(res);
			while (row = mysql_fetch_row(res))
			{
				ChattingRoom* room = new ChattingRoom();
				INT64 RoomID = atoi(row[0]);
				std::string RoomName = row[1];
				room->SetRoomID(RoomID);
				room->SetRoomName(RoomName);
				chattingRoomList.push_back(room);
			}
			// 참여한 UserIDs 찾기
			for (int i = 0; i < chattingRoomList.size(); i++)
			{
				reterr = Singleton<DBManager>::GetInst()->
					ProcessQuery(QUERY_FIND_JOINNEDUSERS_FROM_ROOMID(chattingRoomList[i]->GetRoomID()));

				if (reterr < 0)
				{
					return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
				}

				res = Singleton<DBManager>::GetInst()->GetsqlRes();
				fieldCount = mysql_num_fields(res);

				while (row = mysql_fetch_row(res))
				{
					std::string userid = row[0];
					chattingRoomList[i]->SetJoinnedUser(userid);
				}
				// 방에 전달된 데이터들 정보 담기
				reterr = Singleton<DBManager>::GetInst()->
					ProcessQuery(QUERY_FIND_CHATTINGDATAS_IN_CHATTINGROOM(chattingRoomList[i]->GetRoomID()));

				if (reterr < 0)
				{
					return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
				}

				res = Singleton<DBManager>::GetInst()->GetsqlRes();
				fieldCount = mysql_num_fields(res);

				while (row = mysql_fetch_row(res))
				{
					ChattingData data;
					data.m_RoomID = atoi(row[0]);
					data.m_Sequence = atoi(row[1]);
					data.m_Senddate = row[2];
					data.m_UserID = row[3];
					data.m_Nickname = row[4];
					data.m_Contents = row[5];

					chattingRoomList[i]->SetChatData(data);
				}
			}

#pragma endregion

			short pkid = static_cast<short>(PACKET_ID::PCK_LOGIN_RES);

			*packData.stream << pkid;
			*packData.stream << id;
			*packData.stream << nickname;
			*packData.stream << pass;
			// 패킷 데이터 순서대로 READ 해야함
			for (int i = 0; i < friendsIDList.size(); i++)
			{
				*packData.stream << friendsIDList[i]->GetUserID();
				*packData.stream << friendsIDList[i]->GetUserNick();
				*packData.stream << friendsIDList[i]->GetPassword();

				for (auto iter : friendsIDList[i]->GetFriendList())
				{
					// 친구 정보 전송
					*packData.stream << iter->GetUserID();
					*packData.stream << iter->GetUserNick();
					*packData.stream << iter->GetPassword();
				}
			}

			for (int i = 0; i < chattingRoomList.size(); i++)
			{
				*packData.stream << chattingRoomList[i]->GetRoomID();
				*packData.stream << chattingRoomList[i]->GetRoomName();
				for (auto iter : chattingRoomList[i]->GetJoinnedUsers())
				{
					*packData.stream << iter;
				}

				for (auto iter : chattingRoomList[i]->GetChattingDataList())
				{
					*packData.stream << iter.m_RoomID;
					*packData.stream << iter.m_Sequence;
					*packData.stream << iter.m_Senddate;
					*packData.stream << iter.m_UserID;
					*packData.stream << iter.m_Nickname;
					*packData.stream << iter.m_Contents;
				}
			}

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