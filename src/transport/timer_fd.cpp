#include "transport/timer_fd.hpp"

#include <cerrno>
#include <system_error>

#include <sys/timerfd.h>
#include <unistd.h>

namespace transport
{

TimerFd::TimerFd()
{
    fd_ =
        ::timerfd_create(
            CLOCK_MONOTONIC,
            TFD_CLOEXEC |
            TFD_NONBLOCK);

    if (fd_ == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "timerfd_create");
    }
}

TimerFd::~TimerFd()
{
    if (fd_ != -1)
    {
        ::close(fd_);
    }
}

int TimerFd::fd()
    const noexcept
{
    return fd_;
}

void TimerFd::arm(
    std::uint64_t seconds)
{
    itimerspec timer{};

    // One-shot таймер.
    timer.it_value.tv_sec =
        static_cast<time_t>(
            seconds);

    // it_interval = 0,
    // поэтому таймер не повторяется.
    timer.it_interval = {};

    if (::timerfd_settime(
            fd_,
            0,
            &timer,
            nullptr) == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "timerfd_settime");
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

    if (result !=
        static_cast<ssize_t>(
            sizeof(expirations)))
    {
        return 0;
    }

    return expirations;
}

} // namespace transport