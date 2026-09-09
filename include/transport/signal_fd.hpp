#pragma once

#include <cstdint>

namespace transport
{

class SignalFd
{
public:
    SignalFd();

    ~SignalFd();

    SignalFd(
        const SignalFd&) = delete;

    SignalFd& operator=(
        const SignalFd&) = delete;

    int fd() const noexcept;

    // Потребляет сигналы из fd.
    void consume();
private:
    int fd_ = -1;
};

} // namespace transport