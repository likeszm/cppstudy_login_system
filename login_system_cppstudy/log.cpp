#include "main.h"

Log_t::Log_t()
{
	//打开日志文件

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
	//先获取当前时间
	time_t now_time = time(NULL);	

	//再进行格式转换
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

	// = 运算符重载实现转换格式
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

	// string 自带的方法转换为 char 类型, 实现中文写入
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
