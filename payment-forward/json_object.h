#ifndef GATEWAY_JSON_OBJECT_H_
#define GATEWAY_JSON_OBJECT_H_


#include <list>
#include <string>
#include <unordered_map>


class ForwardRuleOne {
public:
	std::string method;
	std::string origin_url;
	std::string target_url;
};


class IotBaseResponseVo {
public:
	bool success;
	int  code;
	std::string message;
	std::string data;
};

class IotPaymentRecord {
public:
	std::string orderCode;
	std::string orgCode;
	std::string channelCode;
	std::string accountCode;
	std::string totalFee;
	std::string orderDate;

public:
	std::string toString();
};


typedef std::list<ForwardRuleOne> ForwarRuleList;
typedef std::unordered_map<std::string, ForwardRuleOne> ForwardRule;
#endif /* GATEWAY_JSON_OBJECT_H_ */

