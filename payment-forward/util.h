#pragma once


#include <string>


namespace Util {

	//转换字符串为小写
	void makeLower(char* source, int len);

	//转换字符串为小写
	void makeLower(const std::string& input, std::string& output);

	std::string str_toupper(std::string value);

	//数字转字符串
	std::string	num_to_string(int num);

	//字符串转int
	int string_to_num(std::string source);

	double string_to_double(std::string source);

	std::string int_add_int_to_string(int, int);

	std::string increase_string(std::string& source);

	//除去字符串两边空格
	void trimString(const std::string& input, std::string& output);

	//Unicode转UTF8
	std::string	toUTF8(const std::wstring& source);

	//Unicode转MBCS
	std::string	toMBCS(const std::wstring& source);

	std::string GbkToUtf8(const std::string content);

	std::string Utf8ToGbk(const std::string content);

	std::wstring Char2WChar(const char*);

	//隐藏桌面
	void HideTaskWindow();

	//显示桌面
	void ShowTaskWindow();

	//生成流水号
	std::string sequence_no();

	//get date string  format yyyymm


};

