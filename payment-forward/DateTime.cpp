#include "DateTime.h"
#include <sstream>
#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>

DateTime::DateTime() {
	time_t now = time(nullptr);
	mPDateTime = localtime(&now);
}

DateTime::~DateTime() {
}

std::string DateTime::getCurrentDateTime() {
	std::stringstream ss;
	std::string format = "%Y-%m-%d %H:%M:%S";
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now(); //时间点可以做差
	time_t t1 = std::chrono::system_clock::to_time_t(a); //time_t可以格式化
	ss << std::put_time(localtime(&t1), format.c_str());
	std::string str1 = ss.str();
	return str1;
}

std::string  DateTime::getCurrentTime() {
	std::stringstream ss;
	std::string format = "%H:%M:%S";
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now(); //时间点可以做差
	time_t t1 = std::chrono::system_clock::to_time_t(a); //time_t可以格式化
	ss << std::put_time(localtime(&t1), format.c_str());
	std::string str1 = ss.str();
	return str1;
}


std::string DateTime::get14CurrentTime() {
	std::stringstream ss;
	std::string format = "%Y%m%d%H%M%S";
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now(); //时间点可以做差
	time_t t1 = std::chrono::system_clock::to_time_t(a); //time_t可以格式化
	ss << std::put_time(localtime(&t1), format.c_str());
	std::string str1 = ss.str();
	return str1;
}

std::string DateTime::get6CurrentTime() {
	std::stringstream ss;
	std::string format = "%Y%m";
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now(); //时间点可以做差
	time_t t1 = std::chrono::system_clock::to_time_t(a); //time_t可以格式化
	ss << std::put_time(localtime(&t1), format.c_str());
	std::string str1 = ss.str();
	return str1;
}

std::string DateTime::get8CurrentTime() {
	std::stringstream ss;
	std::string format = "%Y%m%d";
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
	time_t t1 = std::chrono::system_clock::to_time_t(a);
	ss << std::put_time(localtime(&t1), format.c_str());
	std::string str1 = ss.str();
	return str1;
}

std::string DateTime::getYearMonthDay() {
	std::stringstream ss;
	std::string format = "%Y-%m-%d";
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
	time_t t1 = std::chrono::system_clock::to_time_t(a);
	ss << std::put_time(localtime(&t1), format.c_str());
	std::string str1 = ss.str();
	return str1;
}

std::string DateTime::getCurrentMilliSecond() {
	auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	return std::to_string(millisec);
}

unsigned int DateTime::Hour() {
	if (mPDateTime == NULL) return 1;
	return mPDateTime->tm_hour;
}


unsigned int DateTime::Minute() {
	if (mPDateTime == NULL) return 1;
	return mPDateTime->tm_min;
}

