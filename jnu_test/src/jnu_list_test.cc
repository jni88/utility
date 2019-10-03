// By JNI
// Implementation of link list unit tests

#include "jnu_list_test.h"
#include <utility>

using namespace jnu_test;

// Test of single list insertion
void SListTest::TestInsert() {
  // Insert objects c, d, e
  ls.InsertHead(c);
  ls.InsertNext(c, d);
  ls.InsertTail(e);
  // After insertion, list 'ls' is:
  // c->d->e
  JNU_UT_EQUAL(ls.Size(), 3);
  JNU_UT_EQUAL(ls.Head(), &c);
  JNU_UT_EQUAL(ls.Tail(), &e);
  JNU_UT_EQUAL(ls.Next(c), &d);
  JNU_UT_EQUAL(ls.Next(d), &e);
  // Insert objects a and b to temp list
  // After insert list 'ls_a' is: a->b
  ls_a.InsertTail(a);
  ls_a.InsertTail(b);
  // Move 'ls_a' to 'ls' head
  ls.InsertHead(std::move(ls_a));
  // After insertion, 'ls' becomes:
  // a->b->c->d->e
  // And 'ls_a' become empty
  JNU_UT_CHECK(!ls_a);
  JNU_UT_EQUAL(ls.Size(), 5);
  JNU_UT_EQUAL(ls.Head(), &a);
  JNU_UT_EQUAL(ls.Next(a), &b);
  JNU_UT_EQUAL(ls.Next(b), &c);
  // Insert h and g to 'ls_a': g->h
  ls_a.InsertHead(h);
  ls_a.InsertHead(g);
  // Move 'ls_a' to 'ls' tail,
  // ls = a->b->c->d->e->g->h
  ls.InsertTail(std::move(ls_a));
  JNU_UT_EQUAL(ls.Size(), 7);
  JNU_UT_EQUAL(ls.Tail(), &h);
  JNU_UT_EQUAL(ls.Next(e), &g);
  JNU_UT_EQUAL(ls.Next(*ls.Tail()), NULL);
  // Insert f to 'ls_a'
  ls_a.InsertTail(f);
  // Move 'ls_a' to next of e
  // ls = a->b->c->d->e->f->g->h
  ls.InsertNext(e, ls_a);
  JNU_UT_EQUAL(ls.Size(), 8);
  JNU_UT_EQUAL(ls.Next(e), &f);
}
// Test of single list deletion
void SListTest::TestDelete() {
  // Delete 'ls' head:
  // ls = b->c->d->e->f->g->h
  ls.DeleteHead();
  JNU_UT_EQUAL(ls.Size(), 7);
  JNU_UT_EQUAL(ls.Head(), &b);
  // Delete from 'ls' head till c
  // ls = d->e->f->g->h
  // ls_a = b->c
  ls_a = ls.DeleteHeadTill(c);
  JNU_UT_EQUAL(ls.Size(), 5);
  JNU_UT_EQUAL(ls.Head(), &d);
  JNU_UT_EQUAL(ls_a.Size(), 2);
  JNU_UT_EQUAL(ls_a.Head(), &b);
  JNU_UT_EQUAL(ls_a.Tail(), &c);
  JNU_UT_EQUAL(ls_a.Next(*ls_a.Tail()), NULL);
  // Delete d's next (e):
  // ls = d->f->g->h
  ls.DeleteNext(d);
  JNU_UT_EQUAL(ls.Size(), 4);
  JNU_UT_EQUAL(ls.Next(d), &f);
  // Delete from d's next till g:
  // ls = d->h
  // ls_a = f->g
  ls_a = ls.DeleteNextTill(d, g);
  JNU_UT_EQUAL(ls.Size(), 2);
  JNU_UT_EQUAL(ls.Next(d), &h);
  JNU_UT_EQUAL(ls_a.Size(), 2);
  JNU_UT_EQUAL(ls_a.Head(), &f);
  JNU_UT_EQUAL(ls_a.Tail(), &g);
  JNU_UT_EQUAL(ls_a.Next(*ls_a.Tail()), NULL);
  // Delete all from d's next
  // ls = d
  // ls_a = f
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
// Test of single link list
void SListTest::Test() {
  TestInsert();  // Insertion test
  TestDelete();  // Deletion test
}
// Test of double link list insertion
void DListTest::TestInsert() {
  // Insert c, d, e:
  // ls = c->d->e
  ls.InsertHead(d);
  ls.InsertPrev(d, c);
  ls.InsertTail(e);
  JNU_UT_EQUAL(ls.Size(), 3);
  JNU_UT_EQUAL(ls.Head(), &c);
  JNU_UT_EQUAL(ls.Tail(), &e);
  JNU_UT_EQUAL(ls.Prev(e), &d);
  JNU_UT_EQUAL(ls.Prev(d), &c);
  // Insert into 'ls_a': ls_a = a->b
  ls_a.InsertTail(a);
  ls_a.InsertTail(b);
  // Move 'ls_a' to head of 'ls':
  // ls = a->b->c->d->e
  ls.InsertHead(std::move(ls_a));
  JNU_UT_CHECK(!ls_a);
  JNU_UT_EQUAL(ls.Size(), 5);
  JNU_UT_EQUAL(ls.Head(), &a);
  JNU_UT_EQUAL(ls.Prev(c), &b);
  JNU_UT_EQUAL(ls.Prev(b), &a);
  // Insert into 'ls_a': ls_a = g->h
  ls_a.InsertHead(h);
  ls_a.InsertHead(g);
  // Move ls_a to tail of 'ls':
  // ls = a->b->c->d->e->g->h
  ls.InsertTail(std::move(ls_a));
  JNU_UT_EQUAL(ls.Size(), 7);
  JNU_UT_EQUAL(ls.Tail(), &h);
  JNU_UT_EQUAL(ls.Prev(g), &e);
  JNU_UT_EQUAL(ls.Prev(*ls.Head()), NULL);
  // Insert into 'ls_a': ls_a = f
  ls_a.InsertTail(f);
  // Move 'ls_a' to previous of g:
  // ls = a->b->c->d->e->f->g->h
  ls.InsertPrev(g, ls_a);
  JNU_UT_EQUAL(ls.Size(), 8);
  JNU_UT_EQUAL(ls.Prev(f), &e);
}
// Test of double link list deletion
void DListTest::TestDelete() {
  // Delete 'ls' head:
  // ls = b->c->d->e->f->g->h
  ls.DeleteHead();
  JNU_UT_EQUAL(ls.Size(), 7);
  JNU_UT_EQUAL(ls.Head(), &b);
  // Delete [head, d):
  // ls = d->e->f->g->h
  // ls_a = b->c
  ls_a = ls.DeletePrevAll(d);
  JNU_UT_EQUAL(ls.Size(), 5);
  JNU_UT_EQUAL(ls.Head(), &d);
  JNU_UT_EQUAL(ls_a.Size(), 2);
  JNU_UT_EQUAL(ls_a.Head(), &b);
  JNU_UT_EQUAL(ls_a.Tail(), &c);
  JNU_UT_EQUAL(ls_a.Prev(*ls_a.Head()), NULL);
  // Delete f's previous (e):
  // ls = d->f->g->h
  ls.DeletePrev(f);
  JNU_UT_EQUAL(ls.Size(), 4);
  JNU_UT_EQUAL(ls.Prev(f), &d);
  // Delete [f, h):
  // ls = d->h
  // ls_a = f->g
  ls_a = ls.DeletePrevTill(h, f);
  JNU_UT_EQUAL(ls.Size(), 2);
  JNU_UT_EQUAL(ls.Prev(h), &d);
  JNU_UT_EQUAL(ls_a.Size(), 2);
  JNU_UT_EQUAL(ls_a.Head(), &f);
  JNU_UT_EQUAL(ls_a.Tail(), &g);
  JNU_UT_EQUAL(ls_a.Prev(*ls_a.Head()), NULL);
  // Delete [h, tail]:
  // ls = d
  // ls_a = h
  ls_a = ls.DeleteTailTill(h);
  JNU_UT_EQUAL(ls.Size(), 1);
  JNU_UT_EQUAL(ls.Head(), &d);
  JNU_UT_EQUAL(ls.Tail(), &d);
  JNU_UT_EQUAL(ls_a.Size(), 1);
  JNU_UT_EQUAL(ls_a.Head(), &h);
  JNU_UT_EQUAL(ls_a.Tail(), &h);
  JNU_UT_EQUAL(ls_a.Prev(*ls_a.Head()), NULL);
  JNU_UT_EQUAL(ls.Prev(*ls.Head()), NULL);
  Item x, y;  // Template objects
  // Insert x and y:
  // ls = x->d->y
  ls.InsertPrev(d, x);
  ls.InsertNext(d, y);
  // Delete d:
  // ls = x->y
  ls.Delete(d);
  JNU_UT_EQUAL(ls.Size(), 2);
  JNU_UT_EQUAL(ls.Head(), &x);
  JNU_UT_EQUAL(ls.Tail(), &y);
  JNU_UT_EQUAL(ls.Next(x), &y);
  JNU_UT_EQUAL(ls.Prev(y), &x);
  // Delete [x, y]:
  // ls is empty now
  ls.DeleteTill(x, y);
  JNU_UT_EQUAL(ls.Size(), 0);
  JNU_UT_EQUAL(ls.Head(), NULL);
  JNU_UT_EQUAL(ls.Tail(), NULL);
}
// Double link list test
void DListTest::Test() {
  TestInsert();  // Insertion test
  TestDelete();  // Deletion test
}
// Link list test, general entry
void ListTest::Test() {
  Run<SListTest>("single link list");  // Single link list tests
  Run<DListTest>("double link list");  // Double link list tests
}
