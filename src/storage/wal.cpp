#include "wal.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include <filesystem>

WAL::WAL(const std::string& file_path) : file_path_(file_path) {

    std::filesystem::path p(file_path_);

    // create directory if it doesn't exist
    std::filesystem::create_directories(p.parent_path());

    // // open WAL file
    // file_.open(file_path_, std::ios::app);

    // if (!file_.is_open()) {
    //     throw std::runtime_error("Failed to open WAL file: " + file_path_);
    // }
}

void WAL::append(const WALEvent& event) {
    std::ofstream file(file_path_, std::ios::app);
    if (!file) {
        std::cerr << "Failed to open WAL file\n";
        return;
    }

    file << serialize(event) << "\n";
}

std::vector<WALEvent> WAL::replay() {
    std::vector<WALEvent> events;
    std::ifstream file(file_path_);
    if (!file) return events;

    std::string line;
    while (std::getline(file, line)) {
        auto event = deserialize(line);
        if (event.has_value())
            events.push_back(event.value());
    }

    return events;
}

std::string WAL::serialize(const WALEvent& event) {
    std::ostringstream oss;

    oss << static_cast<int>(event.type)
        << "|" << event.job_id
        << "|" << event.payload
        << "|" << event.retry_count;

    return oss.str();
}

std::optional<WALEvent> WAL::deserialize(const std::string& line) {
    std::istringstream iss(line);
    std::string token;

    WALEvent event;

    std::getline(iss, token, '|');
    event.type = static_cast<WALEventType>(std::stoi(token));

    std::getline(iss, event.job_id, '|');
    std::getline(iss, event.payload, '|');

    std::getline(iss, token, '|');
    event.retry_count = std::stoi(token);

    return event;
}