//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_UNIQUE_HANDLE_H
#define RAPID_CORE_UNIQUE_HANDLE_H

namespace rapid { namespace core {

template <typename Type, typename Traits>
class unique_handle {
public:
    struct boolean_struct { int member; };
    typedef int boolean_struct::* boolean_type;

    explicit unique_handle(Type value = Traits::invalid()) noexcept
        : value_(value) {
    }

    unique_handle(unique_handle&& other) noexcept
        : value_(other.release()) {
    }

    unique_handle& operator=(unique_handle&& other) noexcept {
        reset(other.release());
        return *this;
    }

    ~unique_handle() {
        close();
    }

    unique_handle(const unique_handle &) = delete;
    unique_handle& operator=(const unique_handle &) = delete;

    Type get() const noexcept {
        return value_;
    }

    void reset(Type value = Traits::invalid()) noexcept {
        if (value_ != value) {
            close();
            value_ = value;
        }
    }

    Type release() noexcept {
        auto value = value_;
        value_ = Traits::invalid();
        return value;
    }

    operator boolean_type() const noexcept {
        return Traits::invalid() != value_ ? &boolean_struct::member : nullptr;
    }

    void close() noexcept {
        if (value_ != Traits::invalid()) {
            Traits::free_resource(value_);
        }
    }
private:
    bool operator==(unique_handle const &);
    bool operator!=(unique_handle const &);

    Type value_;
};

template <typename Type, typename Traits>
auto swap(unique_handle<Type, Traits> & left, unique_handle<Type, Traits> & right) noexcept -> void {
    left.swap(right);
}

template <typename Type, typename Traits>
auto operator==(unique_handle<Type, Traits> const & left, unique_handle<Type, Traits> const & right) noexcept -> bool {
    return left.get() == right.get();
}

template <typename Type, typename Traits>
auto operator!=(unique_handle<Type, Traits> const & left, unique_handle<Type, Traits> const & right) noexcept -> bool {
    return left.get() != right.get();
}

template <typename Type, typename Traits>
auto operator<(unique_handle<Type, Traits> const & left, unique_handle<Type, Traits> const & right) noexcept -> bool {
    return left.get() < right.get();
}

template <typename Type, typename Traits>
auto operator>=(unique_handle<Type, Traits> const & left, unique_handle<Type, Traits> const & right) noexcept -> bool {
    return left.get() >= right.get();
}

template <typename Type, typename Traits>
auto operator>(unique_handle<Type, Traits> const & left, unique_handle<Type, Traits> const & right) noexcept -> bool {
    return left.get() > right.get();
}

template <typename Type, typename Traits>
auto operator<=(unique_handle<Type, Traits> const & left, unique_handle<Type, Traits> const & right) noexcept -> bool {
    return left.get() <= right.get();
}

} }

#endif // RAPID_CORE_UNIQUE_HANDLE_H
