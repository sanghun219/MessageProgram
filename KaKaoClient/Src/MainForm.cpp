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
	m_pTCPNetwork = std::make_unique<TCPNetwork>();
	m_pTCPNetwork->ConnectToServer();
	m_pUser = std::make_unique<User>();

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
		m_pSceen = Singleton<SceneMgr>::GetInst()->GetSceen();
		m_pSceen->SetNetwork(m_pTCPNetwork.get());
		m_pSceen->SetUserInfo(m_pUser.get());
		m_pSceen->CreateUI();
		m_pSceen->Show();
	}
}

void MainForm::PacketProcess()
{
	if (!m_pTCPNetwork)
		return;

	std::unique_ptr<Packet> _pPacket;
	auto packet = m_pTCPNetwork->GetPacket();
	if (packet.stream != nullptr)
		_pPacket = std::make_unique<Packet>(packet);

	if (_pPacket != nullptr)
	{
		short id = -1;
		*_pPacket->stream >> &id;
		m_pSceen->ProcessPacket(static_cast<PACKET_ID>(id), *_pPacket->stream);
		_pPacket.release();
	}
	m_pSceen->Update();
}

MainForm::~MainForm()
{
	if (updatethread.joinable())
		updatethread.join();
}