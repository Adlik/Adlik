#include "cub/env/posix/posix_filesystem.h"
#include "cub/base/assertions.h"
#include "cub/env/fs/path.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace cub {

namespace {
struct PosixReadOnlyRegion : public ReadOnlyRegion {
  PosixReadOnlyRegion(void* addr, uint64_t len) : addr(addr), len(len) {
  }

  ~PosixReadOnlyRegion() override {
    munmap(addr, len);
  }

private:
  OVERRIDE(const void* data() const) {
    return addr;
  }

  OVERRIDE(uint64_t size() const) {
    return len;
  }

private:
  void* addr;
  uint64_t len;
};

struct ReadOnlyRegionMaker {
  ReadOnlyRegionMaker(const std::string& fname)
    : fd(open(fname.c_str(), O_RDONLY)) {
  }

  ReadOnlyRegion* make() {
    return isValid() ? alloc() : nullptr;
  }

  ~ReadOnlyRegionMaker() {
    if (isValid()) {
      close(fd);
    }
  }

private:
  bool isValid() const {
    return fd >= 0;
  }

  ReadOnlyRegion* alloc() {
    struct stat st;
    ::fstat(fd, &st);
    return alloc(st.st_size);
  }

  ReadOnlyRegion* alloc(size_t size) {
    auto addr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    return addr == MAP_FAILED ? nullptr : new PosixReadOnlyRegion(addr, size);
  }

private:
  int fd;
};
}  // namespace

ReadOnlyRegion* PosixFileSystem::mmap(const std::string& fname) const {
  ReadOnlyRegionMaker maker(fname);
  return maker.make();
}

namespace {
struct DirectReader {
  DirectReader(const std::string& dir) : root(dir), dir(opendir(dir.c_str())) {
  }

  Status read(DirentVisitor& visitor) const {
    CUB_ASSERT_VALID_PTR(dir);
    while (auto entry = readdir(dir)) {
      visitChild(entry->d_name, visitor);
    }
    return cub::Success;
  }

  ~DirectReader() {
    if (dir != nullptr) {
      closedir(dir);
    }
  }

private:
  void visitChild(const std::string& baseName, DirentVisitor& visitor) const {
    if (baseName != "." && baseName != "..") {
      visitor.visit(root, baseName);
    }
  }

private:
  std::string root;
  DIR* dir;
};
}  // namespace

Status PosixFileSystem::children(const std::string& dir, DirentVisitor& visitor)
    const {
  DirectReader reader(dir);
  return reader.read(visitor);
}

bool PosixFileSystem::exists(const std::string& fname) const {
  return access(fname.c_str(), F_OK) == 0;
}

}  // namespace cub
