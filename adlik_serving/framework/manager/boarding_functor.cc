// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/manager/boarding_functor.h"

#include <algorithm>
#include <set>
#include <vector>

#include "adlik_serving/framework/domain/model_stream.h"
#include "adlik_serving/framework/manager/managed_store.h"
#include "cub/base/assertions.h"
#include "cub/base/optional.h"
#include "cub/log/log.h"

namespace adlik {
namespace serving {

namespace {
struct ModelTerminator : ManagedModelVisitor {
  ModelTerminator() {
  }
  void stop(ManagedStore& store) {
    for (auto model : models) {
      CUB_PEEK_SUCC_CALL(store.stop(*model));
    }
  }

private:
  OVERRIDE(void visit(const Model& model)) {
    if (model.shouldStop()) {
      models.push_back(&model);
    }
  }

private:
  std::vector<const Model*> models;
};
}  // namespace

void BoardingFunctor::flush() {
  ModelTerminator terminator;
  store.models(terminator);
  terminator.stop(store);
}

namespace {
struct ModelAspirer : ManagedModelVisitor {
  ModelAspirer(const ModelStream& stream, ManagedStore& store) : stream(stream), store(store) {
  }

  void handle() {
    unaspired();
    aspired();
  }

private:
  void unaspired() {
    for (auto model : models) {
      if (!stream.contains(model->getVersion())) {
        store.unaspired(*model);
      }
    }
  }

  void aspired() {
    auto adding = [this](auto version) {
      if (!this->exist(version)) {
        store.start({stream.getName(), version});
      }
    };
    stream.versions(adding);
  }

  bool exist(int version) const {
    return std::find_if(models.cbegin(), models.cend(), [version](auto model) {
             return model->wasAspired() && model->getVersion() == version;
           }) != models.cend();
  }

private:
  OVERRIDE(void visit(const Model& model)) {
    models.push_back(&model);
  }

private:
  const ModelStream& stream;
  ManagedStore& store;
  std::vector<const Model*> models;
};
}  // namespace

void BoardingFunctor::handleOne(const ModelStream& stream) {
  ModelAspirer aspirer(stream, store);
  store.models(stream.getName(), aspirer);
  aspirer.handle();
}

namespace {
struct ShouldSkip : ManagedModelVisitor {
  ShouldSkip(const ModelStream& stream) : shouldSkip(false), stream(stream) {
  }

  bool operator()() const {
    return shouldSkip;
  }

private:
  OVERRIDE(void visit(const Model& model)) {
    if (!model.wasAspired() && stream.contains(model.getVersion())) {
      shouldSkip = true;
    }
  }

private:
  bool shouldSkip;
  const ModelStream& stream;
};
}  // namespace

bool BoardingFunctor::shouldSkip(const ModelStream& stream) const {
  ShouldSkip pred(stream);
  store.models(stream.getName(), pred);
  return pred();
}

void BoardingFunctor::handle(std::vector<ModelStream>& streams) {
  for (auto i = streams.cbegin(); i != streams.cend();) {
    if (shouldSkip(*i)) {
      ++i;
    } else {
      handleOne(*i);
      i = streams.erase(i);
    }
  }
}

namespace {
class ModelAction {
  enum Action { LOAD, UNLOAD };

public:
  void exec(ManagedStore& store) {
    if (action) {
      action->exec(store);
    }
  }

  static ModelAction load(const ModelId& id) {
    return {id, LOAD};
  }

  static ModelAction unload(const ModelId& id) {
    return {id, UNLOAD};
  }

  static ModelAction nop() {
    return {};
  }

  bool operator<(const ModelAction& rhs) const {
    if (!action) {
      return false;
    } else if (!rhs.action) {
      return true;
    } else {
      return action.value() < rhs.action.value();
    }
  }

private:
  struct ActionImpl {
    ActionImpl(const ModelId& id, Action action) : id(id), action(action) {
    }

    void exec(ManagedStore& store) {
      INFO_LOG << "decide load(0)/unload(1): " << action;
      switch (action) {
        case LOAD:
          CUB_ASSERT_SUCC_CALL_VOID(store.load(id));
          break;
        case UNLOAD:
          CUB_ASSERT_SUCC_CALL_VOID(store.unload(id));
          break;
        default:
          break;
      }
    }

    bool operator<(const ActionImpl& rhs) const {
      if (action == UNLOAD) {
        return true;
      } else if (rhs.action == UNLOAD) {
        return false;
      } else {
        return true;
      }
    }

  private:
    ModelId id;
    Action action;
  };

  ModelAction(const ModelId& id, Action action) : action(cub::inplace, id, action) {
  }

  ModelAction() = default;

private:
  cub::Optional<ActionImpl> action;
};

class ActionMaker : public ManagedModelVisitor {
  using Models = std::vector<const Model*>;
  Models models;

public:
  std::vector<ModelAction> getActions() {
    std::vector<ModelAction> actions;
    Models unaspiredModels;
    if (canUnload(unaspiredModels)) {
      for (auto unaspired : unaspiredModels) {
        actions.emplace_back(std::move(ModelAction::unload(*unaspired)));
      }
    }
    Models untouchModels = untouches(models);
    for (auto untouch : untouchModels) {
      actions.emplace_back(std::move(ModelAction::load(*untouch)));
    }
    return actions;
  }

private:
  bool canUnload(Models& unaspireds) {
    bool hasAspired = false, hasServing = false;
    for (auto model : models) {
      if (model->wasAspired()) {
        hasAspired = true;
        if (model->ready()) {
          hasServing = true;
        }
      } else if (model->ready()) {
        unaspireds.push_back(model);
      }
    };
    return !hasAspired || hasServing || unaspireds.size() > 0;
  }

  template <typename F>
  static cub::Optional<ModelId> minmax(const Models& models, F f) {
    auto i =
        f(models.cbegin(), models.cend(), [](auto lhs, auto rhs) { return lhs->getVersion() < rhs->getVersion(); });
    if (i != models.cend()) {
      return **i;
    } else {
      return cub::nilopt;
    }
  }

  static cub::Optional<ModelId> lowest(const Models& models) {
    return minmax(models, [](auto first, auto last, auto cmp) { return std::min_element(first, last, cmp); });
  }

  static cub::Optional<ModelId> highest(const Models& models) {
    return minmax(models, [](auto first, auto last, auto cmp) { return std::max_element(first, last, cmp); });
  }

  static Models untouches(const Models& models) {
    Models result;
    for (auto model : models) {
      if (model->untouched() && model->wasAspired()) {
        result.push_back(model);
      }
    }
    return result;
  }

private:
  OVERRIDE(void visit(const Model& model)) {
    models.push_back(&model);
  }
};

struct ModelExecutor : ManagedNameVisitor {
  ModelExecutor(ManagedStore& store) : store(store) {
  }

  void exec() {
    makeActions();
    for (auto action : actions) {
      action.exec(store);
    }
  }

private:
  void makeActions() {
    ActionMaker maker;
    store.models(maker);
    actions = std::move(maker.getActions());
    std::sort(actions.begin(), actions.end());
  }

private:
  OVERRIDE(void visit(const std::string& name)) {
    names.push_back(name);
  }

private:
  ManagedStore& store;
  std::vector<std::string> names;
  std::vector<ModelAction> actions;
};
}  // namespace

BoardingFunctor::BoardingFunctor(ManagedStore& store) : store(store) {
}

void BoardingFunctor::execute() {
  ModelExecutor executor(store);
  store.names(executor);
  executor.exec();
}

void BoardingFunctor::operator()(std::vector<ModelStream>& streams) {
  handle(streams);
  execute();
  flush();
}

}  // namespace serving
}  // namespace adlik
