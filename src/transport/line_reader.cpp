#include "transport/line_reader.hpp"

namespace transport
{

void LineReader::push(
    std::string_view data)
{
    buffer_.append(data);
}

std::optional<std::string>
LineReader::next()
{
    // Ищем конец строки AT.
    const auto pos =
        buffer_.find_first_of(
            "\r\n");

    // Полной строки нет.
    if (pos == std::string::npos)
    {
        return std::nullopt;
    }

    // Извлекаем строку.
    std::string line =
        buffer_.substr(
            0,
            pos);

    // Пропускаем CR/LF.
    std::size_t consumed =
        pos + 1;

    // Поддержка CRLF и LFLF.
    //
    // AT\r\n\r\n
    //
    // Вторая пустая строка тоже
    // должна быть прочитана.
    while (
        consumed < buffer_.size() &&
        (buffer_[consumed] == '\r' ||
         buffer_[consumed] == '\n'))
    {
        ++consumed;
    }

    buffer_.erase(
        0,
        consumed);

    return line;
}

} // namespace transport