#pragma once

#include <time.h>
#include <limits.h>
#include <string>

const int SECONDS_IN_ONEDAY = 86400;

class DateTime {
public:
	DateTime();
	~DateTime();
public:
	static std::string getCurrentDateTime();
	static std::string getCurrentTime();
	static std::string get14CurrentTime();
	static std::string get6CurrentTime();
	static std::string get8CurrentTime();
	static std::string getYearMonthDay();

public:
	static std::string getCurrentMilliSecond();

public:
	unsigned int Hour();
	unsigned int Minute();
private:
	struct tm* mPDateTime;
};
