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
    int rc_gai;
    struct addrinfo ai_dest{};
    struct addrinfo *ai_dest_ret;

    ai_dest.ai_family = AF_UNSPEC;
    ai_dest.ai_socktype = SOCK_STREAM;
    ai_dest.ai_flags = AI_ADDRCONFIG;

    if ((rc_gai = getaddrinfo(SERVER_IP, SERVER_PORT, &ai_dest, &ai_dest_ret)) != 0)
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