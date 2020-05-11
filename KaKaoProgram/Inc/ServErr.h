#pragma once

enum class ERR_CODE :short
{
	//SOCKET ERROR : 1~

	ERR_NONE = 1 << 0,
	ERR_BIND = 1 << 1,
	ERR_LISTN = 1 << 2,
	ERR_INVALID_SOCK = 1 << 3,
	ERR_CONNECT = 1 << 4,
	ERR_SEND = 1 << 5,
	ERR_WOULDBLOCK = 1 << 6,
	ERR_RECV = 1 << 7,
	ERR_SEND_DISCONNECT = 1 << 8,
	ERR_SESSION_ISNT_CONNECTED = 1 << 9,
	ERR_ACCEPT = 1 << 10,
	ERR_OVER_ACCEPT_CLIENT = 1 << 11,
};

enum class ERR_PCK_CODE : short
{
	ERR_NONE = 1 << 0,
	ERR_PCK_NOTHELLO = 1 << 1,
	ERR_PCK_NOTFOUNDPCK_IN_HASHMAP = 1 << 2,
};

inline ERR_CODE operator |(ERR_CODE e1, ERR_CODE e2)
{
	return static_cast<ERR_CODE>(static_cast<int>(e1) | static_cast<int>(e2));
}

inline ERR_CODE operator &(ERR_CODE e1, ERR_CODE e2)
{
	return static_cast<ERR_CODE>(static_cast<int>(e1) & static_cast<int>(e2));
}
inline bool IsMakeError(ERR_CODE err)
{
	if ((err & ERR_CODE::ERR_NONE) == ERR_CODE::ERR_NONE
		|| ERR_CODE::ERR_WOULDBLOCK == (err & ERR_CODE::ERR_WOULDBLOCK))
	{
		return false;
	}

	return true;
}