#include "jnu_callback_test.h"

using namespace jnu_test;

static int count = 0;
int TestFuncInt(int a) {
  ++count;
  return a;
}
void TestFuncVoid(int a) {
  ++count;
}
struct TestFunc {
  TestFunc()
    : m_on (true) {
  }
  operator bool() const {
    return m_on;
  }
  int operator()(int a) {
    ++count;
    return a;
  }
  bool m_on;
};
struct TestObj {
  int operator()(int a) {
    ++count;
    return a;
  }
  void operator()() {
    ++count;
  }
  int Run(int a) {
    ++count;
    return a;
  }
  void Run() {
    ++count;
  }
};
void CallbackTest::Test() {
  jnu::Callback<jnu::Func<int, int>> a(NULL);
  JNU_UT_CHECK(a(-1, 100) == -1 && count == 0);
  a = TestFuncInt;
  JNU_UT_CHECK(a(-1, 100) == 100 && count == 1);
  a(100);
  JNU_UT_CHECK(count == 2);
  jnu::Callback<void(*)(int)> b(TestFuncVoid);
  b(100);
  JNU_UT_CHECK(count == 3);
  jnu::Callback<TestFunc> c;
  JNU_UT_CHECK(c(-1, 100) == 100 && count == 4);
  c.Get().m_on = false;
  JNU_UT_CHECK(c(-1, 100) == -1 && count == 4);
  c.Get().m_on = true;
  JNU_UT_CHECK(c(-1, 100) == 100 && count == 5);
  c(100);
  JNU_UT_CHECK(count == 6);
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
}
