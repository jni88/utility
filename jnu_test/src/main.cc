// By JNI
// Test case for utility library

#include <stdio.h>
#include <stdlib.h>
#include "jnu_list_test.h"
#include "jnu_memory_test.h"
#include "jnu_callback_test.h"
#include "jnu_array_test.h"
#include "jnu_array_set_test.h"

using namespace jnu_test;

// Test cases
class JnuTest : public jnu::TestCase{
  // Main test entry
  void Test() {
    Run<ListTest>("list");  // List test
    Run<MemoryTest>("memory");  // Memory manager test
    Run<CallbackTest>("callback");  // Callback test
    Run<ArrayTest>("array");  // Array test
    Run<ArraySetTest>("array set");  // Array set test
  }
};
// Main function
int main() {
  JnuTest t;
  t.Start("jnu library");  // Run tests
  t.Exit();
}
