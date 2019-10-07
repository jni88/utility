#include "jnu_memory_test.h"

using namespace jnu_test;

void MMTest::TestBuildin() {
  jnu::memory::MMBuildin mm;
  jnu::memory::MRec r;
  JNU_UT_CHECK(r.Malloc(&mm, 0, 0) && !r);
  JNU_UT_CHECK(!r.Malloc(&mm, 3, 100));
  JNU_UT_CHECK(r.Malloc(&mm, 0, 100) && r);
  JNU_UT_CHECK(r.IsAligned(8));
  JNU_UT_CHECK(r.Malloc(&mm, 128, 100));
  JNU_UT_CHECK(r.IsAligned(128));
}
struct AA {
  AA() {
    printf("construction AA\n");
  }
  ~AA() {
    printf("deconstruction AA\n");
  }
};
void MMTest::TestCustom() {
  jnu::memory::MMCustomDef mm;
  jnu::memory::MRec r;
  JNU_UT_CHECK(r.Malloc(&mm, 0, 0) && !r);
  JNU_UT_CHECK(!r.Malloc(&mm, 3, 100));
  JNU_UT_CHECK(r.Malloc(&mm, 0, 100) && r);
  JNU_UT_CHECK(r.IsAligned(8));
  r.Free();
  JNU_UT_CHECK(r.Realloc(&mm, 128, 100) && r);
  JNU_UT_CHECK(r.IsAligned(128));
  void* ptr = r.Ptr();
  strcpy((char*)ptr, "TestCustom");
  JNU_UT_CHECK(r.Realloc(&mm, 256, 200));
  JNU_UT_CHECK(strcmp((char*)r.Ptr(), "TestCustom") == 0);
  JNU_UT_CHECK(r.Ptr() != ptr);
  JNU_UT_CHECK(r.IsAligned(256));
  ptr = r.Ptr();
  JNU_UT_CHECK(r.Realloc(&mm, 256, 100));
  JNU_UT_EQUAL(r.Ptr(), ptr);
  JNU_UT_CHECK(r.Realloc(&mm, 256, 0) && !r);
}
void MMTest::Test() {
  TestBuildin();
  TestCustom();
}
void MemoryTest::Test() {
  Run<MMTest>("Memory management interface");
}

