#pragma once

#include <time.h>
#include <limits.h>
#include <string>

#define  int64 __int64
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

public:
	unsigned int Hour();
	unsigned int Minute();
private:
	struct tm* mPDateTime;
};
