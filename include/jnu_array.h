#ifndef JNU_ARRAY_H
#define JNU_ARRAY_H

#include <utility>
#include "jnu_memory.h"
#include "jnu_iterator.h"

namespace jnu {
class ARR_MEM_ALLOC {
public:
  template<typename T>
  static T* Initialize(void* ptr, size_t sz) {
    return (T*) ptr;
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
  static T* Initialize(void* ptr, size_t sz) {
    return ::new (ptr) T[sz];
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
  static T* Initialize(void* ptr, size_t sz) {
    return ARR_OBJ_ALLOC::Initialize<T>(ptr, sz);
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
template<typename T, typename A, memory::Align AL>
class ArrayHelp {
public:
  const static size_t TYPE_SZ = sizeof(T);
  const static size_t MAX_SZ = (size_t)(-1);
  typedef T Type;
  typedef Iterator<size_t> Iter;
  static void Copy(T* dst, size_t& d_sz,
                   const T* src, size_t s_sz) {
    A::Copy(dst, src, s_sz);
    if (s_sz < d_sz) {
      A::Destroy(dst + s_sz, d_sz - s_sz);
    }
    d_sz = s_sz;
  }
  static void Move(T* dst, size_t& d_sz,
                   T* src, size_t s_sz) {
    A::Move(dst, src, s_sz);
    if (s_sz < d_sz) {
      A::Destroy(dst + s_sz, d_sz - s_sz);
    }
    d_sz = s_sz;
  }
};
template<typename T, size_t S,
         typename A = ARR_MEM_ALLOC,
         memory::Align AL = 8>
class ArrayS : public ArrayHelp <T, A, AL> {
  typedef ArrayHelp <T, A, AL> Help;
public:
  ArrayS()
    : m_sz (0) {
  }
  ArrayS(const T* arr, size_t sz)
    : m_sz (0) {
    Copy(arr, sz);
  }
  ArrayS(const ArrayS& arr) {
    *this = arr;
  }
  ArrayS(ArrayS&& arr) {
    *this = std::move(arr);
  }
  template<typename C>
  ArrayS& operator=(const C& arr) {
    Copy(arr.Data(), arr.Size());
    return *this;
  }
  ArrayS& operator=(const ArrayS& arr) {
    return operator=<ArrayS>((const ArrayS&)arr);
  }
  bool Copy(const T* arr, size_t sz) {
    if (arr && sz <= S) {
      Help::Copy(m_data, m_sz, arr, sz);
      return true;
    }
    return false;
  }
  ArrayS& operator=(ArrayS&& arr) {
    Move(arr);
    return *this;
  }
  bool Move(T* arr, size_t sz) {
    if (arr && sz <= S) {
      Help::Move(m_data, m_sz, arr, sz);
      return true;
    }
    return false;
  }
  template<typename C>
  bool Move(C& arr) {
    bool res = false;
    if (this != &arr) {
      res = Move(arr.Data(), arr.Size());
      arr.Clear();
    }
    return res;
  }
  const T* Data() const {
    return m_data;
  }
  T* Data() {
    return m_data;
  }
  size_t Size() const {
    return m_sz;
  }
  void Clear() {
    A::Destroy(m_data, m_sz);
  }
private:
  size_t m_sz;
  T m_data[S];
};
}

#endif
