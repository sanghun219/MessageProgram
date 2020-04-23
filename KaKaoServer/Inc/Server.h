#pragma once
#include "Engine.h"
#include "NetworkLogic.h"
class Server : public Engine
{
private:
	void LoadConfig();
	void Run();

	static Server* m_pInst;
public:
	static Server* GetInst()
	{
		if (m_pInst == nullptr)
			m_pInst = new Server();
		return m_pInst;
	}
	void InitServer();
private:
	std::unique_ptr<Config> m_pServerConfig;
	std::unique_ptr<NetworkLogic> m_pNetworkLogic;

public:
	~Server();
};
#define MAX_PATH 260
