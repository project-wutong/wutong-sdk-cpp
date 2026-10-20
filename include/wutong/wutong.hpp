// SPDX-License-Identifier: AGPL-3.0-only
// Wutong Protocol C++ SDK — a thin, header-only, RAII C++20 wrapper over the stable C ABI
// (wutong.h) exported by the Rust core (wutong-core / wutong-ffi). No protocol logic lives
// here: every rule is implemented once, in Rust.
#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include "wutong.h"

namespace wutong {

/// A protocol error. code() is the normative WTP error code (e.g. "ERR_SCHEMA").
class Error : public std::runtime_error {
  public:
    explicit Error(const std::string &message)
        : std::runtime_error(message), code_(message.substr(0, message.find(':'))) {}
    [[nodiscard]] const std::string &code() const noexcept { return code_; }

  private:
    std::string code_;
};

namespace detail {
struct Free {
    void operator()(char *p) const noexcept { wutong_string_free(p); }
};
using Owned = std::unique_ptr<char, Free>;
inline const std::uint8_t *bytes(std::string_view s) noexcept {
    return reinterpret_cast<const std::uint8_t *>(s.data());
}
[[noreturn]] inline void raise(char *err) {
    Owned e{err};
    throw Error(e ? std::string(e.get()) : std::string("ERR_INTERNAL: unknown error"));
}
} // namespace detail

/// ABI version of the linked core.
[[nodiscard]] inline std::uint32_t abi_version() noexcept { return wutong_abi_version(); }

/// Implementation string of the linked core (e.g. "wutong-core-rs/0.1.0").
[[nodiscard]] inline std::string_view implementation() noexcept { return wutong_implementation(); }

/// Strictly parse JSON and return its RFC 8785 (JCS) canonical form.
[[nodiscard]] inline std::string canonicalize(std::string_view json) {
    char *out = nullptr;
    char *err = nullptr;
    if (wutong_jcs(detail::bytes(json), json.size(), &out, &err) != 0)
        detail::raise(err);
    detail::Owned o{out};
    return std::string(o.get());
}

/// Strict parse + JSON Schema + semantic validation of an envelope; throws wutong::Error.
inline void validate(std::string_view json) {
    char *err = nullptr;
    if (wutong_validate_envelope(detail::bytes(json), json.size(), &err) != 0)
        detail::raise(err);
}

/// "sha256:<hex>" of the exact bytes.
[[nodiscard]] inline std::string digest(std::string_view data) {
    detail::Owned d{wutong_sha256(detail::bytes(data), data.size())};
    return std::string(d.get());
}

} // namespace wutong
