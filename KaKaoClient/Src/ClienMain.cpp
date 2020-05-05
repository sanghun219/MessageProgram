#include "PacketTest.h"
#include "ServLibrary.h"
int main()
{
	Singleton<PacketTest>::GetInst()->Init();
	Singleton<PacketTest>::GetInst()->Run();
	WSACleanup();
}