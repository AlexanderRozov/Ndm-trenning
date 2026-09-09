#pragma once

#include <string>

namespace modem
{

// Типы событий модема.
// Одни и те же события проецируются и в AT URC, и в QMI indication.
enum class EventType
{
    NetworkRegistered,
    NetworkLost,

    SignalChanged,

    SimInserted,
    SimRemoved
};

// Данные события, несущие полезную нагрузку
// для конкретных типов событий.
struct ModemEvent
{
    EventType type;

    // Код оператора, например MCC/MNC.
    std::string operatorCode;

    // Уровень сигнала 0..100.
    int signalStrength = 0;
};

} // namespace modem