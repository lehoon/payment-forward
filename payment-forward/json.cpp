#include "json.h"
#include "util.h"
#include "Logger.h"

static int Value2Int(const rapidjson::Value& value) {
	if (value.IsNull())
	{
		return 0;
	}

	std::string temp_string_value = value.GetString();
	return Util::string_to_num(temp_string_value);
}

static double Value2Double(const rapidjson::Value& value) {
	if (value.IsNull())
	{
		return (double)0.0;
	}

	std::string tempValue = value.GetString();
	return Util::string_to_double(tempValue);
}

JsonString JsonDocumentToString(const rapidjson::Document& d) {
	rapidjson::StringBuffer buffer;
	rapidjson::Writer <rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	JsonString result = buffer.GetString();
	return result;
}

JsonString JsonValueToString(const rapidjson::Value& d) {
	rapidjson::StringBuffer buffer;
	rapidjson::Writer <rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	JsonString result = buffer.GetString();
	return result;
}

void JsonObject::addIntField(rapidjson::Document& d, const JsonString& k, int v) {
	rapidjson::Value k_, v_;
	k_.SetString(k.c_str(), d.GetAllocator());
	v_.SetInt(v);
	d.AddMember(k_, v_, d.GetAllocator());
}

void JsonObject::addUint32Field(rapidjson::Document& d, const JsonString& k, unsigned int v) {
	rapidjson::Value k_, v_;
	k_.SetString(k.c_str(), d.GetAllocator());
	v_.SetUint(v);
	d.AddMember(k_, v_, d.GetAllocator());
}

void JsonObject::addUint64Field(rapidjson::Document& d, const JsonString& k, unsigned long long v) {
	rapidjson::Value k_, v_;
	k_.SetString(k.c_str(), d.GetAllocator());
	v_.SetUint64(v);
	d.AddMember(k_, v_, d.GetAllocator());
}

void JsonObject::addFloatField(rapidjson::Document& d, const JsonString& k, float v) {
	rapidjson::Value k_, v_;
	k_.SetString(k.c_str(), d.GetAllocator());
	v_.SetFloat(v);
	d.AddMember(k_, v_, d.GetAllocator());
}

//Add key-Object
void JsonObject::addDoubleField(rapidjson::Document& d, const JsonString& k, double v) {
	rapidjson::Value k_, v_;
	k_.SetString(k.c_str(), d.GetAllocator());
	v_.SetDouble(v);
	d.AddMember(k_, v_, d.GetAllocator());
}

//Add key-value
void JsonObject::addStringField(rapidjson::Document& d, const JsonString& k, const JsonString& v) {
	rapidjson::Value k_, v_;
	k_.SetString(k.c_str(), d.GetAllocator());
	v_.SetString(v.c_str(), d.GetAllocator());
	d.AddMember(k_, v_, d.GetAllocator());
}

//Add key-Object
void JsonObject::addObjectField(rapidjson::Document& d, const JsonString& k, rapidjson::Value& o) {
	rapidjson::Value k_;
	k_.SetString(k.c_str(), d.GetAllocator());
	d.AddMember(k_, o, d.GetAllocator());
}

JsonString JsonObject::toJSONString(const std::map <JsonString, JsonString>& mapinfo) {
	rapidjson::Document d;
	d.SetObject();
	for (std::map<JsonString, JsonString>::const_iterator it = mapinfo.begin(); it != mapinfo.end(); it++) {
		rapidjson::Value k;
		k.SetString(it->first.c_str(), d.GetAllocator());
		rapidjson::Value v;
		v.SetString(it->second.c_str(), d.GetAllocator());
		d.AddMember(k, v, d.GetAllocator());
	}

	return JsonDocumentToString(d);
}

JsonString JsonObject::toJSONString(const httplib::Headers& header) {
	rapidjson::Document d;
	d.SetObject();
	for (std::map<JsonString, JsonString>::const_iterator it = header.begin(); it != header.end(); it++) {
		rapidjson::Value k;
		k.SetString(it->first.c_str(), d.GetAllocator());
		rapidjson::Value v;
		v.SetString(it->second.c_str(), d.GetAllocator());
		d.AddMember(k, v, d.GetAllocator());
	}

	return JsonDocumentToString(d);
}

JsonString JsonObject::toJSONString(IotPaymentRecord& record) {
	rapidjson::Document doc;
	doc.SetObject();
	addStringField(doc, "orgCode", record.orgCode);
	addStringField(doc, "channelCode", record.channelCode);
	addStringField(doc, "accountCode", record.accountCode);
	addStringField(doc, "orderCode", record.orderCode);
	addStringField(doc, "totalFee", record.totalFee);
	addStringField(doc, "orderDate", record.orderDate);
	return JsonDocumentToString(doc);
}

long JsonObject::getLong(const JsonString& jsonString, const JsonString& fieldname) {
	rapidjson::Document d;
	d.Parse(jsonString.c_str());
	rapidjson::Value& s = d[fieldname.c_str()];
	return (long)s.GetInt64();
}

int JsonObject::markRequired(const rapidjson::Document& d, const JsonString& requiredField) {
	if (!d.HasMember(requiredField.c_str())) {
		return JsonObject::LACK_JSON_FIELD;
	}

	return JsonObject::SUCCESS;
}

int JsonObject::markRequired(const rapidjson::Value& v, const JsonString& requiredField) {
	if (!v.HasMember(requiredField.c_str())) {
		return JsonObject::LACK_JSON_FIELD;
	}

	return JsonObject::SUCCESS;
}

static void string_to_upper(std::string& source) {
	std::transform(source.begin(), source.end(), source.begin(), ::toupper);
}

ForwarRuleList JsonObject::Json2ForwarRuleList(const JsonString& jsonString) {
	ForwarRuleList result;

	rapidjson::Document d;
	d.Parse(jsonString.c_str());

	if (d.HasParseError()) {
		SPDLOG_ERROR("数据不是合法的json数据");
		return result;
	}

	for (auto& v : d.GetArray()) {
		if (JsonObject::SUCCESS == markRequired(v, "origin_url") && JsonObject::SUCCESS == markRequired(v, "target_url")) {
			ForwardRuleOne rule;
			rapidjson::Value& method_value = v["method"];
			rapidjson::Value & origin_url_value = v["origin_url"];
			rapidjson::Value & target_url_value = v["target_url"];

			rule.origin_url = origin_url_value.GetString();
			rule.target_url = target_url_value.GetString();
			rule.method = method_value.GetString();
			string_to_upper(rule.method);
			result.push_back(rule);
		}
	}

	return result;
}

int JsonObject::Json2IotBaseResponseVo(const JsonString& jsonString, IotBaseResponseVo& response) {
	rapidjson::Document d;
	d.Parse(jsonString.c_str());

	if (d.HasParseError()) {
		SPDLOG_ERROR("待解析Iot响应数据不是合法的json数据");
		return INVALID_JSON_FORMAT;
	}

	if (markRequired(d, "success") != JsonObject::SUCCESS) {
		SPDLOG_ERROR("待解析Iot响应数据,缺少success参数");
		return JsonObject::LACK_JSON_FIELD;
	}

	const rapidjson::Value& success_value = d["success"];
	if (success_value.IsBool()) {
		response.success = success_value.GetBool();
	}
	else {
		response.success = false;
	}

	if (markRequired(d, "responseCode") != JsonObject::SUCCESS) {
		SPDLOG_ERROR("待解析Iot响应数据,缺少responseCode参数");
		return JsonObject::LACK_JSON_FIELD;
	}

	const rapidjson::Value& responseCode = d["responseCode"];
	response.code = Value2Int(responseCode);

	if (markRequired(d, "message") != JsonObject::SUCCESS) {
		SPDLOG_ERROR("待解析Iot响应数据,缺少msg参数");
		return JsonObject::LACK_JSON_FIELD;
	}

	const rapidjson::Value& message_value = d["message"];
	response.message = message_value.GetString();

	if (markRequired(d, "data") != JsonObject::SUCCESS) {
		SPDLOG_ERROR("待解析Iot响应数据,缺少data参数");
		return JsonObject::LACK_JSON_FIELD;
	}

	const rapidjson::Value& data_value = d["data"];
	response.data = JsonValueToString(data_value);;
	return JsonObject::SUCCESS;
}

int JsonObject::Json2IotPaymentRecord(const JsonString& jsonString, IotPaymentRecord& record) {
	rapidjson::Document d;
	d.Parse(jsonString.c_str());

	if (d.HasParseError()) {
		SPDLOG_ERROR("待解析Iot缴费数据不是合法的json数据");
		return INVALID_JSON_FORMAT;
	}

	if (markRequired(d, "orderCode") != JsonObject::SUCCESS) {
		SPDLOG_ERROR("待解析Iot缴费数据,缺少orderCode参数");
		return JsonObject::LACK_JSON_FIELD;
	}

	const rapidjson::Value& orderCode = d["orderCode"];
	record.orderCode = orderCode.GetString();

	if (markRequired(d, "orgCode") != JsonObject::SUCCESS) {
		SPDLOG_ERROR("待解析Iot响应数据,缺少orgCode参数");
		return JsonObject::LACK_JSON_FIELD;
	}

	const rapidjson::Value& orgCode = d["orgCode"];
	record.orgCode = orgCode.GetString();

	if (markRequired(d, "channelCode") != JsonObject::SUCCESS) {
		SPDLOG_ERROR("待解析Iot响应数据,缺少channelCode参数");
		return JsonObject::LACK_JSON_FIELD;
	}

	const rapidjson::Value& channelCode = d["channelCode"];
	record.channelCode = channelCode.GetString();

	if (markRequired(d, "accountCode") != JsonObject::SUCCESS) {
		SPDLOG_ERROR("待解析Iot响应数据,缺少accountCode参数");
		return JsonObject::LACK_JSON_FIELD;
	}

	const rapidjson::Value& accountCode = d["accountCode"];
	record.accountCode = accountCode.GetString();

	if (markRequired(d, "totalFee") != JsonObject::SUCCESS) {
		SPDLOG_ERROR("待解析Iot响应数据,缺少totalFee参数");
		return JsonObject::LACK_JSON_FIELD;
	}

	const rapidjson::Value& totalFee = d["totalFee"];
	record.totalFee = totalFee.GetString();

	if (markRequired(d, "orderDate") != JsonObject::SUCCESS) {
		SPDLOG_ERROR("待解析Iot响应数据,缺少orderDate参数");
		return JsonObject::LACK_JSON_FIELD;
	}

	const rapidjson::Value& orderDate = d["orderDate"];
	record.orderDate = orderDate.GetString();
	return JsonObject::SUCCESS;
}