#include "at/server.hpp"

#include <stdexcept>

#include <pty.h>
#include <unistd.h>

#include <sys/epoll.h>

namespace at
{

Server::Server(
    modem::Modem& modem,
    modem::EventBus& eventBus,
    transport::EventLoop& loop)
    : modem_(modem),
      eventBus_(eventBus),
      loop_(loop)
{
}

Server::~Server()
{
    // Удаляем все сессии.
    //
    // removeSession() удаляет fd из epoll,
    // затем уничтожает Session.
    while (!sessions_.empty())
    {
        const int fd =
            sessions_.begin()->first;

        removeSession(fd);
    }
}

std::string Server::createPty()
{
    int masterFd = -1;
    int slaveFd = -1;

    char slaveName[128]{};

    // Создаём PTY пару:
    //
    // master <-----> slave
    //
    // Приложение открывает slave (/dev/pts/N),
    // демон читает/пишет в master.
    if (::openpty(
            &masterFd,
            &slaveFd,
            slaveName,
            nullptr,
            nullptr) == -1)
    {
        throw std::runtime_error(
            "openpty failed");
    }

    // Демон не нужен slave fd.
    //
    // PTY мастер будет закрыт при
    // уничтожении Session.
    ::close(slaveFd);

    transport::Tty tty(
        masterFd);

    auto session =
        std::make_unique<Session>(
            std::move(tty),
            modem_,
            eventBus_);

    const int fd =
        session->fd();

    sessions_.emplace(
        fd,
        std::move(session));

    // Регистрируем fd в event loop.
    //
    // Callback захватывает this и fd.
    loop_.addFd(
        fd,
        EPOLLIN |
        EPOLLHUP |
        EPOLLERR,
        [this, fd](
            std::uint32_t events)
        {
            const auto it =
                sessions_.find(fd);

            if (it ==
                sessions_.end())
            {
                return;
            }

            bool alive = true;

            // Сначала обрабатываем запись,
            // если сокет writable.
            if (events & EPOLLOUT)
            {
                alive =
                    it->second
                        ->onWritable(
                            events);
            }

            // Затем чтение.
            if (alive &&
                (events & EPOLLIN))
            {
                alive =
                    it->second
                        ->onReadable(
                            events);
            }

            if (!alive)
            {
                removeSession(fd);
                return;
            }

            updateEvents(fd);
        });

    return slaveName;
}

void Server::updateEvents(
    int fd)
{
    const auto it =
        sessions_.find(fd);

    if (it ==
        sessions_.end())
    {
        return;
    }

    std::uint32_t events =
        EPOLLIN |
        EPOLLHUP |
        EPOLLERR;

    // EPOLLOUT нужен только когда
    // есть данные в TX очереди.
    //
    // Постоянный EPOLLOUT на writable fd
    // вызывает лишние пробуждения.
    if (it->second
            ->hasPendingOutput())
    {
        events |= EPOLLOUT;
    }

    loop_.modifyFd(
        fd,
        events);
}

void Server::removeSession(
    int fd)
{
    // Сначала удаляем из epoll.
    loop_.removeFd(fd);

    // Затем уничтожаем Session:
    //
    // Session
    //   -> Subscription
    //      -> unsubscribe
    //   -> Tty
    //      -> close(fd)
    sessions_.erase(fd);
}

} // namespace at