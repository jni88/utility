#ifndef JNU_ITERATOR_H
#define JNU_ITERATOR_H

namespace jnu {
template<typename T>
class Iterator {
public:
  typedef T Type;
  Iterator() {
  }
  Iterator(const T& it)
    : m_it (it) {
  }
  Iterator(const Iterator& it) {
    *this = it;
  }
  Iterator& operator=(const T& it) {
    m_it = it;
    return *this;
  }
  Iterator& operator=(const Iterator& it) {
    return *this = it.m_it;
  }
  bool operator==(const T& it) const {
    return m_it == it;
  }
  bool operator!=(const T& it) const {
    return !(*this == it);
  }
  bool operator>(const T& it) const {
    return m_it > it;
  }
  bool operator<(const T& it) const {
    return m_it < it;
  }
  bool operator>=(const T& it) const {
    return m_it >= it;
  }
  bool operator<=(const T& it) const {
    return m_it <= it;
  }
  Iterator operator+(const T& it) const {
    return Iterator(m_it + it);
  }
  Iterator operator-(const T& it) const {
    return Iterator(m_it - it);
  }
  Iterator& operator+=(const T& it) {
    m_it += it;
    return *this;
  }
  Iterator& operator-=(const T& it) {
    m_it -= it;
    return *this;
  }
  Iterator& operator++() {
    return *this += 1;
  }
  Iterator& operator--() {
    return *this -= 1;
  }
  Iterator operator++(int) {
    return Iterator(++m_it);
  }
  Iterator operator--(int) {
    return Iterator(--m_it);
  }
  operator T() const {
    return m_it;
  }
private:
  T m_it;
};
}

#endif
