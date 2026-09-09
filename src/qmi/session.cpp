#include "qmi/session.hpp"

#include <cerrno>

#include <sys/epoll.h>

#include "qmi/codec.hpp"
#include "qmi/protocol.hpp"

namespace qmi
{

Session::Session(
    transport::Tty socket,
    Dispatcher& dispatcher,
    modem::EventBus& eventBus)
    : socket_(std::move(socket)),
      dispatcher_(dispatcher),

      // Session подписывается на события
      // при создании.
      subscription_(
          eventBus.subscribe(
              [this](
                  const modem::ModemEvent& event)
              {
                  onEvent(event);
              }))
{
}

int Session::fd() const noexcept
{
    return socket_.fd();
}

bool Session::hasPendingOutput()
    const noexcept
{
    return !outbox_.empty();
}

void Session::queueMessage(
    const Message& message)
{
    // Message -> wire bytes.
    const auto bytes =
        encodeMessage(
            message);

    // Кладем в outbox.
    //
    // bytes - это vector, который
    // перемещается в Chunk.
    outbox_.push(bytes);
}

bool Session::onReadable(
    std::uint32_t events)
{
    if (events &
        (EPOLLHUP | EPOLLERR))
    {
        return false;
    }

    if (!(events & EPOLLIN))
    {
        return true;
    }

    std::uint8_t buffer[4096];

    while (true)
    {
        const ssize_t count =
            socket_.read(
                reinterpret_cast<char*>(
                    buffer),
                sizeof(buffer));

        if (count > 0)
        {
            // Кладем в парсер.
            parser_.push(
                std::span<
                    const std::uint8_t>(
                    buffer,
                    static_cast<
                        std::size_t>(
                        count)));

            // read() может вернуть
            // несколько фреймов сразу.
            while (
                const auto request =
                    parser_.next())
            {
                // Request -> domain operation.
                const Message response =
                    dispatcher_.dispatch(
                        *request);

                // Domain response ->
                // binary protocol response.
                queueMessage(
                    response);
            }

            continue;
        }

        if (count == -1 &&
            errno == EINTR)
        {
            continue;
        }

        if (count == -1 &&
            (errno == EAGAIN ||
             errno == EWOULDBLOCK))
        {
            break;
        }

        // 0 = peer закрыл соединение.
        return false;
    }

    return true;
}

bool Session::onWritable(
    std::uint32_t events)
{
    if (events &
        (EPOLLHUP | EPOLLERR))
    {
        return false;
    }

    if (!(events & EPOLLOUT))
    {
        return true;
    }

    return outbox_.flush(
        socket_.fd());
}

void Session::onEvent(
    const modem::ModemEvent& event)
{
    if (event.type !=
        modem::EventType::
            NetworkRegistered)
    {
        return;
    }

    Message indication;

    indication.header.version =
        1;
    indication.header.flags =
        static_cast<
            std::uint8_t>(
            Flags::Indication);

    // Unsolicited indication
    // не имеет request, поэтому
    // transactionId = 0.
    //
    // Это позволяет клиенту отличить
    // indication от response.
    indication.header.transactionId =
        0;
    indication.header.serviceId =
        static_cast<
            std::uint16_t>(
            Service::Nas);
    indication.header.messageId =
        static_cast<
            std::uint16_t>(
            NasMessage::
                GetRegistration);

    Tlv registration;

    registration.type =
        static_cast<
            std::uint16_t>(
            NasTlv::Registration);

    registration.value.push_back(
        1);

    indication.tlvs.push_back(
        std::move(registration));

    queueMessage(
        indication);
}

} // namespace qmi