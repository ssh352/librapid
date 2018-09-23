TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt
LIBS =-pthread -luuid -lstdc++fs -ldl
INCLUDEPATH += ./thirdparty/spdlog/include\
               ./include/
SOURCES += \
    ./src/acceptor.cpp\
    ./src/buffer.cpp\
    ./src/connector.cpp\
    ./src/channel.cpp\
    ./src/epollpoller.cpp\
    ./src/eventloop.cpp\
    ./src/eventlooppool.cpp\
    ./src/exception.cpp\
    ./src/inetaddress.cpp\
    ./src/logger.cpp\
    ./src/tcpclient.cpp\
    ./src/tcpconnection.cpp\
    ./src/tcpserver.cpp\
    ./src/uuid.cpp\
    ./src/main.cpp \
    src/stacktrace.cpp

HEADERS += \
    include/rapid/core/acceptor.h\
    include/rapid/core/unique_handle.h\
    include/rapid/core/buffer.h\
    include/rapid/core/connector.h\
    include/rapid/core/channel.h\
    include/rapid/core/epollpoller.h\
    include/rapid/core/eventloop.h\
    include/rapid/core/eventlooppool.h\
    include/rapid/core/exception.h\
    include/rapid/core/inetaddress.h\
    include/rapid/core/logger.h\
    include/rapid/core/tcpclient.h\
    include/rapid/core/tcpconnection.h\
    include/rapid/core/tcpserver.h\
    include/rapid/core/uuid.h\
    include/rapid/core/file_desc.h \
    include/rapid/core/stacktrace.h

QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_LFLAGS += -g -rdynamic
