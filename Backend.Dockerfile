FROM gcc:14

WORKDIR /app

COPY tests/EchoServer.cpp .

RUN g++ EchoServer.cpp -pthread -o echo_server

CMD ["./echo_server","9001"]