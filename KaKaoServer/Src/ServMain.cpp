#include "Server.h"
#include <mysql.h>
#pragma comment (lib,"libmySQL.lib")
int main(void)
{
	std::cout << mysql_get_client_info() << std::endl;

	Server::GetInst()->InitServer();
	return 0;
}