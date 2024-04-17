// SPDX-License-Identifier: MIT
#pragma once
#include <stdexcept>

#include <expected.hpp>

namespace nats {

#if __cplusplus > 202002L && __cpp_concepts >= 202002L
#include <expected>
template <typename T, typename E> //
using expected = std::expected<T, E>;
template <typename E> //
using unexpected = std::unexpected<E>;
#else
template <typename T, typename E> //
using expected = tl::expected<T, E>;
template <typename E> //
using unexpected = tl::unexpected<E>;
#endif

template <typename T> using own = T *;
template <typename T> using spy = T *;

using any = void;
using byte = char;

template <typename T> struct chan {};

namespace io {
struct Reader {};
struct Writer {};
} // namespace io

namespace bytes {

struct Buffer {};

} // namespace bytes

namespace tls {

struct Config {};
struct Certificate {};

} // namespace tls

namespace x509 {

struct CertPool {};

} // namespace x509

namespace net {

struct Conn {};

struct Dialer {};

} // namespace net

namespace sync {

struct RWMutex {};
struct WaitGroup {};

} // namespace sync

namespace time {

struct Timer {};

} // namespace time

namespace rand {

struct Rand {};

} // namespace rand

struct [[nodiscard]] Error : std::runtime_error {
  using base = std::runtime_error;
  Error() : base{""} {}
  Error(char const *what) : base{what} {}
  Error(std::string const &what) : base{what} {}

  bool has_error() { return *what() != '\0'; }
};

} // namespace nats
