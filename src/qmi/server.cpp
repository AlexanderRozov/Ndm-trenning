#include "qmi/server.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>

namespace qmi
{

Server::Server(
    const std::string& path,
    modem::Modem& modem,
    modem::EventBus& eventBus,
    transport::EventLoop& loop)
    : path_(path),
      eventBus_(eventBus),
      dispatcher_(modem),
      loop_(loop)
{
    // AF_UNIX + SOCK_STREAM.
    //
    // SOCK_NONBLOCK:
    // read/write не блокируют,
    // управление через event loop.
    //
    // SOCK_CLOEXEC:
    // fd не утекает в exec'нутый
    // дочерний процесс.
    listenFd_ =
        ::socket(
            AF_UNIX,
            SOCK_STREAM |
            SOCK_NONBLOCK |
            SOCK_CLOEXEC,
            0);

    if (listenFd_ == -1)
    {
        throw std::runtime_error(
            "socket(AF_UNIX) failed");
    }

    // Если демон перезапустился,
    // socket файл может остаться.
    ::unlink(
        path_.c_str());

    sockaddr_un address{};

    address.sun_family =
        AF_UNIX;

    if (path_.size() >=
        sizeof(address.sun_path))
    {
        ::close(listenFd_);
        throw std::runtime_error(
            "Unix socket path too long");
    }

    std::strncpy(
        address.sun_path,
        path_.c_str(),
        sizeof(address.sun_path) - 1);

    if (::bind(
            listenFd_,
            reinterpret_cast<
                sockaddr*>(
                &address),
            sizeof(address)) == -1)
    {
        ::close(listenFd_);
        throw std::runtime_error(
            "bind failed");
    }

    if (::listen(
            listenFd_,
            32) == -1)
    {
        ::close(listenFd_);
        throw std::runtime_error(
            "listen failed");
    }

    // Регистрируем listen fd в event loop.
    loop_.addFd(
        listenFd_,
        EPOLLIN,
        [this](
            std::uint32_t events)
        {
            acceptClients(
                events);
        });
}

Server::~Server()
{
    if (listenFd_ != -1)
    {
        loop_.removeFd(
            listenFd_);

        ::close(
            listenFd_);
    }

    // Удаляем все Session.
    sessions_.clear();

    // Удаляем filesystem entry.
    ::unlink(
        path_.c_str());
}

void Server::acceptClients(
    std::uint32_t events)
{
    if (!(events & EPOLLIN))
    {
        return;
    }

    while (true)
    {
        // Non-blocking accept.
        const int fd =
            ::accept4(
                listenFd_,
                nullptr,
                nullptr,
                SOCK_NONBLOCK |
                SOCK_CLOEXEC);

        if (fd == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }

            // Нет ожидающих соединений.
            if (errno == EAGAIN ||
                errno == EWOULDBLOCK)
            {
                break;
            }

            break;
        }

        transport::Tty socket(
            fd);

        auto session =
            std::make_unique<Session>(
                std::move(socket),
                dispatcher_,
                eventBus_);

        const int clientFd =
            session->fd();

        sessions_.emplace(
            clientFd,
            std::move(session));

        loop_.addFd(
            clientFd,
            EPOLLIN |
            EPOLLHUP |
            EPOLLERR,
            [this, clientFd](
                std::uint32_t events)
            {
                handleClient(
                    clientFd,
                    events);
            });
    }
}

void Server::handleClient(
    int fd,
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

    // Затем чтение request.
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
        removeClient(
            fd);
        return;
    }

    updateEvents(
        fd);
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
    if (it->second
            ->hasPendingOutput())
    {
        events |= EPOLLOUT;
    }

    loop_.modifyFd(
        fd,
        events);
}

void Server::removeClient(
    int fd)
{
    // Удаляем из epoll.
    loop_.removeFd(
        fd);

    // Уничтожаем Session:
    //
    // Session
    //   -> Subscription
    //      -> unsubscribe
    //   -> Tty
    //      -> close(fd)
    sessions_.erase(fd);
}

} // namespace qmi