#include "main.h"

Log_t::Log_t()
{
	//����־�ļ�

	log_file = new std::wfstream;

	log_file->open("log.txt", std::ios::out | std::ios::app);
	
}

Log_t::~Log_t()
{
	log_file->close();

	delete log_file;

	log_file = NULL;
}

std::string Log_t::get_time(void)
{
	//�Ȼ�ȡ��ǰʱ��
	time_t now_time = time(NULL);	

	//�ٽ��и�ʽת��
	struct tm now;
	localtime_s(&now, &now_time);

	char temp[20];

	sprintf_s(temp, 20, "%d-%d-%d %d:%d:%d",
		now.tm_year + 1900,
		now.tm_mon + 1,
		now.tm_mday,
		now.tm_hour,
		now.tm_min,
		now.tm_sec);

	// = ���������ʵ��ת����ʽ
	std::string output = temp;

	return output;
}

void Log_t::write(std::string type ,std::string info)
{
	std::string temp = "\n";

	temp += "Time:\t";
	temp += get_time();
	temp += "\n";

	temp += type;
	temp += ":\t";
	temp += info;
	temp += "\n";

	// string �Դ��ķ���ת��Ϊ char ����, ʵ������д��
	*log_file << temp.c_str();
}

void Log_t::info(std::string info)
{
	write("Info", info);
}

void Log_t::error(std::string info)
{
	write("ERROR", info);
}
