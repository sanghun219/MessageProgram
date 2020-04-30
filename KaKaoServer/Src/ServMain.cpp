#include "Server.h"
#include "Singleton.h"
int main(void)
{
	Singleton<Server>::GetInst()->InitServer();
	return 0;
}