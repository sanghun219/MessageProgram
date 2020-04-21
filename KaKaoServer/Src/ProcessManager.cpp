#include "ProcessManager.h"
#include "TCPSocket.h"

ProcessManager* ProcessManager::m_pInst = nullptr;
ERR_CODE ProcessManager::SendHelloPacket(RecvPacket * pRcvPck)
{
	short pk_id = 0;
	pRcvPck->inputStream->Read(pk_id);
	auto reterr = ERR_CODE::ERR_NONE;
	switch (pk_id)
	{
	case (short)PACKET_ID::PCK_LOGIN_REQ:
		reterr = InvolveUserinServer(pRcvPck);
		break;
	default:
		LOG("잘못된 패킷 전달");
		reterr = ERR_CODE::ERR_PCK_NOTHELLO;
		break;
	}

	return reterr;
}
ERR_CODE ProcessManager::InvolveUserinServer(RecvPacket *pRcvPck)
{
	int dataSize = 0;
	std::string userID;
	pRcvPck->inputStream->Read(dataSize);
	//TODO : 나중에 확인한 번해봐라 USER_ID_MAX 인지 -1인지
	pRcvPck->inputStream->Read(userID);
	memcpy((void*)&pRcvPck->session->UserID, (void*)&userID, USER_ID_MAX - 1);

	return ERR_CODE();
}
void ProcessManager::UnpackPacket(RecvPacket * pRcvPck)
{
	//TODO : PacketFactory의 Wrapper함수들 정의, 패킷을 뜯는건 여기서, 처리는 PacketFactory에서
	if (m_IDtoSession.find(pRcvPck->session->idx) == m_IDtoSession.end())
	{
		//여기서 받아올 데이터도 클라이언트에서 처음으로 보내는 데이터
		m_IDtoSession[pRcvPck->session->idx] = (pRcvPck->session);
		SendHelloPacket(pRcvPck);
	}
	else
	{
		// 이미 연결된 세션에대한 로직처리 -
	}
}

void ProcessManager::DisconnectSession(const SESSION_ID id)
{
	if (m_IDtoSession.find(id) != m_IDtoSession.end())
	{
		m_IDtoSession.erase(id);
	}
	else
	{
		LOG("%d 번째 인덱스에 해당하는 session이 존재하지 않습니다.", id);
	}
}

ProcessManager::ProcessManager()
{
}

ProcessManager::~ProcessManager()
{
}