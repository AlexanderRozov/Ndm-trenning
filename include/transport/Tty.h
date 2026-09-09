//
// Created by fog54 on 09.09.2026.
//

#ifndef QMI_MODEM_TTY_H
#define QMI_MODEM_TTY_H
#pragma once

#include <string>

namespace transport
{

    class Tty
    {
    public:
        Tty() = default;

        explicit Tty(int fd);

        ~Tty();

        Tty(const Tty&) = delete;
        Tty& operator=(const Tty&) = delete;

        Tty(Tty&& other) noexcept;
        Tty& operator=(Tty&& other) noexcept;

        bool valid() const noexcept;

        int fd() const noexcept;

        ssize_t read(char* buffer, std::size_t size);

        ssize_t write(const char* buffer, std::size_t size);

    private:
        int fd_ = -1;
    };

}
#endif //QMI_MODEM_TTY_H
