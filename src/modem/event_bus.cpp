#include "modem/event_bus.hpp"

namespace modem
{

EventBus::Subscription::Subscription(
    EventBus* bus,
    SubscriptionId id)
    : bus_(bus),
      id_(id)
{
}

EventBus::Subscription::~Subscription()
{
    reset();
}

EventBus::Subscription::Subscription(
    Subscription&& other) noexcept
    : bus_(other.bus_),
      id_(other.id_)
{
    // Передаём владение Subscription.
    other.bus_ = nullptr;
    other.id_ = 0;
}

EventBus::Subscription&
EventBus::Subscription::operator=(
    Subscription&& other) noexcept
{
    if (this == &other)
        return *this;

    // Освобождаем старую подписку.
    reset();

    // Перебираем владение.
    bus_ = other.bus_;
    id_ = other.id_;

    other.bus_ = nullptr;
    other.id_ = 0;

    return *this;
}

void EventBus::Subscription::reset() noexcept
{
    if (bus_ != nullptr && id_ != 0)
    {
        bus_->unsubscribe(id_);
    }
    bus_ = nullptr;
    id_ = 0;
}

EventBus::Subscription
EventBus::subscribe(Handler handler)
{
    const SubscriptionId id = nextId_++;

    handlers_.emplace(
        id,
        std::move(handler));

    return Subscription(
        this,
        id);
}

void EventBus::unsubscribe(
    SubscriptionId id) noexcept
{
    handlers_.erase(id);
}

void EventBus::publish(
    const ModemEvent& event)
{
    // Собираем ID перед вызовом callback:
    //
    // callback может отписаться (изменить handlers_).
    //
    // Итератор unordered_map инвалидируется
    // при изменении контейнера.
    //
    // Поэтому сначала копируем ID.
    std::vector<SubscriptionId> ids;

    ids.reserve(
        handlers_.size());

    for (const auto& [id, handler] :
         handlers_)
    {
        static_cast<void>(handler);

        ids.push_back(id);
    }

    for (const SubscriptionId id :
         ids)
    {
        const auto it =
            handlers_.find(id);

        // Callback мог удалить себя
        // во время предыдущего вызова.
        if (it != handlers_.end())
        {
            it->second(event);
        }
    }
}

} // namespace modem