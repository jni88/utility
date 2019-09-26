#ifndef JNU_LIST_H
#define JNU_LIST_H

namespace jnu {
template<typename C, C*& (C::*NextF)()>
class SList {
  const static size_t MAX_SZ = (size_t) (-1);
public:
  typedef C Type;
  SList() {
    Clear();
  }
  ~SList() {
  }
  SList(const SList& s) {
    *this = s;
  }
  SList(SList* s) {
    *this = s;
  }
  SList& operator=(const SList& s) {
    m_size = s.m_size;
    m_head = s.m_head;
    m_tail = s.m_tail;
  }
  SList& operator=(SList* s) {
    if (s) {
      *this = *s;
      s->Clear();
    } else {
      Clear();
    }
  }
  void Clear() {
    m_size = 0;
    m_head = NULL;
    m_tail = NULL;
  }
  bool IsEmpty() const {
    return m_size != 0;
  }
  operator bool() const {
    return !IsEmpty();
  }
  C* Head() const {
    return m_head;
  }
  C* Tail() const {
    return m_tail;
  }
  C* Next(C* obj) {
    return obj ? (obj->*NextF)() : NULL;
  }
  size_t Size() const {
    return m_size;
  }
  bool InsertHead(C* obj) {
    if (obj && CanInsert()) {
      (obj->*NextF)() = m_head;
      m_head = obj;
      if (!m_tail) {
        m_tail = obj;
      }
      ++m_size;
      return true;
    }
    return false;
  }
  bool InsertTail(C* obj) {
    if (obj && CanInsert()) {
      (obj->*NextF)() = NULL;
      if (m_tail) {
        (m_tail->*NextF)() = obj;
      } else {
        m_head = obj;
      }
      m_tail = obj;
      ++m_size;
      return true;
    }
    return false;
  }
  bool InsertNext(C* obj, C* next_obj) {
    if (obj && next_obj && CanInsert()) {
      (next_obj->*NextF)() = (obj->*NextF)();
      (obj->*NextF)() = next_obj;
      if (m_tail == obj) {
        m_tail = next_obj;
      }
      ++m_size;
      return true;
    }
    return false;
  }

private:
  bool CanInsert() const {
    return m_size < MAX_SZ;
  }
  bool CanInsert(SList& sl) const {
    return m_size <= (MAX_SZ - sl.Size());
  }
  size_t m_size;
  C* m_head;
  C* m_tail;
};
}

#endif
