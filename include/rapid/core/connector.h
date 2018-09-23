//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_CONNECTOR_H
#define RAPID_CORE_CONNECTOR_H

#include <string>
#include <cstdint>
#include <memory>
#include <atomic>
#include <functional>

#include <rapid/core/file_desc.h>
#include <rapid/core/inetaddress.h>

namespace rapid { namespace core {

class EventLoop;
class Channel;

enum class ConnectState : uint8_t {
    Connecting,
    Connected,
    Disconnected
};

using ConnectedHandler = std::function<void(int)>;

class Connector {
public:
    Connector(EventLoop *loop, const std::string &host, uint16_t port);

    Connector(const Connector &) = delete;
    Connector& operator=(const Connector &) = delete;

    void connect();

    void resetChannel();

    template <typename T>
    void setConnectedHandler(T handler) {
        connectedHandler_ = std::forward<ConnectedHandler>(handler);
    }

private:
    void connecting(FileDesc &fd);

    void retry(std::unique_ptr<Channel> &channel);

    void handleWrite();

    void handleError();

    EventLoop *loop_;
    std::atomic<ConnectState> state_;
    std::unique_ptr<Channel> channel_;
    InetAddress serverAddr_;
    ConnectedHandler connectedHandler_;
};

} }

#endif // RAPID_CORE_CONNECTOR_H
