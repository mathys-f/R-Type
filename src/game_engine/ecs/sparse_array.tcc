#pragma once

#include "sparse_array.hpp"

/// Non-const indexed access to the underlying optional slot.
template <typename Component>
typename SparseArray<Component>::reference_type
SparseArray<Component>::operator[](size_type idx)
{
    return m_data[idx];
}

/// Const indexed access to the underlying optional slot.
template <typename Component>
typename SparseArray<Component>::const_reference_type
SparseArray<Component>::operator[](size_type idx) const
{
    return m_data[idx];
}

/// Iterator to first element.
template <typename Component>
typename SparseArray<Component>::iterator
SparseArray<Component>::begin() noexcept
{
    return m_data.begin();
}

/// Const iterator to first element.
template <typename Component>
typename SparseArray<Component>::const_iterator
SparseArray<Component>::begin() const noexcept
{
    return m_data.begin();
}

/// Const iterator to first element.
template <typename Component>
typename SparseArray<Component>::const_iterator
SparseArray<Component>::cbegin() const noexcept
{
    return m_data.cbegin();
}

/// Iterator past-the-end.
template <typename Component>
typename SparseArray<Component>::iterator
SparseArray<Component>::end() noexcept
{
    return m_data.end();
}

/// Const past-the-end iterator.
template <typename Component>
typename SparseArray<Component>::const_iterator
SparseArray<Component>::end() const noexcept
{
    return m_data.end();
}

/// Const past-the-end iterator.
template <typename Component>
typename SparseArray<Component>::const_iterator
SparseArray<Component>::cend() const noexcept
{
    return m_data.cend();
}

/// Number of slots in the underlying container.
template <typename Component>
typename SparseArray<Component>::size_type
SparseArray<Component>::size() const noexcept
{
    return m_data.size();
}

/// Insert a copy of `value` at `pos`, resizing storage if necessary.
template <typename Component>
typename SparseArray<Component>::reference_type
SparseArray<Component>::insert_at(size_type pos, Component const &value)
{
    if (pos >= m_data.size())
        m_data.resize(pos + 1);
    m_data[pos] = value;
    return m_data[pos];
}

/// Insert by moving `value` into the storage at `pos`.
template <typename Component>
typename SparseArray<Component>::reference_type
SparseArray<Component>::insert_at(size_type pos, Component &&value)
{
    if (pos >= m_data.size())
        m_data.resize(pos + 1);
    m_data[pos] = std::move(value);
    return m_data[pos];
}

/// Emplace a component in-place at `pos`.
template <typename Component>
template <class... Params>
typename SparseArray<Component>::reference_type
SparseArray<Component>::emplace_at(size_type pos, Params &&...params)
{
    if (pos >= m_data.size())
        m_data.resize(pos + 1);
    m_data[pos].emplace(std::forward<Params>(params)...);
    return m_data[pos];
}

/// Erase the stored component at `pos` (makes the optional empty).
template <typename Component>
void SparseArray<Component>::erase(size_type pos)
{
    if (pos >= m_data.size())
        return;
    m_data[pos].reset();
}

/// Find index by pointer comparison to the provided optional.
template <typename Component>
typename SparseArray<Component>::size_type
SparseArray<Component>::get_index(value_type const &value) const
{
    for (size_type i = 0; i < m_data.size(); ++i)
    {
        if (std::addressof(m_data[i]) == std::addressof(value))
            return i;
    }
    return static_cast<size_type>(-1);
}
