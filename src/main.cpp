#include <iostream>
#include <memory>

#include <rapid/core/exception.h>
#include <rapid/core/logger.h>
#include <rapid/core/tcpserver.h>
#include <rapid/core/eventloop.h>
#include <rapid/core/tcpconnection.h>
#include <rapid/core/stacktrace.h>

using namespace rapid;

int main() {
    const std::string_view http_size_100_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 100\r\n"
        "Connection: Keep-Alive\r\n"
        "\r\n"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000";

    log::Logger::get()
            .addFileLogger("http")
            .addConsoleLogger();

    auto logger = log::Logger::get().getLogger("main");

    core::TcpServer svr;

    try {
        svr.start(9090);

        svr.onConnected([logger](auto conn) {
            logger->debug("client {} was connected!", conn->getPeerAddress());
        }).onMessage([&http_size_100_response](auto conn) {
            conn->send(http_size_100_response);
        }).onWrite([&http_size_100_response](auto conn) {
            conn->send(http_size_100_response);
        }).onClose([logger](auto conn) {
            logger->debug("client {} was disconnected!", conn->getPeerAddress());
        });

        std::cin.get();

        svr.stop();
    } catch (const core::AssertException &e) {
        logger->critical("Uncaught exception! {}.\n file:{}:{} stacktrace:{}",
                         e.what(),
                         e.throwedFileName(),
                         e.throwFileline(),
                         e.trace());
    } catch (const std::exception &e) {
        logger->critical("Uncaught exception:{}", e.what());
    }

    logger->debug("server was stopped!");
}
