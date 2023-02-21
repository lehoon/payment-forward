#ifndef PAYMENT_FORWARD_SOCKET_URIL_H_
#define PAYMENT_FORWARD_SOCKET_URIL_H_

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")


class WSInit {
public:
    WSInit() {
        WSADATA wsaData;
        if (WSAStartup(0x0002, &wsaData) == 0) is_valid_ = true;
    }

    ~WSInit() {
        if (is_valid_) WSACleanup();
    }

    bool is_valid_ = false;
};

//测试远端服务器是否在线
bool is_remote_server_alive(std::string& host, std::string &ip, const unsigned short port, int address_family, int socket_flags, const unsigned int connect_timeout);


#endif //PAYMENT_FORWARD_SOCKET_URIL_H_
