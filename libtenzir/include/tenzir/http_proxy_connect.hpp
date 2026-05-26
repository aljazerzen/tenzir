//
//  ▀▀█▀▀ █▀▀▀ █▄  █ ▀▀▀█▀ ▀█▀ █▀▀▄
//    █   █▀▀  █ ▀▄█  ▄▀    █  █▀▀▄
//    ▀   ▀▀▀▀ ▀   ▀ ▀▀▀▀▀ ▀▀▀ ▀  ▀
//
// SPDX-FileCopyrightText: (c) 2026 The Tenzir Contributors
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <chrono>
#include <string>

#include <folly/coro/Task.h>
#include <proxygen/lib/http/coro/client/HTTPClient.h>
#include <proxygen/lib/http/coro/client/HTTPCoroConnector.h>

namespace folly {
class EventBase;
} // namespace folly

namespace tenzir {

/// Opens an `HTTPCoroSession` to `(host, port)` either directly or via
/// the configured HTTP proxy.
///
/// When `tenzir.http-proxy` is configured and `host` is not on the
/// `tenzir.no-proxy` bypass list, this routes through HTTP CONNECT:
///   1. Resolves DNS for the proxy host and opens a session to the
///      proxy. The session is TLS-wrapped when the proxy URL uses
///      `https://`, otherwise plain HTTP.
///   2. Issues `HTTPCoroConnector::proxyConnect` to perform the
///      CONNECT handshake to `(host, port)`. The TLS settings for
///      the *target* (i.e. when the target itself is HTTPS) come from
///      `direct_conn_params`. When the proxy URL includes userinfo,
///      a `Proxy-Authorization: Basic` header is injected on the
///      CONNECT request.
///
/// Without a configured proxy, the behaviour is identical to
/// `HTTPCoroConnector::connect` with a pre-resolved server address.
auto connect_session_via_proxy_if_configured(
  folly::EventBase* evb, std::string host, uint16_t port,
  proxygen::coro::HTTPCoroConnector::ConnectionParams direct_conn_params,
  proxygen::coro::HTTPCoroConnector::SessionParams session_params,
  std::chrono::milliseconds connect_timeout)
  -> folly::coro::Task<proxygen::coro::HTTPCoroSession*>;

} // namespace tenzir
