#pragma once

#define QUERY_FIND_FRIENDS_INFO(ID) \
"SELECT ID,NickName FROM userinfo WHERE ID IN ( \
SELECT fr.FriendID FROM friendsinfo as fr \
left join userinfo as us \
on '" #ID "' = fr.ID \
);"

#define QUERY_FIND_CHATTINGROOM_FROM_USERID(ID) \
"SELECT * FROM chattingroom WHERE RoomID IN ( \
SELECT uicr.roomID FROM userinchatroom AS uicr \
WHERE uicr.UserID = '" #ID "'\
)"

#define QUERY_FIND_JOINNEDUSERS_FROM_ROOMID(ID)\
" SELECT NickName FROM userinfo WHERE ID IN ( SELECT UserID FROM userinchatroom WHERE roomID IN( \
SELECT chr.RoomID FROM chattingroom AS chr \
WHERE chr.roomID ='" #ID "'))"

#define QUERY_FIND_CHATTINGDATAS_IN_CHATTINGROOM(ID) \
"SELECT RoomID, Sequence, SendDate, ID, Nickname, Contents FROM chattingdata \
WHERE RoomID ='" #ID"'"