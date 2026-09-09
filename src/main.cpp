// main() собирает всю композицию.
#include <iostream>

#include <sys/epoll.h>

#include "at/server.hpp"
#include "modem/event_bus.hpp"
#include "modem/modem.hpp"

#include "qmi/server.hpp"

#include "transport/event_loop.hpp"
#include "transport/timer_fd.hpp"

int main()
{
    try
    {
        // Единый event loop.
        transport::EventLoop loop;

        // Общая шина событий.
        modem::EventBus eventBus;

        // Таймер для имитации
        // поиска сети.
        transport::TimerFd networkTimer;

        // Domain объект.
        modem::Modem modem(
            eventBus,

            // Modem просит:
            // "Кто-то запусти таймер".
            //
            // Инфраструктура решает,
            // как это сделать (timerfd, mock).
            [&networkTimer]()
            {
                // Армим 2-секундный timerfd
                // -> EPOLLIN.
                networkTimer.arm(2);
            });

        // AT server: PTY + AT sessions.
        at::Server atServer(
            modem,
            eventBus,
            loop);

        // QMI server: Unix socket + QmiSession.
        qmi::Server qmiServer(
            "/tmp/mini-modem-qmi.sock",
            modem,
            eventBus,
            loop);

        // Создаём PTY устройство.
        const std::string atDevice =
            atServer.createPty();

        std::cout
            << "AT device: "
            << atDevice
            << '\n';

        std::cout
            << "QMI socket: "
            << "/tmp/mini-modem-qmi.sock"
            << '\n';

        // Timerfd в event loop.
        loop.addFd(
            networkTimer.fd(),
            EPOLLIN,
            [&networkTimer, &modem](
                std::uint32_t events)
            {
                if (!(events &
                    EPOLLIN))
                {
                    return;
                }

                // Читаем timerfd, чтобы
                // сбросить событие.
                networkTimer.consume();

                // Пытаемся завершить
                // поиск сети.
                if (modem
                        .networkSearchComplete())
                {
                    std::cout
                        << "Network registered\n";
                }
            });

        // Запускаем главный цикл.
        //
        // Блокирует до SIGINT/SIGTERM.
        loop.run();
    }
    catch (
        const std::exception& e)
    {
        std::cerr
            << "Fatal error: "
            << e.what()
            << '\n';

        return 1;
    }

    return 0;
}