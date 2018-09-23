#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <rapid/core/utility.h>
#include <rapid/core/channel.h>
#include <rapid/core/exception.h>
#include <rapid/core/connector.h>

namespace rapid { namespace core {

Connector::Connector(EventLoop *loop, const std::string &host, uint16_t port)
    : loop_(loop)
    , serverAddr_(host, port) {
}

void Connector::resetChannel() {
    channel_.reset();
}

void Connector::connect() {
    FileDesc fd(socket(serverAddr_.family(),
                       SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP));
    ensureOrThrow(fd, "socket() return fail!");

    auto ret = ::connect(fd.get(), serverAddr_.sockaddr(), serverAddr_.sockAddrLen());
    ensureOrThrow(ret == 0, "connect() return fail!");

    const auto connectError = (ret == 0) ? 0 : errno;

    switch (connectError) {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
        connecting(fd);
        break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
        retry(channel_);
        break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
        fd.reset();
        break;

    default:
        fd.reset();
        ensureOrThrow(false, "unexpected error");
        break;
    }
}

void Connector::connecting(FileDesc &fd) {
     channel_ = std::make_unique<Channel>(loop_, std::move(fd));
     channel_->setWriteHandler([this]() {
         handleWrite();
     });
     channel_->enableWrite();
}

void Connector::handleWrite() {
    if (state_ == ConnectState::Connecting) {
        channel_->disableAll();
        channel_->remove();

        const auto error = getSocketError(channel_->fd().get());
        if (error > 0) {
            retry(channel_);
        } else {
            state_ = ConnectState::Connected;
            auto fd = channel_->releaseFd();
            channel_.reset();
            if (connectedHandler_ != nullptr) connectedHandler_(fd);
        }
    }
}

void Connector::retry(std::unique_ptr<Channel> &channel) {
    channel.reset();
    state_ = ConnectState::Disconnected;
    connect();
}

} }
