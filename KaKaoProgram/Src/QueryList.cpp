#include "QueryList.h"
#include "DBManager.h"
const string QUERY_SET_VA_LIST(const char* query, ...)
{
	char copyQuery[5096];
	ZeroMemory(&copyQuery, sizeof(copyQuery));
	va_list ap;
	va_start(ap, query);
	vsprintf_s(copyQuery + strlen(copyQuery), sizeof(copyQuery), query, ap);
	va_end(ap);
	return copyQuery;
}

const std::string QUERY_FIND_USERID(std::string ID)
{
	std::string retstr = QUERY_SET_VA_LIST("SELECT ID FROM userinfo WHERE ID = '%s'", ID.c_str());

	return retstr;
}

const std::string QUERY_ADD_FRIEND(std::string friendID, std::string UserID)
{
	std::string retstr;

	retstr = QUERY_SET_VA_LIST("INSERT INTO friendsinfo(ID,FriendID) SELECT '%s', '%s' \
		WHERE 0 = (SELECT COUNT(*) FROM friendsinfo) OR '%s' NOT IN\
		(SELECT FriendID FROM friendsinfo WHERE ID = '%s'); "\
		, UserID.c_str(), friendID.c_str(), friendID.c_str(), UserID.c_str());

	return retstr;
}

const std::string QUERY_FIND_FRIENDS_INFO(std::string ID)
{
	std::string retstr = "SELECT ID,NickName FROM userinfo WHERE ID IN ("
		"SELECT fr.FriendID FROM friendsinfo AS fr LEFT JOIN userinfo AS us ON fr.ID = us.ID WHERE us.ID = '";
	retstr += ID;
	retstr += "' ) ORDER BY NickName";
	return retstr;
}

const std::string QUERY_FIND_CHATTINGROOM_FROM_USERID(std::string ID)
{
	std::string retstr =
		"SELECT * FROM chattingroom WHERE RoomID IN ("
		"SELECT uicr.roomID FROM userinchatroom AS uicr "
		"WHERE uicr.UserID = '";
	retstr += ID;
	retstr += "')";
	return retstr;
}

const std::string QUERY_FIND_JOINNEDUSERS_FROM_ROOMID(INT64 ID)
{
	std::string retstr = QUERY_SET_VA_LIST(" SELECT NickName FROM userinfo WHERE ID IN \
		( SELECT UserID FROM userinchatroom WHERE roomID IN ( \
		SELECT chr.RoomID FROM chattingroom AS chr \
		WHERE chr.roomID = %d ))", ID);

	return retstr;
}

const std::string QUERY_FIND_CHATTINGDATAS_IN_CHATTINGROOM(INT64 ID)
{
	std::string retstr = QUERY_SET_VA_LIST("SELECT RoomID, Sequence, SendDate, ID, \
		Nickname, Contents FROM chattingdata WHERE RoomID = %d", ID);

	return retstr;
}

const string QUERY_FIND_NICKNAME_FROM_ID(string ID)
{
	std::string retval = QUERY_SET_VA_LIST("SELECT NickName FROM userinfo WHERE ID = '%s'", ID.c_str());
	return retval;
}

const string QUERY_MAKE_CHATTING_ROOM()
{
	std::string retval = "CALL CreateGetRoomID();";
	return retval;
}

const string QUERY_ROOMID_TO_USERID_INCHATTINGROOM(int roomID, std::string UserID)
{
	std::string retval = QUERY_SET_VA_LIST("INSERT INTO userinchatroom(roomID,UserID) SELECT %d ,'%s'", roomID, UserID.c_str());

	return retval;
}