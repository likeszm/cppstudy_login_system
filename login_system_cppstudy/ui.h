/*
* ����������Ϊһ������
* �����Ǻͽ����߼�����ˣ��Ժ�ģ�
*
* @name		ui
* @date		2023 - 1 - 31
* @version	V2.0
* @anthor	likeszm
*/

#pragma once

#define UI_SIZE 400

#ifndef UI_SIZE		//Ĭ�ϴ�С400
#define UI_SIZE 400
#endif 


class UI_t
{
public:
	UI_t();
	~UI_t();

	std::wstring get_user(void);
	std::wstring get_pswd(void);

	void login_init();
	void login_update(std::wstring User = L"", std::wstring Pswd = L"");
	int get_input(void);

private:
	std::wstring pic_route;

	int text_width, text_height;
	RECT user_rect, pswd_rect;
	std::wstring user, pswd;
};



