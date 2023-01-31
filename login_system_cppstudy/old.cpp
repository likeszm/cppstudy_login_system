/*
* ����: ��¼ϵͳ��C++ѧϰ��¼��
*
* ʵ�ֹ���:
* 1���е�¼��ע�Ṧ�ܣ���¼ʱ�����ж��˺��Ƿ���ڣ������Ƿ���ȷ
* 2���˺ź�����洢��excel����ڣ�ע��ʱ����������Ŀ��ȥ(����ʹ����licxl��,��Ҫ�ڳ�������Ŀ¼�¸���libxl.dll)
* 3��ÿ�����ж����¼txt��ʽ����־(������Ҫ�����ڳ���Ŀ¼���½�log.txt)
*
* ����:
*   ��һ������������˼��ȥдһ��ССС��Ŀ����������ģ�����������߼���Щ���ң��еĵط�����Ҳ��ģ��
* ����Ҳ�ǵ�һ��ʹ�õ������⣬д���ܹ���¼����״̬�ĳ����൱���ҵĳ������ʱ������һ��ά�ȡ�
* �Ҿ�����һ���������ش�ġ�
*/


#include <iostream>
#include "libxl.h"
#include <fstream>
#include <ctime>

//��¼״̬ö��
enum class login_state_type
{
    NOT_LOGIN_IN = 0,
    LOGIN_IN,
    PSWD_ERROR,
    RECOVER_PSWD
};

//��¼������,�洢��¼״̬,������˺ź�����; ��¼������ע�᷽������ӭ����
class Login
{
public:
    Login(void);
    ~Login(void);

    int start(void);
    bool login_in(std::wstring name, std::wstring pswd);
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
    std::wstring name, pswd, temp;

    while (1)
    {
        flag = user.start();

        switch (flag)
        {
        case 1:
            std::cout << "�˺�:";
            std::wcin >> name;
            std::cout << "����:";
            std::wcin >> pswd;
            user.login_in(name, pswd);
            break;
        case 2:
            std::cout << "�˺�:";
            std::wcin >> name;
            std::cout << "����:";
            std::wcin >> pswd;
            user.regist(name, pswd);
            break;
        case 3:
            std::cout << "�ټ�!" << std::endl;
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
        std::cout << "���ӷ�����ʧ��!" << std::endl;
        exit(1);
    }

    if (!load_log())
    {
        std::cout << "WARRING:��־��¼������ʧ��!" << std::endl;
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

    std::cout << "��ӭ,";
    switch (login_state)
    {
    case login_state_type::PSWD_ERROR:
    case login_state_type::RECOVER_PSWD:
        std::cout << "����δ������¼���" << std::endl;
        exit(1);
        break;
    case login_state_type::NOT_LOGIN_IN:
        std::cout << "����δ��¼�����¼��1���� ע�ᣨ2��" << std::endl;
    cmd_table:
        std::cout << "����: ";
        std::wcin >> cmd;
        if (cmd == L"1") return 1;
        if (cmd == L"2") return 2;
        std::wcout << L"��Ǹ,\"" << cmd << L"\"���޷�ʶ���ָ�����������!" << std::endl;
        goto cmd_table;

        break;
    case login_state_type::LOGIN_IN:
        std::wcout << L"�𾴵��û�: " << user_name << L",���ã�" << std::endl;
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
            std::cout << "��¼�ɹ�!" << std::endl;
            login_state = login_state_type::LOGIN_IN;
            login_flag = true;
            time_t now;
            time(&now);
            char buffer[30];
            ctime_s(buffer, 30, &now);
            *clog << "LOG:" << buffer << "-> " << "��¼�ɹ�:" << name.c_str() << "\n\n";
            return true;
        }
        else {
            std::cout << "�������!�����µ�¼!" << std::endl;
            time_t now;
            time(&now);
            char buffer[30];
            ctime_s(buffer, 30, &now);
            *clog << "LOG:" << buffer << "-> " << "�������:" << name.c_str() << "\n\n";
            return false;
        }
    }
    else {
        std::cout << "�û�������!�����µ�¼!" << std::endl;
        time_t now;
        time(&now);
        char buffer[30];
        ctime_s(buffer, 30, &now);
        *clog << "LOG:" << buffer << "-> " << "�û�������:" << name.c_str() << "\n\n";
        return false;
    }
}

bool Login::regist(std::wstring name, std::wstring pswd)
{
    user_name = name;
    password = pswd;

    if (this->user_is_exist(name)) {
        std::cout << "�û��Ѵ���!ע��ʧ��!" << std::endl;
        return false;
    }
    else {
        if (add_account(name, pswd))
        {
            std::cout << "ע��ɹ�!" << std::endl;
            time_t now;
            time(&now);
            char buffer[30];
            ctime_s(buffer, 30, &now);
            *clog << "LOG:" << buffer << "-> " << "ע��ɹ�:" << name.c_str() << "\n\n";
            return true;
        }
        else {
            std::cout << "ע��ʧ��!���ٴγ���!" << std::endl;
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
            if (0 == name.compare(p_sheet->readStr(i, first_col)))
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
            if (0 == name.compare(p_sheet->readStr(i, first_col)))
            {
                goto pswd_check;
            }
        }
    }

pswd_check:

    if (p_sheet->cellType(i, last_col) == libxl::CELLTYPE_STRING)
    {
        if (0 == pswd.compare(p_sheet->readStr(i, last_col)))
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
    libxl::Book* account_book = xlCreateBook();  //��������������

    if (!account_book)
    {
        std::cerr << "ERROR:���������洴��ʧ��" << std::endl;
        exit(1);
    }

    if (!account_book->load(L"account.xlsx"))
    {
        std::cout << "δ��⵽������,���ڴ���---" << std::endl;

        libxl::Sheet* temp_sheet = account_book->addSheet(L"sheet_1");
        temp_sheet->writeStr(1, 1, L"name:");
        temp_sheet->writeStr(1, 2, L"password:");
        temp_sheet->writeStr(2, 1, L"admin");
        temp_sheet->writeStr(2, 2, L"root");

        account_book->save(L"account.xlsx");

        if (!account_book->load(L"account.xlsx"))
        {
            std::cerr << "ERROR:����������ʧ��" << std::endl;
            std::cerr << account_book->errorMessage();
            exit(1);
        }
    }

    libxl::Sheet* account_sheet = account_book->getSheet(0); //��ȡsheet1

    if (!account_sheet)
    {
        std::cerr << "ERROR:����������ʧ��" << std::endl;
        exit(1);
    }

    first_col = account_sheet->firstCol();
    first_row = account_sheet->firstRow();
    last_col = account_sheet->lastCol();
    last_row = account_sheet->lastRow();

    last_col--; //���һ��û����
    last_row--; //���һ��û����

    if ((first_col == last_col) || (first_row == last_row))
    {
        std::cerr << "ERROR:������������!" << std::endl;
        exit(1);
    }

    std::wstring buffer;
    if (account_sheet->cellType(first_row, first_col) == libxl::CELLTYPE_STRING)
    {
        buffer = account_sheet->readStr(first_row, first_col);
        if (buffer.find(L"name") == -1)
        {
            std::cerr << "ERROR:��������name����!" << std::endl;
            exit(1);
        }
    }
    if (account_sheet->cellType(first_row, last_col) == libxl::CELLTYPE_STRING)
    {
        buffer = account_sheet->readStr(first_row, last_col);
        if (buffer.find(L"password") == -1)
        {
            std::cerr << "ERROR:��������password����!" << std::endl;
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
        clog->open("log.txt", std::ios::out | std::ios::app);
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
    //    std::cout << "�����ʽ�޸ĳɹ�!" << std::endl;
    //}
    //else {
    //    std::cout << "�����ʽ�޸�ʧ��!" << std::endl;
    //    exit(1);
    //}


    if (test_book->load(L"C:\\Users\\likes\\Desktop\\�����ļ�.xlsx"))
    {
        std::cout << "�����ļ�����ɹ�!" << std::endl;
    }
    else {
        std::cout << "�ļ�����ʧ�ܣ����Զ�����" << std::endl;
    }

    //libxl::Sheet* sheet_1 = test_book->addSheet(L"����1");
    //std::cout << "�������: " << test_book->sheetCount() << std::endl;

    //libxl::Sheet* sheet_2 = test_book->addSheet(L"����2");
    //std::cout << "�������: " << test_book->sheetCount() << std::endl;

    //libxl::Sheet* sheet_3 = test_book->addSheet(L"����3");
    //std::cout << "�������: " << test_book->sheetCount() << std::endl;

    //test_book->delSheet(1);     //ɾ���ڶ���
    //std::cout << "�������: " << test_book->sheetCount() << std::endl;

    libxl::Sheet* test_sheet = NULL;

    if (!(test_book->sheetCount()))
    {
        std::cout << "δ��⵽sheet,���Զ�����!" << std::endl;
        test_sheet = test_book->insertSheet(0, L"���Բ���");
        if (test_sheet)
        {
            std::cout << "sheet �����ɹ�!" << std::endl;
        }
        else {
            std::cout << "sheet ����ʧ��!" << std::endl;
            exit(1);
        }
    }
    else {
        std::cout << "��⵽sheet����Ϊ: " << test_book->sheetCount() << " ���Զ���ȡ��һ��sheet!" << std::endl;
        test_sheet = test_book->getSheet(0);
        if (!test_sheet)
        {
            std::cout << "��ȡ��һ��sheetʧ��!" << std::endl;
            exit(1);
        }
        else {
            std::cout << "��ȡ��һ��sheet�ɹ�!" << std::endl;
        }
    }

    std::wstring buffer;

    int row = 0, col = 0;
    libxl::CellType temp_cell_type = test_sheet->cellType(row, col);
    if (temp_cell_type == libxl::CELLTYPE_EMPTY)
    {
        std::cout << "(" << row << "," << col << ")"
            << "������,�������"
            << std::endl;
        if (test_sheet->writeNum(row, col, 1112))
        {
            std::cout << "�������ɹ�" << std::endl;
        }
        else {
            std::cout << "�������ʧ��!" << std::endl;
            exit(2);
        }

    }
    else {

        std::cout << "��������,׼����ȡ:" << std::endl;
        switch (temp_cell_type)
        {
        case libxl::CELLTYPE_EMPTY:
            break;
        case libxl::CELLTYPE_NUMBER:
            std::cout << "��������,����ֵΪ:" << test_sheet->readNum(row, col) << std::endl;
            break;
        case libxl::CELLTYPE_STRING:
            ;
            std::cout << "�ַ�����,����ֵΪ:";
            std::wcout << (test_sheet->readStr(row, col)) << std::endl;

            buffer = (test_sheet->readStr(row, col));
            if (buffer == L"����")
            {
                std::cout << "�ַ����Ա�,���Գɹ�!";
                test_sheet->writeStr(row, col, L"���Գɹ�!");
            }
            break;
        case libxl::CELLTYPE_BOOLEAN:
            libxl::Format* temp_format;
            std::cout << "��������,����ֵΪ:" << test_sheet->readBool(row, col) << std::endl;
            break;
        case libxl::CELLTYPE_BLANK:
            std::cout << "�ո�����,����ֵΪ:" << test_sheet->readBlank(row, col, &temp_format) << std::endl;
            break;
        case libxl::CELLTYPE_ERROR:
            std::cout << "��ȡ����!������Ϣ:" << test_sheet->readError(row, col) << std::endl;
            exit(3);
            break;
        default:
            break;
        }
    }


    test_book->save(L"C:\\Users\\likes\\Desktop\\�����ļ�.xlsx");

    test_book->release();

    //   test_book->errorMessage();
}

