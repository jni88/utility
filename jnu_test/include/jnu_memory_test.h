#ifndef JNU_MEMORY_TEST_H
#define JNU_MEMORY_TEST_H

#include "jnu_unit_test.h"
#include "jnu_memory.h"

namespace jnu_test {
class MMTest : public jnu::TestCase {
  void TestBuildin();
  void TestCustom();
  void Test();
};
class MemoryTest : public jnu::TestCase {
  void Test();
};
}

#endif
