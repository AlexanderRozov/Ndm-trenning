#pragma once

#include <sys/epoll.h>

#include <vector>

namespace transport
{

    class Epoll
    {
    public:
        Epoll();

        ~Epoll();

        Epoll(const Epoll&) = delete;
        Epoll& operator=(const Epoll&) = delete;

        Epoll(Epoll&& other) noexcept;
        Epoll& operator=(Epoll&& other) noexcept;

        void add(int fd, std::uint32_t events);

        std::vector<epoll_event> wait(int timeoutMs);

    private:
        int fd_ = -1;
    };

}