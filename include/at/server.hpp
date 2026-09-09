#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "at/session.hpp"

#include "modem/event_bus.hpp"
#include "modem/modem.hpp"

#include "transport/event_loop.hpp"

namespace at
{

class Server
{
public:
    Server(
        modem::Modem& modem,
        modem::EventBus& eventBus,
        transport::EventLoop& loop);

    ~Server();

    Server(
        const Server&) = delete;

    Server& operator=(
        const Server&) = delete;

    // Создаёт новый PTY и возвращает
    // путь к slave устройству:
    //
    // /dev/pts/7
    std::string createPty();
private:
    void removeSession(
        int fd);

    void updateEvents(
        int fd);
private:
    modem::Modem& modem_;

    modem::EventBus& eventBus_;

    transport::EventLoop& loop_;

    // Server владеет Sessions.
    //
    // Когда Server уничтожается,
    // все Sessions уничтожаются тоже.
    std::unordered_map<
        int,
        std::unique_ptr<Session>> sessions_;
};

} // namespace at