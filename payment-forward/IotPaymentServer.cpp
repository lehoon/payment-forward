#include "httplib.h"
#include "Logger.h"
#include "json.h"
#include "DateTime.h"
#include "json_object.h"
#include "IotPaymentServer.h"

CIotPaymentHttpServer::CIotPaymentHttpServer(CConfigure* config) : config_(config), threadHandle_(nullptr), threadId_(0) {
}

CIotPaymentHttpServer::~CIotPaymentHttpServer() {
	if (NULL != threadHandle_) {
		CloseHandle(threadHandle_);
	}
}

bool CIotPaymentHttpServer::InitTask() {
	databaseClient_.Open();
	return true;
}

bool CIotPaymentHttpServer::ExitTask() {
	return true;
}

bool CIotPaymentHttpServer::StartTask() {
	threadHandle_ = (HANDLE)_beginthreadex(NULL, 0,
		CIotPaymentHttpServer::threadFunc, (void*)this, 0, &threadId_);
	return threadId_ != 0;
}

unsigned int CIotPaymentHttpServer::threadFunc(void* p) {
	CIotPaymentHttpServer* threadObj = (CIotPaymentHttpServer*)p;
	return threadObj->Work();
}

void CIotPaymentHttpServer::_get_forward_client() {
}

bool CIotPaymentHttpServer::Work() {
	httplib::Server server_;
	server_.set_base_dir("./");

	server_.Post("/iot/order/sync", [&](const httplib::Request& req, httplib::Response& rsp) {
		std::string content = req.body;
		IotPaymentRecord record;
		if (JsonObject::Json2IotPaymentRecord(content, record) != JsonObject::SUCCESS) {
			return;
		}
		databaseClient_.SubmitIotPaymentRecord(record);
		});

	std::cout << "当前注册的iot Forward转发url规则如下:" << std::endl;
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
		config_->GetValueAsString("iot_server", "host", "localhost"),
		config_->GetValueAsUint32("iot_server", "port", 9530));

	_endthreadex(0);
	return true;
}