#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <span>
#include <vector>

namespace transport
{

// Очередь исходящих данных.
//
// Решает проблему non-blocking сокета:
// write() может записать только часть данных.
//
// Данные ставятся в очередь, flush() отправляет
// столько, сколько можно, и возвращает true,
// если очередь пуста, false если нужно ждать EPOLLOUT.
class Outbox
{
public:
    void push(
        std::span<const std::uint8_t> data);

    bool empty() const noexcept;

    // Отправляет данные в fd.
    //
    // true  -> очередь пуста
    // false -> есть неотправленные данные, нужен EPOLLOUT
    bool flush(
        int fd);
private:
    struct Chunk
    {
        std::vector<std::uint8_t> data;

        // Сколько байт уже отправлено из этого чанка.
        std::size_t offset = 0;
    };

    std::deque<Chunk>
        queue_;
};

} // namespace transport