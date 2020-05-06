#include "Server.h"
#include "ServLibrary.h"
#include "DBManager.h"
#include "Singleton.h"

void Server::LoadConfig()
{
	setlocale(LC_ALL, "");
	m_pServerConfig = new Config();
	wchar_t sPath[MAX_PATH] = { 0, };
	::GetCurrentDirectory(MAX_PATH, sPath);

	wchar_t sIniPath[MAX_PATH] = { 0, };
	_snwprintf_s(sIniPath, _countof(sIniPath), L"%ws\\InitServer.ini", sPath);
	wprintf_s(L"%ws\n", sIniPath);

	// �ؿ� �ʱ�ȭ������ ��ģ��.

	m_pServerConfig->port = (unsigned short)GetPrivateProfileInt(L"Config", L"port", 0, sIniPath);
	m_pServerConfig->backlog = GetPrivateProfileInt(L"Config", L"backlog", 0, sIniPath);
	m_pServerConfig->maxConnectSession = GetPrivateProfileInt(L"Config", L"maxConnectSession", 0, sIniPath);
}

void Server::Run()
{
	while (isOver)
	{
		m_pNetworkLogic->DoRunLoop();
	}

	LOG("������ ����Ǿ����ϴ�.");
}

void Server::InitServer()
{
	LoadConfig();
	Singleton<DBManager>::GetInst()->InitDBManager();
	m_pNetworkLogic = new NetworkLogic();
	auto retErr = m_pNetworkLogic->InitNetworkLogic(m_pServerConfig);

	if (!retErr)
	{
		LOG("InitNetworkLogic Err");
		return;
	}

	// ���� �������� �ʱ�ȭ �� �͵��� �� �صд�.
	isOver = true;
	Run();
}