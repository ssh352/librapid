#include <unistd.h>
#include <algorithm>

#include <rapid/core/exception.h>
#include <rapid/core/channel.h>
#include <rapid/core/utility.h>
#include <rapid/core/epollpoller.h>

namespace rapid { namespace core {

EPollPoller::EPollPoller()
    : epollfd_(epoll_create(EPOLL_CLOEXEC))
    , events_(EPOLL_EVENT_INIT_SIZE)
    , logger_(log::Logger::get().getLogger("EPollPoller")) {
}

void EPollPoller::updateChannel(Channel *channel) {
    if (channel->state() == ChannelStates::NEW || channel->state() == ChannelStates::DEL) {
        update(EPOLL_CTL_ADD, channel);
        channel->setState(ChannelStates::ADD);
    } else {
        if (channel->state() == ChannelStates::DEL) {
            update(EPOLL_CTL_DEL, channel);
            channel->setState(ChannelStates::DEL);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel *channel) {
    update(EPOLL_CTL_DEL, channel);
}

int EPollPoller::poll(std::vector<Channel *> &activeChannels, int epollMillis) {
    activeChannels.clear();

    const auto numEvents = epoll_wait(epollfd_.get(),
                                      events_.data(),
                                      static_cast<int>(events_.size()),
                                      epollMillis);

    if (numEvents > 0) {
        std::for_each(events_.begin(), events_.begin() + numEvents, [&activeChannels](auto event) {
            auto channel = reinterpret_cast<Channel*>(event.data.ptr);
            if (channel != nullptr) {
                channel->setEvents(event.events);
                activeChannels.push_back(channel);
            }
        });

        if (numEvents == static_cast<int>(events_.size())) {
            events_.resize(events_.size() * 2);
            logger_->debug("resize event size:{}", events_.size());
        }
    }

    return numEvents;
}

void EPollPoller::update(int opt, Channel* channel) {
    logger_->trace("update poll-event opt:{}", opt);

    epoll_event event{};
    event.events = channel->channelEvent();
    event.data.ptr = channel;
    auto res = epoll_ctl(epollfd_.get(), opt, channel->fd().get(), &event);
    ensureOrThrow(res == 0, "epoll_ctl return fail!");
}

} }
