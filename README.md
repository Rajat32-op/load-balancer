
# Production-Inspired Layer 4 TCP Load Balancer

A production-inspired Layer 4 (TCP) Load Balancer implemented in modern C++ using Linux networking primitives. The project demonstrates event-driven server design using `epoll`, multiple load balancing algorithms, backend health monitoring, runtime metrics, and containerized deployment.

## Features

* Event-driven architecture using `epoll`
* Non-blocking POSIX sockets
* TCP proxy supporting long-lived connections
* Load balancing algorithms

  * Round Robin
  * Least Connections
  * Weighted Round Robin
* Active backend health checking
* Automatic backend recovery
* Idle connection timeout
* Metrics endpoint (`/metrics`)
* Docker Compose deployment
* Partial I/O handling with support for non-blocking reads/writes

---

## Project Structure

```
.
├── include/
├── src/
├── tests/
├── configs/
├── docker/
├── Dockerfile
├── docker-compose.yml
└── CMakeLists.txt
```

---

## Architecture

```
                    Client
                       │
                  TcpServer
                       │
                 EventLoop (epoll)
                       │
        ┌──────────────┴──────────────┐
        │                             │
   Connection Manager          Health Checker
        │
        ▼
     Scheduler
 (RR / LeastConn / WRR)
        │
        ▼
 Backend Connections
        │
        ▼
 Backend Servers

 Metrics Server
```

---

## Build

### Requirements

* Linux
* C++17 or newer
* CMake
* Docker (optional)

### Build

```bash
cmake -S . -B build
cmake --build build
```

---

## Run

### Local

Start three backend servers:

```bash
./echo_server 9001
./echo_server 9002
./echo_server 9003
```

Run the load balancer:

```bash
./build/load_balancer
```

Connect using:

```bash
nc localhost 8080
```

---

### Docker

Build and start everything:

```bash
docker compose up --build
```

This launches:

* Load Balancer
* Backend 1
* Backend 2
* Backend 3

---

## Metrics

Metrics are exposed on:

```
http://localhost:9090/metrics
```

Example response:

```json
{
  "active_connections": 5,
  "total_connections": 142,
  "requests": 928,
  "bytes_in": 124583,
  "bytes_out": 124583,
  "healthy_backends": 3
}
```

---

## Configuration

The load balancer is configured using `config.json`.

Example:

```json
{
    "listen_ip": "0.0.0.0",
    "listen_port": 8080,

    "algorithm": "least_connection",

    "backends": [
        {
            "host": "127.0.0.1",
            "port": 9001,
            "weight": 1
        },
        {
            "host": "127.0.0.1",
            "port": 9002,
            "weight": 2
        },
        {
            "host": "127.0.0.1",
            "port": 9003,
            "weight": 3
        }
    ]
}
```

For Docker, use service names (`backend1`, `backend2`, `backend3`) instead of `127.0.0.1`.

---

## Technologies Used

* C++17
* Linux
* POSIX Sockets
* epoll
* CMake
* Docker Compose
* nlohmann/json

---

## Future Improvements

* Runtime configuration reload (`SIGHUP`)
* Structured logging
* Benchmarking
* Graceful backend draining
* Prometheus-compatible metrics
* Unit tests
