// By JNI
// Unit tests of memory manage

#ifndef JNU_MEMORY_TEST_H
#define JNU_MEMORY_TEST_H

#include "jnu_unit_test.h"
#include "jnu_memory.h"

namespace jnu_test {
// Memory manage interface test
class MMTest : public jnu::TestCase {
  // Test of buildin methods
  void TestBuildin();
  // Test of custom methods
  void TestCustom();
  // Main entry of interface test
  void Test();
};
// Memory test
class MemoryTest : public jnu::TestCase {
  // General test entry
  void Test();
};
}

#endif
