// Copyright 2021 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ABSL_CLEANUP_INTERNAL_CLEANUP_H_
#define ABSL_CLEANUP_INTERNAL_CLEANUP_H_

#include <type_traits>
#include <utility>

#include "absl/base/internal/invoke.h"
#include "absl/base/thread_annotations.h"
#include "absl/utility/utility.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

namespace cleanup_internal {

struct Tag {};

template <typename Arg, typename... Args>
constexpr bool WasDeduced() {
  return (std::is_same<cleanup_internal::Tag, Arg>::value) &&
         (sizeof...(Args) == 0);
}

template <typename Callback>
constexpr bool ReturnsVoid() {
  return (std::is_same<base_internal::invoke_result_t<Callback>, void>::value);
}

template <typename Callback>
class Storage {
 public:
  Storage() = delete;

  explicit Storage(Callback callback)
      : engaged_(true), callback_(std::move(callback)) {}

  Storage(Storage&& other)
      : engaged_(absl::exchange(other.engaged_, false)),
        callback_(std::move(other.callback_)) {}

  Storage(const Storage& other) = delete;

  Storage& operator=(Storage&& other) = delete;

  Storage& operator=(const Storage& other) = delete;

  bool IsCallbackEngaged() const { return engaged_; }

  void DisengageCallback() { engaged_ = false; }

  void InvokeCallback() ABSL_NO_THREAD_SAFETY_ANALYSIS {
    std::move(callback_)();
  }

 private:
  bool engaged_;
  Callback callback_;
};

}  // namespace cleanup_internal

ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_CLEANUP_INTERNAL_CLEANUP_H_
