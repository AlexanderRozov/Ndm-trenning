cmake_minimum_required(VERSION 4.2)
project(QMI_modem)

set(CMAKE_CXX_STANDARD 17)

add_executable(QMI_modem main.cpp
        src/modem/ModemState.cpp
        include/modem/ModemState.h
        src/at/Parser.cpp
        include/at/Parser.h
        src/at/Command.cpp
        include/at/Command.h
        src/at/dispatcher.cpp
        include/at/dispatcher.h
        src/modem/Modem.cpp
        include/modem/Modem.h
        include/transport/Tty.h
        src/transport/Tty.cpp
        include/transport/epoll.h
        src/transport/epoll.cpp)
