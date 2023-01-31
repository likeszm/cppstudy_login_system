#include "main.h"

UI_t::UI_t(): pic_route(L"head.jpg"), text_height(UI_SIZE / 20), text_width(UI_SIZE / 40), user(L""), pswd(L"")
{
	//建立窗口
	initgraph(UI_SIZE, UI_SIZE);

	//修改背景色
	setbkcolor(WHITE);
	cleardevice();

	//修改字体 大小 和 字体种类
	LOGFONT text_style;
	gettextstyle(&text_style);
	text_style.lfHeight = text_height;
	text_style.lfWidth = text_width;
	_tcscpy_s(text_style.lfFaceName, _T("黑体"));
	//text_style.lfQuality = ANTIALIASED_QUALITY;		//抗锯齿
	//设置字体颜色为黑色
	settextstyle(&text_style);
	settextcolor(BLACK);

	//画线色设置为黑色
	setlinecolor(BLACK);

	//输入框位置初始化
	user_rect = { UI_SIZE / 3, UI_SIZE * 4 / 6 - UI_SIZE / 100, (int)(UI_SIZE * 4.75 / 6), UI_SIZE * 4 / 6 + text_height + UI_SIZE / 100 };
	pswd_rect = { user_rect.left ,(int)(UI_SIZE * 4.75 / 6) - UI_SIZE / 100, user_rect.right, (int)(UI_SIZE * 4.75 / 6 + text_height) + UI_SIZE / 100 };


}

UI_t::~UI_t()
{
	//关闭窗口
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
	//多提供一个接口输入账号密码
	if (! User.empty())
	{
		this->user = User;
	} 
	if (!Pswd.empty())
	{
		this->pswd = Pswd;
	}

	//用户名直接显示
	outtextxy(user_rect.left + UI_SIZE / 80, UI_SIZE * 4 / 6, user.c_str());
	
	//密码需要隐藏
	std::wstring temp;
	for (int i = 0; i < pswd.length(); i++)
	{
		temp += L"*";
	}
	outtextxy(pswd_rect.left + UI_SIZE / 80, (int)(UI_SIZE * 4.78 / 6), temp.c_str());

}

void UI_t::login_init()
{
	//显示头像
	IMAGE head_pic;
	loadimage(&head_pic, pic_route.c_str(), UI_SIZE / 3, UI_SIZE / 3);
	putimage(UI_SIZE / 3, UI_SIZE / 6, &head_pic);

	//显示账户密码文字
	outtextxy(UI_SIZE / 8, UI_SIZE * 4 / 6, _T("用户名: "));
	outtextxy(UI_SIZE / 8, UI_SIZE * 4.75 / 6, _T("密  码: "));

	//显示账户密码输入框
	fillrectangle(user_rect.left, user_rect.top, user_rect.right, user_rect.bottom);
	fillrectangle(pswd_rect.left, pswd_rect.top, pswd_rect.right, pswd_rect.bottom);

	//登录提示文字
	outtextxy(UI_SIZE / 3.25, UI_SIZE * 5.5 / 6, _T("（点击头像进行登录）"));
}

/*
*返回值		消息
* 10		登录请求
* 11		点击了账号输入框
* 12		点击了密码输入框
* 
* 
*/
int UI_t::get_input(void)
{
	ExMessage msg;
	flushmessage();		//先清除消息
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
			//检测到点击了账号输入框
			return 11;
		}

		if (msg.x > pswd_rect.left
			&& msg.x < pswd_rect.right
			&& msg.y > pswd_rect.top
			&& msg.y < pswd_rect.bottom)
		{
			//检测到点击了密码输入框
			return 12;
		}
	}
}
