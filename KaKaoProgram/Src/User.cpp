#include "User.h"

User::User(std::string id, std::string nick, std::string pass, std::vector<User*> friendlist
	, std::vector<ChattingRoom*> chattingroomList)
{
	this->m_id = id;
	this->m_nickname = nick;
	this->m_password = pass;
	this->m_FriendList.assign(friendlist.begin(), friendlist.end());
	this->m_ChattingRoomList.assign(chattingroomList.begin(), chattingroomList.end());
}

User::User(std::string id, std::string nick)
{
	this->m_id = id;
	this->m_nickname = nick;
}

User& User::operator=(const User & user)
{
	if (this == &user)
		return *this;
	this->m_id = user.m_id;
	this->m_nickname = user.m_nickname;

	// 친구도 user 이는 개인적인 정보는 private이기 때문에 일부만 전달.
	if (!user.m_password.empty())
	{
		this->m_password = user.m_password;
		this->m_FriendList.assign(user.m_FriendList.begin(), user.m_FriendList.end());
		this->m_ChattingRoomList.assign(user.m_ChattingRoomList.begin(), user.m_ChattingRoomList.end());
	}

	return *this;
}

void User::Clear()
{
	if (!m_FriendList.empty())
	{
		for (auto& iter : m_FriendList)
		{
			delete iter;
			iter = nullptr;
		}
	}
	if (!m_ChattingRoomList.empty())
	{
		for (auto iter : m_ChattingRoomList)
		{
			delete iter;
			iter = nullptr;
		}
	}
}