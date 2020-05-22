#include "Session.h"
void Session::Clear()
{
	idx = 0;
	seq = 0;
	fd.release();
	stream.release();
	/*ZeroMemory(&fd, sizeof(fd));
	ZeroMemory(&stream, sizeof(stream));*/
}

bool Session::IsConnect()
{
	return (fd->GetSocket() != 0) ? true : false;
}