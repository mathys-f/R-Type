#pragma once

#include <cstddef>
#include <tuple>
#include <iterator>
#include <type_traits>
#include <utility>
#include <algorithm>

namespace ecs {

namespace containers {

/// Iterator that zips multiple containers together producing tuples of references.
template <class... Containers>
class ZipperIterator {
public:
    using ValueType = std::tuple<typename std::add_lvalue_reference<decltype(*std::begin(std::declval<Containers&>()))>::type...>;
    using Reference = ValueType;
    using Pointer = void;
    using DifferenceType = std::ptrdiff_t;
    using IteratorCategory = std::input_iterator_tag;

    using IteratorTuple = std::tuple<decltype(std::begin(std::declval<Containers&>()))...>;

    ZipperIterator() = default;

    ZipperIterator(IteratorTuple iters, std::size_t max, std::size_t idx);

    Reference operator*();
    ZipperIterator& operator++();
    ZipperIterator operator++(int);

    bool operator==(ZipperIterator const& rhs) const;
    bool operator!=(ZipperIterator const& rhs) const;

    std::size_t index() const noexcept;

private:
    IteratorTuple m_iters{};
    std::size_t m_max{0};
    std::size_t m_idx{0};

    template <std::size_t... Is> Reference deref(std::index_sequence<Is...>);
    template <std::size_t... Is> void increment(std::index_sequence<Is...>);
};

/// Zips multiple containers together producing tuples of references.
template <class... Containers>
class Zipper {
public:
    using Iterator = ZipperIterator<Containers...>;
    using ConstIterator = Iterator;

    explicit Zipper(Containers&... cs);

    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;

private:
    std::tuple<decltype(std::begin(std::declval<Containers&>()))...> m_begin_iters;
    std::size_t m_max_size{0};
};

/// Helper to create a Zipper instance from containers.
template <class... Containers>
Zipper<Containers...> make_zipper(Containers&... cs);

/// Like Zipper but provides element index together with tuple of references.
template <class... Containers>
class IndexedZipper {
public:
    class iterator {
        using BaseZipperT = Zipper<Containers...>;
        using BaseIteratorT = typename BaseZipperT::Iterator;

    public:
        using ValueType = std::tuple<std::size_t, typename std::add_lvalue_reference<decltype(*std::begin(std::declval<Containers&>()))>::type...>;
        using Reference = ValueType;
        using Pointer = void;
        using DifferenceType = std::ptrdiff_t;
        using IteratorCategory = std::input_iterator_tag;

        iterator() = default;
        explicit iterator(BaseIteratorT it);

        Reference operator*();
        iterator& operator++();
        iterator operator++(int);

        bool operator==(iterator const& rhs) const;
        bool operator!=(iterator const& rhs) const;

    private:
        BaseIteratorT m_it{};
        template <std::size_t... Is> Reference expand(std::index_sequence<Is...>);
    };

    explicit IndexedZipper(Containers&... cs);

    iterator begin();
    iterator end();

private:
    Zipper<Containers...> m_zip;
};

/// Helper to create an IndexedZipper instance.
template <class... Containers>
IndexedZipper<Containers...> make_indexed_zipper(Containers&... cs);

}  // namespace containers

/// Convenience free functions
template <class... Containers>
auto zipper(Containers&... cs);

template <class... Containers>
auto indexed_zipper(Containers&... cs);

}  // namespace ecs

#include "zipper.tcc"
