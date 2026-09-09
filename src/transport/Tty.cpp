#include "transport/tty.hpp"

#include <unistd.h>

namespace transport
{

Tty::Tty(int fd)
    : fd_(fd)
{
}

Tty::~Tty()
{
    if (fd_ != -1)
    {
        ::close(fd_);
    }
}

Tty::Tty(
    Tty&& other) noexcept
    : fd_(other.fd_)
{
    // Передаём владение fd.
    other.fd_ = -1;
}

Tty& Tty::operator=(
    Tty&& other) noexcept
{
    if (this == &other)
        return *this;

    // Закрываем старый fd, если есть.
    if (fd_ != -1)
    {
        ::close(fd_);
    }

    // Перебираем владение.
    fd_ = other.fd_;

    other.fd_ = -1;

    return *this;
}

bool Tty::valid() const noexcept
{
    return fd_ != -1;
}

int Tty::fd() const noexcept
{
    return fd_;
}

ssize_t Tty::read(
    char* buffer,
    std::size_t size)
{
    return ::read(
        fd_,
        buffer,
        size);
}

ssize_t Tty::write(
    const char* buffer,
    std::size_t size)
{
    return ::write(
        fd_,
        buffer,
        size);
}

} // namespace transport