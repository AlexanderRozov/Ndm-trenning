#pragma once

#include <cstdint>
#include <vector>

namespace qmi
{

// Wire header:
//
// byte 0      version
// byte 1      flags
// byte 2..3   transaction_id
// byte 4..5   service_id
// byte 6..7   message_id
// byte 8..9   payload_length
//
// Всего 10 байт.
struct Header
{
    std::uint8_t version = 1;
    std::uint8_t flags = 0;

    std::uint16_t transactionId = 0;

    std::uint16_t serviceId = 0;
    std::uint16_t messageId = 0;

    std::uint16_t payloadLength = 0;
};

// TLV:
//
// type
// length
// value[]
//
// Все целые в little endian.
struct Tlv
{
    std::uint16_t type = 0;

    // Tlv владеет value.
    //
    // При декодировании: буфер из socket/read()
    // временно живет в span, затем копируется
    // в vector для безопасности lifetime.
    std::vector<std::uint8_t> value;
};

struct Message
{
    Header header;

    std::vector<Tlv> tlvs;
};

} // namespace qmi