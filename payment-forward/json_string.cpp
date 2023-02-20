#include <math.h>
#include <stdio.h>
#include <sstream>
#include "json_string.h"

const JsonString JsonStringOps::nullstr = "";
const JsonString JsonStringOps::TRUES = "true";
const JsonString JsonStringOps::FALSES = "false";

JsonString JsonStringOps::valueOf(bool val) {
	if (val) { return TRUES; }
	else { return FALSES; }
}

//Returns true if str refers to nullstr
bool JsonStringOps::isNullStr(const JsonString& str) {
	return (&str == &nullstr) || (str == "");
}
