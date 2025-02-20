#include "socket.h"

Socket::Socket() { connect_to_server(); }

Socket::~Socket()
{
    if (fd != -1)
    {
        close(fd);
        std::cout << "SOCKET CLOSED." << std::endl;
    }
}

int Socket::get_fd() const { return fd; }

void Socket::connect_to_server()
{
    load_env_file(".env");
    server_ip = std::getenv("SERVER_IP");
    if (!server_ip)
    {
        std::cerr << "ERROR::NOT SETUP ENV SERVER_IP" << std::endl;
        exit(EXIT_FAILURE);
    }
    server_port = std::getenv("SERVER_PORT");
    if (!server_port)
    {
        std::cerr << "ERROR::NOT SETUP ENV SERVER_PORT" << std::endl;
        exit(EXIT_FAILURE);
    }

    int rc_gai;
    struct addrinfo ai_dest{};
    struct addrinfo *ai_dest_ret;

    ai_dest.ai_family = AF_UNSPEC;
    ai_dest.ai_socktype = SOCK_STREAM;
    ai_dest.ai_flags = AI_ADDRCONFIG;

    if ((rc_gai = getaddrinfo(server_ip, server_port, &ai_dest, &ai_dest_ret)) != 0)
    {
        std::cerr << "Fail: getaddrinfo(): " << gai_strerror(rc_gai) << std::endl;
        exit(EXIT_FAILURE);
    }

    fd = socket(ai_dest_ret->ai_family, ai_dest_ret->ai_socktype, ai_dest_ret->ai_protocol);
    if (fd == -1)
    {
        std::cerr << "Fail: socket()" << std::endl;
        freeaddrinfo(ai_dest_ret);
        exit(EXIT_FAILURE);
    }

    if (connect(fd, ai_dest_ret->ai_addr, ai_dest_ret->ai_addrlen) == -1)
    {
        std::cerr << "Fail: connect()" << std::endl;
        freeaddrinfo(ai_dest_ret);
        close(fd);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(ai_dest_ret);
    std::cout << "CONNECTED TO SERVER." << std::endl;
}

void Socket::load_env_file(const std::string &filename)
{
    std::ifstream env_file(filename);
    if (!env_file.is_open())
    {
        std::cerr << "ERROR:: failed to open .env" << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(env_file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string key, value;

        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            key.erase(0, key.find_first_not_of(" \t\n\r"));
            key.erase(key.find_last_not_of(" \t\n\r") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);

            if (setenv(key.c_str(), value.c_str(), 1) != 0)
            {
                std::cerr << "ERROR:: failed to set env" << key << std::endl;
            }
        }
    }

    env_file.close();
}