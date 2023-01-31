#include "main.h"

Account_t::Account_t(): p_book_file(NULL),p_sheet_file(NULL)
{
	p_book_file = xlCreateBook();

	if (p_book_file == NULL)
	{
#ifdef DEBUG
		std::cerr << "ERROR: in file:" << __FILE__" line = " << __LINE__ << std::endl;
#endif // �ڴ�ռ������
		exit(0);
	}

	//û�б���ȴ���һ����
	if (false == p_book_file->load(L"account.xlsx"))
	{
		if (false == create_original_book())
		{
			//��û���Ѵ��ڵ���������޷�����һ���µ������
			exit(0);
		}
	}

	//��sheet(��1��sheet)
	p_sheet_file = p_book_file->getSheet(0);
	if (p_sheet_file == NULL)
	{
		//û�е�һ��sheet
		if (false == create_original_book())
		{
			//��û���Ѵ��ڵ���������޷�����һ���µ������
			exit(0);
		}
	}
	else//����������ھͽ��б�Ҫ���ݼ��(��ͷ��ʽ���)
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
			
			//����ͷ��ֵ�Ƿ���ȷ
			if (temp1.find(L"user") == -1
				|| temp1.find(L"name") == -1
				|| temp2.find(L"password") == -1 
				|| temp3 != L"admin"
				|| temp4 != L"root")
			{
				if (false == create_original_book())
				{
					//�Ѵ��ڵ�������ʽ����ȷ�����޷�����һ���µ������
					exit(0);
				}
			}
		}
		else
		{
			if (false == create_original_book())
			{
				//�Ѵ��ڵ�������ʽ����ȷ���������;Ͳ��ԣ������޷�����һ���µ������
				exit(0);
			}
		}
	}

	//����,����������,�����Ѿ���ɳ�ʼ����(��ʽ��ȷ),����������д

}

Account_t::~Account_t()
{
	//�ͷ��ڴ�
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
#endif // �޷������ļ�
		return false;
	}
	else
	{
		//����ձ������
		temp->clear(temp->firstRow(), temp->lastRow(), temp->firstCol(), temp->lastCol());

		//Ȼ���ʼ������
		temp->writeStr(0, 0, L"user name");
		temp->writeStr(0, 1, L"password");
		temp->writeStr(1, 0, L"admin");
		temp->writeStr(1, 1, L"root");

		//�������
		p_book_file->save(L"account.xlsx");

		p_sheet_file = temp; //�½���sheet����ֱ��ʹ��
		return true;
	}

}

//����0: ����ɹ�
//����1: �˻��Ѵ���,����ʧ��
//����-1 ������ʽ����
//����-2 д���ļ�ʧ��
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
		//�˻��Ѵ���
		return 1;
	}
	else if (flag == -1)
	{
		return -1;
	}
}

//����0: ɾ���ɹ�
//����1: �������,ɾ��ʧ��
//����2: �˻�������,ɾ��ʧ��
//����-1 ������ʽ���� ?
//����-2 �ļ�д��ʧ��
int Account_t::remove(std::wstring name, std::wstring pswd)
{
	int flag = query(name, pswd);

	if (flag == 0)
	{
		//ɾ����Ŀ(���ԸĽ�: ���������������)
		for (int i = 1; i < p_sheet_file->lastRow(); i++)
		{
			if (p_sheet_file->cellType(i, 0) == libxl::CellType::CELLTYPE_STRING)
			{
				std::wstring temp = p_sheet_file->readStr(i, 0);
				if (temp == name)	//�û���ƥ������
				{
					if (false == p_sheet_file->removeRow(i, i))
					{
						return -2;	//ɾ������ʧ��
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

//����0: �޸ĳɹ�
//����1: ������ͬ,�޸�ʧ��
//����2: �˻�������,�޸�ʧ��
//����-1 ������ʽ���� ?
//����-2 �ļ�д��ʧ��
int Account_t::modify(std::wstring name, std::wstring pswd, std::wstring pswd_new)
{
	int flag = query(name, pswd);

	if (flag == 0)
	{
		//ɾ����Ŀ(���ԸĽ�: ���������������)
		for (int i = 1; i < p_sheet_file->lastRow(); i++)
		{
			if (p_sheet_file->cellType(i, 0) == libxl::CellType::CELLTYPE_STRING)
			{
				std::wstring temp = p_sheet_file->readStr(i, 0);
				if (temp == name)	//�û���ƥ������
				{
					if (false ==  p_sheet_file->writeStr(i,1, pswd_new.c_str()))
					{
						return -2;	//���ǲ���ʧ��
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

//����0: �˻��������ȷ
//����1: �������
//����2: �˻�������
//����-1 ������ʽ����
int Account_t::query(std::wstring name, std::wstring pswd)
{
	for (int i = 1; i < p_sheet_file->lastRow(); i++)
	{
		if (p_sheet_file->cellType(i, 0) == libxl::CellType::CELLTYPE_STRING)
		{
			std::wstring temp = p_sheet_file->readStr(i, 0);
			if (temp == name)	//�û���ƥ������
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
					return -1; //���벻���ַ���ʽ
				}
			}
		}
		else if (p_sheet_file->cellType(i, 0) == libxl::CellType::CELLTYPE_EMPTY
			|| p_sheet_file->cellType(i, 0) == libxl::CellType::CELLTYPE_BLANK)
		{
			continue;		//�����ո���߿����ݾ�����
		}
		else
		{
			return -1; //��ǰ�˻��������ַ���ʽ
		}
	}

	//����������Ҳû�鵽,˵���˻�������
	return 2;
}
