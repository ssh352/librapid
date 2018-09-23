//=====================================================================================================================
// Copyright (c) 2018 libnetco project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef OPENSSL_H
#define OPENSSL_H

#include <vector>
#include <mutex>
#include <memory>
#include <thread>

#include <net/core/channel.h>

namespace rapid { namespace core {

class Channel;

class SSLContext {
public:
    static const int SSL_BUF_SIZE = 8192;

    SSLContext();

    ~SSLContext();

    void handshake(Channel &channel);

    int read(void *messsage, size_t len);

    int write(const void *messsage, size_t len);

    SSLContext(const SSLContext &) = delete;
    SSLContext& operator=(const SSLContext &) = delete;

private:
    class SSLContextImpl;
    std::unique_ptr<SSLContextImpl> impl_;
};

} }

#endif // OPENSSL_H
