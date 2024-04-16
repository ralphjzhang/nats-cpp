// SPDX-License-Identifier: MIT
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <thread>

namespace nats {

struct TestConn;

std::shared_ptr<TestConn> run(std::thread &worker, std::string const &port);

using callback = std::function<void(std::string subject, std::string reply_to,
                                    std::string data)>;

void subscribe(std::shared_ptr<TestConn> conn, std::string const &subj,
               callback callback);
} // namespace nats
