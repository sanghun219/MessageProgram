#include "SocketUtil.h"
#include "Logger.h"
#include "ServLibrary.h"
#include <tchar.h>
#include <Windows.h>
#include <locale.h>
#include <stdlib.h>

void SocketUtil::ReportError(const char * DescFuncName)
{
	LPVOID message = nullptr;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&message,
		0,
		nullptr
	);
	wchar_t subMessage[100];
	_snwprintf_s((wchar_t*)subMessage, _countof(subMessage), 100, L"DESC : %hs , Error NO : %d ", DescFuncName, WSAGetLastError());

	MessageBox(NULL, (LPCTSTR)message, (LPCTSTR)subMessage, MB_ICONERROR);

	if (message != nullptr)
		LocalFree(&message);
}

int SocketUtil::GetLastError()
{
	return WSAGetLastError();
}

void SocketUtil::SetSocketOption(SOCKET & socket)
{
	// TCP에 사용되는 Linger
	linger lg;
	lg.l_onoff = 1;
	lg.l_linger = 10;

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