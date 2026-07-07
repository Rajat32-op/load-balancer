FROM ubuntu:24.04 AS builder

RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        nlohmann-json3-dev

WORKDIR /app

COPY . .

RUN rm -rf build
RUN cmake -S . -B build
RUN cmake --build build -j

FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y libstdc++6

WORKDIR /app

COPY --from=builder /app/build/load_balancer .
COPY docker_config.json config.json

EXPOSE 8080
EXPOSE 9090

CMD ["./load_balancer"]