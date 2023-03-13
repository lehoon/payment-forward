#include <list>
#include <string>
#include "json.h"
#include "json_object.h"
#include "json_string.h"


std::string IotPaymentRecord::toString() {
	return JsonObject::toJSONString(*this);
}
