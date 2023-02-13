
#include "DatabaseClient.h"
#include "Logger.h"


const static std::string DDL_CREATE_TABLE_FORWARD_RECORD_SQL = "CREATE TABLE IF NOT EXISTS tb_forward_record("
"[method]							VARCHAR(32),"
"[url]								VARCHAR(32),"
"[origin_host]						VARCHAR(32),"
"[origin_port]						INTEGER,"
"[content]							VARCHAR(2048),"
"[forward_host]						VARCHAR(32),"
"[forward_port]						INTEGER,"
"[forward_url]						VARCHAR(32),"
"[status]							VARCHAR(32),"
"[response]							VARCHAR(2048),"
"[reason]							VARCHAR(128),"
"[created]                          DATE NOT NULL);";

const static std::string DML_INSERT_TABLE_FORWARD_RECORD_SQL = "INSERT INTO tb_forward_record "
"(method,url,origin_host,origin_port,content,forward_host,forward_port,forward_url,status,response,reason,created) "
"VALUES(?,?,?,?,?,?,?,?,?,?,?,datetime('now','localtime'))";

CDatabaseClient::CDatabaseClient() : mDbName("storage.db"), mSqliteDb(new CppSQLite3DB()) {

}

CDatabaseClient::CDatabaseClient(std::string dbName) : mDbName(dbName), mSqliteDb(new CppSQLite3DB()) {

}

CDatabaseClient::~CDatabaseClient() {
	if (mSqliteDb) {
		mSqliteDb->close();
		delete mSqliteDb;
	}
}

bool CDatabaseClient::Open() {
	try {
		mSqliteDb->open(mDbName.c_str());
		mSqliteDb->execDML(DDL_CREATE_TABLE_FORWARD_RECORD_SQL.c_str());
	}
	catch (CppSQLite3Exception& sqliteException) {
		spdlog::error("打开数据库失败,{}", sqliteException.errorMessage());
		return false;
	}
	catch (...) {
		spdlog::error("打开数据库失败");
		return true;
	}

	return true;
}


bool CDatabaseClient::InsertForwardRecord(ForwardRecord& record) {
	try
	{
		CppSQLite3Statement stmt = mSqliteDb->compileStatement(DML_INSERT_TABLE_FORWARD_RECORD_SQL.c_str());
		stmt.bind(1, record.method.c_str());
		stmt.bind(2, record.url.c_str());
		stmt.bind(3, record.originHost.c_str());
		stmt.bind(4, record.originPort);
		stmt.bind(5, record.content.c_str());
		stmt.bind(6, record.forwardHost.c_str());
		stmt.bind(7, record.forwardPort);
		stmt.bind(8, record.forwardUrl.c_str());
		stmt.bind(9, record.status.c_str());
		stmt.bind(10, record.response.c_str());
		stmt.bind(11, record.reason.c_str());
		stmt.execDML();
	}
	catch (CppSQLite3Exception& sqliteException)
	{
		spdlog::error("添加forward数据失败,{}", sqliteException.errorMessage());
		return false;
	}
	catch (...)
	{
		spdlog::error("添加forward数据失败");
		return false;
	}

	return true;
}




