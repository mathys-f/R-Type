#pragma once

#include "zipper.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ecs {
namespace containers {

// ========== ZipperIterator Implementation ==========

template <class... TContainers>
ZipperIterator<TContainers...>::ZipperIterator(IteratorTuple iters, std::size_t max, std::size_t idx)
    : m_iters(std::move(iters)), m_max(max), m_idx(idx) {}

template <class... TContainers>
typename ZipperIterator<TContainers...>::Reference ZipperIterator<TContainers...>::operator*() {
    return deref(std::index_sequence_for<TContainers...>{});
}

template <class... TContainers> ZipperIterator<TContainers...>& ZipperIterator<TContainers...>::operator++() {
    increment(std::index_sequence_for<TContainers...>{});
    ++m_idx;
    return *this;
}

template <class... TContainers> ZipperIterator<TContainers...> ZipperIterator<TContainers...>::operator++(int) {
    ZipperIterator tmp(*this);
    ++(*this);
    return tmp;
}

template <class... TContainers> bool ZipperIterator<TContainers...>::operator==(ZipperIterator const& rhs) const {
    return m_idx == rhs.m_idx;
}

template <class... TContainers> bool ZipperIterator<TContainers...>::operator!=(ZipperIterator const& rhs) const {
    return !(*this == rhs);
}

template <class... TContainers> std::size_t ZipperIterator<TContainers...>::index() const noexcept {
    return m_idx;
}

template <class... TContainers>
template <std::size_t... TIs>
typename ZipperIterator<TContainers...>::Reference ZipperIterator<TContainers...>::deref(std::index_sequence<TIs...>) {
    return Reference(*std::get<TIs>(m_iters)...);
}

template <class... TContainers>
template <std::size_t... TIs>
void ZipperIterator<TContainers...>::increment(std::index_sequence<TIs...>) {
    (void)std::initializer_list<int>{(++std::get<TIs>(m_iters), 0)...};
}

// ========== Zipper Implementation ==========

template <class... TContainers>
Zipper<TContainers...>::Zipper(TContainers&... cs)
    : m_begin_iters(std::begin(cs)...), m_max_size(std::min({static_cast<std::size_t>(cs.size())...})) {}

template <class... TContainers> typename Zipper<TContainers...>::Iterator Zipper<TContainers...>::begin() {
    return Iterator(m_begin_iters, m_max_size, 0);
}

template <class... TContainers> typename Zipper<TContainers...>::Iterator Zipper<TContainers...>::end() {
    return Iterator(m_begin_iters, m_max_size, m_max_size);
}

template <class... TContainers> typename Zipper<TContainers...>::ConstIterator Zipper<TContainers...>::begin() const {
    return const_cast<Zipper*>(this)->begin(); // NOLINT(cppcoreguidelines-pro-type-const-cast)
}

template <class... TContainers> typename Zipper<TContainers...>::ConstIterator Zipper<TContainers...>::end() const {
    return const_cast<Zipper*>(this)->end(); // NOLINT(cppcoreguidelines-pro-type-const-cast)
}

template <class... TContainers> Zipper<TContainers...> make_zipper(TContainers&... cs) {
    return Zipper<TContainers...>(cs...);
}

// ========== IndexedZipper::Iterator Implementation ==========

template <class... TContainers> IndexedZipper<TContainers...>::Iterator::Iterator(BaseIteratorT it) : m_it(it) {}

template <class... TContainers>
typename IndexedZipper<TContainers...>::Iterator::Reference IndexedZipper<TContainers...>::Iterator::operator*() {
    return expand(std::index_sequence_for<TContainers...>{});
}

template <class... TContainers>
typename IndexedZipper<TContainers...>::Iterator& IndexedZipper<TContainers...>::Iterator::operator++() {
    ++m_it;
    return *this;
}

template <class... TContainers>
typename IndexedZipper<TContainers...>::Iterator IndexedZipper<TContainers...>::Iterator::operator++(int) {
    Iterator tmp(*this);
    ++(*this);
    return tmp;
}

template <class... TContainers> bool IndexedZipper<TContainers...>::Iterator::operator==(Iterator const& rhs) const {
    return m_it == rhs.m_it;
}

template <class... TContainers> bool IndexedZipper<TContainers...>::Iterator::operator!=(Iterator const& rhs) const {
    return !(*this == rhs);
}

template <class... TContainers>
template <std::size_t... TIs>
typename IndexedZipper<TContainers...>::Iterator::Reference
IndexedZipper<TContainers...>::Iterator::expand(std::index_sequence<TIs...>) {
    auto t = *m_it;
    return Reference(m_it.index(), std::get<TIs>(t)...);
}

// ========== IndexedZipper Implementation ==========

template <class... TContainers> IndexedZipper<TContainers...>::IndexedZipper(TContainers&... cs) : m_zip(cs...) {}

template <class... TContainers>
typename IndexedZipper<TContainers...>::Iterator IndexedZipper<TContainers...>::begin() {
    return Iterator(m_zip.begin());
}

template <class... TContainers> typename IndexedZipper<TContainers...>::Iterator IndexedZipper<TContainers...>::end() {
    return Iterator(m_zip.end());
}

template <class... TContainers> IndexedZipper<TContainers...> make_indexed_zipper(TContainers&... cs) {
    return IndexedZipper<TContainers...>(cs...);
}

} // namespace containers

template <class... TContainers> auto zipper(TContainers&... cs) {
    return containers::make_zipper(cs...);
}

template <class... TContainers> auto indexed_zipper(TContainers&... cs) {
    return containers::make_indexed_zipper(cs...);
}

} // namespace ecs
