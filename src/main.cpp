#include "broker/broker.h"
#include <iostream>

int main() {
    Broker broker("data/wal.log");

    broker.recover();

    broker.enqueue("job1", "process payment");
    broker.enqueue("job2", "send email");

    auto job = broker.assign();
    if (job.has_value()) {
        std::cout << "Assigned: " << job->id << std::endl;
        broker.ack(job->id);
    }

    return 0;
}