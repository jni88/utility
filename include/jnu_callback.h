#ifndef JNU_CALLBACK_H
#define JNU_CALLBACK_H

#include <type_traits>

namespace jnu {
namespace callback {
template<typename T>
class Callback {
  template<typename R>
  using NoVoid = typename std::enable_if
                 <!std::is_same<R, void>::value, R>::type;
  template<typename... A>
  using RType = typename std::result_of<T(A...)>::type;
public:
  typedef T Type;
  Callback() {
  }
  Callback(const T& obj)
    : m_obj (obj) {
  }
  Callback(const Callback& c) {
    *this = c; 
  }
  Callback& operator=(const Callback& c) {
    m_obj = c.m_obj;
    return *this;
  }
  bool operator==(const Callback& c) const {
    return m_obj == c.m_obj;
  }
  bool operator!=(const Callback& c) const {
    return !(*this == c);
  }
  operator bool() const {
    return (bool) m_obj;
  }
  template<typename... A>
  NoVoid<RType<A...>>
  operator()(const NoVoid<RType<A...>> & d,
             A... arg) {
    if (*this) {
      return m_obj(arg...);
    }
    return d;
  }
  template<typename... H>
  void operator()(H... arg) {
    if (*this) {
      m_obj(arg...);
    }
  }
  const T& Get() const {
    return m_obj;
  }
private:
  T m_obj;
};
template<typename R, typename... H>
class Func {
public:
  typedef R(*Type)(H...);
  typedef R RType;
  Func(const Type& func = NULL)
    : m_func (func) {
  }
  ~Func() {
  }
  Func(const Func& f) {
    *this = f;
  }
  Func& operator=(const Func& f) {
    m_func = f.m_func;
    return *this;
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
  R operator()(H... arg) {
    return (*m_func)(arg...);
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
  typedef R RType;
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
    return *this;
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
  R operator()(H... arg) {
    return m_func(m_arg, arg...);
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
