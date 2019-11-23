#ifndef JNU_STRING_H
#define JNU_STRING_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <string>
#include "jnu_array.h"

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
    return CompareT<StrCmp>(s, sz...);
  }
  template<typename T, typename... S>
  int CompareCase(const T& s, S... sz) const {
    return CompareT<StrCmpCase>(s, sz...);
  }
  template<typename T, typename... S>
  int CompareA(const T& s, S... sz) const {
    return CompareAT<StrCmp>(s, sz...);
  }
  template<typename T, typename... S>
  int CompareACase(const T& s, S... sz) const {
    return CompareAT<StrCmpCase>(s, sz...);
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
  template<typename T, typename... S>
  bool IsSame(const T& t, S... sz) const {
    StringView v(t, sz...);
    return m_size == v.m_size &&
           (IsEmpty() || m_data == v.m_data);
  }
  template<typename T, typename... S>
  bool IsSameData(const T& t, S... sz) const {
    StringView v(t, sz...);
    return m_data == v.m_data ||
           (IsEmpty() && v.IsEmpty());
  }
  template<typename T, typename... S>
  const char* SubStr(const T& t, S... sz) const {
    StringView v(t, sz...);
    return (const char*) memmem(m_data, m_size,
                                v.m_data, v.m_size);
  }
  void ExpandBegin(size_t t) {
    m_data -= t;
    m_size += t;
  }
  void ShrinkBegin(size_t t) {
    m_data += t;
    m_size = t >= m_size ? 0 : m_size - t;
  }
  void SetBegin(const char* t) {
    m_size -= (t - m_data);
    m_data = t;
  }
  void ExpandEnd(size_t t) {
    m_size += t;
  }
  void ShrinkEnd(size_t t) {
    m_size = t >= m_size ? 0 : m_size - t;
  }
  void SetEnd(const char* t) {
    m_size = t > m_data ? t - m_data : 0;
  }
  void SetSize(size_t sz) {
    m_size = sz;
  }
  const char* Begin() const {
    return m_data;
  }
  const char* End() const {
    return m_data + m_size;
  }
  const char* RBegin() const {
    return m_data + m_size - 1;
  }
  const char* REnd() const {
    return m_data - 1;
  }
  const char* FindFirst(char c) const {
    return (const char*) memchr(m_data, c, m_size);
  }
  const char* FindLast(char c) const {
    return (const char*) memrchr(m_data, c, m_size);
  }
  const char* FindFirstNot(char c) const {
    const char* end = End();
    for (const char* t = Begin(); t < end; ++t) {
      if (*t != c) {
        return t;
      }
    }
    return NULL;
  }
  const char* FindLastNot(char c) const {
    const char* rend = REnd();
    for (const char* t = RBegin(); t > rend; --t) {
      if (*t != c) {
        return t;
      }
    }
    return NULL;
  }
  const char* FindFirst(const char* ca) const {
    if (ca) {
      StringView v(*this);
      while(!v.IsEmpty() && *ca != TERM) {
        if (const char* t = v.FindFirst(*ca)) {
          v.SetEnd(t);
        }
        ++ca;
      }
      if (v.End() < End()) {
        return v.End();
      }
    }
    return NULL;
  }
  const char* FindLast(const char* ca) const {
    if (ca) {
      StringView v(*this);
      while(!v.IsEmpty() && *ca != TERM) {
        if (const char* t = v.FindLast(*ca)) {
          v.SetBegin(t + 1);
        }
        ++ca;
      }
      if (v.Begin() > Begin()) {
        return v.Begin() - 1;
      }
    }
    return NULL;
  }
  const char* FindFirstNot(const char* ca) const {
    StringView v(ca);
    const char* end = End();
    for (const char* t = Begin(); t < end; ++t) {
      if (!v.FindFirst(*t)) {
        return t;
      }
    }
    return NULL;
  }
  const char* FindLastNot(const char* ca) const {
    StringView v(ca);
    const char* rend = REnd();
    for (const char* t = RBegin(); t > rend; --t) {
      if (!v.FindFirst(*t)) {
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
  template<typename T>
  void TrimBegin(const T& c) {
    if (const char* t = FindFirstNot(c)) {
      SetBegin(t);
    }
  }
  template<typename T>
  void TrimEnd(const T& c) {
    if (const char* t = FindLastNot(c)) {
      SetEnd(t + 1);
    }
  }
  template<typename T>
  void Trim(const T& c) {
    TrimBegin(c);
    TrimEnd(c);
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
  static int ChrCmp(char a, char b) {
    return (int) a - (int) b;
  }
  static int ChrCmpCase(char a, char b) {
    return toupper(a) - toupper(b);
  }
  static int StrCmp(const char* a, const char* b, size_t sz) {
    return memcmp(a, b, sz);
  }
  static int StrCmpCase(const char* a, const char* b, size_t sz) {
    return strncasecmp(a, b, sz);
  }
  template<int (*F)(const char*, const char*, size_t),
           typename T, typename... S>
  int CompareT(const T& s, S... sz) const {
    StringView v(s, sz...);
    return m_size < v.m_size ? -1 :
           m_size > v.m_size ? 1 :
           (*F)(m_data, v.m_data, m_size);
  }
  template<int (*F)(const char*, const char*, size_t),
           typename T, typename... S>
  int CompareAT(const T& s, S... sz) const {
    StringView v(s, sz...);
    int r = 0;
    size_t size = m_size;
    if (m_size < v.m_size) {
      r = -1;
    } else if (m_size > v.m_size) {
      r = 1;
      size = v.m_size;
    }
    int p = (*F)(m_data, v.m_data, size);
    return p == 0 ? r : p;
  }
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
