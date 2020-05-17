#include "..\Inc\ClientSceenLogin.h"
#include <nana/gui/widgets/picture.hpp>
void ClientSceenLogin::Update()
{
	if (GetCurSceenType() != CLIENT_SCENE_TYPE::LOGIN)
		return;
}

// ��Ŷ�� �޾��� �� ó���ϴ� ���
bool ClientSceenLogin::ProcessPacket(PACKET_ID pkID, const Stream& stream)
{
	switch ((short)pkID)
	{
	case (short)PACKET_ID::PCK_MAKE_ID_RES:
		break;
	case (short)PACKET_ID::PCK_LOGIN_RES:
		loginBtn->caption(charset("�α��� �Ϸ�!").to_bytes(unicode::utf8));
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
	std::string IDName = "īī�� ����";
	std::string passName = "��й�ȣ";
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
	loginBtn = new button(*m_pform, charset("�α���").to_bytes(unicode::utf8));
	loginBtn->events().click([&]()
	{
		LoginRequest();
		loginBtn->caption(charset("�α��� ��..").to_bytes(unicode::utf8));
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
	// ��Ŷ�� ������ �޾ƾ���. id,password�� ������ ������ �ִ¾��̵�����Ȯ��
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