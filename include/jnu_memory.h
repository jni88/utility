#ifndef JNU_MEMORY_H
#define JNU_MEMORY_H

namespace jnu {
namespace memory {
typedef size_t Align;
class Standard {
public:
  static void* Malloc(const Align& al, size_t sz);
  static void Free(void* ptr);
private:
  Standard() {}
};
class Custom {
  typedef size_t SZ_T;
public:
  static void* Malloc(const Align& al, size_t sz);
  static void Free(void* ptr);
private:
  Custom() {}
};
}
}

#endif
