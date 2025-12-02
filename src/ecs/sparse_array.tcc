#pragma once

#include "sparse_array.h"

namespace ecs
{

/// Non-const indexed access to the underlying optional slot.
template <typename Component>
typename SparseArray<Component>::ReferenceType SparseArray<Component>::operator[](SizeType idx)
{
    return m_data[idx];
}

/// Const indexed access to the underlying optional slot.
template <typename Component>
typename SparseArray<Component>::ConstReferenceType
SparseArray<Component>::operator[](SizeType idx) const
{
    return m_data[idx];
}

/// Iterator to first element.
template <typename Component>
typename SparseArray<Component>::Iterator SparseArray<Component>::begin() noexcept
{
    return m_data.begin();
}

/// Const iterator to first element.
template <typename Component>
typename SparseArray<Component>::ConstIterator SparseArray<Component>::begin() const noexcept
{
    return m_data.begin();
}

/// Const iterator to first element.
template <typename Component>
typename SparseArray<Component>::ConstIterator SparseArray<Component>::cbegin() const noexcept
{
    return m_data.cbegin();
}

/// Iterator past-the-end.
template <typename Component>
typename SparseArray<Component>::Iterator SparseArray<Component>::end() noexcept
{
    return m_data.end();
}

/// Const past-the-end iterator.
template <typename Component>
typename SparseArray<Component>::ConstIterator SparseArray<Component>::end() const noexcept
{
    return m_data.end();
}

/// Const past-the-end iterator.
template <typename Component>
typename SparseArray<Component>::ConstIterator SparseArray<Component>::cend() const noexcept
{
    return m_data.cend();
}

/// Number of slots in the underlying container.
template <typename Component>
typename SparseArray<Component>::SizeType SparseArray<Component>::size() const noexcept
{
    return m_data.size();
}

/// Insert a copy of `value` at `pos`, resizing storage if necessary.
template <typename Component>
typename SparseArray<Component>::ReferenceType
SparseArray<Component>::insert_at(SizeType pos, Component const& value)
{
    if (pos >= m_data.size())
        m_data.resize(pos + 1);
    m_data[pos] = value;
    return m_data[pos];
}

/// Insert by moving `value` into the storage at `pos`.
template <typename Component>
typename SparseArray<Component>::ReferenceType SparseArray<Component>::insert_at(SizeType pos,
                                                                                 Component&& value)
{
    if (pos >= m_data.size())
        m_data.resize(pos + 1);
    m_data[pos] = std::move(value);
    return m_data[pos];
}

/// Emplace a component in-place at `pos`.
template <typename Component>
template <class... Params>
typename SparseArray<Component>::ReferenceType
SparseArray<Component>::emplace_at(SizeType pos, Params&&... params)
{
    if (pos >= m_data.size())
        m_data.resize(pos + 1);
    m_data[pos].emplace(std::forward<Params>(params)...);
    return m_data[pos];
}

/// Erase the stored component at `pos` (makes the optional empty).
template <typename Component> void SparseArray<Component>::erase(SizeType pos)
{
    if (pos >= m_data.size())
        return;
    m_data[pos].reset();
}

/// Find index by pointer comparison to the provided optional.
template <typename Component>
typename SparseArray<Component>::SizeType
SparseArray<Component>::get_index(ValueType const& value) const
{
    for (SizeType i = 0; i < m_data.size(); ++i)
    {
        if (std::addressof(m_data[i]) == std::addressof(value))
            return i;
    }
    return static_cast<SizeType>(-1);
}

}  // namespace ecs
