#pragma once

// Does nothing at compile time, only for IDE autocompletion purposes
#include "event_queue.h"

#include <vector>
#include <variant>

namespace engn {

namespace evts {

template<typename... TEventTypes>
template<typename TEventType>
void EventQueue<TEventTypes...>::push(TEventType &&event) {
    events.emplace_back(std::forward<TEventType>(event));
}

template<typename... TEventTypes>
template<typename TEventType>
const TEventType *EventQueue<TEventTypes...>::get_first() const {
    for (const auto &e : events) {
        if (const TEventType *ptr = std::get_if<TEventType>(&e)) {
            return ptr;
        }
    }
    return nullptr;
}

template<typename... TEventTypes>
template<typename TEventType>
const TEventType *EventQueue<TEventTypes...>::get_last() const {
    for (auto it = events.rbegin(); it != events.rend(); ++it) {
        if (const TEventType *ptr = std::get_if<TEventType>(&*it)) {
            return ptr;
        }
    }
    return nullptr;
}

template<typename... TEventTypes>
template<typename TEventType, typename Func>
void EventQueue<TEventTypes...>::for_each(Func &&fn) {
    for (const auto &e : events) {
        if (TEventType *ptr = std::get_if<TEventType>(&e)) {
            fn(*ptr);
        }
    }
}

template<typename... TEventTypes>
void EventQueue<TEventTypes...>::clear() {
    events.clear();
}

} // namespace evts

} // namespace engn
