// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_ARRAY_HH_)
#define GOOBER_ARRAY_HH_
#pragma once

#include "core.hh"

#include <cstdlib>
#include <type_traits>

inline namespace goober {
    // ------------------------------------------------------
    //  * public types *
    // ------------------------------------------------------
    template <typename T>
    struct grArray {
        using size_type = size_t;
        using value_type = T;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = pointer;
        using const_iterator = const_pointer;

        T* first = nullptr;
        T* last = nullptr;
        T* sentinel = nullptr;
        grAllocator const* allocator = nullptr;

        explicit grArray(grAllocator const& alloc) : allocator(&alloc) {}
        inline ~grArray();

        inline grArray(grArray&& rhs) noexcept;
        inline grArray& operator=(grArray&& rhs) noexcept;

        size_type size() const noexcept { return last - first; }
        size_type capacity() const noexcept { return sentinel - first; }

        inline void resize(size_type newSize);
        inline void reserve(size_type newCapacity) { _reserve(newCapacity); }

        inline void shrink_to_fit();

        inline reference push_back(const_reference value);

        friend iterator begin(grArray& array) noexcept { return array.first; }
        friend const_iterator begin(grArray const& array) noexcept { return array.first; }

        friend iterator end(grArray& array) noexcept { return array.last; }
        friend const_iterator end(grArray const& array) noexcept { return array.last; }

        inline grArray<T> _reserve(size_type newCapacity);
    };

    // ------------------------------------------------------
    //  * implementation *
    // ------------------------------------------------------

    template <typename T>
    grArray<T>::~grArray() {
        if (first != nullptr) {
            resize(0);
            shrink_to_fit();
        }
    }

    template <typename T>
    grArray<T>::grArray(grArray&& rhs) noexcept
        : first(rhs.first)
        , last(rhs.last)
        , sentinel(rhs.sentinel)
        , allocator(rhs.allocator) {
        rhs.first = rhs.last = rhs.sentinel = nullptr;
    }

    template <typename T>
    grArray<T>& grArray<T>::operator=(grArray&& rhs) noexcept {
        if (this != &rhs) {
            resize(0);
            shrink_to_fit();

            first = rhs.first;
            last = rhs.last;
            sentinel = rhs.sentinel;
            allocator = rhs.allocator;

            rhs.first = rhs.last = rhs.sentinel = nullptr;
        }
        return *this;
    }

    template <typename T>
    void grArray<T>::resize(size_type newSize) {
        size_type size = last - first;
        if (size < newSize) {
            if (capacity() < newSize)
                _reserve(newSize);

            pointer const newLast = first + newSize;
            if constexpr (!std::is_trivially_default_constructible_v<T>) {
                while (last < newLast())
                    new (last++) T{};
            }
            else
                last = newLast;
        }
        else if (size > newSize) {
            pointer const newLast = first + newSize;
            if constexpr (!std::is_trivially_destructible_v<T>) {
                while (last != newLast)
                    (--last)->~T();
            }
            else
                last = newLast;
        }
    }

    template <typename T>
    void grArray<T>::shrink_to_fit() {
        if (first == last) {
            allocator->free(first, (sentinel - first) * sizeof(T), allocator->userData);
            first = last = sentinel = nullptr;
        }
        else if (last != sentinel) {
            size_type const size = last - first;
            grArray<T> tmp = static_cast<grArray<T>&&>(*this);

            if (size == 0)
                return;

            first = static_cast<T*>(allocator->alloc(size * sizeof(T), allocator->userData));
            last = first;
            sentinel = first + size;

            if constexpr (!std::is_trivially_move_constructible_v<T>) {
                for (pointer it = tmp.first; it != tmp.last; ++it, ++last)
                    new (last) T(static_cast<T&&>(*it));
            }
            else {
                last = first + size;
                std::memcpy(first, tmp.first, size * sizeof(T));
            }
        }
    }

    template <typename T>
    auto grArray<T>::push_back(const_reference value) -> reference {
        if (last == sentinel) {
            auto const size = last - first;
            auto constexpr minSize = 8;
            auto tmp = _reserve(size < minSize ? minSize : /*1.5 * size*/ (size + (size >> 1)));
            return *new (last++) T(value);
        }
        else
            return *new (last++) T(value);
    }

    template <typename T>
    grArray<T> grArray<T>::_reserve(size_type newCapacity) {
        if (sentinel - first >= newCapacity)
            return grArray<T>(*allocator);

        grArray<T> tmp = static_cast<grArray<T>&&>(*this);

        first = static_cast<T*>(allocator->alloc(newCapacity * sizeof(T), allocator->userData));
        last = first;
        sentinel = first + newCapacity;

        if constexpr (!std::is_trivially_move_constructible_v<T>) {
            for (pointer it = tmp.first; it != tmp.last; ++it, ++last)
                new (last) T(static_cast<T&&>(*it));
        }
        else {
            size_type size = tmp.last - tmp.first;
            last = first + size;
            if (size != 0)
                std::memcpy(first, tmp.first, size * sizeof(T));
        }

        return tmp;
    }

} // namespace goober

#endif // defined(GOOBER_ARRAY_HH_)
