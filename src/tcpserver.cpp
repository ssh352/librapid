#include <rapid/core/acceptor.h>
#include <rapid/core/memory.h>
#include <rapid/core/file_desc.h>
#include <rapid/core/exception.h>
#include <rapid/core/tcpserver.h>

namespace rapid { namespace core {

TcpServer::TcpServer()
    : logger_(log::Logger::get().getLogger("TcpServer")) {
}

TcpServer::~TcpServer() {
    stop();
}

void TcpServer::start(uint16_t port) {
    ensureOrThrow(acceptor_ == nullptr, "server must be stop first");

    acceptor_ = std::make_unique<Acceptor>(pool_.nextLoop(), port);
    acceptor_->setNewConnectionHandler([this](auto fd, auto addr, auto addrLen) {
        handleNewConnection(fd, addr, addrLen);
    });
    acceptor_->listen();
    pool_.start();

    logger_->debug("server listen port:{} thread:{}",
                   port,
                   pool_.size());
}

void TcpServer::stop() {
    {
        std::lock_guard<std::mutex> m{mutex_};
        for (auto conn : clients_) {
            conn.second->connectDestroyed();
            conn.second.reset();
        }
        clients_.clear();
    }
    pool_.stop();
    acceptor_.reset();
}

void TcpServer::handleNewConnection(int fd, sockaddr_storage addr, socklen_t addrLen) {
    const auto id = uuid::newUUID();

    const int DefaultAlignedSize = 128;
    auto conn = makeShared<TcpConnection, DefaultAlignedSize>(
                pool_.nextLoop(),
                id,
                FileDesc(fd),
                addr,
                addrLen);

    conn->setConnectedHandler(connectedHandler_);    
    conn->setReadHandler(readHandler_);
    conn->setWriteHandler(writeHandler_);
    conn->setCloseHandler([this](auto conn) {
        handleClose(conn);
    });
    std::lock_guard<std::mutex> m{mutex_};
    clients_.insert(std::make_pair(id, conn));
    conn->connectEstablished();
}

void TcpServer::handleClose(std::shared_ptr<TcpConnection> conn) {
    if (closeHandler_ != nullptr) closeHandler_(conn);
    {
        std::lock_guard<std::mutex> m{mutex_};
        clients_.erase(conn->id());
    }
}

} }
