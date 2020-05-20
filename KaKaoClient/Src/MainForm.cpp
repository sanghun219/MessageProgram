#include "MainForm.h"
#include "IClientScene.h"
#include "ClientSceenLogin.h"
#include "ClientSceenFriendList.h"
#include "User.h"
#include "SceneMgr.h"
#include <thread>

using namespace nana;
void MainForm::Init()
{
	m_TCPNetwork = new TCPNetwork();
	m_TCPNetwork->ConnectToServer();
	m_User = new User();

	Singleton<SceneMgr>::GetInst()->SetSceen(CLIENT_SCENE_TYPE::LOGIN);

	m_timer.elapse([&]() {PacketProcess(); });
	m_timer.interval(std::chrono::milliseconds(32));
	m_timer.start();
}

void MainForm::UpdateSceen()
{
	bool isNeedUpdate = Singleton<SceneMgr>::GetInst()->IsNeedUpdate();
	if (isNeedUpdate)
	{
		m_Sceen = Singleton<SceneMgr>::GetInst()->GetSceen();
		m_Sceen->SetNetwork(m_TCPNetwork);
		m_Sceen->SetUserInfo(m_User);
		m_Sceen->CreateUI();
		m_Sceen->Show();
	}
}

void MainForm::PacketProcess()
{
	if (!m_TCPNetwork)
		return;

	auto packet = m_TCPNetwork->GetPacket();
	if (packet != nullptr)
	{
		short id = -1;
		*packet->stream >> &id;
		m_Sceen->ProcessPacket(static_cast<PACKET_ID>(id), *packet->stream);

		if (packet->stream != nullptr)
		{
			delete packet->stream;
		}
	}
	m_Sceen->Update();
}

MainForm::~MainForm()
{
	if (m_TCPNetwork != nullptr)
		m_TCPNetwork->Clear();
	if (updatethread.joinable())
		updatethread.join();
}