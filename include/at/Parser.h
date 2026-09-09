//
// Created by fog54 on 09.09.2026.
//

#ifndef QMI_MODEM_COMMAND_H
#define QMI_MODEM_COMMAND_H



#include "Command.h"
#include <optional>
#include <string_view>

namespace at {
class Parser {
public:
    std::optional<Command> parse(std::string_view input) const;
};

}

#endif //QMI_MODEM_COMMAND_H
