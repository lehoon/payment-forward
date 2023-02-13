#ifndef NOTIFY_FORWARD_NOTIFY_HTTP_CLIENT_H_
#define NOTIFY_FORWARD_NOTIFY_HTTP_CLIENT_H_

#include <string>

class CNotifyHttpClient
{
private:
	CNotifyHttpClient() {}
	~CNotifyHttpClient() {}

public:
	static void SendShutdownMessage(int port, std::string& key);
};

#endif // !NOTIFY_FORWARD_NOTIFY_HTTP_CLIENT_H_



