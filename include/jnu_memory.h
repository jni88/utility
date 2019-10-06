#ifndef JNU_MEMORY_H
#define JNU_MEMORY_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <new>
#include "jnu_defines.h"

namespace jnu {
namespace memory {
typedef size_t Align;
template<typename C, typename... A>
class MM {
public:
  typedef C Type;
  MM(A... arg)
    : m_mm (arg...) {
  }
  void* Malloc(const Align& al, size_t sz) {
    return m_mm.Malloc(al, sz);
  }
  void* Calloc(const Align& al, size_t n, size_t sz) {
    if (!n || !sz) {
      return NULL;
    }
    size_t mem_sz = n * sz;
    if ((mem_sz / sz) < n) {
      return NULL;
    }
    if (void* ptr = Malloc(al, mem_sz)) {
      memset(ptr, 0, mem_sz);
      return ptr;
    }
    return NULL;
  }
  void* Realloc(const Align& al, void* ptr,
                size_t old_sz, size_t new_sz) {
    if (!ptr) {
      return Malloc(al, new_sz);
    }
    if (!new_sz) {
      Free(ptr);
      return NULL;
    }
    if (new_sz > old_sz) {
      if (void* new_ptr = Malloc(al, new_sz)) {
        memcpy(new_ptr, ptr, old_sz);
        ptr = new_ptr;
      } else {
        return NULL;
      }
    }
    return ptr;
  }
  void Free(void* ptr) {
    m_mm.Free(ptr);
  }
  template<typename T, typename... TA>
  T* New(Align al, TA... arg) {
    if (void* ptr = Malloc(al, sizeof(T))) {
      return new (ptr) T(arg...);
    }
    return NULL;
  }
  template<typename T>
  void Delete(T* t) {
    if (t) {
      t->~T();
      Free(t);
    }
  }
  C& GetImp() {
    return m_mm;
  }
private:
  C m_mm;
};
class Buildin {
  template<typename C, typename... A> friend class MM;
public:
  static void* Malloc(const Align& al, size_t sz) {
    if (!sz || !JNU_IS_POW_2(al)) {
      return NULL;
    }
    if (al <= 1) {
      return malloc(sz);
    }
    void* ptr;
    if (posix_memalign(&ptr, al < JNU_PTR_SZ ? JNU_PTR_SZ : al, sz) != 0) {
      return NULL;
    }
    return ptr;
  }
  static void Free(void* ptr) {
    free(ptr);
  }
private:
  Buildin() {}
};
template<typename C, typename... A>
class Custom {
  typedef size_t SZ_T;
public:
  Custom(A... arg)
    : m_imp(arg...) {
  }
  ~Custom() {
  }
  void* Malloc(const Align& al, size_t sz) {
    if (!sz || !JNU_IS_POW_2(al)) {
      return NULL;
    }
    Align r_a = al == 0 ? JNU_PTR_SZ : al;
    size_t mem_sz = r_a + sizeof(SZ_T);
    if (mem_sz < r_a) {
      return NULL;
    }
    mem_sz += sz;
    if (mem_sz < sz) {
      return NULL;
    }
    if (char* ptr = (char*)m_imp.Malloc(mem_sz)) {
      char* t = ptr + sizeof(SZ_T);
      t += (al - JNU_MOD((uintptr_t)t, r_a));
      *((SZ_T*)(t - sizeof(SZ_T))) = (SZ_T)(t - ptr);
      return t;
    }
    return NULL;
  }
  void Free(void* ptr) {
    if (ptr) {
      SZ_T offset = *((SZ_T*)((char*)ptr - sizeof(SZ_T)));
      m_imp.Free((char*)ptr - offset);
    }
  }
private:
  C m_imp;
};
class CustomDef {
public:
  static void* Malloc(size_t sz) {
    return malloc(sz);
  }
  static void Free(void* ptr) {
    free(ptr);
  }
};
}
}

#endif
