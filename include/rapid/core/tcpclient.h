//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_TCPCLIENT_H
#define RAPID_CORE_TCPCLIENT_H

#include <cstdint>
#include <string>
#include <memory.h>

#include <rapid/core/handler.h>
#include <rapid/core/tcpconnection.h>
#include <rapid/core/connector.h>

namespace rapid { namespace core {

class EventLoop;

class TcpClient
        : public std::enable_shared_from_this<TcpClient> {
public:
    explicit TcpClient(EventLoop *loop);

    TcpClient(const TcpClient &) = delete;
    TcpClient& operator=(const TcpClient &) = delete;

    ~TcpClient();

    void connect(const std::string &host, uint16_t port);

    void disconnect();

    void send(std::string_view str);

    void send(const void* message, size_t len);

    void shutdown();

    template <typename T>
    void setWriteHandler(T handler) {
        writeHandler_ = handler;
    }

    template <typename T>
    void setReadHandler(T handler) {
        readHandler_ = handler;
    }

    template <typename T>
    void setCloseHandler(T handler) {
        closeHandler_ = handler;
    }

private:
    void connected(int fd);

    void handleClose();

    EventLoop *loop_;
    std::unique_ptr<Connector> connector_;
    std::shared_ptr<TcpConnection> connection_;
    Handler readHandler_;
    Handler writeHandler_;
    Handler closeHandler_;
};

} }

#endif // RAPID_CORE_TCPCLIENT_H
