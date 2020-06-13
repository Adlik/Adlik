// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_DEMO_IMAGE_H
#define ADLIK_SERVING_DEMO_IMAGE_H

#include <memory>

namespace adlik {
namespace serving {
namespace demo {
class Image {
  size_t width;
  size_t height;
  size_t channels;
  std::unique_ptr<float[]> data;

public:
  Image(size_t width, size_t height, size_t channels)
      : width{width}, height{height}, channels{channels}, data{std::make_unique<float[]>(width * height * channels)} {
  }

  size_t get_width() const {
    return this->width;
  }

  size_t get_height() const {
    return this->height;
  }

  size_t get_channels() const {
    return this->channels;
  }

  float* get_data() {
    return this->data.get();
  }

  const float* get_data() const {
    return this->data.get();
  }
};
}  // namespace demo
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_DEMO_IMAGE_H
