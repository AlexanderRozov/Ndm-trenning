#pragma once

#include <cstdint>

#include "modem/event_bus.hpp"

#include "qmi/dispatcher.hpp"
#include "qmi/frame_parser.hpp"
#include "qmi/message.hpp"

#include "transport/outbox.hpp"
#include "transport/tty.hpp"

namespace qmi
{

class Session
{
public:
    Session(
        transport::Tty socket,
        Dispatcher& dispatcher,
        modem::EventBus& eventBus);

    Session(
        const Session&) = delete;

    Session& operator=(
        const Session&) = delete;

    int fd() const noexcept;

    bool hasPendingOutput()
        const noexcept;

    bool onReadable(
        std::uint32_t events);

    bool onWritable(
        std::uint32_t events);
private:
    void onEvent(
        const modem::ModemEvent& event);

    void queueMessage(
        const Message& message);

    transport::Tty socket_;

    // Парсит входящие бинарные фреймы.
    FrameParser parser_;

    // Очередь исходящих сообщений.
    transport::Outbox outbox_;

    Dispatcher& dispatcher_;

    // Подписка на domain события.
    modem::EventBus::Subscription
        subscription_;
};

} // namespace qmi