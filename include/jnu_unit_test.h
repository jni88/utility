// By JNI
// Unit test modules

#ifndef JNU_UNIT_TEST_H
#define JNU_UNIT_TEST_H

#include <stdio.h>
#include "jnu_defines.h"

// Print general message
// D: indent (number of char)
// F: print format (same as printf)
// ...: print parameters (should match with format)
#define JNU_UT_MSG_A(D, F, ...) printf("%*s" F "\n", \
                                       D, "", ##__VA_ARGS__)

// Print general message (without parameters)
#define JNU_UT_MSG(D, F) JNU_UT_MSG_A(D, "%s", F)

// Unit test standard output
// D: indent
// L: type (info, warn, error)
// F: print format
// ...: print parameters
#define JNU_UT_OUTPUT_A(D, L, F, ...) JNU_UT_MSG_A(\
                                      D, "%s:%d: " #L ": " \
                                      "in '%s': " F, \
                                      __FILE__, \
                                      __LINE__, \
                                      __PRETTY_FUNCTION__, \
                                      ##__VA_ARGS__)

// Unit test standard output without parameters
#define JNU_UT_OUTPUT(D, L, F) JNU_UT_OUTPUT_A(D, L, "%s", F)

// Unit test info output (with or without parameters)
#define JNU_UT_INFO_A(D, F, ...) JNU_UT_OUTPUT_A(D, info, F, ##__VA_ARGS__)
#define JNU_UT_INFO(D, F) JNU_UT_OUTPUT(D, info, F)

// Unit test warn output (with or without parameters)
#define JNU_UT_WARN_A(D, F, ...) JNU_UT_OUTPUT_A(D, warn, F, ##__VA_ARGS__)
#define JNU_UT_WARN(D, F) JNU_UT_OUTPUT(D, warn, F)

// Unit test error output (with or without parameters)
#define JNU_UT_ERROR_A(D, F, ...) JNU_UT_OUTPUT_A(D, error, F, ##__VA_ARGS__)
#define JNU_UT_ERROR(D, F) JNU_UT_OUTPUT(D, error, F)

// Unit test general check statement:
// E: check if statment E is true
// M: error message if check fails
#define JNU_UT_CHECK_A(E, M) if (!jnu::TestCase::Check(E)) {\
                               JNU_UT_ERROR(jnu::TestCase::Indent(), M); \
                             }

// Unit test check statement
// E: check if E is true
#define JNU_UT_CHECK(E) JNU_UT_CHECK_A(\
                        E, #E " is not true");

// Unit test check equal statement
// A, B: check if A == B
#define JNU_UT_EQUAL(A, B) JNU_UT_CHECK_A(\
                           (A) == (B), \
                           #A " is not equal to " #B)

// Unit test check close (for float values) statement
// A, B, P: check if A and B is close regarding precision P
#define JNU_UT_CLOSE(A, B, P) JNU_UT_CHECK_A(\
                              (A) - (B) < (P) && (B) - (A) < (P), \
                              #A " is not close to " #B)

// Define of unit test modules
namespace jnu {
// Unit test modules
class TestCase {
  const static int INDENT = 2;  // Output indent
  // Unit test report
  class Report {
  public:
    // Default constructor
    Report();
    // Deconstructor
    ~Report();
    // Copy constructor
    Report(const Report& r);
    // Assign operator
    Report& operator=(const Report& r);
    // Aggregate assign operator
    Report& operator+=(const Report& r);
    // Aggregate assign operator (single check)
    bool operator+=(bool passed);
    // Aggregate operator
    Report operator+(const Report& r) const;
    // Bool operator
    operator bool() const;
    // Number of total checks
    unsigned int GetTotal() const {
      return m_total;
    }
    // Number of passed checks
    unsigned int GetPassed() const {
      return m_passed;
    }
  private:
    unsigned int m_total;  // Number of total checks
    unsigned int m_passed;  // Number of passed checks
  };
public:
  // Test module default constructor
  TestCase();
  // Virtual deconstructor
  virtual ~TestCase();
  // Start the current test module
  void Start(const char* name);
  // Run another test module
  void Run(const char* name, TestCase& tc);
  // Run another test module (Template version)
  template<typename C, typename... A>
  void Run(const char* name, A... a) {
    C c(a...);  // Construct test module
    Run(name, c);  // Run it
  }
  // Exit unit test
  static void Exit(bool passed);
  void Exit();
  // Exit with success (return zero)
  static void Succ();
  // Exit with fail (return nozero)
  static void Fail();
protected:
  // Add one check to report
  bool Check(bool passed);
  // Implementation of test module
  virtual void Test();
  // Output indent
  int Indent() const {
    return m_indent + INDENT;
  }
private:
  // Initialize test module
  void Reset(int indent);
  // Run current test module
  const Report& Run(int indent, const char* name);
  mutable int m_indent;  // Current test output indent
  mutable Report m_report;  // Current test report
};
}

#endif
