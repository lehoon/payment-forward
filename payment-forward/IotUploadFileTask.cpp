#include "IotPayHttpClient.h"
#include "Logger.h"
#include "IotUploadFileTask.h"
#include "util.h"
#include "DateTime.h"
#include "DatabaseClient.h"
#include "libxl/libxl.h"


IotUploadFileTask::IotUploadFileTask(CConfigure* config) : _config(config) {

}

IotUploadFileTask::~IotUploadFileTask() {

}

bool IotUploadFileTask::InitTask() {
	if (_config == nullptr) {
		SPDLOG_ERROR("�����ļ�������Ч,��������δ�ɹ�����");
		return false;
	}

	if (!_database_client.Open()) {
		SPDLOG_ERROR("���ݿ��ʧ��,��������δ�ɹ�����");
		return false;
	}

	return true;
}

bool IotUploadFileTask::ExitTask() {
	return true;
}

bool IotUploadFileTask::Work() {
	Sleep(3000);

	DateTime nowDateTime;

	if (nowDateTime.Hour() >= 5) {
		return true;
	}

	std::string current_datetime = DateTime::getYearMonthDay();
	std::string begin_datatime = current_datetime, end_datetime = current_datetime;
	begin_datatime.append(" 00:00:00");
	end_datetime.append(" 23:59:59");
	//query record from database
	std::vector<IotPaymentRecord> recordVec = _database_client.QueryIotPaymentRecord(begin_datatime, end_datetime);
	if (recordVec.size() == 0) return true;

	libxl::Book* book = xlCreateXMLBook();
	if (!book) {
		SPDLOG_ERROR("����xls����ʧ��, {}, {}", begin_datatime, end_datetime);
		return true;
	}

	std::string file_name = ".\\upload\\ZK_";
	file_name.append(_config->GetValueAsString("iot", "channel_code", "channel_code"));
	file_name.append(DateTime::get8CurrentTime());

	std::wstring sheet_name = Util::Char2WChar(file_name.c_str());
	libxl::Sheet* sheet = book->addSheet(sheet_name.c_str());
	file_name.append(".xls");

	if (!sheet) {
		book->release();
		return true;
	}

	sheet->writeStr(0, 0, L"ȼ����˾����");
	sheet->writeStr(0, 1, L"��������");
	sheet->writeStr(0, 2, L"������ˮ��");
	sheet->writeStr(0, 3, L"�û��ɷѺ�");
	sheet->writeStr(0, 4, L"���׽��");
	sheet->writeStr(0, 5, L"����ʱ��");

	int index = 1;

	for (auto record : recordVec) {
		sheet->writeStr(index, 0, Util::Char2WChar(record.orgCode.c_str()).c_str());
		sheet->writeStr(index, 1, Util::Char2WChar(record.channelCode.c_str()).c_str());
		sheet->writeStr(index, 2, Util::Char2WChar(record.orderCode.c_str()).c_str());
		sheet->writeStr(index, 3, Util::Char2WChar(record.accountCode.c_str()).c_str());
		sheet->writeStr(index, 4, Util::Char2WChar(record.totalFee.c_str()).c_str());
		sheet->writeStr(index, 5, Util::Char2WChar(record.orderDate.c_str()).c_str());
		index += 1;
	}

	if (!book->save(Util::Char2WChar(file_name.c_str()).c_str())) {
		book->release();
		return true;
	}

	//upload file
	IotPayHttpClient http_client;
	http_client.set_remote_host(_config->GetValueAsString("iot_server", "host", "localhost"));
	http_client.set_remote_port(_config->GetValueAsUint32("iot_server", "port", 9529));
	IotUploadFileRequest request(file_name);
	IotUploadFileResponse response;

	if (!http_client.InvokeUploadFile(request, response)) {
		//SPDLOG_INFO("�����ļ�������Ч,��������δ�ɹ�����");
		return true;
	}

	_database_client.DeleteIotPaymentRecord();
	return true;
}

