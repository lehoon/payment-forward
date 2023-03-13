#include "json.h"
#include "Logger.h"

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