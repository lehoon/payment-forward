#include "Logger.h"
#include "json.h"
#include "json_object.h"
#include "NotifyHttpProxyServer.h"


CNotifyHttpProxyServer::CNotifyHttpProxyServer(CConfigure* config) : config_(config) {
}

CNotifyHttpProxyServer::~CNotifyHttpProxyServer() {
	if (NULL != threadHandle_) {
		CloseHandle(threadHandle_);
	}
}

bool CNotifyHttpProxyServer::InitTask() {
	_get_forward_client();
	std::string forward_rule_string = config_->GetValueAsString("notify_forward", "forward.rule", "[]");
	ForwarRuleList ruleList = JsonObject::Json2ForwarRuleList(forward_rule_string);
	forward_rule_list_.swap(ruleList);
	databaseClient_.Open();
	return true;
}

bool CNotifyHttpProxyServer::ExitTask() {
	server_.stop();
	return true;
}

bool CNotifyHttpProxyServer::StartTask() {
	threadHandle_ = (HANDLE)_beginthreadex(NULL, 0,
		CNotifyHttpProxyServer::threadFunc, (void*)this, 0, &threadId_);
	return threadId_ != 0;
}

unsigned int CNotifyHttpProxyServer::threadFunc(void* p) {
	CNotifyHttpProxyServer* threadObj = (CNotifyHttpProxyServer*)p;
	return threadObj->Work();
}

void CNotifyHttpProxyServer::_get_forward_client() {
	//domain.com
	std::string protocol = config_->GetValueAsString("notify_forward", "forward.protocol", "http");
	forward_host_ = protocol + "://" + config_->GetValueAsString("notify_forward", "forward.host", "localhost") + ":" +
		std::to_string(config_->GetValueAsUint32("notify_forward", "forward.port", 80));
}

bool CNotifyHttpProxyServer::Work() {
	server_.set_base_dir("./");

	for (auto rule = forward_rule_list_.begin(); rule != forward_rule_list_.end(); rule++) {
		std::cout << rule->method << "," << rule->origin_url << "," << rule->target_url << std::endl;
		if (rule->method.compare("GET") == 0) {
			server_.Get(rule->origin_url, [&, rule](const httplib::Request& req, httplib::Response& rsp) {
				std::string content = req.body;

				httplib::Client forward_client(
					config_->GetValueAsString("notify_forward", "forward.host", "localhost"),
					config_->GetValueAsUint32("notify_forward", "forward.port", 80)
				);

				forward_client.set_connection_timeout(15);
				forward_client.set_write_timeout(30);
				forward_client.set_read_timeout(15);

				auto result = forward_client.Get(rule->target_url);

				ForwardRecord record;
				record.content = content;
				record.method = rule->method;
				record.url = rule->origin_url;
				record.originHost = req.remote_addr;
				record.originPort = req.remote_port;
				record.forwardHost = config_->GetValueAsString("notify_forward", "forward.host", "localhost");
				record.forwardPort = config_->GetValueAsUint32("notify_forward", "forward.port", 80);
				record.forwardUrl = rule->target_url;
				record.status = httplib::to_string(result.error());

				//response message to origin server
				if (result != nullptr) {
					rsp.set_content(result->body, "text/html; charset=utf-8");
					record.reason = result->reason;
					record.response = result->body;
					record.status = std::to_string(result->status);
				}

				databaseClient_.InsertNotifyForwardRecord(record);

				std::cout << "ת��Get��Ϣ[" << rule->origin_url
					<< "], ��"
					<< forward_host_
					<< rule->target_url
					<< ", ��������:"
					<< req.get_header_value("Content-Type")
					<< ", ��Ϣ����:"
					<< content
					<< ",ת��״̬:"
					<< record.status
					<< ",ת��״̬����:"
					<< record.reason
					<< ",ת����Ӧ����:"
					<< record.response
					<< std::endl;

				SPDLOG_INFO("ת����Ϣ[] [{}],��{}/{}, ��������:{}, ��Ϣ����:{},ת��״̬:{},ת��״̬����:{},ת����Ӧ����:{}",
					rule->origin_url,
					forward_host_,
					rule->target_url,
					req.get_header_value("Content-Type"),
					content,
					record.status,
					record.reason,
					record.response);
				});
		}
		else if (rule->method.compare("POST") == 0) {
			server_.Post(rule->origin_url, [&,rule](const httplib::Request& req, httplib::Response& rsp) {
				std::string content = req.body;
				httplib::Client forward_client(
					config_->GetValueAsString("notify_forward", "forward.host", "localhost"),
					config_->GetValueAsUint32("notify_forward", "forward.port", 80)
				);
				forward_client.set_connection_timeout(15);
				forward_client.set_write_timeout(30);
				forward_client.set_read_timeout(15);

				auto result = forward_client.Post(
					rule->target_url,
					content,
					req.get_header_value("Content-Type"));

				ForwardRecord record;
				record.content = content;
				record.method = rule->method;
				record.url = rule->origin_url;
				record.originHost = req.remote_addr;
				record.originPort = req.remote_port;
				record.forwardHost = config_->GetValueAsString("notify_forward", "forward.host", "localhost");
				record.forwardPort = config_->GetValueAsUint32("notify_forward", "forward.port", 80);
				record.forwardUrl = rule->target_url;
				record.status = httplib::to_string(result.error());

				//response message to origin server
				if (result != nullptr) {
					rsp.set_content(result->body, "text/html; charset=utf-8");
					record.reason = result->reason;
					record.response = result->body;
					record.status = std::to_string(result->status);
				}

				databaseClient_.InsertNotifyForwardRecord(record);

				std::cout << "ת��Post��Ϣ[" << rule->origin_url
					<< "], ��"
					<< forward_host_
					<< rule->target_url
					<< ", ��������:"
					<< req.get_header_value("Content-Type")
					<< ", ��Ϣ����:"
					<< content
					<< ",ת��״̬:"
					<< record.status
					<< ",ת��״̬����:"
					<< record.reason
					<< ",ת����Ӧ����:"
					<< record.response
					<< std::endl;

				SPDLOG_INFO("ת��[{}]��Ϣ��{}{}, ��������:{}, ��Ϣ����:{},ת��״̬:{},ת��״̬����:{},ת����Ӧ����:{}",
					rule->method,
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

	std::cout << "��ǰע���Notify Forwardת��url��������:" << std::endl;
	server_.Walk([](const std::string method, std::string url) {
		std::cout << method << "  " << url << std::endl;
		});

	for (auto iter = forward_rule_list_.begin(); iter != forward_rule_list_.end(); iter++) {
		std::cout << iter->method << ": " << iter->origin_url << " ===> " << iter->target_url << std::endl;
	}

	server_.listen(
		config_->GetValueAsString("notify_proxy_server", "proxy.host", "localhost"),
		config_->GetValueAsUint32("notify_proxy_server", "proxy.port", 9527));

	ExitTask();
	_endthreadex(0);
	return true;
}
