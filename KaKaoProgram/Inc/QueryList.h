#pragma once
#include "ServLibrary.h"
using namespace std;
const string QUERY_SET_VA_LIST(const char* query, ...);
const  string QUERY_FIND_USERID(string ID);
const  string QUERY_ADD_FRIEND(string friendID, string UserID);
const  string QUERY_FIND_FRIENDS_INFO(string ID);
const  string QUERY_FIND_CHATTINGROOM_FROM_USERID(string ID);
const  string QUERY_FIND_JOINNEDUSERS_FROM_ROOMID(INT64 ID);
const  string QUERY_FIND_CHATTINGDATAS_IN_CHATTINGROOM(INT64 ID);
const string QUERY_FIND_NICKNAME_FROM_ID(string ID);
const string QUERY_MAKE_CHATTING_ROOM();
const string QUERY_ROOMID_TO_USERID_INCHATTINGROOM(std::string roomID, std::string UserID);
