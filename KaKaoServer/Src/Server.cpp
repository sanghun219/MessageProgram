#include "Server.h"
#include "ServLibrary.h"
#include "ProcessManager.h"

Server* Server::m_pInst = nullptr;

void Server::LoadConfig()
{
	m_pServerConfig = std::make_unique<Config>();
	wchar_t sPath[MAX_PATH] = { 0, };
	::GetCurrentDirectory(MAX_PATH, sPath);

	wchar_t sIniPath[MAX_PATH] = { 0, };

	_snwprintf_s(sIniPath, _countof(sIniPath), _TRUNCATE, L"%s\\InitServer.ini", sPath);

	// 밑에 초기화과정을 거친다.
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
	m_pNetworkLogic->InitNetworkLogic(m_pServerConfig.get());

	// 여기 위에까지 초기화 할 것들을 다 해둔다.
	Run();
}

Server::~Server()
{
}