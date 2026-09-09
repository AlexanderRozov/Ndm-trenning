#include "transport/epoll.h"

#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>

namespace transport
{

    Epoll::Epoll()
    {
        fd_ = ::epoll_create1(EPOLL_CLOEXEC);

        if (fd_ == -1)
        {
            throw std::runtime_error(
                std::strerror(errno));
        }
    }

    Epoll::~Epoll()
    {
        if (fd_ != -1)
        {
            ::close(fd_);
        }
    }

    Epoll::Epoll(Epoll&& other) noexcept
        : fd_(other.fd_)
    {
        other.fd_ = -1;
    }

    Epoll& Epoll::operator=(Epoll&& other) noexcept
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
        event.data.fd = fd;

        if (::epoll_ctl(
                fd_,
                EPOLL_CTL_ADD,
                fd,
                &event) == -1)
        {
            throw std::runtime_error(
                std::strerror(errno));
        }
    }

    std::vector<epoll_event> Epoll::wait(
        int timeoutMs)
    {
        std::vector<epoll_event> events(16);

        const int count = ::epoll_wait(
            fd_,
            events.data(),
            static_cast<int>(events.size()),
            timeoutMs);

        if (count == -1)
        {
            if (errno == EINTR)
                return {};

            throw std::runtime_error(
                std::strerror(errno));
        }

        events.resize(static_cast<std::size_t>(count));

        return events;
    }

}