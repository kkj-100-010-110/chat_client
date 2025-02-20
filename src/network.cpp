#include "network.h"

std::string Network::recv_buffer = "";

void Network::send_cmd_msg(int server_fd, const std::string &msg)
{
    std::string msg_with_newline = msg + "\r\n";
    send(server_fd, msg_with_newline.c_str(), msg_with_newline.size(), 0);
}

void Network::receive_message(int server_fd)
{
    char buf[1024];
    int ret_recv = recv(server_fd, buf, sizeof(buf) - 1, 0);

    if (ret_recv > 0)
    {
        buf[ret_recv] = '\0';
        recv_buffer += buf;
        // message classification
        size_t pos;
        while ((pos = recv_buffer.find("\r\n")) != std::string::npos)
        {
            std::string message = recv_buffer.substr(0, pos);
            recv_buffer.erase(0, pos + 2);

            ChatManager::enqueue_message(message);
        }
    }
    else if (ret_recv == 0)
    {
        std::cerr << "SERVER CLOSED THE CONNECTION.\n";
        exit(1);
    }
    else if (errno == EWOULDBLOCK || errno == EAGAIN)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    else
    {
        std::cerr << "ERROR RECEIVING DATA: " << strerror(errno) << std::endl;
    }
}