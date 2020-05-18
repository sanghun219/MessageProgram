#pragma once
#include "IClientScene.h"
#include "Stream.h"
class ClientSceenLogin : public IClientScene
{
public:
	virtual void Update()override;
	virtual bool ProcessPacket(PACKET_ID pkID, Stream& stream)override;
	void CreateUI(form* pform);
	~ClientSceenLogin();

private:
	void LoginRequest();
	void SignupRequest();
	void SignupResult(Stream & stream);
	void LoginResult(Stream & stream);
	void SettingLoginID(std::string id);
	void SettingLoginPassword(std::string pass);
	void SettingSignupID(std::string id);
	void SettingSignupNick(std::string nick);
	void SettingSignupPass(std::string pass);
	void CreateMakeidUI();
	void CreateSingUpUI();
	void CreateWrongPassUI();
	void ClearSignUpUI();
private:
	form* m_pform;
	form* m_signupform;
	std::string m_loginID;
	std::string m_LoginPass;

	std::string m_signupID;
	std::string m_signupPass;
	std::string m_signupNick;

	button* m_loginBtn;
	button* m_SignUpBtn;
	button* m_SignUpokbtn;

	textbox* m_idbox;
	textbox* m_nicknamebox;
	textbox* m_passbox;

	textbox* m_loginidbox;
	textbox* m_loginpassbox;
};