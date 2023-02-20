#pragma once

#include <spdlog/spdlog.h>


class CLogger
{
private:
	CLogger();
	~CLogger();

public:
	static bool Init();
	static bool Close();
};

