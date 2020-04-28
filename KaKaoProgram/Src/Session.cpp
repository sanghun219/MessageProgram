#include "Session.h"
void Session::Clear()
{
	idx = 0;
	seq = 0;
	std::free(address);
	std::free(fd);
	UserID.clear();
	ZeroMemory(&inStream, sizeof(inStream));
	ZeroMemory(&outStream, sizeof(outStream));
}

bool Session::IsConnect()
{
	return (fd->GetSocket() != 0) ? true : false;
}