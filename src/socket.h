#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "common.h"

class Socket
{
public:
    Socket();
    ~Socket();
    int get_fd() const;

private:
    const char *server_ip;
    const char *server_port;
    int fd = -1;
    void connect_to_server();
    void load_env_file(const std::string &filename);
};

#endif//_SOCKET_H_