#ifndef _CHAT_MANAGER_H_
#define _CHAT_MANAGER_H_

#include <deque>
#include <vector>
#include <string>
#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace ChatManager
{
enum AppState
{
    LOGIN_SCREEN,
    CHAT_LOBBY,
    CHAT_ROOM
};

extern AppState current_state;

extern std::string current_room;
extern std::deque<std::string> chat_messages;
extern std::vector<std::string> room_list;
extern std::unordered_map<std::string, std::vector<std::string>> room_members;
extern std::queue<std::string> message_queue;

extern std::string err_msg;
extern bool show_err_popup;

extern std::mutex queue_mutex;
extern std::condition_variable queue_cv;

void enqueue_message(const std::string &msg);
std::string dequeue_message();
void add_chat_message(const std::string &msg);
void update_room_list(const std::vector<std::string> &new_rooms);
void update_room_members(const std::string &room_name, const std::vector<std::string> &users);
} // namespace ChatManager

#endif//_CHAT_MANAGER_H_