#ifndef H05B2224D_B926_4FC0_A123_97B52B8A99DB
#define H05B2224D_B926_4FC0_A123_97B52B8A99DB

#include <type_traits>

namespace cub {

template <typename T>
struct Placement {
  Placement& operator=(const T& rhs) {
    assignBy(rhs);
    return *this;
  }

  void* alloc() {
    return (void*)&u;
  }

  void free() {
    getObject()->~T();
  }

  const T* operator->() const {
    return getObject();
  }

  T* operator->() {
    return getObject();
  }

  const T& operator*() const {
    return getRef();
  }

  T& operator*() {
    return getRef();
  }

  const T* getObject() const {
    return static_cast<const T*>(static_cast<const void*>(&u));
  }

  T* getObject() {
    return static_cast<T*>(static_cast<void*>(&u));
  }

  const T& getRef() const {
    return *getObject();
  }

  T& getRef() {
    return *getObject();
  }

  void destroy() {
    getObject()->~T();
  }

private:
  void assignBy(const T& rhs) {
    auto p = static_cast<T*>(alloc());
    *p = rhs;
  }

private:
  typename std::aligned_storage<sizeof(T), alignof(T)>::type u;
};

template <typename T>
struct DefaultPlacement : Placement<T> {
  T* init() {
    return new (Placement<T>::alloc()) T();
  }
};

}  // namespace cub

#endif
