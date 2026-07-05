#pragma once

#include <vector>
#include <sys/epoll.h>

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    bool addFD(int fd, uint32_t events);

    bool removeFD(int fd);

    int wait(std::vector<epoll_event>& events,
             int timeout = -1);

private:
    int epollFd_;
};