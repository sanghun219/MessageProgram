#pragma once
#include "TCPNetwork.h"
#include "IClientScene.h"
#include "ClientSceenLogin.h"
#include "ClientMainScene.h"
#include "User.h"
#include <thread>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/form.hpp>

// ��� GUI�� �ٷ��. ��Ȳ������ â��ȯ�� �ʿ��� ����
class MainForm
{
public:
	void Init();
	void UpdateSceen();
	void PacketProcess();
private:
	std::unique_ptr<TCPNetwork> m_pTCPNetwork;
	// �߻� Ŭ������ ��ü�� ��ư��� ����
	IClientScene* m_pSceen;
	std::unique_ptr<ClientSceenLogin> m_pSceenLogin;
	std::unique_ptr<ClientMainScene> m_pSceenFriendList;

	// unique�� ���� ������ �޸� ���� ������. �ذ�Ұ�
	std::unique_ptr<nana::form> m_fm;
	nana::timer m_timer;
	std::thread updatethread;
	std::unique_ptr<User> m_pUser;
	std::recursive_mutex m_rm;
public:
	~MainForm();
};
