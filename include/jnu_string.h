#ifndef JNU_STRING_H
#define JNU_STRING_H

#include <string>
#include "jnu_array.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

namespace jnu {
class StringView {
public:
  static constexpr char TERM = '\0';
  StringView()
    : m_size (0) {
  }
  StringView(const char* s, const size_t sz)
    : m_data (s),
      m_size (s ? sz : 0) {
  }
  StringView(const char* s) {
    *this = s;
  }
  StringView(const std::string& s) {
    *this = s;
  }
  template<typename T>
  StringView(const T& s)
    : m_data (s.Data()),
      m_size (s.Size()) {
  }
  StringView& operator=(const char* s) {
    m_data = s;
    m_size = s ? (const char*) rawmemchr(s, TERM) - s : 0;
    return *this;
  }
  StringView& operator=(const std::string& s) {
    m_data = s.c_str(),
    m_size = s.length();
    return *this;
  }
  template<typename T>
  StringView& operator=(const T& s) {
    m_data = s.Data();
    m_size = s.Size();
    return *this;
  }
  char operator[](size_t i) const {
    return m_data[i];
  }
  template<typename T, typename... S>
  int Compare(const T& s, S... sz) const {
    StringView v(s, sz...);
    return m_size < v.m_size ? -1 :
           m_size > v.m_size ? 1 :
           memcmp(m_data, v.m_data, m_size);
  }
  template<typename T, typename... S>
  int CompareA(const T& s, S... sz) const {
    StringView v(s, sz...);
    int r = 0;
    size_t size = m_size;
    if (m_size < v.m_size) {
      r = -1;
    } else if (m_size > v.m_size) {
      r = 1;
      size = v.m_size;
    }
    int p = memcmp(m_data, v.m_data, size);
    return p == 0 ? r : p;
  }
  template<typename T>
  bool operator==(const T& s) const {
    return Compare(s) == 0;
  }
  template<typename T>
  bool operator!=(const T& s) const {
    return Compare(s) != 0;
  }
  template<typename T>
  bool operator<(const T& s) const {
    return CompareA(s) < 0;
  }
  template<typename T>
  bool operator>(const T& s) const {
    return CompareA(s) > 0;
  }
  const char* FindFirst(char c) const {
    return (const char*) memchr(m_data, c, m_size);
  }
  const char* FindFirst(const char* ca) const {
    StringView v(ca);
    size_t sz = m_size;
    for (size_t i = 0; i < sz; ++i) {
      const char* t = m_data + i;
      if (v.FindFirst(*t)) {
        return t;
      }
    }
    return NULL;
  }
  const char* FindLast(char c) const {
    return (const char*) memrchr(m_data, c, m_size);
  }
  const char* FindLast(const char* ca) const {
    StringView v(ca);
    for (size_t i = m_size; i > 0; --i) {
      const char* t = m_data + i - 1;
      if (v.FindFirst(*t)) {
        return t;
      }
    }
    return NULL;
  }
  template<typename T>
  StringView BreakFirst(const T& c) {
    if (const char* t = FindFirst(c)) {
      StringView v(m_data, t - m_data);
      m_data = t + 1;
      m_size -= (v.Size() + 1);
      return v;
    }
    StringView v(*this);
    m_size = 0;
    return v;
  }
  template<typename T>
  StringView BreakLast(const T& c) {
    if (const char* t = FindLast(c)) {
      StringView v(t + 1, m_size - (t - m_data) - 1);
      m_size = t - m_data;
      return v;
    }
    StringView v(*this);
    m_size = 0;
    return v;
  }
  void ExpandFront(size_t t) {
    m_data -= t;
    m_size += t;
  }
  void ShrinkFront(size_t t) {
    m_data += t;
    m_size = t >= m_size ? 0 : m_size - t;
  }
  void ExpandBack(size_t t) {
    m_size += t;
  }
  void ShrinkBack(size_t t) {
    m_size = t >= m_size ? 0 : m_size - t;
  }
  const char* Data() const {
    return m_data;
  }
  size_t Size() const {
    return m_size;
  }
  bool IsEmpty() const {
    return m_size <= 0;
  }
private:
  const char* m_data;
  size_t m_size;
};
template<typename T>
class StringImp {
public:
  static constexpr char TERM = StringView::TERM;
  typedef T Type;
  StringImp() {
    m_arr.Insert(m_arr.Begin(), TERM, 1);
  }
  const char* Data() const {
    return m_arr.Data();
  }
  char* Data() {
    return m_arr.Data();
  }
  size_t Size() const {
    return m_arr.Size() - 1;
  }
private:
  T m_arr;
};
template<size_t S>
using SString = StringImp<SArray<char, S, ARR_MEM_ALLOC>>;
}

#endif
