#pragma once
#include <variant>
#include <vector>

namespace engn {

namespace evts {

template <typename TVariant> class EventQueue {
  public:
    using ContainerT = std::vector<TVariant>;
    using SizeType = typename ContainerT::size_type;

    using Iterator = typename ContainerT::iterator;
    using ConstIterator = typename ContainerT::const_iterator;

    EventQueue() = default;
    ~EventQueue() = default;

    EventQueue(EventQueue const&) = default;
    EventQueue(EventQueue&&) noexcept = default;
    EventQueue& operator=(EventQueue const&) = default;
    EventQueue& operator=(EventQueue&&) noexcept = default;

    /**
     * Push an element at the end of the queue
     */
    template <typename TEventType> void push(TEventType&& event);

    /**
     * Returns nullptr if not found.
     */
    template <typename TEventType> const TEventType* get_first() const;

    /**
     * Returns nullptr if not found.
     */
    template <typename TEventType> const TEventType* get_last() const;

    /**
     * Each functions must follow this template:
     * `void fn(const TEventType &evt)`
     */
    template <typename TEventType, typename TFunc> void for_each(TFunc&& fn);
    template <typename TEventType, typename TFunc> void for_each(TFunc&& fn) const;

    void clear();

    SizeType size();

    // Iterators
    Iterator begin() noexcept;
    ConstIterator begin() const noexcept;
    ConstIterator cbegin() const noexcept;

    Iterator end() noexcept;
    ConstIterator end() const noexcept;
    ConstIterator cend() const noexcept;

  private:
    ContainerT m_data;
};

} // namespace evts

} // namespace engn

#include "event_queue.tcc"
