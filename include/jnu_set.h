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
  template<typename... Hints>
  Res Insert(const T& t, Hints... hints) {
    Res r = Locate((t.*F)(), hints...);
    if (r.Inserted()) {
      if (!m_data.Insert((*r), t, 1)) {
        return Res();
      }
    }
    return r;
  }
  template<typename... Hints>
  Iter Find(const T& t, Hints... hints) {
    Res r = Locate((t.*F)(), hints...);
    return r.Found() ? *r : Iter();
  }
private:
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
    return Locate(key, res, hints...);
  }
  Res Locate(const K& key, const Res& res) const {
    return Search(key, *res, End());
  }
  template<typename... Hints>
  Res Locate(const K& key, Res& res,
             const Iter& hint, Hints... hints) const {
    if (hint < *res || hint >= End()) {
      return Locate(key, res, hints...);
    }
    const K& hint_key = (hint->*F)();
    if (key < hint_key) {
      return Search(key, *res, hint);
    }
    res.m_it = hint;
    if (hint_key < key) {
      return Locate(key, res, hints...); 
    }
    printf("hint hit\n");
    return res;
  }
  C m_data;
};
}

#endif
