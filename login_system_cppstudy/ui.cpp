#include "main.h"

UI_t::UI_t(): pic_route(L"head.jpg"), text_height(UI_SIZE / 20), text_width(UI_SIZE / 40), user(L""), pswd(L"")
{
	//��������
	initgraph(UI_SIZE, UI_SIZE);

	//�޸ı���ɫ
	setbkcolor(WHITE);
	cleardevice();

	//�޸����� ��С �� ��������
	LOGFONT text_style;
	gettextstyle(&text_style);
	text_style.lfHeight = text_height;
	text_style.lfWidth = text_width;
	_tcscpy_s(text_style.lfFaceName, _T("����"));
	//text_style.lfQuality = ANTIALIASED_QUALITY;		//�����
	//����������ɫΪ��ɫ
	settextstyle(&text_style);
	settextcolor(BLACK);

	//����ɫ����Ϊ��ɫ
	setlinecolor(BLACK);

	//�����λ�ó�ʼ��
	user_rect = { UI_SIZE / 3, UI_SIZE * 4 / 6 - UI_SIZE / 100, (int)(UI_SIZE * 4.75 / 6), UI_SIZE * 4 / 6 + text_height + UI_SIZE / 100 };
	pswd_rect = { user_rect.left ,(int)(UI_SIZE * 4.75 / 6) - UI_SIZE / 100, user_rect.right, (int)(UI_SIZE * 4.75 / 6 + text_height) + UI_SIZE / 100 };


}

UI_t::~UI_t()
{
	//�رմ���
	closegraph();
}

std::wstring UI_t::get_user(void)
{
	return this->user;
}

std::wstring UI_t::get_pswd(void)
{
	return this->pswd;
}

void UI_t::login_update(std::wstring User, std::wstring Pswd)
{
	//���ṩһ���ӿ������˺�����
	if (! User.empty())
	{
		this->user = User;
	} 
	if (!Pswd.empty())
	{
		this->pswd = Pswd;
	}

	//�û���ֱ����ʾ
	outtextxy(user_rect.left + UI_SIZE / 80, UI_SIZE * 4 / 6, user.c_str());
	
	//������Ҫ����
	std::wstring temp;
	for (int i = 0; i < pswd.length(); i++)
	{
		temp += L"*";
	}
	outtextxy(pswd_rect.left + UI_SIZE / 80, (int)(UI_SIZE * 4.78 / 6), temp.c_str());

}

void UI_t::login_init()
{
	//��ʾͷ��
	IMAGE head_pic;
	loadimage(&head_pic, pic_route.c_str(), UI_SIZE / 3, UI_SIZE / 3);
	putimage(UI_SIZE / 3, UI_SIZE / 6, &head_pic);

	//��ʾ�˻���������
	outtextxy(UI_SIZE / 8, UI_SIZE * 4 / 6, _T("�û���: "));
	outtextxy(UI_SIZE / 8, UI_SIZE * 4.75 / 6, _T("��  ��: "));

	//��ʾ�˻����������
	fillrectangle(user_rect.left, user_rect.top, user_rect.right, user_rect.bottom);
	fillrectangle(pswd_rect.left, pswd_rect.top, pswd_rect.right, pswd_rect.bottom);

	//��¼��ʾ����
	outtextxy(UI_SIZE / 3.25, UI_SIZE * 5.5 / 6, _T("�����ͷ����е�¼��"));
}

/*
*����ֵ		��Ϣ
* 10		��¼����
* 11		������˺������
* 12		��������������
* 
* 
*/
int UI_t::get_input(void)
{
	ExMessage msg;
	flushmessage();		//�������Ϣ
	getmessage(&msg);

	if (msg.x > UI_SIZE / 3 
		&& msg.x < UI_SIZE / 3 + UI_SIZE / 3
		&& msg.y > UI_SIZE / 3
		&& msg.y < UI_SIZE / 3 + UI_SIZE / 3
		)
	{
		return 10;
	}

	if (msg.lbutton)
	{
		if (msg.x > user_rect.left
			&& msg.x < user_rect.right
			&& msg.y > user_rect.top
			&& msg.y < user_rect.bottom)
		{
			//��⵽������˺������
			return 11;
		}

		if (msg.x > pswd_rect.left
			&& msg.x < pswd_rect.right
			&& msg.y > pswd_rect.top
			&& msg.y < pswd_rect.bottom)
		{
			//��⵽��������������
			return 12;
		}
	}
}
