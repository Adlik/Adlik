#ifndef HA9D6B92F_3916_4AB0_82DD_CE931400B574
#define HA9D6B92F_3916_4AB0_82DD_CE931400B574

namespace cub {

struct Mutex {
  Mutex();

  void lock();
  bool tryLock();
  void unlock();

  void lockShared();
  bool tryLockShared();
  void unlockShared();

  void* native();

private:
  struct Impl {
    void* space[2];
  };

  Impl mu;
};

}  // namespace cub

#endif
