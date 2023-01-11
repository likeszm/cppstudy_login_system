/*
* 名称: 登录系统（C++学习记录）
* 
* 实现功能:
* 1、有登录和注册功能，登录时可以判断账号是否存在，密码是否正确
* 2、账号和密码存储在excel表格内，注册时将会新增条目进去(由于使用了licxl库,需要在程序运行目录下复制libxl.dll)
* 3、每次运行都会记录txt格式的日志(但是需要自行在程序目录下新建log.txt)
* 
* 其他:
*   第一次用面向对象的思想去写一个小小小项目，问题蛮多的，不仅代码的逻辑有些混乱，有的地方表意也很模糊
* 但是也是第一次使用第三方库，写出能够记录运行状态的程序，相当于我的程序多了时间轴这一个维度。
* 我觉得这一点是意义重大的。
*/

#include <iostream>
#include "libxl.h"
#include <fstream>
#include <ctime>

//登录状态枚举
enum class login_state_type
{
    NOT_LOGIN_IN = 0,
    LOGIN_IN,
    PSWD_ERROR,
    RECOVER_PSWD
};

//登录界面类,存储登录状态,输入的账号和密码; 登录方法、注册方法、欢迎界面
class Login 
{
public:
    Login(void);
    ~Login(void);

    int start(void);
    bool login_in(std::wstring name,std::wstring pswd);
    bool regist(std::wstring name, std::wstring pswd);

    bool login_flag;

private:
    login_state_type login_state;
    std::wstring user_name;
    std::wstring password;
    int first_row, first_col, last_row, last_col;
    libxl::Book* p_book;
    libxl::Sheet* p_sheet;
    std::wofstream* clog;

    bool user_is_exist(std::wstring name);
    bool pswd_checking(std::wstring name, std::wstring pswd);
    bool add_account(std::wstring name, std::wstring pswd);
    bool load_account(void);
    bool load_log(void);
};

void test_libxl(void);

int main()
{
    setlocale(LC_ALL, "chs");
    //test_libxl();

    Login user;
    int flag = 0;
    std::wstring name,pswd,temp;

    while (1)
    {
        flag = user.start();

        switch (flag)
        {
        case 1:
            std::cout << "账号:";
            std::wcin >> name;
            std::cout << "密码:";
            std::wcin >> pswd;
            user.login_in(name, pswd);
            break;
        case 2:
            std::cout << "账号:";
            std::wcin >> name;
            std::cout << "密码:";
            std::wcin >> pswd;
            user.regist(name, pswd);
            break;
        case 3:
            std::cout << "再见!" << std::endl;
            return 0;
        default:
            break;
        }

    }

    return 0;
}

Login::Login(void) : login_state(login_state_type::NOT_LOGIN_IN), user_name(L"NULL"), password(L"-1"), first_row(0), first_col(0), last_row(0), last_col(0), p_book(NULL), p_sheet(NULL), login_flag(false), clog(NULL)
{                                                                                                                                                       
    if (!load_account())
    {
        std::cout << "链接服务器失败!" << std::endl;
        exit(1);
    }

    if (!load_log())
    {
        std::cout << "WARRING:日志记录器启动失败!" << std::endl;
    }
}

Login::~Login(void)
{
    clog->close();
    delete clog;
    p_book->release();
}

int Login::start(void)
{
    std::wstring cmd;

    std::cout << "欢迎,";
    switch (login_state)
    {
        case login_state_type::PSWD_ERROR:
        case login_state_type::RECOVER_PSWD:
            std::cout << "错误！未定义的事件！" << std::endl; 
            exit(1); 
            break;
        case login_state_type::NOT_LOGIN_IN:
            std::cout << "您尚未登录，请登录（1）或 注册（2）" << std::endl;
            cmd_table:
            std::cout << "命令: ";
            std::wcin >> cmd;
            if (cmd == L"1") return 1;
            if (cmd == L"2") return 2;
            std::wcout << L"抱歉,\"" << cmd << L"\"是无法识别的指令，请重新输入!" << std::endl;
            goto cmd_table;

            break;
        case login_state_type::LOGIN_IN:
            std::wcout << L"尊敬的用户: " << user_name << L",您好！" << std::endl;
            return 3;
            break;
        default:
            break;
            return 0;
    }


    
}

bool Login::login_in(std::wstring name, std::wstring pswd)
{
    user_name = name;
    password = pswd;

    if (this->user_is_exist(name))
    {
        if (this->pswd_checking(name, pswd))
        {
            std::cout << "登录成功!" << std::endl;
            login_state = login_state_type::LOGIN_IN;
            login_flag = true;
            time_t now;
            time(&now);
            char buffer[30];
            ctime_s(buffer, 30, &now);
            *clog << "LOG:" << buffer << "-> " << "登录成功:" << name.c_str() << "\n\n";
            return true;
        }
        else {
            std::cout << "密码错误!请重新登录!" << std::endl;
            time_t now;
            time(&now);
            char buffer[30];
            ctime_s(buffer, 30, &now);
            *clog << "LOG:" << buffer << "-> " << "密码错误:" << name.c_str() << "\n\n";
            return false;
        }
    }
    else {
        std::cout << "用户不存在!请重新登录!" << std::endl;
        time_t now;
        time(&now);
        char buffer[30];
        ctime_s(buffer, 30, &now);
        *clog << "LOG:" << buffer << "-> " << "用户不存在:" << name.c_str() << "\n\n";
        return false;
    }
}

bool Login::regist(std::wstring name, std::wstring pswd)
{
    user_name = name;
    password = pswd;

    if (this->user_is_exist(name)) {
        std::cout << "用户已存在!注册失败!" << std::endl;
        return false;
    }
    else {
        if (add_account(name, pswd))
        {
            std::cout << "注册成功!" << std::endl;
            time_t now;
            time(&now);
            char buffer[30];
            ctime_s(buffer, 30, &now);
            *clog << "LOG:" << buffer << "-> " << "注册成功:" << name.c_str() << "\n\n";
            return true;
        }
        else {
            std::cout << "注册失败!请再次尝试!" << std::endl;
            return false;
        }

    }

}

bool Login::user_is_exist(std::wstring name)
{
    for (int i = first_row + 1; i <= last_row; i++)
    {
        if (p_sheet->cellType(i, first_col) == libxl::CELLTYPE_STRING)
        {
            if ( 0 == name.compare(p_sheet->readStr(i, first_col)) )
                return true;
        }
    }
    return false;
}

bool Login::pswd_checking(std::wstring name, std::wstring pswd)
{
    int i = 0;

    for (i = first_row + 1; i <= last_row; i++)
    {
        if (p_sheet->cellType(i, first_col) == libxl::CELLTYPE_STRING)
        {
            if ( 0 == name.compare(p_sheet->readStr(i, first_col)))
            {
                goto pswd_check;
            }
        }
    }

    pswd_check:

    if (p_sheet->cellType(i, last_col) == libxl::CELLTYPE_STRING)
    {
        if ( 0 == pswd.compare(p_sheet->readStr(i, last_col)))
        {
            return true;
        }
    }

    return false;
}

bool Login::add_account(std::wstring name, std::wstring pswd)
{
    if (p_sheet->writeStr(++last_row, first_col, name.c_str()))
    {
        if (p_sheet->writeStr(last_row, last_col, pswd.c_str()))
        {
            p_book->save(L"account.xlsx");
            return true;
        }
    }

    return false;
}

bool Login::load_account(void)
{
    libxl::Book* account_book = xlCreateBook();  //创建工作簿对象

    if (!account_book)
    {
        std::cerr << "ERROR:工作簿缓存创建失败" << std::endl;
        exit(1);
    }

    if (!account_book->load(L"account.xlsx"))
    {
        std::cout << "未检测到工作簿,正在创建---" << std::endl;

        libxl::Sheet* temp_sheet = account_book->addSheet(L"sheet_1");
        temp_sheet->writeStr(1, 1, L"name:");
        temp_sheet->writeStr(1, 2, L"password:");
        temp_sheet->writeStr(2, 1, L"admin");
        temp_sheet->writeStr(2, 2, L"root");

        account_book->save(L"account.xlsx");
       
        if (!account_book->load(L"account.xlsx"))
        {
            std::cerr << "ERROR:工作簿载入失败" << std::endl;
            std::cerr << account_book->errorMessage();
            exit(1);
        }
    }

    libxl::Sheet* account_sheet = account_book->getSheet(0); //获取sheet1

    if (!account_sheet)
    {
        std::cerr << "ERROR:工作表载入失败" << std::endl;
        exit(1);
    }

    first_col = account_sheet->firstCol();
    first_row = account_sheet->firstRow();
    last_col = account_sheet->lastCol();
    last_row = account_sheet->lastRow();

    last_col--; //最后一列没数据
    last_row--; //最后一行没数据

    if ((first_col == last_col) || (first_row == last_row))
    {
        std::cerr << "ERROR:工作表无数据!" << std::endl;
        exit(1);
    }

    std::wstring buffer;
    if (account_sheet->cellType(first_row, first_col) == libxl::CELLTYPE_STRING)
    {
        buffer = account_sheet->readStr(first_row, first_col);
        if (buffer.find(L"name") == -1)
        {
            std::cerr << "ERROR:工作表无name数据!" << std::endl;
            exit(1);
        }
    }
    if (account_sheet->cellType(first_row, last_col) == libxl::CELLTYPE_STRING)
    {
        buffer = account_sheet->readStr(first_row, last_col);
        if (buffer.find(L"password") == -1)
        {
            std::cerr << "ERROR:工作表无password数据!" << std::endl;
            exit(1);
        }
    }

    p_book = account_book;
    p_sheet = account_sheet;
    return true;
}

bool Login::load_log(void)
{
    this->clog = new std::wofstream;

    if (clog)
    {
        clog->open("log.txt",std::ios::out|std::ios::app);
        return true;
    }

    return false;

}



void test_libxl(void)
{

    libxl::Book* test_book = xlCreateBook();
    std::wcout << "p = " << test_book << std::endl;
    
    //if (test_book->setLocale("UTF-8"))
    //{
    //    std::cout << "编码格式修改成功!" << std::endl;
    //}
    //else {
    //    std::cout << "编码格式修改失败!" << std::endl;
    //    exit(1);
    //}


    if (test_book->load(L"C:\\Users\\likes\\Desktop\\测试文件.xlsx"))
    {
        std::cout << "测试文件载入成功!" << std::endl;
    }
    else {
        std::cout << "文件载入失败，将自动创建" << std::endl;
    }

    //libxl::Sheet* sheet_1 = test_book->addSheet(L"测试1");
    //std::cout << "表格数量: " << test_book->sheetCount() << std::endl;

    //libxl::Sheet* sheet_2 = test_book->addSheet(L"测试2");
    //std::cout << "表格数量: " << test_book->sheetCount() << std::endl;

    //libxl::Sheet* sheet_3 = test_book->addSheet(L"测试3");
    //std::cout << "表格数量: " << test_book->sheetCount() << std::endl;

    //test_book->delSheet(1);     //删除第二个
    //std::cout << "表格数量: " << test_book->sheetCount() << std::endl;

    libxl::Sheet* test_sheet = NULL;

    if (!(test_book->sheetCount()))
    {
        std::cout << "未检测到sheet,将自动创建!" << std::endl;
        test_sheet = test_book->insertSheet(0, L"测试插入");
        if (test_sheet)
        {
            std::cout << "sheet 创建成功!" << std::endl;
        }
        else {
            std::cout << "sheet 创建失败!" << std::endl;
            exit(1);
        }
    }
    else {
        std::cout << "检测到sheet个数为: " << test_book->sheetCount() << " 将自动读取第一个sheet!" << std::endl;
        test_sheet = test_book->getSheet(0);
        if (!test_sheet)
        {
            std::cout << "读取第一个sheet失败!" << std::endl;
            exit(1);
        }
        else {
            std::cout << "读取第一个sheet成功!" << std::endl;
        }
    }

    std::wstring buffer;
    
    int row = 0, col = 0;
    libxl::CellType temp_cell_type = test_sheet->cellType(row, col);
    if (temp_cell_type == libxl::CELLTYPE_EMPTY)
    {
        std::cout << "(" << row << "," << col << ")" 
            << "无内容,尝试填充"
            << std::endl;
        if (test_sheet->writeNum(row, col, 1112))
        {
            std::cout << "内容填充成功" << std::endl;
        }
        else {
            std::cout << "内容填充失败!" << std::endl;
            exit(2);
        }

    }
    else {
       
        std::cout << "存在内容,准备读取:" << std::endl;
        switch (temp_cell_type)
        {
        case libxl::CELLTYPE_EMPTY:
            break;
        case libxl::CELLTYPE_NUMBER:
            std::cout << "数字类型,具体值为:" << test_sheet->readNum(row, col) << std::endl;
            break;
        case libxl::CELLTYPE_STRING:
             ;
             std::cout << "字符类型,具体值为:"; 
             std::wcout << (test_sheet->readStr(row, col)) << std::endl;

             buffer = (test_sheet->readStr(row, col));
             if (buffer == L"汉字" )
             {
                 std::cout << "字符串对比,测试成功!";
                 test_sheet->writeStr(row, col, L"测试成功!");
             }
            break;
        case libxl::CELLTYPE_BOOLEAN:
            libxl::Format* temp_format;
            std::cout << "布尔类型,具体值为:" << test_sheet->readBool(row, col) << std::endl;
            break;
        case libxl::CELLTYPE_BLANK:
            std::cout << "空格类型,具体值为:" << test_sheet->readBlank(row, col, &temp_format) << std::endl;
            break;
        case libxl::CELLTYPE_ERROR:
            std::cout << "读取错误!错误信息:" << test_sheet->readError(row, col) <<  std::endl;
            exit(3);
            break;
        default:
            break;
        }
    }


    test_book->save(L"C:\\Users\\likes\\Desktop\\测试文件.xlsx");

    test_book->release();

    //   test_book->errorMessage();
}

