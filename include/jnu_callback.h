#ifndef JNU_CALLBACK_H
#define JNU_CALLBACK_H

#include <type_traits>

namespace jnu {
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
    return m_obj;
  }
  template<typename... A>
  NoVoid<RType<A...>>
  operator()(const NoVoid<RType<A...>> & d, A... arg) {
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
using Func = R(*)(H...);
template<typename R, typename A, typename... H>
class FuncArg {
public:
  typedef Func<R, A&, H...> FType;
  typedef A AType;
  FuncArg() {
  }
  FuncArg(const FType& func)
    : m_func (func) {
  }
  FuncArg(const FType& func, const A& arg)
    : m_func (func),
      m_arg (arg) {
  }
  ~FuncArg() {
  }
  FuncArg(const FuncArg& f) {
    *this = f;
  }
  FuncArg& operator=(const FuncArg& f) {
    m_func = f.m_func;
    m_arg = f.m_arg;
    return *this;
  }
  bool operator==(const FuncArg& f) const {
    return m_func == f.m_func && m_arg == f.m_arg;
  }
  bool operator!=(const FuncArg& f) const {
    return !(*this == f);
  }
  operator bool() const {
    return m_func;
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
template<typename C, typename R, typename... H>
class FuncObj {
  template<typename T>
  using GetType = typename std::remove_pointer<T>::type;
  template<typename T>
  using Obj = typename std::enable_if
              <std::is_same<GetType<T>, T>::value, T>::type;
  template<typename T>
  using Ptr = typename std::enable_if
              <!std::is_same<GetType<T>, T>::value, T>::type;
public:
  typedef C OType;
  typedef R(GetType<C>::*FType)(H...);
  const static bool NOT_POINTER = std::is_same<C, OType>::value;
  FuncObj()
    : m_func (NULL) {
  }
  FuncObj(const C& obj, const FType& func)
    : m_obj (obj),
      m_func (func) {
  }
  ~FuncObj() {
  }
  FuncObj(const FuncObj& o) {
    *this = o;
  }
  FuncObj& operator=(const FuncObj& o) {
    m_obj = o.m_obj;
    m_func = o.m_func;
    return *this;
  }
  bool operator==(const FuncObj& o) const {
    return m_obj == o.m_obj && m_func == o.m_func;
  }
  bool operator!=(const FuncObj& o) const {
    return !(*this == o);
  }
  operator bool() const {
    return m_obj && m_func;
  }
  template<typename T>
  R Exec(Obj<T>& obj, H... arg) {
    return (obj.*m_func)(arg...);
  }
  template<typename T>
  R Exec(Ptr<T> obj, H... arg) {
    return (obj->*m_func)(arg...);
  }
  R operator()(H... arg) {
    return Exec<C>(m_obj, arg...);
  }
  const C& GetObj() const {
    return m_obj;
  }
  const FType& GetFunc() const {
    return m_func;
  }
private:
  C m_obj;
  FType m_func;
};
}

#endif
