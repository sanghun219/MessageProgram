#pragma once
#include "ServLibrary.h"

struct ChattingData
{
	INT64 m_RoomID;
	INT64 m_Sequence;
	// TODO : String -> ��¥ ��ȯ �Լ� ����������.
	std::string m_Senddate;
	std::string m_UserID;
	std::string m_Nickname;
	// TODO : ���߿� Ȥ�� ���� ������ ������ �ʹٸ� CLASS ���� �������� ������ ������ �;��ҵ�
	std::string m_Contents;
};

class ChattingRoom
{
public:
	ChattingRoom() {}
	void SetJoinnedUsers(std::vector<std::string> joinusers);
	void SetJoinnedUser(std::string joinuser);
	void SetChatDataList(std::list<ChattingData> datalist);
	void SetChatData(ChattingData data);
	void SetRoomName(const std::string name);
	void SetRoomID(const INT64 roomID);
	void Read(Stream&  instream);
	void Write(Stream& outStream);

	inline INT64 GetRoomID()const { return m_RoomID; }
	inline std::string GetRoomName()const { return m_RoomName; }
	inline std::vector<std::string> GetJoinnedUsers()const { return m_JoinnedUsers; }
	inline std::list<ChattingData> GetChattingDataList()const { return m_ChattingDataList; }
private:
	std::vector<std::string> m_JoinnedUsers;
	std::list<ChattingData> m_ChattingDataList;
	std::string m_RoomName;
	INT64 m_RoomID;
};
