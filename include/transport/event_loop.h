#include "transport/event_loop.h"

#include <sys/signalfd.h>

#include <unistd.h>

#include <stdexcept>

namespace transport
{

    EventLoop::EventLoop()
    {
        epoll_.add(
            signalFd_.fd(),
            EPOLLIN);
    }

    void EventLoop::addFd(
        int fd,
        std::uint32_t events,
        std::function<void(std::uint32_t)> callback)
    {
        epoll_.add(fd, events);

        callbacks_.insert_or_assign(
            fd,
            std::move(callback));
    }

    void EventLoop::run()
    {
        while (running_)
        {
            const auto events = epoll_.wait(-1);

            for (const auto& event : events)
            {
                if (event.data.fd == signalFd_.fd())
                {
                    signalfd_siginfo info{};

                    const auto result =
                        ::read(
                            signalFd_.fd(),
                            &info,
                            sizeof(info));

                    if (result == sizeof(info))
                    {
                        stop();
                    }

                    continue;
                }

                const auto it =
                    callbacks_.find(event.data.fd);

                if (it != callbacks_.end())
                {
                    it->second(event.events);
                }
            }
        }
    }

    void EventLoop::stop() noexcept
    {
        running_ = false;
    }

}