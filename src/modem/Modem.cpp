//
// Created by fog54 on 09.09.2026.
//

#include "Modem.h"

#include <string>
#include <string_view>


namespace modem
{
    ModemState Modem::state() const noexcept
    {
        return state_;
    }
    void Modem::startNetworkSearch()
    {
        if (state_ != ModemState::Ready)
        {
            return;
        }

        state_ = ModemState::SearchingNetwork;
    }

    void Modem::networkSearchComplete()
    {
        if (state_ != ModemState::SearchingNetwork)
        {
            return;
        }

        state_ = ModemState::Registered;
    }

    dispatcher_.registerCommand(
    "+COPS",
    [this](const at::Command& command)
    {
        if (command.type == at::CommandType::Query)
        {
            if (state_ == ModemState::Registered)
            {
                return std::string(
                    "+COPS: 1,2,\"25001\"\r\nOK\r\n");
            }

            return std::string(
                "+COPS: 0\r\nOK\r\n");
        }

        if (command.type == at::CommandType::Set)
        {
            if (command.arguments.empty())
            {
                return std::string("ERROR\r\n");
            }

            if (command.arguments[0] == "1")
            {
                if (state_ != ModemState::Ready)
                {
                    return std::string(
                        "ERROR\r\n");
                }

                startNetworkSearch();

                return std::string(
                    "OK\r\n");
            }
        }

        return std::string("ERROR\r\n");
    }
);
    std::string Modem::handle(std::string_view input)
    {
        const auto command = parser_.parse(input);

        if (!command)
            return "ERROR\r\n";

        return dispatcher_.dispatch(*command);
    }

}