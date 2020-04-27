#include "Server.h"
#include "ServLibrary.h"
#include "DBManager.h"

Server* Server::m_pInst = nullptr;

void Server::LoadConfig()
{
	m_pServerConfig = std::make_unique<Config>();
	TCHAR sPath[MAX_PATH] = { 0, };
	::GetCurrentDirectory(MAX_PATH, sPath);
	std::cout << (char*)sPath << std::endl;
	char sIniPath[MAX_PATH] = { 0, };
	sprintf_s(sIniPath, "%s\\InitServer.ini", sPath);

	std::cout << (char*)sIniPath << std::endl;
	// 밑에 초기화과정을 거친다.

	m_pServerConfig->port = (unsigned short)GetPrivateProfileInt("Config", "port", 0, sIniPath);
	m_pServerConfig->backlog = GetPrivateProfileInt("Config", "backlog", 0, sIniPath);
	m_pServerConfig->maxConnectSession = GetPrivateProfileInt("Config", "maxConnectSession", 0, sIniPath);
}

void Server::Run()
{
	while (m_pNetworkLogic->DoRunLoop())
	{
	}

	LOG("서버가 종료되었습니다.");
}

void Server::InitServer()
{
	// TODO : 나중에 데이터베이스 초기화 및 서버 구동에필요한 초기화는 여기서 이루어진다.
	LoadConfig();
	m_pNetworkLogic = std::make_unique<NetworkLogic>();
	auto retErr = m_pNetworkLogic->InitNetworkLogic(m_pServerConfig.get());

	if (!retErr)
	{
		LOG("InitNetworkLogic Err");
		return;
	}

	DBManager::GetInst()->InitDBManager();

	// 여기 위에까지 초기화 할 것들을 다 해둔다.

	Run();
}

Server::~Server()
{
}