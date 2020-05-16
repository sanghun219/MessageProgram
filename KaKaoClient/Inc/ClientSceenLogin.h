#pragma once
#include "IClientScene.h"
#include "Stream.h"
class ClientSceenLogin : public IClientScene
{
public:
	virtual void Update()override;
	virtual bool ProcessPacket(PACKET_ID pkID, const Stream& stream)override;
	void CreateUI(form* pform);
	~ClientSceenLogin();

private:
	void LoginRequest();
	void SettingID(std::string id);
	void SettingPassword(std::string pass);
private:
	form* m_pform;
	std::string loginID;
	std::string LoginPass;

	button* loginBtn;
};