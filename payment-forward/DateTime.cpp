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

unsigned int DateTime::Hour() {
	if (mPDateTime == NULL) return 1;
	return mPDateTime->tm_hour;
}


unsigned int DateTime::Minute() {
	if (mPDateTime == NULL) return 1;
	return mPDateTime->tm_min;
}

