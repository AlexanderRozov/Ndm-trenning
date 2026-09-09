#include "at/parser.hpp"

#include <string>

namespace at
{

std::optional<Command> Parser::parse(
    std::string_view input) const
{
    // AT-команда должна начинаться с "AT".
    if (input.size() < 2)
    {
        return std::nullopt;
    }

    if (input.substr(0, 2) !=
        "AT")
    {
        return std::nullopt;
    }

    input.remove_prefix(2);

    // Пустая команда "AT".
    if (input.empty())
    {
        return Command{
            "",
            CommandType::Basic,
            {}
        };
    }

    Command command;

    // AT+CSQ?
    if (input.back() == '?')
    {
        input.remove_suffix(1);

        command.type =
            CommandType::Query;
    }
    else
    {
        command.type =
            CommandType::Basic;
    }

    // AT+COPS=1,2,25001
    const auto equal =
        input.find('=');

    if (equal !=
        std::string_view::npos)
    {
        command.type =
            CommandType::Set;

        command.name =
            std::string(
                input.substr(
                    0,
                    equal));

        input.remove_prefix(
            equal + 1);

        while (!input.empty())
        {
            const auto comma =
                input.find(',');

            if (comma ==
                std::string_view::npos)
            {
                command.arguments.emplace_back(
                    input);

                break;
            }

            command.arguments.emplace_back(
                input.substr(
                    0,
                    comma));

            input.remove_prefix(
                comma + 1);
        }

        return command;
    }

    command.name =
        std::string(input);

    return command;
}

} // namespace at