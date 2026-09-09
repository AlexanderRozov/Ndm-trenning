#pragma once

#include <cstddef>
#include <sys/types.h>

namespace transport
{

// RAII wrapper над Linux file descriptor.
//
// Запрещает копирование, разрешает перемещение.
//
// Правило владения:
//
// Tty a(fd);
// Tty b = a;       // Ошибка компиляции
//
// Tty b = std::move(a); // OK, b владеет fd
//
// При уничтожении Tty закрывает fd.
class Tty
{
public:
    Tty() = default;

    explicit Tty(
        int fd);

    ~Tty();

    Tty(
        const Tty&) = delete;

    Tty& operator=(
        const Tty&) = delete;

    Tty(
        Tty&& other) noexcept;

    Tty& operator=(
        Tty&& other) noexcept;

    bool valid() const noexcept;

    int fd() const noexcept;

    ssize_t read(
        char* buffer,
        std::size_t size);

    ssize_t write(
        const char* buffer,
        std::size_t size);
private:
    int fd_ = -1;
};

} // namespace transport