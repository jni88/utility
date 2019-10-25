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
    Iter& operator=(const Iter&) = delete;
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
    Res(const Iter& it, bool found = false)
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
  Res Insert(const T& t, Hints... hints) {
    return Add<const T, Hints...>(t, &C::Insert, hints...);
  }
  template<typename... Hints>
  size_t Insert(const T* t, size_t t_sz, Hints... hints) {
    return Add<const T, Hints...>(t, t_sz, &C::Insert, hints...);
  }
  template<typename H, typename... Hints>
  IsClass<H, size_t> Insert(const H& arr, Hints... hints) {
    return Insert(arr.Data(), arr.Size(), hints...);
  }
  template<typename... Hints>
  Res Inject(T& t, Hints... hints) {
    return Add<T, Hints...>(t, &C::Inject, hints...);
  }
  template<typename... Hints>
  size_t Inject(T* t, size_t t_sz, Hints... hints) {
    return Add<T, Hints...>(t, t_sz, &C::Inject, hints...);
  }
  template<typename H, typename... Hints>
  IsClass<H, size_t> Inject(H& arr, Hints... hints) {
    size_t sz = Inject(arr.Data(), arr.Size(), hints...);
    arr.Clear();
    return sz;
  }
  template<typename H, typename... Hints>
  size_t InjectRange(H& arr, T* t, size_t t_sz, Hints... hints) {
    size_t sz = Inject(t, t_sz, hints...);
    arr.Delete(t, t_sz);
    return sz;
  }
  template<typename... Hints>
  Iter Find(const T& t, Hints... hints) {
    Res r = Locate((t.*F)(), hints...);
    return r.Found() ? *r : Iter();
  }
private:
  template<typename H, typename... Hints>
  Res Add(H& t,
          Iter (C::*insert)(const Iter&, H*, size_t),
          Hints... hints) {
    Res r = Locate((t.*F)(), hints...);
    if (r.Inserted()) {
      if (!(m_data.*insert)(*r, &t, 1)) {
        return Res();
      }
    }
    return r;
  }
  template<typename H, typename... Hints>
  size_t Add(H* t, size_t t_sz,
             Iter (C::*insert)(const Iter&, H*, size_t),
             Hints... hints) {
    size_t sz = 0;
    Iter h;
    for (size_t i = 0; i < t_sz; ++i) {
      if (Res r = Add<H, Hints...>(t[i], insert, h, hints...)) {
        h = *r;
        ++sz;
      }
    }
    return sz;
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
    return Res(start);
  }
  template<typename... Hints>
  Res Locate(const K& key, Hints... hints) const {
    Res res(Begin(), true);
    return LocateR(key, res, hints...);
  }
  Res LocateR(const K& key, const Res& res) const {
    return Search(key, *res, End());
  }
  template<typename... Hints>
  Res LocateR(const K& key, Res& res,
             const Iter& hint, Hints... hints) const {
    if (!hint || hint < *res || hint >= End()) {
      return LocateR(key, res, hints...);
    }
    const K& hint_key = (hint->*F)();
    if (key < hint_key) {
      return Search(key, *res, hint);
    }
    res.m_it = hint;
    if (hint_key < key) {
      return LocateR(key, res, hints...);
    }
    return res;
  }
  C m_data;
};
}

#endif
