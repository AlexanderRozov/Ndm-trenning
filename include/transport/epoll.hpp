#pragma once

#include <cstdint>
#include <sys/epoll.h>
#include <vector>

namespace transport
{

class Epoll
{
public:
    Epoll();

    ~Epoll();

    Epoll(
        const Epoll&) = delete;

    Epoll& operator=(
        const Epoll&) = delete;

    Epoll(
        Epoll&& other) noexcept;

    Epoll& operator=(
        Epoll&& other) noexcept;

    void add(
        int fd,
        std::uint32_t events);

    // EPOLL_CTL_MOD.
    //
    // Нужен, чтобы включать/выключать EPOLLOUT
    // когда появляются/исчезают данные в TX очереди.
    void modify(
        int fd,
        std::uint32_t events);

    void remove(
        int fd);

    std::vector<epoll_event>
    wait(
        int timeoutMs);
private:
    int fd_ = -1;
};

} // namespace transport