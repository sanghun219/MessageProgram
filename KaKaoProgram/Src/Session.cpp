#include "Session.h"
void Session::Clear()
{
	idx = 0;
	seq = 0;
	std::free(fd);
	ZeroMemory(&stream, sizeof(stream));
	//ZeroMemory(&WriteStream, sizeof(WriteStream));
}

bool Session::IsConnect()
{
	return (fd->GetSocket() != 0) ? true : false;
}