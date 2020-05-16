#include "MainForm.h"
using namespace nana;
void MainForm::Init()
{
	m_TCPNetwork = new TCPNetwork();
	m_TCPNetwork->ConnectToServer();
	m_SceenLogin = new ClientSceenLogin();
	m_SceenLogin->SetNetwork(m_TCPNetwork);

	m_timer.elapse([&]() {PacketProcess(); });
	m_timer.interval(std::chrono::milliseconds(32));
	m_timer.start();
}

void MainForm::CreateGUI()
{
	m_fm = std::make_unique<form>(API::make_center(300, 450));
	std::string CaptionName = "Ä«Ä«¿ÀÅå";

	m_fm->caption(charset(CaptionName).to_bytes(unicode::utf8));

	m_SceenLogin->CreateUI(m_fm.get());
}

void MainForm::Show()
{
	m_fm->show();
	exec();
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
		m_SceenLogin->ProcessPacket(static_cast<PACKET_ID>(id), *packet->stream);
	}

	m_SceenLogin->Update();
}

MainForm::~MainForm()
{
	if (m_TCPNetwork != nullptr)
		m_TCPNetwork->Clear();
}