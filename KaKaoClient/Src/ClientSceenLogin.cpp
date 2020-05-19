#include "..\Inc\ClientSceenLogin.h"
#include <nana/gui/widgets/picture.hpp>
#include "User.h"
void ClientSceenLogin::Update()
{
	if (GetCurSceenType() != CLIENT_SCENE_TYPE::LOGIN)
		return;
}

// ��Ŷ�� �޾��� �� ó���ϴ� ���
bool ClientSceenLogin::ProcessPacket(PACKET_ID pkID, Stream& stream)
{
	switch ((short)pkID)
	{
	case (short)PACKET_ID::PCK_MAKE_ID_RES:
		CreateMakeidUI();
		break;
	case (short)PACKET_ID::PCK_LOGIN_RES:
		LoginResult(stream);
		break;
	case (short)PACKET_ID::PCK_LOGIN_WRONG_PASS_RES:
		CreateWrongPassUI();
		break;
	case (short)PACKET_ID::PCK_SIGN_UP_RES:
		SignupResult(stream);
		break;
	default:
		LOG("%d", pkID);
		LOG("���ŵ� ��Ŷ�� �������� �ʴ� ��Ŷ�Դϴ�. %s", stream.data());
		break;
	}

	return false;
}

void ClientSceenLogin::CreateUI(form* pform)
{
	m_pform = pform;

	m_loginidbox = new textbox(*m_pform);
	m_loginpassbox = new textbox(*m_pform);
	std::string IDName = "īī�� ����";
	std::string passName = "��й�ȣ";
	m_loginidbox->tip_string(charset(IDName).to_bytes(unicode::utf8)).multi_lines(false);
	m_loginpassbox->tip_string(charset(passName).to_bytes(unicode::utf8)).multi_lines(false).mask('*');
	m_loginidbox->events().text_changed([&]()
	{
		SettingLoginID(charset(m_loginidbox->text()).to_bytes(unicode::utf8));
	});
	m_loginpassbox->events().text_changed([&]()
	{
		SettingLoginPassword(charset(m_loginpassbox->text()).to_bytes(unicode::utf8));
	});
	m_loginBtn = new button(*m_pform, charset("�α���").to_bytes(unicode::utf8));
	m_loginBtn->events().click([&]()
	{
		LoginRequest();
		m_loginBtn->caption(charset("�α��� ��..").to_bytes(unicode::utf8));
	});
	m_loginBtn->enabled(false);

	m_SignUpBtn = new button(*m_pform, charset("ȸ������").to_bytes(unicode::utf8));
	m_SignUpBtn->events().click([&]()
	{
		CreateSingUpUI();
	});
	place plc{ *m_pform };
	plc.div("<><weight=80% vert<><weight=70% vert <vert gap=10 textboxs arrange=[25,25]>  <weight=25 gap=10 buttons> ><>><>");
	plc.field("buttons") << *m_loginBtn << *m_SignUpBtn;
	plc.field("textboxs") << *m_loginidbox << *m_loginpassbox;

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
	*pck.stream << m_loginID;
	*pck.stream << m_LoginPass;

	m_tcpNetwork->SendPacket(pck);
	delete pck.stream;
}

void ClientSceenLogin::SignupRequest()
{
	Packet packet;
	ZeroMemory(&packet, sizeof(packet));
	short pkid = static_cast<short>(PACKET_ID::PCK_SIGN_UP_REQ);
	packet.stream = new Stream();
	*packet.stream << pkid;
	*packet.stream << m_signupID;
	*packet.stream << m_signupNick;
	*packet.stream << m_signupPass;

	m_tcpNetwork->SendPacket(packet);
	delete packet.stream;
}

void ClientSceenLogin::SignupResult(Stream & stream)
{
	int res = 0;
	stream >> &res;
	if (res == 1) // OK
	{
		// â���� (ȸ�������� �Ϸ� �ƽ��ϴ�!) - ���ÿ� ȸ������ â �ݴ´�. (������ �������)
		form okform(API::make_center(170, 130));
		label oklabel(okform, charset("ȸ������ �Ϸ�!").to_bytes(unicode::utf8));
		button okbtn(okform, charset("Ȯ��").to_bytes(unicode::utf8));

		okbtn.events().click([&]()
		{
			m_idbox->reset();
			m_passbox->reset();
			m_nicknamebox->reset();
			m_SignUpokbtn->enabled(false);
			okform.close();

			ClearSignUpUI();
		});

		place plc{ okform };
		plc.div("<><weight = 80% vert<><weight = 30% lab><weight = 30% bt> ><>");
		plc["lab"] << oklabel;
		plc["bt"] << okbtn;
		plc.collocate();
		okform.show();
		exec();
	}
	else // ID�� �̹� ������
	{
		// â���� (�̹� �����ϴ� ID �Դϴ�!)  - ������ �������
		form failform(API::make_center(170, 130));
		label failabel(failform, charset("�̹� �����ϴ� ID �Դϴ�!").to_bytes(unicode::utf8));
		button failbtn(failform, charset("Ȯ��").to_bytes(unicode::utf8));

		failbtn.events().click([&]()
		{
			//idbox->clear_undo();

			m_passbox->reset();
			m_nicknamebox->reset();
			m_SignUpokbtn->enabled(false);
			failform.close();
		});
		place plc{ failform };

		plc.div("<><weight = 80% vert<><weight = 30% lab><weight = 30% bt> ><>");
		plc["lab"] << failabel;
		plc["bt"] << failbtn;
		plc.collocate();
		failform.show();
		exec();
	}
}

void ClientSceenLogin::LoginResult(Stream& stream)
{
	// TODO : ��� ������ �����ϱ�.
	// ���� ���� ���� �޾Ƽ� �����ȴ�.
	if (m_User == nullptr)
	{
		LOG("User������ �������� �ʾҽ��ϴ�.");
		return;
	}
	m_User->Read(stream);
	m_loginBtn->caption(charset("�α��� �Ϸ�!").to_bytes(unicode::utf8));
	SetCurSceenType(CLIENT_SCENE_TYPE::FRIEND_LIST);

	LOG("�α����� �Ϸ� �ƽ��ϴ�!");
}

void ClientSceenLogin::SettingSignupID(std::string id)
{
	m_signupID = id;
	if (m_signupID.length() != 0 && m_signupNick.length() != 0 && m_signupPass.length() != 0)
	{
		m_SignUpokbtn->enabled(true);
	}
}

void ClientSceenLogin::SettingSignupNick(std::string nick)
{
	m_signupNick = nick;
	if (m_signupID.length() != 0 && m_signupNick.length() != 0 && m_signupPass.length() != 0)
	{
		m_SignUpokbtn->enabled(true);
	}
}

void ClientSceenLogin::SettingSignupPass(std::string pass)
{
	m_signupPass = pass;
	if (m_signupID.length() != 0 && m_signupNick.length() != 0 && m_signupPass.length() != 0)
	{
		m_SignUpokbtn->enabled(true);
	}
}

void ClientSceenLogin::SettingLoginID(std::string id)
{
	m_loginID = id;
	if (m_loginID.length() != 0 && m_LoginPass.length() != 0)
		m_loginBtn->enabled(true);
}

void ClientSceenLogin::SettingLoginPassword(std::string pass)
{
	m_LoginPass = pass;
	if (m_loginID.length() != 0 && m_LoginPass.length() != 0)
		m_loginBtn->enabled(true);
}

void ClientSceenLogin::CreateMakeidUI()
{
	m_loginBtn->caption(charset("�α���").to_bytes(unicode::utf8));
	form midform(API::make_center(170, 130));
	label midlab(midform, charset("ID�� �������� �ʽ��ϴ�.").to_bytes(unicode::utf8));
	button midbtn(midform, charset("Ȯ��").to_bytes(unicode::utf8));

	midbtn.events().click([&]()
	{
		m_loginpassbox->reset();
		m_LoginPass.clear();
		m_loginBtn->enabled(false);
		midform.close();
	});

	place plc{ midform };

	plc.div("<><weight = 80% vert<><weight = 30% lab><weight = 30% bt> ><>");
	plc["lab"] << midlab;
	plc["bt"] << midbtn;
	plc.collocate();
	midform.show();
	exec();
}

void ClientSceenLogin::CreateSingUpUI()
{
	m_signupform = new form(API::make_center(300, 300));

	m_idbox = new textbox(*m_signupform);
	m_nicknamebox = new textbox(*m_signupform);
	m_passbox = new textbox(*m_signupform);
	m_SignUpokbtn = new button(*m_signupform, charset("ȸ������").to_bytes(unicode::utf8));
	button cancelbtn(*m_signupform, charset("�ڷΰ���").to_bytes(unicode::utf8));

	m_idbox->tip_string(charset("���̵� :").to_bytes(unicode::utf8)).multi_lines(false);
	m_nicknamebox->tip_string(charset("�г��� :").to_bytes(unicode::utf8)).multi_lines(false);
	m_passbox->tip_string(charset("��й�ȣ :").to_bytes(unicode::utf8)).multi_lines(false).mask('*');

	cancelbtn.events().click([&]()
	{
		m_signupID.clear();
		m_signupPass.clear();
		m_signupNick.clear();
		ClearSignUpUI();
	});
	m_SignUpokbtn->enabled(false);
	m_SignUpokbtn->events().click([&]()
	{
		SignupRequest();
	});

	m_idbox->events().text_changed([&]() {SettingSignupID(charset(m_idbox->text()).to_bytes(unicode::utf8)); });
	m_passbox->events().text_changed([&]() {SettingSignupPass(charset(m_passbox->text()).to_bytes(unicode::utf8)); });
	m_nicknamebox->events().text_changed([&]() {SettingSignupNick(charset(m_nicknamebox->text()).to_bytes(unicode::utf8)); });
	place plc{ *m_signupform };
	plc.div("<>< weight = 60% vert<>< weight = 40% vert gap = 10 tbox arrange = [25,25,25]><weight = 25 gap = 10 btn><>><>");

	plc["tbox"] << *m_idbox << *m_nicknamebox << *m_passbox;
	plc["btn"] << *m_SignUpokbtn << cancelbtn;
	plc.collocate();
	m_signupform->show();
	exec();
}

void ClientSceenLogin::CreateWrongPassUI()
{
	m_loginBtn->caption(charset("�α���").to_bytes(unicode::utf8));
	form wrongform(API::make_center(290, 160));
	label wronglabel(wrongform, charset("���� �Ǵ� ��й�ȣ�� �ٽ� Ȯ���� �ּ���.").to_bytes(unicode::utf8));
	button wrongbtn(wrongform, charset("Ȯ��").to_bytes(unicode::utf8));
	wrongbtn.events().click([&]()
	{
		wrongform.close();
	});
	std::cout << "asdasd";
	place plc{ wrongform };
	plc.div("<><weight = 80% vert<>< weight = 30% lab><weight = 20% bt><> ><>");
	plc["lab"] << wronglabel;
	plc["bt"] << wrongbtn;

	plc.collocate();
	wrongform.show();
	exec();
}

void ClientSceenLogin::ClearSignUpUI()
{
	delete m_SignUpokbtn;
	m_SignUpokbtn = nullptr;
	m_signupform->close();
	delete m_signupform;
	m_signupform = nullptr;
}