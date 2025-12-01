#pragma once

#include <tuple>
#include <utility>
#include <type_traits>
#include <cstddef>
#include <algorithm>
#include <iterator>

namespace containers
{
    template <class... Containers>
    class zipper_iterator
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

        zipper_iterator() = default;

        zipper_iterator(iterator_tuple iters,
                        std::size_t max,
                        std::size_t idx)
            : _iters(std::move(iters)), _max(max), _idx(idx)
        {
        }

        reference operator*()
        {
            return deref(std::index_sequence_for<Containers...>{});
        }

        zipper_iterator &operator++()
        {
            increment(std::index_sequence_for<Containers...>{});
            ++_idx;
            return *this;
        }

        zipper_iterator operator++(int)
        {
            zipper_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        bool operator==(zipper_iterator const &rhs) const
        {
            return _idx == rhs._idx;
        }

        bool operator!=(zipper_iterator const &rhs) const
        {
            return !(*this == rhs);
        }

        std::size_t index() const noexcept { return _idx; }

    private:
        iterator_tuple _iters{};
        std::size_t _max{0};
        std::size_t _idx{0};

        template <std::size_t... Is>
        reference deref(std::index_sequence<Is...>)
        {
            return reference(*std::get<Is>(_iters)...);
        }

        template <std::size_t... Is>
        void increment(std::index_sequence<Is...>)
        {
            (void)std::initializer_list<int>{
                (++std::get<Is>(_iters), 0)...};
        }
    };

    template <class... Containers>
    class zipper
    {
        template <class Container>
        using iterator_t =
            decltype(std::begin(std::declval<Container &>()));

        using iterator_tuple = std::tuple<iterator_t<Containers>...>;

    public:
        using iterator       = zipper_iterator<Containers...>;
        using const_iterator = iterator;

        explicit zipper(Containers &...cs)
            : _begin_iters(iterator_tuple{std::begin(cs)...})
            , _max_size(std::min({static_cast<std::size_t>(cs.size())...}))
        {
        }

        iterator begin()
        {
            return iterator(_begin_iters, _max_size, 0);
        }

        iterator end()
        {
            // same iterators, index = max_size (never dereferenced)
            return iterator(_begin_iters, _max_size, _max_size);
        }

        const_iterator begin() const { return const_cast<zipper *>(this)->begin(); }
        const_iterator end() const { return const_cast<zipper *>(this)->end(); }

    private:
        iterator_tuple _begin_iters;
        std::size_t _max_size{0};
    };

    template <class... Containers>
    zipper<Containers...> make_zipper(Containers &...cs)
    {
        return zipper<Containers...>(cs...);
    }

    // indexed_zipper: yields (index, c0[i], c1[i], ...)
    template <class... Containers>
    class indexed_zipper
    {
    public:
        class iterator
        {
            using base_zipper_t    = zipper<Containers...>;
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

            explicit iterator(base_iterator_t it) : _it(it) {}

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

            bool operator==(iterator const &rhs) const { return _it == rhs._it; }
            bool operator!=(iterator const &rhs) const { return !(*this == rhs); }

        private:
            base_iterator_t _it{};

            template <std::size_t... Is>
            reference expand(std::index_sequence<Is...>)
            {
                auto t = *_it; // tuple<ref...>
                return reference(_it.index(), std::get<Is>(t)...);
            }
        };

        explicit indexed_zipper(Containers &...cs)
            : _zip(cs...)
        {
        }

        iterator begin() { return iterator(_zip.begin()); }
        iterator end() { return iterator(_zip.end()); }

    private:
        zipper<Containers...> _zip;
    };

    template <class... Containers>
    indexed_zipper<Containers...> make_indexed_zipper(Containers &...cs)
    {
        return indexed_zipper<Containers...>(cs...);
    }

} // namespace containers

// Convenience free functions in global namespace
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
