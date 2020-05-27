#pragma once
#include "IClientScene.h"

#include <nana/gui/widgets/tabbar.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/panel.hpp>

struct ChatRoomUI
{
	nana::form* uiform;
	nana::listbox* chatlist;
	nana::textbox* input;
	nana::button* sendbtn;
};

class ClientMainScene : public IClientScene
{
public:
	virtual void Update()override;
	virtual bool ProcessPacket(PACKET_ID pkID, Stream& stream)override;

public:

	virtual void CreateUI()override;

	void SetSearchtext(nana::textbox* tb, nana::listbox* lb);
private:
	void FrMainFindIDBtn();
	void FrInFindIDBtn();
	void FrInCloseFindIDFormBtn();
	void FrInIDBox();
	void FrMainOpenChatUI();
	void FindResult(Stream& stream);
	void AddFriendResult(Stream& stream);
	void AddChattingRoom();

	//채팅방 UI
	void CreateChatUI();
	void CreateChattingRoom(const std::vector<std::string>& ids, int ChatRoomID = -1, bool isCreatedRoom = false);
private:
	std::unordered_map<std::string, std::string> m_IDtoNick;

	std::string m_frSearchtext;

	nana::textbox* m_pFriendSearchtBox;
	nana::label* m_pTitlelb;
	nana::button* m_pFindIDBtn;
	nana::listbox* m_pFriendListBox;
	nana::tabbar<std::string>* m_pTabbar;
	nana::panel<false>* m_pFriendpanel;
	nana::panel<false>* m_pChattingRoompanel;
	nana::form* m_pFindIDuiForm;
	nana::textbox* m_pfindIDBox;
	nana::label* m_pfrInlb;
	nana::button* m_pfrInOkBtn;
	nana::button* m_pfrInCloseBtn;
	nana::label* m_pfrInFoundIDlab;
	nana::button* m_pfrInFoundIDreqFriend;

	std::string foundID;

	// 채팅방
	nana::textbox* m_pSearchChatRoom;
	nana::listbox* m_pChatRoomList;
	nana::button* m_pMakeChattingRoomBtn;
	nana::label* m_pChatComment;

	std::vector<ChatRoomUI*> m_pChattingRooms;
};