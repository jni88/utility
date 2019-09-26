#ifndef JNU_UNIT_TEST_H
#define JNU_UNIT_TEST_H

#include <stdio.h>
#include "jnu_defines.h"

#define JNU_UT_MSG_A(D, F, ...) printf("%*s" F "\n", \
                                       D, "", ##__VA_ARGS__)
#define JNU_UT_MSG(D, F) JNU_UT_MSG_A(D, "%s", F)

#define JNU_UT_OUTPUT_A(D, L, F, ...) JNU_UT_MSG_A(\
                                      D, "%s:%d: " #L ": " F, \
                                      __FILE__, \
                                      __LINE__, \
                                      ##__VA_ARGS__)

#define JNU_UT_OUTPUT(D, L, F) JNU_UT_OUTPUT_A(D, L, "%s", F)

#define JNU_UT_INFO_A(D, F, ...) JNU_UT_OUTPUT_A(D, info, F, ##__VA_ARGS__)
#define JNU_UT_INFO(D, F) JNU_UT_OUTPUT(D, info, F)

#define JNU_UT_WARN_A(D, F, ...) JNU_UT_OUTPUT_A(D, warn, F, ##__VA_ARGS__)
#define JNU_UT_WARN(D, F) JNU_UT_OUTPUT(D, warn, F)

#define JNU_UT_ERROR_A(D, F, ...) JNU_UT_OUTPUT_A(D, error, F, ##__VA_ARGS__)
#define JNU_UT_ERROR(D, F) JNU_UT_OUTPUT(D, error, F)

#define JNU_UT_CHECK_A(E, M) if (!jnu::TestCase::Check(E)) {\
                               JNU_UT_ERROR(jnu::TestCase::Indent(), M); \
                             }

#define JNU_UT_CHECK(E) JNU_UT_CHECK_A(\
                        E, "Check failed: " #E " is not true");

#define JNU_UT_EQUAL(A, B) JNU_UT_CHECK_A(\
                           (A) == (B), \
                           "Check equal failed: " \
                           #A " is not equal to " #B)

#define JNU_UT_CLOSE(A, B, P) JNU_UT_CHECK_A(\
                              (A) - (B) < (P) && (B) - (A) < (P), \
                              "Check close failed: " \
                              #A " is not close to " #B)

namespace jnu {
class TestCase {
  const static int INDENT = 2;
  class Report {
  public:
    Report();
    ~Report();
    Report(const Report& r);
    Report& operator=(const Report& r);
    Report& operator+=(const Report& r);
    bool operator+=(bool passed);
    Report operator+(const Report& r) const;
    operator bool() const;
    unsigned int GetTotal() const { return m_total; }
    unsigned int GetPassed() const { return m_passed; }
  private:
    unsigned int m_total;
    unsigned int m_passed;
  };
public:
  TestCase();
  virtual ~TestCase();
  void Start(const char* name);
  void Run(const char* name, TestCase& tc);
  template<typename C, typename... A>
  void Run(const char* name, A... a) {
    C c(a...);
    Run(name, c);
  }
  static void Exit(bool passed);
  void Exit();
  static void Succ();
  static void Fail();
protected:
  bool Check(bool passed);
  virtual void Test() {};
  int Indent() const { return m_indent + INDENT; }
private:
  void Reset(int indent);
  const Report& Run(int indent, const char* name);
  mutable int m_indent;
  mutable Report m_report;
};
}

#endif
