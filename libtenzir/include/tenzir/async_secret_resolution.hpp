//
//  ▀▀█▀▀ █▀▀▀ █▄  █ ▀▀▀█▀ ▀█▀ █▀▀▄
//    █   █▀▀  █ ▀▄█  ▄▀    █  █▀▀▄
//    ▀   ▀▀▀▀ ▀   ▀ ▀▀▀▀▀ ▀▀▀ ▀  ▀
//
// SPDX-FileCopyrightText: (c) 2025 The Tenzir Contributors
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "tenzir/async/shared_mutex.hpp"
#include "tenzir/detail/heterogeneous_string_hash.hpp"
#include "tenzir/ecc.hpp"
#include "tenzir/option.hpp"

namespace tenzir {

class SecretCache {
public:
  constexpr static auto ttl = std::chrono::minutes{15};

  auto lookup(std::string_view key) const -> Option<ecc::cleansing_blob> {
    auto it = data_.find(key);
    if (it == data_.end()) {
      return None{};
    }
    auto& [value, last_used] = it->second;
    last_used = time::clock::now();
    return value;
  }

  auto insert(std::string key, ecc::cleansing_blob value) -> void {
    data_.try_emplace(std::move(key), std::move(value), time::clock::now());
  }

  auto cleanup() -> duration {
    auto const now = time::clock::now();
    auto const cutoff = now - ttl;
    auto wait = duration{ttl};
    for (auto it = data_.begin(); it != data_.end();) {
      const auto& [_, entry] = *it;
      auto last_used = entry.last_used.load(std::memory_order_relaxed);
      if (last_used < cutoff) {
        it = data_.erase(it);
        continue;
      }
      auto const age = (now - last_used);
      auto const entry_wait = ttl - age;
      if (entry_wait < wait) {
        wait = entry_wait;
      }
      ++it;
    }
    return wait;
  }

  static auto instance() -> SharedMutex<SecretCache>& {
    static auto instance = SharedMutex<SecretCache>{};
    return instance;
  }

private:
  struct cache_entry {
    ecc::cleansing_blob value;
    mutable Atomic<time> last_used;
  };

  std::unordered_map<std::string, cache_entry, detail::heterogeneous_string_hash,
                     detail::heterogeneous_string_equal>
    data_;
};
} // namespace tenzir
