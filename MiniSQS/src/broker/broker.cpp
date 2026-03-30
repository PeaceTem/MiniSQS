#include "broker.h"
#include <iostream>

Broker::Broker(const std::string& wal_path) {
    wal_ = std::make_unique<WAL>(wal_path);
}

void Broker::recover() {
    auto events = wal_->replay();

    for (const auto& event : events) {
        applyEvent(event);
    }

    for (auto& [job_id, job] : all_jobs_) {
        if (job.status == JobStatus::IN_PROGRESS) {

            if (job.retry_count >= max_retries_) {
                moveToDLQ(job_id);
            } else {
                retry(job_id);
            }
        }
    }
    
    std::cout << "Recovery complete. "
              << "Pending: " << pending_.size()
              << ", In-progress: " << in_progress_.size()
              << ", DLQ: " << dead_letter_.size()
              << std::endl;
}

void Broker::applyEvent(const WALEvent& event) {
    switch (event.type) {

        case WALEventType::ENQUEUE: {
            Job job(event.job_id, event.payload);
            job.status = JobStatus::PENDING;

            all_jobs_[job.id] = job;
            pending_.push(job.id);
            break;
        }

        case WALEventType::ASSIGN: {
            auto& job = all_jobs_[event.job_id];
            job.status = JobStatus::IN_PROGRESS;

            in_progress_[job.id] = job;

            visibility_deadlines_[job.id] =
            std::chrono::steady_clock::now() +
            std::chrono::seconds(visibility_timeout_seconds_);
            
            break;
        }

        case WALEventType::ACK: {
            in_progress_.erase(event.job_id);
            visibility_deadlines_.erase(event.job_id);
            all_jobs_[event.job_id].status = JobStatus::COMPLETED;
            break;
        }

        case WALEventType::RETRY: {
            auto& job = all_jobs_[event.job_id];
            job.retry_count = event.retry_count;
            job.status = JobStatus::PENDING;

            in_progress_.erase(event.job_id);
            pending_.push(job.id);
            break;
        }

        case WALEventType::MOVE_TO_DLQ: {
            in_progress_.erase(event.job_id);
            dead_letter_.insert(event.job_id);
            all_jobs_[event.job_id].status = JobStatus::DEAD_LETTER;
            break;
        }
    }
}

void Broker::enqueue(const std::string& job_id, const std::string& payload) {
    WALEvent event{
        WALEventType::ENQUEUE,
        job_id,
        payload,
        0
    };

    wal_->append(event);
    applyEvent(event);
}

std::optional<Job> Broker::assign() {
    if (pending_.empty())
        return std::nullopt;

    std::string job_id = pending_.front();
    pending_.pop();

    WALEvent event{
        WALEventType::ASSIGN,
        job_id,
        "",
        0
    };

    wal_->append(event);
    applyEvent(event);

    return all_jobs_[job_id];
}

void Broker::ack(const std::string& job_id) {
    WALEvent event{
        WALEventType::ACK,
        job_id,
        "",
        0
    };

    wal_->append(event);
    applyEvent(event);
}

void Broker::retry(const std::string& job_id) {
    auto& job = all_jobs_[job_id];
    job.retry_count++;

    WALEvent event{
        WALEventType::RETRY,
        job_id,
        "",
        job.retry_count
    };

    wal_->append(event);
    applyEvent(event);
}

void Broker::checkTimeouts() {
    auto now = std::chrono::steady_clock::now();

    for (auto it = visibility_deadlines_.begin();
         it != visibility_deadlines_.end();) {

        if (now >= it->second) {

            std::string job_id = it->first;
            auto& job = all_jobs_[job_id];

            if (job.retry_count >= max_retries_) {
                moveToDLQ(job_id);
            } else {
                retry(job_id);
            }

            it = visibility_deadlines_.erase(it);
        } else {
            ++it;
        }
    }
}