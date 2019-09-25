#ifndef ADLIK_SERVING_RUNTIME_OPENVINO_OPENVINO_H
#define ADLIK_SERVING_RUNTIME_OPENVINO_OPENVINO_H

#define OPENVINO_NS openvino
#define OPENVINO_NS_BEGIN namespace OPENVINO_NS {
#define OPENVINO_NS_END }
#define USING_OPENVINO_NS using namespace OPENVINO_NS;
#define FWD_DECL_OPENVINO(type) \
  namespace OPENVINO_NS {       \
  struct type;                  \
  }

#endif
