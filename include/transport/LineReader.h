#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace transport
{

    class LineReader
    {
    public:
        void push(std::string_view data);

        std::optional<std::string> next();

    private:
        std::string buffer_;
    };

}