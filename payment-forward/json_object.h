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


typedef std::list<ForwardRuleOne> ForwarRuleList;
typedef std::unordered_map<std::string, ForwardRuleOne> ForwardRule;
#endif /* GATEWAY_JSON_OBJECT_H_ */

