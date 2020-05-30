#include "ClientMainScene.h"
#include "User.h"
#include "SceneMgr.h"
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/checkbox.hpp>

void ClientMainScene::Update()
{
	if (Singleton<SceneMgr>::GetInst()->GetSceen()->GetCurSceenType() != CLIENT_SCENE_TYPE::MAIN)
		return;

	// 변경사항을 요구하는 경우가 있을수 있다.
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
	case (short)PACKET_ID::PCK_MAKE_CHATTING_ROOM_RES:
		AddChattingRoom(stream);
		break;
		// 방에대한처리
	case (short)PACKET_ID::PCK_UPDATE_CHATTING_DATA_RES:
		UpdateChattingRoom(stream);
	default:
		break;
	}

	return false;
}

void ClientMainScene::CreateUI()
{
	// 초기화
	for (auto iter : m_User->GetFriendList())
	{
		std::string nickname = iter->GetUserNick();
		std::string id = iter->GetUserID();
		m_IDtoNick[id] = nickname;
	}

	for (int i = 0; i < m_User->GetChattingRoomList().size(); i++)
	{
		m_RoomListIdxtoRoomID[i] = m_User->GetChattingRoomList().at(i)->GetRoomID();
		m_RoomIDtoRoomListIdx[m_User->GetChattingRoomList().at(i)->GetRoomID()] = i;
	}

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

	m_pFriendSearchtBox->events().text_changed([&]() {SetSearchtextInFriendList(m_pFriendSearchtBox, m_pFriendListBox); });

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

	m_pSearchChatRoom->events().text_changed([&]()
	{
		SetSearchtextInChattingList(m_pSearchChatRoom, m_pChatRoomList);
	});

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
			chatroomname.clear();
		}
		else
		{
			m_pChatRoomList->at(0).push_back(charset(iter->GetRoomName()).to_bytes(unicode::utf8));
			chatroomname.clear();
		}
	}

	auto a = m_pChatRoomList->events().dbl_click([&](const nana::arg_mouse& arg)
	{
		for (auto iter : m_pChatRoomList->selected())
		{
			std::vector<std::string> vs;
			int chatroomid = m_RoomListIdxtoRoomID[iter.item];
			CreateChattingRoom(vs, chatroomid, true);
			// 이제 해야하는게 이거 제대로 적용됐는지 볼려면 채팅시스템만들어야함
		}
	});
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

	m_pFriendListBox->events().dbl_click([&]()
	{
		std::vector<std::string> vs;
		for (auto iter : m_pFriendListBox->selected())
		{
			int idx = iter.item;
			vs.push_back(m_User->GetFriendList()[idx]->GetUserID());
			CreateChattingRoom(vs);
			// 친구 아이디 누르면 해당 인덱스의 유저아이디를 통해서 채팅방을열도록함.
		}
	});

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

void ClientMainScene::SetSearchtextInFriendList(nana::textbox* tb, nana::listbox* lb)
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

void ClientMainScene::SetSearchtextInChattingList(nana::textbox * tb, nana::listbox * lb)
{
	std::string str;
	std::wstring wstr = tb->caption_wstring();
	std::list<std::string> searchlist;

	if (convert_unicode_to_ansi_string(str, wstr.c_str(), wstr.size()) != 0)
	{
		str.clear();
	}

	for (auto iter : m_User->GetChattingRoomList())
	{
		std::string RoomName = iter->GetRoomName();
		if (RoomName.empty())
		{
			std::string caption;
			for (auto iter2 : iter->GetJoinnedUsers())
			{
				caption.append(iter2);
				caption.push_back(',');
			}
			caption.pop_back();

			if (!str.empty())
			{
				if (caption.find(str) != std::string::npos)
				{
					searchlist.push_back(caption);
				}
			}
			else
				searchlist.clear();
		}
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
			for (auto iter : m_User->GetChattingRoomList())
			{
				std::string caption;
				for (auto iter2 : iter->GetJoinnedUsers())
				{
					caption.append(iter2);
					caption.push_back(',');
				}
				caption.pop_back();
				lb->at(0).push_back(charset(caption).to_bytes(unicode::utf8));
				caption.clear();
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
	std::wstring wtext = m_pfindIDBox->caption_wstring();
	convert_unicode_to_ansi_string(sendtext, wtext.c_str(), wtext.size());
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
	std::wstring wtext = m_pfindIDBox->caption_wstring();
	convert_unicode_to_ansi_string(convertedText, wtext.c_str(), wtext.size());
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
		stream >> &foundID;
		if (foundID == m_User->GetUserID())
		{
			// 자기자신은 친구로할수없음
			form _fr = form(API::make_center(200, 150), nana::appear::decorate<appear::taskbar, appear::sizable>());
			_fr.bgcolor(nana::colors::light_yellow);
			label* _lb = new label(_fr, charset("자기 자신은 추가할 수 없습니다").to_bytes(nana::unicode::utf8));

			button* _ok = new button(_fr, charset("확인").to_bytes(unicode::utf8));
			_ok->bgcolor(nana::colors::antique_white);
			_ok->events().click([&]()
			{
				_fr.close();
			});
			place _plc{ _fr };
			_plc.div("vert maring = [20,10,20]< ><<weight = 10 >lb<weight = 10>><><weight = 30% gap = 10 btn>");
			_plc["lb"] << *_lb;
			_plc["btn"] << *_ok;
			_plc.collocate();

			_fr.show();
			exec();
			return;
		}

		// 창하나 띄워서 찾은 아이디 텍스트로 띄워주고 친구추가 할거냐고 물어보기
		form _fr = form(API::make_center(200, 150), nana::appear::decorate<appear::taskbar, appear::sizable>());
		_fr.bgcolor(nana::colors::light_yellow);
		label* _lb = new label(_fr);
		button* _ok = new button(_fr, charset("친구 추가").to_bytes(unicode::utf8));
		_ok->bgcolor(nana::colors::antique_white);
		button* _no = new button(_fr, charset("닫기").to_bytes(unicode::utf8));
		_no->bgcolor(nana::colors::antique_white);

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

void ClientMainScene::AddChattingRoom(Stream& stream)
{
	// 채팅방 친구 목록 받아오니 이걸로 ui에 추가
	int numrows = -1;
	stream >> &numrows;
	std::string nicks;
	nicks.append(m_User->GetUserNick());
	nicks.push_back(',');
	for (int i = 0; i < numrows - 1; i++)
	{
		std::string id;
		stream >> &id;

		nicks.append(m_IDtoNick[id]);
		nicks.push_back(',');
	}
	nicks.pop_back();
	m_pChatRoomList->at(0).push_back(charset(nicks).to_bytes(nana::unicode::utf8));
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
	std::vector<std::string> _vm;
	for (auto iter : m_User->GetFriendList())
	{
		_lb->at(0).push_back(charset(iter->GetUserNick()).to_bytes(unicode::utf8));
		_vm.push_back(iter->GetUserID());
	}

	_tb->events().text_changed([&]()
	{
		SetSearchtextInFriendList(_tb, _lb);
	});

	_ok->events().click([&]()
	{
		auto checkedbox = _lb->checked();
		std::vector<std::string> ids;
		for (auto iter : checkedbox)
		{
			auto index = iter.item;
			std::string id = _vm[index];
			ids.push_back(id);
		}
		CreateChattingRoom(ids);
		_form.close();
	});

	_close->events().click([&]() {_form.close(); });

	_plc.collocate();
	_form.show();
	exec();
}

void ClientMainScene::CreateChattingRoom(const std::vector<std::string>& ids, int ChatRoomID, bool isCreatedRoom)
{
	// 채팅방 만들기 , 기존 채팅방 ..
	ChatRoomUI ui;
	form* _fr = new nana::form(API::make_center(250, 400), nana::appear::decorate<appear::taskbar, appear::sizable>());
	listbox* _chattings = new nana::listbox(*_fr);

	_chattings->fgcolor(nana::colors::black);

	textbox* _Input = new nana::textbox(*_fr);

	_chattings->bgcolor(nana::colors::antique_white);
	_chattings->append_header("");
	_chattings->show_header(false);

	// 리스트박스 항목들의 가로폭을 조정함 (최대 300)

	_Input->multi_lines(false);
	_Input->bgcolor(nana::colors::white);
	// 라인을 벗어나면 다음 라인으로 이동함.
	_Input->line_wrapped(true);
	_Input->multi_lines(false);

	button* _sendbtn = new nana::button(*_fr, charset("전송").to_bytes(unicode::utf8));
	_fr->bgcolor(nana::colors::light_yellow);

	if (isCreatedRoom)
	{
		// 여기서 chatroonid는 룸id가 아니라 유저가 가지고 있는 채팅룸의 인덱스임
		// 채팅룸 인덱스를 진짜번호로 바꿔야함. db상 조인했을때 나타나는순서대로 기입될것

		if (!m_User->GetChattingRoomList()[m_RoomIDtoRoomListIdx[ChatRoomID]]
			->GetChattingDataList().empty())
		{
			for (auto iter : m_User->GetChattingRoomList()[m_RoomIDtoRoomListIdx[ChatRoomID]]->GetChattingDataList())
			{
				// Userid + data + 날짜
				std::string id = "보낸 사람 : ";
				id.append(iter.m_Nickname);
				_chattings->at(0).push_back(charset(id).to_bytes(nana::unicode::utf8));
				std::string contentstime = "[";
				if (iter.m_Contents.length() > 25)
				{
					int count = iter.m_Contents.length() / 25;
					for (int i = 0; i < count; i++)
					{
						char copydata[25] = { 0, };
						memcpy(copydata, iter.m_Contents.c_str(), 25 - 1);
						contentstime.append(copydata);
						if (i != count - 1)
						{
							_chattings->at(0).push_back(charset(contentstime).to_bytes(nana::unicode::utf8));
							contentstime.clear();
						}
						else
						{
							contentstime.append("]  ");
							contentstime += iter.m_Senddate;
						}
					}
				}
				else
				{
					contentstime.append(iter.m_Contents);
					contentstime += "]  ";
					contentstime += iter.m_Senddate;
				}

				_chattings->at(0).push_back(charset(contentstime).to_bytes(nana::unicode::utf8));
			}
		}
	}
	else
	{
		// 서버에 채팅방 만들어달라고 요청하고 룸넘버 받아야함.
		short pkid = static_cast<short>(PACKET_ID::PCK_MAKE_CHATTING_ROOM_REQ);
		Packet SendPacket;
		SendPacket.stream = new Stream();
		*SendPacket.stream << pkid;
		*SendPacket.stream << ids.size();
		for (auto iter : ids)
		{
			*SendPacket.stream << iter;
		}
		std::string myID = m_User->GetUserID();
		*SendPacket.stream << myID;
		m_tcpNetwork->SendPacket(SendPacket);
	}

	_Input->events().key_char([&](const nana::arg_keyboard& _arg)
	{
		if (_arg.key == keyboard::enter)
		{
			// send
			SendChatData(_Input, _chattings, ChatRoomID);
			_chattings->scroll(true);
		};
	});
	_sendbtn->bgcolor(nana::colors::antique_white);
	_sendbtn->events().click([&]()
	{
		SendChatData(_Input, _chattings, ChatRoomID);
		_chattings->scroll(true);
	});

	_chattings->column_at(0).width(300);
	_chattings->column_at(0).fit_content(300);
	_chattings->auto_draw(true);

	std::unique_ptr<place> _plc = std::make_unique<place>(*_fr);
	_plc->div("vert <><weight=10%><weight =60% chat><weight =30% gap = 10 arrange = [80%,20%] input>");
	(*_plc)["chat"] << *_chattings;
	(*_plc)["input"] << *_Input << *_sendbtn;

	ui.uiform = _fr;
	ui.chatlist = _chattings;
	ui.input = _Input;
	ui.sendbtn = _sendbtn;
	ui.roomkey = ChatRoomID;

	m_pChattingRooms.push_back(&ui);

	_plc->collocate();
	_fr->show();
	exec();
}

void ClientMainScene::SendChatData(textbox* _Input, listbox* _chattings, const int ChatRoomID)
{
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
		std::string _id = "보낸 사람 : ";
		_id.append("나");
		_chattings->at(0).push_back(charset(_id).to_bytes(nana::unicode::utf8));
		std::string contentstime = "[";
		if (contents.length() > 25)
		{
			int count = contents.length() / 25;
			for (int i = 0; i < count; i++)
			{
				char copydata[25] = { 0, };
				memcpy(copydata, contents.c_str(), 25 - 1);
				contentstime.append(copydata);
				if (i != count - 1)
				{
					_chattings->at(0).push_back(charset(contentstime).to_bytes(nana::unicode::utf8));
					contentstime.clear();
				}
				else
				{
					contentstime.append("]  ");
					contentstime += date;
				}
			}
		}
		else
		{
			contentstime.append(contents);
			contentstime += "]  ";
			contentstime += date;
		}

		_chattings->at(0).push_back(charset(contentstime).to_bytes(nana::unicode::utf8));
		_Input->reset();
	}
}

void ClientMainScene::UpdateChattingRoom(Stream & stream)
{
	int roomid;
	std::string senddate;
	std::string id;
	std::string nick;
	std::string contents;

	stream >> &roomid;
	stream >> &senddate;
	stream >> &id;
	stream >> &nick;
	stream >> &contents;

	int idx = m_RoomIDtoRoomListIdx[roomid];

	// 기존의 채팅방 번호를 알아야함.
	for (int i = 0; i < m_pChattingRooms.size(); i++)
	{
		if (m_pChattingRooms[i]->roomkey == roomid)
		{
			UpdateReadData(m_pChattingRooms[i]->input, m_pChattingRooms[i]->chatlist, roomid, senddate, id
				, nick, contents);
		}
	}
}

void ClientMainScene::UpdateReadData(nana::textbox* _Input, nana::listbox* _chattings, const int ChatRoomID,
	const std::string senddate, const std::string id, const std::string nick, const std::string contents)
{
	int RoomID = ChatRoomID;

	std::string _id = "보낸 사람 : ";
	_id.append(id);
	_chattings->at(0).push_back(charset(_id).to_bytes(nana::unicode::utf8));
	std::string contentstime = "[";
	if (contents.length() > 25)
	{
		int count = contents.length() / 25;
		for (int i = 0; i < count; i++)
		{
			char copydata[25] = { 0, };
			memcpy(copydata, contents.c_str(), 25 - 1);
			contentstime.append(copydata);
			if (i != count - 1)
			{
				_chattings->at(0).push_back(charset(contentstime).to_bytes(nana::unicode::utf8));
				contentstime.clear();
			}
			else
			{
				contentstime.append("]  ");
				contentstime += senddate;
			}
		}
	}
	else
	{
		contentstime.append(contents);
		contentstime += "]  ";
		contentstime += senddate;
	}

	_chattings->at(0).push_back(charset(contentstime).to_bytes(nana::unicode::utf8));
	// TODO : 이건 개발자 성향에 따라서 메시지 받으면 바로 스크롤내릴지 결정하길
	_chattings->scroll(true);
	_Input->reset();
}