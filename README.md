# Wutong SDK for C++ · 梧桐 C++ SDK

[![CI](https://github.com/project-wutong/wutong-sdk-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/project-wutong/wutong-sdk-cpp/actions/workflows/ci.yml)
C++20 · CMake ≥ 3.24 · header-only over the Rust core's C ABI · `AGPL-3.0-only`

## 中文

头文件形式的 C++20 RAII 封装，链接 Rust 核心导出的稳定 C ABI（`wutong-core/crates/wutong-ffi`，`wutong.h`）。
C++ 侧不实现任何协议规则；错误以 `wutong::Error`（`code()` 为规范错误码）抛出。
（协议包自带的 C++23 参考实现保留在 `wutong-protocol` 中，仅作为独立一致性实现。）

```cmake
include(FetchContent)
FetchContent_Declare(wutong_sdk GIT_REPOSITORY https://github.com/project-wutong/wutong-sdk-cpp.git GIT_TAG main)
FetchContent_MakeAvailable(wutong_sdk)
target_link_libraries(app PRIVATE Wutong::sdk)
```

```cpp
#include <wutong/wutong.hpp>
auto canon = wutong::canonicalize(json);
wutong::validate(envelope);   // throws wutong::Error{"ERR_SCHEMA: ..."}
```

构建需要 Rust 工具链（CMake 会调用 `cargo build -p wutong-ffi`）。可用 `-DWUTONG_CORE_DIR=` 指向本地 `wutong-core`。

## English

Header-only C++20 wrapper over the stable C ABI of the Rust core. CMake fetches `wutong-core`
(`-DWUTONG_CORE_REF=` to pin) and builds `libwutong_ffi` with cargo. Tests validate every normative
example shipped with the core.
