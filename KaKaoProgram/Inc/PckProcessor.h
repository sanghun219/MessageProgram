#pragma once
#include "ServLibrary.h"
#include "PacketInfo.h"
class TCPSocket;
namespace PacketProc
{
	class PckProcessor
	{
	public:
		void InitPckInfo();
		ERR_PCK_CODE Process(const Packet& inPacket);
		inline std::queue< Packet>* GetPacketQueue() { return &m_sendpckQueue; }
	private:
#pragma region 로그인
		ERR_PCK_CODE Process_LOGIN_REQ(const Packet& RecvPacket);
		ERR_PCK_CODE Process_SIGN_UP_REQ(const Packet& RecvPacket);
#pragma endregion

#pragma region 메인
		ERR_PCK_CODE Process_PCK_FIND_ID_REQ(const Packet& RecvPacket);
		ERR_PCK_CODE Process_PCK_ADD_ID_REQ(const Packet& RecvPacket);
		ERR_PCK_CODE Process_PCK_FIND_NICKNAME_REQ(const Packet& RecvPacket);
		ERR_PCK_CODE Process_PCK_MAKE_CHATTING_ROOM_REQ(const Packet& RecvPacket);
		ERR_PCK_CODE Process_PCK_FIND_USERS_IN_ROOM_REQ(const Packet& RecvPacket);
		ERR_PCK_CODE Process_PCK_SEND_DATA_REQ(const Packet& RecvPacket);
#pragma endregion
		std::recursive_mutex m_rm;
	private:
		typedef ERR_PCK_CODE(PckProcessor::*PckProcessFunction)(const Packet&);
		std::unordered_map < short, PckProcessFunction> m_PckIDtoFunc;
		std::unordered_map <std::string, size_t> m_UserIDtoSocketIdx;
		std::unordered_map <size_t, std::string > m_SocketIdxTOuserID;
		std::queue<Packet> m_sendpckQueue;
	public:
		~PckProcessor() {};
	};
}
