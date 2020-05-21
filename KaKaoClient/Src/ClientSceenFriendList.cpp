#include "ClientSceenFriendList.h"
#include "User.h"
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
	m_pform = new form(API::make_center(350, 500), nana::appear::decorate<appear::taskbar, appear::sizable>());
	m_pform->bgcolor(colors::light_yellow);
	// text , 이름검색 친구목록 두루루룩

	m_pfrgroup.create(*m_pform, nana::rectangle(0, 0, 350, 500));
	m_pchgroup.create(*m_pform);
	m_pchgroup.caption(charset("채팅방").to_bytes(unicode::utf8));
	m_pform->div("vert margin = 10 ");

	m_pfrgroup.caption(charset("친구목록").to_bytes(unicode::utf8));
	tabbar<size_t> tb(m_pfrgroup, nana::rectangle(0, 50, 50, 100));
	tb.push_back("Page 1");
	tb.push_back("Page 2");

	panel<true> page1(m_pfrgroup, true), page2(m_pfrgroup, true);
	page1.bgcolor(colors::light_yellow);
	page2.bgcolor(colors::light_green);
	tb.tab_bgcolor(0, colors::light_yellow);
	tb.tab_bgcolor(1, colors::light_green);
	tb.attach(0, page1);
	tb.attach(1, page2);

	m_pfrgroup.div("vert margin = 15 <tabbar weight =20> <tabpage>");
	m_pfrgroup["tabbar"] << tb;
	m_pfrgroup["tabpage"].fasten(page1);
	m_pfrgroup["tabpage"].fasten(page2);
	m_pfrgroup.collocate();

	m_pTitlelb = new label(*m_pform, nana::rectangle(90, 50, 50, 120));
	m_pTitlelb->caption(charset("친구").to_bytes(unicode::utf8));

	m_pSearchtBox = new textbox(*m_pform, nana::rectangle(90, 70, 100, 30));
	m_pSearchtBox->tip_string(charset("이름 검색").to_bytes(unicode::utf8)).multi_lines(false);
	/*m_pChattroomListBtn = new button(*m_pform, nana::rectangle(0, 0, 30, 40));
	m_pChattroomListBtn->caption(charset("채팅").to_bytes(unicode::utf8));*/
	m_pFriendListBox = new listbox(*m_pform, nana::rectangle(90, 100, 150, 300));
	m_pTabbar = new tabbar<std::string>(*m_pform);
	m_pTabbar->push_back("test");
	m_pFriendListBox->append_header(charset("친구목록").to_bytes(unicode::utf8), 145);

	for (auto iter : m_User->GetFriendList())
	{
		m_pFriendListBox->at(0).append({ charset(iter->GetUserNick()).to_bytes(unicode::utf8) });
	}
}