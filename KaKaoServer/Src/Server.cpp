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
	// �ؿ� �ʱ�ȭ������ ��ģ��.

	m_pServerConfig->port = (unsigned short)GetPrivateProfileInt("Config", "port", 0, sIniPath);
	m_pServerConfig->backlog = GetPrivateProfileInt("Config", "backlog", 0, sIniPath);
	m_pServerConfig->maxConnectSession = GetPrivateProfileInt("Config", "maxConnectSession", 0, sIniPath);
}

void Server::Run()
{
	while (m_pNetworkLogic->DoRunLoop())
	{
	}

	LOG("������ ����Ǿ����ϴ�.");
}

void Server::InitServer()
{
	// TODO : ���߿� �����ͺ��̽� �ʱ�ȭ �� ���� �������ʿ��� �ʱ�ȭ�� ���⼭ �̷������.
	LoadConfig();
	m_pNetworkLogic = std::make_unique<NetworkLogic>();
	auto retErr = m_pNetworkLogic->InitNetworkLogic(m_pServerConfig.get());

	if (!retErr)
	{
		LOG("InitNetworkLogic Err");
		return;
	}

	DBManager::GetInst()->InitDBManager();

	// ���� �������� �ʱ�ȭ �� �͵��� �� �صд�.

	Run();
}

Server::~Server()
{
}