// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "types.hpp"

namespace nats {

struct Defaults {
  constexpr static auto Version = "1.0.0";
  constexpr static auto URL = "nats:://127.0.0.1:4222";
  constexpr static auto Port = 4222;
  constexpr static auto MaxReconnect = 60;
  constexpr static auto ReconnectWait = std::chrono::seconds(2);
  constexpr static auto ReconnectJitter = std::chrono::milliseconds(100);
  constexpr static auto ReconnectJitterTLS = std::chrono::seconds(1);
  constexpr static auto Timeout = std::chrono::seconds(2);
  constexpr static auto PingInterval = std::chrono::minutes(2);
  constexpr static auto MaxPingOut = 2;
  constexpr static auto MaxChanLen = 64 * 1024;             // 64k
  constexpr static auto ReconnectBufSize = 8 * 1024 * 1024; // 8MB
  constexpr static auto RequestChanLen = 8;
  constexpr static auto DrainTimeout = std::chrono::seconds(30);
  constexpr static auto FlusherTimeout = std::chrono::minutes(1);
  constexpr static auto LangString = "C++";
};

struct ErrStr {
  constexpr static auto STALE_CONNECTION = "stale connection";
  constexpr static auto PERMISSIONS_ERR = "permissions violation";
  constexpr static auto AUTHORIZATION_ERR = "authorization violation";
  constexpr static auto AUTHORIZATION_EXPIRED_ERR =
      "user authorization expired";
  constexpr static auto AUTHORIZATION_REVOKED_ERR =
      "user authorization revoked";
  constexpr static auto ACCOUNT_AUTHORIZATION_EXPIRED_ERR =
      "account authorization expired";
  constexpr static auto MAX_CONNECTION_ERR = "maximum connections exceeded";
};

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

struct Conn;
struct Options;
struct Error;
struct Subscription;

using ConnHandler = std::function<void(Conn &)>;
using ConnErrHandler = std::function<void(Conn &, Error &)>;
using ErrHandler = std::function<void(Conn &, Subscription &, Error &)>;
using UserJWTHandler = std::function<void(std::string const &, Error &)>;
using TLSCertHandler = std::function<void(tls::Certificate, Error &)>;
using RootCAsHandler = std::function<void(x509::CertPool, Error &)>;
using SignatureHandler = std::function<void(std::span<byte>, Error)>;
using AuthTokenHandler = std::function<void(std::string const &)>;
using ReconnectDelayHandler =
    std::function<std::chrono::milliseconds(int attempts)>;

struct asyncCB {
  std::function<void()> f;
  asyncCB *next;
};

struct asyncCallbacksHandler {
  std::mutex mu;
  std::condition_variable cond;
  asyncCB *head;
  asyncCB *tail;
};

using Option = std::function<Error(Options &)>;

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
  tls::Config tls_config{};
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
  net::Dialer dialer{};
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
};

inline Options get_default_options() {
  return Options{
      .allow_reconnect = true,
      .max_reconnect = Defaults::MaxReconnect,
      .reconnect_wait = Defaults::ReconnectWait,
      .reconnect_jitter = Defaults::ReconnectJitter,
      .reconnect_jitter_tls = Defaults::ReconnectJitterTLS,
      .timeout = Defaults::Timeout,
      .drain_timeout = Defaults::DrainTimeout,
      .flusher_timeout = Defaults::FlusherTimeout,
      .ping_interval = Defaults::PingInterval,
      .max_pings_out = Defaults::MaxPingOut,
      .reconnect_buf_size = Defaults::ReconnectBufSize,
      .sub_chan_len = Defaults::MaxChanLen,
  };
}

struct defaults {
  constexpr static auto scratch_size = 512;
  constexpr static auto buf_size = 32768;
  constexpr static auto flush_chan_size = 1;
  constexpr static auto srv_pool_size = 4;
  constexpr static auto nuid_size = 22;
  constexpr static auto ws_port_string = "80";
  constexpr static auto wss_port_string = "443";
  constexpr static auto port_string = "4222";
};

struct srv {};

struct serverInfo {};
struct parseState {};

struct Msg {};
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

struct Conn {
  sync::RWMutex mu;
  Options opts;
  sync::WaitGroup wg;
  std::vector<spy<srv>> srv_pool;
  spy<srv> current;
  std::unordered_map<std::string, std::string> urls;
  net::Conn conn;
  spy<Writer> bw;
  spy<Reader> br;
  chan<any> fch;
  serverInfo info;
  int64_t ssid;
  sync::RWMutex subs_mu;
  std::unordered_map<int64_t, spy<Subscription>> subs;
  spy<asyncCallbacksHandler> ach;
  std::vector<chan<any>> pongs;
  std::array<byte, defaults::scratch_size> scratch;
  Status status;
  std::unordered_map<Status, std::vector<chan<Status>>> stat_listeners;
  bool initc;
  Error err;
  spy<parseState> ps;
  spy<time::Timer> ptmr;
  int pout;
  bool ar; // abort reconnect
  chan<any> rqch;
  bool ws; // true if websocket connection

  // new style response handler
  std::string resp_sub;        // the wildcard subject
  std::string resp_sub_prefix; // the wildcard prefix including trailing .
  int resp_sub_len; // the length of the wildcard prefix excluding trailing .
  spy<Subscription> resp_mux; // a single response subscription
  std::unordered_map<std::string,
                     chan<spy<Msg>>>
      resp_map;         // request map for response msg channels
  rand::Rand resp_rand; // used for generating suffix

  // msg filters for testing, protected by subs_mu
  std::unordered_map<std::string, msgFilter> filters;
};

struct jsSub {};
struct MsgHandler {};

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

struct SubscriptionType {};

struct Subscription {
  std::mutex mu;
  int64_t sid;

  std::string subject;
  std::string queue;

  spy<jsSub> jsi;

  uint64_t delivered;
  uint64_t max;
  spy<Conn> conn;
  MsgHandler mcb;
  chan<spy<Msg>> mch;
  bool closed;
  bool sc;
  bool conn_closed;
  bool draining;
  SubStatus status;
  std::unordered_map<chan<SubStatus>, std::vector<SubStatus>> stat_listeners;

  // type of subscription
  SubscriptionType typ;

  // async linked list
  spy<Msg> p_head;
  spy<Msg> p_tail;
  std::condition_variable p_cond;
  std::function<void(std::string subject)> p_done;

  // pending stats, async subscriptions, high-speed etc.
  int p_msgs;
  int p_bytes;
  int p_msgs_max;
  int p_bytes_max;
  int p_msgs_limit;
  int p_bytes_limit;
  int dropped;
};

} // namespace nats
