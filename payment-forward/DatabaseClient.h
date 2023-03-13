#pragma once
#include "CppSQLite3.h"
#include "json_object.h"
#include <string>

class CppSQLite3DB;

typedef struct _ForwardRecordTag {
	std::string method;
	std::string url;
	std::string originHost;
	int originPort;
	std::string header;
	std::string content;
	std::string forwardHost;
	int forwardPort;
	std::string forwardUrl;
	std::string status;
	std::string response;
	std::string reason;
}ForwardRecord, *PForwardRecord;

class CDatabaseClient
{
public:
	CDatabaseClient();
	CDatabaseClient(std::string dbName);
	~CDatabaseClient();

public:
	bool Open();
	bool InsertNotifyForwardRecord(ForwardRecord&);
	bool InsertUnionForwardRecord(ForwardRecord&);
	bool InsertIotForwardRecord(ForwardRecord&);


	////////////////////////////////////////////////////iot///////////////////////////////////////////////
	bool SubmitIotPaymentRecord(IotPaymentRecord&);
	bool DeleteIotPaymentRecord();
	std::vector<IotPaymentRecord> QueryIotPaymentRecord(std::string&, std::string&);


private:
	std::string mDbName;
	CppSQLite3DB* mSqliteDb;
};

