#include <chrono>
#include <iostream>
#include <span>
#include <thread>

#include "../src/jetstream.hpp"
#include "../src/nats.hpp"

int main(int argc, char const *argv[]) {
  using namespace std::chrono_literals;
  auto conn = nats::TcpConnection{};
  auto thread = nats::run(conn, "4222");
  std::this_thread::sleep_for(100ms);

  conn.subscribe("subj", "", [](auto subj, auto reply, auto data) {
    std::cout << "subject: " << subj << "  reply: " << reply << "\n"
              << "  data: " << data.data() << std::endl;
  });

  thread.join();
}
