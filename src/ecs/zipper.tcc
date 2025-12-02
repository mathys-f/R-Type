#pragma once

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
/// Iterator that zips multiple containers together producing tuples of
/// references to the underlying container elements.
template <class... Containers> class ZipperIterator
{
    template <class Container> using IteratorT = decltype(std::begin(std::declval<Container&>()));

    template <class Container> using RefT = decltype(*std::declval<IteratorT<Container>>());

  public:
    using ValueType = std::tuple<RefT<Containers>...>;
    using Reference = ValueType;
    using Pointer = void;
    using DifferenceType = std::ptrdiff_t;
    using IteratorCategory = std::input_iterator_tag;

    using IteratorTuple = std::tuple<IteratorT<Containers>...>;

    ZipperIterator() = default;

    ZipperIterator(iterator_tuple iters, std::size_t max, std::size_t idx)
        : m_iters(std::move(iters)), m_max(max), m_idx(idx)
    {
    }

    reference operator*()
    {
        return deref(std::index_sequence_for<Containers...>{});
    }

    ZipperIterator& operator++()
    {
        increment(std::index_sequence_for<Containers...>{});
        ++m_idx;
        return *this;
    }

    ZipperIterator operator++(int)
    {
        ZipperIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    bool operator==(ZipperIterator const& rhs) const
    {
        return m_idx == rhs.m_idx;
    }

    bool operator!=(ZipperIterator const& rhs) const
    {
        return !(*this == rhs);
    }

    std::size_t index() const noexcept
    {
        return m_idx;
    }

  private:
    IteratorTuple m_iters{};
    std::size_t m_max{0};
    std::size_t m_idx{0};

    template <std::size_t... Is> reference deref(std::index_sequence<Is...>)
    {
        return reference(*std::get<Is>(m_iters)...);
    }

    template <std::size_t... Is> void increment(std::index_sequence<Is...>)
    {
        (void)std::initializer_list<int>{(++std::get<Is>(m_iters), 0)...};
    }
};

template <class... Containers> class Zipper
{
    template <class Container> using IteratorT = decltype(std::begin(std::declval<Container&>()));

    using IteratorTuple = std::tuple<IteratorT<Containers>...>;

  public:
    using Iterator = ZipperIterator<Containers...>;
    using ConstIterator = Iterator;

    /// Construct a zipper from multiple containers.
    /// @param cs Variadic list of containers to zip together.
    explicit Zipper(Containers&... cs)
        : m_begin_iters(IteratorTuple{std::begin(cs)...}),
          m_max_size(std::min({static_cast<std::size_t>(cs.size())...}))
    {
    }

    Iterator begin()
    {
        return Iterator(m_begin_iters, m_max_size, 0);
    }

    Iterator end()
    {
        return Iterator(m_begin_iters, m_max_size, m_max_size);
    }

    ConstIterator begin() const
    {
        return const_cast<Zipper*>(this)->begin();
    }
    ConstIterator end() const
    {
        return const_cast<Zipper*>(this)->end();
    }

  private:
    IteratorTuple m_begin_iters;
    std::size_t m_max_size{0};
};

/// Helper that creates a `Zipper` instance from given containers.
template <class... Containers> Zipper<Containers...> make_zipper(Containers&... cs)
{
    return Zipper<Containers...>(cs...);
}

/// Like `Zipper` but provides the element index together with the tuple
/// of references for each zipped element.
template <class... Containers> class IndexedZipper
{
  public:
    class iterator
    {
        using BaseZipperT = Zipper<Containers...>;
        using BaseIteratorT = typename BaseZipperT::Iterator;

        template <class Container> using RefT = decltype(*std::begin(std::declval<Container&>()));

      public:
        using ValueType = std::tuple<std::size_t, RefT<Containers>...>;
        using Reference = ValueType;
        using Pointer = void;
        using DifferenceType = std::ptrdiff_t;
        using IteratorCategory = std::input_iterator_tag;

        iterator() = default;

        explicit iterator(BaseIteratorT it) : m_it(it)
        {
        }

        reference operator*()
        {
            return expand(std::index_sequence_for<Containers...>{});
        }

        iterator& operator++()
        {
            ++m_it;
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        bool operator==(iterator const& rhs) const
        {
            return m_it == rhs.m_it;
        }
        bool operator!=(iterator const& rhs) const
        {
            return !(*this == rhs);
        }

      private:
        BaseIteratorT m_it{};

        template <std::size_t... Is> reference expand(std::index_sequence<Is...>)
        {
            auto t = *m_it;
            return reference(m_it.index(), std::get<Is>(t)...);
        }
    };

    explicit IndexedZipper(Containers&... cs) : m_zip(cs...)
    {
    }

    iterator begin()
    {
        return iterator(m_zip.begin());
    }
    iterator end()
    {
        return iterator(m_zip.end());
    }

  private:
    Zipper<Containers...> m_zip;
};

/// Helper to create an `IndexedZipper` instance.
template <class... Containers> IndexedZipper<Containers...> make_indexed_zipper(Containers&... cs)
{
    return IndexedZipper<Containers...>(cs...);
}

}  // namespace containers

template <class... Containers> auto zipper(Containers&... cs)
{
    return containers::make_zipper(cs...);
}

template <class... Containers> auto indexed_zipper(Containers&... cs)
{
    return containers::make_indexed_zipper(cs...);
}

}  // namespace ecs
