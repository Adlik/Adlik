#include "adlik_serving/framework/manager/runtime_suite.h"

#include "cub/base/assertions.h"

namespace adlik {
namespace serving {

RuntimeSuite::~RuntimeSuite() {
  for (auto it : runtimes) {
    delete it.second;
  }
}

void RuntimeSuite::add(const char* name, Runtime* runtime) {
  INFO_LOG << "found runtime " << name;
  runtimes.insert({name, runtime});
}

Runtime* RuntimeSuite::get(const std::string& name) {
  auto found = runtimes.find(name);
  return found != runtimes.end() ? found->second : 0;
}

void RuntimeSuite::init(cub::ProgramOptions& prog) {
  for (auto it : runtimes) {
    it.second->init(prog);
  }
}

cub::Status RuntimeSuite::config(const RuntimeContext& ctxt) {
  for (auto it : runtimes) {
    CUB_ASSERT_SUCC_CALL(it.second->config(ctxt));
  }
  return cub::Success;
}

}  // namespace serving
}  // namespace adlik
