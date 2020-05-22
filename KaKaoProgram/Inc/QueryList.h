#pragma once
#include <string.h>

inline const std::string QUERY_FIND_FRIENDS_INFO(std::string ID)
{
	std::string retstr = "SELECT ID,NickName FROM userinfo WHERE ID IN ("
		"SELECT fr.FriendID FROM friendsinfo AS fr LEFT JOIN userinfo AS us ON fr.ID = us.ID WHERE us.ID = '";
	retstr += ID;
	retstr += "' ) ORDER BY NickName";
	return retstr;
}

inline const std::string QUERY_FIND_CHATTINGROOM_FROM_USERID(std::string ID)
{
	std::string retstr = "SELECT * FROM chattingroom WHERE RoomID IN ("
		"SELECT uicr.roomID FROM userinchatroom AS uicr "
		"WHERE uicr.UserID = '";
	retstr += ID;
	retstr += "')";
	return retstr;
}

inline const std::string QUERY_FIND_JOINNEDUSERS_FROM_ROOMID(INT64 ID)
{
	std::string retstr = " SELECT NickName FROM userinfo WHERE ID IN "
		"( SELECT UserID FROM userinchatroom WHERE roomID IN ( "
		"SELECT chr.RoomID FROM chattingroom AS chr "
		"WHERE chr.roomID = ";
	retstr += static_cast<int>(ID);
	retstr += "))";
	return retstr;
}

inline const std::string QUERY_FIND_CHATTINGDATAS_IN_CHATTINGROOM(INT64 ID)
{
	std::string retstr = "SELECT RoomID, Sequence, SendDate, ID, Nickname, Contents FROM chattingdata"
		"WHERE RoomID = ";
	retstr += static_cast<int>(ID);

	return retstr;
}
