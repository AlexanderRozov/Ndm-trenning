#include "transport/epoll.hpp"

#include <cerrno>
#include <system_error>

#include <unistd.h>

namespace transport
{

Epoll::Epoll()
{
    fd_ =
        ::epoll_create1(
            EPOLL_CLOEXEC);

    if (fd_ == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "epoll_create1");
    }
}

Epoll::~Epoll()
{
    if (fd_ != -1)
    {
        ::close(fd_);
    }
}

Epoll::Epoll(
    Epoll&& other) noexcept
    : fd_(other.fd_)
{
    other.fd_ = -1;
}

Epoll& Epoll::operator=(
    Epoll&& other) noexcept
{
    if (this == &other)
        return *this;

    if (fd_ != -1)
    {
        ::close(fd_);
    }

    fd_ = other.fd_;
    other.fd_ = -1;

    return *this;
}

void Epoll::add(
    int fd,
    std::uint32_t events)
{
    epoll_event event{};

    event.events = events;

    // fd используется как идентификатор.
    event.data.fd = fd;

    if (::epoll_ctl(
            fd_,
            EPOLL_CTL_ADD,
            fd,
            &event) == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "epoll_ctl ADD");
    }
}

void Epoll::modify(
    int fd,
    std::uint32_t events)
{
    epoll_event event{};

    event.events = events;
    event.data.fd = fd;

    if (::epoll_ctl(
            fd_,
            EPOLL_CTL_MOD,
            fd,
            &event) == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "epoll_ctl MOD");
    }
}

void Epoll::remove(int fd)
{
    if (::epoll_ctl(
            fd_,
            EPOLL_CTL_DEL,
            fd,
            nullptr) == -1)
    {
        // fd мог уже быть закрыт.
        if (errno != ENOENT &&
            errno != EBADF)
        {
            throw std::system_error(
                errno,
                std::generic_category(),
                "epoll_ctl DEL");
        }
    }
}

std::vector<epoll_event> Epoll::wait(
    int timeoutMs)
{
    // Буфер для событий.
    std::vector<epoll_event> events(
        64);

    int count;

    // epoll_wait может вернуть EINTR.
    do
    {
        count =
            ::epoll_wait(
                fd_,
                events.data(),
                static_cast<int>(
                    events.size()),
                timeoutMs);
    } while (
        count == -1 &&
        errno == EINTR);

    if (count == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "epoll_wait");
    }

    events.resize(
        static_cast<std::size_t>(
            count));

    return events;
}

} // namespace transport