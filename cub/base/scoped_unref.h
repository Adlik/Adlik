#ifndef H7DD81110_AD3E_4547_A8EB_C93C5C4E753D
#define H7DD81110_AD3E_4547_A8EB_C93C5C4E753D

namespace cub {

struct RefCounter;

struct ScopedUnref {
  explicit ScopedUnref(RefCounter* obj);
  ~ScopedUnref();

private:
  RefCounter* obj;
};

}  // namespace cub

#endif
