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
	m_pform = new form(API::make_center(450, 600));
	// text , �̸��˻� ģ����� �η���

	m_pTitlelb = new label(*m_pform, charset("ģ��").to_bytes(unicode::utf8));
	m_pSearchtBox = new textbox(*m_pform);
	m_pSearchtBox->tip_string(charset("�̸� �˻�").to_bytes(unicode::utf8)).multi_lines(false);
	m_pChattroomListBtn = new button(*m_pform, charset("ä��").to_bytes(unicode::utf8));
	m_pFriendListBox = new listbox(*m_pform);
	std::cout << m_User->GetFriendList().size() << std::endl;
	for (auto iter : m_User->GetFriendList())
	{
		std::cout << iter->GetUserID() << std::endl;
	}

	m_pFriendListBox->append_header(charset("Header").to_bytes(unicode::utf8));
	m_pFriendListBox->at(0).append({ "int" });
	m_pFriendListBox->at(0).append({ "asd" });

	place plc{ *m_pform };
	plc.div("<><weight = 80% vert<><weight = 60% friend><>><>");
	plc["friend"] << *m_pFriendListBox;

	plc.collocate();
}