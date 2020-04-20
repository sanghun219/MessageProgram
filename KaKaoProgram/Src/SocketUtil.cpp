#include "SocketUtil.h"
#include "Logger.h"
#include "ServLibrary.h"

void SocketUtil::ReportError(const char * DescFuncName)
{
	char Message[1024];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(TCHAR*)&Message,
		0,
		nullptr
	);

	LOG("[ERROR!!] DESC : %s , Error : %d - %s", DescFuncName, GetLastError(), Message);
}

void SocketUtil::SetSocketOption(SOCKET & socket)
{
	linger lg;
	lg.l_onoff = 1;
	lg.l_linger = 0;

	auto reuse = 1;

	setsockopt(socket, SOL_SOCKET, SO_LINGER, (const char*)&lg, sizeof(lg));
	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
}

void SocketUtil::SetSocketBufferSize(SOCKET & socket, const int sndBufSize, const int rcvBufSize)
{
	setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (const char*)&sndBufSize, sizeof(sndBufSize));
	setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (const char*)&rcvBufSize, sizeof(rcvBufSize));
}

void SocketUtil::SetSocketNonblock(SOCKET& socket, bool isNonBlock)
{
	auto argp = 1;

	if (isNonBlock)
	{
		ioctlsocket(socket, FIONBIO, (u_long*)&argp);
	}
}