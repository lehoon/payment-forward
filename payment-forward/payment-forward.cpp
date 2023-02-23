// notify-forward.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <signal.h>

#include "Logger.h"
#include "semaphore.h"
#include "Configure.h"
#include "socketutil.h"
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
		config.Dump2LogFile();

		CNotifyHttpProxyServer notifyHttpProxyServer(&config);
		if (!notifyHttpProxyServer.InitTask()) {
			SPDLOG_ERROR("消息代理程序未定义规则,未启动代理程序");
		}
		else {
			if (!notifyHttpProxyServer.StartTask()) {
				SPDLOG_ERROR("notify http proxy server启动失败.");
				std::cout << "notify http server启动失败." << std::endl;
			}
		}

		SPDLOG_INFO("启动银联通知转发服务成功");
		Sleep(3000);

		CUnionHttpProxyServer unionHttpProxyServer(&config);
		if (!unionHttpProxyServer.InitTask()) {
			SPDLOG_ERROR("消息代理程序未定义规则,未启动代理程序");
		}
		else {
			if (!unionHttpProxyServer.StartTask()) {
				SPDLOG_ERROR("union pay http proxy server启动失败.");
				std::cout << "union pay http server启动失败." << std::endl;
			}
		}

		SPDLOG_INFO("启动银联请求转发服务成功");
		static base::semaphore sem;
		signal(SIGINT, [](int) { 
			signal(SIGINT, SIG_IGN);
			sem.post(); 
		});
		sem.wait();

		SPDLOG_INFO("接收到退出系统信号,开始退出银联请求转发服务");
		CNotifyHttpClient::SendShutdownRequest(config.GetValueAsString("unionpay_forward", "proxy.host", "localhost"),
				config.GetValueAsUint32("unionpay_forward", "proxy.port", 9528),
				config.GetValueAsString("manager", "key", ""));
		Sleep(2000);
		
		SPDLOG_INFO("退出银联请求转发服务成功");
		SPDLOG_INFO("接收到退出系统信号,开始退出银联通知转发服务");
		CNotifyHttpClient::SendShutdownRequest(config.GetValueAsString("notify_forward", "proxy.host", "localhost"),
				config.GetValueAsUint32("notify_forward", "proxy.port", 9527),
				config.GetValueAsString("manager", "key", ""));
		Sleep(2000);
		SPDLOG_INFO("退出银联通知转发服务成功");
		SPDLOG_INFO("程序退出");
		CLogger::Close();
	}

	std::cout << "程序退出." << std::endl;
}
