#pragma once

#include <string>
#include <vector>

namespace at
{

enum class CommandType
{
    Basic,
    Query,
    Set
};

struct Command
{
    std::string name;

    CommandType type;

    std::vector<std::string>
        arguments;
};

} // namespace at