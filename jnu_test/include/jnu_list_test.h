// By JNI
// Test of single and double link list

#ifndef JNU_LIST_TEST_H
#define JNU_LIST_TEST_H

#include "jnu_unit_test.h"
#include "jnu_list.h"

namespace jnu_test {
struct Item;
typedef jnu::SLink<Item> SLink;  // Define of single link
typedef jnu::DLink<Item> DLink;  // Define of double link
// Test object
struct Item {
  typedef SLink::Node SNode;  // Define of single link node
  typedef DLink::Node DNode;  // Define of double link node
  // Access single link node
  SNode& GetSNode() {
    return m_s_node;
  }
  // Access double link node
  DNode& GetDNode() {
    return m_d_node;
  }
  SNode m_s_node;  // Single link node
  DNode m_d_node;  // Double link node
};
// Test of single link list
class SListTest : public jnu::TestCase {
  typedef SLink::List<&Item::GetSNode> List;  // Define of single list
  // Test of list insertion
  void TestInsert();
  // Test of list deletion
  void TestDelete();
  // Main test entry
  void Test();
  List ls, ls_a;  // Test lists
  Item a, b, c, d, e, f, g, h;  // Test objects
};
// Test of double link list
class DListTest : public jnu::TestCase {
  typedef DLink::List<&Item::GetDNode> List;  // Define of double list
  // Test of list insertion
  void TestInsert();
  // Test of list deletion
  void TestDelete();
  // Main test entry
  void Test();
  List ls, ls_a;  // Test lists
  Item a, b, c, d, e, f, g, h;  // Test objects
};
// Test of single and double link lists
class ListTest : public jnu::TestCase {
  // Main test entry
  void Test();
};
}

#endif
