#include "modem/modem.hpp"

namespace modem
{

Modem::Modem(
    EventBus& eventBus,
    NetworkSearchCallback callback)
    : eventBus_(eventBus),
      networkSearchCallback_(
          std::move(callback))
{
}

bool Modem::startNetworkSearch()
{
    // Переход только из READY.
    if (state_ != ModemState::Ready)
        return false;

    state_ =
        ModemState::SearchingNetwork;

    // Domain layer не знает о таймерах:
    // "Кто-то запустит таймер".
    //
    // Инфраструктура (main) решает,
    // как это сделать (timerfd, mock и т.д.).
    if (networkSearchCallback_)
    {
        networkSearchCallback_();
    }

    return true;
}

bool Modem::networkSearchComplete()
{
    if (state_ !=
        ModemState::SearchingNetwork)
    {
        return false;
    }

    state_ =
        ModemState::Registered;

    // Domain event.
    //
    // AT и QMI получат это событие
    // через EventBus и сконвертируют
    // в свои форматы (URC / indication).
    eventBus_.publish({
        EventType::NetworkRegistered,
        operatorCode_,
        signalStrength_
    });

    return true;
}

ModemState Modem::state()
    const noexcept
{
    return state_;
}

bool Modem::isRegistered()
    const noexcept
{
    return state_ ==
        ModemState::Registered;
}

int Modem::signalStrength()
    const noexcept
{
    return signalStrength_;
}

const std::string& Modem::operatorCode()
    const noexcept
{
    return operatorCode_;
}

std::string Modem::handleAt(
    std::string_view command)
{
    // Базовые команды.
    if (command == "AT")
    {
        return "OK\r\n";
    }

    // Информация о модели.
    if (command == "ATI")
    {
        return
            "mini-modem 1.0\r\n"
            "C++20 Linux emulator\r\n"
            "OK\r\n";
    }

    // Echo ON.
    if (command == "ATE1")
    {
        echo_ = true;

        return "OK\r\n";
    }

    // Echo OFF.
    if (command == "ATE0")
    {
        echo_ = false;

        return "OK\r\n";
    }

    // Текущий оператор.
    if (command == "AT+COPS?")
    {
        if (!isRegistered())
        {
            return
                "+COPS: 0\r\n"
                "OK\r\n";
        }

        return
            "+COPS: 1,2,\""
            + operatorCode_
            + "\"\r\n"
            "OK\r\n";
    }

    // Запуск поиска сети.
    if (command == "AT+COPS=1")
    {
        if (!startNetworkSearch())
        {
            return "ERROR\r\n";
        }

        return "OK\r\n";
    }

    // Качество сигнала.
    if (command == "AT+CSQ")
    {
        return
            "+CSQ: "
            + std::to_string(
                signalStrength_)
            + ",99\r\n"
            "OK\r\n";
    }

    // Неизвестная команда.
    return "ERROR\r\n";
}

} // namespace modem