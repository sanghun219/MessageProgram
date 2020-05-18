#pragma once
#include "TCPNetwork.h"
#include "ClientSceenLogin.h"

// 모든 GUI를 다룬다. 상황에따른 창전환이 필요한 공간
class User;
class MainForm
{
public:
	void Init();
	void CreateGUI();
	void Show();
	void PacketProcess();

private:
	TCPNetwork* m_TCPNetwork;
	ClientSceenLogin* m_SceenLogin;
	// unique를 쓰지 않으니 메모리 릭에 빠진다. 해결불가
	std::unique_ptr<form> m_fm;
	timer m_timer;

	User* m_User;
public:
	~MainForm();
};
