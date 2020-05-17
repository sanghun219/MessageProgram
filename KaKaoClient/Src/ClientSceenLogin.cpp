#include "..\Inc\ClientSceenLogin.h"
#include <nana/gui/widgets/picture.hpp>
void ClientSceenLogin::Update()
{
	if (GetCurSceenType() != CLIENT_SCENE_TYPE::LOGIN)
		return;
}

// 패킷을 받았을 때 처리하는 방식
bool ClientSceenLogin::ProcessPacket(PACKET_ID pkID, const Stream& stream)
{
	switch ((short)pkID)
	{
	case (short)PACKET_ID::PCK_MAKE_ID_RES:
		break;
	case (short)PACKET_ID::PCK_LOGIN_RES:
		loginBtn->caption(charset("로그인 완료!").to_bytes(unicode::utf8));
		SetCurSceenType(CLIENT_SCENE_TYPE::FRIEND_LIST);
		break;
	}

	return false;
}

void ClientSceenLogin::CreateUI(form* pform)
{
	m_pform = pform;

	textbox idbox(*m_pform);
	textbox passbox(*m_pform);
	std::string IDName = "카카오 계정";
	std::string passName = "비밀번호";
	idbox.tip_string(charset(IDName).to_bytes(unicode::utf8)).multi_lines(false);
	passbox.tip_string(charset(passName).to_bytes(unicode::utf8)).multi_lines(false).mask('*');
	idbox.events().text_changed([&]()
	{
		SettingID(charset(idbox.text()).to_bytes(unicode::utf8));
	});
	passbox.events().text_changed([&]()
	{
		SettingPassword(charset(passbox.text()).to_bytes(unicode::utf8));
	});
	loginBtn = new button(*m_pform, charset("로그인").to_bytes(unicode::utf8));
	loginBtn->events().click([&]()
	{
		LoginRequest();
		loginBtn->caption(charset("로그인 중..").to_bytes(unicode::utf8));
	});

	place plc{ *m_pform };
	plc.div("<><weight=80% vertical<><weight=70% vertical <vertical gap=10 textboxs arrange=[25,25]>  <weight=25 gap=10 buttons> ><>><>");
	plc.field("buttons") << *loginBtn;
	plc.field("textboxs") << idbox << passbox;

	plc.collocate();
	m_pform->show();
	exec();
}

ClientSceenLogin::~ClientSceenLogin()
{
}

void ClientSceenLogin::LoginRequest()
{
	// 패킷을 보내고 받아야함. id,password를 서버에 보내고 있는아이디인지확인
	Packet pck;
	ZeroMemory(&pck, sizeof(pck));
	pck.stream = new Stream();
	short pckid = static_cast<short>(PACKET_ID::PCK_LOGIN_REQ);
	*pck.stream << pckid;
	*pck.stream << loginID;
	*pck.stream << LoginPass;

	m_tcpNetwork->SendPacket(pck);
}

void ClientSceenLogin::SettingID(std::string id)
{
	loginID = id;
}

void ClientSceenLogin::SettingPassword(std::string pass)
{
	LoginPass = pass;
}