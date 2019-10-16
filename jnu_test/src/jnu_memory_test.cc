// By JNI
// Implementation of memory unit tests

#include "jnu_memory_test.h"
#include <utility>

using namespace jnu_test;

// Test of buildin methods
void MMTest::TestBuildin() {
  // Buildin interface
  jnu::memory::MMBase& mm = jnu::memory::MM_BUILDIN;
  jnu::memory::Mem r(&mm);  // Memory record
  JNU_UT_CHECK(r.Malloc(0, 0) && !r);  // Empty memory
  JNU_UT_CHECK(!r.Malloc(3, 100));  // Invalid alignment
  JNU_UT_CHECK(r.Malloc(0, 100) && r);  // Zero alignment
  JNU_UT_CHECK(r.IsAligned(8));  // Aligned with 8 (malloc)
  JNU_UT_CHECK(r.Malloc(128, 100));  // 128 alignment
  JNU_UT_CHECK(r.IsAligned(128));  // Check aligned
  jnu::memory::Mem t = std::move(r);  // Move operator
  JNU_UT_CHECK(t && !r);
}
// Test class
struct ObjTest {
  ObjTest(int a)
    : m_val (a) {
  }
  ~ObjTest() {
  }
  int m_val;
};
// Test of custom interface
void MMTest::TestCustom() {
  // Custom interface
  jnu::memory::MMBase& mm = jnu::memory::MM_CUSTOM_DEF;
  jnu::memory::Mem r(&mm);  // Memory record
  // Test Malloc
  JNU_UT_CHECK(r.Malloc(0, 0) && !r);
  JNU_UT_CHECK(!r.Malloc(3, 100));
  JNU_UT_CHECK(r.Malloc(0, 100) && r);
  JNU_UT_CHECK(r.IsAligned(8));
  // Test Realloc
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
  // Test object allocate
  jnu::memory::Obj<ObjTest> oa(&mm);
  int oa_sz = 10;
  int val = 13579;
  JNU_UT_CHECK(oa.NewArr(128, oa_sz, val) && oa);
  JNU_UT_CHECK(oa.IsAligned(128));
  for (int i = 0; i < oa_sz; ++i) {
    JNU_UT_EQUAL(oa.Ptr()[i].m_val, val);
  }
  // Test object record move operator
  jnu::memory::Obj<ObjTest> oa_t = std::move(oa);
  JNU_UT_CHECK(oa_t && !oa);
}
// Interface test
void MMTest::Test() {
  TestBuildin();  // Buildin test
  TestCustom();  // Custom test
}
// Main entry of memory test
void MemoryTest::Test() {
  // Run interface tests
  Run<MMTest>("Memory management interface");
}

