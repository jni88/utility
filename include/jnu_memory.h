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
class Mem {
public:
  Mem(MMBase* mm = NULL)
    : m_mm (mm) {
    Reset();
  }
  ~Mem() {
    Free();
  }
  Mem(const Mem& r) = delete;
  Mem& operator=(const Mem& r) = delete;
  Mem(Mem&& r) {
    Reset();
    Move(r);
  }
  Mem& operator=(Mem&& r) {
    Move(r);
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
  bool Malloc(const Align& al, size_t sz) {
    return Malloc(m_mm, al, sz);
  }
  bool Calloc(MMBase* mm, const Align& al, size_t n, size_t sz) {
    if (!n || !sz) {
      Free();
      return true;
    }
    size_t mem_sz = n * sz;
    if ((mem_sz / sz) == n && Malloc(mm, al, mem_sz)) {
      memset(m_ptr, 0, mem_sz);
      return true;
    }
    return false;
  }
  bool Calloc(const Align& al, size_t n, size_t sz) {
    return Calloc(m_mm, al, n, sz);
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
    Mem r(mm);
    if (r.Malloc(al, sz)) {
      memcpy(r.m_ptr, m_ptr, m_sz);
      Move(r);
      return true;
    }
    return false;
  }
  void Free() {
    if (m_ptr) {
      m_mm->Free(m_ptr);
      Reset();
    }
  }
  bool Realloc(const Align& al, size_t sz) {
    return Realloc(m_mm, al, sz);
  }
protected:
  MMBase* GetMM() const {
    return m_mm;
  }
  void Move(Mem& r) {
    if (this != &r) {
      Free();
      m_mm = r.m_mm;
      m_ptr = r.m_ptr;
      m_sz = r.m_sz;
      r.Reset();
    }
  }
private:
  void Reset() {
    m_ptr = NULL;
    m_sz = 0;
  }
  MMBase* m_mm;
  void* m_ptr;
  size_t m_sz;
};
template<typename C>
class Obj : private Mem {
public:
  Obj(MMBase* mm = NULL)
    : Mem (mm) {
    Reset();
  }
  ~Obj() {
    Finalize();
  }
  Obj(const Obj& o) = delete;
  Obj& operator=(const Obj& o) = delete;
  Obj(Obj&& o) {
    Reset();
    Move(o);
  }
  Obj& operator=(Obj&& o) {
    Move(o);
    return *this;
  }
  operator bool() const {
    return Size() > 0;
  }
  C* Ptr() const {
    return (C*)Mem::Ptr();
  }
  size_t Size() const {
    return m_sz;
  }
  bool IsAligned(const Align& al) const {
    return Mem::IsAligned(al);
  }
  template<typename... A>
  bool New(MMBase* mm, const Align& al, A... arg) {
    Obj t(mm, 1);
    if (t.Malloc(al, sizeof(C))) {
      ::new (t.Ptr()) C(arg...);
      Move(t);
      return true;
    }
    return false;
  }
  template<typename... A>
  bool New(const Align& al, A... arg) {
    return New(Mem::GetMM(), al, arg...);
  }
  bool NewArr(MMBase* mm, const Align& al, size_t sz) {
    if (DoNewArr(mm, al, sz)) {
      ::new (Ptr()) C[sz];
      return true;
    }
    return false;
  }
  bool NewArr(const Align& al, size_t sz) {
    return NewArr(Mem::GetMM(), al, sz);
  }
  template<typename... A>
  bool NewArr(MMBase* mm, const Align& al, size_t sz, A... arg) {
    if (DoNewArr(mm, al, sz)) {
      for (size_t i = 0; i < sz; ++i) {
        ::new (Ptr() + i) C(arg...);
      }
      return true;
    }
    return false;
  }
  template<typename... A>
  bool NewArr(const Align& al, size_t sz, A... arg) {
    return NewArr(Mem::GetMM(), al, sz, arg...);
  }
  void Delete() {
    Finalize();
    Mem::Free();
  }
private:
  Obj(MMBase* mm, size_t sz)
    : Mem (mm),
      m_sz (sz) {
  }
  void Move(Obj& o) {
    if (this != &o) {
      Finalize();
      Mem::Move(o);
      m_sz = o.m_sz;
      o.Reset();
    }
  }
  void Reset() {
    m_sz = 0;
  }
  void Finalize() {
    size_t sz = m_sz;
    if (sz) {
      Ptr()->~C();
      for (size_t i = 1; i < sz; ++i) {
        Ptr()[i].~C();
      }
    }
  }
  bool DoNewArr(MMBase* mm, const Align& al, size_t sz) {
    if (!sz) {
      Delete();
      return true;
    }
    Obj t(mm, sz);
    size_t mem_sz = sz * sizeof(C);
    if (mem_sz / sz == sizeof(C) && t.Malloc(al, mem_sz)) {
      Move(t);
      return true;
    }
    return false;
  }
  size_t m_sz;
};
}
}

#endif
