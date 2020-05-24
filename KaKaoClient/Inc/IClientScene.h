#pragma once
#include "ServLibrary.h"
#include "Stream.h"
#include "TCPNetwork.h"
#include "Stream.h"
#include "User.h"
#include "nanaUtil.h"
#include <regex>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/tabbar.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui.hpp>
using namespace nana;

enum class CLIENT_SCENE_TYPE
{
	LOGIN,
	MAKEID,
	FRIEND_LIST,
	CHATTING_ROOM_LIST,
	CHATTING_ROOM,
	END,
};
class User;
class IClientScene
{
public:
	virtual ~IClientScene() {}

	virtual void Update() {}
	virtual bool ProcessPacket(PACKET_ID pkID, Stream& stream) { return false; }
	virtual void Show() { m_pform->show(); exec(); }
	virtual void CreateUI() = 0;
	virtual void SetUserInfo(User* user) { this->m_User = user; }
	virtual void SetNetwork(TCPNetwork* tcpNetwork) { this->m_tcpNetwork = tcpNetwork; }
	static void UnicodeToAnsi(const wchar_t* pszText, const int destSize, char* pszDest)
	{
		_snprintf_s(pszDest, destSize, _TRUNCATE, "%ws", pszText);
	}
	static CLIENT_SCENE_TYPE GetCurSceenType() { return m_SceneType; }
	static void SetCurSceenType(const CLIENT_SCENE_TYPE sceneType) { m_SceneType = sceneType; }
protected:
	static CLIENT_SCENE_TYPE m_SceneType;
	TCPNetwork* m_tcpNetwork;
	User* m_User;
	nana::form* m_pform;
};
