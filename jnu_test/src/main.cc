#include <stdio.h>
#include <stdlib.h>
#include "jnu_list_test.h"
#include "jnu_memory_test.h"
#include "jnu_callback_test.h"
#include "jnu_array_test.h"

using namespace jnu_test;

class JnuTest : public jnu::TestCase{
  void Test() {
    Run<ListTest>("list");
    Run<MemoryTest>("memory");
    Run<CallbackTest>("callback");
    Run<ArrayTest>("array");
  }
};
int main() {
  JnuTest t;
  t.Start("jnu library");
  t.Exit();
}
