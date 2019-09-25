#ifndef HA4523AF4_6868_41CD_83FB_71C39087D875
#define HA4523AF4_6868_41CD_83FB_71C39087D875

namespace adlik {
namespace serving {

struct AspiredState {
  AspiredState();

  void unaspired();
  bool wasAspired() const;

private:
  bool aspired;
};

}  // namespace serving
}  // namespace adlik

#endif
