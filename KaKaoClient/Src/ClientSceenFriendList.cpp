#include "ClientSceenFriendList.h"
#include "User.h"
#include "nanaUtil.h"
#include <set>

void ClientSceenFriendList::Update()
{
	if (GetCurSceenType() != CLIENT_SCENE_TYPE::FRIEND_LIST)
		return;
}

bool ClientSceenFriendList::ProcessPacket(PACKET_ID pkID, Stream & stream)
{
	return false;
}

void ClientSceenFriendList::CreateUI()
{
	m_pform = new form(API::make_center(250, 400), nana::appear::decorate<appear::taskbar, appear::sizable>());
	m_pform->bgcolor(colors::light_yellow);
	// 패널을 2개두고 탭바에의해서 전환되도록해야함. 탭바에 탭패이지를 붙여야함.
	m_pFriendpanel = new nana::panel<false>(*m_pform);
	m_pChattingRoompanel = new nana::panel<false>(*m_pform);
	m_pTabbar = new nana::tabbar<std::string>(*m_pform);
	m_pFriendSearchtBox = new nana::textbox(*m_pFriendpanel);
	m_pFriendListBox = new nana::listbox(*m_pFriendpanel);
	m_pFindIDBtn = new nana::button(*m_pFriendpanel, charset("ID 검색").to_bytes(unicode::utf8));

	m_pFindIDBtn->events().click([&]() {});

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

	m_pFriendListBox->events().category_dbl_click([&]() {});

	place friendplc{ *m_pFriendpanel };

	friendplc.div("vert margin =[25,50,20,50] < search weight = 6% ><weight = 6% ><frlist>");
	friendplc["search"] << *m_pFriendSearchtBox;
	friendplc["frlist"] << *m_pFriendListBox;

	place plc{ *m_pform };
	plc.div("vert <weight = 30 tab><tabframe>");

	plc["tab"] << *m_pTabbar;
	plc["tabframe"].fasten(*m_pFriendpanel).fasten(*m_pChattingRoompanel);
	plc.collocate();
}

void ClientSceenFriendList::SetSearchtext()
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