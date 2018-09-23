#include <rapid/core/eventloop.h>
#include <rapid/core/exception.h>
#include <rapid/core/channel.h>

namespace rapid { namespace core {

Channel::Channel(EventLoop* loop, FileDesc &&fd)
    : state_(NEW)
    , fd_(std::move(fd))
    , channelEvent_(NONE)
    , events_(0)
    , loop_(loop)
    , logger_(log::Logger::get().getLogger("Channel")) {
}

void Channel::setEvents(uint32_t events) {
    events_ = events;
}

void Channel::update() {
    loop_->updateChannel(this);
}

void Channel::remove() {
    loop_->removeChannel(this);
}

void Channel::send(const void* message, size_t len) {
    (void) write(fd_.get(), message, len);
}

void Channel::weakup() {
    uint64_t one = 1;
    auto ret = write(fd_.get(), &one, sizeof(one));
    ensureOrThrow(ret >= 0, "wakeup fail!");
}

} }
