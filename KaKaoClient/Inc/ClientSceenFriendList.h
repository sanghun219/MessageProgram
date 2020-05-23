#pragma once
#include "IClientScene.h"

#include <nana/gui/widgets/tabbar.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/panel.hpp>
class ClientSceenFriendList : public IClientScene
{
public:
	virtual void Update()override;
	virtual bool ProcessPacket(PACKET_ID pkID, Stream& stream)override;

public:

	virtual void CreateUI()override;

	void SetSearchtext();
private:
	std::string m_frSearchtext;

	nana::textbox* m_pFriendSearchtBox;
	nana::label* m_pTitlelb;
	nana::button* m_pFindIDBtn;
	nana::listbox* m_pFriendListBox;
	nana::tabbar<std::string>* m_pTabbar;
	nana::panel<false>* m_pFriendpanel;
	nana::panel<false>* m_pChattingRoompanel;
};