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

			std::string id = row[0];
			std::string nickname = row[1];
			std::string pass = row[2];
			std::vector<User*> friendsIDList;
			std::vector<ChattingRoom*> chattingRoomList;
#pragma region ģ����� ��ȸ
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

#pragma region ä�ù��� ��ȸ
			// ä�ù� ID ã��
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
			// ������ UserIDs ã��
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
				// �濡 ���޵� �����͵� ���� ���
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
			// ��Ŷ ������ ������� READ �ؾ���
			*packData.stream << friendsIDList.size();
			for (int i = 0; i < friendsIDList.size(); i++)
			{
				*packData.stream << friendsIDList[i]->GetUserID();
				*packData.stream << friendsIDList[i]->GetUserNick();

				for (auto iter : friendsIDList[i]->GetFriendList())
				{
					// ģ�� ���� ����
					*packData.stream << iter->GetUserID();
					*packData.stream << iter->GetUserNick();
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