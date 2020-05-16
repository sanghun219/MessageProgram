#pragma once
#include "TCPNetwork.h"
#include "ClientSceenLogin.h"
#include <memory.h>
// ��� GUI�� �ٷ��. ��Ȳ������ â��ȯ�� �ʿ��� ����

class MainForm
{
public:
	void Init();
	void CreateGUI();
	void Show();
	void PacketProcess();

private:
	TCPNetwork* m_TCPNetwork;
	ClientSceenLogin* m_SceenLogin;
	// unique�� ���� ������ �޸� ���� ������. �ذ�Ұ�
	std::unique_ptr<form> m_fm;
	timer m_timer;
public:
	~MainForm();
};
