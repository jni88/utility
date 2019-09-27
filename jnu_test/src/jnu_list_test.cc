#include "jnu_list_test.h"
#include "jnu_list.h"

using namespace jnu_test;

struct Item;
typedef jnu::SLink<Item> SLink;
struct Item {
  typedef SLink::Node Node;
  Node& GetNode() {
    return m_node;
  }
  Node m_node;
};
void SListTest::Test() {
  typedef SLink::List<&Item::GetNode> List;
  List ls;
  Item a, b, c, d;
  ls.InsertHead(b);
  ls.InsertHead(a);
  ls.InsertTail(c);
  ls.InsertTail(d);
  JNU_UT_EQUAL(ls.Size(), 4);
  JNU_UT_EQUAL(ls.Head(), &a);
  JNU_UT_EQUAL(ls.Next(a), &b);
  JNU_UT_EQUAL(ls.Next(b), &c);
  JNU_UT_EQUAL(ls.Next(c), &d);
}
void ListTest::Test() {
  Run<SListTest>("single list");
}
