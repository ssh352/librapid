//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_TCPCONNECTION_H
#define RAPID_CORE_TCPCONNECTION_H

#include <atomic>
#include <memory>
#include <functional>
#include <any>
#include <vector>

#include <sys/socket.h>

#include <rapid/core/logger.h>
#include <rapid/core/inetaddress.h>
#include <rapid/core/file_desc.h>
#include <rapid/core/channel.h>
#include <rapid/core/buffer.h>
#include <rapid/core/handler.h>
#include <rapid/core/uuid.h>

namespace rapid { namespace core {

enum class ConnectionState : uint8_t {
    Disconnected,
    Connecting,
    Connected,
    Disconnecting,
};

class EventLoop;

class TcpConnection
        : public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop *loop,
                  const uuid &id,
                  FileDesc &&fd,
                  const sockaddr_storage &peerAddr,
                  socklen_t peerAddrLen);

    TcpConnection(EventLoop *loop,
                  const uuid &id,
                  FileDesc &&fd,
                  const InetAddress &peerAddr);

    ~TcpConnection();

    TcpConnection(const TcpConnection &) = delete;
    TcpConnection& operator=(const TcpConnection &) = delete;

    bool isConnected() const {
        return state_ == ConnectionState::Connected;
    }

    bool isDisconnected() const {
        return state_ == ConnectionState::Disconnected;
    }

    template <typename T>
    void setConnectedHandler(T handler) {
        connectedHandler_ = handler;
    }

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

    void send(std::string_view str);

    void send(const void* message, size_t len);

    void shutdown();

    void connectEstablished();

    void connectDestroyed();

    const Buffer& readBuffer() const;

    uuid id() const {
        return id_;
    }

    std::string getPeerAddress() const;

private:
    void handleRead();

    void handleWrite();

    void handleClose();

    std::atomic<ConnectionState> state_;
    InetAddress peerAddr;
    uuid id_;    
    EventLoop *loop_;
    Channel channel_;
    Handler connectedHandler_;
    Handler readHandler_;
    Handler writeHandler_;
    Handler closeHandler_;    
    Buffer read_buffer_;
    Buffer write_buffer_;
    std::shared_ptr<spdlog::logger> logger_;
};

} }

#endif // RAPID_CORE_TCPCONNECTION_H
