#ifndef __GATEWAY_JSON_STRING_H_
#define __GATEWAY_JSON_STRING_H_

#include <string>
#include <sstream>

#define JsonString std::string
#define NULLSTR JsonStringOps::nullstr
#define isNull JsonStringOps::isNullStr

class JsonStringOps {
public:
	static const JsonString nullstr;

	static bool isNullStr(const JsonString& str);
	static JsonString valueOf(bool val);

	static const JsonString TRUES;
	static const JsonString FALSES;
};

#endif  /**__GATEWAY_JSON_STRING_H_*/
