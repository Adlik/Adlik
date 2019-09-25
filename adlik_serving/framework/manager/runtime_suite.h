#ifndef H4F9C0189_654D_4E8C_9AEA_DDE5FF008123
#define H4F9C0189_654D_4E8C_9AEA_DDE5FF008123

#include <string>
#include <unordered_map>

#include "adlik_serving/framework/manager/runtime.h"
#include "cub/gof/singleton.h"

namespace adlik {
namespace serving {

DEF_SINGLETON(RuntimeSuite) EXTENDS(Runtime) {
  ~RuntimeSuite();
  void add(const char*, Runtime*);
  Runtime* get(const std::string&);

private:
  OVERRIDE(void init(cub::ProgramOptions&));
  OVERRIDE(cub::Status config(const RuntimeContext&));

private:
  std::unordered_map<std::string, Runtime*> runtimes;
};

}  // namespace serving
}  // namespace adlik

#endif
