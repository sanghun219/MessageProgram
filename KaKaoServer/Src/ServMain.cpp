#include "Server.h"
#include "Singleton.h"
#include <thread>
int main(void)
{
	std::thread t1([&]() {Singleton<Server>::GetInst()->InitServer(); });

	std::cout << "�ƹ� Ű�� ������ ����˴ϴ�." << std::endl;
	getchar();
	Singleton<Server>::GetInst()->isOver = false;
	t1.join();
	WSACleanup();
	return 0;
}