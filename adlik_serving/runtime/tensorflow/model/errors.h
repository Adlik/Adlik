// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H3E7F075D_8AA3_4E41_B53D_DE0E9B931B13
#define H3E7F075D_8AA3_4E41_B53D_DE0E9B931B13

#include "tensorflow/core/lib/core/errors.h"

#define TF_ASSERT_TRUE(...)                                 \
  do {                                                      \
    bool _result = (__VA_ARGS__);                           \
    if (TF_PREDICT_FALSE(!_result))                         \
      return ::tensorflow::errors::Internal("assert fail"); \
  } while (0)

#define TF_ASSERT_FALSE(...)                                \
  do {                                                      \
    bool _result = (__VA_ARGS__);                           \
    if (TF_PREDICT_FALSE(_result))                          \
      return ::tensorflow::errors::Internal("assert fail"); \
  } while (0)

#define TF_ASSERT_TRUE_VOID(...)    \
  do {                              \
    bool _result = (__VA_ARGS__);   \
    if (TF_PREDICT_FALSE(!_result)) \
      return;                       \
  } while (0)

#define TF_ASSERT_SUCC_CALL_VOID(...)                   \
  do {                                                  \
    const ::tensorflow::Status _status = (__VA_ARGS__); \
    if (TF_PREDICT_FALSE(!_status.ok()))                \
      return;                                           \
  } while (0)

#define TF_ASSERT_SUCC_CALL(...) TF_RETURN_IF_ERROR(__VA_ARGS__)

#endif
