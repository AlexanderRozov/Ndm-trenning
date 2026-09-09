#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "modem/event_bus.hpp"
#include "modem/modem.hpp"

#include "qmi/dispatcher.hpp"
#include "qmi/session.hpp"

#include "transport/event_loop.hpp"

namespace qmi
{

class Server
{
public:
    Server(
        const std::string& path,
        modem::Modem& modem,
        modem::EventBus& eventBus,
        transport::EventLoop& loop);

    ~Server();

    Server(
        const Server&) = delete;

    Server& operator=(
        const Server&) = delete;
private:
    void acceptClients(
        std::uint32_t events);

    void handleClient(
        int fd,
        std::uint32_t events);

    void updateEvents(
        int fd);

    void removeClient(
        int fd);
private:
    int listenFd_ = -1;

    std::string path_;

    modem::EventBus& eventBus_;

    // Один Dispatcher на все сессии,
    // так как он stateless.
    Dispatcher dispatcher_;

    transport::EventLoop& loop_;

    // Server владеет Sessions.
    std::unordered_map<
        int,
        std::unique_ptr<Session>> sessions_;
};

} // namespace qmi