//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_CHANNEL_H
#define RAPID_CORE_CHANNEL_H

#include <functional>
#include <cassert>

#include <sys/epoll.h>

#include <rapid/core/file_desc.h>
#include <rapid/core/logger.h>

namespace rapid { namespace core {

class EventLoop;

enum ChannelEvents : uint32_t {
    NONE = 0,
    WRITE = EPOLLOUT,
    READ = EPOLLIN | EPOLLPRI
};

enum ChannelStates : uint8_t {
    NEW,
    ADD,
    DEL,
};

using ChannelHandler = std::function<void()>;

class Channel {
public:
    Channel(EventLoop* loop, FileDesc &&fd);

    Channel(const Channel &) = delete;
    Channel& operator=(const Channel &) = delete;

    void enableWrite() {
        channelEvent_ |= WRITE;
        update();
    }

    void disableWrite() {
        channelEvent_ &= ~WRITE;
        update();
    }

    void enableRead() {
        channelEvent_ |= READ;
        update();
    }

    void disableRead() {
        channelEvent_ &= ~READ;
        update();
    }

    void disableAll() {
        channelEvent_ = NONE;
        update();
    }

    int releaseFd() {
        return fd_.release();
    }

    uint8_t state() const {
        return static_cast<uint8_t>(state_);
    }

    void setState(ChannelStates s) {
        state_ = s;
    }

    uint32_t channelEvent() const {
        return channelEvent_;
    }

    const FileDesc & fd() const {
        return fd_;
    }

    void send(const void* message, size_t len);

    template <typename T>
    void setWriteHandler(T handler) {
        writeHandler_ = std::forward<ChannelHandler>(handler);
    }

    template <typename T>
    void setReadHandler(T handler) {
        readHandler_ = std::forward<ChannelHandler>(handler);
    }

    template <typename T>
    void setCloseHandler(T handler) {
        closeHandler_ = std::forward<ChannelHandler>(handler);
    }

    void handleEvent() {
        if ((events_ & EPOLLHUP) && !(events_ & EPOLLIN)) {
            if (closeHandler_ != nullptr) closeHandler_();
        } else if (events_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
            if (readHandler_ != nullptr) readHandler_();
        } else if (events_ & EPOLLOUT) {
            if (writeHandler_ != nullptr) writeHandler_();
        } else {
            logger_->critical("Invalid event type" && 0);
        }
    }

    void setEvents(uint32_t events);

    void update();

    void remove();

    void weakup();

private:
    uint8_t state_;
    FileDesc fd_;
    uint32_t channelEvent_;
    uint32_t events_;   
    EventLoop* loop_;
    ChannelHandler writeHandler_;
    ChannelHandler readHandler_;
    ChannelHandler closeHandler_;
    std::shared_ptr<spdlog::logger> logger_;
};

} }

#endif // RAPID_CORE_CHANNEL_H
