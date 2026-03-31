#include "scheduler.h"
#include <chrono>

Scheduler::Scheduler(Broker& broker)
    : broker_(broker) {}

void Scheduler::start() {
    running_ = true;

    worker_ = std::thread([this]() {
        while (running_) {
            broker_.checkTimeouts();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void Scheduler::stop() {
    running_ = false;
    if (worker_.joinable())
        worker_.join();
}