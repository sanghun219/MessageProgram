#pragma once
#include "ServLibrary.h"
#include "ChattingRoom.h"

// UI�� ǥ���� �͵��� �����ؾ���.
// ���������� ���� �������� ���¸� Ȯ���� �� �ֵ��� �ؾ���.
class User
{
public:
	User(std::string id, std::string nick, std::string pass, std::vector<User*> friendlist
		, std::vector<ChattingRoom*> chattingroomList);
	// ģ�� ��Ͽ� �ʿ��� ������
	User(std::string id, std::string nick);
	User& operator = (const User& user);
	User() {}
	void Clear();

	inline std::string GetUserID()const { return m_id; }
	inline std::string GetUserNick()const { return m_nickname; }
	inline std::string GetPassword()const { return m_password; }
	inline std::vector<User*> GetFriendList()const { return m_FriendList; }
	inline std::vector<ChattingRoom*> GetChattingRoomList()const { return m_ChattingRoomList; }
private:
	std::string m_id;
	std::string m_nickname;
	std::string m_password;

	std::vector<User*> m_FriendList;
	std::vector<ChattingRoom*> m_ChattingRoomList;
};
