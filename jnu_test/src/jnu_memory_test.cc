#include "jnu_memory_test.h"
#include <utility>

using namespace jnu_test;

void MMTest::TestBuildin() {
  jnu::memory::MMBuildin mm;
  jnu::memory::Mem r(&mm);
  JNU_UT_CHECK(r.Malloc(0, 0) && !r);
  JNU_UT_CHECK(!r.Malloc(3, 100));
  JNU_UT_CHECK(r.Malloc(0, 100) && r);
  JNU_UT_CHECK(r.IsAligned(8));
  JNU_UT_CHECK(r.Malloc(128, 100));
  JNU_UT_CHECK(r.IsAligned(128));
}
struct ObjTest {
  ObjTest(int a)
    : m_val (a) {
  }
  ~ObjTest() {
  }
  int m_val;
};
void MMTest::TestCustom() {
  jnu::memory::MMCustomDef mm;
  jnu::memory::Mem r(&mm);
  JNU_UT_CHECK(r.Malloc(0, 0) && !r);
  JNU_UT_CHECK(!r.Malloc(3, 100));
  JNU_UT_CHECK(r.Malloc(0, 100) && r);
  JNU_UT_CHECK(r.IsAligned(8));
  r.Free();
  JNU_UT_CHECK(r.Realloc(128, 100) && r);
  JNU_UT_CHECK(r.IsAligned(128));
  void* ptr = r.Ptr();
  strcpy((char*)ptr, "TestCustom");
  JNU_UT_CHECK(r.Realloc(256, 200));
  JNU_UT_CHECK(strcmp((char*)r.Ptr(), "TestCustom") == 0);
  JNU_UT_CHECK(r.Ptr() != ptr);
  JNU_UT_CHECK(r.IsAligned(256));
  ptr = r.Ptr();
  JNU_UT_CHECK(r.Realloc(256, 100));
  JNU_UT_EQUAL(r.Ptr(), ptr);
  JNU_UT_CHECK(r.Realloc(256, 0) && !r);
  jnu::memory::Obj<ObjTest> oa(&mm);
  int oa_sz = 10;
  int val = 13579;
  JNU_UT_CHECK(oa.NewArr(128, oa_sz, val) && oa);
  JNU_UT_CHECK(oa.IsAligned(128));
  for (int i = 0; i < oa_sz; ++i) {
    JNU_UT_EQUAL(oa.Ptr()[i].m_val, val);
  }
}
void MMTest::Test() {
  TestBuildin();
  TestCustom();
}
void MemoryTest::Test() {
  Run<MMTest>("Memory management interface");
}

