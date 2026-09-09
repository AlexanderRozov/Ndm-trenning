#include "at/session.hpp"

#include <cerrno>
#include <span>

#include <sys/epoll.h>

namespace at
{

Session::Session(
    transport::Tty tty,
    modem::Modem& modem,
    modem::EventBus& eventBus)
    : tty_(std::move(tty)),
      modem_(modem),

      // Важно: порядок инициализации членов
      // соответствует порядку объявления в классе.
      //
      // subscription_ инициализируется последним,
      // после outbox_ и reader_.
      //
      // Это важно, потому что callback
      // в EventBus может обращаться к tty_ и outbox_.
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
    return tty_.fd();
}

bool Session::hasPendingOutput()
    const noexcept
{
    return !outbox_.empty();
}

void Session::queueText(
    std::string_view text)
{
    // Помещаем текст в outbox.
    //
    // std::string хранит char,
    // а transport работает с uint8_t.
    outbox_.push(
        std::span<const std::uint8_t>(
            reinterpret_cast<
                const std::uint8_t*>(
                text.data()),
            text.size()));
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

    char buffer[1024];

    while (true)
    {
        const ssize_t count =
            tty_.read(
                buffer,
                sizeof(buffer));

        if (count > 0)
        {
            // Данные получены, кладем в LineReader.
            reader_.push(
                std::string_view(
                    buffer,
                    static_cast<
                        std::size_t>(
                        count)));

            // Обрабатываем все полные строки.
            while (
                const auto line =
                    reader_.next())
            {
                const std::string response =
                    modem_.handleAt(
                        *line);

                queueText(response);
            }

            continue;
        }

        if (count == -1 &&
            errno == EINTR)
        {
            continue;
        }

        // Non-blocking fd: нет данных сейчас.
        if (count == -1 &&
            (errno == EAGAIN ||
             errno == EWOULDBLOCK))
        {
            break;
        }

        // EOF / ошибка.
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
        tty_.fd());
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

    // Domain event ->
    // AT unsolicited result code.
    queueText(
        "\r\n+COPS: 1,2,\""
        + event.operatorCode
        + "\"\r\n");
}

} // namespace at