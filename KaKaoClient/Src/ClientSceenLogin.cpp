#include "..\Inc\ClientSceenLogin.h"
#include <nana/gui/widgets/picture.hpp>
#include "User.h"
void ClientSceenLogin::Update()
{
	if (GetCurSceenType() != CLIENT_SCENE_TYPE::LOGIN)
		return;
}

// 패킷을 받았을 때 처리하는 방식
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
		LOG("수신된 패킷은 존재하지 않는 패킷입니다. %s", stream.data());
		break;
	}

	return false;
}

void ClientSceenLogin::CreateUI(form* pform)
{
	m_pform = pform;

	m_loginidbox = new textbox(*m_pform);
	m_loginpassbox = new textbox(*m_pform);
	std::string IDName = "카카오 계정";
	std::string passName = "비밀번호";
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
	m_loginBtn = new button(*m_pform, charset("로그인").to_bytes(unicode::utf8));
	m_loginBtn->events().click([&]()
	{
		LoginRequest();
		m_loginBtn->caption(charset("로그인 중..").to_bytes(unicode::utf8));
	});
	m_loginBtn->enabled(false);

	m_SignUpBtn = new button(*m_pform, charset("회원가입").to_bytes(unicode::utf8));
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
	// 패킷을 보내고 받아야함. id,password를 서버에 보내고 있는아이디인지확인
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
		// 창띄운다 (회원가입이 완료 됐습니다!) - 동시에 회원가입 창 닫는다. (떼이터 다지우고)
		form okform(API::make_center(170, 130));
		label oklabel(okform, charset("회원가입 완료!").to_bytes(unicode::utf8));
		button okbtn(okform, charset("확인").to_bytes(unicode::utf8));

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
	else // ID가 이미 존재함
	{
		// 창띄운다 (이미 존재하는 ID 입니다!)  - 데이터 다지우고
		form failform(API::make_center(170, 130));
		label failabel(failform, charset("이미 존재하는 ID 입니다!").to_bytes(unicode::utf8));
		button failbtn(failform, charset("확인").to_bytes(unicode::utf8));

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
	// TODO : 어떻게 받을지 생각하기.
	// 유저 정보 전부 받아서 관리된다.
	if (m_User == nullptr)
	{
		LOG("User정보가 설정되지 않았습니다.");
		return;
	}
	m_User->Read(stream);
	m_loginBtn->caption(charset("로그인 완료!").to_bytes(unicode::utf8));
	SetCurSceenType(CLIENT_SCENE_TYPE::FRIEND_LIST);

	LOG("로그인이 완료 됐습니다!");
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
	m_loginBtn->caption(charset("로그인").to_bytes(unicode::utf8));
	form midform(API::make_center(170, 130));
	label midlab(midform, charset("ID가 존재하지 않습니다.").to_bytes(unicode::utf8));
	button midbtn(midform, charset("확인").to_bytes(unicode::utf8));

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
	m_SignUpokbtn = new button(*m_signupform, charset("회원가입").to_bytes(unicode::utf8));
	button cancelbtn(*m_signupform, charset("뒤로가기").to_bytes(unicode::utf8));

	m_idbox->tip_string(charset("아이디 :").to_bytes(unicode::utf8)).multi_lines(false);
	m_nicknamebox->tip_string(charset("닉네임 :").to_bytes(unicode::utf8)).multi_lines(false);
	m_passbox->tip_string(charset("비밀번호 :").to_bytes(unicode::utf8)).multi_lines(false).mask('*');

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
	m_loginBtn->caption(charset("로그인").to_bytes(unicode::utf8));
	form wrongform(API::make_center(290, 160));
	label wronglabel(wrongform, charset("계정 또는 비밀번호를 다시 확인해 주세요.").to_bytes(unicode::utf8));
	button wrongbtn(wrongform, charset("확인").to_bytes(unicode::utf8));
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