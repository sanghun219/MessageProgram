#pragma once
#include "IClientScene.h"

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
};