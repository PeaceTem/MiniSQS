#pragma once
#include <string>
#include <chrono>

enum class JobStatus {
    PENDING,
    IN_PROGRESS,
    COMPLETED,
    FAILED,
    DEAD_LETTER
};

struct Job {
    std::string id;
    std::string payload;
    int retry_count = 0;
    JobStatus status = JobStatus::PENDING;
    std::chrono::system_clock::time_point created_at;

    Job() = default;

    Job(const std::string& job_id, const std::string& data)
        : id(job_id), payload(data),
          created_at(std::chrono::system_clock::now()) {}
};