#include <unistd.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include <boost/ut.hpp>

#include "../src/nats.hpp"

namespace ut = boost::ut;
using namespace ut;
using namespace ut::spec;

using namespace nats;
using namespace std::literals;

using TestConnection = nats::Connection<std::ostringstream>;

int main() {
  auto server_pid = fork();
  if (server_pid == 0) {
    // child
    std::system("./st/nats-server");
    return 0;
  }

  std::cerr << "parent" << std::endl;

  "[Connection] tests"_test = [] {
    TcpConnection conn;
    ConnectConfig cfg{
        .addr = "localhost",
        .port = "4222",
        .verbose = true,
        .pedantic = true,
        .user = "user",
        .password = "pwd",
        .auth_token = "token",
    };

    describe("connect") = [&] {
      it("connects to server and get INFO") = [&] {
        TcpConnection conn;
        bool on_info_called{false};
        conn.on_info = [&on_info_called](std::string_view info) -> Error {
          on_info_called = true;
          expect(info.starts_with("INFO"));
          return {};
        };
        auto client_thread = nats::run(conn);
        std::this_thread::sleep_for(1s);

        expect(on_info_called);
        // conn.publish("subj1", "reply-to-me", "data");

        client_thread.join();
      };
    };
  };

  auto cmd = "kill " + std::to_string(server_pid);
  std::system(cmd.c_str());

  return 0;
}
