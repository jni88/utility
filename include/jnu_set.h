// By JNI
// Implement sorted set on the base of array
// The insertion and deletion operations are carried on
// by shifting memories
// Keep sorted data in continuous memory space may
// provide efficiency for some types of applications

#ifndef JNU_SET_H
#define JNU_SET_H

#include <utility>
#include <functional>
#include "jnu_memory.h"

namespace jnu {
// Sorted set
// Template arguments:
// C - the array data type (static, dynamic or hybrid)
// K - the type of key
// KEY - the function for getting key from underline objects
// LESS - comparison of key (less than)
template<typename C, typename K,
         auto KEY = (const K& (*)(const typename C::Type)) NULL,
         auto LESS = (bool (*) (const K&, const K&)) NULL>
class SetT {
  // Default key comparison if LESS is not defined (NULL)
  template<decltype(LESS) L>
  static typename std::enable_if<L == NULL, bool>::type
  Compare(const K& a, const K& b) {
    return a < b;  // Default comparison
  }
  // Use LESS as comparison is it is defined (not NULL)
  template<decltype(LESS) L>
  static typename std::enable_if<L != NULL, bool>::type
  Compare(const K& a, const K& b) {
    return L(a, b);  // Use LESS as comparison
  }
  // Default get key function is KEY is not defined (NULL)
  typedef typename C::Type T;  // Underline data type
  template<decltype(KEY) L>
  static const typename std::enable_if <L == NULL, K>::type&
  GetKey(const T& t) {
    return (const K&) t;  // Use type cast as default
  }
  // Use defined KEY function to get key from object
  template<decltype(KEY) L>
  static const typename std::enable_if <L != NULL, K>::type&
  GetKey(const T& t) {
    return L(t);  // Use LESS function to get key
  }
public:
  typedef T Type;  // Underline data type
  typedef K Key;  // Key type
  typedef typename C::Iter Iter;  // Array iteration
  // Search or insertion result structure
  class Res {
    friend class SetT;
  public:
    // Get underline iterator
    Iter& operator*() {
      return m_it;
    }
    // Get underline const iterator
    const Iter& operator*() const {
      return m_it;
    }
    // Check if underline iterator is valid
    operator bool() const {
      return (bool) m_it;
    }
    // For insertion, check if data inserted
    bool Inserted() const {
      return !m_found;
    }
    // For searching, check if data found
    bool Found() const {
      return m_found;
    }
  private:
    // Default constructor
    Res()
      : m_found (false) {
    }
    // Constructor
    Res(const Iter& it, bool found)
      : m_it (it),
        m_found (found) {
    }
    Iter m_it;  // Array iterator
    bool m_found;  // Flag for data found
  };
  // Set constructor
  // Input: rsv_sz - reserve memory size
  //        mm - memory mamanger (use buildin as default)
  SetT(size_t rsv_sz = 0,
      memory::MMBase* mm = &memory::MM_BUILDIN)
    : m_data (rsv_sz, mm) {
  }
  // Deconstructor
  ~SetT() {
  }
  // Copy constructor
  SetT(const SetT& s) {
    *this = s;
  }
  // Move constructor
  SetT(SetT&& s) {
    *this = std::move(s);
  }
  // Assign operator
  SetT& operator=(const SetT& s) {
    m_data = s.m_data;
    return *this;
  }
  // Move operator
  SetT& operator==(SetT&& s) {
    m_data = std::move(s.m_data);
    return *this;
  }
  // Reserve memory
  bool Reserve(size_t rsv_sz) {
    return m_data.Reserve(rsv_sz);
  }
  // Clear set and free memory
  void Free() {
    m_data.Free();
  }
  // Clear set only
  void Clear() {
    m_data.Clear();
  }
  // Free unused memory
  bool Recycle() {
    return m_data.Recycle();
  }
  // Get set size
  size_t Size() const {
    return m_data.Size();
  }
  // Check if set is emoty
  bool IsEmpty() const {
    return Size() <= 0;
  }
  // Get underline const array
  const C& Data() const {
    return m_data;
  }
  // Get underline array
  // Dangerous: user should be careful to
  // maintain the order of the set
  C& Data() {
    return m_data;
  }
  // Get begin of set
  Iter Begin() const {
    return m_data.Begin();
  }
  // Get end of set (exclude)
  Iter End() const {
    return m_data.End();
  }
  // Get reversed begin of set
  Iter RBegin() const {
    return m_data.RBegin();
  }
  // Get reversed end of set (exclude)
  Iter REnd() const {
    return m_data.REnd();
  }
  // Get element at position i
  T& operator[](size_t i) {
    return m_data.operator[](i);
  }
  // Insert (unsorted) array of data
  // Input: t, t_sz - input data array
  //        replace - true: replace existing objects
  //        hints - the list of hint insert positions
  // Return: true - insert successful
  template<typename... Hints>
  bool Insert(const T* t, size_t t_sz,
              bool replace, Hints... hints) {
    // Do add array
    return Add<const T, Hints...>(t, t_sz, &C::Insert,
                                  replace, hints...);
  }
  // Insert (unsorted) array of data
  // Input array is define by [t, t_end)
  template<typename... Hints>
  bool Insert(const T* t, const T* t_end,
              bool replace, Hints... hints) {
    return Insert(t, Distance(t, t_end),
                  replace, hints...);
  }
  // Insert single element
  // Return: insert result, including
  //         the position of insertion and if
  //         the element already exists
  template<typename... Hints>
  Res Insert(const T& t, bool replace, Hints... hints) {
    return Add<const T, Hints...>(&t, 1, &C::Insert,
                                  replace, hints...);
  }
  // Insert sorted array
  // Assume input array t and t_sz is pre-sorted
  // Insert sorted array is faster than unsorted array
  // But you have to be sure, the input array is indeed
  // sorted.
  template<typename... Hints>
  bool InsertSorted(const T* t, size_t t_sz,
                    bool replace, Hints... hints) {
    // Use Merge function to insert sorted array
    return Merge<const T, Hints...>(t, t_sz, &C::Insert,
                                    replace, hints...);
  }
  // Sorted array is defined by [t, t_end)
  template<typename... Hints>
  bool InsertSorted(const T* t, const T* t_end,
                    bool replace, Hints... hints) {
    return InsertSorted(t, Distance(t, t_end),
                        replace, hints...);
  }
  // Inject (unsorted) array
  // Elements in array will be move to set
  template<typename... Hints>
  bool Inject(T* t, size_t t_sz,
              bool replace, Hints... hints) {
    return Add<T, Hints...>(t, t_sz, &C::Inject,
                            replace, hints...);
  }
  // Inject (unsorted) array [t, t_end)
  template<typename... Hints>
  bool Inject(T* t, T* t_end,
              bool replace, Hints... hints) {
    return Inject(t, Distance(t, t_end),
                  replace, hints...);
  }
  // Inject single element
  template<typename... Hints>
  bool Inject(T& t, bool replace, Hints... hints) {
    return Inject(&t, 1, replace, hints...);
  }
  // Inject sorted array
  template<typename... Hints>
  bool InjectSorted(T* t, size_t t_sz,
                    bool replace, Hints... hints) {
    return Merge<T, Hints...>(t, t_sz, &C::Inject,
                              replace, hints...);
  }
  // Inject sorted array [t, t_end)
  template<typename... Hints>
  bool InjectSorted(T* t, T* t_end,
                    bool replace, Hints... hints) {
    return InjectSorted(t, Distance(t, t_end),
                        replace, hints...);
  }
  // Inject unsorted array
  // The input array will be cleared after inject
  template<typename H, typename... Hints>
  bool Inject(H& arr, T* t, size_t t_sz,
              bool replace, Hints... hints) {
    if (Inject(t, t_sz, replace, hints...)) {
      arr.Delete(t, t_sz);
      return true;
    }
    return false;
  }
  // Inject unsorted array [t, t_end)
  template<typename H, typename... Hints>
  bool Inject(H& arr, T* t, T* t_end,
              bool replace, Hints... hints) {
    return Inject(arr, t, Distance(t, t_end),
                  replace, hints...);
  }
  // Inject sorted array
  template<typename H, typename... Hints>
  bool InjectSorted(H& arr, T* t, size_t t_sz,
                    bool replace, Hints... hints) {
    if (InjectSorted(t, t_sz, replace, hints...)) {
      arr.Delete(t, t_sz);
      return true;
    }
    return false;
  }
  // Inject sorted array [t, t_end)
  template<typename H, typename... Hints>
  bool InjectSorted(H& arr, T* t, T* t_end,
                    bool replace, Hints... hints) {
    return InjectSorted(arr, t, Distance(t, t_end),
                        replace, hints...);
  }
  // Delete elements [p, p + t_sz)
  Iter Delete(const Iter& p, size_t t_sz) {
    return m_data.Delete(p, t_sz);
  }
  // Delete elements [p, p_end)
  Iter Delete(const Iter& p, const Iter& p_end) {
    return Delete(p, Distance(p, p_end));
  }
  // Locate key
  // Return: the location of key,
  //         if not found, the location the key should
  //         be inserted
  template<typename... Hints>
  Res Locate(const K& key, Hints... hints) const {
    return LocateR(key, Begin(), End(), hints...);
  }
  // Find key
  // Return: the location if found
  //         invalid location if not found
  template<typename... Hints>
  Iter Find(const K& key, Hints... hints) const {
    Res r = Locate(key, hints...);
    return r.Found() ? *r : Iter();
  }
private:
  // Calculate distance between two iterators
  size_t Distance(const T* s, const T* e) {
    return s < e ? (size_t) (e - s) : 0;
  }
  template<typename H, typename... Hints>
  Res Add(H* t, size_t t_sz,
          Iter (C::*insert)(const Iter&, H*, size_t),
          bool replace,
          Hints... hints) {
    if (m_data.Reserve(t_sz)) {
      Res r(m_data.End(), false);
      for (size_t i = 0; i < t_sz; ++i) {
        r = Locate(GetKey<KEY>(t[i]), *r, hints...);
        if (!r.Found()) {
          (m_data.*insert)(*r, t + i, 1);
        } else if (replace) {
          *(*(*r)) = t[i];
        }
      }
      return r;
    }
    return Res();
  }
  template<typename H, typename... Hints>
  Res Merge(H* t, size_t t_sz,
            Iter (C::*insert)(const Iter&, H*, size_t),
            bool replace,
            Hints... hints) {
    if (m_data.Reserve(t_sz)) {
      H* t_end = t + t_sz;
      H* a = t;
      Iter a_it;
      while (t < t_end) {
        Res r = Locate(GetKey<KEY>(*t), *r, hints...);
        if (r.Found()) {
          if (replace) {
            *(*(*r)) = *t;
          }
          (m_data.*insert)(a_it, a, t - a);
          a = t + 1;
        } else if (*r != a_it) {
          (m_data.*insert)(a_it, a, t - a);
          if (*r == m_data.End()) {
            (m_data.*insert)(*r, t, t_end - t);
            return r;
          }
          a_it = *r + (t - a);
          a = t;
        }
        ++t;
      }
      (m_data.*insert)(a_it, a, t_end - a);
      return Res(m_data.End(), false);
    }
    return Res();
  }
  static bool Less(const K& a, const K& b) {
    return Compare<LESS>(a, b);
  }
  Res Search(const Key& key, const Iter& s, const Iter& e) const {
    Iter start = s;
    Iter end = e;
    while (start < end) {
      Iter mid = start + (end - start) / 2;
      const Key& mid_key = GetKey<KEY>(*(*mid));
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
  Res LocateR(const K& key, const Iter& s, const Iter& e) const {
    return Search(key, s, e);
  }
  template<typename... Hints>
  Res LocateR(const K& key, const Iter& s, const Iter& e,
             const Iter& hint, Hints... hints) const {
    if (hint < s || hint >= e) {
      return LocateR(key, s, e, hints...);
    }
    const K& hint_key = GetKey<KEY>(*(*hint));
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
template<typename C,
         auto LESS = (bool (*) (const typename C::Type&,
                                const typename C::Type&)) NULL>
using Set = SetT<C, typename C::Type,
                 (const typename C::Type& (*)(const typename C::Type)) NULL,
                 LESS>;
template<typename C,
         auto LESS = (bool (*) (const typename C::Type::FirstType&,
                                const typename C::Type::FirstType&)) NULL>
using Map = SetT<C, typename C::Type::FirstType, C::Type::GetFirst, LESS>;
}

#endif
