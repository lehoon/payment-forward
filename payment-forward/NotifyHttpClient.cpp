#include "NotifyHttpClient.h"
#include "httplib.h"

void CNotifyHttpClient::SendShutdownRequest(std::string host, int port, std::string key) {
	httplib::Client client(host, port);
	client.set_connection_timeout(5);
	client.set_read_timeout(5);
	client.set_write_timeout(5);
	httplib::Result result = client.Get("/forward/shutdown?key=" + key);
	if (result.operator bool()) {
	}
}

