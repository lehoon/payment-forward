#pragma once

#include "Configure.h"
#include "IThreadWrap.h"
#include "WorkThread.h"
#include "DatabaseClient.h"

class IotUploadFileTask : public IThreadUser
{
public:
	IotUploadFileTask(CConfigure *config);
	~IotUploadFileTask();

public:
	bool InitTask();
	bool ExitTask();
	bool Work();

private:
	CConfigure* _config;
	CDatabaseClient _database_client;
};
