#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>
#include <cassert>

#include <rapid/core/exception.h>
#include <rapid/core/acceptor.h>

namespace rapid { namespace core {

Acceptor::Acceptor(EventLoop* loop, uint16_t port)
    : channel_(loop, bindLocalAddress(port)) {
    channel_.setReadHandler([this] () {
        handleRead();
    });
}

Acceptor::Acceptor(EventLoop* loop, const std::string &address, uint16_t port)
    : channel_(loop, bindAddress(address, port)) {
    channel_.setReadHandler([this] () {
        handleRead();
    });
}

void Acceptor::listen() {
    auto res = ::listen(channel_.fd().get(), SOMAXCONN);
    ensureOrThrow(res != -1, "start listen fail!");
    channel_.enableRead();
}

void Acceptor::handleRead() {
    sockaddr_storage addr{};
    socklen_t addrLen = sizeof(addr);

    assert(channel_.fd().get() != -1);

    FileDesc fd(accept4(channel_.fd().get(), reinterpret_cast<sockaddr*>(&addr), &addrLen, SOCK_NONBLOCK));
    if (fd) {
        if (newConnectionHandler_ != nullptr) {
            // std::function can't move FileDesc, bypasss fd and release or close!
            newConnectionHandler_(fd.release(), addr, addrLen);
        }
    }
}

FileDesc Acceptor::bindLocalAddress(uint16_t port) {
    sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons (port);
    localAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    FileDesc fd(socket(AF_INET, SOCK_STREAM, 0));
    auto res = bind(fd.get(), reinterpret_cast<sockaddr*>(&localAddr), sizeof(localAddr));
    ensureOrThrow(res == 0, "bind TCP port fail!");
    return fd;
}

FileDesc Acceptor::bindAddress(const std::string &address, uint16_t port) {
    addrinfo hints{};
    addrinfo *result{};
    addrinfo *rp{};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    auto info = getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints, &result);
    ensureOrThrow(info == 0, "getaddrinfo() return fail");

    FileDesc fd;
    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        FileDesc tryBindSocket(socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol));
        ensureOrThrow(tryBindSocket, "socket() return fail!");

        auto enable = 1;
        auto res = setsockopt(tryBindSocket.get(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&enable), sizeof(int));
        assert(res == 0);

        res = setsockopt(tryBindSocket.get(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&enable), sizeof(int));
        assert(res == 0);

        if (bind(tryBindSocket.get(), rp->ai_addr, rp->ai_addrlen) == 0) {
            fd = std::move(tryBindSocket);
            break;
        }        
    }
    freeaddrinfo(result);
    return fd;
}

} }
