#ifndef GATEWAY_JSON_H_
#define GATEWAY_JSON_H_

#include <map>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "json_string.h"
#include "json_object.h"

#include "httplib.h"

class JsonObject {
private:
	JsonObject() {}
	~JsonObject() {}
public:
	static JsonString toJSONString(const std::map <JsonString, JsonString>& mapinfo);
	static JsonString toJSONString(const httplib::Headers & header);
	static JsonString toJSONString(IotPaymentRecord&);
	static long getLong(const JsonString& jsonString, const JsonString& fieldname);

	static int markRequired(const rapidjson::Document& d, const JsonString& requiredField);
	static int markRequired(const rapidjson::Value& v, const JsonString& requiredField);

public:
public:
	static ForwarRuleList Json2ForwarRuleList(const JsonString&);
	static int Json2IotBaseResponseVo(const JsonString&, IotBaseResponseVo&);
	static int Json2IotPaymentRecord(const JsonString&, IotPaymentRecord&);

private:
	static void addStringField(rapidjson::Document& d, const JsonString& k, const JsonString& v);
	static void addObjectField(rapidjson::Document& d, const JsonString& k, rapidjson::Value& o);
	static void addFloatField(rapidjson::Document& d, const JsonString& k, float v);
	static void addDoubleField(rapidjson::Document& d, const JsonString& k, double v);
	static void addIntField(rapidjson::Document& d, const JsonString& k, int v);
	static void addUint32Field(rapidjson::Document& d, const JsonString& k, unsigned int v);
	static void addUint64Field(rapidjson::Document& d, const JsonString& k, unsigned long long v);

public:
public:
	/**invalid param（参数错误）*/
	static const int INVALID_PARAM = 400;
	/**invalid json*/
	static const int INVALID_JSON_FORMAT = 100001;
	/**缺少字段*/
	static const int LACK_JSON_FIELD = 100002;
	/**success*/
	static const int SUCCESS = 0;
};
#endif /* GATEWAY_JSON_H_ */

