#pragma once
#include "ServLibrary.h"
class Engine
{
protected:
	static Engine* m_pInst;

public:
	static Engine* GetInst()
	{
		if (m_pInst == nullptr)
			m_pInst = new Engine();
		return m_pInst;
	}
protected:
	Engine() {}
public:
	virtual ~Engine() {};
};

Engine* Engine::m_pInst = nullptr;