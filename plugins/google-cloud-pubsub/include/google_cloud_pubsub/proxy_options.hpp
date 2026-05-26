//
//  ▀▀█▀▀ █▀▀▀ █▄  █ ▀▀▀█▀ ▀█▀ █▀▀▄
//    █   █▀▀  █ ▀▄█  ▄▀    █  █▀▀▄
//    ▀   ▀▀▀▀ ▀   ▀ ▀▀▀▀▀ ▀▀▀ ▀  ▀
//
// SPDX-FileCopyrightText: (c) 2026 The Tenzir Contributors
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <tenzir/proxy_settings.hpp>

#include <google/cloud/grpc_options.h>
#include <google/cloud/options.h>
#include <grpc/impl/channel_arg_names.h>

#include <map>
#include <string>
#include <utility>

namespace tenzir::plugins::google_cloud_pubsub {

/// Returns `opts` with the resolved `tenzir.http-proxy` URL set as a
/// gRPC channel argument when a proxy is configured and the well-known
/// Pub/Sub endpoint is not on `tenzir.no-proxy`. The pubsub
/// connection-factory functions all accept a `google::cloud::Options`
/// argument; threading the channel arg through that is the documented
/// upstream way to add an HTTP CONNECT proxy.
inline auto with_proxy_options(google::cloud::Options opts)
  -> google::cloud::Options {
  if (auto proxy = proxy_for_host("pubsub.googleapis.com")) {
    opts.set<google::cloud::GrpcChannelArgumentsOption>(
      std::map<std::string, std::string>{
        {GRPC_ARG_HTTP_PROXY, *proxy},
      });
  }
  return opts;
}

} // namespace tenzir::plugins::google_cloud_pubsub
