//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_FILE_DESC_H
#define RAPID_CORE_FILE_DESC_H

#include <unistd.h>

#include <rapid/core/unique_handle.h>

namespace rapid { namespace core {

struct FileDescriptorTraits {
    static int invalid() noexcept {
        return -1;
    }

    static void free_resource(int value) noexcept {
        ::close(value);
    }
};

using FileDesc = unique_handle<int, FileDescriptorTraits>;

} }

#endif // RAPID_CORE_FILE_DESC_H
