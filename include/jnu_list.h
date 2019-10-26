// By JNI
// Implementation of link lisk (single and double)
// Objects can be linked to each other
// by burying link nodes inside,
// therefore no need for extra memory
// allocation

#ifndef JNU_LIST_H
#define JNU_LIST_H

namespace jnu {
// General template for link list
// Each list maintains:
// list size, list head and list tail.
// template argument:
// C: Target object class type
// N: Type of link list node
// F: Member function for getting the node
template<typename C, typename N, N& (C::*F)()>
class ListBase {
  // Maximum number of objects in a list
  const static size_t MAX_SZ = (size_t) (-1);
public:
  typedef C Type;  // Type of target object
  // Default constructor
  ListBase() {
  }
  // Deconstructor
  ~ListBase() {
  }
  // Copy constructor
  ListBase(const ListBase& s) {
    *this = s;
  }
  // Move constructor
  ListBase(ListBase&& s) {
    *this = s;
  }
  // Assign operator
  ListBase& operator=(const ListBase& s) {
    m_size = s.m_size;  // Copy list size
    m_head = s.m_head;  // Copy list head
    m_tail = s.m_tail;  // Copy list tail
    return *this;
  }
  // Move operator
  ListBase& operator=(ListBase&& s) {
    if (this != &s) {  // Cannot move to itself
      *this = s;  // Assign s to self
      s.Clear();  // Clear s
    }
    return *this;
  }
  // Clear list
  void Clear() {
    m_size = 0;  // Reset list size
    m_head = NULL;  // Reset list head
    m_tail = NULL;  // Reset list tail
  }
  // Check if list is empty
  bool IsEmpty() const {
    return m_size == 0;
  }
  // Get list head
  C* Head() const {
    return m_head;
  }
  // Get list tail
  C* Tail() const {
    return m_tail;
  }
  // Get list size
  size_t Size() const {
    return m_size;
  }
  // Get next of 'obj' from list
  static C* Next(C& obj) {
    return (obj.*F)().Next();
  }
  // Get previous of 'obj' from list
  // Note: only for double link list
  static C* Prev(C& obj) {
    return (obj.*F)().Prev();
  }
  // Insert 'obj' into list head
  bool InsertHead(C& obj) {
    // Insert single object into list head
    return InsertHead(obj, obj, 1);
  }
  // Insert a list 'ls' into list head
  bool InsertHead(const ListBase& ls) {
    // For nonempty list 'ls',
    // insert it into list head
    return ls.IsEmpty() ||
           InsertHead(*ls.Head(), *ls.Tail(), ls.Size());
  }
  // Move a list 'ls' into list head
  bool InsertHead(ListBase&& ls) {
    if (InsertHead(ls)) {  // Insert list into head
      ls.Clear();  // Clear input list
      return true;
    }
    return false;
  }
  // Insert 'obj' into list tail
  bool InsertTail(C& obj) {
    // Insert single object into list tail
    return InsertTail(obj, obj, 1);
  }
  // Insert a list 'ls' into list tail
  bool InsertTail(const ListBase& ls) {
    // For nonempty list 'ls',
    // insert it into list tail
    return ls.IsEmpty() ||
           InsertTail(*ls.Head(), *ls.Tail(), ls.Size());
  }
  // Move a list 'ls' into list tail
  bool InsertTail(ListBase&& ls) {
    if (InsertTail(ls)) {  // Insert list into tail
      ls.Clear();  // Clear input list
      return true;
    }
    return false;
  }
  // Insert object 'next_obj' into
  // next of the current object 'obj'
  // Input: obj - the current object in the list
  //        next_obj - the object is to be inserted
  bool InsertNext(C& obj, C& next_obj) {
    // Insert single object into next of 'obj'
    return InsertNext(obj, next_obj, next_obj, 1);
  }
  // Insert list 'ls' into
  // next of the current object 'obj'
  bool InsertNext(C& obj, const ListBase& ls) {
    // Insert if 'ls' is not empty
    return ls.IsEmpty() ||
           InsertNext(obj, *ls.Head(), *ls.Tail(), ls.Size());
  }
  // Move list 'ls' into
  // next of the current object 'obj'
  bool InsertNext(C& obj, ListBase&& ls) {
    if (InsertNext(obj, ls)) {  // Insert
      ls.Clear();  // Clear list
      return true;
    }
    return false;
  }
  // Insert object 'prev_obj' into
  // previous of the current object 'obj'
  // Input: obj - the current object in the list
  //        prev_obj - the object is to be inserted
  // Note: only for double link list
  bool InsertPrev(C& obj, C& prev_obj) {
    // Insert single object into previous of 'obj'
    return InsertPrev(obj, prev_obj, prev_obj, 1);
  }
  // Insert list 'ls' into
  // previous of the current object 'obj'
  // Note: only for double link list
  bool InsertPrev(C& obj, const ListBase& ls) {
    // Insert if 'ls' is not empty
    return ls.IsEmpty() ||
           InsertPrev(obj, *ls.Head(), *ls.Tail(), ls.Size());
  }
  // Move list 'ls' into
  // previous of the current object 'obj'
  // Note: only for double link list
  bool InsertPrev(C& obj, ListBase&& ls) {
    if (InsertPrev(obj, ls)) {  // Insert
      ls.Clear();  // Clear list
      return true;
    }
    return false;
  }
  // Delete list head
  // Return: the deleted head
  C* DeleteHead() {
    C* head = m_head;  // List head
    if (head) {  // List is not empty
      DeleteHead(*head);  // Delete the head
      --m_size;  // Reduce list size
    }
    return head;
  }
  // Delete from head till an object
  // Input: e - end of delete (included)
  // Return: the list of deleted objects
  ListBase DeleteHeadTill(C& e) {
    ListBase ls;  // Return list
    ls.m_head = m_head;  // Deleted head
    ls.m_tail = &e;  // Deleted tail = e
    ls.m_size = CountFromHead(e);  // Count deleted size
    DeleteHead(e);  // Do delete from head to e
    m_size -= ls.m_size;  // Reduce current list size
    return ls;  // Return deleted list
  }
  // Delete list tail
  // Return: the deleted tail
  // Note: only for double link list
  C* DeleteTail() {
    C* tail = m_tail;  // List tail
    if (tail) {  // List is not empty
      DeleteTail(*tail);  // Delete tail
      --m_size;  // Reduce list size
    }
    return tail;
  }
  // Delete from tail till an object
  // Input: e - end of delete (included)
  // Return: the list of deleted objects
  // Note: only for double link list
  ListBase DeleteTailTill(C& e) {
    ListBase ls;  // Return list
    ls.m_head = &e;  // Deleted head = e
    ls.m_tail = m_tail;  // Deleted tail
    ls.m_size = CountFromTail(e);  // Count deleted size
    DeleteTail(e);  // Do delete from tail to e
    m_size -= ls.m_size;  // Reduce current list size
    return ls;  // Return deleted list
  }
  // Delete next of object 'obj'
  // Return: the deleted object
  C* DeleteNext(C& obj) {
    C* next_obj = Next(obj);  // Next of obj
    if (next_obj) {  // obj is not at tail
      DeleteNext(obj, *next_obj);  // Do delete next
      --m_size;  // Reduce list size
    }
    return next_obj;
  }
  // Delete from next of 'obj' till object e
  // Input: obj - current object, its next is to be deleted
  //        e - the end of delete (included)
  // Return: the deleted list of objects
  ListBase DeleteNextTill(C& obj, C& e) {
    ListBase ls;  // Return list
    C* next_obj = Next(obj);  // Next of obj
    ls.m_head = next_obj;  // Deleted head = Next of obj
    ls.m_tail = &e;  // Deleted tail = e
    ls.m_size = Count(*next_obj, e);  // Count deleted size
    DeleteNext(obj, e);  // Delete from next of obj to e
    m_size -= ls.m_size;  // Reduce current list size
    return ls;  // Return deleted list
  }
  // Delete from next of 'obj' all the way till tail
  // Input: obj - current object, its next is to be deleted
  // Return: the deleted list
  ListBase DeleteNextAll(C& obj) {
    if (m_tail != &obj) {  // obj is not tail
      return DeleteNextTill(obj, *m_tail);  // Delete next till tail
    }
    return ListBase();  // obj is tail, do nothing
  }
  // Delete previous of object 'obj'
  // Return: the deleted object
  // Note: only for double link list
  C* DeletePrev(C& obj) {
    C* prev_obj = Prev(obj);  // Previous of obj
    if (prev_obj) {  // obj is not at head
      DeletePrev(obj, *prev_obj);  // Delete obj's previous
      --m_size;  // Reduce list size
    }
    return prev_obj;
  }
  // Delete from obj's previous till object e
  // Input: obj - target object, its previous is to be deleted
  //        e - end object of delete
  // Return: the deleted list
  // Note: only for double link list
  ListBase DeletePrevTill(C& obj, C& e) {
    ListBase ls;  // Return list
    C* prev_obj = Prev(obj);  // Previous of obj
    ls.m_head = &e;  // Deleted head = e
    ls.m_tail = prev_obj;  // Delete tail = previous of obj
    ls.m_size = Count(e, *prev_obj);  // Count deleted size
    DeletePrev(obj, e);  // Do delete previous till e
    m_size -= ls.m_size;  // Reduce current list size
    return ls;  // Return deleted list
  }
  // Delete from obj's previous all the way till head
  // Input: obj - target object, its previous is to be deleted
  // Return: the deleted list
  // Note: only for double link list
  ListBase DeletePrevAll(C& obj) {
    if (m_head != &obj) {  // obj is not at head
      return DeletePrevTill(obj, *m_head);  // Delete till head
    }
    return ListBase();  // obj is head, do nothing
  }
  // Delete the current object 'obj'
  // Return: the deleted object 'obj'
  // Note: only for double link list
  C* Delete(C& obj) {
    Delete(obj, obj);  // Delete single object
    --m_size;  // Reduce list size
    return &obj;
  }
  // Delete from obj to e
  // Input: obj - start of delete
  //        e - end of delete
  // Return: the deleted list
  ListBase DeleteTill(C& obj, C& e) {
    ListBase ls;  // Return list
    ls.m_head = &obj;  // Deleted head = obj
    ls.m_tail = &e;  // Deleted tail = e
    ls.m_size = Count(obj, e);  // Count deleted size
    Delete(obj, e);  // Do delete
    m_size -= ls.m_size;  // Reduce current list size
    return ls;  // Return deleted list
  }
private:
  // Check if can be inserted
  // Input: sz - the inserted size
  bool CanInsert(size_t sz) const {
      return m_size <= MAX_SZ - sz;
  }
  // Count objects from head till e
  size_t CountFromHead(C& e) {
    if (&e == m_tail) {  // e is tail
      return m_size;  // Directly return list size
    }
    return DoCount(*m_head, e);  // Otherwise do count
  }
  // Count objects from tail till e
  size_t CountFromTail(C& e) {
    if (&e == m_head) {  // e is head
      return m_size;  // Directly return list size
    }
    return DoCount(e, *m_tail);  // Otherwise do count
  }
  // Count from s to e
  size_t Count(C& s, C& e) {
    if (&s == m_head && &e == m_tail) {  // Actually entire list
      return m_size;  // Directly return list size
    }
    return DoCount(s, e);  // Otherwise do count
  }
  // Do count from s to e
  size_t DoCount(C& s, C& e) {
    size_t count = 1;  // Initialize count
    C* t = &s;  // Start of count
    while (t != &e) {  // Not meet end
      ++count;  // Increase count
      t = Next(*t);  // Move to next
    }
    return count;  // Count of objects from s to e
  }
  // Insert object list [s, e] into list head
  // Input: [s, e]: list to be inserted
  //        sz: object count from s to e
  // Return: true - insert success
  bool InsertHead(C& s, C& e, size_t sz) {
    if (CanInsert(sz)) {  // Check size
      if (m_head) {  // Current list not empty
        N::Link(e, *m_head, F);  // Link e to list head
      } else {  // Current list is empty
        m_tail = &e;  // New tail = e
      }
      m_head = &s;  // New head = s;
      m_size += sz;  // Increase current list size
      return true;
    }
    return false;
  }
  // Insert object list [s, e] into list tail
  // Input: [s, e]: list to be inserted
  //        sz - size of [s, e]
  // Return: true - insert success
  bool InsertTail(C& s, C& e, size_t sz) {
    if (CanInsert(sz)) {  // Check size
      if (m_tail) {  // Current list not empty
        N::Link(*m_tail, s, F);  // Link list tail to s
      } else {  // Current list is empty
        m_head = &s;  // New head = s
      }
      m_tail = &e;  // New tail = e
      m_size += sz;  // Increase current list size
      return true;
    }
    return false;
  }
  // Insert list [s, e] to obj's next
  // Input: obj - current object
  //        [s, e] - list to be inserted to obj's next
  //        sz - size of [s, e]
  bool InsertNext(C& obj, C& s, C& e, size_t sz) {
    if (CanInsert(sz)) {  // Check size
      C* next_obj = Next(obj);  // Get next of obj
      N::Link(obj, s, F);  // Link obj to s
      if (next_obj) {  // If obj is not at tail
        N::Link(e, *next_obj, F);  // Link e to obj's next
      } else {  // Obj is tail
        m_tail = &e;  // New tail = e
      }
      m_size += sz;  // Increase current list size
      return true;
    }
    return false;
  }
  // Insert list [s, e] to obj's previous
  // Input: obj - the current object
  //        [s, e] - list to be inserted to obj's previous
  //        sz - size of [s, e]
  bool InsertPrev(C& obj, C& s, C& e, size_t sz) {
    if (CanInsert(sz)) {  // Check size
      C* prev_obj = Prev(obj);  // Get previous of obj
      N::Link(e, obj, F);  // Link e to obj
      if (prev_obj) {  // If obj is not at head
        N::Link(*prev_obj, s, F);  // Link obj's previous to s
      } else {  // Obj is at head
        m_head = &s;  // New head = s
      }
      m_size += sz;  // Increase current list size
      return true;
    }
    return false;
  }
  // Delete list [head, e]
  void DeleteHead(C& e) {
    C* next_e = Next(e);  // Get next of e
    if (next_e) {  // e is not at tail
      N::Break(e, *next_e, F);  // Break e with its next
    } else {  // e is at tail
      m_tail = NULL;  // Entire list becomes empty
    }
    m_head = next_e;  // New head = next of e
  }
  // Delete list [e, tail]
  void DeleteTail(C& e) {
    C* prev_e = Prev(e);  // Get previous of e
    if (prev_e) {  // e is not at head
      N::Break(*prev_e, e, F);  // Break e's previous with e
    } else {  // e is at head
      m_head = NULL;  // Entire list becomes empty
    }
    m_tail = prev_e;  // New tail = previous of e
  }
  // Delete list [obj's next, e]
  void DeleteNext(C& obj, C& e) {
    C* next_obj = Next(obj);  // Get next of obj
    C* next_e = Next(e);  // Get next of e
    N::Break(obj, *next_obj, F);  // Break obj from its next
    if (next_e) {  // If e is not at tail
      N::Break(e, *next_e, F);  // Break e from its next
      N::Link(obj, *next_e, F);  // Link obj with e's next
    } else {  // e is at tail
      m_tail = &obj;  // New tail = obj
    }
  }
  // Delete list [e, obj's previous]
  void DeletePrev(C& obj, C& e) {
    C* prev_obj = Prev(obj);  // Get previous of obj
    C* prev_e = Prev(e);  // Get previous of e
    N::Break(*prev_obj, obj, F);  // Break obj's previous with obj
    if (prev_e) {  // If e is not at head
      N::Break(*prev_e, e, F);  // Break e's previous with e
      N::Link(*prev_e, obj, F);  // Link e's previous with obj
    } else {  // e is at head
      m_head = &obj;  // New head = obj
    }
  }
  // Delete list [s, e]
  void Delete(C& s, C& e) {
    C* prev_s = Prev(s);  // Get previous of s
    C* next_e = Next(e);  // Get next of e
    if (prev_s) {  // If s is not at head
      N::Break(*prev_s, s, F);  // Break s's previous with s
    } else {  // s is at head
      m_head = next_e;  // New head = next of e
    }
    if (next_e) {  // If e is not at tail
      N::Break(e, *next_e, F);  // Break e with its next
    } else {  // e is at tail
      m_tail = prev_s;  // New tail = prev of s
    }
    // Link s's previous with e's next
    // if they both are valid
    if (prev_s && next_e) {
      N::Link(*prev_s, *next_e, F);
    }
  }
  size_t m_size;  // List size
  C* m_head;  // List head
  C* m_tail;  // List tail
};
// Single link list, built on base of ListBase
// Template parameter: C - object class type
template<typename C>
class SLink {
public:
  // Node of single link list,
  // it is to be buried in side of target objects
  // to form single list
  class Node {
    // ListBase need access some of its private members
    template<C, typename Node, Node& (C::*F)()> friend class jnu::ListBase;
  public:
    // Default constructor
    Node()
      : m_next (NULL) {
    }
    // Deconstructor
    ~Node() {
    }
  private:
    // Access next linked object
    C* Next() const {
      return m_next;
    }
    // Link b to the next of a
    // Input: a, b: a is to be linked with b
    //        f: Access link nodes of a and b
    static void Link(C& a, C& b, Node& (C::*f)()) {
      (a.*f)().m_next = &b;  // Link b to the next of a
    }
    // Break the link between a and b
    // Object b is previous linked as next of a
    // Input: a, b: their link is to be break
    //        f: Access link nodes of a and b
    static void Break(C& a, C& b, Node& (C::*f)()) {
      (a.*f)().m_next = NULL;  // Break link
    }
    C* m_next;  // Pointing to next object
  };
  // Single link list
  // Template parameter: F - Access object's link node
  template<Node& (C::*F)()>
  class List : public ListBase<C, Node, F> {
  public:
    // It is built on base of ListBase
    typedef ListBase<C, Node, F> Base;
    // Default constructor
    List() {
    }
    // Deconstructor
    ~List() {
    }
    // Assign operator
    List& operator=(const Base& s) {
      Base::operator=(s);
      return *this;
    }
    // Move operator
    List& operator=(Base&& s) {
      Base::operator=(s);
      return *this;
    }
  };
};
// Double link list, built on base of ListBase
// Template parameter: C - object class type
template<typename C>
class DLink {
public:
  // Node of double link list,
  // it is to be buried in side of target objects
  // to form double list
  class Node {
    // ListBase need access some of its private members
    template<C, typename Node, Node& (C::*F)()> friend class jnu::ListBase;
  public:
    // Default constructor
    Node()
      : m_next (NULL),
        m_prev (NULL) {
    }
    // Deconstructor
    ~Node() {
    }
  private:
    // Access the next object
    C* Next() const {
      return m_next;
    }
    // Access the previous object
    C* Prev() const {
      return m_prev;
    }
    // Link b to be the next of a,
    // and a to be the previous of b
    // Input: a, b: objects to be linked
    //        f: node access function
    static void Link(C& a, C& b, Node& (C::*f)()) {
      (a.*f)().m_next = &b;  // b to be the next of a
      (b.*f)().m_prev = &a;  // a to be the previous of b
    }
    // Break the link between a and b
    // Previously, b is the next of a, and
    // a is the previous of b
    // Input: a, b: the link between which to be broken
    //        f: node access function
    static void Break(C& a, C& b, Node& (C::*f)()) {
      (a.*f)().m_next = NULL;  // Break a's next
      (b.*f)().m_prev = NULL;  // Break b's previous
    }
    C* m_next;  // Pointing to next object
    C* m_prev;  // Pointing to previous object
  };
  // Double link list
  // Template parameter: F - Access object's link node
  template<Node& (C::*F)()>
  class List : public ListBase<C, Node, F> {
  public:
    // It is built on base of ListBase
    typedef ListBase<C, Node, F> Base;
    // Default constructor
    List() {
    }
    // Deconstructor
    ~List() {
    }
    // Assign operator
    List& operator=(const Base& s) {
      Base::operator=(s);
      return *this;
    }
    // Move operator
    List& operator=(Base&& s) {
      Base::operator=(s);
      return *this;
    }
  };
};
}

#endif
