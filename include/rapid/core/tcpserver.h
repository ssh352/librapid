//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_TCPSERVER_H
#define RAPID_CORE_TCPSERVER_H

#include <memory>
#include <unordered_map>
#include <mutex>

#include <rapid/core/eventlooppool.h>
#include <rapid/core/acceptor.h>
#include <rapid/core/tcpconnection.h>
#include <rapid/core/uuid.h>
#include <rapid/core/handler.h>
#include <rapid/core/logger.h>

namespace rapid { namespace core {

class TcpServer {
public:
    TcpServer();

    ~TcpServer();

    TcpServer(const TcpServer &) = delete;
    TcpServer& operator=(const TcpServer &) = delete;

    void start(uint16_t port);

    void stop();

    template <typename T>
    TcpServer& onConnected(T handler) {
        connectedHandler_ = std::forward<Handler>(handler);
        return *this;
    }

    template <typename T>
    TcpServer& onWrite(T handler) {
        writeHandler_ = std::forward<Handler>(handler);
        return *this;
    }

    template <typename T>
    TcpServer& onMessage(T handler) {
        readHandler_ = std::forward<Handler>(handler);
        return *this;
    }

    template <typename T>
    TcpServer& onClose(T handler) {
        closeHandler_ = std::forward<Handler>(handler);
        return *this;
    }

private:
    void handleNewConnection(int fd, sockaddr_storage addr, socklen_t addrLen);

    void handleClose(std::shared_ptr<TcpConnection> conn);

    EventLoopPool pool_;
    std::unique_ptr<Acceptor> acceptor_;
    Handler connectedHandler_;
    Handler readHandler_;
    Handler writeHandler_;
    Handler closeHandler_;
    mutable std::mutex mutex_;
    std::shared_ptr<spdlog::logger> logger_;
    std::unordered_map<uuid, std::shared_ptr<TcpConnection>> clients_;
};


} }

#endif // RAPID_CORE_TCPSERVER_H
