#ifndef JNU_LIST_H
#define JNU_LIST_H

namespace jnu {
template<typename C, typename N, N& (C::*F)()>
class ListBase {
  const static size_t MAX_SZ = (size_t) (-1);
public:
  typedef C Type;
  ListBase() {
  }
  ~ListBase() {
  }
  ListBase(const ListBase& s) {
    *this = s;
  }
  ListBase(ListBase&& s) {
    *this = s;
  }
  ListBase& operator=(const ListBase& s) {
    m_size = s.m_size;
    m_head = s.m_head;
    m_tail = s.m_tail;
    return *this;
  }
  ListBase& operator=(ListBase&& s) {
    if (this != &s) {
      *this = s;
      s.Clear();
    }
    return *this;
  }
  void Clear() {
    m_size = 0;
    m_head = NULL;
    m_tail = NULL;
  }
  bool IsEmpty() const {
    return m_size == 0;
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
  size_t Size() const {
    return m_size;
  }
  static C* Next(C& obj) {
    return (obj.*F)().Next();
  }
  static C* Prev(C& obj) {
    return (obj.*F)().Prev();
  }
  bool InsertHead(C& obj) {
    return InsertHead(obj, obj, 1);
  }
  bool InsertHead(const ListBase& ls) {
    return !ls || InsertHead(*ls.Head(), *ls.Tail(), ls.Size());
  }
  bool InsertHead(ListBase&& ls) {
    if (InsertHead(ls)) {
      ls.Clear();
      return true;
    }
    return false;
  }
  bool InsertTail(C& obj) {
    return InsertTail(obj, obj, 1);
  }
  bool InsertTail(const ListBase& ls) {
    return !ls || InsertTail(*ls.Head(), *ls.Tail(), ls.Size());
  }
  bool InsertTail(ListBase&& ls) {
    if (InsertTail(ls)) {
      ls.Clear();
      return true;
    }
    return false;
  }
  bool InsertNext(C& obj, C& next_obj) {
    return InsertNext(obj, next_obj, next_obj, 1);
  }
  bool InsertNext(C& obj, const ListBase& ls) {
    return !ls || InsertNext(obj, *ls.Head(), *ls.Tail(), ls.Size());
  }
  bool InsertNext(C& obj, ListBase&& ls) {
    if (InsertNext(obj, ls)) {
      ls.Clear();
      return true;
    }
    return false;
  }
  bool InsertPrev(C& obj, C& prev_obj) {
    return InsertPrev(obj, prev_obj, prev_obj, 1);
  }
  bool InsertPrev(C& obj, const ListBase& ls) {
    return !ls || InsertPrev(obj, *ls.Head(), *ls.Tail(), ls.Size());
  }
  bool InsertPrev(C& obj, ListBase&& ls) {
    if (InsertPrev(obj, ls)) {
      ls.Clear();
      return true;
    }
    return false;
  }
  C* DeleteHead() {
    C* head = m_head;
    if (head) {
      DeleteHead(*head);
      --m_size;
    }
    return head;
  }
  ListBase DeleteHeadTill(C& e) {
    ListBase ls;
    ls.m_head = m_head;
    ls.m_tail = &e;
    ls.m_size = CountFromHead(e);
    DeleteHead(e);
    m_size -= ls.m_size;
    return ls;
  }
  C* DeleteTail() {
    C* tail = m_tail;
    if (tail) {
      DeleteTail(*tail);
      --m_size;
    }
    return tail;
  }
  ListBase DeleteTailTill(C& e) {
    ListBase ls;
    ls.m_head = &e;
    ls.m_tail = m_tail;
    ls.m_size = CountFromTail(e);
    DeleteTail(e);
    m_size -= ls.m_size;
    return ls;
  }
  C* DeleteNext(C& obj) {
    C* next_obj = Next(obj);
    if (next_obj) {
      DeleteNext(obj, *next_obj);
      --m_size;
    }
    return next_obj;
  }
  ListBase DeleteNextTill(C& obj, C& e) {
    ListBase ls;
    C* next_obj = Next(obj);
    ls.m_head = next_obj;
    ls.m_tail = &e;
    ls.m_size = Count(*next_obj, e);
    DeleteNext(obj, e);
    m_size -= ls.m_size;
    return ls;
  }
  ListBase DeleteNextAll(C& obj) {
    ListBase ls;
    if (m_tail != &obj) {
      return DeleteNextTill(obj, *m_tail);
    }
    return ListBase();
  }
  C* DeletePrev(C& obj) {
    C* prev_obj = Prev(obj);
    if (prev_obj) {
      DeletePrev(obj, *prev_obj);
      --m_size;
    }
    return prev_obj;
  }
  ListBase DeletePrevTill(C& obj, C& e) {
    ListBase ls;
    C* prev_obj = Prev(obj);
    ls.m_head = &e;
    ls.m_tail = prev_obj;
    ls.m_size = Count(e, *prev_obj);
    DeletePrev(obj, e);
    m_size -= ls.m_size;
    return ls;
  }
  ListBase DeletePrevAll(C& obj) {
    ListBase ls;
    if (m_head != &obj) {
      return DeletePrevTill(obj, *m_head);
    }
    return ListBase();
  }
  C* Delete(C& obj) {
    Delete(obj, obj);
    --m_size;
    return &obj;
  }
  ListBase DeleteTill(C& obj, C& e) {
    ListBase ls;
    ls.m_head = &obj;
    ls.m_tail = &e;
    ls.m_size = Count(obj, e);
    Delete(obj, e);
    m_size -= ls.m_size;
    return ls;
  }
private:
  bool CanInsert(size_t sz) const {
      return m_size <= MAX_SZ - sz;
    }
  size_t CountFromHead(C& e) {
    if (&e == m_tail) {
      return m_size;
    }
    return DoCount(*m_head, e);
  }
  size_t CountFromTail(C& e) {
    if (&e == m_head) {
      return m_size;
    }
    return DoCount(e, *m_tail);
  }
  size_t Count(C& s, C& e) {
    if (&s == m_head && &e == m_tail) {
      return m_size;
    }
    return DoCount(s, e);
  }
  size_t DoCount(C& s, C& e) {
    size_t count = 1;
    C* t = &s;
    while (t != &e) {
      ++count;
      t = Next(*t);
    }
    return count;
  }
  bool InsertHead(C& s, C& e, size_t sz) {
    if (CanInsert(sz)) {
      if (m_head) {
        N::Link(e, *m_head, F);
      } else {
        m_tail = &e;
      }
      m_head = &s;
      m_size += sz;
      return true;
    }
    return false;
  }
  bool InsertTail(C& s, C& e, size_t sz) {
    if (CanInsert(sz)) {
      if (m_tail) {
        N::Link(*m_tail, s, F);
      } else {
        m_head = &s;
      }
      m_tail = &e;
      m_size += sz;
      return true;
    }
    return false;
  }
  bool InsertNext(C& obj, C& s, C& e, size_t sz) {
    if (CanInsert(sz)) {
      C* next_obj = Next(obj);
      N::Link(obj, s, F);
      if (next_obj) {
        N::Link(e, *next_obj, F);
      } else {
        m_tail = &e;
      }
      m_size += sz;
      return true;
    }
    return false;
  }
  bool InsertPrev(C& obj, C& s, C& e, size_t sz) {
    if (CanInsert(sz)) {
      C* prev_obj = Prev(obj);
      N::Link(e, obj, F);
      if (prev_obj) {
        N::Link(*prev_obj, s, F);
      } else {
        m_head = &s;
      }
      m_size += sz;
      return true;
    }
    return false;
  }
  void DeleteHead(C& e) {
    m_head = Next(e);
    if (m_head) {
      N::Break(e, *m_head, F);
    } else {
      m_tail = NULL;
    }
  }
  void DeleteTail(C& e) {
    m_tail = Prev(e);
    if (m_tail) {
      N::Break(*m_tail, e, F);
    } else {
      m_head = NULL;
    }
  }
  void DeleteNext(C& obj, C& e) {
    C* next_obj = Next(obj);
    C* next_e = Next(e);
    N::Break(obj, *next_obj, F);
    if (next_e) {
      N::Break(e, *next_e, F);
      N::Link(obj, *next_e, F);
    } else {
      m_tail = &obj;
    }
  }
  void DeletePrev(C& obj, C& e) {
    C* prev_obj = Prev(obj);
    C* prev_e = Prev(e);
    N::Break(*prev_obj, obj, F);
    if (prev_e) {
      N::Break(*prev_e, e, F);
      N::Link(*prev_e, obj, F);
    } else {
      m_head = &obj;
    }
  }
  void Delete(C& s, C& e) {
    C* prev_s = Prev(s);
    C* next_e = Next(e);
    if (prev_s) {
      N::Break(*prev_s, s, F);
    } else {
      m_head = next_e;
    }
    if (next_e) {
      N::Break(e, *next_e, F);
    } else {
      m_tail = prev_s;
    }
  }
  size_t m_size;
  C* m_head;
  C* m_tail;
};
template<typename C>
class SLink {
public:
  class Node {
    template<C, typename Node, Node& (C::*F)()> friend class jnu::ListBase;
  public:
    Node()
      : m_next (NULL) {
    }
    ~Node() {
    }
  private:
    C* Next() const {
      return m_next;
    }
    static void Link(C& a, C& b, Node& (C::*f)()) {
      (a.*f)().m_next = &b;
    }
    static void Break(C& a, C& b, Node& (C::*f)()) {
      (a.*f)().m_next = NULL;
    }
    C* m_next;
  };
  template<Node& (C::*F)()>
  class List : public ListBase<C, Node, F> {
  public:
    typedef ListBase<C, Node, F> Base;
    List() {
    }
    ~List() {
    }
    List& operator=(const Base& s) {
      Base::operator=(s);
      return *this;
    }
    List& operator=(Base&& s) {
      Base::operator=(s);
      return *this;
    }
  };
};
template<typename C>
class DLink {
public:
  class Node {
    template<C, typename Node, Node& (C::*F)()> friend class jnu::ListBase;
  public:
    Node()
      : m_next (NULL),
        m_prev (NULL) {
    }
    ~Node() {
    }
  private:
    C* Next() const {
      return m_next;
    }
    C* Prev() const {
      return m_prev;
    }
    static void Link(C& a, C& b, Node& (C::*f)()) {
      (a.*f)().m_next = &b;
      (b.*f)().m_prev = &a;
    }
    static void Break(C& a, C& b, Node& (C::*f)()) {
      (a.*f)().m_next = NULL;
      (b.*f)().m_prev = NULL;
    }
    C* m_next;
    C* m_prev;
  };
  template<Node& (C::*F)()>
  class List : public ListBase<C, Node, F> {
  public:
    typedef ListBase<C, Node, F> Base;
    List() {
    }
    ~List() {
    }
    List& operator=(const Base& s) {
      Base::operator=(s);
      return *this;
    }
    List& operator=(Base&& s) {
      Base::operator=(s);
      return *this;
    }
  };
};
}

#endif
