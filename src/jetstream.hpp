// SPDX-License-Identifier: MIT
#pragma once

#include "nats.hpp"

namespace jetstream {

struct Consumer {
  auto consume(auto handler);
};

struct Stream {
  auto consumer(std::string_view);
};

struct Context {
  auto stream(std::string_view);
};

template <typename Connection> //
Context create(Connection &c);

} // namespace jetstream
