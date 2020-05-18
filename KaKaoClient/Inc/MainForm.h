#pragma once
#include "TCPNetwork.h"
#include "ClientSceenLogin.h"

// ��� GUI�� �ٷ��. ��Ȳ������ â��ȯ�� �ʿ��� ����
class User;
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

	User* m_User;
public:
	~MainForm();
};
