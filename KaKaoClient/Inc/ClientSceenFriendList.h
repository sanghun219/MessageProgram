#pragma once
#include "IClientScene.h"
#include <nana/gui/widgets/tabbar.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/group.hpp>
class ClientSceenFriendList : public IClientScene
{
public:
	virtual void Update()override;
	virtual bool ProcessPacket(PACKET_ID pkID, Stream& stream)override;

public:

	virtual void CreateUI()override;

private:

	nana::textbox* m_pSearchtBox;
	nana::label* m_pTitlelb;
	nana::button* m_pChattroomListBtn;
	nana::listbox* m_pFriendListBox;
	nana::tabbar<std::string>* m_pTabbar;
	nana::group m_pfrgroup;
	nana::group m_pchgroup;
};