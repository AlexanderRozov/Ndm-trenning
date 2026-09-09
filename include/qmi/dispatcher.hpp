#pragma once

#include "modem/modem.hpp"

#include "qmi/message.hpp"

namespace qmi
{

class Dispatcher
{
public:
    explicit Dispatcher(
        modem::Modem& modem);

    Message dispatch(
        const Message& request);
private:
    Message handleNas(
        const Message& request);

    Message handleGetSignal(
        const Message& request);

    Message handleGetRegistration(
        const Message& request);

    Message handleGetOperator(
        const Message& request);
private:
    // Dispatcher не владеет Modem.
    modem::Modem& modem_;
};

} // namespace qmi