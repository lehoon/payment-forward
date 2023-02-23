#include "httplib.h"
#include "Logger.h"
#include "json.h"
#include "DateTime.h"
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
	_get_forward_client();
	std::string forward_rule_string = config_->GetValueAsString("unionpay_forward", "forward.rule", "[]");
	ForwarRuleList ruleList = JsonObject::Json2ForwarRuleList(forward_rule_string);

	if (ruleList.empty()) {
		SPDLOG_INFO("未定义转发规则,代理程序退出");
		return false;
	}

	forward_rule_list_.swap(ruleList);
	databaseClient_.Open();
	return true;
}

bool CUnionHttpProxyServer::ExitTask() {
	forward_rule_list_.clear();
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

void CUnionHttpProxyServer::_get_forward_client() {
	forward_host_ = config_->GetValueAsString("unionpay_forward", "forward.domain", "https://qra.95516.com");
}

bool CUnionHttpProxyServer::Work() {
	httplib::Server server_;
	server_.set_base_dir("./");

	for (auto rule = forward_rule_list_.begin(); rule != forward_rule_list_.end(); rule++) {
		SPDLOG_INFO("method:{}, {} ===> {}{}", rule->method, rule->origin_url, forward_host_, rule->target_url);

		if (rule->method.compare("POST") == 0) {
			server_.Post(rule->origin_url, [&, rule](const httplib::Request& req, httplib::Response& rsp) {
				std::string content = req.body;
				httplib::Client forward_client(forward_host_);
				forward_client.set_connection_timeout(15);
				forward_client.set_write_timeout(30);
				forward_client.set_read_timeout(15);
				httplib::Headers headers;
				auto result = forward_client.Post(
					rule->target_url,
					headers,
					content,
					req.get_header_value("Content-Type"));

				ForwardRecord record;
				record.content = content;
				record.method = "post";
				record.url = rule->origin_url;
				record.originHost = req.remote_addr;
				record.originPort = req.remote_port;
				record.forwardHost = forward_host_;
				record.forwardPort = 80;
				record.forwardUrl = rule->target_url;
				record.status = httplib::to_string(result.error());

				//response message to origin server
				if (result != nullptr) {
					rsp.set_content(result->body, "text/html; charset=utf-8");
					record.reason = result->reason;
					record.response = result->body;
					record.status = std::to_string(result->status);
				}

				databaseClient_.InsertUnionForwardRecord(record);

				std::cout
					<< DateTime::getCurrentDateTime()
					<< ",转发消息["
					<< rule->origin_url
					<< "], 到"
					<< forward_host_
					<< rule->target_url
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
					rule->target_url,
					req.get_header_value("Content-Type"),
					content,
					record.status,
					record.reason,
					record.response);
				});
		}
	}

	std::cout << "当前注册的UnionPay Forward转发url规则如下:" << std::endl;

	//server_.Walk([](const std::string method, std::string url) {
	//	std::cout << method << "  " << url << std::endl;
	//	});
	for (auto iter = forward_rule_list_.begin(); iter != forward_rule_list_.end(); iter++) {
		std::cout << iter->method << ": " << iter->origin_url << " ===> " << forward_host_ << iter->target_url << std::endl;
	}

	std::cout << std::endl;
	std::cout << std::endl;
	
	server_.Get(R"(/forward/shutdown)", [&](const httplib::Request& req, httplib::Response& rsp) {
		std::string key = req.get_param_value("key");
		std::string shutdownKey = config_->GetValueAsString("manager", "key", "");
		if (shutdownKey.compare(key) != 0) {
			SPDLOG_ERROR("非法请求关闭本地服务,key={}", key);
			rsp.set_content("操作失败,密钥不正确.", "text/html; charset=utf-8");
			return;
		}

		server_.stop();
		});

	bool isRunning = server_.listen(
		config_->GetValueAsString("unionpay_forward", "proxy.host", "localhost"),
		config_->GetValueAsUint32("unionpay_forward", "proxy.port", 9528));

	_endthreadex(0);
	return true;
}





