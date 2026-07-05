#include "core/EventLoop.hpp"
#include <fcntl.h>
#include<unistd.h>
#include<stdexcept>

EventLoop::EventLoop()
{
    epollFd_ = epoll_create1(0);

    if (epollFd_ == -1)
        throw std::runtime_error("epoll_create1 failed");
}

EventLoop::~EventLoop()
{
    close(epollFd_);
}

bool EventLoop::addFD(int fd,
                      uint32_t events)
{
    epoll_event event{};

    event.events = events;
    event.data.fd = fd;

    return epoll_ctl(
            epollFd_,
            EPOLL_CTL_ADD,
            fd,
            &event) != -1;
}

bool EventLoop::removeFD(int fd)
{
    return epoll_ctl(
            epollFd_,
            EPOLL_CTL_DEL,
            fd,
            nullptr) != -1;
}

int EventLoop::wait(
        std::vector<epoll_event>& events,
        int timeout)
{
    events.resize(64);

    int n = epoll_wait(
                epollFd_,
                events.data(),
                events.size(),
                timeout);

    if (n > 0)
        events.resize(n);

    return n;
}