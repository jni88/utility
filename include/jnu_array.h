#ifndef JNU_ARRAY_H
#define JNU_ARRAY_H

#include <utility>
#include "jnu_defines.h"
#include "jnu_memory.h"
#include "jnu_iterator.h"

namespace jnu {
class ARR_MEM_ALLOC {
public:
  template<typename T>
  static void Initialize(T* ptr, size_t sz) {
  }
  template<typename T>
  static void Destroy(T* ptr, size_t sz) {
  }
  template<typename T>
  static void Copy(T* dst, const T* src, size_t sz) {
    memmove(dst, src, sz * sizeof(T));
  }
  template<typename T>
  static void Move(T* dst, T* src, size_t sz) {
    Copy<T>(dst, src, sz);
  }
private:
  ARR_MEM_ALLOC() {}
};
class ARR_OBJ_ALLOC {
public:
  template<typename T>
  static void Initialize(T* ptr, size_t sz) {
    ::new (ptr) T[sz];
  }
  template<typename T>
  static void Destroy(T* ptr, size_t sz) {
    for(size_t i = 0; i < sz; ++i) {
      ptr[i].~T();
    }
  }
  template<typename T>
  static void Copy(T* dst, const T* src, size_t sz) {
    if (src > dst) {
      for (size_t i = 0; i < sz; ++i) {
        dst[i] = src[i];
      }
    } else if (src < dst) {
      for (size_t i = sz; i > 0; --i) {
        dst[i - 1] = src[i - 1];
      }
    }
  }
  template<typename T>
  static void Move(T* dst, T* src, size_t sz) {
    if (src > dst) {
      for (size_t i = 0; i < sz; ++i) {
        dst[i] = std::move(src[i]);
      }
    } else if (src < dst) {
      for (size_t i = sz; i > 0; --i) {
        dst[i - 1] = std::move(src[i - 1]);
      }
    }
  }
private:
  ARR_OBJ_ALLOC() {}
};
class ARR_OBJ_MV_ALLOC {
public:
  template<typename T>
  static void Initialize(void* ptr, size_t sz) {
    ARR_OBJ_ALLOC::Initialize<T>(ptr, sz);
  }
  template<typename T>
  static void Destroy(T* ptr, size_t sz) {
    ARR_OBJ_ALLOC::Destroy<T>(ptr, sz);
  }
  template<typename T>
  static void Copy(T* dst, const T* src, size_t sz) {
    ARR_MEM_ALLOC::Copy(dst, src, sz);
  }
  template<typename T>
  static void Move(T* dst, T* src, size_t sz) {
    ARR_MEM_ALLOC::Move(dst, src, sz);
  }
private:
  ARR_OBJ_MV_ALLOC() {}
};
template<typename C>
class ArrayImp : private C {
  typedef typename C::Type T;
  typedef typename C::Alloc A;
public:
  typedef T Type;
  typedef Iterator<T*, (T*)NULL> Iter;
  typedef Iterator<const T*, (T*)NULL> IterC;
  ArrayImp(memory::MMBase* mm = &memory::MM_BUILDIN,
           size_t rsv_sz = 0)
    : C (mm),
      m_sz (0) {
    Reserve(rsv_sz);
  }
  ~ArrayImp() {
    Free();
  }
  ArrayImp(const T* arr, size_t sz)
    : m_sz (0) {
    Copy(arr, sz);
  }
  ArrayImp(const ArrayImp& arr) {
    *this = arr;
  }
  ArrayImp(ArrayImp&& arr) {
    *this = std::move(arr);
  }
  template<typename H>
  ArrayImp& operator=(const H& arr) {
    Copy(arr.Data(), arr.Size());
    return *this;
  }
  ArrayImp& operator=(const ArrayImp& arr) {
    return operator=<ArrayImp>((const ArrayImp&)arr);
  }
  bool Reserve(size_t rsv_sz) {
    return C::Reserve(m_sz, rsv_sz);
  }
  void Free() {
    Clear();
    C::FreeTill(0);
  }
  bool Recycle() {
    return C::FreeTill(m_sz);
  }
  bool Copy(const T* arr, size_t sz) {
    if (!arr || !sz) {
      Clear();
      return true;
    }
    if (Data() != arr) {
      if (!Reserve(sz)) {
        return false;
      }
      A::Copy(Data(), arr, sz);
    }
    if (sz < m_sz) {
      A::Destroy(Data() + sz, m_sz - sz);
    }
    m_sz = sz;
    return true;
  }
  ArrayImp& operator=(ArrayImp&& arr) {
    Move(arr);
    return *this;
  }
  bool Move(ArrayImp& arr) {
    if (this == &arr) {
      return true;
    }
    if (C::Move(m_sz, arr, arr.m_sz)) {
      arr.Clear();
      return true;
    }
    return false;
  }
  bool Move(T* arr, size_t sz) {
    if (!arr || !sz) {
      Clear();
      return true;
    }
    if (Data() != arr) {
      if (!Reserve(sz)) {
        return false;
      }
      A::Move(Data(), arr, sz);
    }
    if (sz < m_sz) {
      A::Destroy(Data() + sz, m_sz - sz);
    }
    m_sz = sz;
    return true;
  }
  template<typename H>
  bool Move(H& arr) {
    bool res = false;
    if (this != &arr) {
      res = Move(arr.Data(), arr.Size());
      arr.Clear();
    }
    return res;
  }
  const T* Data() const {
    return C::Data();
  }
  T* Data() {
    return C::Data();
  }
  size_t Size() const {
    return m_sz;
  }
  operator bool() const {
    return m_sz;
  }
  void Clear() {
    A::Destroy(Data(), m_sz);
    m_sz = 0;
  }
  Iter Expand(const Iter& p, size_t t_sz) {
    size_t sz = m_sz;
    size_t new_sz = sz + t_sz;
    if (new_sz < sz || !Reserve(new_sz)) {
      return Iter();
    }
    T* pos = Adjust(p);
    T* end = End();
    A::Initialize(end, t_sz);
    A::Move(pos + t_sz, pos, end - pos);
    m_sz = new_sz;
    return Iter(pos);
  }
  Iter Insert(const Iter& p, const T& t, size_t t_sz) {
    Iter pos = Expand(p, t_sz);
    if (pos) {
      Iter end = pos + t_sz;
      for (Iter i = pos; i < end; ++i) {
        *i = t;
      }
    }
    return pos;
  }
  Iter Insert(const Iter& p, const T* t, size_t t_sz) {
    Iter pos = Expand(p, t_sz);
    if (pos) {
      A::Copy(pos, t, t_sz);
    }
    return pos;
  }
  template<typename H>
  Iter Insert(const Iter& p, const H& t) {
    return Insert(p, t.Data(), t.Size());
  }
  Iter Inject(const Iter& p, T* t, size_t t_sz) {
    Iter pos = Expand(p, t_sz);
    if (pos) {
      A::Move(pos, t, t_sz);
    }
    return pos;
  }
  template<typename H>
  Iter Inject(const Iter& p, const H& t) {
    Iter pos = Inject(p, t.Data(), t.Size());
    if (pos) {
      t.Clear();
    }
    return pos;
  }
  Iter Delete(const Iter& p, size_t t_sz) {
    T* pos = Adjust(p);
    T* end = End();
    size_t sz = end - pos;
    if (t_sz > sz) {
      t_sz = sz;
    }
    sz -= t_sz;
    A::Move(pos, pos + t_sz, sz);
    A::Destroy(pos + sz, t_sz);
    m_sz -= t_sz;
    return Iter(pos);
  }
  Iter DeleteFlip(const Iter& p, size_t t_sz) {
    T* pos = Adjust(p);
    T* end = End();
    size_t sz = end - pos;
    if (t_sz > sz) {
      t_sz = sz;
    }
    end -= t_sz;
    A::Move(pos, end, t_sz);
    A::Destroy(end, t_sz);
    m_sz -= t_sz;
    return Iter(pos);
  }
  Iter Begin() {
    return Iter(Data());
  }
  IterC Begin() const {
    return IterC(Data());
  }
  Iter End() {
    return Iter(Data() + m_sz);
  }
  IterC End() const {
    return IterC(Data() + m_sz);
  }
  Iter RBegin() {
    return Iter(End() - 1);
  }
  IterC RBegin() const {
    return IterC(End() - 1);
  }
  Iter REnd() {
    return Iter(Begin() - 1);
  }
  IterC REnd() const {
    return IterC(Begin() - 1);
  }
  T& operator[](size_t i) {
    return Data()[i];
  }
  const T& operator[](size_t i) const {
    return Data()[i];
  }
private:
  Iter Adjust(const Iter& p) {
    Iter begin = Begin();
    Iter end = End();
    if (p > begin) {
      if (p < end) {
        return p;
      }
      return end;
    }
    return begin;
  }
  size_t m_sz;
};
template<typename T, size_t S, typename A>
class SArrayDef {
protected:
  typedef T Type;
  typedef A Alloc;
  SArrayDef(memory::MMBase* mm) {
  }
  T* Data() {
    return (T*) m_data;
  }
  const T* Data() const {
    return (T*) m_data;
  }
  bool Reserve(size_t sz, size_t rsv_sz) {
    return rsv_sz <= S;
  }
  bool FreeTill(size_t sz) {
    return false;
  }
  bool Move(size_t& sz, SArrayDef& arr, size_t& a_sz) {
    if (a_sz < sz) {
      A::Destroy(Data() + a_sz, sz - a_sz);
    } else {
      A::Initialize(Data() + sz, a_sz - sz);
    }
    A::Move(Data(), arr.Data(), a_sz);
    sz = a_sz;
    return true;
  }
private:
  char m_data[S * sizeof(T)];
};
template<typename T, typename A, size_t R, memory::Align AL>
class DArrayDef {
  const static size_t ROUND = R ? R : 1;
protected:
  typedef T Type;
  typedef A Alloc;
  DArrayDef(memory::MMBase* mm)
    : m_data (NULL),
      m_mem_sz (0),
      m_mm (mm) {
  }
  T* Data() {
    return m_data;
  }
  const T* Data() const {
    return m_data;
  }
  bool Reserve(size_t sz, size_t rsv_sz) {
    if (rsv_sz <= m_mem_sz) {
      return true;
    }
    if (!m_mm) {
      return false;
    }
    size_t n_sz = rsv_sz * sizeof(T);
    if (n_sz / rsv_sz != sizeof(T)) {
      return false;
    }
    size_t r_sz = Round(n_sz);
    T* ptr = (T*)m_mm->Malloc(AL, r_sz);
    if (!ptr) {
      if (r_sz <= n_sz) {
        return false;
      }
      r_sz = n_sz;
      ptr = (T*)m_mm->Malloc(AL, r_sz);
      if (!ptr) {
        return false;
      }
    }
    A::Initialize(ptr, sz);
    A::Move(ptr, m_data, sz);
    A::Destroy(m_data, sz);
    m_mm->Free(m_data);
    m_data = ptr;
    m_mem_sz = r_sz;
    return true;
  }
  bool FreeTill(size_t sz) {
    if (sz >= m_mem_sz) {
      return true;
    }
    if (!sz) {
      if (m_data) {
        m_mm->Free(m_data);
        m_data = NULL;
        m_mem_sz = 0;
      }
      return true;
    }
    if (T* ptr = (T*)m_mm->Malloc(AL, sz)) {
      A::Initialize(ptr, sz);
      A::Move(ptr, m_data, sz);
      A::Destroy(m_data, sz);
      m_mm->Free(m_data);
      m_data = ptr;
      m_mem_sz = sz;
      return true;
    }
    return false;
  }
  bool Move(size_t& sz, DArrayDef& arr, size_t& a_sz) {
    if (m_data) {
      A::Destroy(m_data, sz);
      m_mm->Free(m_data);
    }
    m_data = arr.m_data;
    m_mem_sz = arr.m_mem_sz;
    m_mm = arr.m_mm;
    sz = a_sz;
    arr.m_data = NULL;
    arr.m_mem_sz = 0;
    a_sz = 0;
    return true;
  }
private:
  size_t Round(size_t sz) {
    if (size_t r = JNU_MOD(sz, ROUND)) {
      size_t n_sz = ROUND - r + sz;
      sz = JNU_MAX(n_sz, sz);
    }
    return sz;
  }
  T* m_data;
  size_t m_mem_sz;
  memory::MMBase* m_mm;
};
template<typename T, size_t S, typename A, size_t R, memory::Align AL>
class HArrayDef : private SArrayDef<T, S, A>,
                  private DArrayDef<T, A, R, AL> {
  typedef SArrayDef<T, S, A> SArr;
  typedef DArrayDef<T, A, R, AL> DArr;
public:
  typedef T Type;
  typedef A Alloc;
  HArrayDef(memory::MMBase* mm)
    : SArr (mm),
      DArr (mm) {
  }
  T* Data() {
    return DArr::Data() ? DArr::Data() : SArr::Data();
  }
  const T* Data() const {
    return DArr::Data() ? DArr::Data() : SArr::Data();
  }
  bool Reserve(size_t sz, size_t rsv_sz) {
    if (DArr::Data()) {
      return DArr::Reserve(sz, rsv_sz);
    }
    if (SArr::Reserve(sz, rsv_sz)) {
      return true;
    }
    if (DArr::Reserve(0, rsv_sz)) {
      A::Initialize(DArr::Data(), sz);
      A::Move(DArr::Data(), SArr::Data(), sz);
      A::Destroy(SArr::Data(), sz);
      return true;
    }
    return false;
  }
  bool FreeTill(size_t sz) {
    if (!DArr::Data()) {
      return SArr::FreeTill(sz);
    }
    if (sz > S) {
      return DArr::FreeTill(sz);
    }
    A::Initialize(SArr::Data(), sz);
    A::Move(SArr::Data(), DArr::Data(), sz);
    return DArr::FreeTill(0);
  }
  bool Move(size_t& sz, HArrayDef& arr, size_t& a_sz) {
    if (!DArr::Data()) {
      if (!arr.DArr::Data()) {
        return SArr::Move(sz, (SArr&) arr, a_sz);
      }
      A::Destroy(SArr::Data(), sz);
      return DArr::Move(sz, (DArr&) arr, a_sz);
    }
    if (arr.DArr::Data()) {
      return DArr::Move(sz, (DArr&) arr, a_sz);
    }
    A::Destroy(DArr::Data(), sz);
    DArr::FreeTill(0);
    sz = 0;
    return SArr::Move(sz, (SArr&) arr, a_sz);
  }
};
template<typename T, size_t S, typename A>
using SArray = ArrayImp<SArrayDef<T, S, A>>;
template<typename T, typename A, size_t R, memory::Align AL = 8>
using DArray = ArrayImp<DArrayDef<T, A, R, AL>>;
template<typename T, size_t S, typename A, size_t R, memory::Align AL = 8>
using HArray = ArrayImp<HArrayDef<T, S, A, R, AL>>;
}

#endif
