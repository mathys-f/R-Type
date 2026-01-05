#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ecs {

namespace containers {

/// Iterator that zips multiple containers together producing tuples of references.
template <class... TContainers> class ZipperIterator {
  public:
    using ValueType =
        std::tuple<typename std::add_lvalue_reference<decltype(*std::begin(std::declval<TContainers&>()))>::type...>;
    using Reference = ValueType;
    using Pointer = void;
    using DifferenceType = std::ptrdiff_t;
    using IteratorCategory = std::input_iterator_tag;

    using IteratorTuple = std::tuple<decltype(std::begin(std::declval<TContainers&>()))...>;

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

    template <std::size_t... TIs> Reference deref(std::index_sequence<TIs...>);
    template <std::size_t... TIs> void increment(std::index_sequence<TIs...>);
};

/// Zips multiple containers together producing tuples of references.
template <class... TContainers> class Zipper {
  public:
    using Iterator = ZipperIterator<TContainers...>;
    using ConstIterator = Iterator;

    explicit Zipper(TContainers&... cs);

    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;

  private:
    std::tuple<decltype(std::begin(std::declval<TContainers&>()))...> m_begin_iters;
    std::size_t m_max_size{0};
};

/// Helper to create a Zipper instance from containers.
template <class... TContainers> Zipper<TContainers...> make_zipper(TContainers&... cs);

/// Like Zipper but provides element index together with tuple of references.
template <class... TContainers> class IndexedZipper {
  public:
    class Iterator {
        using BaseZipperT = Zipper<TContainers...>;
        using BaseIteratorT = typename BaseZipperT::Iterator;

      public:
        using ValueType = std::tuple<std::size_t, typename std::add_lvalue_reference<decltype(*std::begin(
                                                      std::declval<TContainers&>()))>::type...>;
        using Reference = ValueType;
        using Pointer = void;
        using DifferenceType = std::ptrdiff_t;
        using IteratorCategory = std::input_iterator_tag;

        Iterator() = default;
        explicit Iterator(BaseIteratorT it);

        Reference operator*();
        Iterator& operator++();
        Iterator operator++(int);

        bool operator==(Iterator const& rhs) const;
        bool operator!=(Iterator const& rhs) const;

      private:
        BaseIteratorT m_it{};
        template <std::size_t... TIs> Reference expand(std::index_sequence<TIs...>);
    };

    explicit IndexedZipper(TContainers&... cs);

    Iterator begin();
    Iterator end();

  private:
    Zipper<TContainers...> m_zip;
};

/// Helper to create an IndexedZipper instance.
template <class... TContainers> IndexedZipper<TContainers...> make_indexed_zipper(TContainers&... cs);

} // namespace containers

/// Convenience free functions
template <class... TContainers> auto zipper(TContainers&... cs);

template <class... TContainers> auto indexed_zipper(TContainers&... cs);
} // namespace ecs

#include "zipper.tcc"
