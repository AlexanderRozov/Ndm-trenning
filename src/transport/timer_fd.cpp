#include "transport/timer_fd.h"

#include <sys/timerfd.h>

#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>

namespace transport
{

    TimerFd::TimerFd()
    {
        fd_ = ::timerfd_create(
            CLOCK_MONOTONIC,
            TFD_CLOEXEC);

        if (fd_ == -1)
        {
            throw std::runtime_error(
                std::strerror(errno));
        }
    }

    TimerFd::~TimerFd()
    {
        if (fd_ != -1)
        {
            ::close(fd_);
        }
    }

    TimerFd::TimerFd(TimerFd&& other) noexcept
        : fd_(other.fd_)
    {
        other.fd_ = -1;
    }

    TimerFd& TimerFd::operator=(
        TimerFd&& other) noexcept
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

    int TimerFd::fd() const noexcept
    {
        return fd_;
    }

    void TimerFd::arm(
        std::uint64_t seconds)
    {
        itimerspec spec{};

        spec.it_value.tv_sec =
            static_cast<time_t>(seconds);

        if (::timerfd_settime(
                fd_,
                0,
                &spec,
                nullptr) == -1)
        {
            throw std::runtime_error(
                std::strerror(errno));
        }
    }

    std::uint64_t TimerFd::consume()
    {
        std::uint64_t expirations = 0;

        const ssize_t result =
            ::read(
                fd_,
                &expirations,
                sizeof(expirations));

        if (result != sizeof(expirations))
        {
            throw std::runtime_error(
                "timerfd read failed");
        }

        return expirations;
    }

}