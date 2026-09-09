#pragma once

#include <cstdint>

namespace transport
{

    class TimerFd
    {
    public:
        TimerFd();

        ~TimerFd();

        TimerFd(const TimerFd&) = delete;
        TimerFd& operator=(const TimerFd&) = delete;

        TimerFd(TimerFd&& other) noexcept;
        TimerFd& operator=(TimerFd&& other) noexcept;

        int fd() const noexcept;

        void arm(
            std::uint64_t seconds);

        std::uint64_t consume();

    private:
        int fd_ = -1;
    };

}