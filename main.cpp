#include "modem/modem.h"
#include "transport/epoll.h"
#include "transport/event_loop.h"
#include "transport/line_reader.h"
#include "transport/tty.h"

#include <pty.h>

#include <iostream>
#include <string>
#include <unistd.h>

int main()
{
    int masterFd = -1;
    int slaveFd = -1;

    char slaveName[256]{};

    if (::openpty(
            &masterFd,
            &slaveFd,
            slaveName,
            nullptr,
            nullptr) == -1)
    {
        std::cerr << "openpty failed\n";
        return 1;
    }

    ::close(slaveFd);

    std::cout
        << "Modem PTY: "
        << slaveName
        << '\n';

    transport::Tty tty(masterFd);

    transport::LineReader reader;

    modem::Modem modem;

    transport::EventLoop loop;

    loop.addFd(
        tty.fd(),
        EPOLLIN,
        [&tty, &reader, &modem]
        (std::uint32_t events)
        {
            if (!(events & EPOLLIN))
                return;

            char buffer[256];

            const ssize_t count =
                tty.read(buffer, sizeof(buffer));

            if (count <= 0)
                return;

            const std::string_view data(
                buffer,
                static_cast<std::size_t>(count));

            while (true)
            {
                const auto line =
                    reader.push(data);

                if (!line)
                    break;

                const std::string response =
                    modem.handle(*line);

                tty.write(
                    response.data(),
                    response.size());
            }
        });

    loop.run();

    std::cout << "Modem stopped\n";

    return 0;
}