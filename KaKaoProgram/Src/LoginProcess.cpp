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

		// idã�� ���� ������ ����, ������ ������ �ε� �� user�� �� ������� �ش� �����Ϳ� �Բ� send
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT * FROM userinfo WHERE ID = '%s'", UserID.data());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		// TODO : ���̵� ������ ���̵� ������ RES , ������ Ư�� �����͸� ��� �����϶�� RES
		if (mysql_num_fields(res) == 0)
		{
			short pkID = static_cast<short>(PACKET_ID::PCK_MAKE_ID_RES);
			std::string msg = "��ϵ��� ���� ID�Դϴ�. ȸ�������� �Ͻðڽ��ϱ�?";
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
		Stream stream = *packData.stream;
		stream >> &ID;
		stream >> &Nickname;
		stream >> &Password;

		// �ߺ� ID�� �߻��Ѱ�� �ٽ� �����϶�� ����
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT ID FROM userinfo WHERE ID = '%s';", ID);
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();
		if (mysql_num_fields(res))
		{
			// �ٽ� ������ �޽���
		}
		else
		{
			// ȸ������ �ƴٴ� �޽���
		}

		return ERR_PCK_CODE::ERR_NONE;
	}
	ERR_PCK_CODE PckProcessor::Process_SIGN_UP_REQ(const Packet& packData)
	{
		return ERR_PCK_CODE::ERR_NONE;
	}
};