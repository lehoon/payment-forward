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
		rsp.set_content("success", "text/html; charset=utf-8");
		httplib::Client forward_client(
			config_->GetValueAsString("forward", "host", "localhost"),
			config_->GetValueAsUint32("forward", "port", 9527)
			);

		forward_client.set_connection_timeout(60);
		forward_client.set_write_timeout(180);
		forward_client.set_read_timeout(180);
		std::cout << "转发消息[/notify],到http://" 
			<< config_->GetValueAsString("forward", "host", "localhost") 
			<<":" << config_->GetValueAsUint32("forward", "port", 9527)
			<< ", 请求类型:"
			<< req.get_header_value("Content-Type")
			<< ", 消息内容:"
			<< content << std::endl;
		forward_client.Post(config_->GetValueAsString("forward", "notify_url" ,"/payment/notify"), 
			content, 
			req.get_header_value("Content-Type"));
		});

	server_.Get(R"(/shutdown)", [&](const httplib::Request& req, httplib::Response& rsp) {
		std::string key = req.get_param_value("key");
		std::string shutdownKey = config_->GetValueAsString("manager", "key", "");
		if (key.compare(shutdownKey) != 0) {
			std::cout << "接收到错误的停止服务密钥,不能停止服务, key=" << key << std::endl;
			rsp.set_content("错误的停止服务密钥,不能停止服务", "text/html; charset=utf-8");
			return;
		}

		std::cout << "接收到退出信号" << std::endl;
		server_.stop();
		});

	std::cout << "当前注册的转发url规则如下:" << std::endl;
	server_.Walk([](const std::string method, std::string url) {
		std::cout << method << "  " << url << std::endl;
		});

	server_.listen("0.0.0.0", config_->GetValueAsUint32("server", "port", 9527));
	ExitTask();
	_endthreadex(0);
	return true;
}

