#include "Server.h"
#include "Singleton.h"
int main(void)
{
	Singleton<Server>::GetInst()->InitServer();
	WSACleanup();
	return 0;
}