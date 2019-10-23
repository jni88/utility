// By JNI
// Test cases of array

#ifndef JNU_ARRAY_TEST_H
#define JNU_ARRAY_TEST_H

#include "jnu_unit_test.h"
#include "jnu_array.h"

namespace jnu_test {
// Template for test different types array
template<typename C>
class ArrayTestImp : public jnu::TestCase {
  // Main test entry
  void Test();
};
// Array tests
class ArrayTest : public jnu::TestCase {
  // Main test entry
  void Test();
};
}

#endif
