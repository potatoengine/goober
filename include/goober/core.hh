// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_CORE_HH_)
#define GOOBER_CORE_HH_
#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <type_traits>

#define GOOBER_API

inline namespace goober {
    // ------------------------------------------------------
    //  * grAllocator memory allocation handler *
    // ------------------------------------------------------

    /// @brief Allocator wrapper for Goober
    struct grAllocator {
        using Allocate = void* (*)(std::size_t sizeInBytes, void* userData);
        using Deallocate = void (*)(void* memory, std::size_t sizeInBytes, void* userData);

        GOOBER_API grAllocator() noexcept;
        grAllocator(Allocate allocate, Deallocate deallocate, void* data = nullptr) noexcept
            : _allocate(allocate)
            , _deallocate(deallocate)
            , _userData(data) {}

        /// @brief Allocates memory.
        /// @param bytes Number of bytes to allocate.
        /// @return Allocates memory; may return nullptr on failure.
        void* allocate(std::size_t bytes) const { return _allocate(bytes, _userData); }

        /// @brief Deallocates memory allocated via this allocator.
        /// @param memory Memory to deallocate. May be nullptr.
        /// @param bytes Size in bytes of memory to deallocate.
        void deallocate(void* memory, std::size_t bytes) const {
            _deallocate(memory, bytes, _userData);
        }

    private:
        Allocate _allocate = nullptr;
        Deallocate _deallocate = nullptr;
        void* _userData = nullptr;
    };

    // ------------------------------------------------------
    //  * component-wise vectors *
    // ------------------------------------------------------

    /// @brief Two-component floating-point vector type.
    struct grVec2 {
        float x = 0.f;
        float y = 0.f;

        grVec2() = default;
        constexpr grVec2(float xi, float yi) noexcept : x(xi), y(yi) {}

        constexpr friend grVec2 operator+(grVec2 l, grVec2 r) noexcept {
            return {l.x + r.x, l.y + r.y};
        }
        constexpr friend grVec2 operator-(grVec2 l, grVec2 r) noexcept {
            return {l.x - r.x, l.y - r.y};
        }

        constexpr friend grVec2 operator*(grVec2 l, float r) noexcept { return {l.x * r, l.y * r}; }

        constexpr friend bool operator==(grVec2 l, grVec2 r) noexcept {
            return l.x == r.x && l.y == r.y;
        }
        constexpr friend bool operator!=(grVec2 l, grVec2 r) noexcept {
            return l.x != r.x || l.y != r.y;
        }
    };

    /// @brief Four-component floating-point vector type.
    struct grVec4 {
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;
        float w = 0.f;

        grVec4() = default;
        constexpr grVec4(float xi, float yi, float zi, float wi) noexcept
            : x(xi)
            , y(yi)
            , z(zi)
            , w(wi) {}

        constexpr friend bool operator==(grVec4 l, grVec4 r) noexcept {
            return l.x == r.x && l.y == r.y && l.z == r.z && l.w == r.w;
        }
        constexpr friend bool operator!=(grVec4 l, grVec4 r) noexcept {
            return l.x != r.x || l.y != r.y || l.z != r.z || l.w != r.w;
        }
    };

    // ------------------------------------------------------
    //  * RGBA colors *
    // ------------------------------------------------------

    struct grColor {
        using Component = unsigned char;

        Component r = 0;
        Component g = 0;
        Component b = 0;
        Component a = 255;

        grColor() = default;
        constexpr grColor(Component ri, Component gi, Component bi, Component ai = 255) noexcept
            : r(ri)
            , g(gi)
            , b(bi)
            , a(ai) {}
    };

    namespace grColors {
        static constexpr grColor black{0, 0, 0, 255};
        static constexpr grColor white{255, 255, 255, 255};
        static constexpr grColor grey{176, 176, 176, 255};
        static constexpr grColor darkgrey{96, 96, 96, 255};
        static constexpr grColor red{255, 0, 0, 255};
        static constexpr grColor green{0, 255, 0, 255};
        static constexpr grColor blue{0, 0, 255, 255};
        static constexpr grColor yellow{255, 255, 0, 255};
        static constexpr grColor magenta{255, 0, 255, 255};
        static constexpr grColor cyan{0, 255, 255, 255};
    } // namespace grColors

    // ------------------------------------------------------
    //  * grArray dynamic array *
    // ------------------------------------------------------

    /// @brief Minimal dynamic array for goober.
    /// @tparam T Type of value contained in the array.
    template <typename T>
    struct grArray {
        using size_type = std::size_t;
        using value_type = T;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = pointer;
        using const_iterator = const_pointer;

        explicit grArray(grAllocator const& alloc) : _allocator(&alloc) {}
        inline ~grArray();

        inline grArray(grArray&& rhs) noexcept;
        inline grArray& operator=(grArray&& rhs) noexcept;

        bool empty() const noexcept { return _data == _sentinel; }
        size_type size() const noexcept { return _sentinel - _data; }
        size_type capacity() const noexcept { return _reserved - _data; }

        pointer data() noexcept { return _data; }
        const_pointer data() const noexcept { return _data; }

        reference front() noexcept { return *_data; }
        const_reference front() const noexcept { return *_data; }
        reference back() noexcept { return *(_sentinel - 1); }
        const_reference back() const noexcept { return *(_sentinel - 1); }

        reference operator[](size_type index) noexcept { return _data[index]; }
        const_reference operator[](size_type index) const noexcept { return _data[index]; }

        inline void resize(size_type newSize);
        inline void reserve(size_type newCapacity);

        inline void shrink_to_fit();

        void clear() noexcept { resize(0); }

        inline reference push_back(const_reference value);

        iterator begin() noexcept { return _data; }
        const_iterator begin() const noexcept { return _data; }

        iterator end() noexcept { return _sentinel; }
        const_iterator end() const noexcept { return _sentinel; }

    private:
        inline void _reallocate(size_type newCapacity);

        T* _data = nullptr;
        T* _sentinel = nullptr;
        T* _reserved = nullptr;
        grAllocator const* _allocator = nullptr;
    };

    // ------------------------------------------------------
    //  * grDrawList drawing helper *
    // ------------------------------------------------------

    struct grDrawList {
        using Index = std::uint16_t;
        using Offset = std::uint32_t;

        struct Vertex {
            grVec2 pos;
            grVec2 uv;
            grColor rgba;
        };

        struct Command {
            Offset indexStart = 0;
            Offset indexCount = 0;
        };

        grArray<Index> indices;
        grArray<Vertex> vertices;
        grArray<Command> commands;

        grDrawList(grAllocator const& allocator)
            : indices(allocator)
            , vertices(allocator)
            , commands(allocator) {}

        GOOBER_API void drawRect(grVec2 ul, grVec2 br, grColor color);

        void reset() noexcept {
            indices.clear();
            vertices.clear();
            commands.clear();
        }
    };

    // ------------------------------------------------------
    //  * grStatus and grResult error handling *
    // ------------------------------------------------------

    /// @brief Status codes returned by fallable goober functions.
    enum class grStatus : std::uint16_t {
        Ok,
        NullArgument,
        BadAlloc,
        Empty,
    };

    /// @brief Simple wrapper for functions that can return a value or failure status code.
    /// @tparam T Returned type for non-failure results.
    template <typename T>
    struct grResult {
        grStatus status = grStatus::Empty;
        T value;

        grResult() = default;
        grResult(grStatus stat) : status(stat), value{} {}
        grResult(T&& val) : status(grStatus::Ok), value(static_cast<T&&>(val)) {}

        /// @brief Checks if the result is in a valid state.
        /// @return True if the status is Ok.
        explicit operator bool() const noexcept { return status == grStatus::Ok; }
    };

    // ------------------------------------------------------
    //  * input mask types *
    // ------------------------------------------------------

    /// @brief Keyboard modifier key mask.
    enum class grModMask : std::uint16_t {
        None,

        LeftShift = (1 << 0),
        RightShift = (1 << 1),
        LeftAlt = (1 << 2),
        RightAlt = (1 << 3),
        LeftCtrl = (1 << 4),
        RightCtrl = (1 << 5),

        Shift = LeftShift | RightShift,
        Alt = LeftAlt | RightAlt,
        Ctrl = LeftCtrl | RightCtrl,
    };
    constexpr grModMask operator|(grModMask l, grModMask r) noexcept {
        return static_cast<grModMask>(static_cast<uint16_t>(l) | static_cast<uint16_t>(r));
    }
    constexpr grModMask operator&(grModMask l, grModMask r) noexcept {
        return static_cast<grModMask>(static_cast<uint16_t>(l) & static_cast<uint16_t>(r));
    }

    /// @brief Mouse button mask.
    enum class grButtonMask : std::uint16_t {
        None,
        Left = (1 << 0),
        Right = (1 << 1),
        Middle = (1 << 2)
    };
    constexpr grButtonMask operator|(grButtonMask l, grButtonMask r) noexcept {
        return static_cast<grButtonMask>(static_cast<uint16_t>(l) | static_cast<uint16_t>(r));
    }
    constexpr grButtonMask operator&(grButtonMask l, grButtonMask r) noexcept {
        return static_cast<grButtonMask>(static_cast<uint16_t>(l) & static_cast<uint16_t>(r));
    }

    // ------------------------------------------------------
    //  * miscellaneous public types *
    // ------------------------------------------------------

    /// @brief Unique identifier used by goober.
    enum class grId : std::uint64_t {};

    // ------------------------------------------------------
    //  * grContext core goober state *
    // ------------------------------------------------------

    /// @brief Core state object for goober.
    struct grContext {
        grAllocator allocator;
        grVec2 mousePosLast;
        grVec2 mousePos;
        grVec2 mousePosDelta;
        grModMask modifiers{};
        grButtonMask mouseButtonsLast{};
        grButtonMask mouseButtons{};
        float deltaTime = 0.f;

        grDrawList draw;

        grContext(grAllocator const& alloc) : allocator(alloc), draw(alloc) {}
    };

    // ------------------------------------------------------
    //  * miscellaneous helper functions *
    // ------------------------------------------------------

    /// @brief FNV1a hash function.
    /// @param data Bytes to hash. May only be nullptr if length is 0.
    /// @param length Number of bytes to hash. Must be 0 if data is nullptr.
    /// @return 64-bit hash of data.
    constexpr std::uint64_t grHashFnv1a(char const* data, std::size_t length) noexcept {
        std::uint64_t state = 14695981039346656037ull;
        for (size_t index = 0; index != length; ++index)
            state = (state ^ data[length]) * 1099511628211ull;
        return state;
    }

    /// @brief Tests if a point is contained within a bounding box.
    /// @param aabb Bounding box (x1, y1, x2, y2)
    /// @param pos Position to test
    /// @return True if pos is contained within aabb.
    constexpr bool grIsContained(grVec4 aabb, grVec2 pos) noexcept {
        return pos.x >= aabb.x && pos.x < aabb.z && pos.y >= aabb.y && pos.y < aabb.w;
    }

    // ------------------------------------------------------
    //  * core public interfaces *
    // ------------------------------------------------------

    GOOBER_API grResult<grContext*> grCreateContext(grAllocator& allocator);
    GOOBER_API grStatus grDestroyContext(grContext* context);

    GOOBER_API grStatus grBeginFrame(grContext* context, float deltaTime);
    GOOBER_API grStatus grEndFrame(grContext* context);

    GOOBER_API bool grIsMouseDown(grContext const* context, grButtonMask button) noexcept;
    GOOBER_API bool grIsMousePressed(grContext const* context, grButtonMask button) noexcept;
    GOOBER_API bool grIsMouseReleased(grContext const* context, grButtonMask button) noexcept;

    GOOBER_API bool grIsMouseOver(grContext const* context, grVec4 area) noexcept;
    GOOBER_API bool grIsMouseEntering(grContext const* context, grVec4 area) noexcept;
    GOOBER_API bool grIsMouseLeaving(grContext const* context, grVec4 area) noexcept;

    // ------------------------------------------------------
    //  * grArray implementation *
    // ------------------------------------------------------

    template <typename T>
    grArray<T>::~grArray() {
        if (_data != nullptr) {
            resize(0);
            shrink_to_fit();
        }
    }

    template <typename T>
    grArray<T>::grArray(grArray&& rhs) noexcept
        : _data(rhs._data)
        , _sentinel(rhs._sentinel)
        , _reserved(rhs._reserved)
        , _allocator(rhs._allocator) {
        rhs._data = rhs._sentinel = rhs._reserved = nullptr;
    }

    template <typename T>
    grArray<T>& grArray<T>::operator=(grArray&& rhs) noexcept {
        if (this != &rhs) {
            resize(0);
            shrink_to_fit();

            _data = rhs._data;
            _sentinel = rhs._sentinel;
            _reserved = rhs._reserved;
            _allocator = rhs._allocator;

            rhs._data = rhs._sentinel = rhs._reserved = nullptr;
        }
        return *this;
    }

    template <typename T>
    void grArray<T>::resize(size_type newSize) {
        size_type size = _sentinel - _data;
        if (size < newSize) {
            if (capacity() < newSize)
                reserve(newSize);

            pointer const new_sentinel = _data + newSize;
            if constexpr (!std::is_trivially_default_constructible_v<T>) {
                while (_sentinel < new_sentinel)
                    new (_sentinel++) T{};
            }
            else
                _sentinel = new_sentinel;
        }
        else if (size > newSize) {
            pointer const new_sentinel = _data + newSize;
            if constexpr (!std::is_trivially_destructible_v<T>) {
                while (_sentinel != new_sentinel)
                    (--_sentinel)->~T();
            }
            else
                _sentinel = new_sentinel;
        }
    }

    template <typename T>
    void grArray<T>::reserve(size_type newCapacity) {
        if (static_cast<size_type>(_reserved - _data) >= newCapacity)
            return;

        _reallocate(newCapacity);
    }

    template <typename T>
    void grArray<T>::shrink_to_fit() {
        if (_data == _sentinel) {
            _allocator->deallocate(_data, (_reserved - _data) * sizeof(T));
            _data = _sentinel = _reserved = nullptr;
        }
        else if (_sentinel != _reserved) {
            _reallocate(_sentinel - _data);
        }
    }

    template <typename T>
    auto grArray<T>::push_back(const_reference value) -> reference {
        if (_sentinel != _reserved)
            return *new (_sentinel++) T(value);

        auto constexpr minCapacity = 8;
        auto const size = _sentinel - _data;
        auto const newCapacity =
            size < minCapacity ? minCapacity : /*1.5 * size*/ (size + (size >> 1));

        if constexpr (std::is_nothrow_move_constructible_v<T>) {
            _reallocate(newCapacity);
            return *new (_sentinel++) T(value);
        }
        else {
            auto tmp(value);
            _reallocate(newCapacity);
            return *new (_sentinel++) T(static_cast<reference&&>(tmp));
        }
    }

    template <typename T>
    void grArray<T>::_reallocate(size_type newCapacity) {
        grArray<T> tmp = static_cast<grArray<T>&&>(*this);

        _data = static_cast<T*>(_allocator->allocate(newCapacity * sizeof(T)));
        _sentinel = _data;
        _reserved = _data + newCapacity;

        if constexpr (!std::is_trivially_move_constructible_v<T>) {
            for (pointer it = tmp._data; it != tmp._sentinel; ++it, ++_sentinel)
                new (_sentinel) T(static_cast<T&&>(*it));
        }
        else {
            size_type size = tmp._sentinel - tmp._data;
            if (size != 0) {
                _sentinel = _data + size;
                std::memcpy(_data, tmp._data, size * sizeof(T));
            }
        }
    }

} // namespace goober

#endif // defined(GOOBER_CORE_HH_)
