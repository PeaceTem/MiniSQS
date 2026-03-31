# MiniSQS

MiniSQS is a **lightweight, persistent, event-sourced distributed job queue** built in C++.  
It supports:

- TCP-based job submission (planned)
- Worker pull model
- Persistent disk-backed queue (Write-Ahead Log)
- Retry mechanism
- Dead Letter Queue (DLQ)
- Job timeouts
- Thread-safe in-memory job management

---

## Table of Contents

1. [Features](#features)
2. [Architecture](#architecture)
3. [Folder Structure](#folder-structure)
4. [Building](#building)
5. [Running](#running)
6. [Usage Example](#usage-example)
7. [Contributing](#contributing)
8. [License](#license)

---

## Features

- Event-sourced design: all actions are recorded in a WAL for crash recovery.
- Dead Letter Queue: failed jobs can be moved for inspection.
- Retry mechanism: automatically retries failed jobs up to `max_retries`.
- Visibility timeouts: jobs are reassigned if workers fail to acknowledge.
- Thread-safe queues using STL containers.

---

## Architecture

MiniSQS has three main components:

1. **Broker** – manages job queues, in-progress jobs, DLQ, and WAL persistence.
2. **WAL (Write-Ahead Log)** – ensures durability of all job events.
3. **Workers** – pull jobs from the broker, process them, and acknowledge completion.

Event flow:
```
enqueue() → WAL append → applyEvent()
assign() → WAL append → applyEvent()
ack() → WAL append → applyEvent()
retry() → WAL append → applyEvent()
moveToDLQ() → WAL append → applyEvent()
```

---

## Folder Structure
MiniSQS/
├── src/ # C++ source code
│ ├── broker/
│ ├── storage/
│ └── main.cpp
├── data/ # WAL and logs (auto-created)
├── build/ # build folder (ignored in Git)
├── CMakeLists.txt
├── README.md
└── LICENSE


---

## Building

**Requirements:**

- C++17 or higher
- CMake
- Ninja or Make
- MSYS2 (Windows) / Linux / macOS

**Build steps:**

```bash
mkdir build
cd build
cmake -G Ninja ..
ninja
```

This will generate the MiniSQS.exe executable in the build/ folder.

### Running

```bash
./MiniSQS.exe
```
The program automatically creates the data/wal.log file if it doesn't exist.