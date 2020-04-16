#pragma once
#include "Engine.h"

class NetworkLogic;
class Server : public Engine
{
private:
	void LoadConfig();
	void Run();
public:
	static void StaticInit();
	void InitServer();
private:
	std::unique_ptr<Config> m_pServerConfig;
	std::unique_ptr<NetworkLogic> m_pNetworkLogic;

public:
	~Server();
};
#define MAX_PATH 260
