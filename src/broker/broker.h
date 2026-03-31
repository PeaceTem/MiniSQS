#pragma once
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <chrono>
#include "../storage/wal.h"
#include "../common/job.h"

class Broker {
public:
    explicit Broker(const std::string& wal_path);

    void recover();   // rebuild state from WAL

    void enqueue(const std::string& job_id, const std::string& payload);
    std::optional<Job> assign();
    void ack(const std::string& job_id);
    void retry(const std::string& job_id);
    void moveToDLQ(const std::string& job_id);
    std::unordered_map<std::string,
        std::chrono::steady_clock::time_point> visibility_deadlines_;

    int visibility_timeout_seconds_ = 10;
    int max_retries_ = 3;

    void checkTimeouts();
    
private:
    std::unique_ptr<WAL> wal_;

    std::queue<std::string> pending_;
    std::unordered_map<std::string, Job> in_progress_;
    std::unordered_map<std::string, Job> all_jobs_;
    std::unordered_set<std::string> dead_letter_;

    void applyEvent(const WALEvent& event);
};