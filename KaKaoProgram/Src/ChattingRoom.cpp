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

void ChattingRoom::Read(Stream& instream)
{
	instream >> &this->m_RoomID;
	instream >> &this->m_RoomName;

	INT32 size = 0;
	instream >> &size;
	for (INT32 i = 0; i < size; i++)
	{
		std::string joinnedUserName;
		instream >> &joinnedUserName;
		m_JoinnedUsers.push_back(joinnedUserName);
	}

	instream >> &size;

	for (INT32 i = 0; i < size; i++)
	{
		ChattingData chatdata;
		instream >> &chatdata.m_RoomID;
		instream >> &chatdata.m_Sequence;
		instream >> &chatdata.m_Senddate;
		instream >> &chatdata.m_UserID;
		instream >> &chatdata.m_Nickname;
		instream >> &chatdata.m_Contents;
		m_ChattingDataList.push_back(chatdata);
	}
}

void ChattingRoom::Write(Stream& outStream)
{
	outStream << this->m_RoomID;
	outStream << this->m_RoomName;

	INT32 size = this->m_JoinnedUsers.size();
	outStream << size;

	for (auto iter : this->m_JoinnedUsers)
	{
		outStream << iter;
	}

	size = this->m_ChattingDataList.size();
	outStream << size;

	for (auto iter : this->m_ChattingDataList)
	{
		outStream << iter.m_RoomID;
		outStream << iter.m_Sequence;
		outStream << iter.m_Senddate;
		outStream << iter.m_UserID;
		outStream << iter.m_Nickname;
		outStream << iter.m_Contents;
	}
}