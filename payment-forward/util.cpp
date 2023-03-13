#include "util.h"
#include "DateTime.h"

#include <windows.h>
#include <algorithm>
#include <shellapi.h>
#include <iomanip>
#include <sstream>

#pragma warning(disable :4996)

namespace Util {

	void makeLower(char* source, int len)
	{
		std::transform(source, source + len, source, tolower);
	}

	void makeLower(const std::string& input, std::string& output)
	{
		size_t	length = input.size();
		if (length > 0)
		{
			output.resize(length);
			std::transform(input.begin(), input.end(), output.begin(), tolower);
		}
	}

	std::string str_toupper(std::string value) {
		std::transform(
			value.begin(), value.end(), value.begin(),
			[](unsigned char ch) -> unsigned char { return std::toupper(ch); });
		return value;
	}

	void trimString(const std::string& input, std::string& output)
	{
		output = input;
		size_t	start_index = input.find_first_not_of(" ");
		size_t	end_index = input.find_last_not_of(" ");
		if (start_index != std::string::npos &&
			end_index >= start_index)
		{
			output = input.substr(start_index, end_index + 1 - start_index);
		}
	}

	std::string	num_to_string(int num)
	{
		char buff[33] = { 0 };
		sprintf_s(buff, 33, "%d", num);
		return std::string(buff);
	}

	//字符串转int
	int string_to_num(std::string source) {
		int int_temple;
		std::stringstream temp;
		temp << source;
		temp >> int_temple;
		return int_temple;
	}

	double string_to_double(std::string source) {
		double double_temple;
		std::stringstream temp;
		temp.setf(std::ios::fixed);
		temp << source;
		temp >> double_temple;
		return double_temple;
	}

	std::string int_add_int_to_string(int num_one, int num_two) {
		std::string string_one = std::to_string(num_one);
		std::string string_two = std::to_string(num_two);
		std::string string_three = string_one + "." + string_two;
		return string_three;
	}

	std::string increase_string(std::string& source) {
		int v = string_to_num(source);
		v += 1;
		return std::to_string(v);
	}

	std::string	toUTF8(const std::wstring& source)
	{
		int bytes = WideCharToMultiByte(CP_UTF8, 0, source.c_str(), -1, NULL, 0, NULL, NULL);
		char* buffer = new char[bytes];
		bytes = WideCharToMultiByte(CP_UTF8, 0, source.c_str(), -1, buffer, bytes, NULL, NULL);

		std::string	dest(buffer);
		delete[] buffer;
		return dest;
	}

	std::string	toMBCS(const std::wstring& source)
	{
		int bytes = WideCharToMultiByte(CP_ACP, 0, source.c_str(), -1, NULL, 0, NULL, NULL);
		char* buffer = new char[bytes];
		bytes = WideCharToMultiByte(CP_ACP, 0, source.c_str(), -1, buffer, bytes, NULL, NULL);

		std::string	dest(buffer);
		delete[] buffer;
		return dest;
	}

	std::string GbkToUtf8(const std::string content) {
		int len = MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
		std::string strTemp = str;
		if (wstr) delete[] wstr;
		if (str) delete[] str;
		return strTemp;
	}

	std::string Utf8ToGbk(const std::string content) {
		int len = MultiByteToWideChar(CP_UTF8, 0, content.c_str(), -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_UTF8, 0, content.c_str(), -1, wstr, len);
		len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
		std::string strTemp = str;
		if (wstr) delete[] wstr;
		if (str) delete[] str;
		return strTemp;
	}

	std::wstring Char2WChar(const char* source) {
		int len = MultiByteToWideChar(CP_ACP, 0, source, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, source, -1, wstr, len);
		std::wstring strTemp = wstr;
		if (wstr) delete[] wstr;
		return strTemp;
	}

	void HideTaskWindow() {
		HWND hWnd = ::FindWindow(TEXT("Shell_traywnd"), TEXT(""));
		::SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_HIDEWINDOW);

		APPBARDATA appData;
		memset(&appData, 0, sizeof(APPBARDATA));
		appData.cbSize = sizeof(appData);
		appData.hWnd = hWnd;

		if (appData.hWnd != NULL)
		{
			appData.lParam = ABS_AUTOHIDE | ABS_ALWAYSONTOP;
			SHAppBarMessage(ABM_SETSTATE, &appData); //设置窗口全屏显示 
		}
	}

	void ShowTaskWindow() {
		HWND hWnd = ::FindWindow(TEXT("Shell_traywnd"), TEXT(""));
		::SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_SHOWWINDOW);

		APPBARDATA appData;
		memset(&appData, 0, sizeof(APPBARDATA));
		appData.cbSize = sizeof(appData);
		appData.hWnd = hWnd;

		if (appData.hWnd != NULL)
		{
			appData.lParam = ABS_ALWAYSONTOP;
			SHAppBarMessage(ABM_SETSTATE, &appData); //设置窗口全屏显示 
		}
	}

	static std::string fill_number_string_with_width(unsigned int number, int width) {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(width) << number;
		return ss.str();
	}

	static std::string fill_number_string_with_five(unsigned int number) {
		if (number > 99999) return "99999";
		return fill_number_string_with_width(number, 5);
	}

	//生成流水号
	std::string sequence_no() {
		std::string sequence_code;
		srand((unsigned int)time(NULL));
		sequence_code.append(DateTime::get14CurrentTime());
		sequence_code.append(fill_number_string_with_five(rand()));
		return sequence_code;
	}

};

