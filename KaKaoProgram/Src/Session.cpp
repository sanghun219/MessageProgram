#include "Session.h"
void Session::Clear()
{
	idx = 0;
	seq = 0;
	std::free(address);
	std::free(fd);
	UserID[0] = { 0, };
	ZeroMemory(&ReadStream, sizeof(ReadStream));
	ZeroMemory(&WriteStream, sizeof(WriteStream));
}

bool Session::IsConnect()
{
	return (fd->GetSocket() != 0) ? true : false;
}