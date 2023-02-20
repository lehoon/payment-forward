#include "Logger.h"
#include "json.h"
#include "json_object.h"
#include "UnionHttpProxyServer.h"

CUnionHttpProxyServer::CUnionHttpProxyServer(CConfigure* config) : config_(config) {
}

CUnionHttpProxyServer::~CUnionHttpProxyServer() {
	if (NULL != threadHandle_) {
		CloseHandle(threadHandle_);
	}
}

bool CUnionHttpProxyServer::InitTask() {
	//std::string forward_rule_string = config_->GetValueAsString("unionpay_forward", "forward.rule", "[]");
	//ForwarRuleList ruleList = JsonObject::Json2ForwarRuleList(forward_rule_string);
	//forward_rule_list_.swap(ruleList);
	databaseClient_.Open();
	return true;
}

bool CUnionHttpProxyServer::ExitTask() {
	server_.stop();
	return true;
}

bool CUnionHttpProxyServer::StartTask() {
	threadHandle_ = (HANDLE)_beginthreadex(NULL, 0,
		CUnionHttpProxyServer::threadFunc, (void*)this, 0, &threadId_);
	return threadId_ != 0;
}

unsigned int CUnionHttpProxyServer::threadFunc(void* p) {
	CUnionHttpProxyServer* threadObj = (CUnionHttpProxyServer*)p;
	return threadObj->Work();
}

httplib::Client CUnionHttpProxyServer::_get_forward_client() {
	int type = config_->GetValueAsUint32("unionpay_forward", "unionpay.type", 0);

	if (type == 1) {
		//ip:port
		forward_host_ = config_->GetValueAsString("unionpay_forward", "forward.host", "https://qra.95516.com");
		return httplib::Client(forward_host_);
	}

	//domain.com
	std::string protocol = config_->GetValueAsString("unionpay_forward", "forward.protocol", "http");
	forward_host_ = protocol + "://" + config_->GetValueAsString("unionpay_forward", "forward.host", "localhost") + ":" +
		std::to_string(config_->GetValueAsUint32("unionpay_forward", "forward.port", 80));

	return httplib::Client(
		config_->GetValueAsString("unionpay_forward", "forward.host", "localhost"),
		config_->GetValueAsUint32("unionpay_forward", "forward.port", 80)
	);
}

bool CUnionHttpProxyServer::Work() {
	server_.set_base_dir("./");

	server_.Post("/pay/gateway", [&](const httplib::Request& req, httplib::Response& rsp) {
		std::string content = req.body;
		httplib::Client forward_client(
			config_->GetValueAsString("unionpay_forward", "forward.host", "https://qra.95516.com")
		);
		forward_client.set_connection_timeout(15);
		forward_client.set_write_timeout(30);
		forward_client.set_read_timeout(15);
		httplib::Headers headers;
		auto result = forward_client.Post(
			"/pay/gateway",
			headers,
			content,
			req.get_header_value("Content-Type"));

		ForwardRecord record;
		record.content = content;
		record.method = "post";
		record.url = "/pay/gateway";
		record.originHost = req.remote_addr;
		record.originPort = req.remote_port;
		record.forwardHost = config_->GetValueAsString("unionpay_forward", "forward.host", "localhost");
		record.forwardPort = config_->GetValueAsUint32("unionpay_forward", "forward.port", 80);
		record.forwardUrl = "/pay/gateway";
		record.status = httplib::to_string(result.error());

		//response message to origin server
		if (result != nullptr) {
			rsp.set_content(result->body, "text/html; charset=utf-8");
			record.reason = result->reason;
			record.response = result->body;
			record.status = std::to_string(result->status);
		}

		databaseClient_.InsertUnionForwardRecord(record);

		std::cout << "转发消息[" << "/pay/gateway"
			<< "], 到"
			<< forward_host_
			<< "/pay/gateway"
			<< ", 请求类型:"
			<< req.get_header_value("Content-Type")
			<< ", 消息内容:"
			<< content
			<< ",转发状态:"
			<< record.status
			<< ",转发状态描述:"
			<< record.reason
			<< ",转发响应内容:"
			<< record.response
			<< std::endl;

		SPDLOG_INFO("转发[{}]消息到{}{}, 请求类型:{}, 消息内容:{},转发状态:{},转发状态描述:{},转发响应内容:{}",
			"post",
			forward_host_,
			"/pay/gateway",
			req.get_header_value("Content-Type"),
			content,
			record.status,
			record.reason,
			record.response);
		});

	std::cout << "当前注册的UnionPay Forward转发url规则如下:" << std::endl;
	server_.Walk([](const std::string method, std::string url) {
		std::cout << method << "  " << url << std::endl;
		});
	//for (auto iter = forward_rule_list_.begin(); iter != forward_rule_list_.end(); iter++) {
	//	std::cout << iter->method <<": " << iter->origin_url << " ===> " << iter->target_url << std::endl;
	//}

	bool isRunning = server_.listen(
		config_->GetValueAsString("unionpay_forward", "proxy.host", "localhost"), 
		config_->GetValueAsUint32("unionpay_forward", "proxy.port", 9528));

	ExitTask();
	_endthreadex(0);
	return true;
}





