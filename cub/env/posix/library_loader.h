#ifndef HCDEB97D2_E132_493B_85E9_A57AA728C6E2
#define HCDEB97D2_E132_493B_85E9_A57AA728C6E2

namespace cub {

struct LibraryLoader {
  explicit LibraryLoader(const char* name);
  void* getSymbol(const char* symbol) const;

private:
  void* handle = nullptr;
};

}  // namespace cub

#endif
