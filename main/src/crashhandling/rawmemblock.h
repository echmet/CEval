#ifndef RAWMEMBLOCK_H
#define RAWMEMBLOCK_H

#ifdef CRASHHANDLING_LINUX

#include <cstdint>
#include <unistd.h>
#include <sys/mman.h>
#include <syscall.h>

template <typename PtrType>
class RawMemBlock {
private:
  inline void * sys_mmap(void *start, size_t len, int prot, int flags, int fd, off_t off)
  {
    static const size_t UNIT = 4096ULL;
    static const off_t OFF_MASK ((-0x2000ULL << (8*sizeof(long)-1)) | (UNIT-1));

    if (off & OFF_MASK) {
      return MAP_FAILED;
    }
    if (len >= PTRDIFF_MAX) {
      return MAP_FAILED;
    }

  #ifdef __NR_mmap2
    return (void *)syscall(__NR_mmap2, start, len, prot, flags, fd, off/UNIT);
  #else
    return (void *)syscall(__NR_mmap, start, len, prot, flags, fd, off);
  #endif
  }

  size_t mapSize() const
  {
    return m_NPages * m_pageSize;
  }

  size_t pagesNeeded(const int bytes)
  {
    return (bytes % m_pageSize == 0) ? (bytes / m_pageSize) : (bytes / m_pageSize) + 1;
  }

  PtrType *m_mem;
  const size_t m_pageSize;
  const size_t m_NPages;

public:
  explicit RawMemBlock() :
    m_mem(nullptr),
    m_pageSize(sysconf(_SC_PAGESIZE)),
    m_NPages(0)
  {
  }

  explicit RawMemBlock(const size_t bytes) :
    m_mem(nullptr),
    m_pageSize(sysconf(_SC_PAGESIZE)),
    m_NPages(pagesNeeded(bytes))
  {
    void *m = sys_mmap(nullptr, mapSize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (m != MAP_FAILED)
      m_mem = static_cast<PtrType *>(m);
  }

  explicit RawMemBlock(const size_t bytes, const int backingFd) :
    m_mem(nullptr),
    m_pageSize(sysconf(_SC_PAGESIZE)),
    m_NPages(pagesNeeded(bytes))
  {
    ftruncate(backingFd, mapSize());
    void *m = sys_mmap(nullptr, mapSize(), PROT_READ | PROT_WRITE, MAP_SHARED, backingFd, 0);
    if (m != MAP_FAILED)
      m_mem = static_cast<PtrType *>(m);
  }

  ~RawMemBlock()
  {
    if (m_mem != nullptr)
      munmap(m_mem, mapSize());
  }

  bool allocate(const size_t bytes)
  {
    if (m_mem != nullptr)
      return false;

    const_cast<size_t&>(m_NPages) = pagesNeeded(bytes);

    void *m = sys_mmap(nullptr, mapSize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (m != MAP_FAILED)
      m_mem = static_cast<PtrType *>(m);

    return m_mem != nullptr;
  }

  bool allocate(const size_t bytes, const int backingFd)
  {
    if (m_mem != nullptr)
      return false;

    const_cast<size_t&>(m_NPages) = pagesNeeded(bytes);

    void *m = sys_mmap(nullptr, mapSize(), PROT_READ | PROT_WRITE, MAP_SHARED, backingFd, 0);
    if (m != MAP_FAILED)
      m_mem = static_cast<PtrType *>(m);

    return m_mem != nullptr;
  }

  bool ok()
  {
    return m_mem != nullptr;
  }

  PtrType *mem() const
  {
    return m_mem;
  }
};


#endif // CRASHHANDLING_LINUX

#endif // RAWMEMBLOCK_H
