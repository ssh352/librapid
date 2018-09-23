#include <rapid/core/eventlooppool.h>

namespace rapid { namespace core {

EventLoopPool::EventLoopPool(uint32_t numThread) {
    loops_.reserve(numThread);
    for (uint32_t i = 0; i < numThread; ++i) {
        loops_.push_back(std::make_unique<EventLoop>());
    }
}

void EventLoopPool::start() {
    for (auto &loop : loops_) {
        loop->start();
    }
}

void EventLoopPool::stop() {
    for (auto &loop : loops_) {
        loop->stop();
    }

    for (auto &loop : loops_) {
        loop->waitForEnd();
    }
    loops_.clear();
}

} }
