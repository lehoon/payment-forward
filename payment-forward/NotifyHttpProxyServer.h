#ifndef NOTIFY_FORWARD_NOTIFY_HTTP_SERVER_H_
#define NOTIFY_FORWARD_NOTIFY_HTTP_SERVER_H_

#include "Configure.h"
#include "json_object.h"
#include "DatabaseClient.h"


class CNotifyHttpProxyServer
{
public:
	CNotifyHttpProxyServer(CConfigure* config);
	~CNotifyHttpProxyServer();

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
	ForwarRuleList forward_rule_list_;
	std::string forward_host_;
};



#endif // !NOTIFY_FORWARD_NOTIFY_HTTP_SERVER_H_

