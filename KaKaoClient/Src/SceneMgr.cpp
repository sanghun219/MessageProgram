#include "SceneMgr.h"
#include "ClientMainScene.h"
#include "ClientSceenLogin.h"
IClientScene* SceneMgr::m_pClientScene = nullptr;
int SceneMgr::m_NeedUpdateCount = 0;
void SceneMgr::SetSceen(CLIENT_SCENE_TYPE sceen)
{
	if (m_pClientScene != nullptr)
	{
		delete m_pClientScene;
		m_pClientScene = nullptr;
	}

	switch (sceen)
	{
	case CLIENT_SCENE_TYPE::LOGIN:
		m_pClientScene = new ClientSceenLogin();
		break;
	case CLIENT_SCENE_TYPE::FRIEND_LIST:
		m_pClientScene = new ClientMainScene();
		break;
	default:
		break;
	}
	m_NeedUpdateCount++;
	m_pClientScene->SetCurSceenType(sceen);
}

bool SceneMgr::IsNeedUpdate()
{
	if (m_NeedUpdateCount > 0)
	{
		--m_NeedUpdateCount;
		return true;
	}
	return false;
}