#pragma once
#include "TCPNetwork.h"
#include "IClientScene.h"
#include "ClientSceenLogin.h"
#include "ClientMainScene.h"
#include "User.h"
#include <thread>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/form.hpp>

// 모든 GUI를 다룬다. 상황에따른 창전환이 필요한 공간
class MainForm
{
public:
	void Init();
	void UpdateSceen();
	void PacketProcess();
private:
	std::unique_ptr<TCPNetwork> m_pTCPNetwork;
	// 추상 클래스는 객체로 담아갈수 없음
	IClientScene* m_pSceen;
	std::unique_ptr<ClientSceenLogin> m_pSceenLogin;
	std::unique_ptr<ClientMainScene> m_pSceenFriendList;

	// unique를 쓰지 않으니 메모리 릭에 빠진다. 해결불가
	std::unique_ptr<nana::form> m_fm;
	nana::timer m_timer;
	std::thread updatethread;
	std::unique_ptr<User> m_pUser;
	std::recursive_mutex m_rm;
public:
	~MainForm();
};
