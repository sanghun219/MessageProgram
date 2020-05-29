#include "PckProcessor.h"
#include "DBManager.h"
#include "Singleton.h"
#include "User.h"
#include "ChattingRoom.h"
namespace PacketProc
{
	ERR_PCK_CODE PckProcessor::Process_LOGIN_REQ(const Packet& RecvPacket)
	{
		// UserData  : UserID , Password
		std::string UserID;
		std::string UserPass;

		*RecvPacket.stream >> &UserID;
		*RecvPacket.stream >> &UserPass;

		Packet SendPacket;
		SendPacket.stream = new Stream();
		SendPacket.pkHeader.SessionIdx = RecvPacket.pkHeader.SessionIdx;

		// id찾는 쿼리 없을시 생성, 있을시 데이터 로딩 후 user에 다 집어놓고 해당 데이터와 함께 send
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT * FROM userinfo WHERE ID = '%s'", UserID.data());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		// TODO : 아이디 없으면 아이디 만들라는 RES , 있으면 특정 데이터를 들고 접속하라는 RES
		if (mysql_num_rows(res) == 0)
		{
			short pkID = static_cast<short>(PACKET_ID::PCK_MAKE_ID_RES);
			*SendPacket.stream << pkID;
			m_sendpckQueue.push(SendPacket);
			return ERR_PCK_CODE::ERR_NONE;
		}
		else
		{
			/*
				ID,
				USERNICKNAME,
				PASS,
			*/

			// 아이디는 있찌만 password가 없는 경우 로그인 정보 오류 전달
			Singleton<DBManager>::GetInst()->ProcessQuery("SELECT Pass FROM userinfo WHERE ID = '%s' AND Pass = '%s'", UserID.c_str(), UserPass.c_str());
			res = Singleton<DBManager>::GetInst()->GetsqlRes();

			// 아이디는 있지만 비밀번호가 맞지않음
			if (mysql_num_rows(res) == 0)
			{
				short pkID = static_cast<short>(PACKET_ID::PCK_LOGIN_WRONG_PASS_RES);
				*SendPacket.stream << pkID;
				m_sendpckQueue.push(SendPacket);
				return ERR_PCK_CODE::ERR_NONE;
			}

			Singleton<DBManager>::GetInst()->ProcessQuery("SELECT * FROM userinfo WHERE ID = '%s'", UserID.data());
			res = Singleton<DBManager>::GetInst()->GetsqlRes();
			MYSQL_ROW row;
			unsigned int fieldCount = mysql_num_fields(res);

			row = mysql_fetch_row(res);

			User* user = new User();
			std::string id = row[0];
			std::string nickname = row[1];
			std::string pass = row[2];
			std::vector<ChattingRoom*> chattingRoomList;

			user->SetUserID(id);
			user->SetUserNick(nickname);
			user->SetUserPass(pass);
#pragma region 친구목록 조회
			auto reterr = Singleton<DBManager>::GetInst()->
				ProcessQuery(QUERY_FIND_FRIENDS_INFO(id).c_str());

			if (reterr < 0)
			{
				return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
			}

			res = Singleton<DBManager>::GetInst()->GetsqlRes();

			while ((row = mysql_fetch_row(res)))
			{
				std::string friendsID = row[0];
				std::string friendsNickName = row[1];
				User* myFriend = new User(friendsID, friendsNickName);
				user->SetFriendList(myFriend);
			}
#pragma endregion

#pragma region 채팅방목록 조회
			// 채팅방 ID 찾기
			reterr = Singleton<DBManager>::GetInst()->
				ProcessQuery(QUERY_FIND_CHATTINGROOM_FROM_USERID(id).c_str());

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
				// 룸 네임이 없으면
				if (row[1] != nullptr)
				{
					std::string RoomName = row[1];
					room->SetRoomName(RoomName);
				}
				room->SetRoomID(RoomID);
				chattingRoomList.push_back(room);
			}

			for (auto iter : chattingRoomList)
			{
				reterr = Singleton<DBManager>::GetInst()->
					ProcessQuery(QUERY_FIND_JOINNEDUSERS_FROM_ROOMID(iter->GetRoomID()).c_str());

				if (reterr < 0)
				{
					return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
				}

				res = Singleton<DBManager>::GetInst()->GetsqlRes();
				fieldCount = mysql_num_fields(res);

				while (row = mysql_fetch_row(res))
				{
					iter->SetJoinnedUser(row[0]);
				}

				reterr = Singleton<DBManager>::GetInst()->
					ProcessQuery(QUERY_FIND_CHATTINGDATAS_IN_CHATTINGROOM(iter->GetRoomID()).c_str());

				if (reterr < 0)
				{
					return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
				}
				fieldCount = mysql_num_rows(res);
				res = Singleton<DBManager>::GetInst()->GetsqlRes();
				if (fieldCount == 0)
				{
				}
				else
				{
					while (row = mysql_fetch_row(res))
					{
						ChattingData chatdata;
						chatdata.m_RoomID = atoi(row[0]);
						chatdata.m_Sequence = atoi(row[1]);
						chatdata.m_Senddate = row[2];
						chatdata.m_UserID = row[3];
						chatdata.m_Nickname = row[4];
						chatdata.m_Contents = row[5];
						iter->SetChatData(chatdata);
					}
				}
			}

#pragma endregion
			user->SetChattingRoomList(chattingRoomList);
			short pkid = static_cast<short>(PACKET_ID::PCK_LOGIN_RES);
			*SendPacket.stream << pkid;

			user->Write(*SendPacket.stream);
			m_sendpckQueue.push(SendPacket);
		}
		m_SocketIdxTOuserID[RecvPacket.pkHeader.SessionIdx] = UserID;
		m_UserIDtoSocketIdx[UserID] = RecvPacket.pkHeader.SessionIdx;
		std::cout << "유저 ID : " << UserID << std::endl;
		std::cout << "크기 : " << m_SocketIdxTOuserID.size() << std::endl;

		return ERR_PCK_CODE::ERR_NONE;
	}
	ERR_PCK_CODE PckProcessor::Process_SIGN_UP_REQ(const Packet& RecvPacket)
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

		*RecvPacket.stream >> &ID;
		*RecvPacket.stream >> &Nickname;
		*RecvPacket.stream >> &Password;

		Packet SendPacket;
		SendPacket.stream = new Stream();
		SendPacket.pkHeader.SessionIdx = RecvPacket.pkHeader.SessionIdx;
		// 중복 ID가 발생한경우 다시 가입하라고 보냄
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT ID FROM userinfo WHERE ID = '%s';", ID.c_str());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();
		if (mysql_num_rows(res) != 0)
		{
			// 다시 만들라는 메시지
			short pkid = static_cast<short>(PACKET_ID::PCK_SIGN_UP_RES);
			int value = -1;
			*SendPacket.stream << pkid;
			*SendPacket.stream << value;
			m_sendpckQueue.push(SendPacket);
		}
		else
		{
			// 회원가입 됐다는 메시지
			short pkid = static_cast<short>(PACKET_ID::PCK_SIGN_UP_RES);
			int value = 1;
			*SendPacket.stream << pkid;
			*SendPacket.stream << value;
			m_sendpckQueue.push(SendPacket);

			// DB에 id,pass,nickname insert함
			Singleton<DBManager>::GetInst()->ProcessQuery("INSERT INTO userinfo VALUES ('%s' ,'%s' , '%s' );", ID.c_str(), Nickname.c_str(), Password.c_str());
		}

		return ERR_PCK_CODE::ERR_NONE;
	}
};