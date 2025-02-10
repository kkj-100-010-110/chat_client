#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "common.h"

constexpr const char* SERVER_IP = "192.168.0.5";
constexpr const char* SERVER_PORT = "8080";

class Socket
{
public:
    Socket();
    ~Socket();
    int get_fd() const;

private:
    int fd = -1;
    void connect_to_server();
};

#endif//_SOCKET_H_