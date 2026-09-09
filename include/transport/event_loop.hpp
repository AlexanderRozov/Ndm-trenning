#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

#include "transport/epoll.hpp"
#include "transport/signal_fd.hpp"

namespace transport
{

class EventLoop
{
public:
    EventLoop();

    void addFd(
        int fd,
        std::uint32_t events,
        std::function<void(
            std::uint32_t)> callback);

    void modifyFd(
        int fd,
        std::uint32_t events);

    void removeFd(
        int fd);

    void run();

    void stop() noexcept;
private:
    Epoll epoll_;

    SignalFd signalFd_;

    bool running_ =
        true;

    // Callback ассоциированный с fd.
    std::unordered_map<
        int,
        std::function<void(
            std::uint32_t)>> callbacks_;
};

} // namespace transport