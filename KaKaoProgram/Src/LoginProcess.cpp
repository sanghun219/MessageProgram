#include "PckProcessor.h"
#include "DBManager.h"
namespace PacketProc
{
	ERR_CODE Process_LOGIN_REQ(const Packet& packData)
	{
		char userID[128] = { 0, };
		packData.session->inStream->Read(userID, sizeof(userID));
		packData.session->UserID = userID;

		// idã�� ���� ������ ����, ������ ������ �ε� �� user�� �� ������� �ش� �����Ϳ� �Բ� send
		DBManager::GetInst()->ProcessQuery("SELECT * FROM UserData WHERE userID = '%s'", userID);
		MYSQL_RES* res = DBManager::GetInst()->GetsqlRes();

		// TODO : ���̵� ������ ���̵� ������ RES , ������ Ư�� �����͸� ��� �����϶�� RES
		if (mysql_fetch_row(res) == NULL)
		{
		}
		else
		{
		}

		return ERR_CODE::ERR_NONE;
	}
};