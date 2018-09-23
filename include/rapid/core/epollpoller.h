//=====================================================================================================================
// Copyright (c) 2018 librapid project. All rights reserved.
// More license information, please see LICENSE file in module root folder.
//=====================================================================================================================

#ifndef RAPID_CORE_EPOLLPOLLER_H
#define RAPID_CORE_EPOLLPOLLER_H

#include <vector>

#include <sys/epoll.h>
#include <sys/time.h>

#include <rapid/core/file_desc.h>
#include <rapid/core/logger.h>

namespace rapid { namespace core {

class Channel;

class EPollPoller {
public:
    static const int32_t EPOLL_EVENT_INIT_SIZE = 128;

    EPollPoller();

    EPollPoller(const EPollPoller &) = delete;
    EPollPoller& operator=(const EPollPoller &) = delete;

    void updateChannel(Channel *channel);

    void removeChannel(Channel *channel);

    int poll(std::vector<Channel *> &activeChannels, int epollMillis);

private:
    void update(int opt, Channel* channel);

    FileDesc epollfd_;
    std::vector<epoll_event> events_;
    std::shared_ptr<spdlog::logger> logger_;
};

} }

#endif // RAPID_CORE_EPOLLPOLLER_H
