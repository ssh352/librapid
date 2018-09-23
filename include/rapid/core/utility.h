//=====================================================================================================================
// Copyright (c) 2018 libnetco project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef UTILITY_H
#define UTILITY_H

#include <string>

#include <sys/time.h>
#include <sys/socket.h>

#include <system_error>

namespace rapid { namespace core {

static std::string getSystemErrorString(const std::string& msg = "") {
    return std::system_error(errno, std::system_category(), msg).what();
}

static int getSocketError(int fd) {
    int optval = errno;
    auto optlen = static_cast<socklen_t>(sizeof(optval));
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &optval, &optlen);
    return optval;
}

static uint64_t getNanoTime() {
    timeval tv{};
    gettimeofday(&tv, nullptr);
    return static_cast<uint64_t>(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

} }

#endif // UTILITY_H
