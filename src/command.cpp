#include "command.h"
#include "network.h"

Command::Command() { setup_command_map(); }

void Command::setup_command_map()
{
    command_map["LIST"] = [this](int server_fd, const std::string &msg) { handle_list(msg); };
    command_map["ID "] = [this](int server_fd, const std::string &msg) { ChatManager::current_state = ChatManager::CHAT_LOBBY; };
    command_map["JOIN_SUCCESS"] = [this](int server_fd, const std::string &msg)
    {
        handle_join_success(msg);
        Network::send_cmd_msg(server_fd, "MEMBER");
    };
    command_map["OPEN_SUCCESS"] = [this](int server_fd, const std::string &msg)
    {
        Network::send_cmd_msg(server_fd, "LIST");
    };
    command_map["MSG"] = [this](int server_fd, const std::string &msg) { handle_msg_success(msg); };
    command_map["MEMBER_SUCCESS"] = [this](int server_fd, const std::string &msg) { handle_member_success(msg); };
    command_map["INFO"] = [this](int server_fd, const std::string &msg) { handle_info(msg); };
    command_map["LEAVE_SUCCESS"] = [this](int server_fd, const std::string &msg) { handle_leave_success(msg); };
    command_map["ROOM_DELETE"] = [this](int server_fd, const std::string &msg) {};
}

void Command::command_handler(int server_fd, const std::string& msg)
{
    for (const auto &e : command_map)
    {
        if (msg.find(e.first) == 0)
        {
            e.second(server_fd, msg);
            return;
        }
    }
    handle_err(msg);
}

void Command::handle_list(const std::string &message)
{
    std::string payload = message.substr(5);

    if (payload.empty() || payload == "\r\n" || payload == "\n")
    {
        ChatManager::room_list.clear();
        return;
    }

    std::vector<std::string> rooms;
    std::istringstream iss(payload);
    std::string room;

    while (std::getline(iss, room, ','))
    {
        room.erase(0, room.find_first_not_of("\t\r\n"));
        room.erase(room.find_last_not_of("\t\r\n") + 1);

        if (!room.empty())
        {
            rooms.push_back(room);
        }
    }
    ChatManager::update_room_list(rooms);
}

void Command::handle_join_success(const std::string& message)
{
    std::string payload = message.substr(13);
    size_t delimiter = payload.find('|');
    std::string room_name = payload.substr(0, delimiter);
    ChatManager::current_room = room_name;
    std::string id = payload.substr(delimiter+1);
    ChatManager::room_members[room_name].push_back(id);
    ChatManager::current_state = ChatManager::CHAT_ROOM;
}

void Command::handle_open_success(int server_fd, const std::string& message)
{
    Network::send_cmd_msg(server_fd, "LIST");
    ChatManager::current_state = ChatManager::CHAT_ROOM;
}

void Command::handle_msg_success(const std::string& message)
{
    std::string msg = message.substr(4);
    ChatManager::add_chat_message(msg);
}

void Command::handle_leave_success(const std::string& message)
{
    std::string payload = message.substr(14);
    size_t delimiter = payload.find('|');
    std::string room_name = payload.substr(0, delimiter);
    std::string id = payload.substr(delimiter+1);
    auto& users = ChatManager::room_members[room_name];
    auto it = std::find(users.begin(), users.end(), id);
    if (it != users.end())
        users.erase(it);
    ChatManager::current_room = "";
    ChatManager::current_state = ChatManager::CHAT_LOBBY;
}

void Command::handle_member_success(const std::string& message)
{
    std::string payload = message.substr(15);

    if (payload == "\r\n" || payload == "\n")
    {
        std::cout << "No payload to update." << std::endl;
        return;
    }

    size_t delimiter = payload.find('|');
    std::string room_name = payload.substr(0, delimiter);
    payload = payload.substr(delimiter+1);

    std::istringstream iss(payload);
    std::vector<std::string> members;
    std::string member;

    while (std::getline(iss, member, ','))
    {
        if (!member.empty() && member.find_first_not_of(" \t\r\n") != std::string::npos)
        {
            members.push_back(member);
        }
    }

    ChatManager::update_room_members(room_name, members);
}

void Command::handle_info(const std::string& message)
{
    std::string payload = message.substr(5);

    if (payload == "\r\n" || payload == "\n")
    {
        std::cout << "No payload to update." << std::endl;
        return;
    }

    size_t delimiter = payload.find('|');
    std::string room_name = payload.substr(0, delimiter);
    payload = payload.substr(delimiter+1);

    std::istringstream iss(payload);
    std::vector<std::string> members;
    std::string member;

    while (std::getline(iss, member, ','))
    {
        if (!member.empty() && member.find_first_not_of(" \t\r\n") != std::string::npos)
        {
            members.push_back(member);
        }
    }

    ChatManager::update_room_members(room_name, members);
}

void Command::handle_room_delete(const std::string& message)
{
    std::string payload = message.substr(12);
    auto it = std::find(ChatManager::room_list.begin(), ChatManager::room_list.end(), payload);
    if (it != ChatManager::room_list.end())
        ChatManager::room_list.erase(it);
    ChatManager::room_members.erase(payload);
}

void Command::handle_err(const std::string& message)
{
    ChatManager::err_msg = message;
    ChatManager::show_err_popup = true;
}