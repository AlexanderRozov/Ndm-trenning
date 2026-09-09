#include "modem/modem.h"
#include "transport/tty.h"

#include <fcntl.h>
#include <iostream>
#include <pty.h>
#include <unistd.h>

int main()
{
    int masterFd = -1;
    int slaveFd = -1;

    char slaveName[256]{};

    if (openpty(
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

    modem::Modem modem;

    char buffer[256];

    while (true)
    {
        const ssize_t count =
            tty.read(buffer, sizeof(buffer));

        if (count <= 0)
            break;

        std::string input(
            buffer,
            static_cast<std::size_t>(count));

        const std::string response =
            modem.handle(input);

        tty.write(
            response.data(),
            response.size());
    }

    return 0;
}