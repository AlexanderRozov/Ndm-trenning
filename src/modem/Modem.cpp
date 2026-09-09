//
// Created by fog54 on 09.09.2026.
//

#include "Modem.h"

#include <string>
#include <string_view>


namespace modem
{

    Modem::Modem()
    {
        dispatcher_.registerCommand(
            "",
            [](const at::Command&)
            {
                return "OK\r\n";
            }
        );

        dispatcher_.registerCommand(
            "I",
            [](const at::Command&)
            {
                return "MiniModem 0.1\r\nOK\r\n";
            }
        );

        dispatcher_.registerCommand(
            "E",
            [this](const at::Command& command)
            {
                if (command.arguments.empty())
                    return "ERROR\r\n";

                if (command.arguments[0] == "0")
                {
                    echo_ = false;
                    return std::string("OK\r\n");
                }

                if (command.arguments[0] == "1")
                {
                    echo_ = true;
                    return std::string("OK\r\n");
                }

                return std::string("ERROR\r\n");
            }
        );
    }

    std::string Modem::handle(std::string_view input)
    {
        const auto command = parser_.parse(input);

        if (!command)
            return "ERROR\r\n";

        return dispatcher_.dispatch(*command);
    }

}