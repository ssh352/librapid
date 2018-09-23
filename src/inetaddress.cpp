#include <array>
#include <netdb.h>
#include <arpa/inet.h>

#include <rapid/core/exception.h>
#include <rapid/core/inetaddress.h>

namespace rapid { namespace core {

static int getAddressFamily(const std::string &host) {
    struct addrinfo hints {}, *res = nullptr;

    int family = 0;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), nullptr, &hints, &res) == 0) {
        family = res->ai_family;
        freeaddrinfo(res);
    }
    return family;
}

InetAddress InetAddress::getPeerAddr(int fd) {
    sockaddr_storage address{};
    auto addrlen = static_cast<socklen_t>(sizeof(sockaddr_storage));
    getpeername(fd, reinterpret_cast<struct sockaddr *>(&address), &addrlen);
    return InetAddress(address, addrlen);
}

InetAddress::InetAddress(const std::string &host, uint16_t port) {
    const auto family = getAddressFamily(host);

    switch (family) {
    case AF_INET:
        inet_pton(AF_INET,
                  host.c_str(),
                  reinterpret_cast<struct sockaddr *>(&addr_));
        break;
    case AF_INET6:
        inet_pton(AF_INET6,
                  host.c_str(),
                  reinterpret_cast<struct sockaddr *>(&addr_));
        break;
    default:
        ensureOrThrow(false, "unknown address family");
        break;
    }
}

InetAddress::InetAddress(const sockaddr_storage &addr, socklen_t addrLen)
    : addr_(addr)
    , addrLen_(addrLen) {
}

std::string InetAddress::getAddressAndPort() const {
    std::array<char, NI_MAXHOST> hoststr;
    hoststr.fill(0);

    std::array<char, NI_MAXSERV> portstr;
    portstr.fill(0);

    getnameinfo(sockaddr(),
                addrLen_,
                hoststr.data(),
                hoststr.size(),
                portstr.data(),
                portstr.size(),
                NI_NUMERICHOST | NI_NUMERICSERV);

    return std::string(hoststr.data())
            + ":"
            + std::string(portstr.data());
}

} }
