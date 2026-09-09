#pragma once

#include <functional>
#include <string>
#include <string_view>

#include "modem/event_bus.hpp"
#include "modem/modem_state.hpp"

namespace modem
{

class Modem
{
public:
    // Callback, который вызывается когда FSM
    // переходит в состояние поиска сети.
    //
    // В main() этот callback армит timerfd.
    using NetworkSearchCallback =
        std::function<void()>;

    Modem(
        EventBus& eventBus,
        NetworkSearchCallback callback);

    // Запуск поиска сети.
    bool startNetworkSearch();

    // Завершение поиска сети (вызывается по таймеру).
    bool networkSearchComplete();

    ModemState state() const noexcept;

    bool isRegistered() const noexcept;

    int signalStrength() const noexcept;

    const std::string&
    operatorCode() const noexcept;

    // Обработка AT команд.
    //
    // В продакшене здесь должен быть Parser + Dispatcher.
    std::string handleAt(
        std::string_view command);
private:
    EventBus& eventBus_;

    NetworkSearchCallback
        networkSearchCallback_;

    ModemState state_ =
        ModemState::Ready;

    int signalStrength_ =
        75;

    std::string operatorCode_ =
        "25001";

    bool echo_ =
        true;
};

} // namespace modem