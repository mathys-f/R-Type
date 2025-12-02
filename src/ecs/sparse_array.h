#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

/// SparseArray is a compact optional-storage container for components indexed by
/// entity ids. It provides random access insertion/erasure while keeping the
/// storage contiguous as a vector of optional values.
/// @tparam Component Type stored inside the sparse array.
template <typename Component> class SparseArray
{
  public:
    using ValueType = std::optional<Component>;
    using ReferenceType = ValueType&;
    using ConstReferenceType = ValueType const&;
    using ContainerT = std::vector<ValueType>;
    using SizeType = typename ContainerT::size_type;

    using Iterator = typename ContainerT::iterator;
    using ConstIterator = typename ContainerT::const_iterator;

    SparseArray() = default;
    ~SparseArray() = default;

    SparseArray(SparseArray const&) = default;
    SparseArray(SparseArray&&) noexcept = default;
    SparseArray& operator=(SparseArray const&) = default;
    SparseArray& operator=(SparseArray&&) noexcept = default;

    /// Access element by index (non-const). If the element is not present the
    /// returned optional will be empty.
    /// @param idx Position to access.
    /// @return Reference to the optional-wrapped component.
    ReferenceType operator[](SizeType idx);

    /// Access element by index (const).
    /// @param idx Position to access.
    /// @return Const reference to the optional-wrapped component.
    ConstReferenceType operator[](SizeType idx) const;

    /// Iterator to first element.
    Iterator begin() noexcept;
    /// Const iterator to first element.
    ConstIterator begin() const noexcept;
    ConstIterator cbegin() const noexcept;

    /// Iterator past-the-end.
    Iterator end() noexcept;
    ConstIterator end() const noexcept;
    ConstIterator cend() const noexcept;

    /// Number of slots in the underlying storage (including empty optionals).
    /// @return The slot count.
    SizeType size() const noexcept;

    /// Insert a copy of `value` at `pos`, resizing storage if necessary.
    /// @param pos Index where to insert.
    /// @param value Component instance to copy.
    /// @return Reference to the storage slot now containing the component.
    ReferenceType insert_at(SizeType pos, Component const& value);

    /// Insert by moving `value` into the storage at `pos`.
    /// @param pos Index where to insert.
    /// @param value Component instance to move.
    /// @return Reference to the storage slot now containing the component.
    ReferenceType insert_at(SizeType pos, Component&& value);

    /// Construct a component in-place at `pos` forwarding constructor arguments.
    /// @tparam Params Constructor parameter pack.
    /// @param pos Index where to emplace.
    /// @param params Forwarded to component constructor.
    /// @return Reference to the emplaced storage slot.
    template <class... Params> ReferenceType emplace_at(SizeType pos, Params&&... params);

    /// Erase the component at `pos` (makes the optional empty).
    /// @param pos Index of the slot to clear.
    void erase(SizeType pos);

    /// Find the index of a given optional value pointer within the storage.
    /// @param value Optional reference to compare by address.
    /// @return Index of matching slot, or `static_cast<SizeType>(-1)` if not found.
    SizeType get_index(ValueType const& value) const;

  private:
    ContainerT m_data;
};

#include "sparse_array.tcc"
