// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/demo/decode_png.h"

#include <algorithm>
#include <stdexcept>

#include "absl/types/optional.h"
#include "png.h"

using absl::optional;
using std::decay_t;
using std::logic_error;
using std::runtime_error;

namespace {
template <class F>
class CleanUp {
  optional<F> func;

public:
  explicit CleanUp(F func) : func{std::move(func)} {
  }

  CleanUp(CleanUp&& other) : func{std::exchange(other.func, absl::nullopt)} {
  }

  CleanUp(const CleanUp&) = delete;

  ~CleanUp() {
    if (func) {
      (*func)();
    };
  }

  CleanUp& operator=(const CleanUp&) = delete;
  CleanUp& operator=(CleanUp&&) = delete;
};

template <class F>
CleanUp<decay_t<F>> make_clean_up(F&& func) {
  return CleanUp<decay_t<F>>(std::forward<F>(func));
}
}  // namespace

namespace adlik {
namespace serving {
namespace demo {
Image decode_png(const char* file_path, size_t channels) {
  png_image raw_image{};

  raw_image.version = PNG_IMAGE_VERSION;

  if (png_image_begin_read_from_file(&raw_image, file_path) == 0) {
    throw runtime_error{"Unable to read image."};
  }

  const auto clean_up = make_clean_up([&raw_image] { png_image_free(&raw_image); });

  switch (channels) {
    case 1:
      raw_image.format = PNG_FORMAT_GRAY;
      break;
    case 3:
      raw_image.format = PNG_FORMAT_RGB;
      break;
    case 4:
      raw_image.format = PNG_FORMAT_RGBA;
      break;
    default:
      throw logic_error{"Invalid channels."};
  }

  const auto buffer_size = PNG_IMAGE_SIZE(raw_image);

  if (buffer_size != raw_image.height * raw_image.width * channels) {
    throw runtime_error{"Unexpected image size."};
  }

  const auto temp_buffer = std::make_unique<uint8_t[]>(buffer_size);

  if (png_image_finish_read(&raw_image, nullptr, temp_buffer.get(), 0, nullptr) == 0) {
    throw runtime_error{"Unable to finish reading image."};
  }

  Image result{raw_image.width, raw_image.height, channels};

  std::transform(temp_buffer.get(), temp_buffer.get() + buffer_size, result.get_data(), [](auto num) {
    return static_cast<float>(num);
  });

  return result;
}
}  // namespace demo
}  // namespace serving
}  // namespace adlik
