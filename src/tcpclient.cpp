#include <cassert>
#include <rapid/core/memory.h>
#include <rapid/core/channel.h>
#include <rapid/core/exception.h>
#include <rapid/core/tcpclient.h>

namespace rapid { namespace core {

TcpClient::TcpClient(EventLoop *loop)
    : loop_(loop) {
    assert(loop != nullptr);
}

TcpClient::~TcpClient() {
    disconnect();
}

void TcpClient::connect(const std::string &host, uint16_t port) {
    ensureOrThrow(connector_ == nullptr, "connect must be before disconnect");

    connector_ = std::make_unique<Connector>(loop_, host, port);
    connector_->setConnectedHandler([this](auto fd) {
        connected(fd);
    });
    connector_->connect();
}

void TcpClient::disconnect() {
    if (connection_ != nullptr)
        connection_->shutdown();

    if (connector_ != nullptr)
        connector_->resetChannel();
}

void TcpClient::handleClose() {
    connection_->connectDestroyed();
    if (closeHandler_ != nullptr) closeHandler_(connection_);
    connection_.reset();
}

void TcpClient::connected(int fd) {
    const auto peerAddr = InetAddress::getPeerAddr(fd);
    connection_ = makeShared<TcpConnection>(loop_, uuid::newUUID(), FileDesc(fd), peerAddr);

    auto client = shared_from_this();
    connection_->setCloseHandler([client](std::shared_ptr<TcpConnection>) {
        client->handleClose();
    });
    connection_->setReadHandler(readHandler_);
    connection_->setWriteHandler(writeHandler_);
    connection_->connectEstablished();
}

void TcpClient::send(std::string_view str) {
    connection_->send(str);
}

void TcpClient::send(const void* message, size_t len) {
    connection_->send(message, len);
}

void TcpClient::shutdown() {
    connection_->shutdown();
}

} }
