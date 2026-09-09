#include "transport/outbox.hpp"

#include <cerrno>
#include <unistd.h>

namespace transport
{

void Outbox::push(
    std::span<const std::uint8_t> data)
{
    if (data.empty())
        return;

    Chunk chunk;

    // Копируем данные в чанк.
    //
    // Внимание: vector не владеет памятью
    // span, поэтому assign создаёт копию.
    chunk.data.assign(
        data.begin(),
        data.end());

    queue_.push_back(
        std::move(chunk));
}

bool Outbox::empty() const noexcept
{
    return queue_.empty();
}

bool Outbox::flush(int fd)
{
    while (!queue_.empty())
    {
        Chunk& chunk =
            queue_.front();

        const std::size_t remaining =
            chunk.data.size()
            - chunk.offset;

        const ssize_t written =
            ::write(
                fd,
                chunk.data.data()
                    + chunk.offset,
                remaining);

        if (written > 0)
        {
            chunk.offset +=
                static_cast<
                    std::size_t>(
                    written);

            // Чанк полностью отправлен.
            if (chunk.offset ==
                chunk.data.size())
            {
                queue_.pop_front();
            }

            continue;
        }

        // Прервано сигналом.
        if (written == -1 &&
            errno == EINTR)
        {
            continue;
        }

        // Сокет/TTY не готов к записи.
        //
        // Нужно подождать EPOLLOUT.
        if (written == -1 &&
            (errno == EAGAIN ||
             errno == EWOULDBLOCK))
        {
            return true;
        }

        // Ошибка.
        return false;
    }

    return true;
}

} // namespace transport