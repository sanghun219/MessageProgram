#pragma once
#include "IClientScene.h"
#include "Singleton.h"
class SceneMgr
{
	friend Singleton<SceneMgr>;
public:
	void SetSceen(CLIENT_SCENE_TYPE sceen);
	inline IClientScene* GetSceen()
	{
		if (m_pClientScene == nullptr)
		{
			LOG("SetSceen이 호출되기 이전에 GetSceen이 호출되었습니다!");
			return nullptr;
		}
		return m_pClientScene;
	}
	bool IsNeedUpdate();
private:
	static IClientScene* m_pClientScene;
	static int m_NeedUpdateCount;
};