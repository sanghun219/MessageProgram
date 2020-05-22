#pragma once
#include "ServLibrary.h"
#include "ChattingRoom.h"

// UI에 표시할 것들을 정리해야함.
// 서버측에서 현재 유저들의 상태를 확인할 수 있도록 해야함.
class Stream;
class User
{
public:
	User(std::string id, std::string nick, std::string pass, std::vector<User*> friendlist
		, std::vector<ChattingRoom*> chattingroomList);
	// 친구 목록에 필요한 생성자
	User(std::string id, std::string nick);
	User& operator = (const User& user);
	User() {}
	~User();
	void Clear();

	void Read(Stream& inStream, bool isFriend = false);
	void Write(Stream& outStream, bool isFriend = false);

	inline std::string GetUserID()const { return m_id; }
	inline std::string GetUserNick()const { return m_nickname; }
	inline std::string GetPassword()const { return m_password; }
	inline std::vector<User*> GetFriendList()const { return m_FriendList; }
	inline std::vector<ChattingRoom*> GetChattingRoomList()const { return m_ChattingRoomList; }

	inline void SetUserID(std::string id) { m_id = id; }
	inline void SetUserNick(std::string nick) { m_nickname = nick; }
	inline void SetUserPass(std::string pass) { m_password = pass; }
	inline void SetFriendList(std::vector<User*> friendlist) { m_FriendList.assign(friendlist.begin(), friendlist.end()); }
	inline void SetFriendList(User* friends) { m_FriendList.push_back(friends); }
	inline void SetChattingRoomList(std::vector<ChattingRoom*> chatroomlist) { m_ChattingRoomList.assign(chatroomlist.begin(), chatroomlist.end()); }
	inline void SetChattingRoomList(ChattingRoom* chatroom) { m_ChattingRoomList.push_back(chatroom); }
private:
	std::string m_id;
	std::string m_nickname;
	std::string m_password;

	std::vector<User*> m_FriendList;
	std::vector<ChattingRoom*> m_ChattingRoomList;
};
