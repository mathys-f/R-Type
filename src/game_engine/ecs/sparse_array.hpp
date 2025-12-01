#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

/// SparseArray is a compact optional-storage container for components indexed by
/// entity ids. It provides random access insertion/erasure while keeping the
/// storage contiguous as a vector of optional values.
/// @tparam Component Type stored inside the sparse array.
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

    /// Access element by index (non-const). If the element is not present the
    /// returned optional will be empty.
    /// @param idx Position to access.
    /// @return Reference to the optional-wrapped component.
    reference_type operator[](size_type idx);

    /// Access element by index (const).
    /// @param idx Position to access.
    /// @return Const reference to the optional-wrapped component.
    const_reference_type operator[](size_type idx) const;

    /// Iterator to first element.
    iterator begin() noexcept;
    /// Const iterator to first element.
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;

    /// Iterator past-the-end.
    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

    /// Number of slots in the underlying storage (including empty optionals).
    /// @return The slot count.
    size_type size() const noexcept;

    /// Insert a copy of `value` at `pos`, resizing storage if necessary.
    /// @param pos Index where to insert.
    /// @param value Component instance to copy.
    /// @return Reference to the storage slot now containing the component.
    reference_type insert_at(size_type pos, Component const &value);

    /// Insert by moving `value` into the storage at `pos`.
    /// @param pos Index where to insert.
    /// @param value Component instance to move.
    /// @return Reference to the storage slot now containing the component.
    reference_type insert_at(size_type pos, Component &&value);

    /// Construct a component in-place at `pos` forwarding constructor arguments.
    /// @tparam Params Constructor parameter pack.
    /// @param pos Index where to emplace.
    /// @param params Forwarded to component constructor.
    /// @return Reference to the emplaced storage slot.
    template <class... Params>
    reference_type emplace_at(size_type pos, Params &&...params);

    /// Erase the component at `pos` (makes the optional empty).
    /// @param pos Index of the slot to clear.
    void erase(size_type pos);

    /// Find the index of a given optional value pointer within the storage.
    /// @param value Optional reference to compare by address.
    /// @return Index of matching slot, or `static_cast<size_type>(-1)` if not found.
    size_type get_index(value_type const &value) const;

 private:
    container_t m_data;
};

#include "sparse_array.tcc"
