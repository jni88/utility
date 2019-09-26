#include "jnu_list_test.h"
#include "jnu_list.h"

using namespace jnu_test;

struct Item {
  Item*& Next() {
    return m_next;
  }
  Item* m_next;
};
void SListTest::Test() {
  typedef jnu::SList<Item, &Item::Next> List;
  List ls;
  Item a, b, c, d;
  ls.InsertHead(&b);
  ls.InsertHead(&a);
  ls.InsertTail(&c);
  ls.InsertTail(&d);
  JNU_UT_EQUAL(ls.Size(), 4);
}
void ListTest::Test() {
  Run<SListTest>("single list");
}
