#ifndef _PAYMENT_IOT_HTTP_CLIENT_H_
#define _PAYMENT_IOT_HTTP_CLIENT_H_

#include "httplib.h"
#include "Configure.h"
#include "json_object.h"

class IotPayHttpClient;

///////////////////////////////////IotBaseRequest////////////////////////////////////////////////////////////////
class IotBaseRequest {
public:
	IotBaseRequest();
	virtual ~IotBaseRequest();
	friend class IotPayHttpClient;

protected:
	void set_secret(const std::string);
	void set_appKey(const std::string);
	void set_version(const std::string);
	void set_public_key(const std::string);

public:
	virtual httplib::Headers Header();
	virtual std::string Body() = 0;
	virtual std::string OrderCode();

protected:
	std::string _gen_order_code();
	std::string _encode_rsa(std::string&);
	void _set_sign_param(std::string key, std::string value);

private:
	std::string _md5_sign();

protected:
	std::string _time_stamp;
	std::string _app_key;
	std::string _secret;
	std::string _version;
	std::string _rsa_public_key;
	std::string _account_no;
	std::map<std::string, std::string> _sign_param_map;
};

///////////////////////////////////IotBaseResponse//////////////////////////////////////////////////////////////////////////
class IotBaseResponse {
public:
	IotBaseResponse();
	virtual ~IotBaseResponse();
	friend class IotPayHttpClient;

public:
	bool Success() const { return _base_response.success; }
	int ResponseCode() const { return _base_response.code; }
	std::string Message() const { return _base_response.message; }

protected:
	void _set_response_data(std::string&);
	std::string _decode_rsa(std::string&);
	virtual bool _decode_json(std::string&) = 0;

protected:
	std::string _rsa_public_key;
	IotBaseResponseVo _base_response;
};

///////////////////////////////////IotUploadFileRequest/////////////////////////////////////////////////////////////////////
class IotUploadFileRequest : public IotBaseRequest {
public:
	IotUploadFileRequest();
	IotUploadFileRequest(std::string&);
	~IotUploadFileRequest();

public:
	virtual std::string Body() override;

private:
	std::string _file_name;
};

class IotUploadFileResponse : public IotBaseResponse {
public:
	IotUploadFileResponse();
	~IotUploadFileResponse();

public:
	bool _decode_json(std::string&) { return true; }
};

///////////////////////////////////IotPayHttpClient/////////////////////////////////////////////////////////////////////////
class IotPayHttpClient {
public:
	IotPayHttpClient();
	IotPayHttpClient(CConfigure* _config);
	~IotPayHttpClient();

public:
	void set_remote_host(const std::string& remote_host);
	void set_remote_port(const unsigned short remote_port);

public:
	bool InvokeUploadFile(IotUploadFileRequest&, IotUploadFileResponse&);

private:
	std::string _remote_host;
	unsigned short _remote_port; 
	CConfigure *_config;
};

#endif // _PAYMENT_IOT_HTTP_CLIENT_H_