#pragma once
#include "ServLibrary.h"
#include "Stream.h"
#include "TCPNetwork.h"
#include "Stream.h"
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui.hpp>
using namespace nana;

enum class CLIENT_SCENE_TYPE
{
	LOGIN,
	FRIEND_LIST,
	CHATTING_ROOM_LIST,
	CHATTING_ROOM,
	END,
};

class IClientScene
{
public:
	virtual ~IClientScene() {}

	virtual void Update() {}
	virtual bool ProcessPacket(PACKET_ID pkID, const Stream& stream) { return false; }

	virtual void SetNetwork(TCPNetwork* tcpNetwork) { this->m_tcpNetwork = tcpNetwork; }
	static void UnicodeToAnsi(const wchar_t* pszText, const int destSize, char* pszDest)
	{
		_snprintf_s(pszDest, destSize, _TRUNCATE, "%S", pszText);
	}
	static CLIENT_SCENE_TYPE GetCurSceenType() { return m_SceneType; }
	static void SetCurSceenType(const CLIENT_SCENE_TYPE sceneType) { m_SceneType = sceneType; }
protected:
	static CLIENT_SCENE_TYPE m_SceneType;
	TCPNetwork* m_tcpNetwork;
};
