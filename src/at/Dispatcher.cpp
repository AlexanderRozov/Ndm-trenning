#include <string>

#include "at/dispatcher.hpp"

struct Command;

namespace at
{

    void Dispatcher::registerCommand(
        std::string name,
        Handler handler)
    {
        handlers_.insert_or_assign(
            std::move(name),
            std::move(handler)
        );
    }

    std::string Dispatcher::dispatch(
        const Command& command) const
    {
        if (command.name.empty())
        {
            return "OK\r\n";
        }

        const auto it = handlers_.find(command.name);

        if (it == handlers_.end())
        {
            return "ERROR\r\n";
        }

        return it->second(command);
    }

}
