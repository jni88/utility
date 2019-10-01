#ifndef JNU_LIST_TEST_H
#define JNU_LIST_TEST_H

#include "jnu_unit_test.h"
#include "jnu_list.h"

namespace jnu_test {
struct Item;
typedef jnu::SLink<Item> SLink;
typedef jnu::DLink<Item> DLink;
struct Item {
  typedef SLink::Node SNode;
  typedef DLink::Node DNode;
  SNode& GetSNode() {
    return m_s_node;
  }
  DNode& GetDNode() {
    return m_d_node;
  }
  SNode m_s_node;
  DNode m_d_node;
};
class SListTest : public jnu::TestCase {
  typedef SLink::List<&Item::GetSNode> List;
  void TestInsert();
  void TestDelete();
  void Test();
  List ls, ls_a;
  Item a, b, c, d, e, f, g, h;
};
class DListTest : public jnu::TestCase {
  typedef DLink::List<&Item::GetDNode> List;
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
