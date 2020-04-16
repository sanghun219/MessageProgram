#pragma once
#include "ServLibrary.h"
class SocketUtil
{
public:
	static void ReportError(const char* DescFuncName);
	static void SetSocketOption(SOCKET& socket);
	static void SetSocketBufferSize(SOCKET & socket, const int sndBufSize, const int rcvBufSize);
	static void SetSocketNonblock(SOCKET& socket, bool isNonBlock);
};

#define REPORT_ERROR(DescFuncName) SocketUtil::ReportError(DescFuncName)