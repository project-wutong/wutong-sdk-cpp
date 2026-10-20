# SPDX-License-Identifier: AGPL-3.0-only
# Provides the imported target Wutong::ffi (static libwutong_ffi from the Rust core).
#   -DWUTONG_CORE_DIR=/path/to/wutong-core   use a local checkout
#   otherwise wutong-core is fetched at WUTONG_CORE_REF (default: main).
include(FetchContent)
set(WUTONG_CORE_REF "main" CACHE STRING "wutong-core git ref to build")
if(NOT WUTONG_CORE_DIR)
  FetchContent_Declare(wutong_core
    GIT_REPOSITORY https://github.com/project-wutong/wutong-core.git
    GIT_TAG ${WUTONG_CORE_REF}
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(wutong_core)
  set(WUTONG_CORE_DIR ${wutong_core_SOURCE_DIR})
endif()
find_program(CARGO cargo REQUIRED)
set(_wt_target ${CMAKE_BINARY_DIR}/cargo)
if(WIN32)
  set(_wt_lib ${_wt_target}/release/wutong_ffi.lib)
else()
  set(_wt_lib ${_wt_target}/release/libwutong_ffi.a)
endif()
add_custom_command(OUTPUT ${_wt_lib}
  COMMAND ${CARGO} build --release --locked -p wutong-ffi --target-dir ${_wt_target}
  WORKING_DIRECTORY ${WUTONG_CORE_DIR}
  COMMENT "Building wutong-ffi (Rust core)" USES_TERMINAL VERBATIM)
add_custom_target(wutong_ffi_build DEPENDS ${_wt_lib})
add_library(wutong_ffi STATIC IMPORTED GLOBAL)
set_target_properties(wutong_ffi PROPERTIES
  IMPORTED_LOCATION ${_wt_lib}
  INTERFACE_INCLUDE_DIRECTORIES ${WUTONG_CORE_DIR}/crates/wutong-ffi/include)
add_dependencies(wutong_ffi wutong_ffi_build)
find_package(Threads REQUIRED)
if(WIN32)
  set_property(TARGET wutong_ffi APPEND PROPERTY INTERFACE_LINK_LIBRARIES ws2_32 userenv ntdll bcrypt)
elseif(APPLE)
  set_property(TARGET wutong_ffi APPEND PROPERTY INTERFACE_LINK_LIBRARIES Threads::Threads)
else()
  set_property(TARGET wutong_ffi APPEND PROPERTY INTERFACE_LINK_LIBRARIES Threads::Threads ${CMAKE_DL_LIBS} m)
endif()
add_library(Wutong::ffi ALIAS wutong_ffi)
