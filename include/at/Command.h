//
// Created by fog54 on 09.09.2026.
//

#ifndef QMI_MODEM_COMMAND_H
#define QMI_MODEM_COMMAND_H
#include <string>
#include <vector>


enum class CommandType {
  Basic,
  Query,
  Set
};

struct  Command
{
    std::string name;
    CommandType type;
    std::vector<std::string> params;
};


#endif //QMI_MODEM_COMMAND_H
