#include "Server.h"
#include "NetworkLogic.h"
#include "ServLibrary.h"
void Server::LoadConfig()
{
	m_pServerConfig = std::make_unique<Config>();
	wchar_t sPath[MAX_PATH] = { 0, };
	::GetCurrentDirectory(MAX_PATH, sPath);

	wchar_t sIniPath[MAX_PATH] = { 0, };

	_snwprintf_s(sIniPath, _countof(sIniPath), _TRUNCATE, L"%s\\InitServer.ini", sPath);

	// �ؿ� �ʱ�ȭ������ ��ģ��.
}

void Server::Run()
{
	while (m_pNetworkLogic->DoRunLoop())
	{
	}

	LOG("������ ����Ǿ����ϴ�.");
}

void Server::StaticInit()
{
	m_pInst = new Server();
}

void Server::InitServer()
{
	// TODO : ���߿� �����ͺ��̽� �ʱ�ȭ �� ���� �������ʿ��� �ʱ�ȭ�� ���⼭ �̷������.
	LoadConfig();

	m_pNetworkLogic = std::make_unique<NetworkLogic>();
	m_pNetworkLogic->InitNetworkLogic(m_pServerConfig.get());

	// ���� �������� �ʱ�ȭ �� �͵��� �� �صд�.
	Run();
}

Server::~Server()
{
}