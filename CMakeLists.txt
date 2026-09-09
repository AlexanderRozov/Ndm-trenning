cmake_minimum_required(VERSION 3.16)
project(mini_modem
    VERSION 0.1
    LANGUAGES CXX
)

# Требуем C++20:
# std::span используется для zero-copy байтовых буферов.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(mini-modemd
     src/main.cpp
     # Modem/domain
     src/modem/modem.cpp
     src/modem/event_bus.cpp

     # AT protocol
     src/at/parser.cpp
     src/at/session.cpp
     src/at/server.cpp

     # QMI-like protocol
     src/qmi/codec.cpp
     src/qmi/dispatcher.cpp
     src/qmi/frame_parser.cpp
     src/qmi/session.cpp
     src/qmi/server.cpp

     # Linux transport
     src/transport/tty.cpp
     src/transport/line_reader.cpp
     src/transport/outbox.cpp
     src/transport/epoll.cpp
     src/transport/event_loop.cpp
     src/transport/signal_fd.cpp
     src/transport/timer_fd.cpp
)

# Заголовочные файлы в include/
target_include_directories(
    mini-modemd
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Строгие предупреждения.
# Для embedded/Linux разработки полезно включить максимум проверок.
target_compile_options(
    mini-modemd
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
)

# openpty() требует libutil на Linux.
target_link_libraries(
    mini-modemd
    PRIVATE
        util
)

# QMI client (отдельная утилита для тестов)
add_executable(qmi-client
    client/qmi-client.cpp
    src/qmi/codec.cpp
)
target_include_directories(qmi-client PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_options(qmi-client PRIVATE -Wall -Wextra -Wpedantic -std=c++20)