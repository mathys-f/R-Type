#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

template <typename Component>
class SparseArray
{
public:
    using value_type           = std::optional<Component>;
    using reference_type       = value_type &;
    using const_reference_type = value_type const &;
    using container_t          = std::vector<value_type>;
    using size_type            = typename container_t::size_type;

    using iterator       = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    SparseArray() = default;
    ~SparseArray() = default;

    SparseArray(SparseArray const &)            = default;
    SparseArray(SparseArray &&) noexcept        = default;
    SparseArray &operator=(SparseArray const &) = default;
    SparseArray &operator=(SparseArray &&) noexcept = default;

    reference_type operator[](size_type idx);
    const_reference_type operator[](size_type idx) const;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;

    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

    size_type size() const noexcept;

    reference_type insert_at(size_type pos, Component const &value);
    reference_type insert_at(size_type pos, Component &&value);

    template <class... Params>
    reference_type emplace_at(size_type pos, Params &&...params);

    void erase(size_type pos);

    size_type get_index(value_type const &value) const;

private:
    container_t _data;
};

#include "SparseArray.tcc"
