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
  typedef typename C::IterC IterC;
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
      return m_inserted;
    }
  private:
    Res()
      : m_inserted (false) {
    }
    Res(const Iter& it, bool inserted)
      : m_it (it),
        m_inserted (inserted) {
    }
    Iter m_it;
    bool m_inserted;
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
  C& Data() {
    return m_data;
  }
  const C& Data() const {
    return m_data;
  }
  Iter Begin() {
    return m_data.Begin();
  }
  IterC Begin() const {
    return m_data.Begin();
  }
  Iter End() {
    return m_data.End();
  }
  IterC End() const {
    return m_data.End();
  }
  Res Insert(const T& t) {
    Res r = Locate(m_data.Begin(), m_data.End(), (t.*F)());
    if (r.Inserted()) {
      if (!m_data.Insert((*r), t, 1)) {
        return Res();
      } 
    }
    return r;
  }
private:
  static bool Less(const K& a, const K& b) {
    return compare<LESS>(a, b);
  }
  Res Locate(const Iter& s, const Iter& e, const Key& key) {
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
        return Res(mid, false);
      }
    }
    return Res(start, true);
  }
  Res Locate(const Iter& hint, const K& key) {
    if (hint < Begin() || hint >= End()) {
      return Locate(Begin(), End());
    }
    const Key& hint_key = (hint->*F)();
    if (Less(key, hint_key)) {
      return Locate(Begin(), hint);
    }
    if (Less(hint_key, key)) {
      return Locate(hint + 1, End());
    }
    return hint;
  }
  C m_data;
};
}

#endif
