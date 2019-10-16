#ifndef JNU_ARRAY_H
#define JNU_ARRAY_H

#include <utility>
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
template<typename C, typename A>
class ArrayImp : private C {
  typedef typename C::Type T;
public:
  typedef T Type;
  typedef Iterator<size_t> Iter;
  constexpr static size_t MAX_SZ = -1;
  constexpr static size_t NO_POS = MAX_SZ;
  ArrayImp(size_t rsv_sz = 0)
    : m_sz (0) {
    Reserve(rsv_sz);
  }
  ~ArrayImp() {
    Clear();
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
    return C::Reserve(rsv_sz);
  }
  bool Copy(const T* arr, size_t sz) {
    if (!arr) {
      sz = 0;
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
  bool Move(T* arr, size_t sz) {
    if (!arr) {
      sz = 0;
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
    size_t start = p < m_sz ? (size_t) p : m_sz;
    if (t_sz) {
      size_t sz = m_sz;
      size_t new_sz = sz + t_sz;
      if (new_sz <= sz || !C::Reserve(new_sz)) {
        return Iter(NO_POS);
      }
      A::Initialize(Data() + sz, t_sz);
      A::Copy(Data() + start + t_sz, Data() + start, sz - start);
      m_sz = new_sz;
    }
    return Iter(start);
  }
  bool Insert(const Iter& p, const T& t, size_t t_sz) {
    Iter start = Expand(p, t_sz);
    if (start != NO_POS) {
      for (size_t i = start; i < start + t_sz; ++i) {
        Data()[i] = t;
      }
      return true;
    }
    return false;
  }
  bool Insert(const Iter& p, const T* t, size_t t_sz) {
    Iter start = Expand(p, t_sz);
    if (start != NO_POS) {
      A::Copy(Data() + start, t, t_sz);
      return true;
    }
    return false;
  }
  template<typename H>
  bool Insert(const Iter& p, const H& t) {
    return Insert(p, t.Data(), t.Size());
  }
  bool Inject(const Iter& p, T* t, size_t t_sz) {
    Iter start = Expand(p, t_sz);
    if (start != NO_POS) {
      A::Move(Data() + start, t, t_sz);
      return true;
    }
    return false;
  }
  template<typename H>
  bool Inject(const Iter& p, const H& t) {
    if (Inject(p, t.Data(), t.Size())) {
      t.Clear();
      return true;
    }
    return false;
  }
  static Iter Begin() {
    return Iter(0);
  }
  Iter End() const {
    return Iter(m_sz);
  }
private:
  size_t m_sz;
  T m_data;
};
template<typename T, size_t S>
class StaticArr {
  template<typename C, typename A> friend class ArrayImp;
  typedef T Type;
  T* Data() {
    return (T*) m_data;
  }
  const T* Data() const {
    return (T*) m_data;
  }
  bool Reserve(size_t rsv_sz) {
    return rsv_sz <= S;
  }
  char m_data[S * sizeof(T)];
};
template<typename T, size_t S, typename A>
using ArrayS = ArrayImp<StaticArr<T, S>, A>;
}

#endif
