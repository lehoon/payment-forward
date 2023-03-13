#include "IotPayHttpClient.h"

#include "rsa.h"
#include "Md5.h"
#include "json.h"
#include "util.h"
#include "base64.h"
#include "Logger.h"
#include "Configure.h"
#include "DateTime.h"


///////////////////////////////////IotBaseRequest////////////////////////////////////////////////////////////////
IotBaseRequest::IotBaseRequest() {
	//获取当前时间 毫秒
	_time_stamp = DateTime::getCurrentMilliSecond();
}

IotBaseRequest::~IotBaseRequest() {

}

httplib::Headers IotBaseRequest::Header() {
	httplib::Headers header;
	std::string _sign = _md5_sign();
	header.emplace("sign", _sign);
	header.emplace("appKey", _app_key);
	header.emplace("version", _version);
	//header.emplace("time_stamp", _time_stamp);
	header.emplace("Content-Type", "application/json");
	return header;
}

std::string IotBaseRequest::Body() {
	return std::string("");
}

std::string IotBaseRequest::_encode_rsa(std::string& in) {
	SPDLOG_INFO("rsa加密,origin={}", in);
	std::string encrypt_text = security::rsa_public_key_encrypt(_rsa_public_key, in);
	return httplib::detail::base64_encode(encrypt_text);
}

std::string IotBaseRequest::OrderCode() {
	return std::string("");
}

std::string IotBaseRequest::_md5_sign() {
	std::string content(_secret);

	for (auto iter = _sign_param_map.begin(); iter != _sign_param_map.end(); ++iter) {
		if (0 == iter->second.length()) {
			continue;
		}
		content.append(iter->first);
		content.append("=");
		content.append(iter->second);
		content.append("&");
	}

	std::string sign_content = content.substr(0, content.length() - 1);
	std::string md5 = Util::str_toupper(CMd5::md5(sign_content));
	SPDLOG_INFO("IotBaseRequest计算md5值={},md5={}", sign_content, md5);
	return md5;
}

void IotBaseRequest::_set_sign_param(std::string key, std::string value) {
	_sign_param_map.emplace(key, value);
}

void IotBaseRequest::set_secret(const std::string secret) {
	_secret = secret;
}

void IotBaseRequest::set_appKey(const std::string appKey) {
	_app_key = appKey;
}

void IotBaseRequest::set_version(const std::string version) {
	_version = version;
}

void IotBaseRequest::set_public_key(const std::string publicKey) {
	_rsa_public_key = publicKey;
}

///////////////////////////////////IotBaseResponse////////////////////////////////////////////////////////////////
IotBaseResponse::IotBaseResponse() {
	_base_response.success = false;
	_base_response.code = -1;
	_base_response.message = "未设置响应数据";
}

IotBaseResponse::~IotBaseResponse() {
}

std::string IotBaseResponse::_decode_rsa(std::string& rsa_data) {
	std::string encrypted_data = base64_decode(rsa_data);
	_base_response.data = security::rsa_public_key_decrypt(_rsa_public_key, encrypted_data);
	if (_base_response.data.length() == 0) {
		SPDLOG_ERROR("rsa解密失败, {}", encrypted_data);
	}

	if (!_decode_json(_base_response.data)) {
		SPDLOG_ERROR("json转对象失败, {}", _base_response.data);
	}

	return std::string("");
}

void IotBaseResponse::_set_response_data(std::string& response) {
	if(JsonObject::SUCCESS != JsonObject::Json2IotBaseResponseVo(response, _base_response)) {
		_base_response.success = false;
		_base_response.message = "错误的响应报文,转换json对象失败.";
		return;
	}

	if (_base_response.code == 1001) {
		SPDLOG_INFO("responseCode = 1001,开始进行rsa解密");
		_decode_rsa(_base_response.data);
		return;
	}

	SPDLOG_ERROR("responseCode={}, message={}", _base_response.code, _base_response.message);
}

///////////////////////////////////IotUploadFileRequest/////////////////////////////////////////////////////////////////////
IotUploadFileRequest::IotUploadFileRequest(){

}

IotUploadFileRequest::IotUploadFileRequest(std::string& file_name) : _file_name(file_name) {

}

IotUploadFileRequest::~IotUploadFileRequest() {

}

std::string IotUploadFileRequest::Body() {
	return _file_name;
}

IotUploadFileResponse::IotUploadFileResponse() {

}

IotUploadFileResponse::~IotUploadFileResponse() {

}


///////////////////////////////////IotPayHttpClient/////////////////////////////////////////////////////////////////////////
IotPayHttpClient::IotPayHttpClient() {

}

IotPayHttpClient::IotPayHttpClient(CConfigure* config) : _config(config) {

}

IotPayHttpClient::~IotPayHttpClient() {

}

void IotPayHttpClient::set_remote_host(const std::string& remote_host) {
	_remote_host = remote_host;
}

void IotPayHttpClient::set_remote_port(const unsigned short remote_port) {
	_remote_port = remote_port;
}

bool IotPayHttpClient::InvokeUploadFile(IotUploadFileRequest& request, IotUploadFileResponse& response) {
	httplib::Client client(_remote_host, _remote_port);
	client.set_connection_timeout(10);
	client.set_read_timeout(10);
	client.set_write_timeout(10);

	//获取header 所需参数
	std::string app_key = _config->GetValueAsString("iot", "app_key", "");
	std::string version = _config->GetValueAsString("iot", "version", "");
	std::string secret = _config->GetValueAsString("iot", "secret", "");
	std::string rsa_public_key = _config->GetValueAsString("iot", "rsa_public_key", "");
	std::string request_url = _config->GetValueAsString("iot", "upload_file_url", "");

	request.set_appKey(app_key);
	request.set_version(version);
	request.set_secret(secret);
	request.set_public_key(rsa_public_key);

	std::string content = request.Body();
	SPDLOG_INFO("上传对账文件请求数据:{}", content);

	std::ifstream stream(content);
	std::stringstream buffer_stream;
	buffer_stream << stream.rdbuf();

	httplib::MultipartFormDataItems items = {
		{"file", buffer_stream.str(), request.Body(), "application/octet-stream"}
	};

	httplib::Headers headers = request.Header();
	auto result = client.Post(request_url, headers, items);

	if (!result) {
		return false;
	}

	SPDLOG_INFO("缴费响应数据:{}", result->body);
	response._set_response_data(result->body);
	return true;
}












