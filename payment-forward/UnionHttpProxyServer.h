#ifndef NOTIFY_FORWARD_UNION_HTTP_SERVER_H_
#define NOTIFY_FORWARD_UNION_HTTP_SERVER_H_

#include "httplib.h"
#include "Configure.h"
#include "json_object.h"
#include "DatabaseClient.h"

class CUnionHttpProxyServer
{
public:
	CUnionHttpProxyServer(CConfigure* config);
	~CUnionHttpProxyServer();

public:
	bool InitTask();
	bool ExitTask();
	bool StartTask();

public:
	static unsigned int WINAPI threadFunc(void* p);
	bool Work();

private:
	httplib::Client _get_forward_client();

private:
	httplib::Server server_;
	HANDLE threadHandle_;
	unsigned int threadId_;
	CConfigure* config_;
	CDatabaseClient databaseClient_;
	ForwarRuleList forward_rule_list_;
	std::string forward_host_;
};

#endif