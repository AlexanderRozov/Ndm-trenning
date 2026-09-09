#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "modem/modem_event.hpp"

namespace modem
{

class EventBus
{
public:
    using SubscriptionId = std::uint64_t;

    using Handler =
        std::function<void(const ModemEvent&)>;

    // RAII-подписка.
    //
    // Когда Subscription уничтожается, callback
    // автоматически отписывается от EventBus.
    //
    // Это гарантирует, что callback не будет вызван
    // после уничтожения объекта-владельца (Session).
    //
    // Жизненный цикл:
    //
    // EventBus
    //    |
    //    +--> callback -> живёт пока живёт Session
    //
    class Subscription
    {
    public:
        Subscription() = default;

        ~Subscription();

        Subscription(
            const Subscription&) = delete;

        Subscription& operator=(
            const Subscription&) = delete;

        Subscription(
            Subscription&& other) noexcept;

        Subscription& operator=(
            Subscription&& other) noexcept;

        void reset() noexcept;
    private:
        friend class EventBus;

        Subscription(
            EventBus* bus,
            SubscriptionId id);

        // EventBus владеет Subscription.
        EventBus* bus_ = nullptr;

        SubscriptionId id_ = 0;
    };

    Subscription subscribe(
        Handler handler);

    void publish(
        const ModemEvent& event);
private:
    void unsubscribe(
        SubscriptionId id) noexcept;

    SubscriptionId nextId_ = 1;
    std::unordered_map<
        SubscriptionId,
        Handler> handlers_;
};

} // namespace modem