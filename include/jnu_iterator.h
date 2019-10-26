// By JNI
// General iterator template
// It is suitable for most of
// primary or custom defined
// data structures

#ifndef JNU_ITERATOR_H
#define JNU_ITERATOR_H

namespace jnu {
// General iterator template
// Template arguments:
// T: underline data type
// INV: Define of invaild value,
//      for example, for pointers
//      invalid value usually is
//      NULL
template<typename T, T INV>
class Iterator {
public:
  typedef T Type;  // Define of underline type
  constexpr static T INV_VAL = INV;
  // Default constructor,
  // Initialize underline value to
  // invalid
  Iterator()
    : m_it (INV) {
  }
  // Constructor with input value
  Iterator(const T& it)
    : m_it (it) {
  }
  // Copy constructor
  Iterator(const Iterator& it) {
    *this = it;
  }
  // Assign operator with input value
  Iterator& operator=(const T& it) {
    m_it = it;
    return *this;
  }
  // Assign operator with another iterator
  Iterator& operator=(const Iterator& it) {
    return *this = it.m_it;
  }
  // Check if iterator is valid
  bool IsValid() const {
    return m_it != INV;
  }
  // Template for equal operator
  // C: input type
  template<typename C>
  bool operator==(const C& c) const {
    return m_it == c;
  }
  // Template for not equal operator
  template<typename C>
  bool operator!=(const C& c) const {
    return m_it != c;
  }
  // Template for bigger operator
  template<typename C>
  bool operator>(const C& c) const {
    return m_it > c;
  }
  // Template for smaller operator
  template<typename C>
  bool operator<(const C& c) const {
    return m_it < c;
  }
  // Template for bigger or equal operator
  template<typename C>
  bool operator>=(const C& c) const {
    return m_it >= c;
  }
  // Template for smaller or equal operator
  template<typename C>
  bool operator<=(const C& c) const {
    return m_it <= c;
  }
  // Template for add assign operator
  template<typename C>
  Iterator& operator+=(const C& c) {
    m_it += c;
    return *this;
  }
  // Template for subtract assign operator
  template<typename C>
  Iterator& operator-=(const C& c) {
    m_it -= c;
    return *this;
  }
  // Template for add distance operator
  template<typename C>
  Iterator operator+(const C& c) const {
    return Iterator(m_it + c);
  }
  // Template for subtract distance operator
  template<typename C>
  Iterator operator-(const C& c) const {
    return Iterator(m_it - c);
  }
  // Template for pre-increment operator
  Iterator& operator++() {
    return *this += 1;
  }
  // Template for pre-decrement operator
  Iterator& operator--() {
    return *this -= 1;
  }
  // Template for post-increment operator
  Iterator operator++(int) {
    Iterator t(m_it);
    ++(*this);
    return t;
  }
  // Template for post-decrement operator
  Iterator operator--(int) {
    Iterator t(m_it);
    --(*this);
    return t;
  }
  // Access underline value
  operator T() const {
    return m_it;
  }
  const T& operator->() const {
    return m_it;
  }
  T& operator->() {
    return m_it;
  }
  const T& operator*() const {
    return m_it;
  }
  T& operator*() {
    return m_it;
  }
  // Calculate distance between two iterators
  size_t operator-(const Iterator& b) const {
    return (size_t)(m_it - b.m_it);
  }
private:
  T m_it;  // Underline value
};
}

#endif
