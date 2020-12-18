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

#if !defined(grAlloc)
#define grAlloc(bytes) std::malloc((bytes))
#endif
#if !defined(grFree)
#define grFree(mem) std::free((mem))
#endif

inline namespace goober {
    // ------------------------------------------------------
    //  * forward declarations *
    // ------------------------------------------------------

    struct grContext;
    struct grFont;
    struct grFontAtlas;
    struct grPortal;
    struct grDrawList;

    // ------------------------------------------------------
    //  * miscellaneous public types *
    // ------------------------------------------------------

    /// @brief Unique identifier used by goober.
    using grId = std::uint64_t;

    /// @brief Texture id.
    using grTextureId = std::uint64_t;

    /// @brief Font id.
    using grFontId = std::uint64_t;

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

    /// @brief Axis-aligned box
    struct grRect {
        grVec2 minimum;
        grVec2 maximum;

        grRect() = default;
        constexpr grRect(grVec2 mini, grVec2 maxi) noexcept : minimum(mini), maximum(maxi) {}
        constexpr grRect(float x0, float y0, float x1, float y1) noexcept
            : minimum(x0, y0)
            , maximum(x1, y1) {}

        const bool empty() const noexcept { return maximum.x > minimum.x && maximum.y > minimum.y; }
        constexpr grVec2 size() const noexcept { return maximum - minimum; }
        constexpr grVec2 center() const noexcept { return minimum + (maximum - minimum) * 0.5f; }
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
    //  * grBoxed dynamic memory *
    // ------------------------------------------------------

    /// @brief Manages lifetime of a heap-allocated object.
    /// @tparam T Type of object.
    template <typename T>
    struct grBoxed {
        grBoxed() = default;
        explicit grBoxed(T* object) noexcept : _object(object) {}
        ~grBoxed() { reset(); }

        grBoxed(grBoxed const&) = delete;
        grBoxed& operator=(grBoxed const&) = delete;

        bool empty() const noexcept { return _object != nullptr; }
        T* get() const noexcept { return _object; }

        void reset();
        void reset(T* object);

        T* operator->() const noexcept { return _object; }
        T& operator*() const noexcept { return *_object; }

    private:
        T* _object = nullptr;
    };

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

        grArray() = default;
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

        inline void pop_back() noexcept;

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
    };

    // ------------------------------------------------------
    //  * string helpers *
    // ------------------------------------------------------

    /// @brief Non-owning reference to a view of characters; not NUL-terminated.
    struct grStringView {
        using value_type = char const;
        using pointer = char const*;
        using size_type = std::size_t;

        pointer data = nullptr;
        pointer sentinel = nullptr;

        grStringView() = default;
        grStringView(std::nullptr_t) = delete;
        /*implicit*/ grStringView(char const* zstr) noexcept
            : data(zstr)
            , sentinel(data + std::strlen(data)) {}
        template <std::size_t N>
        /*implicit*/ constexpr grStringView(char const (&nstr)[N]) noexcept
            : data(nstr)
            , sentinel(nstr + (N - 1)) {}

        constexpr bool empty() const noexcept { return data == sentinel; }
        constexpr size_type size() const noexcept { return sentinel - data; }

        constexpr pointer begin() const noexcept { return data; }
        constexpr pointer end() const noexcept { return sentinel; }
    };

    /// @brief Owning container for an immutable string.
    struct grString {
        using value_type = char const;
        using pointer = char const*;
        using size_type = std::size_t;

        grString() = default;
        grString(std::nullptr_t) = delete;
        explicit grString(pointer zstr) : grString(zstr, std::strlen(zstr)) {}
        explicit grString(grStringView str) : grString(str.data, str.size()) {}
        inline explicit grString(pointer nstr, size_type size);
        inline ~grString();

        constexpr grString(grString&& rhs) noexcept : _data(rhs._data), _sentinel(rhs._sentinel) {
            rhs._data = rhs._sentinel = nullptr;
        }
        inline grString& operator=(grString&& rhs) noexcept;

        bool empty() const noexcept { return _data == _sentinel; }
        size_type size() const noexcept { return _sentinel - _data; }

        pointer begin() const noexcept { return _data; }
        pointer end() const noexcept { return _sentinel; }

    private:
        char* _data = nullptr;
        char* _sentinel = nullptr;
    };

    // ------------------------------------------------------
    //  * grStatus and grResult error handling *
    // ------------------------------------------------------

    /// @brief Status codes returned by fallable goober functions.
    enum class grStatus : std::uint16_t {
        Ok,
        NullArgument,
        InvalidId,
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
        grResult(T const& val) : status(grStatus::Ok), value(val) {}

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
    //  * grContext core goober state *
    // ------------------------------------------------------

    /// @brief Core state object for goober.
    struct grContext {
        grVec2 mousePosLast;
        grVec2 mousePos;
        grVec2 mousePosDelta;
        grModMask modifiers{};
        grButtonMask mouseButtonsLast{};
        grButtonMask mouseButtons{};
        float deltaTime = 0.f;
        grPortal* root = nullptr;
        grArray<grPortal*> portalStack;
        grArray<grPortal*> portals;
        grArray<grFont*> fonts;
        grId activeId = {};
        grId activeIdNext = {};
        grPortal* currentPortal = nullptr;
        grDrawList* currentDrawList = nullptr;
        grFontAtlas* fontAtlas = nullptr;
        grFontId nextFontId = 1;
    };

    // ------------------------------------------------------
    //  * grPortal widget and state container *
    // ------------------------------------------------------

    struct grPortal {
        grBoxed<grDrawList> draw;
        grString name;
        grId id = {};
        grArray<grId> idStack;
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

    /// @brief FNV1a hash function for strings.
    /// @param str String to hash.
    /// @return 64-bit hash of string.
    constexpr std::uint64_t grHashFnv1a(grStringView str) noexcept {
        return grHashFnv1a(str.data, str.size());
    }

    /// @brief Combines two hash values.
    /// @param seed First hash value.
    /// @param hash Second hash value.
    /// @return A new hash.
    constexpr std::uint64_t grHashCombine(std::uint64_t seed, std::uint64_t hash) noexcept {
        // from CityHash
        constexpr std::uint64_t mul = 0x9ddfea08eb382d69ull;
        std::uint64_t a = (hash ^ seed) * mul;
        a ^= a >> 47;
        std::uint64_t b = (seed ^ a) * mul;
        b ^= b >> 47;
        seed = b * mul;
        return seed;
    }

    /// @brief Tests if a point is contained within a bounding box.
    /// @param aabb Bounding box (x1, y1, x2, y2)
    /// @param pos Position to test
    /// @return True if pos is contained within aabb.
    constexpr bool grIsContained(grRect aabb, grVec2 pos) noexcept {
        return pos.x >= aabb.minimum.x && pos.x < aabb.maximum.x && pos.y >= aabb.minimum.y &&
            pos.y < aabb.maximum.y;
    }

    // ------------------------------------------------------
    //  * core public interfaces *
    // ------------------------------------------------------

    GOOBER_API grResult<grContext*> grCreateContext();
    GOOBER_API grStatus grDestroyContext(grContext* context);

    GOOBER_API grResult<grId> grBeginPortal(grContext* context, grStringView name);
    GOOBER_API grStatus grEndPortal(grContext* context);
    GOOBER_API grPortal* grCurrentPortal(grContext* context);

    GOOBER_API grDrawList* grCurrentDrawList(grContext* context);

    GOOBER_API grStatus grBeginFrame(grContext* context, float deltaTime);
    GOOBER_API grStatus grEndFrame(grContext* context);

    GOOBER_API grId grGetId(grContext const* context, uint64_t hash) noexcept;
    inline grId grGetId(grContext const* context, void const* ptr) noexcept {
        return grGetId(context, grHashFnv1a(reinterpret_cast<char const*>(&ptr), sizeof(ptr)));
    }
    inline grId grGetId(grContext const* context, grStringView str) noexcept {
        return grGetId(context, grHashFnv1a(str));
    }

    GOOBER_API grStatus grPushId(grContext* context, grId id);
    GOOBER_API grStatus grPopId(grContext* context) noexcept;

    GOOBER_API bool grIsMouseDown(grContext const* context, grButtonMask button) noexcept;
    GOOBER_API bool grIsMousePressed(grContext const* context, grButtonMask button) noexcept;
    GOOBER_API bool grIsMouseReleased(grContext const* context, grButtonMask button) noexcept;

    GOOBER_API bool grIsMouseOver(grContext const* context, grRect area) noexcept;
    GOOBER_API bool grIsMouseEntering(grContext const* context, grRect area) noexcept;
    GOOBER_API bool grIsMouseLeaving(grContext const* context, grRect area) noexcept;

    // ------------------------------------------------------
    //  * grBoxed implementation *
    // ------------------------------------------------------

    template <typename T>
    void grBoxed<T>::reset() {
        if (_object != nullptr) {
            _object->~T();
            grFree(_object);
            _object = nullptr;
        }
    }

    template <typename T>
    void grBoxed<T>::reset(T* object) {
        if (_object != nullptr && _object != object) {
            _object->~T();
            grFree(_object);
        }
        _object = object;
    }

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
        , _reserved(rhs._reserved) {
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
    void grArray<T>::pop_back() noexcept {
        (--_sentinel)->~T();
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
            grFree(_data);
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

        _data = static_cast<T*>(grAlloc(newCapacity * sizeof(T)));
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

    // ------------------------------------------------------
    //  * grString implementation *
    // ------------------------------------------------------

    grString::grString(pointer nstr, size_type size)
        : _data(static_cast<char*>(grAlloc(size + 1)))
        , _sentinel(_data + size) {
        std::memcpy(_data, nstr, size);
        _data[size] = '\0';
    }

    grString::~grString() {
        if (_data != nullptr)
            grFree(_data);
    }

    grString& grString::operator=(grString&& rhs) noexcept {
        if (_data != nullptr && _data != rhs._data)
            grFree(_data);

        _data = rhs._data;
        _sentinel = rhs._sentinel;

        rhs._data = rhs._sentinel = nullptr;
        return *this;
    }

} // namespace goober

#endif // defined(GOOBER_CORE_HH_)
