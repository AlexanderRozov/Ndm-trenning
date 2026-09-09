#pragma once

#include "at/dispatcher.h"
#include "at/parser.h"
#include "modem/modem_state.h"

#include <string>
#include <string_view>

namespace modem
{

    class Modem
    {
    public:
        Modem();

        std::string handle(std::string_view input);

        void startNetworkSearch();

        void networkSearchComplete();

        ModemState state() const noexcept;

    private:
        at::Parser parser_;
        at::Dispatcher dispatcher_;

        ModemState state_ = ModemState::Ready;

        bool echo_ = true;
    };

}