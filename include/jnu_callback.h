#ifndef JNU_CALLBACK_H
#define JNU_CALLBACK_H

#include <type_traits>

namespace jnu {
namespace callback {
template<typename R>
using NoVoid = typename std::enable_if<!std::is_same<R, void>::value, R>;
template<typename R, typename... H>
class Func {
public:
  typedef R(*Type)(H...);
  Func(const Type& func = NULL)
    : m_func (func) {
  }
  ~Func() {
  }
  Func(const Func& f) {
    *this = f;
  }
  Func& operator=(const Func& f) {
    return m_func = f.m_func;
  }
  bool operator==(const Func& f) const {
    return m_func == f.m_func;
  }
  bool operator!=(const Func& f) const {
    return !(*this == f);
  }
  operator bool() const {
    return m_func != NULL;
  }
  template<typename T = NoVoid<R>>
  typename T::type operator()(const typename T::type& d, H... arg) {
    if (m_func) {
      return (*m_func)(arg...);
    }
    return d;
  }
  void operator()(H... arg) {
    if (m_func) {
      (*m_func)(arg...);
    }
  }
  const Type& Get() const {
    return m_func;
  }
private:
  Type m_func;
};
template<typename R, typename A, typename... H>
class FuncA {
public:
  typedef Func<R, A&, H...> FType;
  typedef A AType;
  typedef typename FType::Type Type;
  FuncA(const Type& func = NULL)
    : m_func (func) {
  }
  FuncA(const Type& func, const A& arg)
    : m_func (func),
      m_arg (arg) {
  }
  ~FuncA() {
  }
  FuncA(const FuncA& f) {
    *this = f;
  }
  FuncA& operator=(const FuncA& f) {
    m_func = f.m_func;
    m_arg = f.m_arg;
  }
  bool operator==(const FuncA& f) const {
    return m_func == f.m_func && m_arg == f.m_arg;
  }
  bool operator!=(const FuncA& f) const {
    return !(*this == f);
  }
  operator bool() const {
    return m_func == true;
  }
  template<typename T = NoVoid<R>>
  typename T::type operator()(const typename T::type& d, H... arg) {
    return m_func(d, m_arg, arg...);
  }
  void operator()(H... arg) {
    if (m_func) {
      m_func(m_arg, arg...);
    }
  }
  const FType& GetFunc() const {
    return m_func;
  }
  const A& GetArg() const {
    return m_arg;
  }
private:
  FType m_func;
  A m_arg;
};
}
}

#endif
