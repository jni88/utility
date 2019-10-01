#include "jnu_list_test.h"
#include <utility>

using namespace jnu_test;

void SListTest::TestInsert() {
  List s = ls;
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
  JNU_UT_EQUAL(ls.Next(*ls.Tail()), NULL);
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
  JNU_UT_EQUAL(ls_a.Next(*ls_a.Tail()), NULL);
  ls.DeleteNext(d);
  JNU_UT_EQUAL(ls.Size(), 4);
  JNU_UT_EQUAL(ls.Next(d), &f);
  ls_a = ls.DeleteNextTill(d, g);
  JNU_UT_EQUAL(ls.Size(), 2);
  JNU_UT_EQUAL(ls.Next(d), &h);
  JNU_UT_EQUAL(ls_a.Size(), 2);
  JNU_UT_EQUAL(ls_a.Head(), &f);
  JNU_UT_EQUAL(ls_a.Tail(), &g);
  JNU_UT_EQUAL(ls_a.Next(*ls_a.Tail()), NULL);
  ls_a = ls.DeleteNextAll(d);
  JNU_UT_EQUAL(ls.Size(), 1);
  JNU_UT_EQUAL(ls.Head(), &d);
  JNU_UT_EQUAL(ls.Tail(), &d);
  JNU_UT_EQUAL(ls_a.Size(), 1);
  JNU_UT_EQUAL(ls_a.Head(), &h);
  JNU_UT_EQUAL(ls_a.Tail(), &h);
  JNU_UT_EQUAL(ls_a.Next(*ls_a.Tail()), NULL);
  JNU_UT_EQUAL(ls.Next(*ls.Tail()), NULL);
}
void SListTest::Test() {
  TestInsert();
  TestDelete();
}
void DListTest::TestInsert() {
  List s = ls;
  ls.InsertHead(d);
  ls.InsertPrev(d, c);
  ls.InsertTail(e);
  JNU_UT_EQUAL(ls.Size(), 3);
  JNU_UT_EQUAL(ls.Head(), &c);
  JNU_UT_EQUAL(ls.Tail(), &e);
  JNU_UT_EQUAL(ls.Prev(e), &d);
  JNU_UT_EQUAL(ls.Prev(d), &c);
  ls_a.InsertTail(a);
  ls_a.InsertTail(b);
  ls.InsertHead(std::move(ls_a));
  JNU_UT_CHECK(!ls_a);
  JNU_UT_EQUAL(ls.Size(), 5);
  JNU_UT_EQUAL(ls.Head(), &a);
  JNU_UT_EQUAL(ls.Prev(c), &b);
  JNU_UT_EQUAL(ls.Prev(b), &a);
  ls_a.InsertHead(h);
  ls_a.InsertHead(g);
  ls.InsertTail(std::move(ls_a));
  JNU_UT_EQUAL(ls.Size(), 7);
  JNU_UT_EQUAL(ls.Tail(), &h);
  JNU_UT_EQUAL(ls.Prev(g), &e);
  JNU_UT_EQUAL(ls.Prev(*ls.Head()), NULL);
  ls_a.InsertTail(f);
  ls.InsertPrev(g, ls_a);
  JNU_UT_EQUAL(ls.Size(), 8);
  JNU_UT_EQUAL(ls.Prev(f), &e);
}
void DListTest::TestDelete() {
  ls.DeleteHead();
  JNU_UT_EQUAL(ls.Size(), 7);
  JNU_UT_EQUAL(ls.Head(), &b);
  ls_a = ls.DeletePrevAll(d);
  JNU_UT_EQUAL(ls.Size(), 5);
  JNU_UT_EQUAL(ls.Head(), &d);
  JNU_UT_EQUAL(ls_a.Size(), 2);
  JNU_UT_EQUAL(ls_a.Head(), &b);
  JNU_UT_EQUAL(ls_a.Tail(), &c);
  JNU_UT_EQUAL(ls_a.Prev(*ls_a.Head()), NULL);
  ls.DeletePrev(f);
  JNU_UT_EQUAL(ls.Size(), 4);
  JNU_UT_EQUAL(ls.Next(d), &f);
  ls_a = ls.DeletePrevTill(h, f);
  JNU_UT_EQUAL(ls.Size(), 2);
  JNU_UT_EQUAL(ls.Next(d), &h);
  JNU_UT_EQUAL(ls_a.Size(), 2);
  JNU_UT_EQUAL(ls_a.Head(), &f);
  JNU_UT_EQUAL(ls_a.Tail(), &g);
  JNU_UT_EQUAL(ls_a.Prev(*ls_a.Head()), NULL);
  ls_a = ls.DeleteTailTill(h);
  JNU_UT_EQUAL(ls.Size(), 1);
  JNU_UT_EQUAL(ls.Head(), &d);
  JNU_UT_EQUAL(ls.Tail(), &d);
  JNU_UT_EQUAL(ls_a.Size(), 1);
  JNU_UT_EQUAL(ls_a.Head(), &h);
  JNU_UT_EQUAL(ls_a.Tail(), &h);
  JNU_UT_EQUAL(ls_a.Prev(*ls_a.Head()), NULL);
  JNU_UT_EQUAL(ls.Prev(*ls.Head()), NULL);
  ls.Delete(d);
  JNU_UT_EQUAL(ls.Size(), 0);
  JNU_UT_EQUAL(ls.Head(), NULL);
  JNU_UT_EQUAL(ls.Tail(), NULL);
}
void DListTest::Test() {
  TestInsert();
  TestDelete();
}
void ListTest::Test() {
  Run<SListTest>("single link list");
  Run<DListTest>("double link list");
}
