#include "lb/LoadBalancer.hpp"
#include "utils/Loggers.hpp"
#include "lb/Backend.hpp"
#include "lb/RoundRobinScheduler.hpp"
#include "lb/LeastConnectionScheduler.hpp"
#include "lb/WeightedRRScheduler.hpp"

LoadBalancer::LoadBalancer(const Config& config)
    : config_(config),
      server_(config.listenIp, config.listenPort),metricsServer_(metrics_)
{
}

LoadBalancer::~LoadBalancer()
{
    healthChecker_->stop();
    metricsServer_.stop();
}

bool LoadBalancer::initialize()
{
    if (!server_.start())
    {
        Logger::error("Failed to start server.");
        return false;
    }

    if (!eventLoop_.addFD(server_.getListenFd(), EPOLLIN))
    {
        Logger::error("Failed to register listening socket with epoll.");
        return false;
    }

    for(auto& backendConfig : config_.backends)
    {
        backends_.emplace_back(std::make_unique<Backend>(backendConfig.host, backendConfig.port, backendConfig.weight));
    }

    if(config_.algorithm == "round_robin")
        scheduler_ = std::make_unique<RoundRobinScheduler>(backends_);
    else if(config_.algorithm == "least_connection")
        scheduler_ = std::make_unique<LeastConnectionScheduler>(backends_);
    else if(config_.algorithm == "weighted_round_robin")
        scheduler_ = std::make_unique<WeightedRoundRobinScheduler>(backends_);
    else
    {
        Logger::error("Unknown scheduling algorithm: " + config_.algorithm);
        return false;
    }

    healthChecker_ = std::make_unique<HealthChecker>(backends_,metrics_);
    healthChecker_->start();
    metricsServer_.start();

    return true;
}


void LoadBalancer::acceptNewClient()
{
    int clientFd = server_.acceptClient();
    if (clientFd == -1)
    {
        Logger::error("Failed to accept new client.");
        return;
    }

    Backend* backend = scheduler_->selectBackend();
    int backendFd = backend->connectBackend();

    if (backendFd == -1)
    {
        Logger::error("Failed to connect to backend server.");
        close(clientFd);
        return;
    }

    backend->incrementConnections();
    auto connection = std::make_shared<Connection>(clientFd, backendFd);
    connections_[clientFd] = connection;
    connections_[backendFd] = connection;
    connections_[clientFd]->setBackend(backend);

    metrics_.incrementActiveConnections();
    metrics_.incrementTotalConnections();

    // Register both client and backend FDs with the event loop.
    eventLoop_.addFD(clientFd, EPOLLIN);
    eventLoop_.addFD(backendFd, EPOLLIN);
}

void LoadBalancer::handleRead(int fd)
{
    auto it = connections_.find(fd);

    if (it == connections_.end())
        return;

    auto conn = it->second;

    int fromFd;
    int toFd;

    std::vector<char>* buffer;

    if (fd == conn->getClientFd())
    {
        fromFd = conn->getClientFd();
        toFd   = conn->getBackendFd();

        buffer = &conn->clientToBackendBuffer();
    }
    else
    {
        fromFd = conn->getBackendFd();
        toFd   = conn->getClientFd();

        buffer = &conn->backendToClientBuffer();
    }

    char temp[4096];

    ssize_t n =
        recv(fromFd,
             temp,
             sizeof(temp),
             0);

    if (n == 0)
    {
        cleanupConnection(fd);
        return;
    }

    if (n < 0)
    {
        if(errno != EAGAIN &&
           errno != EWOULDBLOCK)
        {
            cleanupConnection(fd);
        }

        return;
    }

    conn->updateActivity();

    metrics_.addBytesIn(n);
    metrics_.incrementRequests();

    buffer->insert(
        buffer->end(),
        temp,
        temp+n);

    handleWrite(toFd);
}

void LoadBalancer::handleWrite(int fd)
{
    auto it = connections_.find(fd);

    if(it == connections_.end())
        return;

    auto conn = it->second;

    std::vector<char>* buffer;

    if(fd == conn->getBackendFd())
    {
        buffer = &conn->clientToBackendBuffer();
    }
    else
    {
        buffer = &conn->backendToClientBuffer();
    }

    if(buffer->empty())
    {
        eventLoop_.modifyFD(fd,
                            EPOLLIN);

        return;
    }

    ssize_t sent =
        send(fd,
             buffer->data(),
             buffer->size(),
             0);

    if(sent > 0)
    {
        metrics_.addBytesOut(sent);

        buffer->erase(
            buffer->begin(),
            buffer->begin()+sent);
    }
    else if(sent<0){
         if (errno == EAGAIN || errno == EWOULDBLOCK){
            eventLoop_.modifyFD(fd, EPOLLIN | EPOLLOUT);
            return;
        }

        cleanupConnection(fd);
        return;
    }

    if(buffer->empty())
    {
        eventLoop_.modifyFD(fd,
                            EPOLLIN);
    }
    else
    {
        eventLoop_.modifyFD(
            fd,
            EPOLLIN | EPOLLOUT);
    }
}


void LoadBalancer::handleSocketEvent(const epoll_event& event)
{
    int fd = event.data.fd;

    if (event.events & EPOLLIN)
        handleRead(fd);

    if (event.events & EPOLLOUT)
        handleWrite(fd);
}

void LoadBalancer::cleanupConnection(int fd)
{
    auto it = connections_.find(fd);
    if (it != connections_.end())
    {
        auto connection = it->second;
        int clientFd = connection->getClientFd();
        int backendFd = connection->getBackendFd();
        connection->getBackend()->decrementConnections();
        
        eventLoop_.removeFD(clientFd);
        eventLoop_.removeFD(backendFd);

        connection->close();

        connections_.erase(clientFd);
        connections_.erase(backendFd);

        metrics_.decrementActiveConnections();
    }
}

void LoadBalancer::run(){

    auto lastCleanup = std::chrono::steady_clock::now();

    while (true)
    {
        int numEvents = eventLoop_.wait(events_,1000);
        if(numEvents < 0)
        {
            continue;
        }
        for (auto& event : events_)
        {
            if (event.data.fd == server_.getListenFd())
                acceptNewClient();
            else{
                handleSocketEvent(event);
            }
        }
        auto now = std::chrono::steady_clock::now();

        if (now - lastCleanup >= std::chrono::seconds(1))
        {
            checkIdleConnections();
            lastCleanup = now;
        }
    }
}

void LoadBalancer::checkIdleConnections() {
    auto now = std::chrono::steady_clock::now();
    std::vector<int> fdsToCleanup;

    for (const auto& pair : connections_) {
        auto connection = pair.second;
        if (now - connection->lastActivity() > IDLE_TIMEOUT) {
            fdsToCleanup.push_back(connection->getClientFd());
            fdsToCleanup.push_back(connection->getBackendFd());
        }
    }

    for (int fd : fdsToCleanup) {
        cleanupConnection(fd);
    }
}