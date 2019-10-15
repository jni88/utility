// By JNI
// Test cases of callback

#include "jnu_callback_test.h"

using namespace jnu_test;

// Global count
static int count = 0;
// Test callback function with integer return 
int TestFuncInt(int a) {
  ++count;
  return a;
}
// Test callback function with void return
void TestFuncVoid(int a) {
  ++count;
}
// Test callback functor
struct TestFunc {
  TestFunc()
    : m_on (true) {
  }
  // Check if functor is valid
  operator bool() const {
    return m_on;
  }
  // Callback function
  int operator()(int a) {
    ++count;
    return a;
  }
  bool m_on;
};
// Test callback with general object
struct TestObj {
  // Callback with integer input and return
  int operator()(int a) {
    ++count;
    return a;
  }
  // Callback with void return
  void operator()() {
    ++count;
  }
  // Member function callback with
  // integer input and return
  int Run(int a) {
    ++count;
    return a;
  }
  // Member function callback with
  // void return
  void Run() {
    ++count;
  }
};
// Main test entry
void CallbackTest::Test() {
  // Test of callback function with interger return
  jnu::Callback<jnu::Func<int, int>> a(NULL);
  JNU_UT_CHECK(a(-1, 100) == -1 && count == 0);
  a = TestFuncInt;
  JNU_UT_CHECK(a(-1, 100) == 100 && count == 1);
  a(100);
  JNU_UT_CHECK(count == 2);
  // Test of callback function with void return
  jnu::Callback<void(*)(int)> b(TestFuncVoid);
  b(100);
  JNU_UT_CHECK(count == 3);
  // Test of callback functor
  jnu::Callback<TestFunc> c;
  JNU_UT_CHECK(c(-1, 100) == 100 && count == 4);
  c.Get().m_on = false;
  JNU_UT_CHECK(c(-1, 100) == -1 && count == 4);
  c.Get().m_on = true;
  JNU_UT_CHECK(c(-1, 100) == 100 && count == 5);
  c(100);
  JNU_UT_CHECK(count == 6);
  // Test of callback with general object
  // Using general member function as callback
  TestObj to;
  jnu::FuncObj<TestObj, int, int> obj;
  obj.GetObj() = to;
  obj.GetFunc() = &TestObj::Run;
  jnu::Callback<jnu::FuncObj<TestObj, int, int>> d(obj);
  JNU_UT_CHECK(d(-1, 100) == 100 && count == 7);
  d.Get().GetFunc() = NULL;
  JNU_UT_CHECK(d(-1, 100) == -1 && count == 7);
  jnu::FuncObj<TestObj*, void> ptr(NULL, &TestObj::Run);
  jnu::Callback<jnu::FuncObj<TestObj*, void>> e(ptr);
  e();
  JNU_UT_CHECK(count == 7);
  e.Get().GetObj() = &to;
  e();
  JNU_UT_CHECK(count == 8);
  // Using operator as callback
  jnu::FuncObj<TestObj, int, int> functor(TestObj(), &TestObj::operator());
  jnu::Callback<jnu::FuncObj<TestObj, int, int>>f(functor);
  JNU_UT_CHECK(f(-1, 100) == 100 && count == 9);
  jnu::FuncObj<TestObj*, void> fptr(NULL, &TestObj::operator());
  jnu::Callback<jnu::FuncObj<TestObj*, void>>fp(fptr);
  fp();
  JNU_UT_CHECK(count == 9);
  fp.Get().GetObj() = &to;
  fp();
  JNU_UT_CHECK(count == 10);
}
