#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include "qmi/message.hpp"

namespace qmi
{

// Последовательный reader.
//
// Избегает reinterpret_cast<Header*>.
//
// Целочисленные поля читаются побайтово,
// что безопасно для alignment, endianness
// и strict aliasing/object lifetime.
class ByteReader
{
public:
    explicit ByteReader(
        std::span<const std::uint8_t> data);

    bool readU8(
        std::uint8_t& value);

    bool readU16(
        std::uint16_t& value);

    bool readBytes(
        std::span<const std::uint8_t>& value,
        std::size_t size);

    std::size_t remaining()
        const noexcept;
private:
    // span не владеет данными.
    std::span<const std::uint8_t>
        data_;
};

class ByteWriter
{
public:
    void writeU8(
        std::uint8_t value);

    void writeU16(
        std::uint16_t value);

    void writeBytes(
        std::span<const std::uint8_t> data);

    const std::vector<std::uint8_t>&
    data() const noexcept;
private:
    std::vector<std::uint8_t>
        buffer_;
};

bool decodeMessage(
    std::span<const std::uint8_t> bytes,
    Message& message);

std::vector<std::uint8_t> encodeMessage(
    const Message& message);

} // namespace qmi