#ifndef JNU_SET_H
#define JNU_SET_H

#include <utility>
#include <functional>
#include "jnu_memory.h"

namespace jnu {
template<typename C, typename K,
         const K& (C::Type::*F)() const,
         auto LESS = (bool (*) (const K&, const K&)) NULL>
class SetT {
  template<typename H, typename R>
  using IsClass = typename std::enable_if
                  <std::is_class<H>::value, R>::type;
  template<decltype(LESS) L>
  typename std::enable_if<L == NULL, bool>::type
  static compare(const K& a, const K& b) {
    return a < b;
  }
  template<decltype(LESS) L>
  typename std::enable_if<L != NULL, bool>::type
  static compare(const K& a, const K& b) {
    return L(a, b);
  }
  typedef typename C::Type T;
public:
  typedef T Type;
  typedef K Key;
  typedef typename C::Iter Iter;
  class Res {
    friend class SetT;
  public:
    Iter& operator*() {
      return m_it;
    }
    const Iter& operator*() const {
      return m_it;
    }
    operator bool() const {
      return (bool) m_it;
    }
    bool Inserted() const {
      return !m_found;
    }
    bool Found() const {
      return m_found;
    }
  private:
    Res()
      : m_found (false) {
    }
    Res(const Iter& it, bool found)
      : m_it (it),
        m_found (found) {
    }
    Iter m_it;
    bool m_found;
  };
  SetT(size_t rsv_sz = 0,
      memory::MMBase* mm = &memory::MM_BUILDIN)
    : m_data (rsv_sz, mm) {
  }
  ~SetT() {
  }
  SetT(const SetT& s) {
    *this = s;
  }
  SetT(SetT&& s) {
    *this = std::move(s);
  }
  SetT& operator=(const SetT& s) {
    m_data = s.m_data;
    return *this;
  }
  SetT& operator==(SetT&& s) {
    m_data = std::move(s.m_data);
    return *this;
  }
  bool Reserve(size_t rsv_sz) {
    return m_data.Reserve(rsv_sz);
  }
  void Free() {
    m_data.Free();
  }
  void Clear() {
    m_data.Clear();
  }
  bool Recycle() {
    return m_data.Recycle();
  }
  size_t Size() const {
    return m_data.Size();
  }
  bool IsEmpty() const {
    return Size() <= 0;
  }
  operator bool() const {
    return Size() > 0;
  }
  C& Data() const {
    return m_data;
  }
  Iter Begin() const {
    return m_data.Begin();
  }
  Iter End() const {
    return m_data.End();
  }
  Iter RBegin() const {
    return m_data.RBegin();
  }
  Iter REnd() const {
    return m_data.REnd();
  }
  template<typename... Hints>
  bool Insert(const T* t, size_t t_sz, Hints... hints) {
    return Add<const T, Hints...>(t, t_sz, &C::Insert, hints...);
  }
  template<typename... Hints>
  bool Insert(const T* t, const T* t_end, Hints... hints) {
    return Insert(t, Distance(t, t_end), hints...);
  }
  template<typename... Hints>
  Res Insert(const T& t, Hints... hints) {
    return Insert(&t, 1, hints...);
  }
  template<typename H, typename... Hints>
  IsClass<H, bool> Insert(const H& arr, Hints... hints) {
    return Insert(arr.Data(), arr.Size(), hints...);
  }
  template<typename... Hints>
  bool Inject(T* t, size_t t_sz, Hints... hints) {
    return Add<T, Hints...>(t, t_sz, &C::Inject, hints...);
  }
  template<typename... Hints>
  bool Inject(T* t, T* t_end, Hints... hints) {
    return Inject(t, Distance(t, t_end), hints...);
  }
  template<typename... Hints>
  bool Inject(T& t, Hints... hints) {
    return Inject(&t, 1, hints...);
  }
  template<typename H, typename... Hints>
  IsClass<H, bool> Inject(H& arr, Hints... hints) {
    if (Inject(arr.Data(), arr.Size(), hints...)) {
      arr.Clear();
      return true;
    }
    return false;
  }
  template<typename H, typename... Hints>
  bool InjectRange(H& arr, T* t, size_t t_sz, Hints... hints) {
    t = arr.Adjust(t, t_sz);
    if (Inject(t, t_sz, hints...)) {
      arr.Delete(t, t_sz);
      return true;
    }
    return false;
  }
  template<typename H, typename... Hints>
  bool InjectRange(H& arr, T* t, T* t_end, Hints... hints) {
    return InjectRange(arr, t, Distance(t, t_end), hints...);
  }
  template<typename... Hints>
  Iter Find(const T& t, Hints... hints) {
    Res r = Locate((t.*F)(), hints...);
    return r.Found() ? *r : Iter();
  }
private:
  size_t Distance(const T* s, const T* e) {
    return s < e ? (size_t) (e - s) : 0;
  }
  template<typename H, typename... Hints>
  Res Add(H* t, size_t t_sz,
          Iter (C::*insert)(const Iter&, H*, size_t),
          Hints... hints) {
    Res r;
    if (m_data.Reserve(t_sz)) {
      for (size_t i = 0; i < t_sz; ++i) {
        r = Locate((t[i].*F)(), *r, hints...);
        if (!r.Found()) {
          (m_data.*insert)(*r, t + i, 1);
        }
      }
    }
    return r;
  }
  static bool Less(const K& a, const K& b) {
    return compare<LESS>(a, b);
  }
  Res Search(const Key& key, const Iter& s, const Iter& e) const {
    Iter start = s;
    Iter end = e;
    while (start < end) {
      Iter mid = start + (end - start) / 2;
      const Key& mid_key = (mid->*F)();
      if (Less(key, mid_key)) {
        end = mid;
      } else if (Less(mid_key, key)) {
        start = mid + 1;
      } else {
        return Res(mid, true);
      }
    }
    return Res(start, false);
  }
  template<typename... Hints>
  Res Locate(const K& key, Hints... hints) const {
    return LocateR(key, Begin(), End(), hints...);
  }
  Res LocateR(const K& key, const Iter& s, const Iter& e) const {
    return Search(key, s, e);
  }
  template<typename... Hints>
  Res LocateR(const K& key, const Iter& s, const Iter& e,
             const Iter& hint, Hints... hints) const {
    if (hint < s || hint >= e) {
      return LocateR(key, s, e, hints...);
    }
    const K& hint_key = (hint->*F)();
    if (key < hint_key) {
      return LocateR(key, s, hint, hints...);
    }
    if (hint_key < key) {
      return LocateR(key, hint + 1, e, hints...);
    }
    return Res(hint, true);
  }
  C m_data;
};
}

#endif
