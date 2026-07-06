#pragma once

#include <vector>

class Backend;

class Scheduler
{
public:
    virtual ~Scheduler() = default;

    virtual Backend* selectBackend() = 0;
};