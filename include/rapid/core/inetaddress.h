//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_INETADDRESS_H
#define RAPID_CORE_INETADDRESS_H

#include <sys/socket.h>

#include <string>

namespace rapid { namespace core {

class InetAddress {
public:
    InetAddress(const std::string &host, uint16_t port);

    InetAddress(const sockaddr_storage &addr, socklen_t addrLen);

    sa_family_t family() const {
        return addr_.ss_family;
    }

    const struct sockaddr* sockaddr() const {
        return reinterpret_cast<const struct sockaddr*>(&addr_);
    }

    socklen_t sockAddrLen() const {
        return addrLen_;
    }

    std::string getAddressAndPort() const;

    static InetAddress getPeerAddr(int fd);
private:
    sockaddr_storage addr_;
    socklen_t addrLen_;
};

} }

#endif // RAPID_CORE_INETADDRESS_H
