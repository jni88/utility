// By JNI
// Wrap function or functors used as
// the purpose of providing callbacks to
// other procedures or threads

#ifndef JNU_CALLBACK_H
#define JNU_CALLBACK_H

#include <type_traits>

namespace jnu {
// Callback wrap of functions or functors
// It safely triggers the underline functions
// or functors
// template parementer: T - underline function type
template<typename T>
class Callback {
  // Check if type R is void
  template<typename R>
  using IsVoid = typename std::is_same<R, void>;
  // Enable if R is not a void
  template<typename R>
  using NVoid = typename std::enable_if<!IsVoid<R>::value, R>::type;
  // Template for deducing function return type
  template<typename... H>
  using RType = typename std::result_of<T(H...)>::type;
public:
  typedef T Type;  // Define of underline function type
  // Default constructor
  Callback() {
  }
  // Constructor
  Callback(const T& obj)
    : m_obj (obj) {
  }
  // Deconstructor
  ~Callback() {
  }
  // Copy constructor
  Callback(const Callback& c) {
    *this = c; 
  }
  // Assign operator
  Callback& operator=(const Callback& c) {
    m_obj = c.m_obj;
    return *this;
  }
  Callback& operator=(const T& obj) {
    m_obj = obj;
    return *this;
  }
  // Equal operator
  bool operator==(const Callback& c) const {
    return m_obj == c.m_obj;
  }
  bool operator==(const T& obj) const {
    return m_obj == obj;
  }
  // Not equal operator
  bool operator!=(const Callback& c) const {
    return !(*this == c);
  }
  bool operator!=(const T& obj) const {
    return !(*this == obj);
  }
  // Bool operator, check if it is safe for invoking
  operator bool() const {
    return m_obj;
  }
  // Compiled when return type is not void
  // H - function input argument types
  // Input: d - default value if not valid
  //        arg - function arguments
  // Return: d - fail, function return - success
  template<typename... H>
  NVoid<RType<H...>> operator()(const RType<H...>& d, H... arg) {
    if (*this) {
      return m_obj(arg...);
    }
    return d;
  }
  // Compiled when return type is void
  // H - function input argument types
  // Input: arg - function arguments
  // No return
  template<typename... H>
  void operator()(H... arg) {
    if (*this) {
      m_obj(arg...);
    }
  }
  // Get underline function
  const T& Get() const {
    return m_obj;
  }
  // Set underline function
  T& Get() {
    return m_obj;
  }
private:
  T m_obj;  // Underline function
};
// Define of function pointer
// It can be used in Callback wrap
// template parameters:
// R - return type
// H - input types
template<typename R, typename... H>
using Func = R(*)(H...);
// Wrap function with one argument
// that needs to be stored with callback
// template parameters:
// R - function return type
// A - argument type, need to be stored
// H - the rest argument types
// The function signature should be like:
template<typename R, typename A, typename... H>
class FuncArg {
public:
  typedef Func<R, A&, H...> FType;  // Function type
  typedef A AType;  // Argument type
  // Default constructor
  FuncArg() {
  }
  // Constructor with function input
  FuncArg(const FType& func)
    : m_func (func) {
  }
  // Constructor with function and argument input
  FuncArg(const FType& func, const A& arg)
    : m_func (func),
      m_arg (arg) {
  }
  // Deconstructor
  ~FuncArg() {
  }
  // Copy constructor
  FuncArg(const FuncArg& f) {
    *this = f;
  }
  // Assign operator
  FuncArg& operator=(const FuncArg& f) {
    m_func = f.m_func;
    m_arg = f.m_arg;
    return *this;
  }
  // Equal operator
  // Both function and argument are equal
  bool operator==(const FuncArg& f) const {
    return m_func == f.m_func && m_arg == f.m_arg;
  }
  // Not equal operator
  bool operator!=(const FuncArg& f) const {
    return !(*this == f);
  }
  // Bool operator (only function needs to be valid)
  operator bool() const {
    return m_func;
  }
  // Invoke operator
  R operator()(H... arg) {
    return m_func(m_arg, arg...);
  }
  // Get underline function
  const FType& GetFunc() const {
    return m_func;
  }
  // Get underline argument
  const A& GetArg() const {
    return m_arg;
  }
private:
  FType m_func;  // Function
  A m_arg;  // Stored argument
};
// Functor with defined member function
// as callback
// template parameters:
// C - Object type
// R - member function type
// H - function input argument types
template<typename C, typename R, typename... H>
class FuncObj {
  // Deduce real type from pointer (in case C is pointer)
  template<typename T>
  using GetType = typename std::remove_pointer<T>::type;
  template<typename T>
  using IsPtr = typename std::is_pointer<T>;
  // For conditional compile when C is pointer
  template<typename T>
  using Ptr = typename std::enable_if<IsPtr<T>::value, T>::type;
  // For conditional compile when C is not pointer
  template<typename T>
  using NPtr = typename std::enable_if<!IsPtr<T>::value, T>::type;
public:
  typedef R(GetType<C>::*FType)(H...);  // Member function signature
  typedef C AType;  // Underline object type
  // Default constructor
  FuncObj()
    : m_func (NULL) {
  }
  // Constructor
  FuncObj(const C& obj, const FType& func)
    : m_obj (obj),
      m_func (func) {
  }
  // Deconstructor
  ~FuncObj() {
  }
  // Copy constructor
  FuncObj(const FuncObj& o) {
    *this = o;
  }
  // Assign operator
  FuncObj& operator=(const FuncObj& o) {
    m_obj = o.m_obj;
    m_func = o.m_func;
    return *this;
  }
  // Equal operator
  bool operator==(const FuncObj& o) const {
    return m_obj == o.m_obj && m_func == o.m_func;
  }
  // Not equal operator
  bool operator!=(const FuncObj& o) const {
    return !(*this == o);
  }
  // Bool operator, both object
  // and function need to be valid
  operator bool() const {
    return CheckObj<C>(m_obj) && m_func;
  }
  // Invoke operator
  // Automatically the right version of
  // 'Exec' depending the type of C
  R operator()(H... arg) {
    return Exec<C>(m_obj, arg...);
  }
  // Get underline object
  const C& GetObj() const {
    return m_obj;
  }
  // Set underline object
  C& GetObj() {
    return m_obj;
  }
  // Get underline member function
  const FType& GetFunc() const {
    return m_func;
  }
  // Set underine object
  FType& GetFunc() {
    return m_func;
  }
private:
  // Always true when underline is object
  template<typename T>
  static bool CheckObj(const NPtr<T>& obj) {
    return true;
  }
  // When underline is pointer, need check NULL
  template<typename T>
  static bool CheckObj(const Ptr<T>& obj) {
    return obj;
  }
  // Compile only if C is not pointer
  template<typename T>
  R Exec(NPtr<T>& obj, H... arg) {
    return (obj.*m_func)(arg...);
  }
  // Compile only if C is pointer
  template<typename T>
  R Exec(Ptr<T>& obj, H... arg) {
    return (obj->*m_func)(arg...);
  }
  C m_obj;  // Underline object
  FType m_func;  // Underline member function
};
}

#endif
