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

    /**
     * Push an element at the end of the queue
     */
    template<typename TEventType>
    void push(TEventType &&event);

    /**
     * Returns nullptr if not found.
     */
    template<typename TEventType>
    const TEventType *get_first() const;

    /**
     * Returns nullptr if not found.
     */
    template<typename TEventType>
    const TEventType *get_last() const;

    template<typename TEventType, typename Func>
    void for_each(Func &&fn);

    void clear();

};

} // namespace evts

} // namespace engn

#include "events/event_queue.tcc"
