#include "transport/event_loop.hpp"

#include <sys/epoll.h>

namespace transport
{

EventLoop::EventLoop()
{
    // SIGINT/SIGTERM добавляем в epoll.
    addFd(
        signalFd_.fd(),
        EPOLLIN,
        [this](std::uint32_t)
        {
            signalFd_.consume();

            // Останавливаем event loop
            // при получении сигнала.
            stop();
        });
}

void EventLoop::addFd(
    int fd,
    std::uint32_t events,
    std::function<void(
        std::uint32_t)> callback)
{
    epoll_.add(
        fd,
        events);

    callbacks_.emplace(
        fd,
        std::move(callback));
}

void EventLoop::modifyFd(
    int fd,
    std::uint32_t events)
{
    epoll_.modify(
        fd,
        events);
}

void EventLoop::removeFd(
    int fd)
{
    epoll_.remove(fd);

    callbacks_.erase(fd);
}

void EventLoop::run()
{
    running_ = true;

    while (running_)
    {
        const auto events =
            epoll_.wait(-1);

        for (const auto& event :
             events)
        {
            const auto it =
                callbacks_.find(
                    event.data.fd);

            if (it ==
                callbacks_.end())
            {
                continue;
            }

            it->second(
                event.events);
        }
    }
}

void EventLoop::stop()
    noexcept
{
    running_ = false;
}

} // namespace transport