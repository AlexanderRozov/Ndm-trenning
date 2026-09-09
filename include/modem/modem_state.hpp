#pragma once

namespace modem
{

// Состояния модема.
// В реальном проекте enum может расширяться под более сложную FSM.
enum class ModemState
{
    Off,

    // Инициализация железа.
    Booting,

    // Модем готов к командам, но не в сети.
    Ready,

    // Поиск сети.
    SearchingNetwork,

    // Зарегистрирован в сети.
    Registered,

    // Активное data-соединение.
    DataConnected
};

} // namespace modem