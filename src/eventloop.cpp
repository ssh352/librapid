#include <sys/eventfd.h>
#include <unistd.h>

#include <rapid/core/utility.h>
#include <rapid/core/exception.h>
#include <rapid/core/channel.h>
#include <rapid/core/epollpoller.h>
#include <rapid/core/file_desc.h>
#include <rapid/core/eventloop.h>

namespace rapid { namespace core {

EventLoop::EventLoop()
    : quit_(true)
    , wakeupChannel_(this, FileDesc(eventfd(0, EFD_NONBLOCK | FD_CLOEXEC)))
    , logger_(log::Logger::get().getLogger("EventLoop")) {
    wakeupChannel_.enableRead();
}

void EventLoop::start() {
    quit_ = false;

    thread_ = std::thread([this]() {
        logger_->debug("thread:{} start!", std::this_thread::get_id());

        while (!quit_) {
            auto numEventPoolled = poller_.poll(activeChannels_, INT32_MAX);

            if (numEventPoolled == -1) {
                if (errno == EINTR) {
                    continue;
                }
                logger_->critical("poll return fail! {}", getSystemErrorString());
                break;
            }

            logger_->trace("polled event:{}", numEventPoolled);

            for (auto channel : activeChannels_) {
                try {
                    channel->handleEvent();
                } catch (const AssertException &e) {
                    logger_->critical("Uncaught exception! {}\n.file:{}:{} stacktrace:{}",
                                     e.what(),
                                     e.throwedFileName(),
                                     e.throwFileline(),
                                     e.trace());
                } catch (const std::exception &e) {
                    logger_->critical("Uncaught exception:{}", e.what());
                }
            }
        }

        logger_->debug("thread:{} end!", std::this_thread::get_id());
    });
}

void EventLoop::stop() {    
    quit_ = true;
    wakeup();    
}

void EventLoop::waitForEnd() {
    if (thread_.joinable()) thread_.join();
}

void EventLoop::updateChannel(Channel *channel) {
    poller_.updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    poller_.removeChannel(channel);
}

void EventLoop::wakeup() {
    wakeupChannel_.weakup();
}

} }
