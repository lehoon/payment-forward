#include "httplib.h"
#include "Logger.h"
#include "json.h"
#include "DateTime.h"
#include "json_object.h"
#include "IotHttpProxyServer.h"

CIotHttpProxyServer::CIotHttpProxyServer(CConfigure* config) : config_(config) {
}

CIotHttpProxyServer::~CIotHttpProxyServer() {
	if (NULL != threadHandle_) {
		CloseHandle(threadHandle_);
	}
}

bool CIotHttpProxyServer::InitTask() {
	_get_forward_client();
	std::string forward_rule_string = config_->GetValueAsString("iot_forward", "forward.rule", "[]");
	ForwarRuleList ruleList = JsonObject::Json2ForwarRuleList(forward_rule_string);

	if (ruleList.empty()) {
		SPDLOG_INFO("δ����ת������,��������˳�");
		return false;
	}

	forward_rule_list_.swap(ruleList);
	databaseClient_.Open();
	return true;
}

bool CIotHttpProxyServer::ExitTask() {
	forward_rule_list_.clear();
	return true;
}

bool CIotHttpProxyServer::StartTask() {
	threadHandle_ = (HANDLE)_beginthreadex(NULL, 0,
		CIotHttpProxyServer::threadFunc, (void*)this, 0, &threadId_);
	return threadId_ != 0;
}

unsigned int CIotHttpProxyServer::threadFunc(void* p) {
	CIotHttpProxyServer* threadObj = (CIotHttpProxyServer*)p;
	return threadObj->Work();
}

void CIotHttpProxyServer::_get_forward_client() {
	forward_host_ = config_->GetValueAsString("iot_forward", "forward.domain", "");
}

bool CIotHttpProxyServer::Work() {
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
				auto result = forward_client.Post(
					rule->target_url,
					req.headers,
					content,
					req.get_header_value("Content-Type"));

				ForwardRecord record;
				record.content = content;
				record.method = "post";
				record.header = JsonObject::toJSONString(req.headers);
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

				databaseClient_.InsertIotForwardRecord(record);

				std::cout
					<< DateTime::getCurrentDateTime()
					<< ",ת����Ϣ["
					<< rule->origin_url
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

	std::cout << "��ǰע���iot Forwardת��url��������:" << std::endl;

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
			SPDLOG_ERROR("�Ƿ�����رձ��ط���,key={}", key);
			rsp.set_content("����ʧ��,��Կ����ȷ.", "text/html; charset=utf-8");
			return;
		}

		server_.stop();
		});

	bool isRunning = server_.listen(
		config_->GetValueAsString("iot_forward", "proxy.host", "localhost"),
		config_->GetValueAsUint32("iot_forward", "proxy.port", 9529));

	_endthreadex(0);
	return true;
}





