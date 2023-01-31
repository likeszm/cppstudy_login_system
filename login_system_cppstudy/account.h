/*
* ��������Ϊһ������
* 
* @name		account
* @date		2023 - 1 - 31
* @version	V2.0
* @anthor	likeszm
*/

#pragma once

class Account_t
{
public:
	Account_t();
	~Account_t();

	int	insert(std::wstring name, std::wstring pswd);	//��
	int	remove(std::wstring name, std::wstring pswd);	//ɾ
	int modify(std::wstring name, std::wstring pswd, std::wstring pswd_new);	//��
	int query (std::wstring name, std::wstring pswd);	//��

private:
	libxl::Book* p_book_file;
	libxl::Sheet* p_sheet_file;
	bool create_original_book(void);
};



