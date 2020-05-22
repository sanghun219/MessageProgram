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

		*packData.stream >> &UserID;
		*packData.stream >> &UserPass;

		// idã�� ���� ������ ����, ������ ������ �ε� �� user�� �� ������� �ش� �����Ϳ� �Բ� send
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT * FROM userinfo WHERE ID = '%s'", UserID.data());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		// TODO : ���̵� ������ ���̵� ������ RES , ������ Ư�� �����͸� ��� �����϶�� RES
		if (mysql_num_rows(res) == 0)
		{
			short pkID = static_cast<short>(PACKET_ID::PCK_MAKE_ID_RES);
			*packData.stream << pkID;
			m_sendpckQueue.push(packData);
			return ERR_PCK_CODE::ERR_NONE;
		}
		else
		{
			/*
				ID,
				USERNICKNAME,
				PASS,
			*/

			// ���̵�� ��� password�� ���� ��� �α��� ���� ���� ����
			Singleton<DBManager>::GetInst()->ProcessQuery("SELECT Pass FROM userinfo WHERE ID = '%s' AND Pass = '%s'", UserID.c_str(), UserPass.c_str());
			res = Singleton<DBManager>::GetInst()->GetsqlRes();

			// ���̵�� ������ ��й�ȣ�� ��������
			if (mysql_num_rows(res) == 0)
			{
				short pkID = static_cast<short>(PACKET_ID::PCK_LOGIN_WRONG_PASS_RES);
				*packData.stream << pkID;
				m_sendpckQueue.push(packData);
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
#pragma region ģ����� ��ȸ
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

#pragma region ä�ù��� ��ȸ
			// ä�ù� ID ã��
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
				std::string RoomName = row[1];
				room->SetRoomID(RoomID);
				room->SetRoomName(RoomName);

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

				for (size_t i = 0; i < fieldCount; i++)
				{
					iter->SetJoinnedUser(row[0]);
				}

				reterr = Singleton<DBManager>::GetInst()->
					ProcessQuery(QUERY_FIND_CHATTINGDATAS_IN_CHATTINGROOM(iter->GetRoomID()).c_str());

				if (reterr < 0)
				{
					return ERR_PCK_CODE::ERR_PCK_NOTEXISTQUERY;
				}
				fieldCount = mysql_num_fields(res);

				if (fieldCount == 0)
				{
				}
				else
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

#pragma endregion
			user->SetChattingRoomList(chattingRoomList);
			short pkid = static_cast<short>(PACKET_ID::PCK_LOGIN_RES);
			*packData.stream << pkid;

			user->Write(*packData.stream);
			m_sendpckQueue.push(packData);
		}

		return ERR_PCK_CODE::ERR_NONE;
	}
	ERR_PCK_CODE PckProcessor::Process_SIGN_UP_REQ(const Packet& packData)
	{
		// �������� ���Դٴ� �����Ͽ� �ش� ������ db�� �Է��� ���� ���ؾ���
		// ��Ŷ ������ �Ѱ��ش�.

		/*
			ID
			UserNickname
			password
		*/

		std::string ID;
		std::string Nickname;
		std::string Password;

		*packData.stream >> &ID;
		*packData.stream >> &Nickname;
		*packData.stream >> &Password;

		// �ߺ� ID�� �߻��Ѱ�� �ٽ� �����϶�� ����
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT ID FROM userinfo WHERE ID = '%s';", ID.c_str());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();
		if (mysql_num_rows(res) != 0)
		{
			// �ٽ� ������ �޽���
			short pkid = static_cast<short>(PACKET_ID::PCK_SIGN_UP_RES);
			int value = -1;
			*packData.stream << pkid;
			*packData.stream << value;
			m_sendpckQueue.push(packData);
		}
		else
		{
			// ȸ������ �ƴٴ� �޽���
			short pkid = static_cast<short>(PACKET_ID::PCK_SIGN_UP_RES);
			int value = 1;
			*packData.stream << pkid;
			*packData.stream << value;
			m_sendpckQueue.push(packData);

			// DB�� id,pass,nickname insert��
			Singleton<DBManager>::GetInst()->ProcessQuery("INSERT INTO userinfo VALUES ('%s' ,'%s' , '%s' );", ID.c_str(), Nickname.c_str(), Password.c_str());
		}

		return ERR_PCK_CODE::ERR_NONE;
	}
};