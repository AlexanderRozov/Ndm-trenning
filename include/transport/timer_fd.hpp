#pragma once

#include <cstdint>

namespace transport
{

class TimerFd
{
public:
    TimerFd();

    ~TimerFd();

    TimerFd(
        const TimerFd&) = delete;

    TimerFd& operator=(
        const TimerFd&) = delete;

    int fd() const noexcept;

    // Запускает one-shot таймер.
    void arm(
        std::uint64_t seconds);

    // Читает количество срабатываний.
    std::uint64_t consume();
private:
    int fd_ = -1;
};

} // namespace transport