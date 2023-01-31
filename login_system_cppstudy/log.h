/*
* 日志文件作为一个对象
*
* @name		log
* @date		2023 - 1 - 31
* @version	V2.0
* @anthor	likeszm
*/

#pragma once

class Log_t
{
public:
	Log_t();
	~Log_t();

	void write(std::string type, std::string info);
	void info(std::string info);
	void error(std::string info);

private:
	std::wfstream* log_file;		//日志文件指针
	std::string get_time(void);
};
