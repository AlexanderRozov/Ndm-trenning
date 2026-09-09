#include <iostream>
#include <string>

#include "modem.h"

int main()
{
    modem::Modem modem;

    std::string line;

    while (std::getline(std::cin, line))
    {
        std::cout << modem.handle(line);
        std::cout.flush();
    }

    return 0;
}