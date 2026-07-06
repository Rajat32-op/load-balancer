#pragma once

#include <vector>
#include <sys/epoll.h>
#include <functional>
using EventCallback = std::function<void(int)>;

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    bool addFD(int fd, uint32_t events);

    bool removeFD(int fd);

    int wait(std::vector<epoll_event>& events,
             int timeout = -1);

    void run();

private:
    int epollFd_;
    std::unordered_map<int, EventCallback> callbacks_;
};