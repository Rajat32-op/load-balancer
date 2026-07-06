#include "config/Config.hpp"
#include "lb/LoadBalancer.hpp"

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>

int main()
{
    Config config;

    if (!config.load("config.json"))
        return 1;

    LoadBalancer lb(config);

    if (!lb.initialize())
        return 1;

    lb.run();
}