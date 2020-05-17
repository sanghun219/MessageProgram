#include "ChattingRoom.h"

void ChattingRoom::SetJoinnedUsers(std::vector<std::string> joinusers)
{
	m_JoinnedUsers.assign(joinusers.begin(), joinusers.end());
}

void ChattingRoom::SetJoinnedUser(std::string joinuser)
{
	m_JoinnedUsers.push_back(joinuser);
}

void ChattingRoom::SetChatDataList(std::list<ChattingData> datalist)
{
	m_ChattingDataList.assign(datalist.begin(), datalist.end());
}

void ChattingRoom::SetChatData(ChattingData data)
{
	m_ChattingDataList.push_back(data);
}

void ChattingRoom::SetRoomName(const std::string name)
{
	m_RoomName.assign(name.begin(), name.end());
}

void ChattingRoom::SetRoomID(const INT64 roomID)
{
	m_RoomID = roomID;
}