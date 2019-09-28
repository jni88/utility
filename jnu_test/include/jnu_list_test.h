#ifndef JNU_LIST_TEST_H
#define JNU_LIST_TEST_H

#include "jnu_unit_test.h"
#include "jnu_list.h"

namespace jnu_test {
struct Item;
typedef jnu::SLink<Item> SLink;
struct Item {
  typedef SLink::Node SNode;
  SNode& GetSNode() {
    return m_s_node;
  }
  SNode m_s_node;
};
class SListTest : public jnu::TestCase {
  typedef SLink::List<&Item::GetSNode> List;
  void TestInsert();
  void TestDelete();
  void Test();
  List ls, ls_a;
  Item a, b, c, d, e, f, g, h;
};
class ListTest : public jnu::TestCase {
  void Test();
};
}

#endif
