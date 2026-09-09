#pragma once

#include <optional>
#include <string_view>

#include "at/command.hpp"

namespace at
{

class Parser
{
public:
    std::optional<Command>
    parse(
        std::string_view input) const;
};

} // namespace at