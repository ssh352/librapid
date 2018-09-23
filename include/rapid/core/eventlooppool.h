//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_EVENTLOOPPOOL_H
#define RAPID_CORE_EVENTLOOPPOOL_H

#include <thread>
#include <vector>
#include <memory>
#include <atomic>

#include <rapid/core/eventloop.h>

namespace rapid { namespace core {

class EventLoopPool {
public:
    explicit EventLoopPool(uint32_t numThread = std::thread::hardware_concurrency());

    EventLoopPool(const EventLoopPool &) = delete;
    EventLoopPool& operator=(const EventLoopPool &) = delete;

    void start();

    void stop();

    EventLoop* nextLoop();

    size_t size() const {
        return loops_.size();
    }

private:
    std::atomic<uint32_t> count_;
    std::vector<std::unique_ptr<EventLoop>> loops_;
};

inline EventLoop* EventLoopPool::nextLoop() {
    return loops_[count_++ % loops_.size()].get();
}

} }

#endif // RAPID_CORE_EVENTLOOPPOOL_H
