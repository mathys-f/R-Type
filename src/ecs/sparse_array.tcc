#pragma once

#include "sparse_array.h"

namespace ecs {

/// Non-const indexed access to the underlying optional slot.
template <typename TComponent>
typename SparseArray<TComponent>::ReferenceType SparseArray<TComponent>::operator[](SizeType idx) {
    return m_data[idx];
}

/// Const indexed access to the underlying optional slot.
template <typename TComponent>
typename SparseArray<TComponent>::ConstReferenceType SparseArray<TComponent>::operator[](SizeType idx) const {
    return m_data[idx];
}

/// Iterator to first element.
template <typename TComponent> typename SparseArray<TComponent>::Iterator SparseArray<TComponent>::begin() noexcept {
    return m_data.begin();
}

/// Const iterator to first element.
template <typename TComponent>
typename SparseArray<TComponent>::ConstIterator SparseArray<TComponent>::begin() const noexcept {
    return m_data.begin();
}

/// Const iterator to first element.
template <typename TComponent>
typename SparseArray<TComponent>::ConstIterator SparseArray<TComponent>::cbegin() const noexcept {
    return m_data.cbegin();
}

/// Iterator past-the-end.
template <typename TComponent> typename SparseArray<TComponent>::Iterator SparseArray<TComponent>::end() noexcept {
    return m_data.end();
}

/// Const past-the-end iterator.
template <typename TComponent>
typename SparseArray<TComponent>::ConstIterator SparseArray<TComponent>::end() const noexcept {
    return m_data.end();
}

/// Const past-the-end iterator.
template <typename TComponent>
typename SparseArray<TComponent>::ConstIterator SparseArray<TComponent>::cend() const noexcept {
    return m_data.cend();
}

/// Number of slots in the underlying container.
template <typename TComponent>
typename SparseArray<TComponent>::SizeType SparseArray<TComponent>::size() const noexcept {
    return m_data.size();
}

/// Insert a copy of `value` at `pos`, resizing storage if necessary.
template <typename TComponent>
typename SparseArray<TComponent>::ReferenceType SparseArray<TComponent>::insert_at(SizeType pos,
                                                                                   TComponent const& value) {
    if (pos >= m_data.size())
        m_data.resize(pos + 1);
    m_data[pos] = value;
    return m_data[pos];
}

/// Insert by moving `value` into the storage at `pos`.
template <typename TComponent>
typename SparseArray<TComponent>::ReferenceType SparseArray<TComponent>::insert_at(SizeType pos, TComponent&& value) {
    if (pos >= m_data.size())
        m_data.resize(pos + 1);
    m_data[pos] = std::move(value);
    return m_data[pos];
}

/// Emplace a component in-place at `pos`.
template <typename TComponent>
template <class... TParams>
typename SparseArray<TComponent>::ReferenceType SparseArray<TComponent>::emplace_at(SizeType pos, TParams&&... params) {
    if (pos >= m_data.size())
        m_data.resize(pos + 1);
    m_data[pos].emplace(std::forward<TParams>(params)...);
    return m_data[pos];
}

/// Erase the stored component at `pos` (makes the optional empty).
template <typename TComponent> void SparseArray<TComponent>::erase(SizeType pos) {
    if (pos >= m_data.size())
        return;
    m_data[pos].reset();
}

/// Find index by pointer comparison to the provided optional.
template <typename TComponent>
typename SparseArray<TComponent>::SizeType SparseArray<TComponent>::get_index(ValueType const& value) const {
    for (SizeType i = 0; i < m_data.size(); ++i) {
        if (std::addressof(m_data[i]) == std::addressof(value))
            return i;
    }
    return static_cast<SizeType>(-1);
}

} // namespace ecs
