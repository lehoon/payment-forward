#include "Logger.h"

#include <spdlog/sinks/daily_file_sink.h>


CLogger::CLogger() {
}

CLogger::~CLogger() {
}

bool CLogger::Init() {
	auto logger = spdlog::daily_logger_mt("daily_logger", "logs/forward.log", 2, 30);
	spdlog::set_pattern("[%Y-%m-%d %T][thread %t][%l]%v");
	spdlog::set_default_logger(logger);
	spdlog::set_level(spdlog::level::debug);
	spdlog::flush_on(spdlog::level::info);
	spdlog::flush_every(std::chrono::seconds(1));
	return true;
}

bool CLogger::Close() {
	spdlog::drop_all();
	spdlog::shutdown();
	return true;
}