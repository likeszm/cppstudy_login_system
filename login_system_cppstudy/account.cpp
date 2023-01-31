#include "main.h"

Account_t::Account_t(): p_book_file(NULL),p_sheet_file(NULL)
{
	p_book_file = xlCreateBook();

	if (p_book_file == NULL)
	{
#ifdef DEBUG
		std::cerr << "ERROR: in file:" << __FILE__" line = " << __LINE__ << std::endl;
#endif // 内存占用满了
		exit(0);
	}

	//没有表就先创建一个表
	if (false == p_book_file->load(L"account.xlsx"))
	{
		if (false == create_original_book())
		{
			//既没有已存在的密码表，又无法创建一个新的密码表
			exit(0);
		}
	}

	//打开sheet(第1个sheet)
	p_sheet_file = p_book_file->getSheet(0);
	if (p_sheet_file == NULL)
	{
		//没有第一个sheet
		if (false == create_original_book())
		{
			//既没有已存在的密码表，又无法创建一个新的密码表
			exit(0);
		}
	}
	else//如果密码表存在就进行必要数据检查(表头格式检查)
	{
		if(	p_sheet_file->cellType(0, 0) == libxl::CellType::CELLTYPE_STRING
			&& p_sheet_file->cellType(0, 1) == libxl::CellType::CELLTYPE_STRING
			&& p_sheet_file->cellType(1, 0) == libxl::CellType::CELLTYPE_STRING
			&& p_sheet_file->cellType(1, 1) == libxl::CellType::CELLTYPE_STRING )
		{
			std::wstring temp1 = p_sheet_file->readStr(0, 0);
			std::wstring temp2 = p_sheet_file->readStr(0, 1);
			std::wstring temp3 = p_sheet_file->readStr(1, 0);
			std::wstring temp4 = p_sheet_file->readStr(1, 1);
			
			//检查表头的值是否正确
			if (temp1.find(L"user") == -1
				|| temp1.find(L"name") == -1
				|| temp2.find(L"password") == -1 
				|| temp3 != L"admin"
				|| temp4 != L"root")
			{
				if (false == create_original_book())
				{
					//已存在的密码表格式不正确，又无法创建一个新的密码表
					exit(0);
				}
			}
		}
		else
		{
			if (false == create_original_book())
			{
				//已存在的密码表格式不正确（数据类型就不对），又无法创建一个新的密码表
				exit(0);
			}
		}
	}

	//到此,密码表检查完成,并且已经完成初始化了(格式正确),可以正常读写

}

Account_t::~Account_t()
{
	//释放内存
	p_book_file->release();

	p_book_file = NULL;
	p_sheet_file = NULL;
}

bool Account_t::create_original_book(void)
{
	libxl::Sheet* temp = NULL;

	if (p_sheet_file == NULL)
	{
		temp = p_book_file->addSheet(L"account_sheet");
	}
	else
	{
		temp = p_sheet_file;
	}

	if (temp == NULL)
	{
#ifdef DEBUG
		std::cerr << "ERROR: in file:" << __FILE__" line = " << __LINE__ << std::endl;
#endif // 无法创建文件
		return false;
	}
	else
	{
		//先清空表的数据
		temp->clear(temp->firstRow(), temp->lastRow(), temp->firstCol(), temp->lastCol());

		//然后初始化内容
		temp->writeStr(0, 0, L"user name");
		temp->writeStr(0, 1, L"password");
		temp->writeStr(1, 0, L"admin");
		temp->writeStr(1, 1, L"root");

		//保存更新
		p_book_file->save(L"account.xlsx");

		p_sheet_file = temp; //新建的sheet可以直接使用
		return true;
	}

}

//返回0: 插入成功
//返回1: 账户已存在,插入失败
//返回-1 密码表格式有误
//返回-2 写入文件失败
int Account_t::insert(std::wstring name, std::wstring pswd)
{
	int flag = query(name, pswd);

	if (flag == 2)
	{
		int last_row = p_sheet_file->lastRow();

		p_sheet_file->writeStr(last_row, 0, name.c_str());
		p_sheet_file->writeStr(last_row, 1, pswd.c_str());

		if ( false == p_book_file->save(L"account.xlsx"))
		{
			return -2;
		}
		else
		{
			return 0;
		}
		
	}
	else if (flag == 0 || flag == 1)
	{
		//账户已存在
		return 1;
	}
	else if (flag == -1)
	{
		return -1;
	}
}

//返回0: 删除成功
//返回1: 密码错误,删除失败
//返回2: 账户不存在,删除失败
//返回-1 密码表格式出错 ?
//返回-2 文件写入失败
int Account_t::remove(std::wstring name, std::wstring pswd)
{
	int flag = query(name, pswd);

	if (flag == 0)
	{
		//删除条目(可以改进: 将下面的内容上移)
		for (int i = 1; i < p_sheet_file->lastRow(); i++)
		{
			if (p_sheet_file->cellType(i, 0) == libxl::CellType::CELLTYPE_STRING)
			{
				std::wstring temp = p_sheet_file->readStr(i, 0);
				if (temp == name)	//用户名匹配上了
				{
					if (false == p_sheet_file->removeRow(i, i))
					{
						return -2;	//删除操作失败
					}
				}
			}
		}

		if (false == p_book_file->save(L"account.xlsx"))
		{
			return -2;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return flag;
	}
	return 0;
}

//返回0: 修改成功
//返回1: 密码相同,修改失败
//返回2: 账户不存在,修改失败
//返回-1 密码表格式出错 ?
//返回-2 文件写入失败
int Account_t::modify(std::wstring name, std::wstring pswd, std::wstring pswd_new)
{
	int flag = query(name, pswd);

	if (flag == 0)
	{
		//删除条目(可以改进: 将下面的内容上移)
		for (int i = 1; i < p_sheet_file->lastRow(); i++)
		{
			if (p_sheet_file->cellType(i, 0) == libxl::CellType::CELLTYPE_STRING)
			{
				std::wstring temp = p_sheet_file->readStr(i, 0);
				if (temp == name)	//用户名匹配上了
				{
					if (false ==  p_sheet_file->writeStr(i,1, pswd_new.c_str()))
					{
						return -2;	//覆盖操作失败
					}
				}
			}
		}

		if (false == p_book_file->save(L"account.xlsx"))
		{
			return -2;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return flag;
	}
	return 0;
}

//返回0: 账户密码均正确
//返回1: 密码错误
//返回2: 账户不存在
//返回-1 密码表格式出错
int Account_t::query(std::wstring name, std::wstring pswd)
{
	for (int i = 1; i < p_sheet_file->lastRow(); i++)
	{
		if (p_sheet_file->cellType(i, 0) == libxl::CellType::CELLTYPE_STRING)
		{
			std::wstring temp = p_sheet_file->readStr(i, 0);
			if (temp == name)	//用户名匹配上了
			{
				if (p_sheet_file->cellType(i, 1) == libxl::CellType::CELLTYPE_STRING)
				{
					std::wstring temp = p_sheet_file->readStr(i, 1);
					if (temp == pswd)
					{
						return 0;
					}
					else
					{
						return 1;
					}
				}
				else
				{
					return -1; //密码不是字符格式
				}
			}
		}
		else if (p_sheet_file->cellType(i, 0) == libxl::CellType::CELLTYPE_EMPTY
			|| p_sheet_file->cellType(i, 0) == libxl::CellType::CELLTYPE_BLANK)
		{
			continue;		//遇到空格或者空内容就跳过
		}
		else
		{
			return -1; //当前账户名不是字符格式
		}
	}

	//遍历整个表也没查到,说明账户不存在
	return 2;
}
