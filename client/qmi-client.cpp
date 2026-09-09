#include <cstdint>
#include <cstring>
#include <iostream>
#include <span>

#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>

#include "qmi/codec.hpp"
#include "qmi/message.hpp"
#include "qmi/protocol.hpp"

int main()
{
    // Подключаемся к Unix Domain Socket.
    const int fd =
        ::socket(
            AF_UNIX,
            SOCK_STREAM,
            0);

    if (fd == -1)
    {
        std::cerr
            << "socket failed\n";

        return 1;
    }

    sockaddr_un address{};

    address.sun_family =
        AF_UNIX;

    std::strncpy(
        address.sun_path,
        "/tmp/mini-modem-qmi.sock",
        sizeof(address.sun_path) - 1);

    if (::connect(
            fd,
            reinterpret_cast<
                sockaddr*>(
                &address),
            sizeof(address)) == -1)
    {
        std::cerr
            << "connect failed\n";

        ::close(fd);

        return 1;
    }

    // Формируем QMI-like запрос:
    //
    // Service    = NAS
    // Message    = GetSignal
    // Transaction = 42
    qmi::Message request;

    request.header.version =
        1;

    request.header.flags =
        static_cast<
            std::uint8_t>(
            qmi::Flags::Request);

    request.header.transactionId =
        42;

    request.header.serviceId =
        static_cast<
            std::uint16_t>(
            qmi::Service::Nas);

    request.header.messageId =
        static_cast<
            std::uint16_t>(
            qmi::NasMessage::GetSignal);

    // Request без TLV.
    const auto packet =
        qmi::encodeMessage(
            request);

    const ssize_t written =
        ::write(
            fd,
            packet.data(),
            packet.size());

    if (written !=
        static_cast<ssize_t>(
            packet.size()))
    {
        std::cerr
            << "write failed\n";

        ::close(fd);

        return 1;
    }

    std::uint8_t buffer[4096];

    const ssize_t received =
        ::read(
            fd,
            buffer,
            sizeof(buffer));

    if (received <= 0)
    {
        std::cerr
            << "read failed\n";

        ::close(fd);

        return 1;
    }

    qmi::Message response;

    if (!qmi::decodeMessage(
            std::span<
                const std::uint8_t>(
                buffer,
                static_cast<
                    std::size_t>(
                    received)),
            response))
    {
        std::cerr
            << "decode failed\n";

        ::close(fd);

        return 1;
    }

    std::cout
        << "transaction = "
        << response.header
            .transactionId
        << '\n';

    for (const auto& tlv :
         response.tlvs)
    {
        std::cout
            << "TLV type = "
            << tlv.type
            << '\n';

        if (tlv.type ==
            static_cast<
                std::uint16_t>(
                qmi::NasTlv::
                    SignalStrength))
        {
            if (!tlv.value.empty())
            {
                std::cout
                    << "signal = "
                    << static_cast<int>(
                        tlv.value[0])
                    << '\n';
            }
        }
    }

    ::close(fd);

    return 0;
}