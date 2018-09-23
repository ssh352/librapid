#include <array>

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include <rapid/core/tcpconnection.h>

namespace rapid { namespace core {

TcpConnection::TcpConnection(EventLoop *loop,
                             const uuid &id,
                             FileDesc &&fd,
                             const sockaddr_storage &peerAddr,
                             socklen_t peerAddrLen)
    : TcpConnection(loop, id, std::move(fd), InetAddress(peerAddr, peerAddrLen)) {
}

TcpConnection::TcpConnection(EventLoop *loop,
                             const uuid &id,
                             FileDesc &&fd,
                             const InetAddress &peerAddr)
    : state_(ConnectionState::Disconnected)
    , peerAddr(peerAddr)
    , id_(id)
    , loop_(loop)
    , channel_(loop_, std::move(fd))
    , logger_(log::Logger::get().getLogger("TcpConnection")) {
    channel_.setCloseHandler([this] { handleClose(); });
    channel_.setReadHandler([this] { handleRead(); });
    channel_.setWriteHandler([this] { handleWrite(); });
}

TcpConnection::~TcpConnection() {    
}

void TcpConnection::send(const void* message, size_t len) {
   channel_.send(message, len);
}

void TcpConnection::send(std::string_view str) {
    send(str.data(), str.length());
}

void TcpConnection::shutdown() {
    ::shutdown(channel_.fd().get(), SHUT_WR);
}

void TcpConnection::connectEstablished() {
    state_ = ConnectionState::Connected;
    channel_.enableRead();
    if (connectedHandler_ != nullptr) connectedHandler_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    state_ = ConnectionState::Disconnected;
    handleClose();
}

const Buffer& TcpConnection::readBuffer() const {
    return read_buffer_;
}

std::string TcpConnection::getPeerAddress() const {
    return peerAddr.getAddressAndPort();
}

void TcpConnection::handleRead() {
    if (state_ == ConnectionState::Disconnected) {
        return;
    }

    char buffer[4096]{};
    const auto byteRead = read(channel_.fd().get(), buffer, 4096);

    if (byteRead > 0) {
        read_buffer_.copyFrom(buffer, byteRead);
        if (readHandler_ != nullptr) readHandler_(shared_from_this());
        read_buffer_.clear();
    } else if (byteRead == 0) {
        handleClose();
    }
}

void TcpConnection::handleWrite() {
    const auto byteWritten = write(channel_.fd().get(),
                                   write_buffer_.slice().data(),
                                   write_buffer_.slice().length());

    if (byteWritten > 0) {
        if (writeHandler_ != nullptr) writeHandler_(shared_from_this());
    } else if (state_ == ConnectionState::Disconnecting) {
        shutdown();
    }
}

void TcpConnection::handleClose() {
    // Connection must be valid!
    channel_.disableAll();
    channel_.remove();

    state_ = ConnectionState::Disconnected;
    if (closeHandler_ != nullptr) closeHandler_(shared_from_this());
}

} }
