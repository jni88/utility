#ifndef JNU_LIST_H
#define JNU_LIST_H

namespace jnu {
template<typename C>
class SLink {
public:
  class Node;
  template<Node& (C::*F)()> class List;
  class Node {
    template<Node& (C::*F)()> friend class List;
  public:
    Node()
      : m_next (NULL) {
    }
    Node(const Node& n) {
      *this = n;
    }
    Node(Node&& n) {
      *this = n;
    }
    Node& operator=(const Node& n) {
      m_next = n.m_next;
      return *this;
    }
    Node& operator=(Node&& n) {
      if (this != &n) {
        *this = n;
        n.m_next = NULL;
      }
      return *this;
    }
  private:
    C* m_next;
  };
  template<Node& (C::*F)()>
  class List {
    const static size_t MAX_SZ = (size_t) (-1);
  public:
    typedef C Type;
    List() {
      Clear();
    }
    ~List() {
    }
    List(const List& s) {
      *this = s;
    }
    List(List&& s) {
      *this = s;
    }
    List& operator=(const List& s) {
      m_size = s.m_size;
      m_head = s.m_head;
      m_tail = s.m_tail;
      return *this;
    }
    List& operator=(List&& s) {
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
    C* Next(C& obj) {
      return (obj.*F)().m_next;
    }
    size_t Size() const {
      return m_size;
    }
    bool InsertHead(C& obj) {
      return InsertHead(obj, obj, 1);
    }
    bool InsertHead(const List& ls) {
      return !ls || InsertHead(*ls.Head(), *ls.Tail(), ls.Size());
    }
    bool InsertHead(List&& ls) {
      if (InsertHead(ls)) {
        ls.Clear();
        return true;
      }
      return false;
    }
    bool InsertTail(C& obj) {
      return InsertTail(obj, obj, 1);
    }
    bool InsertTail(const List& ls) {
      return !ls || InsertTail(*ls.Head(), *ls.Tail(), ls.Size());
    }
    bool InsertTail(List&& ls) {
      if (InsertTail(ls)) {
        ls.Clear();
        return true;
      }
      return false;
    }
    bool InsertNext(C& obj, C& next_obj) {
      return InsertNext(obj, next_obj, next_obj, 1);
    }
    bool InsertNext(C& obj, const List& ls) {
      return !ls || InsertNext(obj, *ls.Head(), *ls.Tail(), ls.Size());
    }
    bool InsertNext(C& obj, List&& ls) {
      if (InsertNext(obj, ls)) {
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
    List DeleteHeadTill(C& e) {
      List ls;
      ls.m_head = m_head;
      ls.m_tail = &e;
      ls.m_size = Count(e);
      DeleteHead(e);
      m_size -= ls.m_size;
      return ls;
    }
    C* DeleteNext(C& obj) {
      C* next_obj = (obj.*F)().m_next;
      if (next_obj) {
        DeleteNext(obj, *next_obj);
        --m_size;
      }
      return next_obj;
    }
    List DeleteNextTill(C& obj, C& e) {
      List ls;
      C* next_obj = (obj.*F)().m_next;
      ls.m_head = next_obj;
      ls.m_tail = &e;
      ls.m_size = Count(*next_obj, e);
      DeleteNext(obj, e);
      m_size -= ls.m_size;
      return ls;
    }
    List DeleteNextAll(C& obj) {
      List ls;
      if (m_tail != &obj) {
        return DeleteNextTill(obj, *m_tail);
      }
      return List();
    }
  private:
    bool CanInsert(size_t sz) const {
      return m_size <= MAX_SZ - sz;
    }
    size_t Count(C& e) {
      if (&e == m_tail) {
        return m_size;
      }
      return DoCount(*m_head, e);
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
        t = (t->*F)().m_next;
      }
      return count;
    }
    bool InsertHead(C& s, C& e, size_t sz) {
      if (CanInsert(sz)) {
        (e.*F)().m_next = m_head;
        m_head = &s;
        if (!m_tail) {
          m_tail = &e;
        }
        m_size += sz;
        return true;
      }
      return false;
    }
    bool InsertTail(C& s, C& e, size_t sz) {
      if (CanInsert(sz)) {
        if (m_tail) {
          (m_tail->*F)().m_next = &s;
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
        (e.*F)().m_next = (obj.*F)().m_next;
        (obj.*F)().m_next = &s;
        if (m_tail == &obj) {
          m_tail = &e;
        }
        m_size += sz;
        return true;
      }
      return false;
    }
    void DeleteHead(C& e) {
      m_head = (e.*F)().m_next;
      (e.*F)().m_next = NULL;
    }
    void DeleteNext(C& obj, C& e) {
      (obj.*F)().m_next = (e.*F)().m_next;
      (e.*F)().m_next = NULL;
      if (m_tail == &e) {
        m_tail = &obj;
      }
    }
    size_t m_size;
    C* m_head;
    C* m_tail;
  };
};
}

#endif
