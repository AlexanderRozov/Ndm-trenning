#pragma once

#include <cstdint>

namespace qmi
{

// QMI service IDs.
//
// Соответствуют официальным Qualcomm QMI ID.
// Используются в protocol layer.
enum class Service : std::uint16_t
{
    Control = 0,
    Nas = 1,
    Wds = 2,
    Dms = 3,
    Uim = 4
};

enum class NasMessage : std::uint16_t
{
    GetSignal = 1,

    GetRegistration = 2,

    GetOperator = 3,

    StartNetwork = 4
};

enum class NasTlv : std::uint16_t
{
    SignalStrength = 1,

    Registration = 2,

    Operator = 3,

    Technology = 4
};

enum class Flags : std::uint8_t
{
    Request = 0x01,

    Response = 0x02,

    // Unsolicited indication.
    Indication = 0x04
};

} // namespace qmi