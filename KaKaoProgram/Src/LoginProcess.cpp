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
		// idã�� ���� ������ ����, ������ ������ �ε� �� user�� �� ������� �ش� �����Ϳ� �Բ� send
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT * FROM UserData WHERE userID = '%s'", UserID.data());
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		// TODO : ���̵� ������ ���̵� ������ RES , ������ Ư�� �����͸� ��� �����϶�� RES
		if (mysql_fetch_row(res) == NULL)
		{
			short pkDir = static_cast<short>(PACKET_DIR::StoC);
			short pkID = static_cast<short>(PACKET_ID::PCK_MAKE_ID_RES);
			std::string msg = "��ϵ��� ���� ID�Դϴ�. ���ο� ������ ����ڽ��ϴ�!";
			packData.session->WriteStream = new Stream();
			*packData.session->WriteStream << pkDir;
			*packData.session->WriteStream << pkID;
			*packData.session->WriteStream << msg;

			m_sendpckQueue.push(packData);
		}
		else
		{
			/*���� �����ͺ��̽� �ڷ��� ��Ŷ�� ������ ���.  */
		}

		return ERR_CODE::ERR_NONE;
	}
};