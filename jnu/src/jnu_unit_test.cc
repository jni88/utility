// By JNI
// Implementation of unit test module

#include <stdlib.h>
#include "jnu_unit_test.h"

using namespace jnu;

// Unit test report default constructor
inline TestCase::Report::Report()
  : m_total (0),
    m_passed (0) {
}
// Report deconstructor
inline TestCase::Report::~Report() {
}
// Report Copy constructor
inline TestCase::Report::Report(const Report& r) {
  *this = r;
}
// Report Assign operator
inline TestCase::Report&
TestCase::Report::operator=(const Report& r) {
  m_total = r.m_total;  // Copy total checks
  m_passed = r.m_passed;  // Copy passed checks
  return *this;
}
// Report aggregate assign operator
inline TestCase::Report&
TestCase::Report::operator+=(const Report& r) {
  m_total += r.m_total;  // Aggregate total checks
  m_passed += r.m_passed;  // Aggregate passed checks
  return *this;
}
// Report aggregate assign operator
// passed: true - passed single check
//         false - failed single check
// Return: the value of passed
inline bool TestCase::Report::operator+=(bool passed) {
  ++m_total;  // Increase total checks
  if (passed) {  // If passed, increase passed checks
    ++m_passed;
  }
  return passed;  // Return passed value
}
// Report aggregate operator
inline TestCase::Report
TestCase::Report::operator+(const Report& r) const {
  Report t = *this;  // Temp report = current
  t += r;  // Temp aggregates with input
  return t;  // Return temp report
}
// Report bool operator
// Return: true - all checks passed
//         false - some checks failed
inline TestCase::Report::operator bool() const {
  return m_total == m_passed;
}
// Test module default constructor
TestCase::TestCase()
  : m_indent (0) {
}
// Test module deconstructor
TestCase::~TestCase() {
}
// Start current test module
// name: module description string
void TestCase::Start(const char* name) {
  Run(m_indent, name);  // Run current module
}
// Run a test module
// name: description of the target test module
// tc: target test module
void TestCase::Run(const char* name, TestCase& tc) {
  // Run target test module (with increased indent)
  const Report& r = tc.Run(m_indent + INDENT, name);
  m_report += r;  // Aggregate test report
}
// Exit unit test
// passed: true - exit zero
//         false - exit nonzero
void TestCase::Exit(bool passed) {
  exit(passed ? 0 : 1);
}
// Exit current test module
// All checks passed - exit zero
// some checks failed - exit nonzero
void TestCase::Exit() {
  Exit(m_report);
}
// Exit with success
void TestCase::Succ() {
  Exit(true);
}
// Exit with fail
void TestCase::Fail() {
  Exit(false);
}
// Add one check to report
// passed: true - passed check
//         false - failed check
bool TestCase::Check(bool passed) {
  return m_report += passed;
}
// Virtual implementation of test module
void TestCase::Test() {
}
// Initialize test module
// indent: the indent of current test module
inline void TestCase::Reset(int indent) {
  m_indent = indent;  // Assign indent
  m_report = Report();  // Initialize test report
}
// Run current test module
// indent: indent of current test module
inline const TestCase::Report&
TestCase::Run(int indent, const char* name) {
  Reset(indent);  // Initialize test module
  // Message at beginning of test module
  JNU_UT_MSG_A(m_indent, "Entering test: '%s'", name);
  Test();  // Run test module implementation
  // Message at end of test module (print report)
  JNU_UT_MSG_A(m_indent, "Leaving test: '%s' (%u/%u)",
               name,
               m_report.GetPassed(),
               m_report.GetTotal());
  return m_report;  // Return report of current test
}
