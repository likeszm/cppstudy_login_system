/*
* 登录界面2.0
*	在学习了EaseX图形库，以及更深入的了解了面向对象的思想后，本人重构了这个项目
* 
* @name		main
* @date		2023-1-31
* @version	V2.0
* @anthor	likeszm
*/

#include "main.h"

int main()
{

	Log_t log;

	log.info("测试文本");


	Account_t account;

	UI_t ui;
	ui.login_init();
	while (true)
	{
		int cmd = ui.get_input();

		wchar_t s[20];

		switch (cmd)
		{
		case 10:

			if (account.query(ui.get_user(), ui.get_pswd()))
			{

			} 
			else
			{
				log.info("登录成功!");
			}

			break;
		case 11:
			if (InputBox(s, 20, L"请输入账号:", L"账号", L"None", 200, 100, false))
			{
				std::wstring temp = s;
				ui.login_update(temp);
			}
			break;
		case 12:
			if (InputBox(s, 20, L"请输入密码:", L"密码", L"None", 200, 100, false))
			{
				std::wstring temp = s;
				ui.login_update(L"",temp);
			}
			break;
		default:
			break;
		}

	}
	

//	std::cout << "query return value = " << account.query(L"ac!23", L"测试--") << std::endl;

//	std::cout << "remove return value = " << account.remove(L"ac!23", L"测试--") << std::endl;

//	std::cout << "modify return value = " << account.modify(L"1111", L"测试--", L"新密码!!") << std::endl;

	std::wcout << "测试完成" << std::endl;

	return 0;
}

