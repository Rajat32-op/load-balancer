#include "network/Connection.hpp"

#include "utils/Loggers.hpp"

#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int clientFd, int backendFd)
    : clientFd_(clientFd),
      backendFd_(backendFd),
      backend_(nullptr),
      lastActivity_(std::chrono::steady_clock::now())
{
}

int Connection::getBackendFd(){
    return backendFd_;
}

int Connection::getClientFd(){
    return clientFd_;
}

void Connection::close()
{
    if (clientFd_ != -1)
    {
        ::close(clientFd_);
        clientFd_ = -1;
    }

    if (backendFd_ != -1)
    {
        ::close(backendFd_);
        backendFd_ = -1;
    }
}

Backend *Connection::getBackend(){
    return backend_;
}

void Connection::setBackend(Backend *backend){
    backend_ = backend;
}

std::chrono::steady_clock::time_point Connection::lastActivity() const {
    return lastActivity_;
}

void Connection::updateActivity() {
    lastActivity_ = std::chrono::steady_clock::now();
}