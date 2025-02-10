#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "common.h"
#include "chat_manager.h"

#include <sstream>

class Network;

using CommandHandlerFunc = std::function<void(int, const std::string&)>;

class Command
{
public:
    Command();
    void command_handler(int server_fd, const std::string &msg);

private:
    std::unordered_map<std::string, CommandHandlerFunc> command_map;

    void setup_command_map();
    void handle_list(const std::string &message);
    void handle_join_success(const std::string &message);
    void handle_open_success(int server_fd, const std::string &message);
    void handle_msg_success(const std::string &message);
    void handle_member_success(const std::string &message);
    void handle_info(const std::string &message);
    void handle_leave_success(const std::string &message);
    void handle_room_delete(const std::string &message);
    void handle_err(const std::string &message);
};

#endif//_COMMAND_H_