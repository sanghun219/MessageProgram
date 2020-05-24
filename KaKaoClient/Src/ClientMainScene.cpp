#include "ClientMainScene.h"
#include "User.h"

void ClientMainScene::Update()
{
	if (GetCurSceenType() != CLIENT_SCENE_TYPE::FRIEND_LIST)
		return;
}

bool ClientMainScene::ProcessPacket(PACKET_ID pkID, Stream & stream)
{
	switch ((short)pkID)
	{
	case (short)PACKET_ID::PCK_FIND_ID_RES:
		FindResult(stream);
		break;
	case (short)PACKET_ID::PCK_ADD_FRIEND_RES:
		LOG("친구추가 완료!\n");
		AddFriendResult(stream);
		break;
	default:
		break;
	}

	return false;
}

void ClientMainScene::CreateUI()
{
	m_pform = new form(API::make_center(250, 400), nana::appear::decorate<appear::taskbar, appear::sizable>());
	m_pform->bgcolor(colors::light_yellow);
	// 패널을 2개두고 탭바에의해서 전환되도록해야함. 탭바에 탭패이지를 붙여야함.
	m_pFriendpanel = new nana::panel<false>(*m_pform);
	m_pChattingRoompanel = new nana::panel<false>(*m_pform);
	m_pTabbar = new nana::tabbar<std::string>(*m_pform);
	m_pTabbar->bgcolor(colors::antique_white);
	m_pFriendSearchtBox = new nana::textbox(*m_pFriendpanel);
	m_pFriendListBox = new nana::listbox(*m_pFriendpanel);
	m_pFriendListBox->bgcolor(nana::colors::antique_white);
	m_pFindIDBtn = new nana::button(*m_pFriendpanel, charset("친구추가").to_bytes(unicode::utf8));
	m_pFindIDBtn->bgcolor(nana::colors::antique_white);
	m_pFindIDBtn->events().click([&]() {FrMainFindIDBtn(); });

	m_pTabbar->append(charset("친구목록").to_bytes(unicode::utf8), *m_pFriendpanel);
	m_pTabbar->append(charset("채팅").to_bytes(unicode::utf8), *m_pChattingRoompanel);
	m_pFriendSearchtBox->multi_lines(false);

	m_pFriendSearchtBox->events().text_changed([&]() {SetSearchtext(); });

	m_pFriendpanel->bgcolor(colors::light_yellow);
	m_pFriendSearchtBox->tip_string(charset("친구찾기").to_bytes(unicode::utf8));
	m_pFriendListBox->append_header("");
	m_pFriendListBox->show_header(false);
	for (auto iter : m_User->GetFriendList())
	{
		m_pFriendListBox->at(0).append(charset(iter->GetUserNick()).to_bytes(unicode::utf8));
	}

	m_pFriendListBox->events().dbl_click([&]() {FrMainOpenChatUI(); });

	place friendplc{ *m_pFriendpanel };

	friendplc.div("vert margin =[25,50,20,50] < search weight = 6% gap = 10 ><weight = 6% ><frlist>");
	friendplc["search"] << *m_pFriendSearchtBox << *m_pFindIDBtn;
	friendplc["frlist"] << *m_pFriendListBox;

	place plc{ *m_pform };
	plc.div("vert <weight = 30 tab><tabframe>");

	plc["tab"] << *m_pTabbar;
	plc["tabframe"].fasten(*m_pFriendpanel).fasten(*m_pChattingRoompanel);
	plc.collocate();
}

void ClientMainScene::SetSearchtext()
{
	std::wstring wstr = m_pFriendSearchtBox->caption_wstring();
	std::list<std::string> searchlist;

	if (convert_unicode_to_ansi_string(m_frSearchtext, wstr.c_str(), wstr.size()) != 0)
	{
		m_frSearchtext.clear();
	}

	for (auto iter : m_User->GetFriendList())
	{
		std::string nick = iter->GetUserNick();
		if (!m_frSearchtext.empty())
		{
			if (nick.find(m_frSearchtext) != std::string::npos)
			{
				searchlist.push_back(nick);
				break;
			}
		}
		else
			searchlist.clear();
	}
	m_pFriendListBox->clear();
	if (!searchlist.empty())
	{
		for (auto iter : searchlist)
		{
			m_pFriendListBox->at(0).push_back(charset(iter).to_bytes(unicode::utf8));
		}
	}
	else
	{
		if (m_frSearchtext.length() == 0)
			for (auto iter : m_User->GetFriendList())
			{
				m_pFriendListBox->at(0).push_back(charset(iter->GetUserNick()).to_bytes(unicode::utf8));
			}
	}
	searchlist.clear();
}

void ClientMainScene::FrMainFindIDBtn()
{
	m_pFindIDuiForm = new nana::form(API::make_center(250, 200), nana::appear::decorate<appear::taskbar, appear::sizable>());
	m_pFindIDuiForm->bgcolor(nana::colors::light_yellow);
	// label , textbox, btn(종료,추가)

	m_pfrInlb = new label(*m_pFindIDuiForm, charset("친구 추가").to_bytes(unicode::utf8));
	m_pfrInOkBtn = new button(*m_pFindIDuiForm, charset("친구 추가").to_bytes(unicode::utf8));
	m_pfrInCloseBtn = new button(*m_pFindIDuiForm, charset("뒤로 가기").to_bytes(unicode::utf8));
	m_pfindIDBox = new textbox(*m_pFindIDuiForm);
	m_pfrInOkBtn->bgcolor(nana::colors::antique_white);
	m_pfrInCloseBtn->bgcolor(nana::colors::antique_white);
	m_pfindIDBox->tip_string("ID").multi_lines(false);

	m_pfrInOkBtn->events().click([&]() {FrInFindIDBtn(); });
	m_pfrInCloseBtn->events().click([&]() {FrInCloseFindIDFormBtn(); });
	m_pfindIDBox->events().text_changed([&]() {FrInIDBox(); });
	m_pfrInOkBtn->enabled(false);
	place plc{ *m_pFindIDuiForm };
	plc.div("vert margin = [25,50,20,50]  < weight = 10% lab><>< vert weight = 20% tbox ><>< vert weight = 30 <gap =10 tbtn>>");
	plc["lab"] << *m_pfrInlb;
	plc["tbox"] << *m_pfindIDBox;
	plc["tbtn"] << *m_pfrInOkBtn << *m_pfrInCloseBtn;

	plc.collocate();
	m_pFindIDuiForm->show();
	exec();
}

void ClientMainScene::FrInFindIDBtn()
{
	// ID 있는지 검색 쿼리 진행

	std::string sendtext;
	convert_unicode_to_ansi_string(sendtext, m_pfindIDBox->caption_wstring().c_str(), m_pfindIDBox->caption_wstring().size());

	Packet pck;
	ZeroMemory(&pck, sizeof(pck));
	pck.stream = new Stream();
	short pkid = static_cast<short>(PACKET_ID::PCK_FIND_ID_REQ);
	*pck.stream << pkid;

	*pck.stream << sendtext;

	m_tcpNetwork->SendPacket(pck);
}

void ClientMainScene::FrInCloseFindIDFormBtn()
{
	m_pfindIDBox->caption().clear();
	m_pFindIDuiForm->close();
}

void ClientMainScene::FrInIDBox()
{
	std::string convertedText;
	convert_unicode_to_ansi_string(convertedText, m_pfindIDBox->caption_wstring().c_str(), m_pfindIDBox->caption_wstring().size());
	if (convertedText.length() < 1)return;
	if (!m_pfrInOkBtn->enabled())
		m_pfrInOkBtn->enabled(true);
}

void ClientMainScene::FrMainOpenChatUI()
{
	// 더블클릭 하자마자 바로 채팅방 띄워야함.
	std::cout << "double click!" << std::endl;
}

void ClientMainScene::FindResult(Stream & stream)
{
	// 찾았나 못찾았나를 bool로 보내줄것

	bool isfind = false;
	stream >> &isfind;
	if (isfind)
	{
		// 창하나 띄워서 찾은 아이디 텍스트로 띄워주고 친구추가 할거냐고 물어보기
		form _fr = form(API::make_center(200, 150), nana::appear::decorate<appear::taskbar, appear::sizable>());
		_fr.bgcolor(nana::colors::light_yellow);
		label* _lb = new label(_fr);
		button* _ok = new button(_fr, charset("친구 추가").to_bytes(unicode::utf8));
		_ok->bgcolor(nana::colors::antique_white);
		button* _no = new button(_fr, charset("닫기").to_bytes(unicode::utf8));
		_no->bgcolor(nana::colors::antique_white);
		stream >> &foundID;
		std::string capID = "ID : " + foundID;
		_lb->caption(charset(capID).to_bytes(unicode::utf8));
		_ok->events().click([&]()
		{
			// 해당 유저에 친구 추가 쿼리 실행
			std::cout << "find??" << std::endl;
			Packet sendpack;
			sendpack.stream = new Stream();
			short pkid = static_cast<short>(PACKET_ID::PCK_ADD_FRIEND_REQ);
			*sendpack.stream << pkid;
			*sendpack.stream << m_User->GetUserID();
			*sendpack.stream << foundID;
			m_tcpNetwork->SendPacket(sendpack);
			delete sendpack.stream;
		});
		_no->events().click([&]()
		{
			// 창종료
			_lb->close();
			_no->close();
			_ok->close();
			_fr.close();

			delete _lb;
			delete _no;
			delete _ok;
		});

		place _plc{ _fr };
		_plc.div("vert maring = [20,10,20,10]< ><<weight = 10 >lb<weight = 10>><><weight = 30% gap = 10 btn>");
		_plc["lb"] << *_lb;
		_plc["btn"] << *_ok << *_no;
		_plc.collocate();

		_fr.show();
		exec();
	}
	else
	{
		// 창하나 띄워서 아이디를 잘못 입력했다하고 돌아가기

		form _fr = form(API::make_center(200, 150), nana::appear::decorate<appear::taskbar, appear::sizable>());
		_fr.bgcolor(nana::colors::light_yellow);
		label* _lb = new label(_fr);
		button* _ok = new button(_fr, charset("확인").to_bytes(unicode::utf8));
		_lb->caption(charset("ID를 찾을 수 없습니다").to_bytes(unicode::utf8));
		_ok->events().click([&]()
		{
			_ok->close();
			_lb->close();
			_fr.close();
		});

		place _plc{ _fr };
		_plc.div("vert margin = [30,25,20,15] <weight = 10%><><vert weight = 30% lb><weight = 30% btn");
		_plc["lb"] << *_lb;
		_plc["btn"] << *_ok;
		_plc.collocate();
		_fr.show();
		exec();
	}
}

void ClientMainScene::AddFriendResult(Stream & stream)
{
	User* user = new User();
	bool isChanged = false;
	stream >> &isChanged;
	if (isChanged)
	{
		user->Read(stream, true);
		m_User->SetFriendList(user);
		m_pFriendListBox->at(0).push_back(charset(user->GetUserNick()).to_bytes(unicode::utf8));
		m_pFriendListBox->sort_col(0);
	}
	else
	{
		form _form = form(API::make_center(200, 150), nana::appear::decorate<appear::taskbar, appear::sizable>());
		_form.bgcolor(nana::colors::light_yellow);
		label* _lb = new label(_form);
		_lb->caption(charset("이미 친구에요!").to_bytes(unicode::utf8));
		button* _ok = new button(_form);
		_ok->caption(charset("확인").to_bytes(unicode::utf8));
		_ok->events().click([&]() {_form.close(); });
		place plc{ _form };
		plc.div("vert margin = [10,10,10,10] <><weight = 30% lb><><weight = 15% ok>");
		plc["lb"] << *_lb;
		plc["ok"] << *_ok;
		plc.collocate();
		_form.show();
		exec();
		return;
	}
}