# SPDX-License-Identifier: AGPL-3.0-only
cmake_minimum_required(VERSION 3.24)
project(wutong-sdk-cpp VERSION 0.1.0 LANGUAGES CXX)

option(WUTONG_SDK_BUILD_TESTS "Build tests" ${PROJECT_IS_TOP_LEVEL})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(WutongCore)

add_library(wutong_sdk INTERFACE)
add_library(Wutong::sdk ALIAS wutong_sdk)
target_compile_features(wutong_sdk INTERFACE cxx_std_20)
target_include_directories(wutong_sdk INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(wutong_sdk INTERFACE Wutong::ffi)

if(WUTONG_SDK_BUILD_TESTS)
  enable_testing()
  add_executable(wutong_sdk_tests tests/sdk_tests.cpp)
  target_link_libraries(wutong_sdk_tests PRIVATE Wutong::sdk)
  target_compile_definitions(wutong_sdk_tests PRIVATE
    WUTONG_VECTORS_DIR="${WUTONG_CORE_DIR}/conformance/examples")
  if(MSVC)
    target_compile_options(wutong_sdk_tests PRIVATE /W4 /WX)
  else()
    target_compile_options(wutong_sdk_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
  endif()
  add_test(NAME wutong_sdk_tests COMMAND wutong_sdk_tests)
endif()
