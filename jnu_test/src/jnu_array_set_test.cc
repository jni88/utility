#include <string>
#include "jnu_array.h"
#include "jnu_array_set.h"
#include "jnu_array_set_test.h"

using namespace jnu_test;
// Test object for array
struct TestObj {
  TestObj() {
    // Dynamic allocate
    // To test if objects'
    // constructor and deconstructor are
    // properly called
    m_ptr = malloc(128);
  }
  TestObj(const char* str)
    : m_str (str) {
    // Dynamic allocate
    m_ptr = malloc(128);
  }
  TestObj& operator=(const TestObj& t) {
    m_str = t.m_str;
    return *this;
  }
  TestObj& operator=(TestObj&& t) {
    m_str = std::move(t.m_str);
    return *this;
  }
  ~TestObj() {
    // Free memory
    // If freed multiple times, it will crash
    free (m_ptr);
  }
  std::string m_str;
  void* m_ptr;
};
void ArraySetTest::Test() {
  typedef jnu::HArrayPair<std::string, TestObj,
                          2, jnu::ARR_OBJ_ALLOC, 8> SArr;
  typedef jnu::ArrayMap<SArr> SMap;
  SArr a;
  SMap m1, m2;
  a.Insert(a.Begin(), SArr::Type("888", TestObj("888")), 1);
  a.Insert(a.Begin(), SArr::Type("333", TestObj("333")), 1);
  JNU_UT_CHECK(m1.Inject(a, a.Begin(), a.End(), false));
  a.Insert(a.Begin(), SArr::Type("222", TestObj("222")), 1);
  a.Insert(a.Begin(), SArr::Type("333", TestObj("aaa")), 1);
  a.Insert(a.Begin(), SArr::Type("444", TestObj("444")), 1);
  JNU_UT_CHECK(m1.Inject(a, a.Begin(), a.End(), true,
                         m1.Begin() + 1, m1.End()));
  JNU_UT_EQUAL(m1.Size(), 4);
  JNU_UT_CHECK(m1[0].First() == "222" && m1[0].Second().m_str == "222");
  JNU_UT_CHECK(m1[1].First() == "333" && m1[1].Second().m_str == "aaa");
  JNU_UT_CHECK(m1[2].First() == "444" && m1[2].Second().m_str == "444");
  JNU_UT_CHECK(m1[3].First() == "888" && m1[3].Second().m_str == "888");
  a.Insert(a.Begin(), SArr::Type("777", TestObj("777")), 1);
  a.Insert(a.Begin(), SArr::Type("444", TestObj("ccc")), 1);
  a.Insert(a.Begin(), SArr::Type("555", TestObj("555")), 1);
  a.Insert(a.Begin(), SArr::Type("999", TestObj("999")), 1);
  a.Insert(a.Begin(), SArr::Type("111", TestObj("111")), 1);
  JNU_UT_CHECK(m2.Inject(a, a.Begin(), a.End(), false));
  JNU_UT_CHECK(m1.InjectSorted(m2, m2.Begin(), m2.Begin() + 3, true,
                               m1.Begin(), m1.End()));
  JNU_UT_EQUAL(m1.Size(), 6);
  JNU_UT_EQUAL(m2.Size(), 2);
  JNU_UT_CHECK(m1[0].First() == "111" && m1[0].Second().m_str == "111");
  JNU_UT_CHECK(m1[3].First() == "444" && m1[3].Second().m_str == "ccc");
  JNU_UT_CHECK(m1[4].First() == "555" && m1[4].Second().m_str == "555");
  JNU_UT_CHECK(m1.InjectSorted(m2, m2.Begin(), m2.End(), false));
  JNU_UT_EQUAL(m1.Size(), 8);
  JNU_UT_CHECK(m2.IsEmpty());
  JNU_UT_CHECK((m1.End() - 1)->First() == "999" &&
               (m1.End() - 1)->Second().m_str == "999");
  JNU_UT_CHECK((m1.End() - 3)->First() == "777" &&
               (m1.End() - 3)->Second().m_str == "777");
  SMap::Res r = m1.Locate("000");
  JNU_UT_CHECK(!r.Found() && (*r) == m1.Begin());
  r = m1.Locate("234");
  JNU_UT_CHECK(!r.Found() && (*r) == (m1.Begin() + 2));
  r = m1.Locate("444");
  JNU_UT_CHECK(r.Found() && (*r) == (m1.Begin() + 3));
  JNU_UT_CHECK(m1.Find("111")->Second().m_str == "111");
  JNU_UT_CHECK(m1.Find("222")->Second().m_str == "222");
  JNU_UT_CHECK(m1.Find("333")->Second().m_str == "aaa");
  JNU_UT_CHECK(m1.Find("444")->Second().m_str == "ccc");
  JNU_UT_CHECK(m1.Find("555")->Second().m_str == "555");
  JNU_UT_CHECK(m1.Find("777")->Second().m_str == "777");
  JNU_UT_CHECK(m1.Find("888")->Second().m_str == "888");
  JNU_UT_CHECK(m1.Find("999")->Second().m_str == "999");
  JNU_UT_CHECK(!m1.Find("123"));
  JNU_UT_CHECK(m1.Delete(m1.End() - 2, m1.End() + 100));
  JNU_UT_EQUAL(m1.Size(), 6);
  JNU_UT_CHECK(m1.Delete(m1.Begin() - 2, 4));
  JNU_UT_EQUAL(m1.Size(), 4);
  JNU_UT_CHECK(m1[0].First() == "333" && m1[0].Second().m_str == "aaa");
  JNU_UT_CHECK(m1[1].First() == "444" && m1[1].Second().m_str == "ccc");
  JNU_UT_CHECK(m1[2].First() == "555" && m1[2].Second().m_str == "555");
  JNU_UT_CHECK(m1[3].First() == "777" && m1[3].Second().m_str == "777");
  m1.Free();
  JNU_UT_CHECK(m1.IsEmpty());
}
