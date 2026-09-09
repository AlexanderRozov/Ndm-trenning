#include <optional>
#include <string_view>

#include "transport/line_reader.h"

namespace transport
{

    void LineReader::push(std::string_view data)
    {
        buffer_.append(data);
    }

    std::optional<std::string> LineReader::next()
    {
        const auto pos =
            buffer_.find_first_of("\r\n");

        if (pos == std::string::npos)
            return std::nullopt;

        std::string line =
            buffer_.substr(0, pos);

        buffer_.erase(0, pos + 1);

        return line;
    }

}