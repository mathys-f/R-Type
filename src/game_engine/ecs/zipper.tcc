#pragma once

#include <tuple>
#include <utility>
#include <type_traits>
#include <cstddef>
#include <algorithm>
#include <iterator>

namespace containers
{
    /// Iterator that zips multiple containers together producing tuples of
    /// references to the underlying container elements.
    template <class... Containers>
    class ZipperIterator
    {
        template <class Container>
        using iterator_t =
            decltype(std::begin(std::declval<Container &>()));

        template <class Container>
        using ref_t = decltype(*std::declval<iterator_t<Container>>());

    public:
        using value_type        = std::tuple<ref_t<Containers>...>;
        using reference         = value_type;
        using pointer           = void;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        using iterator_tuple = std::tuple<iterator_t<Containers>...>;

        ZipperIterator() = default;

        ZipperIterator(iterator_tuple iters,
                       std::size_t max,
                       std::size_t idx)
            : m_iters(std::move(iters)), m_max(max), m_idx(idx)
        {
        }

        reference operator*()
        {
            return deref(std::index_sequence_for<Containers...>{});
        }

        ZipperIterator &operator++()
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

        bool operator==(ZipperIterator const &rhs) const
        {
            return m_idx == rhs.m_idx;
        }

        bool operator!=(ZipperIterator const &rhs) const
        {
            return !(*this == rhs);
        }

        std::size_t index() const noexcept { return m_idx; }

    private:
        iterator_tuple m_iters{};
        std::size_t m_max{0};
        std::size_t m_idx{0};

        template <std::size_t... Is>
        reference deref(std::index_sequence<Is...>)
        {
            return reference(*std::get<Is>(m_iters)...);
        }

        template <std::size_t... Is>
        void increment(std::index_sequence<Is...>)
        {
            (void)std::initializer_list<int>{
                (++std::get<Is>(m_iters), 0)...};
        }
    };

    template <class... Containers>
    class Zipper
    {
        template <class Container>
        using iterator_t =
            decltype(std::begin(std::declval<Container &>()));

        using iterator_tuple = std::tuple<iterator_t<Containers>...>;

    public:
        using iterator       = ZipperIterator<Containers...>;
        using const_iterator = iterator;

        /// Construct a zipper from multiple containers.
        /// @param cs Variadic list of containers to zip together.
        explicit Zipper(Containers &...cs)
            : m_begin_iters(iterator_tuple{std::begin(cs)...})
            , m_max_size(std::min({static_cast<std::size_t>(cs.size())...}))
        {
        }

        iterator begin()
        {
            return iterator(m_begin_iters, m_max_size, 0);
        }

        iterator end()
        {
            return iterator(m_begin_iters, m_max_size, m_max_size);
        }

        const_iterator begin() const { return const_cast<Zipper *>(this)->begin(); }
        const_iterator end() const { return const_cast<Zipper *>(this)->end(); }

    private:
        iterator_tuple m_begin_iters;
        std::size_t m_max_size{0};
    };

    /// Helper that creates a `Zipper` instance from given containers.
    template <class... Containers>
    Zipper<Containers...> make_zipper(Containers &...cs)
    {
        return Zipper<Containers...>(cs...);
    }

    /// Like `Zipper` but provides the element index together with the tuple
    /// of references for each zipped element.
    template <class... Containers>
    class IndexedZipper
    {
    public:
        class iterator
        {
            using base_zipper_t    = Zipper<Containers...>;
            using base_iterator_t  = typename base_zipper_t::iterator;

            template <class Container>
            using ref_t = decltype(*std::begin(std::declval<Container &>()));

        public:
            using value_type        = std::tuple<std::size_t, ref_t<Containers>...>;
            using reference         = value_type;
            using pointer           = void;
            using difference_type   = std::ptrdiff_t;
            using iterator_category = std::input_iterator_tag;

            iterator() = default;

            explicit iterator(base_iterator_t it) : m_it(it) {}

            reference operator*()
            {
                return expand(std::index_sequence_for<Containers...>{});
            }

            iterator &operator++()
            {
                ++_it;
                return *this;
            }

            iterator operator++(int)
            {
                iterator tmp(*this);
                ++(*this);
                return tmp;
            }

            bool operator==(iterator const &rhs) const { return m_it == rhs.m_it; }
            bool operator!=(iterator const &rhs) const { return !(*this == rhs); }

        private:
            base_iterator_t m_it{};

            template <std::size_t... Is>
            reference expand(std::index_sequence<Is...>)
            {
                auto t = *m_it;
                return reference(m_it.index(), std::get<Is>(t)...);
            }
        };

        explicit IndexedZipper(Containers &...cs)
            : m_zip(cs...)
        {
        }

        iterator begin() { return iterator(m_zip.begin()); }
        iterator end() { return iterator(m_zip.end()); }

    private:
        Zipper<Containers...> m_zip;
    };

    /// Helper to create an `IndexedZipper` instance.
    template <class... Containers>
    IndexedZipper<Containers...> make_indexed_zipper(Containers &...cs)
    {
        return IndexedZipper<Containers...>(cs...);
    }

}

template <class... Containers>
auto zipper(Containers &...cs)
{
    return containers::make_zipper(cs...);
}

template <class... Containers>
auto indexed_zipper(Containers &...cs)
{
    return containers::make_indexed_zipper(cs...);
}
