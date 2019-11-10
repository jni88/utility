// By JNI
// Implement array test cases

#include "jnu_array_test.h"

using namespace jnu_test;
// General test cases for all types of array
template<typename C>
void ArrayTestImp<C>::Test() {
  typedef typename C::Type TestObj;
  C a, b, c;  // Temp array variables for test
  TestObj o_arr[2];  // Input static array
  o_arr[0].m_str = "d_0123456789";
  o_arr[1].m_str = "b_0123456789";
  // Test array insert
  JNU_UT_CHECK(a.Insert(a.Begin(), TestObj("c_0123456789"), 1));
  JNU_UT_CHECK(a.Insert(a.Begin(), TestObj("a_0123456789"), 1));
  JNU_UT_CHECK(a.Insert(a.End(), o_arr, 1));
  JNU_UT_CHECK(a.Insert(a.Begin() + 1, o_arr + 1, 1));
  JNU_UT_EQUAL(a.Size(), 4);
  JNU_UT_EQUAL(a[0].m_str, "a_0123456789");
  JNU_UT_EQUAL(a[1].m_str, "b_0123456789");
  JNU_UT_EQUAL(a[2].m_str, "c_0123456789");
  JNU_UT_EQUAL(a[3].m_str, "d_0123456789");
  // Insert (or inject) overlap array is not allowed
  JNU_UT_CHECK(!a.Insert(a.Begin(), a.Begin(), 3));
  // Test of array inject
  JNU_UT_CHECK(b.Inject(b.Begin(), o_arr, 2));
  JNU_UT_CHECK(o_arr[0].m_str.empty() && o_arr[1].m_str.empty());
  // Test of array assign
  c = b;
  JNU_UT_EQUAL(c.Size(), 2);
  JNU_UT_CHECK(c[0].m_str == b[0].m_str && c[1].m_str == b[1].m_str);
  // Test of array inject
  JNU_UT_CHECK(c.Inject(c.Begin() + 1, b, b.Begin(), b.End()));
  JNU_UT_EQUAL(c.Size(), 4);
  JNU_UT_CHECK(b.IsEmpty());
  JNU_UT_EQUAL(c[0].m_str, "d_0123456789");
  JNU_UT_EQUAL(c[1].m_str, "d_0123456789");
  JNU_UT_EQUAL(c[2].m_str, "b_0123456789");
  JNU_UT_EQUAL(c[3].m_str, "b_0123456789");
  // Test of array move
  c = std::move(a);
  JNU_UT_CHECK(a.IsEmpty());
  JNU_UT_EQUAL(c.Size(), 4);
  JNU_UT_EQUAL(c[0].m_str, "a_0123456789");
  JNU_UT_EQUAL(c[1].m_str, "b_0123456789");
  JNU_UT_EQUAL(c[2].m_str, "c_0123456789");
  JNU_UT_EQUAL(c[3].m_str, "d_0123456789");
  // Test of array delete (shift delete)
  JNU_UT_CHECK(c.Delete(c.Begin() - 10, 10));
  JNU_UT_CHECK(c.Delete(c.End(), 10));
  JNU_UT_EQUAL(c.Size(), 4);
  JNU_UT_CHECK(c.Delete(c.Begin(), 1));
  JNU_UT_EQUAL(c.Size(), 3);
  JNU_UT_EQUAL(c[0].m_str, "b_0123456789");
  JNU_UT_EQUAL(c[1].m_str, "c_0123456789");
  JNU_UT_EQUAL(c[2].m_str, "d_0123456789");
  // Test of array flip delete
  JNU_UT_CHECK(c.DeleteFlip(c.Begin(), 1));
  JNU_UT_EQUAL(c.Size(), 2);
  JNU_UT_EQUAL(c[0].m_str, "d_0123456789");
  JNU_UT_EQUAL(c[1].m_str, "c_0123456789");
  JNU_UT_CHECK(c.DeleteFlip(c.Begin(), 100));
  JNU_UT_CHECK(c.IsEmpty());
}
// Main test entry
void ArrayTest::Test() {
  // Test of static array
  typedef jnu::SArray<TestObj, 8, jnu::ARR_OBJ_ALLOC> SArr;
  Run<ArrayTestImp<SArr>>("static array");
  // Test of dynamic array
  typedef jnu::DArray<TestObj, jnu::ARR_OBJ_ALLOC, 2> DArr;
  Run<ArrayTestImp<DArr>>("dynamic array");
  // Test of hybrid array
  typedef jnu::HArray<TestObj, 2, jnu::ARR_OBJ_ALLOC, 2> HArr;
  Run<ArrayTestImp<HArr>>("hybrid array");
}
