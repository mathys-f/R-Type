#pragma once

#include "zipper.h"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ecs
{
namespace containers
{

// ========== ZipperIterator Implementation ==========

template <class... Containers>
ZipperIterator<Containers...>::ZipperIterator(IteratorTuple iters, std::size_t max, std::size_t idx)
    : m_iters(std::move(iters)), m_max(max), m_idx(idx)
{
}

template <class... Containers>
typename ZipperIterator<Containers...>::Reference ZipperIterator<Containers...>::operator*()
{
    return deref(std::index_sequence_for<Containers...>{});
}

template <class... Containers>
ZipperIterator<Containers...>& ZipperIterator<Containers...>::operator++()
{
    increment(std::index_sequence_for<Containers...>{});
    ++m_idx;
    return *this;
}

template <class... Containers>
ZipperIterator<Containers...> ZipperIterator<Containers...>::operator++(int)
{
    ZipperIterator tmp(*this);
    ++(*this);
    return tmp;
}

template <class... Containers>
bool ZipperIterator<Containers...>::operator==(ZipperIterator const& rhs) const
{
    return m_idx == rhs.m_idx;
}

template <class... Containers>
bool ZipperIterator<Containers...>::operator!=(ZipperIterator const& rhs) const
{
    return !(*this == rhs);
}

template <class... Containers>
std::size_t ZipperIterator<Containers...>::index() const noexcept
{
    return m_idx;
}

template <class... Containers>
template <std::size_t... Is>
typename ZipperIterator<Containers...>::Reference
ZipperIterator<Containers...>::deref(std::index_sequence<Is...>)
{
    return Reference(*std::get<Is>(m_iters)...);
}

template <class... Containers>
template <std::size_t... Is>
void ZipperIterator<Containers...>::increment(std::index_sequence<Is...>)
{
    (void)std::initializer_list<int>{(++std::get<Is>(m_iters), 0)...};
}

// ========== Zipper Implementation ==========

template <class... Containers>
Zipper<Containers...>::Zipper(Containers&... cs)
    : m_begin_iters(std::begin(cs)...),
      m_max_size(std::min({static_cast<std::size_t>(cs.size())...}))
{
}

template <class... Containers>
typename Zipper<Containers...>::Iterator Zipper<Containers...>::begin()
{
    return Iterator(m_begin_iters, m_max_size, 0);
}

template <class... Containers>
typename Zipper<Containers...>::Iterator Zipper<Containers...>::end()
{
    return Iterator(m_begin_iters, m_max_size, m_max_size);
}

template <class... Containers>
typename Zipper<Containers...>::ConstIterator Zipper<Containers...>::begin() const
{
    return const_cast<Zipper*>(this)->begin();
}

template <class... Containers>
typename Zipper<Containers...>::ConstIterator Zipper<Containers...>::end() const
{
    return const_cast<Zipper*>(this)->end();
}

template <class... Containers>
Zipper<Containers...> make_zipper(Containers&... cs)
{
    return Zipper<Containers...>(cs...);
}

// ========== IndexedZipper::iterator Implementation ==========

template <class... Containers>
IndexedZipper<Containers...>::iterator::iterator(BaseIteratorT it) : m_it(it)
{
}

template <class... Containers>
typename IndexedZipper<Containers...>::iterator::Reference
IndexedZipper<Containers...>::iterator::operator*()
{
    return expand(std::index_sequence_for<Containers...>{});
}

template <class... Containers>
typename IndexedZipper<Containers...>::iterator&
IndexedZipper<Containers...>::iterator::operator++()
{
    ++m_it;
    return *this;
}

template <class... Containers>
typename IndexedZipper<Containers...>::iterator
IndexedZipper<Containers...>::iterator::operator++(int)
{
    iterator tmp(*this);
    ++(*this);
    return tmp;
}

template <class... Containers>
bool IndexedZipper<Containers...>::iterator::operator==(iterator const& rhs) const
{
    return m_it == rhs.m_it;
}

template <class... Containers>
bool IndexedZipper<Containers...>::iterator::operator!=(iterator const& rhs) const
{
    return !(*this == rhs);
}

template <class... Containers>
template <std::size_t... Is>
typename IndexedZipper<Containers...>::iterator::Reference
IndexedZipper<Containers...>::iterator::expand(std::index_sequence<Is...>)
{
    auto t = *m_it;
    return Reference(m_it.index(), std::get<Is>(t)...);
}

// ========== IndexedZipper Implementation ==========

template <class... Containers>
IndexedZipper<Containers...>::IndexedZipper(Containers&... cs) : m_zip(cs...)
{
}

template <class... Containers>
typename IndexedZipper<Containers...>::iterator IndexedZipper<Containers...>::begin()
{
    return iterator(m_zip.begin());
}

template <class... Containers>
typename IndexedZipper<Containers...>::iterator IndexedZipper<Containers...>::end()
{
    return iterator(m_zip.end());
}

template <class... Containers>
IndexedZipper<Containers...> make_indexed_zipper(Containers&... cs)
{
    return IndexedZipper<Containers...>(cs...);
}

}  // namespace containers

template <class... Containers>
auto zipper(Containers&... cs)
{
    return containers::make_zipper(cs...);
}

template <class... Containers>
auto indexed_zipper(Containers&... cs)
{
    return containers::make_indexed_zipper(cs...);
}

}  // namespace ecs
