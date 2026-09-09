#include "transport/signal_fd.hpp"

#include <csignal>

#include <sys/signalfd.h>
#include <unistd.h>

namespace transport
{

SignalFd::SignalFd()
{
    sigset_t mask;

    ::sigemptyset(&mask);

    // Блокируем SIGINT и SIGTERM.
    //
    // Зачем?
    //
    // В демоне Linux сигналы доставляются
    // случайному потоку. Чтобы event loop
    // их обрабатывал, блокируем их во всех
    // потоках и читаем через signalfd.
    //
    // signalfd превращает сигналы в fd event,
    // который epoll может мониторить.
    ::sigaddset(
        &mask,
        SIGINT);

    ::sigaddset(
        &mask,
        SIGTERM);

    ::sigprocmask(
        SIG_BLOCK,
        &mask,
        nullptr);

    fd_ =
        ::signalfd(
            -1,
            &mask,
            SFD_CLOEXEC |
            SFD_NONBLOCK);
}

SignalFd::~SignalFd()
{
    if (fd_ != -1)
    {
        ::close(fd_);
    }
}

int SignalFd::fd()
    const noexcept
{
    return fd_;
}

void SignalFd::consume()
{
    signalfd_siginfo info{};

    // signalfd возвращает структуру
    // для каждого сигнала.
    //
    // Читаем до конца, чтобы
    // сбросить EPOLLIN.
    while (
        ::read(
            fd_,
            &info,
            sizeof(info)) ==
        sizeof(info))
    {
    }
}

} // namespace transport