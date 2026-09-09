#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace transport
{

// AT протокол работает с текстовыми строками.
// Transport может отдавать данные фрагментами:
//
// AT+COPS=1\r\nAT+CSQ\r\n
//
// LineReader накапливает байты и выдаёт
// полные строки по разделителям \r\n.
class LineReader
{
public:
    void push(
        std::string_view data);

    std::optional<std::string>
    next();
private:
    // Буфер недопрочитанных байтов.
    std::string buffer_;
};

} // namespace transport