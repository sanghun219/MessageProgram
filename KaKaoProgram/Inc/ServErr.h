#pragma once

enum class ERR_CODE :short
{
	//SOCKET ERROR : 1~

	ERR_NONE = 1,
	ERR_BIND = 2,
	ERR_LISTN = 3,
	ERR_INVALID_SOCK = 4,
	ERR_CONNECT = 5,
	ERR_SEND = 6,
	ERR_WOULDBLOCK = 7,
	ERR_RECV = 8,
	ERR_SEND_DISCONNECT = 9,

	// LOGIC ERROR : 21~
};