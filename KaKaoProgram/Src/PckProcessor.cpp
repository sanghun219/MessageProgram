#include "PckProcessor.h"
#include "DBManager.h"
#include "Stream.h"

namespace PacketProc
{
	void PckProcessor::InitPckInfo()
	{
		m_PckIDtoFunc[(short)PACKET_ID::PCK_LOGIN_REQ] = &PckProcessor::Process_LOGIN_REQ;
	}

	ERR_CODE PckProcessor::Process(const Packet& inPacket)
	{
		short id = static_cast<short>(inPacket.pckData.pkHeader.id);

		if (m_PckIDtoFunc.find(id) == m_PckIDtoFunc.end())
		{
			// TODO : å������ ���°�� �ٽ� �������� ���� ��Ŷ ID�� �׿�����Ʈ�� ������ �н�
			LOG("PckProcessor::Process ERR_PCK_NOTFOUNDPCK_IN_HASHMAP!");
			return ERR_CODE::ERR_PCK_NOTFOUNDPCK_IN_HASHMAP;
		}
		else
		{
			(this->*m_PckIDtoFunc[id])(inPacket);
			return ERR_CODE::ERR_NONE;
		}
	}
}