/*
* ��¼����2.0
*	��ѧϰ��EaseXͼ�ο⣬�Լ���������˽�����������˼��󣬱����ع��������Ŀ
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

	log.info("�����ı�");


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
				log.info("��¼�ɹ�!");
			}

			break;
		case 11:
			if (InputBox(s, 20, L"�������˺�:", L"�˺�", L"None", 200, 100, false))
			{
				std::wstring temp = s;
				ui.login_update(temp);
			}
			break;
		case 12:
			if (InputBox(s, 20, L"����������:", L"����", L"None", 200, 100, false))
			{
				std::wstring temp = s;
				ui.login_update(L"",temp);
			}
			break;
		default:
			break;
		}

	}
	

//	std::cout << "query return value = " << account.query(L"ac!23", L"����--") << std::endl;

//	std::cout << "remove return value = " << account.remove(L"ac!23", L"����--") << std::endl;

//	std::cout << "modify return value = " << account.modify(L"1111", L"����--", L"������!!") << std::endl;

	std::wcout << "�������" << std::endl;

	return 0;
}

