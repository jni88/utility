// By JNI
// Implementation of memory management interface
// The interface can either be a wrap of buildin
// functions (posix_memalign), or
// it can be built from custom-made aligned memory
// allocation functions
// In addition, an application-level memory allocation
// mechanism is developed in order to minimizing
// system call

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
static bool IsAligned(const Align& al, void* ptr) {
  return al <= 1 || JNU_MOD((uintptr_t)ptr, al) == 0;
}
class MMBase {
public:
  virtual void* Malloc(const Align& al, size_t sz) = 0;
  virtual void Free(void* ptr) = 0;
  template<typename T, typename... TA>
  T* New(const Align& al, TA... arg) {
    if (void* ptr = Malloc(al, sizeof(T))) {
      return ::new (ptr) T(arg...);
    }
    return NULL;
  }
  template<typename T, typename... TA>
  T* NewArr(const Align& al, size_t cnt, TA... arg) {
    if (cnt) {
      size_t sz = cnt * sizeof(T);
      if (sz / cnt == sizeof(T)) {
        if (void* ptr = Malloc(al, sz)) {
          return ::new (ptr) T[cnt](arg...);
        }
      }
      return NULL;
    }
  }
  template<typename T>
  void Delete(T* t) {
    if (t) {
      t->~T();
      Free(t);
    }
  }
  template<typename T>
  void DeleteArr(T* t, size_t cnt) {
    if (t) {
      for (size_t i = 0; i < cnt; ++i) {
        t[i].~T();
      }
      Free(t);
    }
  }
protected:
  MMBase() {}
  virtual ~MMBase() {}
};
template<typename C, typename... A>
class MM : public MMBase {
public:
  typedef C Type;
  MM(A... arg)
    : m_mm (arg...) {
  }
  ~MM() {
  }
  void* Malloc(const Align& al, size_t sz) {
    return m_mm.Malloc(al, sz);
  }
  void Free(void* ptr) {
    m_mm.Free(ptr);
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
typedef MM<Buildin> MMBuildin;
typedef MM<Custom<CustomDef>> MMCustomDef;
class MRec {
public:
  MRec() {
    Reset();
  }
  ~MRec() {
    Free();
  }
  MRec(const MRec& r) = delete;
  MRec& operator=(const MRec& r) = delete;
  MRec(MRec& r) {
    Reset();
    *this = (MRec&) r;
  }
  MRec& operator=(MRec& r) {
    if (this != &r) {
      Free();
      m_mm = r.m_mm;
      m_ptr = r.m_ptr;
      m_sz = r.m_sz;
      r.Reset();
    }
    return *this;
  }
  operator bool() const {
    return Ptr() != NULL;
  }
  void* Ptr() const {
    return m_ptr;
  }
  size_t Size() const {
    return m_sz;
  }
  bool IsAligned(const Align& al) const {
    return jnu::memory::IsAligned(al, m_ptr);
  }
  bool Malloc(MMBase* mm, const Align& al, size_t sz) {
    if (!sz) {
      Free();
      return true;
    }
    if (mm) {
      if (void* ptr = mm->Malloc(al, sz)) {
        Free();
        m_mm = mm;
        m_ptr = ptr;
        m_sz = sz;
        return true;
      }
    }
    return false;
  }
  bool Calloc(MMBase* mm, const Align& al, size_t n, size_t sz) {
    if (!n || !sz) {
      Free();
      return true;
    }
    size_t mem_sz = n * sz;
    if ((mem_sz / sz) == n) {
      if (Malloc(mm, al, mem_sz)) {
        memset(m_ptr, 0, mem_sz);
        return true;
      }
    }
    return false;
  }
  bool Realloc(MMBase* mm, const Align& al, size_t sz) {
    if (!sz) {
      Free();
      return true;
    }
    if (sz <= m_sz && IsAligned(al) && mm == m_mm) {
      m_sz = sz;
      return true;
    }
    MRec r;
    if (r.Malloc(mm, al, sz)) {
      memcpy(r.m_ptr, m_ptr, m_sz);
      *this = r;
      return true;
    }
    return false;
  }
  void Free() {
    if (m_mm) {
      m_mm->Free(m_ptr);
      Reset();
    }
  }
private:
  void Reset() {
    m_mm = NULL;
    m_ptr = NULL;
    m_sz = 0;
  }
  MMBase* m_mm;
  void* m_ptr;
  size_t m_sz;
};
}
}

#endif
