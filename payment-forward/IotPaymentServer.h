#pragma once
#include "Configure.h"
#include "json_object.h"
#include "DatabaseClient.h"

class CIotPaymentHttpServer
{
public:
	CIotPaymentHttpServer(CConfigure* config);
	~CIotPaymentHttpServer();

public:
	bool InitTask();
	bool ExitTask();
	bool StartTask();

public:
	static unsigned int WINAPI threadFunc(void* p);
	bool Work();

private:
	void _get_forward_client();

private:
	HANDLE threadHandle_;
	unsigned int threadId_;
	CConfigure* config_;
	CDatabaseClient databaseClient_;
};

