#pragma once
#include "broker.h"
#include <thread>
#include <atomic>

class Scheduler {
public:
    explicit Scheduler(Broker& broker);
    void start();
    void stop();

private:
    Broker& broker_;
    std::thread worker_;
    std::atomic<bool> running_{false};
};