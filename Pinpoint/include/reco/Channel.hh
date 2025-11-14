// This file is part of the ACTS project.
//
// Copyright (C) 2016 CERN for the benefit of the ACTS project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "reco/MultiIndex.hh"

#include <cstdint>

class Channel : public Acts::MultiIndex<std::uint64_t, 12, 12, 16, 8, 16> {
  using Base = Acts::MultiIndex<std::uint64_t, 12, 12, 16, 8, 16>;

 public:
  using Base::Base;
  using Base::Value;

  // Construct an invalid Channel with all levels set to zero.
  constexpr Channel() : Base(Base::Zeros()) {}
  Channel(const Channel&) = default;
  Channel(Channel&&) = default;
  Channel& operator=(const Channel&) = default;
  Channel& operator=(Channel&&) = default;

  /// Return the strip identifier.
  constexpr Value stripNumber() const { return level(0); }
  /// Return the side identifier.
  constexpr Value sideNumber() const { return level(1); }
  /// Return the module identifier.
  constexpr Value moduleNumber() const { return level(2); }
  /// Return the layer identifier.
  constexpr Value LayerNumber() const { return level(3); }


  /// Set the strip identifier.
  constexpr Channel& setStrip(Value id) {
    set(0, id);
    return *this;
  }
  /// Set the side identifier.
  constexpr Channel& setSide(Value id) {
    set(1, id);
    return *this;
  }
  /// Set the module identifier.
  constexpr Channel& setModule(Value id) {
    set(2, id);
    return *this;
  }
  /// Set the layer identifier.
  constexpr Channel& setLayer(Value id) {
    set(3, id);
    return *this;
  }

  /// Reduce the Channel to the strip identifier.
  constexpr Channel strip() const {
    return Channel()
        .setStrip(stripNumber())
        .setSide(sideNumber())
        .setModule(moduleNumber());
  }

  friend inline std::ostream& operator<<(std::ostream& os, Channel Channel) {
    os << "s=" << Channel.stripNumber()
       << "|d=" << Channel.sideNumber() << "|m=" << Channel.moduleNumber()
       << "|l=" << Channel.LayerNumber();
    return os;
  }
};

// specialize std::hash so Channel can be used e.g. in an unordered_map
namespace std {
template <>
struct hash<Channel> {
  auto operator()(Channel Channel) const noexcept {
    return std::hash<Channel::Value>()(Channel.value());
  }
};
}  // namespace std
