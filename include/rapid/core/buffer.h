//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_CHABUFFFER_H
#define RAPID_CORE_CHABUFFFER_H

#include <string>

namespace rapid { namespace core {

// todo: implementation SOO ?
class Buffer {
public:
    Buffer();

    void copyFrom(const char *buffer, size_t len);

    std::string_view slice() const;

    std::string readToEnd() const;

    size_t readable() const;

    void clear();
private:
    std::string buffer_;
};

} }

#endif // RAPID_CORE_CHABUFFFER_H
