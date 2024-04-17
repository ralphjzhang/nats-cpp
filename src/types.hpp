// SPDX-License-Identifier: MIT
#pragma once
#include <any>
#include <array>
#include <bit>
#include <chrono>
#include <cstring>
#include <functional>
#include <mutex>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#if __cplusplus > 202002L
#include <expected>
#else
// #include <expected.hpp>
#endif

namespace nats {

#if __cplusplus > 202002L
using std::expected;
using std::unexpected;
#else
// using tl::expected;
// using tl::unexpected;
#endif

template <typename T> using own = T *;
template <typename T> using spy = T *;

using any = void;
using byte = char;

template <typename T> struct chan {};

// ----------------------------------------
// Error type and friends
namespace err {

constexpr auto STALE_CONNECTION = "stale connection";
constexpr auto PERMISSIONS_ERR = "permissions violation";
constexpr auto AUTHORIZATION_ERR = "authorization violation";
constexpr auto AUTHORIZATION_EXPIRED_ERR = "user authorization expired";
constexpr auto AUTHORIZATION_REVOKED_ERR = "user authorization revoked";
constexpr auto ACCOUNT_AUTHORIZATION_EXPIRED_ERR =
    "account authorization expired";
constexpr auto MAX_CONNECTION_ERR = "maximum connections exceeded";

} // namespace err

struct [[nodiscard]] Error : std::runtime_error {
  using base = std::runtime_error;
  Error() : base{""} {}
  Error(char const *what) : base{what} {}
  Error(std::string const &what) : base{what} {}

  bool has_error() { return *what() != '\0'; }
};

struct Err {
  inline static auto ConnectionClosed = Error("nats: connection closed");
  inline static auto ConnectionDraining = Error("nats: connection draining");
  inline static auto DrainTimeout =
      Error("nats: draining connection timed out");
  inline static auto ConnectionReconnecting =
      Error("nats: connection reconnecting");
  inline static auto SecureConnRequired =
      Error("nats: secure connection required");
  inline static auto SecureConnWanted =
      Error("nats: secure connection not available");
  inline static auto BadSubscription = Error("nats: invalid subscription");
  inline static auto TypeSubscription =
      Error("nats: invalid subscription type");
  inline static auto BadSubject = Error("nats: invalid subject");
  inline static auto BadQueueName = Error("nats: invalid queue name");
  inline static auto SlowConsumer =
      Error("nats: slow consumer, messages dropped");
  inline static auto Timeout = Error("nats: timeout");
  inline static auto BadTimeout = Error("nats: timeout invalid");
  inline static auto Authorization = Error("nats: authorization violation");
  inline static auto AuthExpired = Error("nats: authentication expired");
  inline static auto AuthRevoked = Error("nats: authentication revoked");
  inline static auto AccountAuthExpired =
      Error("nats: account authentication expired");
  inline static auto NoServers =
      Error("nats: no servers available for connection");
  inline static auto JsonParse =
      Error("nats: connect message, json parse error");
  inline static auto ChanArg =
      Error("nats: argument needs to be a channel type");
  inline static auto MaxPayload = Error("nats: maximum payload exceeded");
  inline static auto MaxMessages = Error("nats: maximum messages delivered");
  inline static auto SyncSubRequired =
      Error("nats: illegal call on an async subscription");
  inline static auto MultipleTLSConfigs =
      Error("nats: multiple tls.Configs not allowed");
  inline static auto ClientCertOrRootCAsRequired =
      Error("nats: at least one of certCB or rootCAsCB must be set");
  inline static auto NoInfoReceived =
      Error("nats: protocol exception, INFO not received");
  inline static auto ReconnectBufExceeded =
      Error("nats: outbound buffer limit exceeded");
  inline static auto InvalidConnection = Error("nats: invalid connection");
  inline static auto InvalidMsg = Error("nats: invalid message or message nil");
  inline static auto InvalidArg = Error("nats: invalid argument");
  inline static auto InvalidContext = Error("nats: invalid context");
  inline static auto NoDeadlineContext =
      Error("nats: context requires a deadline");
  inline static auto NoEchoNotSupported =
      Error("nats: no echo option not supported by this server");
  inline static auto ClientIDNotSupported =
      Error("nats: client ID not supported by this server");
  inline static auto UserButNoSigCB =
      Error("nats: user callback defined without a signature handler");
  inline static auto NkeyButNoSigCB =
      Error("nats: nkey defined without a signature handler");
  inline static auto NoUserCB = Error("nats: user callback not defined");
  inline static auto NkeyAndUser =
      Error("nats: user callback and nkey defined");
  inline static auto NkeysNotSupported =
      Error("nats: nkeys not supported by the server");
  inline static auto StaleConnection =
      Error("nats: " + std::string(err::STALE_CONNECTION));
  inline static auto TokenAlreadySet =
      Error("nats: token and token handler both set");
  inline static auto MsgNotBound =
      Error("nats: message is not bound to subscription/connection");
  inline static auto MsgNoReply = Error("nats: message does not have a reply");
  inline static auto ClientIPNotSupported =
      Error("nats: client IP not supported by this server");
  inline static auto Disconnected = Error("nats: server is disconnected");
  inline static auto HeadersNotSupported =
      Error("nats: headers not supported by this server");
  inline static auto BadHeaderMsg =
      Error("nats: message could not decode headers");
  inline static auto NoResponders =
      Error("nats: no responders available for request");
  inline static auto MaxConnectionsExceeded =
      Error("nats: server maximum connections exceeded");
  inline static auto ConnectionNotTLS = Error("nats: connection is not tls");
};
// Error type and friends
// ----------------------------------------

namespace io {
struct Reader {};
struct Writer {};
} // namespace io

namespace tls {

constexpr auto Scheme = "tls";
constexpr auto VersionTLS10 = 0x0301;
constexpr auto VersionTLS11 = 0x0302;
constexpr auto VersionTLS12 = 0x0303;
constexpr auto VersionTLS13 = 0x0304;

// Deprecated: SSLv3 is cryptographically broken, and is no longer
// supported by this package. See golang.org/issue/32716.
constexpr auto VersionSSL30 = 0x0300;

struct Config {
  uint16_t min_version;
};

struct Certificate {};

} // namespace tls

namespace x509 {

struct CertPool {};

} // namespace x509

namespace net {

struct Conn {};

struct Dialer {
  std::chrono::seconds timeout;
};

using IP = std::vector<byte>;

struct Addr {
  inline static std::string z0 = "";
  inline static std::string z4 = "z4";
  inline static std::string z6noz = "z6noz";

  // addr is the hi and lo bits of an IPv6 address. If z==z4,
  // hi and lo contain the IPv4-mapped IPv6 address.
  //
  // hi and lo are constructed by interpreting a 16-byte IPv6
  // address as a big-endian 128-bit number. The most significant
  // bits of that number go into hi, the rest into lo.
  //
  // For example, 0011:2233:4455:6677:8899:aabb:ccdd:eeff is stored as:
  //  addr.hi = 0x0011223344556677
  //  addr.lo = 0x8899aabbccddeeff
  //
  // We store IPs like this, rather than as [16]byte, because it
  // turns most operations on IPs into arithmetic and bit-twiddling
  // operations on 64-bit registers, which is much faster than
  // bytewise processing.
  struct {
    uint64_t hi{0};
    uint64_t lo{0};
  } addr;

  // z is a combination of the address family and the IPv6 zone.
  // null means invalid IP address
  // z4 means an IPv4 address
  // z6noz means an IPv6 address without a zone
  // otherwise it's the interned zone name string.
  std::any z;

  uint8_t v4(uint8_t i) {
    return static_cast<uint8_t>(addr.lo >> ((3 - i) * 8));
  }

  bool is6() {
    auto val = std::any_cast<std::string>(z);
    return val != Addr::z0 and val != Addr::z4;
  }

  std::array<byte, 16> as16() {
    auto ret = std::array<byte, 16>{};
    auto hi = std::byteswap(addr.hi);
    auto lo = std::byteswap(addr.lo);
    memcpy(&ret[0], &hi, 8);
    memcpy(&ret[8], &lo, 8);
    return ret;
  }

  std::string zone() {
    if (not z.has_value())
      return "";
    return std::any_cast<std::string>(z);
  }

  Addr with_zone(std::string const &zone) {
    if (not is6())
      return *this;
    if (zone == "") {
      z = Addr::z6noz;
      return *this;
    }
    z = zone;
    return *this;
  }
};

inline Addr addr_from4(std::array<byte, 4> addr) {
  return Addr{
      .addr =
          {
              .hi = 0,
              .lo = (0xffff00000000 | uint64_t(addr[0] << 24) |
                     uint64_t(addr[1] << 16) | uint64_t(addr[2] << 8) |
                     uint64_t(addr[3])),
          },
      .z = Addr::z4,
  };
}

inline Addr addr_from16(std::array<byte, 16> addr) {
  auto hi = uint64_t(*addr.data());
  auto lo = uint64_t(*(addr.data() + 8));
  return Addr{
      .addr =
          {
              .hi = std::byteswap(hi),
              .lo = std::byteswap(lo),

          },
      .z = Addr::z6noz,
  };
}

inline expected<Addr, Error> parse_ipv4(std::string_view s) {
  std::array<byte, 4> fields;
  int pos{0}, val{0};
  int dig_len{0};
  for (auto i = 0u; i < s.size(); ++i) {
    auto c = s[i];
    if (c >= '0' and c <= '9') {
      if (dig_len == 1 and val == 0)
        return unexpected(Error("IPv4 field has octet with leading zero"));
      val = val * 10 + c - '0';
      ++dig_len;
      if (val > 255)
        return unexpected(Error("IPv4 field has value > 255"));
    } else if (c == '.') {
      // .1.2.3
      // 1.2.3.
      // 1..2.3
      if (i == 0 or i == s.size() - 1 or s[i - 1] == '.')
        return unexpected(Error("IPv4 field must have at least one digit"));
      // 1.2.3.4.5
      if (pos == 3)
        return unexpected(Error("IPv4 address too long"));
      fields[pos++] = static_cast<uint8_t>(val);
      val = 0;
      dig_len = 0;
    } else {
      return unexpected(Error("unexpected character"));
    }
  }
  if (pos < 3)
    return unexpected(Error("IPv4 address too short"));
  fields[3] = static_cast<uint8_t>(val);
  return addr_from4(fields);
}

inline Addr ipv6_unspecified() {
  return {
      .addr = {},
      .z = Addr::z6noz,
  };
}

inline expected<Addr, Error> parse_ipv6(std::string_view in) {
  auto s = in;
  std::string zone = "";
  auto i = s.find_first_of('%');
  if (i != s.npos) {
    s = in.substr(0, i), zone = in.substr(i + 1);
    if (zone == "")
      return unexpected(Error("zone must be a non-empty string"));
  }

  auto ip = std::array<byte, 16>{};
  auto ellipsis = -1; // position of ellipsis in ip

  // might have leading ellipsis
  if (s.size() >= 2 and s[0] == ':' and s[1] == ':') {
    ellipsis = 0;
    s = s.substr(2);
    // might be only ellipsis
    if (s.size() == 0)
      return ipv6_unspecified().with_zone(zone);
  }

  // loop parsing hex numbers followed by colon
  i = 0;
  while (i < 16) {
    auto off = 0u;
    auto acc = uint32_t(0);
    for (; off < s.size(); ++off) {
      auto c = s[off];
      if (c >= '0' and c <= '9')
        acc = (acc << 4) + uint32_t(c - '0');
      else if (c >= 'a' and c <= 'f')
        acc = (acc << 4) + uint32_t(c - 'a' + 10);
      else if (c >= 'A' and c <= 'F')
        acc = (acc << 4) + uint32_t(c - 'A' + 10);
      else
        break;

      if (acc > std::numeric_limits<uint16_t>::max())
        return unexpected(Error("IPv6 field has value >= 2^16"));
    }
    if (off == 0)
      return unexpected(
          Error("each colon-separated field must have at least one digit"));

    // if followed by dot, might be in trailing IPv4
    if (off < s.size() and s[off] == '.') {
      if (ellipsis < 0 and i != 12)
        return unexpected(Error("embedded IPv4 address must replace the final "
                                "2 fields of the address"));
      if (i + 4 > 16)
        return unexpected(Error("too many hex fields to fit an embedded IPv4 "
                                "at the end of the address"));
      auto ipv4 = parse_ipv4(s);
      if (not ipv4.has_value())
        return unexpected(Error(ipv4.error()));

      ip[i] = ipv4.value().v4(0);
      ip[i + 1] = ipv4.value().v4(1);
      ip[i + 2] = ipv4.value().v4(2);
      ip[i + 3] = ipv4.value().v4(3);
      s = "";
      i += 4;
      break;
    }

    // save the 16-bit chunk
    ip[i] = static_cast<byte>(acc >> 8);
    ip[i + 1] = static_cast<byte>(acc);
    i += 2;

    // stop at end of string
    s = s.substr(off);
    if (s.size() == 0)
      break;

    // otherwise must be followed by colon and more
    if (s[0] != ':')
      return unexpected(Error("unexpected character, want colon"));
    else if (s.size() == 1)
      return unexpected(Error("colon must be followed by more characters"));
    s = s.substr(1);

    // look for ellipsis
    if (s[0] == ':') {
      if (ellipsis >= 0)
        return unexpected(Error("multiple :: in address"));
      ellipsis = i;
      s = s.substr(i);
      if (s.size() == 0)
        break;
    }
  }

  // must have used entire string
  if (s.size() != 0)
    return unexpected(Error("trailing garbage after address"));
  if (i < 16) {
    if (ellipsis < 0)
      return unexpected(Error("address string too short"));
    auto n = 16 - i;
    for (int j = i - 1; j >= ellipsis; --j)
      ip[j + n] = ip[j];
    for (int j = ellipsis + n - 1; j >= ellipsis; --j)
      ip[j] = 0;
  } else if (ellipsis >= 0) {
    return unexpected(
        Error("the :: must expand to at least one field of zeros"));
  }
  return addr_from16(ip).with_zone(zone);
}

inline expected<Addr, Error> parse_addr(std::string_view s) {
  for (auto c : s) {
    switch (c) {
    case '.':
      return parse_ipv4(s);
    case ':':
      return parse_ipv6(s);
    case '%':
      return unexpected(Error("missing IPv6 address"));
    }
  }
  return unexpected(Error("unable to parse IP"));
}

inline std::pair<std::array<byte, 16>, bool> _parse_ip(std::string_view s) {
  auto ip = parse_addr(s);
  if (not ip.has_value() or ip->zone() != "")
    return {{}, false};
  return {ip->as16(), true};
}

inline IP parse_ip(std::string_view s) {
  if (auto [addr, valid] = _parse_ip(s); valid)
    return IP{addr.begin(), addr.end()};
  return {};
}

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

namespace url {

struct Url {
  std::string_view scheme;
  std::string_view hostname() const {}
};

} // namespace url

namespace strings {

inline std::vector<std::string_view> split(std::string_view sv,
                                           std::string_view delims) {}
inline std::string_view trim_spaces(std::string_view sv) {}

} // namespace strings

struct Statistics {
  uint64_t in_msgs;
  uint64_t out_msgs;
  uint64_t in_bytes;
  uint64_t out_bytes;
  uint64_t reconnects;
};

// ----------------------------------------
// Header type and friends
using Header = std::unordered_map<std::string, std::vector<std::string>>;

// Header type and friends
// ----------------------------------------

// ----------------------------------------
// Status type and friends
enum struct Status {
  DISCONNECTED,
  CONNECTED,
  CLOSED,
  RECONNECTING,
  CONNECTING,
  DRAINING_SUBS,
  DRAINING_PUBS,
};

inline std::string_view to_strv(Status status) {
  using enum Status;
  switch (status) {
  case DISCONNECTED:
    return "DISCONNECTED";
  case CONNECTED:
    return "CONNECTED";
  case CLOSED:
    return "CLOSED";
  case RECONNECTING:
    return "RECONNECTING";
  case CONNECTING:
    return "CONNECTING";
  case DRAINING_SUBS:
    return "DRAINING_SUBS";
  case DRAINING_PUBS:
    return "DRAINING_PUBS";
  default:
    return "unknown status";
  };
}

namespace defaults {

constexpr auto Version = "1.0.0";
constexpr auto URL = "nats:://127.0.0.1:4222";
constexpr auto Port = 4222;
constexpr auto MaxReconnect = 60;
constexpr auto ReconnectWait = std::chrono::seconds(2);
constexpr auto ReconnectJitter = std::chrono::milliseconds(100);
constexpr auto ReconnectJitterTLS = std::chrono::seconds(1);
constexpr auto Timeout = std::chrono::seconds(2);
constexpr auto PingInterval = std::chrono::minutes(2);
constexpr auto MaxPingOut = 2;
constexpr auto MaxChanLen = 64 * 1024;             // 64k
constexpr auto ReconnectBufSize = 8 * 1024 * 1024; // 8MB
constexpr auto RequestChanLen = 8;
constexpr auto DrainTimeout = std::chrono::seconds(30);
constexpr auto FlusherTimeout = std::chrono::minutes(1);
constexpr auto LangString = "C++";

constexpr auto scratch_size = 512;
constexpr auto buf_size = 32768;
constexpr auto flush_chan_size = 1;
constexpr auto srv_pool_size = 4;
constexpr auto nuid_size = 22;
constexpr auto ws_port_string = "80";
constexpr auto wss_port_string = "443";
constexpr auto port_string = "4222";

} // namespace defaults
// Status type and friends
// ----------------------------------------

// ----------------------------------------
// Conn type and friends
struct srv {
  spy<url::Url> url;
  bool did_connect;
  int reconnects;
  Error last_err;
  bool is_implicit;
  std::string tls_name;
};

struct serverInfo {
  std::string id;
  std::string name;
  int proto;
  std::string version;
  std::string host;
  int port;
  bool headers;
  bool auth_required;
  bool tls_required;
  bool tls_available;
  int64_t max_payload;
  uint64_t cid;
  std::string client_ip;
  std::string nonce;
  std::string cluster;
  std::vector<std::string> connect_urls;
  bool lame_duck_mode;
};

enum struct clientProto {
  _, // Zero
  Info,
};

struct connectInfo {
  bool verbose;
  bool pedantic;
  std::string user_jwt;
  std::string nkey;
  std::string signature;
  std::string user;
  std::string pass;
  std::string token;
  bool tls;
  std::string name;
  std::string lang;
  std::string version;
  int protocol;
  bool echo;
  bool headers;
  bool no_responders;
};

struct parseState {};

struct barrierInfo {
  int64_t refs;
  std::function<void()> f;
};

struct Conn;
struct Subscription;

using ConnHandler = std::function<void(spy<Conn>)>;
using ConnErrHandler = std::function<void(Conn &, Error &)>;
using ErrHandler = std::function<void(spy<Conn>, spy<Subscription>, Error &)>;
using UserJWTHandler = std::function<void(std::string const &, Error &)>;
using TLSCertHandler = std::function<void(tls::Certificate, Error &)>;
using RootCAsHandler = std::function<void(x509::CertPool, Error &)>;
using SignatureHandler = std::function<void(std::span<byte>, Error)>;
using AuthTokenHandler = std::function<void(std::string const &)>;
using ReconnectDelayHandler =
    std::function<std::chrono::milliseconds(int attempts)>;

struct asyncCB {
  std::function<void()> f;
  std::shared_ptr<asyncCB> next;
};

struct asyncCallbacksHandler {
  std::mutex mu;
  std::condition_variable cond;
  std::shared_ptr<asyncCB> head;
  std::shared_ptr<asyncCB> tail;

  void push(auto f) { push_or_close(f, false); }

  void close() { push_or_close(nullptr, true); }

  void async_cb_dispatcher() {
    auto lock = std::unique_lock{mu};
    cond.wait(lock, [this] { return head != nullptr; });
    auto cur = head;
    head = cur->next;
    if (cur == tail)
      tail = nullptr;
    lock.unlock();

    if (cur->f == nullptr)
      return;
    cur->f();
  }

  void push_or_close(std::function<void()> f, bool close) {
    auto _ = std::lock_guard{mu};

    // make sure that library is not calling push with nil function,
    // since this is used to notify the dispatcher that it should stop.
    if (not close and f == nullptr)
      throw Error("pushing a nil callback");

    auto cb = std::make_shared<asyncCB>();
    cb->f = f;
    if (tail)
      tail->next = cb;
    else
      head = cb;
    tail = cb;
    if (close)
      cond.notify_all();
    else
      cond.notify_one();
  }
};

struct CustomDialer {
  expected<net::Conn, Error> dial(std::string_view network,
                                  std::string_view address);
};

struct InProcessConnProvider {
  expected<net::Conn, Error> inprocess_conn();
};

struct Options {
  std::string url{};
  InProcessConnProvider inprocess_server{};
  std::vector<std::string> servers{};
  bool no_randomize{};
  bool no_echo{};
  std::string name{};
  bool verbose{};
  bool pedantic{};
  bool secure{};
  std::optional<tls::Config> tls_config{};
  TLSCertHandler tls_cert_cb{};
  bool tls_handshake_first{};
  RootCAsHandler rootcas_cb{};
  bool allow_reconnect{};
  int max_reconnect{};
  std::chrono::seconds reconnect_wait{};
  ReconnectDelayHandler custom_reconnect_delay_cb{};
  std::chrono::milliseconds reconnect_jitter{};
  std::chrono::milliseconds reconnect_jitter_tls{};
  std::chrono::seconds timeout{};
  std::chrono::seconds drain_timeout{};
  std::chrono::minutes flusher_timeout{};
  std::chrono::seconds ping_interval{};
  int max_pings_out{};
  ConnHandler closed_cb{};
  ConnHandler disconnected_cb{};
  ConnErrHandler disconnected_err_cb{};
  ConnHandler connected_cb{};
  ConnHandler discovered_servers_cb{};
  ErrHandler async_error_cb{};
  int reconnect_buf_size{};
  int sub_chan_len{};
  UserJWTHandler user_jwt{};
  std::string nkey{};
  SignatureHandler signature_cb{};
  std::string user{};
  std::string password{};
  std::string token{};
  AuthTokenHandler token_handler{};
  std::shared_ptr<net::Dialer> dialer{};
  CustomDialer custom_dialer{};
  bool use_old_request_style{};
  bool no_callbacks_after_client_close{};
  ConnHandler lame_duck_mode_handler{};
  bool retry_on_failed_connect{};
  bool compression{};
  std::string proxy_path{};
  std::string inbox_prefix{};
  bool ignore_auth_error_abort{};
  bool skip_host_lookup{};

  expected<std::unique_ptr<Conn>, Error> connect();
};

inline Options get_default_options() {
  return Options{
      .allow_reconnect = true,
      .max_reconnect = defaults::MaxReconnect,
      .reconnect_wait = defaults::ReconnectWait,
      .reconnect_jitter = defaults::ReconnectJitter,
      .reconnect_jitter_tls = defaults::ReconnectJitterTLS,
      .timeout = defaults::Timeout,
      .drain_timeout = defaults::DrainTimeout,
      .flusher_timeout = defaults::FlusherTimeout,
      .ping_interval = defaults::PingInterval,
      .max_pings_out = defaults::MaxPingOut,
      .reconnect_buf_size = defaults::ReconnectBufSize,
      .sub_chan_len = defaults::MaxChanLen,
  };
}

using Option = std::function<Error(Options &)>;

enum struct SubStatus {
  Active,
  Draining,
  Closed,
  SlowConsumer,
};

inline std::string_view to_strv(SubStatus status) {
  using enum SubStatus;
  switch (status) {
  case Active:
    return "Active";
  case Draining:
    return "Draining";
  case Closed:
    return "Closed";
  case SlowConsumer:
    return "SlowConsumer";
  default:
    return "unknown status";
  }
}

namespace bytes {

struct Buffer {
  expected<size_t, Error> write_string(auto);

  std::vector<byte> bytes();
};

} // namespace bytes

namespace http {

struct Header {
  template <typename T> Header(T &&);

  Error write(auto);
};

} // namespace http

// ----------------------------------------
// Msg and friends
struct Msg {
  constexpr static std::string_view hdrLine = "NATS/1.0\r\n";
  constexpr static std::string_view crlf = "\r\n";
  constexpr static auto hdrPreEnd = hdrLine.size() - crlf.size();
  constexpr static auto statusHdr = "Status";
  constexpr static auto descrHdr = "Description";
  constexpr static auto lastConsumerSeqHdr = "Nats-Last-Consumer";
  constexpr static auto lastStreamSeqHdr = "Nats-Last-Stream";
  constexpr static auto consumerStalledHdr = "Nats-Consumer-Stalled";
  constexpr static auto noResponders = "503";
  constexpr static auto noMessagesSts = "404";
  constexpr static auto reqTimeoutSts = "408";
  constexpr static auto jetStream409Sts = "409";
  constexpr static auto controlMsg = "100";
  constexpr static auto statusLen = 3; // e.g. 20x, 40x, 50x

  std::string subject;
  std::string reply;
  Header header;
  std::span<byte> data;
  spy<Subscription> sub;
  // internal
  spy<Msg> next;
  int wsz;
  spy<barrierInfo> barrier;
  uint32_t ackd;

  size_t size() {
    if (wsz)
      return wsz;

    auto hdr_bytes = header_bytes().value();
    return subject.size() + reply.size() + hdr_bytes.size() + data.size();
  }

  expected<std::vector<byte>, Error> header_bytes() {
    if (header.empty())
      return {};

    auto b = bytes::Buffer{};
    auto res = b.write_string(hdrLine);
    if (not res.has_value())
      return unexpected(Err::BadHeaderMsg);

    auto err = http::Header(header).write(b);
    if (err.has_error())
      return unexpected(Err::BadHeaderMsg);

    res = b.write_string(crlf);
    if (not res.has_value())
      return unexpected(Err::BadHeaderMsg);

    return b.bytes();
  }
};

using MsgHandler = std::function<void(spy<Msg>)>;

struct msgFilter {};

struct Reader {
  io::Reader r;
  std::span<byte> buf;
  int off;
  int n;
};

struct Writer {
  io::Writer w;
  std::span<byte> bufs;
  int limit;
  spy<bytes::Buffer> pending;
};
// Msg and friends
// ----------------------------------------

namespace proto {

constexpr auto _CRLF_ = "\r\n";
constexpr auto _EMPTY_ = "";
constexpr auto _SPC_ = " ";
constexpr auto _PUB_P_ = "PUB ";
constexpr auto _HPUB_P_ = "HPUB ";

constexpr auto _OK_OP_ = "+OK";
constexpr auto _ERR_OP_ = "-ERR";
constexpr auto _PONG_OP_ = "PONG";
constexpr auto _INFO_OP_ = "INFO";

constexpr auto CONNECT = "CONNECT %s\r\n";
constexpr auto PING = "PING\r\n";
constexpr auto PONG = "PONG\r\n";
constexpr auto SUB = "SUB %s %s %d\r\n";
constexpr auto UNSUB = "UNSUB %d %s\r\n";
constexpr auto OK = "+OK\r\n";

} // namespace proto

} // namespace nats
