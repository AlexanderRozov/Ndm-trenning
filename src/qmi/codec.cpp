#include "qmi/codec.hpp"

namespace qmi
{

ByteReader::ByteReader(
    std::span<const std::uint8_t> data)
    : data_(data)
{
}

bool ByteReader::readU8(
    std::uint8_t& value)
{
    if (data_.empty())
    {
        return false;
    }

    value =
        data_[0];

    data_ =
        data_.subspan(1);

    return true;
}

bool ByteReader::readU16(
    std::uint16_t& value)
{
    // Проверяем, что есть 2 байта
    // перед чтением.
    if (data_.size() < 2)
    {
        return false;
    }

    // Little endian:
    //
    // 0x34 0x12
    //
    // Значение: 0x1234
    value =
        static_cast<std::uint16_t>(
            data_[0]) |
        static_cast<std::uint16_t>(
            static_cast<std::uint16_t>(
                data_[1])
            << 8);

    data_ =
        data_.subspan(2);

    return true;
}

bool ByteReader::readBytes(
    std::span<const std::uint8_t>& value,
    std::size_t size)
{
    if (data_.size() < size)
    {
        return false;
    }

    // Zero-copy view.
    value =
        data_.first(size);

    data_ =
        data_.subspan(size);

    return true;
}

std::size_t ByteReader::remaining()
    const noexcept
{
    return data_.size();
}

void ByteWriter::writeU8(
    std::uint8_t value)
{
    buffer_.push_back(
        value);
}

void ByteWriter::writeU16(
    std::uint16_t value)
{
    // Little endian.
    buffer_.push_back(
        static_cast<std::uint8_t>(
            value & 0xff));

    buffer_.push_back(
        static_cast<std::uint8_t>(
            value >> 8));
}

void ByteWriter::writeBytes(
    std::span<const std::uint8_t> data)
{
    buffer_.insert(
        buffer_.end(),
        data.begin(),
        data.end());
}

const std::vector<std::uint8_t>&
ByteWriter::data() const noexcept
{
    return buffer_;
}

namespace {

bool decodeTlv(
    ByteReader& reader,
    Tlv& tlv)
{
    std::uint16_t length = 0;

    if (!reader.readU16(
            tlv.type))
    {
        return false;
    }

    if (!reader.readU16(
            length))
    {
        return false;
    }

    // Проверяем, что length байт
    // доступно в буфере.
    if (reader.remaining() <
        length)
    {
        return false;
    }

    std::span<
        const std::uint8_t>
        value;

    if (!reader.readBytes(
            value,
            length))
    {
        return false;
    }

    // Копируем в owned vector,
    // так как исходный буфер
    // может быть переиспользован.
    tlv.value.assign(
        value.begin(),
        value.end());

    return true;
}

void encodeTlv(
    ByteWriter& writer,
    const Tlv& tlv)
{
    writer.writeU16(
        tlv.type);

    writer.writeU16(
        static_cast<
            std::uint16_t>(
            tlv.value.size()));

    writer.writeBytes(
        tlv.value);
}

bool decodeHeader(
    ByteReader& reader,
    Header& header)
{
    if (!reader.readU8(
            header.version))
        return false;

    if (!reader.readU8(
            header.flags))
        return false;

    if (!reader.readU16(
            header.transactionId))
        return false;

    if (!reader.readU16(
            header.serviceId))
        return false;

    if (!reader.readU16(
            header.messageId))
        return false;

    if (!reader.readU16(
            header.payloadLength))
        return false;

    return true;
}

void encodeHeader(
    ByteWriter& writer,
    const Header& header)
{
    writer.writeU8(
        header.version);

    writer.writeU8(
        header.flags);

    writer.writeU16(
        header.transactionId);

    writer.writeU16(
        header.serviceId);

    writer.writeU16(
        header.messageId);

    writer.writeU16(
        header.payloadLength);
}

} // namespace

bool decodeMessage(
    std::span<const std::uint8_t> bytes,
    Message& message)
{
    ByteReader reader(bytes);

    // Декодируем header = 10 байт.
    // decodeHeader() читает побайтово,
    // так что alignment не проблема.
    if (!decodeHeader(
            reader,
            message.header))
    {
        return false;
    }

    // Проверяем, что payload
    // полностью доступен.
    if (reader.remaining() <
        message.header.payloadLength)
    {
        return false;
    }

    std::span<
        const std::uint8_t>
        payload;

    if (!reader.readBytes(
            payload,
            message.header.payloadLength))
    {
        return false;
    }

    ByteReader payloadReader(
        payload);

    message.tlvs.clear();

    while (
        payloadReader.remaining() >
        0)
    {
        Tlv tlv;

        if (!decodeTlv(
                payloadReader,
                tlv))
        {
            return false;
        }

        message.tlvs.push_back(
            std::move(tlv));
    }

    return true;
}

std::vector<std::uint8_t>
encodeMessage(
    const Message& message)
{
    // Сначала кодируем payload.
    ByteWriter payloadWriter;

    for (const Tlv& tlv :
         message.tlvs)
    {
        encodeTlv(
            payloadWriter,
            tlv);
    }

    const auto& payload =
        payloadWriter.data();

    // Затем header с правильной
    // payloadLength.
    Header header =
        message.header;

    header.payloadLength =
        static_cast<
            std::uint16_t>(
            payload.size());

    // Собираем итоговый буфер:
    // header + payload.
    ByteWriter writer;

    encodeHeader(
        writer,
        header);

    writer.writeBytes(
        payload);

    return writer.data();
}

} // namespace qmi