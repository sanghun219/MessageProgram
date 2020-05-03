#include "Server.h"
#include "Singleton.h"
#include <thread>
int main(void)
{
	std::thread t1([&]() {Singleton<Server>::GetInst()->InitServer(); });

	std::cout << "아무 키나 누르면 종료됩니다." << std::endl;
	getchar();
	Singleton<Server>::GetInst()->isOver = false;
	t1.join();
	WSACleanup();
	return 0;
}