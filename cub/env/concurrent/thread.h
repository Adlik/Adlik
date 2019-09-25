#ifndef H73C3A36C_6C53_433C_912B_967BB46AD5F1
#define H73C3A36C_6C53_433C_912B_967BB46AD5F1

#include <functional>

namespace cub {

using thread_t = std::function<void()>;

struct Thread {
  // join thread until stops running.
  virtual ~Thread() = 0;
};

}  // namespace cub

#endif
