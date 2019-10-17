#include "jnu_array_test.h"
#include <utility>

using namespace jnu_test;

void SArrayTest::Test() {
  typedef jnu::SArray<char, 16, jnu::ARR_MEM_ALLOC> CharArr;
  CharArr a;
  a.Insert(a.Begin(), 'c', 10);
  a.Insert(a.End(), 'v', 1);
  a[6] = 't';
  a.DeleteFlip(a.Begin(), 3);
  printf("str = %.*s\n", (int)a.Size(), a.Data());
}
void ArrayTest::Test() {
  Run<SArrayTest>("static array");
}
