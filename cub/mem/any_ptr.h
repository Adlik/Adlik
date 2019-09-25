#ifndef H445BD36A_04BE_484C_8055_23E88A8DB055
#define H445BD36A_04BE_484C_8055_23E88A8DB055

#include <cstddef>

namespace cub {

struct AnyPtr {
  AnyPtr() noexcept : id(TypeId<void>()), p(nullptr) {
  }

  AnyPtr(std::nullptr_t) noexcept : AnyPtr() {
  }

  template <typename T>
  AnyPtr(T* ptr) noexcept : id(TypeId<T>()), p(const_cast<void*>(reinterpret_cast<const void*>(ptr))) {
  }

  template <typename T>
  T* get() const noexcept {
    return id == TypeId<T>() ? reinterpret_cast<T*>(p) : nullptr;
  }

private:
  // TypeId is different for a const and non-const.
  template <typename Type>
  static size_t TypeId() {
    static int dummy;
    return reinterpret_cast<std::size_t>(&dummy);
  }

private:
  std::size_t id;
  void* p;
};

}  // namespace cub

#endif
