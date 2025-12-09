#pragma once

// Does nothing at compile time, only for IDE autocompletion purposes
#include "event_queue.h"

#include <vector>
#include <variant>

namespace engn {

namespace evts {

template<typename TVariant>
template<typename TEventType>
void EventQueue<TVariant>::push(TEventType &&event) {
    m_data.emplace_back(std::forward<TEventType>(event));
}

template<typename TVariant>
template<typename TEventType>
const TEventType *EventQueue<TVariant>::get_first() const {
    for (const auto &e : m_data) {
        if (const TEventType *ptr = std::get_if<TEventType>(&e)) {
            return ptr;
        }
    }
    return nullptr;
}

template<typename TVariant>
template<typename TEventType>
const TEventType *EventQueue<TVariant>::get_last() const {
    for (auto it = m_data.rbegin(); it != m_data.rend(); ++it) {
        if (const TEventType *ptr = std::get_if<TEventType>(&*it)) {
            return ptr;
        }
    }
    return nullptr;
}

template<typename TVariant>
template<typename TEventType, typename Func>
void EventQueue<TVariant>::for_each(Func &&fn) {
    for (auto &e : m_data) {
        if (TEventType *ptr = std::get_if<TEventType>(&e)) {
            fn(*ptr);
        }
    }
}

template<typename TVariant>
template<typename TEventType, typename Func>
void EventQueue<TVariant>::for_each(Func &&fn) const{
    for (const auto &e : m_data) {
        if (const TEventType *ptr = std::get_if<TEventType>(&e)) {
            fn(*ptr);
        }
    }
}

template<typename TVariant>
void EventQueue<TVariant>::clear() {
    m_data.clear();
}

template<typename TVariant>
EventQueue<TVariant>::SizeType EventQueue<TVariant>::size() {
    return m_data.size();
}

// Iterators

template<typename TVariant>
EventQueue<TVariant>::Iterator EventQueue<TVariant>::begin() noexcept {
    return m_data.begin();
}

template<typename TVariant>
EventQueue<TVariant>::ConstIterator EventQueue<TVariant>::begin() const noexcept {
    return m_data.cbegin();
}

template<typename TVariant>
EventQueue<TVariant>::ConstIterator EventQueue<TVariant>::cbegin() const noexcept {
    return m_data.cbegin();
}

template<typename TVariant>
EventQueue<TVariant>::Iterator EventQueue<TVariant>::end() noexcept {
    return m_data.end();
}

template<typename TVariant>
EventQueue<TVariant>::ConstIterator EventQueue<TVariant>::end() const noexcept {
    return m_data.cend();
}

template<typename TVariant>
EventQueue<TVariant>::ConstIterator EventQueue<TVariant>::cend() const noexcept {
    return m_data.cend();
}

} // namespace evts

} // namespace engn
