//=====================================================================================================================
// Copyright (c) 2018 libnetco project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <string>
#include <functional>

#include <sys/socket.h>

#include <rapid/core/file_desc.h>
#include <rapid/core/channel.h>

namespace rapid { namespace core {

class EventLoop;

using AcceptConnectionHandler = std::function<void(int, sockaddr_storage, socklen_t)>;

class Acceptor {
public:
    Acceptor(EventLoop* loop, const std::string &address, uint16_t port);

    Acceptor(EventLoop* loop, uint16_t port);

    Acceptor(const Acceptor &) = delete;
    Acceptor& operator=(const Acceptor &) = delete;

    void listen();

    template <typename T>
    void setNewConnectionHandler(T handler) {
        newConnectionHandler_ = std::forward<AcceptConnectionHandler>(handler);
    }

private:
    FileDesc bindLocalAddress(uint16_t port);

    FileDesc bindAddress(const std::string &address, uint16_t port);

    void handleRead();

    Channel channel_;
    AcceptConnectionHandler newConnectionHandler_;
};

} }

#endif // ACCEPTOR_H
