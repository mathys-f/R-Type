#pragma once

#include "sparse_array.h"

template <typename Component>
typename SparseArray<Component>::reference_type
SparseArray<Component>::operator[](size_type idx)
{
    return _data[idx];
}

template <typename Component>
typename SparseArray<Component>::const_reference_type
SparseArray<Component>::operator[](size_type idx) const
{
    return _data[idx];
}

template <typename Component>
typename SparseArray<Component>::iterator
SparseArray<Component>::begin() noexcept
{
    return _data.begin();
}

template <typename Component>
typename SparseArray<Component>::const_iterator
SparseArray<Component>::begin() const noexcept
{
    return _data.begin();
}

template <typename Component>
typename SparseArray<Component>::const_iterator
SparseArray<Component>::cbegin() const noexcept
{
    return _data.cbegin();
}

template <typename Component>
typename SparseArray<Component>::iterator
SparseArray<Component>::end() noexcept
{
    return _data.end();
}

template <typename Component>
typename SparseArray<Component>::const_iterator
SparseArray<Component>::end() const noexcept
{
    return _data.end();
}

template <typename Component>
typename SparseArray<Component>::const_iterator
SparseArray<Component>::cend() const noexcept
{
    return _data.cend();
}

template <typename Component>
typename SparseArray<Component>::size_type
SparseArray<Component>::size() const noexcept
{
    return _data.size();
}

template <typename Component>
typename SparseArray<Component>::reference_type
SparseArray<Component>::insert_at(size_type pos, Component const &value)
{
    if (pos >= _data.size())
        _data.resize(pos + 1);
    _data[pos] = value;
    return _data[pos];
}

template <typename Component>
typename SparseArray<Component>::reference_type
SparseArray<Component>::insert_at(size_type pos, Component &&value)
{
    if (pos >= _data.size())
        _data.resize(pos + 1);
    _data[pos] = std::move(value);
    return _data[pos];
}

template <typename Component>
template <class... Params>
typename SparseArray<Component>::reference_type
SparseArray<Component>::emplace_at(size_type pos, Params &&...params)
{
    if (pos >= _data.size())
        _data.resize(pos + 1);
    _data[pos].emplace(std::forward<Params>(params)...);
    return _data[pos];
}

template <typename Component>
void SparseArray<Component>::erase(size_type pos)
{
    if (pos >= _data.size())
        return;
    _data[pos].reset();
}

template <typename Component>
typename SparseArray<Component>::size_type
SparseArray<Component>::get_index(value_type const &value) const
{
    for (size_type i = 0; i < _data.size(); ++i)
    {
        if (std::addressof(_data[i]) == std::addressof(value))
            return i;
    }
    return static_cast<size_type>(-1);
}
