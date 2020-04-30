#include "PckProcessor.h"
#include "DBManager.h"
#include "Singleton.h"
namespace PacketProc
{
	ERR_CODE PckProcessor::Process_LOGIN_REQ(const Packet& packData)
	{
		char userID[128] = { 0, };
		packData.session->inStream->Read(userID, sizeof(userID));
		packData.session->UserID = userID;

		// idã�� ���� ������ ����, ������ ������ �ε� �� user�� �� ������� �ش� �����Ϳ� �Բ� send
		Singleton<DBManager>::GetInst()->ProcessQuery("SELECT * FROM UserData WHERE userID = '%s'", userID);
		MYSQL_RES* res = Singleton<DBManager>::GetInst()->GetsqlRes();

		// TODO : ���̵� ������ ���̵� ������ RES , ������ Ư�� �����͸� ��� �����϶�� RES
		if (mysql_fetch_row(res) == NULL)
		{
			Packet pck;
			pck = GetPacketSettingParam(PACKET_ID::PCK_MAKE_ID_RES, PACKET_DIR::StoC,
				0, nullptr, packData.session);
			m_sendpckQueue.push(pck);
		}
		else
		{
			/*���� �����ͺ��̽� �ڷ��� ��Ŷ�� ������ ���.  */
		}

		return ERR_CODE::ERR_NONE;
	}
};