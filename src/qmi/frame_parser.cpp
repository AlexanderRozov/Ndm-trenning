#include "qmi/frame_parser.hpp"

#include "qmi/codec.hpp"

namespace qmi
{

void FrameParser::push(
    std::span<const std::uint8_t> data)
{
    // Socket может прислать:
    //
    // целый фрейм
    // половину фрейма
    // несколько фреймов сразу
    //
    // Всё копируем в буфер.
    buffer_.insert(
        buffer_.end(),
        data.begin(),
        data.end());
}

std::optional<Message>
FrameParser::next()
{
    // Header должен быть полностью в буфере.
    if (buffer_.size() <
        HeaderSize)
    {
        return std::nullopt;
    }

    // payload_length в байтах 8 и 9:
    //
    // byte 8
    // byte 9
    // little endian.
    const std::uint16_t payloadLength =
        static_cast<std::uint16_t>(
            buffer_[8]) |
        static_cast<std::uint16_t>(
            static_cast<std::uint16_t>(
                buffer_[9])
            << 8);

    const std::size_t frameSize =
        HeaderSize +
        payloadLength;

    // Header есть, но фрейм
    // ещё не بالكامل в буфере.
    if (buffer_.size() <
        frameSize)
    {
        return std::nullopt;
    }

    const std::span<
        const std::uint8_t>
        frame(
            buffer_.data(),
            frameSize);

    Message message;

    if (!decodeMessage(
            frame,
            message))
    {
        // При ошибке декодирования
        // очищаем буфер, чтобы
        // не зациклиться на плохом фрейме.
        buffer_.clear();

        return std::nullopt;
    }

    // Удаляем обработанный фрейм.
    //
    // Остальные байты (если есть)
    // останутся для следующего next().
    buffer_.erase(
        buffer_.begin(),
        buffer_.begin() +
        static_cast<
            std::ptrdiff_t>(
            frameSize));

    return message;
}

} // namespace qmi