#include "NotifyHttpServer.h"

CNotifyHttpServer::CNotifyHttpServer(CConfigure* config) : config_(config) {
}

CNotifyHttpServer::~CNotifyHttpServer() {
	if (NULL != threadHandle_) {
		CloseHandle(threadHandle_);
	}
}

bool CNotifyHttpServer::InitTask() {
	return true;
}

bool CNotifyHttpServer::ExitTask() {
	return true;
}

bool CNotifyHttpServer::StartTask() {
	threadHandle_ = (HANDLE)_beginthreadex(NULL, 0,
		CNotifyHttpServer::threadFunc, (void*)this, 0, &threadId_);
	return threadId_ != 0;
}

unsigned int CNotifyHttpServer::threadFunc(void* p)
{
	CNotifyHttpServer* threadObj = (CNotifyHttpServer*)p;
	return threadObj->Work();
}

bool CNotifyHttpServer::Work() {
	server_.set_base_dir("./");

	server_.Post(R"(/notify)", [&](const httplib::Request& req, httplib::Response& rsp) {
		std::string content = req.body;
		rsp.set_content("success", "text/plain; charset=utf-8");
		httplib::Client forward_client(
			config_->GetValueAsString("client", "host", "localhost"),
			config_->GetValueAsUint32("client", "port", 9527)
			);

		forward_client.set_connection_timeout(60);
		forward_client.set_write_timeout(180);
		forward_client.set_read_timeout(180);
		std::cout << "开始转发消息." << content << std::endl;
		forward_client.Post(config_->GetValueAsString("client", "notify_url" ,"/payment/notify"), content, "text/plain; charset=utf-8");
		});

	server_.Get(R"(/payment/shutdown)", [&](const httplib::Request& req, httplib::Response& res) {
		std::cout << "接收到退出信号" << std::endl;
		server_.stop();
		});

	server_.listen("0.0.0.0", config_->GetValueAsUint32("server", "port", 9527));
	ExitTask();
	_endthreadex(0);
	return true;
}

