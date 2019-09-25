#ifndef H329FAAFF_56EB_4BC6_AD7D_189810BB7798
#define H329FAAFF_56EB_4BC6_AD7D_189810BB7798

#include "adlik_serving/framework/domain/aspired_state.h"
#include "adlik_serving/framework/domain/model_id.h"
#include "cub/base/status.h"

namespace adlik {
namespace serving {

struct BareModelLoader;

struct ModelState : ModelId, AspiredState {
  ModelState() = default;
  ModelState(const ModelId&);

  bool untouched() const;
  bool ready() const;
  bool finished() const;
  bool appeared() const;
  bool idle() const;
  bool healthy() const;
  bool shouldStop() const;

  cub::Status loadBy(BareModelLoader&);
  cub::Status unloadBy(BareModelLoader&);

  const char* str() const;

private:
  enum State {
    NEW,
    LOADING,
    READY,
    UNLOADING,
    DISABLED,
    ERROR,
  } state = NEW;

private:
  cub::Status error(cub::Status);
  cub::Status ok(State to);

  cub::Status transfer(State from, State to);
  cub::Status transfer(State from, State to, cub::Status);
};

}  // namespace serving
}  // namespace adlik

#endif
