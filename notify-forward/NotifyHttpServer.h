#ifndef NOTIFY_FORWARD_NOTIFY_HTTP_SERVER_H_
#define NOTIFY_FORWARD_NOTIFY_HTTP_SERVER_H_


#include "httplib.h"
#include "Configure.h"

class CNotifyHttpServer
{
public:
	CNotifyHttpServer(CConfigure *config);
	~CNotifyHttpServer();

public:
	bool InitTask();
	bool ExitTask();
	bool StartTask();

public:
	static unsigned int WINAPI threadFunc(void* p);
	bool Work();

private:
	httplib::Server server_;
	HANDLE threadHandle_;
	unsigned int threadId_;
	CConfigure* config_;
};



#endif // !NOTIFY_FORWARD_NOTIFY_HTTP_SERVER_H_

