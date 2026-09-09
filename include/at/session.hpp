#pragma once

#include <cstdint>

#include "modem/event_bus.hpp"
#include "modem/modem.hpp"

#include "transport/line_reader.hpp"
#include "transport/outbox.hpp"
#include "transport/tty.hpp"

namespace at
{

class Session
{
public:
    Session(
        transport::Tty tty,
        modem::Modem& modem,
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

    void queueText(
        std::string_view text);
private:
    // Session владеет transport fd.
    transport::Tty tty_;

    // Накапливает входящие AT-команды.
    transport::LineReader reader_;

    // Очередь исходящих ответов.
    transport::Outbox outbox_;

    // Modem не владеется Session.
    modem::Modem& modem_;

    // Subscription живёт пока живёт Session.
    // Callback держит указатель на this.
    modem::EventBus::Subscription
        subscription_;
};

} // namespace at