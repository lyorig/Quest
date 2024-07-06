#pragma once

#include <algorithm>
#include <cassert>
#include <utility>

#include <halcyon/types/numeric.hpp>

// static_vector.hpp:
// A lobotomy-inducing vector with a fixed capacity.

namespace hq {
    template <typename T, std::size_t N, typename Size_Type = std::size_t>
    class static_vector {
    public:
        using pointer       = T*;
        using const_pointer = const T*;

        using reference       = T&;
        using const_reference = const T&;

        using iterator       = pointer;
        using const_iterator = const_pointer;

        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_pointer>;

        // Constructors & destructor

        constexpr static_vector()
            : m_size { 0 } {};

        constexpr static_vector(std::initializer_list<T> il)
            : m_size { static_cast<Size_Type>(il.size()) } {
            assert(il.size() <= capacity());
            std::copy(il.begin(), il.end(), begin());
        }

        constexpr ~static_vector() {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for (auto& obj : *this) {
                    obj.~T();
                }
            }
        }

        // Modifiers

        constexpr void push_back(const T& obj) {
            emplace_back(obj);
        }

        constexpr void push_back(T&& obj) {
            emplace_back(std::move(obj));
        }

        constexpr void insert(iterator pos, T&& obj) {
            emplace(pos, std::move(obj));
        }

        template <typename... Args>
            requires std::is_constructible_v<T, Args...>
        constexpr void emplace_back(Args&&... args) {
            emplace(end(), std::forward<Args>(args)...);
        }

        template <typename... Args>
        constexpr void emplace(iterator pos, Args&&... args) {
            assert(size() < capacity());
            ++m_size;
            std::shift_right(pos, end(), 1);
            std::construct_at(pos, std::forward<Args>(args)...);
        }

        constexpr void resize(Size_Type sz) {
            if (sz == m_size) {
                return;
            }

            if (sz < m_size) {
                erase(begin() + sz, end());
            }

            else { // sz > m_size
                auto oldsz { m_size };
                m_size = sz;
                for (; oldsz != sz; ++oldsz) {
                    *(begin() + sz) = T {};
                }
            }
        }

        constexpr iterator erase(iterator pos) {
            std::shift_left(pos, end(), 1);
            --m_size;

            return pos;
        }

        constexpr void erase(iterator first, iterator last) {
            const auto dist = last - first;

            std::shift_left(first, end(), dist);
            m_size -= dist;

            return last;
        }

        constexpr void clear() {
            erase(begin(), end());
        }

        // Querying

        constexpr Size_Type size() const {
            return m_size;
        }

        constexpr static Size_Type capacity() {
            return N;
        }

        constexpr bool empty() const {
            return m_size == 0;
        }

        // Iterators

        constexpr iterator begin() {
            if constexpr (std::is_same_v<storage_type, T>) {
                return &m_data[0];
            }

            else {
                return &m_data[0].actual;
            }
        }

        constexpr iterator end() {
            return begin() + size();
        }

        constexpr const_iterator begin() const {
            if constexpr (std::is_same_v<storage_type, T>) {
                return &m_data[0];
            }

            else {
                return &m_data[0].actual;
            }
        }

        constexpr const_iterator end() const {
            return begin() + size();
        }

        constexpr reverse_iterator rbegin() {
            return std::make_reverse_iterator(end());
        }

        constexpr const_reverse_iterator rbegin() const {
            return std::make_reverse_iterator(end());
        }

        constexpr reverse_iterator rend() {
            return std::make_reverse_iterator(begin());
        }

        constexpr const_reverse_iterator rend() const {
            return std::make_reverse_iterator(begin());
        }

        constexpr reference front() {
            return *begin();
        }

        constexpr reference back() {
            return *(end() - 1);
        }

        constexpr const_reference front() const {
            return *begin();
        }

        constexpr const_reference back() const {
            return *(end() - 1);
        }

        constexpr reference operator[](Size_Type sz) {
            return *(begin() + sz);
        }

        constexpr const_reference operator[](Size_Type sz) const {
            return *(begin() + sz);
        }

    private:
        union dummy {
            dummy()
                : empty {} { }

            ~dummy() { }

            struct {
            } empty;

            T actual;
        };

        using storage_type = std::conditional_t<std::is_trivially_destructible_v<T>, T, dummy>;

        storage_type m_data[N];

        Size_Type m_size;

        // Destroys, but doesn't update member variables (e.g. size).
        void destroy_range(iterator begin, iterator end) {
            for (; begin != end; ++begin) {
                std::destroy_at(begin);
            }
        }
    };
}