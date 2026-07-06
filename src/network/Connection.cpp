#include "network/Connection.hpp"

#include "utils/Loggers.hpp"

#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int clientFd, int backendFd)
    : clientFd_(clientFd),
      backendFd_(backendFd)
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