#include "transport/signal_fd.h"

#include <sys/signalfd.h>

#include <signal.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <pthread.h>
#include <stdexcept>

namespace transport
{

    SignalFd::SignalFd()
    {
        sigset_t mask;

        ::sigemptyset(&mask);
        ::sigaddset(&mask, SIGINT);
        ::sigaddset(&mask, SIGTERM);

        if (::sigprocmask(
                SIG_BLOCK,
                &mask,
                nullptr) == -1)
        {
            throw std::runtime_error(
                std::strerror(errno));
        }

        fd_ = ::signalfd(
            -1,
            &mask,
            SFD_CLOEXEC);

        if (fd_ == -1)
        {
            throw std::runtime_error(
                std::strerror(errno));
        }
    }

    SignalFd::~SignalFd()
    {
        if (fd_ != -1)
        {
            ::close(fd_);
        }
    }

    SignalFd::SignalFd(SignalFd&& other) noexcept
        : fd_(other.fd_)
    {
        other.fd_ = -1;
    }

    SignalFd& SignalFd::operator=(
        SignalFd&& other) noexcept
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

    int SignalFd::fd() noexcept
    {
        return fd_;
    }

}