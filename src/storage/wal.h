#pragma once
#include <string>
#include <vector>
#include <optional>
// #include <fstream>
#include "../common/job.h"

enum class WALEventType {
    ENQUEUE,
    ASSIGN,
    ACK,
    RETRY,
    MOVE_TO_DLQ
};

struct WALEvent {
    WALEventType type;
    std::string job_id;
    std::string payload;  // only for ENQUEUE
    int retry_count = 0;  // for RETRY
};

class WAL {
public:
    explicit WAL(const std::string& file_path);

    void append(const WALEvent& event);
    std::vector<WALEvent> replay();

private:
    std::string file_path_;
    // std::ofstream file_;

    std::string serialize(const WALEvent& event);
    std::optional<WALEvent> deserialize(const std::string& line);
};