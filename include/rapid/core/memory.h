//=====================================================================================================================
// Copyright (c) 2018 libnetco project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef MEMROY_H
#define MEMROY_H

#include <stdlib.h>
#include <memory>

namespace rapid { namespace core {

static constexpr bool isValidAlignment(size_t alignment) {
    return (alignment & (alignment - 1)) == 0;
}

static void * alignedMalloc(size_t size, int align) {
    void *ptr = nullptr;
    (void) posix_memalign(&ptr, align, size);
    return ptr;
}

static void alignedFree(void *p) {
    free(p);
}

template <typename T, size_t AlignedSize>
class aligned_allocator : public std::allocator<T> {
public:
    using value_type = T;
    using pointer = T*;

    aligned_allocator()                                         = default;
    aligned_allocator(const aligned_allocator&)                 = default;
    aligned_allocator& operator=(const aligned_allocator&)      = default;
    aligned_allocator(aligned_allocator&&) noexcept             = default;
    aligned_allocator& operator=(aligned_allocator&&) noexcept  = default;

    template <typename U>
    aligned_allocator(const aligned_allocator<U, AlignedSize>&) {
    }

    virtual ~aligned_allocator() noexcept = default;

    template <typename U>
    struct rebind {
        using other = aligned_allocator<U, AlignedSize>;
    };

    T* allocate(std::size_t n) {        
        return static_cast<pointer>(alignedMalloc(n * sizeof(T), AlignedSize));
    }

    void deallocate(T* p, size_t) {
        alignedFree(p);
    }
};

template <typename T, typename U, size_t AlignedSize>
constexpr bool operator==(const aligned_allocator<T, AlignedSize>&,
                          const aligned_allocator<U, AlignedSize>&) {
    return true;
}

template <typename T, typename U, size_t AlignedSize>
constexpr bool operator!=(const aligned_allocator<T, AlignedSize>&,
                          const aligned_allocator<U, AlignedSize>&) {
    return false;
}

template <typename T, std::size_t AlignSize = std::alignment_of<T>::value, typename... Args>
std::shared_ptr<T> makeShared(Args&&... args) {
    static_assert(isValidAlignment(AlignSize), "alignment size is invalid");

    const std::size_t default_alignment = 16;

    if (AlignSize > default_alignment) {
        typedef aligned_allocator<T, AlignSize> alloc_type;
        return std::allocate_shared<T, alloc_type>(alloc_type(), std::forward<Args>(args)...);
    }
    else {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}

} }

#endif // MEMROY_H
