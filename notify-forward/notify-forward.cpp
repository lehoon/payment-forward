// notify-forward.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>
#include <signal.h>

#include "Logger.h"
#include "semaphore.h"
#include "Configure.h"
#include "NotifyHttpClient.h"
#include "NotifyHttpServer.h"

int main()
{
    {
        CConfigure config;
        if (config.Init("./config.ini")) {
            std::cout << "当前目录下未找到配置文件,程序退出." << std::endl;
            return -1;
        }

        CLogger::Init();
        CNotifyHttpServer notifyHttpServer(&config);
        notifyHttpServer.InitTask();
        if (!notifyHttpServer.StartTask()) {
            std::cout << "http server启动失败." << std::endl;
            return -1;
        }

        static base::semaphore sem;
        signal(SIGINT, [](int) { sem.post(); });
        sem.wait();

        std::string key = config.GetValueAsString("manager", "key", "");
        CNotifyHttpClient::SendShutdownMessage(config.GetValueAsUint32("server", "port", 9527), key);
        CLogger::Close;
    }

    std::cout << "程序退出." << std::endl;
}
