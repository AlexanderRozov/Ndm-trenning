#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <vector>

#include "qmi/message.hpp"

namespace qmi
{

// В отличие от AT LineReader:
//
// AT:
//
// bytes -> ищем '\r'
//
// QMI:
//
// bytes -> читаем payload_length из header
//
//             10-byte header
//                   |
//                   v
//             payload_length
//                   |
//                   v
//          10 + payloadLength
//
// Это length-based framing.
class FrameParser
{
public:
    void push(
        std::span<const std::uint8_t> data);

    std::optional<Message>
    next();
private:
    static constexpr std::size_t
        HeaderSize = 10;

    std::vector<std::uint8_t>
        buffer_;
};

} // namespace qmi