#include <iostream>

#include "../src/nats.hpp"
#include "nats.hpp"

namespace nats {

struct TestConn {
  TcpConnection conn;
};

std::shared_ptr<TestConn> run(std::thread &worker, std::string const &port) {
  auto conn = std::make_shared<TestConn>(TcpConnection{});
  std::thread([&conn, port]() { conn->conn.run(port); }).swap(worker);
  return conn;
}

void subscribe(std::shared_ptr<TestConn> conn, std::string const &subj,
               callback callback) {
  conn->conn.subscribe(
      subj, "",
      [callback](std::string_view subject, std::string_view reply_to,
                 std::string &&data) {
        std::cout << "callback............." << std::endl;
        callback(subject.data(), reply_to.data(), data);
        std::cout << ".......callback............." << std::endl;
      });
}

}; // namespace nats
