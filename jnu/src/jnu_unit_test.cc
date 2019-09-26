#include <stdlib.h>
#include "jnu_unit_test.h"

using namespace jnu;

inline TestCase::Report::Report()
  : m_total (0),
    m_passed (0) {
}
inline TestCase::Report::~Report() {
}
inline TestCase::Report::Report(const Report& r) {
  *this = r;
}
inline TestCase::Report&
TestCase::Report::operator=(const Report& r) {
  m_total = r.m_total;
  m_passed = r.m_passed;
  return *this;
}
inline TestCase::Report&
TestCase::Report::operator+=(const Report& r) {
  m_total += r.m_total;
  m_passed += r.m_passed;
  return *this;
}
inline bool TestCase::Report::operator+=(bool passed) {
  ++m_total;
  if (passed) {
    ++m_passed;
  }
  return passed;
}
inline TestCase::Report
TestCase::Report::operator+(const Report& r) const {
  Report t = *this;
  t += r;
  return t;
}
inline TestCase::Report::operator bool() const {
  return m_total == m_passed;
}
TestCase::TestCase()
  : m_indent (0) {
}
TestCase::~TestCase() {
}
void TestCase::Start(const char* name) {
  Run(m_indent, name);
}
void TestCase::Run(const char* name, TestCase& tc) {
  const Report& r = tc.Run(m_indent + INDENT, name);
  m_report += r;
}
void TestCase::Exit(bool passed) {
  exit(passed ? 0 : 1);
}
void TestCase::Exit() {
  Exit(m_report);
}
void TestCase::Succ() {
  Exit(true);
}
void TestCase::Fail() {
  Exit(false);
}
bool TestCase::Check(bool passed) {
  return m_report += passed;
}
inline void TestCase::Reset(int indent) {
  m_indent = indent;
  m_report = Report();
}
inline const TestCase::Report&
TestCase::Run(int indent, const char* name) {
  Reset(indent);
  JNU_UT_MSG_A(m_indent, "Entering test: '%s'", name);
  Test();
  JNU_UT_MSG_A(m_indent, "Leaving test: '%s' (%u/%u)",
               name,
               m_report.GetPassed(),
               m_report.GetTotal());
  return m_report;
}
