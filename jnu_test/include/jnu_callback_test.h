// By JNI
// Test of callback functions

#ifndef JNU_CALLBACK_TEST_H
#define JNU_CALLBACK_TEST_H

#include "jnu_unit_test.h"
#include "jnu_callback.h"

namespace jnu_test {
// Callback test case
class CallbackTest : public jnu::TestCase {
  // Main test entry
  void Test();
};
}

#endif
