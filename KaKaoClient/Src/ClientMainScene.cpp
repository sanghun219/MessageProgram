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
	m_pChattingRoompanel->bgcolor(nana::colors::antique_white);
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

	m_pFriendSearchtBox->events().text_changed([&]() {SetSearchtext(m_pFriendSearchtBox, m_pFriendListBox); });

	m_pFriendpanel->bgcolor(colors::light_yellow);
	m_pFriendSearchtBox->tip_string(charset("친구찾기").to_bytes(unicode::utf8));
	m_pFriendListBox->append_header("");
	m_pFriendListBox->show_header(false);

#pragma region 채팅방
	m_pSearchChatRoom = new nana::textbox(*m_pChattingRoompanel);
	m_pChatRoomList = new nana::listbox(*m_pChattingRoompanel);
	m_pMakeChattingRoomBtn = new nana::button(*m_pChattingRoompanel, charset("새로운채팅").to_bytes(unicode::utf8));
	m_pChatComment = new nana::label(*m_pChattingRoompanel);
	m_pSearchChatRoom->bgcolor(nana::colors::antique_white);
	m_pSearchChatRoom->multi_lines(false).tip_string(charset("참여자 검색").to_bytes(unicode::utf8));
	m_pMakeChattingRoomBtn->bgcolor(nana::colors::antique_white);
	m_pChatRoomList->bgcolor(nana::colors::antique_white);

	m_pChatRoomList->append_header("");
	m_pChatRoomList->show_header(false);
	m_pMakeChattingRoomBtn->events().click([&]()
	{
		// 대화 상태 선택 UI 생성
		CreateChatUI();
	});
	std::string chatroomname;
	for (auto iter : m_User->GetChattingRoomList())
	{
		if (iter->GetRoomName().empty())
		{
			// 친구 이름 , 참여한 이름 몇을 적음
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

	// 초기화
	for (auto iter : m_User->GetFriendList())
	{
		std::string nickname = iter->GetUserNick();
		std::string id = iter->GetUserID();
		m_IDtoNick[id] = nickname;
		m_NicktoID[nickname] = id;
	}
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
		std::string capID = "닉네임 : " + m_IDtoNick[foundID];
		_lb->caption(charset(capID).to_bytes(unicode::utf8));
		_ok->events().click([&]()
		{
			// 해당 유저에 친구 추가 쿼리 실행
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

void ClientMainScene::CreateChatUI()
{
	form _form = form(API::make_center(250, 400), nana::appear::decorate<appear::taskbar, appear::sizable>());
	_form.bgcolor(nana::colors::light_yellow);
	// 텍스트박스 하나 리스트박스 하나 확인 버튼 취소 버튼
	textbox* _tb = new textbox(_form);
	listbox* _lb = new listbox(_form);
	label* _label = new label(_form, charset("친구목록").to_bytes(unicode::utf8));
	button* _ok = new button(_form, charset("확인").to_bytes(unicode::utf8));
	button* _close = new button(_form, charset("취소").to_bytes(unicode::utf8));

	_tb->bgcolor(nana::colors::antique_white);
	_ok->bgcolor(nana::colors::antique_white);
	_close->bgcolor(nana::colors::antique_white);
	_tb->multi_lines(false).tip_string(charset("대화상대 검색").to_bytes(unicode::utf8));
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
		_lb->at(0).push_back(charset(iter->GetUserNick()).to_bytes(unicode::utf8));
	}

	_tb->events().text_changed([&]()
	{
		SetSearchtext(_tb, _lb);
	});

	_ok->events().click([&]()
	{
		auto checkedbox = _lb->checked();
		std::vector<std::string> names;
		for (auto iter : checkedbox)
		{
			auto index = iter.item;
			std::string name;
			std::wstring wname;

			// 해당 인덱스의 유저들을 찾고 대화창 만드는 함수로 넘겨주자
			_lb->at(0).at(index).resolve_to(wname);
			convert_unicode_to_ansi_string(name, wname.c_str(), wname.size());
			names.push_back(name);
		}
		CreateChattingRoom(names);
		_form.close();
	});

	_close->events().click([&]() {_form.close(); });

	_plc.collocate();
	_form.show();
	exec();
}

void ClientMainScene::CreateChattingRoom(const std::vector<std::string>& nicknames, int ChatRoomID, bool isCreatedRoom)
{
	for (auto iter : nicknames)
		std::cout << iter << std::endl;

	// 채팅방 만들기 , 기존 채팅방 ..
	std::unique_ptr<ChatRoomUI> ui = std::make_unique<ChatRoomUI>();
	std::unique_ptr<form> _fr = std::make_unique<nana::form>(API::make_center(250, 400), nana::appear::decorate<appear::taskbar, appear::sizable>());
	std::unique_ptr<listbox> _chattings = std::make_unique<nana::listbox>(*_fr.get());
	std::unique_ptr<textbox> _Input = std::make_unique<nana::textbox>(*_fr.get());
	std::unique_ptr<button> _sendbtn = std::make_unique<nana::button>(*_fr.get(), charset("전송").to_bytes(unicode::utf8));
	_fr->bgcolor(nana::colors::light_yellow);
	_fr->events().destroy([&]()
	{
		for (auto iter = m_pChattingRooms.begin(); iter != m_pChattingRooms.end(); )
		{
		}
	});
	// 얘가 기존이 존재한 방인지 아닌지를 판단 해야함. 기존 존재방이면 정보 다 끌고 오자고

	if (isCreatedRoom)
	{
		// 여기서 chatroonid는 룸id가 아니라 유저가 가지고 있는 채팅룸의 인덱스임
		// 채팅룸 인덱스를 진짜번호로 바꿔야함.
		for (auto iter : m_User->GetChattingRoomList()[ChatRoomID]->GetChattingDataList())
		{
			_chattings->at(0).push_back(iter.m_UserID);
			std::string contentstime;
			contentstime += iter.m_Contents;
			contentstime += "  ";
			contentstime += iter.m_Senddate;
			_chattings->at(0).push_back(contentstime);
		}
	}
	else
	{
		// 서버에 채팅방 만들어달라고 요청하고 룸넘버 받아야함.
		short pkid = static_cast<short>(PACKET_ID::PCK_MAKE_CHATTING_ROOM_REQ);
		Packet SendPacket;
		SendPacket.stream = new Stream();
		*SendPacket.stream << pkid;
		m_tcpNetwork->SendPacket(SendPacket);
	}

	_chattings->bgcolor(nana::colors::antique_white);
	_chattings->append_header("");
	_chattings->show_header(false);

	_Input->bgcolor(nana::colors::white);
	_Input->multi_lines(true);
	_Input->events().key_char([&](const nana::arg_keyboard& _arg)
	{
		if (_arg.key == keyboard::enter)
		{
			// send
			std::cout << "Send!" << std::endl;
			std::wstring ws = _Input->caption_wstring();
			std::string s;
			convert_unicode_to_ansi_string(s, ws.c_str(), ws.size());

			if (!s.empty())
			{
				std::string date = ConvertCurrentDateTimeToString();
				std::string id = m_User->GetUserID();
				std::string nick = m_User->GetUserNick();
				int RoomID = ChatRoomID;
				std::string contents;
				contents.assign(s.begin(), s.end());

				Packet Sendpacket;
				Sendpacket.stream = new Stream();
				short pkid = static_cast<short>(PACKET_ID::PCK_SEND_CHATTING_DATA_REQ);
				*Sendpacket.stream << pkid;
				*Sendpacket.stream << ChatRoomID;
				*Sendpacket.stream << date;
				*Sendpacket.stream << id;
				*Sendpacket.stream << nick;
				*Sendpacket.stream << contents;

				m_tcpNetwork->SendPacket(Sendpacket);
			}
		}
	});

	_sendbtn->bgcolor(nana::colors::antique_white);
	_sendbtn->events().click([&]()
	{
		// send
	});

	std::unique_ptr<place> _plc = std::make_unique<place>(*_fr.get());
	_plc->div("vert <><weight=10%><weight =60% chat><weight =30% gap = 10 arrange = [80%,20%] input>");
	(*_plc)["chat"] << *_chattings.get();
	(*_plc)["input"] << *_Input.get() << *_sendbtn.get();

	_plc->collocate();
	_fr->show();
	exec();

	ui->uiform = _fr.get();
	ui->chatlist = _chattings.get();
	ui->input = _Input.get();
	ui->sendbtn = _sendbtn.get();

	m_pChattingRooms.push_back(ui.get());
}