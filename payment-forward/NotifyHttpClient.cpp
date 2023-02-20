#include "NotifyHttpClient.h"
#include "httplib.h"

void CNotifyHttpClient::SendShutdownMessage(std::string host, int port, std::string & key) {
	httplib::Client client(host, port);
	client.set_connection_timeout(5);
	client.set_read_timeout(5);
	client.set_write_timeout(5);
	httplib::Result result = client.Get("/shutdown?key=" + key);
	if (result.operator bool()) {

	}
}

