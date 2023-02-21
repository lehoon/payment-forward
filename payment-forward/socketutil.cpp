#include "socketutil.h"

#include <fcntl.h>

static WSInit ws_init;

void select_timeout(int timeout) {
    SOCKET socket = ::socket(AF_INET, SOCK_STREAM, 0);
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socket, &readfds);
    timeval tv = {};
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    ::select(socket + 1, &readfds, NULL, NULL, &tv);
    closesocket(socket);
}

static void set_nio_socket(SOCKET socket) {
    int flag = 1;
    ioctlsocket(socket, FIONBIO, (u_long FAR*)&flag);
}

//测试远端服务器是否在线
bool is_remote_server_alive(std::string& host, std::string &ip, const unsigned short port, int address_family, int socket_flags, const unsigned int connect_timeout) {
    const char* node = nullptr;
    struct addrinfo hints;
    struct addrinfo* result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    if (!ip.empty()) {
        node = ip.c_str();
        // Ask getaddrinfo to convert IP in c-string to address
        hints.ai_family = AF_UNSPEC;
        hints.ai_flags = AI_NUMERICHOST;
    }
    else {
        if (!host.empty()) { node = host.c_str(); }
        hints.ai_family = address_family;
        hints.ai_flags = socket_flags;
    }

    auto service = std::to_string(port);
    if (getaddrinfo(node, service.c_str(), &hints, &result)) {
        return false;
    }

    for (auto rp = result; rp; rp = rp->ai_next) {
        auto sock =
            WSASocketW(rp->ai_family, rp->ai_socktype, rp->ai_protocol, nullptr, 0,
                WSA_FLAG_NO_HANDLE_INHERIT | WSA_FLAG_OVERLAPPED);

        if (sock == INVALID_SOCKET) {
            sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        }

        if (sock == INVALID_SOCKET) { continue; }

        if (rp->ai_family == AF_INET6) {
            int no = 0;
            setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<char*>(&no),
                sizeof(no));
        }

        set_nio_socket(sock);

        if(-1 != ::connect(sock, (*rp).ai_addr, static_cast<socklen_t>((*rp).ai_addrlen))) {
            freeaddrinfo(result);
            closesocket(sock);
            return true;
        }

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        timeval tv = {};
        tv.tv_sec = connect_timeout;
        tv.tv_usec = 0;
        if(::select(sock + 1, &readfds, NULL, NULL, &tv) <= 0) {
            freeaddrinfo(result);
            closesocket(sock);
            return false;
        } else {
            int error = -1;
            int opt_len = sizeof(int);
            getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&error, &opt_len);
            if (errno == 0) {
                freeaddrinfo(result);
                closesocket(sock);
                return true;
            } else {
                continue;
            }
        }

        closesocket(sock);
    }

    freeaddrinfo(result);
    return false;
}

