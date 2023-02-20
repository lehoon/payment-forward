// notify-forward.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <signal.h>

#include "Logger.h"
#include "semaphore.h"
#include "Configure.h"
#include "NotifyHttpClient.h"
#include "NotifyHttpProxyServer.h"
#include "UnionHttpProxyServer.h"

int main()
{
	{
		CConfigure config;
		if (config.Init("./config.ini")) {
			std::cout << "当前目录下未找到配置文件,程序退出." << std::endl;
			return -1;
		}

		CLogger::Init();
		CNotifyHttpProxyServer notifyHttpProxyServer(&config);
		notifyHttpProxyServer.InitTask();
		if (!notifyHttpProxyServer.StartTask()) {
			SPDLOG_ERROR("notify http proxy server启动失败.");
			std::cout << "notify http server启动失败." << std::endl;
		}

		CUnionHttpProxyServer unionHttpProxyServer(&config);
		unionHttpProxyServer.InitTask();
		if (!unionHttpProxyServer.StartTask()) {
			SPDLOG_ERROR("union pay http proxy server启动失败.");
			std::cout << "union pay http server启动失败." << std::endl;
		}

		static base::semaphore sem;
		signal(SIGINT, [](int) { sem.post(); });
		sem.wait();

		unionHttpProxyServer.ExitTask();
		notifyHttpProxyServer.ExitTask();
		CLogger::Close();
	}

	std::cout << "程序退出." << std::endl;
}
