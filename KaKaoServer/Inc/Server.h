#pragma once
#include "ServLibrary.h"
#include "NetworkLogic.h"
#include "Singleton.h"

class Server
{
	friend Singleton<Server>;
private:
	void LoadConfig();
	void Run();

public:
	void InitServer();
	inline void GameOver() { isOver = true; }
private:
	std::shared_ptr<Config> m_pServerConfig;
	std::unique_ptr<NetworkLogic> m_pNetworkLogic;
public:
	bool isOver;
private:
	Server() {}
public:
	~Server();
};
#define MAX_PATH 260
