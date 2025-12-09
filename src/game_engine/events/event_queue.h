#pragma once
#include <vector>
#include <variant>

namespace engn {

namespace evts {

template<typename... TEventTypes>
class EventQueue {
public:
    using Event = std::variant<TEventTypes...>;

    std::vector<Event> events;

    template<typename TEventType>
    void push(TEventType &&event);

    template<typename TEventType>
    const TEventType *get_first() const;

    template<typename TEventType>
    const TEventType *get_last() const;

    template<typename TEventType, typename Func>
    void for_each(Func &&fn);

    void clear();

};

} // namespace evts

} // namespace engn

#include "events/event_queue.tcc"
