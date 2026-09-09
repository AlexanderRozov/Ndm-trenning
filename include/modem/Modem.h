#pragma once

#include "at/dispatcher.hpp"
#include "at/parser.hpp"

#include <string>
#include <string_view>

namespace modem
{

    class Modem
    {
    public:
        Modem();

        std::string handle(std::string_view input);

    private:
        at::Parser parser_;
        at::Dispatcher dispatcher_;

        bool echo_ = true;
    };

}