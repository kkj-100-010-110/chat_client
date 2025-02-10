#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <sys/socket.h>

#include "common.h"
#include "command.h"

class Network
{
public:
    static void send_cmd_msg(int server_fd, const std::string& msg);
    static void receive_message(int server_fd);
private:
    static std::string recv_buffer;
};

#endif//_NETWORK_H_