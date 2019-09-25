#ifndef H53DE2AB9_4BEA_4E6C_BE94_B8CF6B180019
#define H53DE2AB9_4BEA_4E6C_BE94_B8CF6B180019

#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct Model;
struct ModelId;
struct ModelConfig;

DEFINE_ROLE(ModelFactory) {
  ABSTRACT(Model * create(const ModelId&, const ModelConfig&));
};

}  // namespace serving
}  // namespace adlik

#endif
