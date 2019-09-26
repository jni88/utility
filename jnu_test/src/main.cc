#include <stdio.h>
#include <stdlib.h>
#include "jnu_list_test.h"

using namespace jnu_test;

class JnuTest : public jnu::TestCase{
  void Test() {
    Run<ListTest>("list");
  }
};
int main() {
  JnuTest t;
  t.Start("jnu library");
  t.Exit();
}
