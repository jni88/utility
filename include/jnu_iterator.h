#ifndef JNU_ITERATOR_H
#define JNU_ITERATOR_H

namespace jnu {
template<typename T, T INV>
class Iterator {
public:
  typedef T Type;
  Iterator()
    : m_it (INV) {
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
  operator bool() const {
    return m_it != INV;
  }
  template<typename C>
  bool operator==(const C& c) const {
    return m_it == c;
  }
  template<typename C>
  bool operator!=(const C& c) const {
    return m_it != c;
  }
  template<typename C>
  bool operator>(const C& c) const {
    return m_it > c;
  }
  template<typename C>
  bool operator<(const C& c) const {
    return m_it < c;
  }
  template<typename C>
  bool operator>=(const C& c) const {
    return m_it >= c;
  }
  template<typename C>
  bool operator<=(const C& c) const {
    return m_it <= c;
  }
  template<typename C>
  Iterator& operator+=(const C& c) {
    m_it += c;
    return *this;
  }
  template<typename C>
  Iterator& operator-=(const C& c) {
    m_it -= c;
    return *this;
  }
  template<typename C>
  Iterator operator+(const C& c) const {
    return Iterator(m_it + c);
  }
  template<typename C>
  Iterator operator-(const C& c) const {
    return Iterator(m_it - c);
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
