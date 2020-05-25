#include "ClientMainScene.h"
#include "User.h"
#include "SceneMgr.h"
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/checkbox.hpp>

void ClientMainScene::Update()
{
	if (Singleton<SceneMgr>::GetInst()->GetSceen()->GetCurSceenType() != CLIENT_SCENE_TYPE::MAIN)
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
		LOG("ģ���߰� �Ϸ�!\n");
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
	// �г��� 2���ΰ� �ǹٿ����ؼ� ��ȯ�ǵ����ؾ���. �ǹٿ� ���������� �ٿ�����.
	m_pFriendpanel = new nana::panel<false>(*m_pform);
	m_pChattingRoompanel = new nana::panel<false>(*m_pform);
	m_pChattingRoompanel->bgcolor(nana::colors::antique_white);
	m_pTabbar = new nana::tabbar<std::string>(*m_pform);
	m_pTabbar->bgcolor(colors::antique_white);
	m_pFriendSearchtBox = new nana::textbox(*m_pFriendpanel);
	m_pFriendListBox = new nana::listbox(*m_pFriendpanel);
	m_pFriendListBox->bgcolor(nana::colors::antique_white);
	m_pFindIDBtn = new nana::button(*m_pFriendpanel, charset("ģ���߰�").to_bytes(unicode::utf8));
	m_pFindIDBtn->bgcolor(nana::colors::antique_white);
	m_pFindIDBtn->events().click([&]() {FrMainFindIDBtn(); });

	m_pTabbar->append(charset("ģ�����").to_bytes(unicode::utf8), *m_pFriendpanel);
	m_pTabbar->append(charset("ä��").to_bytes(unicode::utf8), *m_pChattingRoompanel);
	m_pFriendSearchtBox->multi_lines(false);

	m_pFriendSearchtBox->events().text_changed([&]() {SetSearchtext(m_pFriendSearchtBox, m_pFriendListBox); });

	m_pFriendpanel->bgcolor(colors::light_yellow);
	m_pFriendSearchtBox->tip_string(charset("ģ��ã��").to_bytes(unicode::utf8));
	m_pFriendListBox->append_header("");
	m_pFriendListBox->show_header(false);

#pragma region ä�ù�
	m_pSearchChatRoom = new nana::textbox(*m_pChattingRoompanel);
	m_pChatRoomList = new nana::listbox(*m_pChattingRoompanel);
	m_pMakeChattingRoomBtn = new nana::button(*m_pChattingRoompanel, charset("���ο�ä��").to_bytes(unicode::utf8));
	m_pChatComment = new nana::label(*m_pChattingRoompanel);
	m_pSearchChatRoom->bgcolor(nana::colors::antique_white);
	m_pSearchChatRoom->multi_lines(false).tip_string(charset("������ �˻�").to_bytes(unicode::utf8));
	m_pMakeChattingRoomBtn->bgcolor(nana::colors::antique_white);
	m_pChatRoomList->bgcolor(nana::colors::antique_white);

	m_pChatRoomList->append_header("");
	m_pChatRoomList->show_header(false);
	m_pMakeChattingRoomBtn->events().click([&]()
	{
		// ��ȭ ���� ���� UI ����
		CreateChatUI();
	});
	std::string chatroomname;
	for (auto iter : m_User->GetChattingRoomList())
	{
		if (iter->GetRoomName().empty())
		{
			// ģ�� �̸� , ������ �̸� ���� ����
			for (auto iter2 : iter->GetJoinnedUsers())
			{
				chatroomname.append(iter2).append(",");
			}
			chatroomname.pop_back();
			m_pChatRoomList->at(0).push_back(charset(chatroomname).to_bytes(unicode::utf8));
		}
		else
		{
			m_pChatRoomList->at(0).push_back(charset(iter->GetRoomName()).to_bytes(unicode::utf8));
		}
	}

	place chatplc{ *m_pChattingRoompanel };
	chatplc.div("vert<margin = [20,15,20,15] weight = 18% gap =10 search><margin = [20,0,20,0] chat>");
	chatplc["search"] << *m_pSearchChatRoom << *m_pMakeChattingRoomBtn;
	chatplc["chat"] << *m_pChatRoomList;
	chatplc.collocate();

#pragma endregion

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

void ClientMainScene::SetSearchtext(nana::textbox* tb, nana::listbox* lb)
{
	std::string str;
	std::wstring wstr = tb->caption_wstring();
	std::list<std::string> searchlist;

	if (convert_unicode_to_ansi_string(str, wstr.c_str(), wstr.size()) != 0)
	{
		str.clear();
	}

	for (auto iter : m_User->GetFriendList())
	{
		std::string nick = iter->GetUserNick();
		if (!str.empty())
		{
			if (nick.find(str) != std::string::npos)
			{
				searchlist.push_back(nick);
				break;
			}
		}
		else
			searchlist.clear();
	}
	lb->clear();
	if (!searchlist.empty())
	{
		for (auto iter : searchlist)
		{
			lb->at(0).push_back(charset(iter).to_bytes(unicode::utf8));
		}
	}
	else
	{
		if (str.length() == 0)
			for (auto iter : m_User->GetFriendList())
			{
				lb->at(0).push_back(charset(iter->GetUserNick()).to_bytes(unicode::utf8));
			}
	}
	searchlist.clear();
}

void ClientMainScene::FrMainFindIDBtn()
{
	m_pFindIDuiForm = new nana::form(API::make_center(250, 200), nana::appear::decorate<appear::taskbar, appear::sizable>());
	m_pFindIDuiForm->bgcolor(nana::colors::light_yellow);
	// label , textbox, btn(����,�߰�)

	m_pfrInlb = new label(*m_pFindIDuiForm, charset("ģ�� �߰�").to_bytes(unicode::utf8));
	m_pfrInOkBtn = new button(*m_pFindIDuiForm, charset("ģ�� �߰�").to_bytes(unicode::utf8));
	m_pfrInCloseBtn = new button(*m_pFindIDuiForm, charset("�ڷ� ����").to_bytes(unicode::utf8));
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
	// ID �ִ��� �˻� ���� ����

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
	// ����Ŭ�� ���ڸ��� �ٷ� ä�ù� �������.
	std::cout << "double click!" << std::endl;
}

void ClientMainScene::FindResult(Stream & stream)
{
	// ã�ҳ� ��ã�ҳ��� bool�� �����ٰ�

	bool isfind = false;
	stream >> &isfind;
	if (isfind)
	{
		// â�ϳ� ����� ã�� ���̵� �ؽ�Ʈ�� ����ְ� ģ���߰� �Ұųİ� �����
		form _fr = form(API::make_center(200, 150), nana::appear::decorate<appear::taskbar, appear::sizable>());
		_fr.bgcolor(nana::colors::light_yellow);
		label* _lb = new label(_fr);
		button* _ok = new button(_fr, charset("ģ�� �߰�").to_bytes(unicode::utf8));
		_ok->bgcolor(nana::colors::antique_white);
		button* _no = new button(_fr, charset("�ݱ�").to_bytes(unicode::utf8));
		_no->bgcolor(nana::colors::antique_white);
		stream >> &foundID;
		std::string capID = "ID : " + foundID;
		_lb->caption(charset(capID).to_bytes(unicode::utf8));
		_ok->events().click([&]()
		{
			// �ش� ������ ģ�� �߰� ���� ����
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
			// â����
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
		// â�ϳ� ����� ���̵� �߸� �Է��ߴ��ϰ� ���ư���

		form _fr = form(API::make_center(200, 150), nana::appear::decorate<appear::taskbar, appear::sizable>());
		_fr.bgcolor(nana::colors::light_yellow);
		label* _lb = new label(_fr);
		button* _ok = new button(_fr, charset("Ȯ��").to_bytes(unicode::utf8));
		_lb->caption(charset("ID�� ã�� �� �����ϴ�").to_bytes(unicode::utf8));
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
		_lb->caption(charset("�̹� ģ������!").to_bytes(unicode::utf8));
		button* _ok = new button(_form);
		_ok->caption(charset("Ȯ��").to_bytes(unicode::utf8));
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

void ClientMainScene::CreateChatUI()
{
	form _form = form(API::make_center(250, 400), nana::appear::decorate<appear::taskbar, appear::sizable>());
	_form.bgcolor(nana::colors::light_yellow);
	// �ؽ�Ʈ�ڽ� �ϳ� ����Ʈ�ڽ� �ϳ� Ȯ�� ��ư ��� ��ư
	textbox* _tb = new textbox(_form);
	listbox* _lb = new listbox(_form);
	label* _label = new label(_form, charset("ģ�����").to_bytes(unicode::utf8));
	button* _ok = new button(_form, charset("Ȯ��").to_bytes(unicode::utf8));
	button* _close = new button(_form, charset("���").to_bytes(unicode::utf8));

	_tb->bgcolor(nana::colors::antique_white);
	_ok->bgcolor(nana::colors::antique_white);
	_close->bgcolor(nana::colors::antique_white);
	_tb->multi_lines(false).tip_string(charset("��ȭ��� �˻�").to_bytes(unicode::utf8));
	_lb->bgcolor(nana::colors::antique_white);

	place _plc{ _form };
	_plc.div("vert<margin = [25,5,25,5] weight = 20% search><weight = 10% lb>< \
		weight = 40% list><><margin = [10,0,10,0] gap = 10 weight = 15% btn>");
	_plc["search"] << *_tb;
	_plc["btn"] << *_ok << *_close;
	_plc["list"] << *_lb;
	_plc["lb"] << *_label;

	_lb->append_header("");
	_lb->show_header(false);
	_lb->checkable(true);
	for (auto iter : m_User->GetFriendList())
	{
		_lb->at(0).push_back(iter->GetUserNick());
	}

	_tb->events().text_changed([&]()
	{
		SetSearchtext(_tb, _lb);
	});

	_plc.collocate();
	_form.show();
	exec();
}