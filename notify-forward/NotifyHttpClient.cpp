#include "NotifyHttpClient.h"
#include "httplib.h"

void CNotifyHttpClient::SendShutdownMessage(int port, std::string & key) {
	httplib::Client client("localhost", port);
	client.set_connection_timeout(5);
	client.set_read_timeout(5);
	client.set_write_timeout(5);
	client.Get("/shutdown?key=" + key);
}

