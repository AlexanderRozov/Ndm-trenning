#pragma once

namespace transport
{

    class SignalFd
    {
    public:
        SignalFd();

        ~SignalFd();

        SignalFd(const SignalFd&) = delete;
        SignalFd& operator=(const SignalFd&) = delete;

        SignalFd(SignalFd&& other) noexcept;
        SignalFd& operator=(SignalFd&& other) noexcept;

        int fd() const noexcept;

    private:
        int fd_ = -1;
    };

}