#include "chat_manager.h"

namespace ChatManager
{
ChatManager::AppState current_state = ChatManager::LOGIN_SCREEN;

std::string current_room = "";
std::deque<std::string> chat_messages;
std::vector<std::string> room_list;
std::unordered_map<std::string, std::vector<std::string>> room_members;
std::queue<std::string> message_queue;

std::string err_msg = "";
bool show_err_popup = false;

std::mutex queue_mutex;
std::condition_variable queue_cv;

const size_t MAX_CHAT_MESSAGES = 100;

void enqueue_message(const std::string &msg)
{
    std::lock_guard<std::mutex> lock(queue_mutex);
    message_queue.push(msg);
    queue_cv.notify_one();
}

std::string dequeue_message()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    if (message_queue.empty())
        return "";

    std::string msg = message_queue.front();
    message_queue.pop();
    return msg;
}

void add_chat_message(const std::string &msg)
{
    if (chat_messages.size() >= MAX_CHAT_MESSAGES)
    {
        chat_messages.pop_front();
    }
    chat_messages.push_back(msg);
}

void update_room_list(const std::vector<std::string> &new_rooms) { room_list = new_rooms; }
void update_room_members(const std::string &room_name, const std::vector<std::string> &members) { room_members[room_name] = members; }
} // namespace ChatManager