#pragma once
#include "TCPNetwork.h"
#include "IClientScene.h"
#include <thread>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/form.hpp>
// 모든 GUI를 다룬다. 상황에따른 창전환이 필요한 공간
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

	// unique를 쓰지 않으니 메모리 릭에 빠진다. 해결불가
	std::unique_ptr<nana::form> m_fm;
	nana::timer m_timer;
	std::thread updatethread;
	User* m_User;
public:
	~MainForm();
};
