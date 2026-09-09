#include "qmi/dispatcher.hpp"

#include "qmi/protocol.hpp"

namespace qmi
{

Dispatcher::Dispatcher(
    modem::Modem& modem)
    : modem_(modem)
{
}

Message Dispatcher::dispatch(
    const Message& request)
{
    if (request.header.serviceId ==
        static_cast<std::uint16_t>(
            Service::Nas))
    {
        return handleNas(
            request);
    }

    // Неизвестный сервис.
    //
    // В реальном QMI здесь должен быть
    // result/error TLV.
    Message response;

    response.header.version = 1;
    response.header.flags =
        static_cast<
            std::uint8_t>(
            Flags::Response);

    // Response копирует transaction ID
    // из request.
    response.header.transactionId =
        request.header.transactionId;

    response.header.serviceId =
        request.header.serviceId;

    response.header.messageId =
        request.header.messageId;

    return response;
}

Message Dispatcher::handleNas(
    const Message& request)
{
    switch (
        static_cast<NasMessage>(
            request.header.messageId))
    {
        case NasMessage::GetSignal:
            return handleGetSignal(
                request);

        case NasMessage::GetRegistration:
            return handleGetRegistration(
                request);

        case NasMessage::GetOperator:
            return handleGetOperator(
                request);

        default:
            break;
    }

    Message response;

    response.header.version = 1;
    response.header.flags =
        static_cast<
            std::uint8_t>(
            Flags::Response);

    response.header.transactionId =
        request.header.transactionId;

    response.header.serviceId =
        request.header.serviceId;

    response.header.messageId =
        request.header.messageId;

    return response;
}

Message Dispatcher::handleGetSignal(
    const Message& request)
{
    Message response;

    response.header.version = 1;

    response.header.flags =
        static_cast<
            std::uint8_t>(
            Flags::Response);

    // Response копирует transaction ID.
    response.header.transactionId =
        request.header.transactionId;

    response.header.serviceId =
        static_cast<
            std::uint16_t>(
            Service::Nas);

    response.header.messageId =
        static_cast<
            std::uint16_t>(
            NasMessage::GetSignal);

    Tlv signal;

    signal.type =
        static_cast<
            std::uint16_t>(
            NasTlv::SignalStrength);

    signal.value.push_back(
        static_cast<
            std::uint8_t>(
            modem_.signalStrength()));

    response.tlvs.push_back(
        std::move(signal));

    return response;
}

Message Dispatcher::handleGetRegistration(
    const Message& request)
{
    Message response;

    response.header.version = 1;
    response.header.flags =
        static_cast<
            std::uint8_t>(
            Flags::Response);

    response.header.transactionId =
        request.header.transactionId;

    response.header.serviceId =
        static_cast<
            std::uint16_t>(
            Service::Nas);

    response.header.messageId =
        static_cast<
            std::uint16_t>(
            NasMessage::GetRegistration);

    Tlv registration;

    registration.type =
        static_cast<
            std::uint16_t>(
            NasTlv::Registration);

    registration.value.push_back(
        modem_.isRegistered()
            ? 1
            : 0);

    response.tlvs.push_back(
        std::move(registration));

    return response;
}

Message Dispatcher::handleGetOperator(
    const Message& request)
{
    Message response;

    response.header.version = 1;
    response.header.flags =
        static_cast<
            std::uint8_t>(
            Flags::Response);

    response.header.transactionId =
        request.header.transactionId;

    response.header.serviceId =
        static_cast<
            std::uint16_t>(
            Service::Nas);

    response.header.messageId =
        static_cast<
            std::uint16_t>(
            NasMessage::GetOperator);

    Tlv operatorTlv;

    operatorTlv.type =
        static_cast<
            std::uint16_t>(
            NasTlv::Operator);

    const std::string& code =
        modem_.operatorCode();

    operatorTlv.value.assign(
        code.begin(),
        code.end());

    response.tlvs.push_back(
        std::move(operatorTlv));

    return response;
}

} // namespace qmi