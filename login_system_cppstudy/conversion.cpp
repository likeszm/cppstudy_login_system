/*
* @name		conversion
* @date		2023 - 1 - 31
* @version	V2.0
* @anthor	likeszm
*/
#include "main.h"

#include <string>
#include <locale>
#include <codecvt>


inline void raise_error(void)
{
#ifdef DEBUG
	std::cerr << "ERROR: in file:" << __FILE__" line = " << __LINE__ << std::endl;
#endif // DEBUG
}

//convert string to wstring
inline std::wstring to_wide_string(const std::string& input)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(input);
}

//convert wstring to string 
 std::string to_byte_string(const std::wstring& input)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(input);
}

