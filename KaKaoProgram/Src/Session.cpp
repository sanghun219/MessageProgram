#include "Session.h"
void Session::Clear()
{
	idx = -1;
	seq = 0;
	fd.release();
	stream.release();
}

bool Session::IsConnect()
{
	if (fd != nullptr)
		return (fd->GetSocket() != 0) ? true : false;
	else
		return false;
}