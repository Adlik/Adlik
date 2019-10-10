// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_UTIL_NAME_DEF_H
#define ADLIK_SERVING_RUNTIME_UTIL_NAME_DEF_H

namespace adlik {
namespace serving {

#define MODEL_WRAPPER(Prefix) Prefix##Wrapper

#define MODEL_FACTORY(Prefix) Prefix##Factory

#define RUNTIME(Prefix) Prefix##Runtime

}  // namespace serving
}  // namespace adlik

#endif
