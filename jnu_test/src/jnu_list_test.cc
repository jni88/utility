#include "jnu_list_test.h"
#include <utility>

using namespace jnu_test;

void SListTest::TestInsert() {
  ls.InsertHead(c);
  ls.InsertNext(c, d);
  ls.InsertTail(e);
  JNU_UT_EQUAL(ls.Size(), 3);
  JNU_UT_EQUAL(ls.Head(), &c);
  JNU_UT_EQUAL(ls.Tail(), &e);
  JNU_UT_EQUAL(ls.Next(c), &d);
  JNU_UT_EQUAL(ls.Next(d), &e);
  ls_a.InsertTail(a);
  ls_a.InsertTail(b);
  ls.InsertHead(std::move(ls_a));
  JNU_UT_CHECK(!ls_a);
  JNU_UT_EQUAL(ls.Size(), 5);
  JNU_UT_EQUAL(ls.Head(), &a);
  JNU_UT_EQUAL(ls.Next(a), &b);
  JNU_UT_EQUAL(ls.Next(b), &c);
  ls_a.InsertHead(h);
  ls_a.InsertHead(g);
  ls.InsertTail(std::move(ls_a));
  JNU_UT_EQUAL(ls.Size(), 7);
  JNU_UT_EQUAL(ls.Tail(), &h);
  JNU_UT_EQUAL(ls.Next(e), &g);
  ls_a.InsertTail(f);
  ls.InsertNext(e, ls_a);
  JNU_UT_EQUAL(ls.Size(), 8);
  JNU_UT_EQUAL(ls.Next(e), &f);
}
void SListTest::TestDelete() {
  ls.DeleteHead();
  JNU_UT_EQUAL(ls.Size(), 7);
  JNU_UT_EQUAL(ls.Head(), &b);
  ls_a = ls.DeleteHeadTill(c);
  JNU_UT_EQUAL(ls.Size(), 5);
  JNU_UT_EQUAL(ls.Head(), &d);
  JNU_UT_EQUAL(ls_a.Size(), 2);
  JNU_UT_EQUAL(ls_a.Head(), &b);
  JNU_UT_EQUAL(ls_a.Tail(), &c);
  ls.DeleteNext(d);
  JNU_UT_EQUAL(ls.Size(), 4);
  JNU_UT_EQUAL(ls.Next(d), &f);
  ls_a = ls.DeleteNextTill(d, g);
  JNU_UT_EQUAL(ls.Size(), 2);
  JNU_UT_EQUAL(ls.Next(d), &h);
  JNU_UT_EQUAL(ls_a.Size(), 2);
  JNU_UT_EQUAL(ls_a.Head(), &f);
  JNU_UT_EQUAL(ls_a.Tail(), &g);
  ls_a = ls.DeleteNextAll(d);
  JNU_UT_EQUAL(ls.Size(), 1);
  JNU_UT_EQUAL(ls.Head(), &d);
  JNU_UT_EQUAL(ls.Tail(), &d);
  JNU_UT_EQUAL(ls_a.Size(), 1);
  JNU_UT_EQUAL(ls_a.Head(), &h);
  JNU_UT_EQUAL(ls_a.Tail(), &h);
}
void SListTest::Test() {
  TestInsert();
  TestDelete();
}
void ListTest::Test() {
  Run<SListTest>("single list");
}
