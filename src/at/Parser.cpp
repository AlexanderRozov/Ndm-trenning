#include "at/Parser.hpp"

#include <algorithm>
#include <cctype>
#include <optional>
#include <string_view>

struct Command;

namespace at
{

    std::optional<Command> Parser::parse(std::string_view input) const
    {
        if (input.empty())
            return std::nullopt;

        if (input.size() < 2)
            return std::nullopt;

        if (input[0] != 'A' || input[1] != 'T')
            return std::nullopt;

        input.remove_prefix(2);

        if (input.empty())
        {
            return Command{
                "",
                CommandType::Basic,
                {}
            };
        }

        Command command;

        if (input.back() == '?')
        {
            input.remove_suffix(1);

            command.name = std::string(input);
            command.type = CommandType::Query;

            return command;
        }

        const auto equalPos = input.find('=');

        if (equalPos != std::string_view::npos)
        {
            command.name = std::string(input.substr(0, equalPos));
            command.type = CommandType::Set;

            const auto args = input.substr(equalPos + 1);

            std::size_t start = 0;

            while (start < args.size())
            {
                const auto comma = args.find(',', start);

                if (comma == std::string_view::npos)
                {
                    command.arguments.emplace_back(
                        args.substr(start)
                    );

                    break;
                }

                command.arguments.emplace_back(
                    args.substr(start, comma - start)
                );

                start = comma + 1;
            }

            return command;
        }

        command.name = std::string(input);
        command.type = CommandType::Basic;

        return command;
    }

}
