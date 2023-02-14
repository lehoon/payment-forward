#include "Logger.h"
#include "NotifyHttpServer.h"


CNotifyHttpServer::CNotifyHttpServer(CConfigure* config) : config_(config) {
}

CNotifyHttpServer::~CNotifyHttpServer() {
	if (NULL != threadHandle_) {
		CloseHandle(threadHandle_);
	}
}

bool CNotifyHttpServer::InitTask() {
	databaseClient_.Open();
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

		httplib::Client forward_client(
			config_->GetValueAsString("forward", "host", "localhost"),
			config_->GetValueAsUint32("forward", "port", 9527)
			);

		forward_client.set_connection_timeout(15);
		forward_client.set_write_timeout(30);
		forward_client.set_read_timeout(15);

		auto result = forward_client.Post(
			config_->GetValueAsString("forward", "notify_url" ,"/payment/notify"), 
			content, 
			req.get_header_value("Content-Type"));

		ForwardRecord record;
		record.content = content;
		record.method = "post";
		record.url = "/notify";
		record.originHost = req.remote_addr;
		record.originPort = req.remote_port;
		record.forwardHost = config_->GetValueAsString("forward", "host", "localhost");
		record.forwardPort = config_->GetValueAsUint32("forward", "port", 9527);
		record.forwardUrl = config_->GetValueAsString("forward", "notify_url", "/notify");
		record.status = httplib::to_string(result.error());

		//response message to origin server
		if (result != nullptr) {
			rsp.set_content(result->body, "text/html; charset=utf-8");
			record.reason = result->reason;
			record.response = result->body;
		}

		databaseClient_.InsertForwardRecord(record);

		std::cout << "转发消息[/notify],到http://"
			<< config_->GetValueAsString("forward", "host", "localhost")
			<< ":" << config_->GetValueAsUint32("forward", "port", 9527)
			<< ", 请求类型:"
			<< req.get_header_value("Content-Type")
			<< ", 消息内容:"
			<< content 
			<<",转发状态:"
			<<record.status
			<<",转发状态描述:"
			<<record.reason
			<<",转发响应内容:"
			<<record.response
			<< std::endl;

		SPDLOG_INFO("转发消息[/notify],到http://{}:{}, 请求类型:{}, 消息内容:{},转发状态:{},转发状态描述:{},转发响应内容:{}",
			config_->GetValueAsString("forward", "host", "localhost"),
			config_->GetValueAsUint32("forward", "port", 9527),
			req.get_header_value("Content-Type"),
			content, record.status, record.reason, record.response);
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
		if (url.compare("/shutdown") == 0) return;
		std::cout << method << "  " << url << std::endl;
		});

	server_.listen("0.0.0.0", config_->GetValueAsUint32("server", "port", 9527));

	ExitTask();
	_endthreadex(0);
	return true;
}

