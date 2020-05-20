#pragma once
#include "TCPNetwork.h"
#include "IClientScene.h"
#include <thread>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/form.hpp>
// ��� GUI�� �ٷ��. ��Ȳ������ â��ȯ�� �ʿ��� ����
class User;
class ClientSceenLogin;
class ClientSceenFriendList;
class MainForm
{
public:
	void Init();
	void UpdateSceen();
	void PacketProcess();
private:
	TCPNetwork* m_TCPNetwork;
	IClientScene* m_Sceen;
	ClientSceenLogin* m_SceenLogin;
	ClientSceenFriendList* m_SceenFriendList;

	// unique�� ���� ������ �޸� ���� ������. �ذ�Ұ�
	std::unique_ptr<nana::form> m_fm;
	nana::timer m_timer;
	std::thread updatethread;
	User* m_User;
public:
	~MainForm();
};
