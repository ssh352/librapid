//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_EVENTLOOP_H
#define RAPID_CORE_EVENTLOOP_H

#include <vector>
#include <thread>
#include <atomic>

#include <rapid/core/channel.h>
#include <rapid/core/epollpoller.h>
#include <rapid/core/logger.h>

namespace rapid { namespace core {

class EventLoop {
public:
    EventLoop();

    EventLoop(const EventLoop &) = delete;
    EventLoop& operator=(const EventLoop &) = delete;

    void start();

    void stop();

    void waitForEnd();

    void updateChannel(Channel *channel);

    void removeChannel(Channel *channel);

    void wakeup();

private:
    std::atomic<bool> quit_;
    std::thread thread_;
    EPollPoller poller_;
    Channel wakeupChannel_;
    std::vector<Channel*> activeChannels_;
    std::shared_ptr<spdlog::logger> logger_;
};

} }

#endif // RAPID_CORE_EVENTLOOP_H
