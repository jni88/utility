// By JNI
// Test of array set

#ifndef JNU_ARRAY_SET_TEST_H
#define JNU_ARRAY_SET_TEST_H

#include "jnu_unit_test.h"
#include "jnu_array_set.h"
#include <utility>
#include <string>

// Test cases for array set
namespace jnu_test {
class ArraySetTest : public jnu::TestCase {
  // Test object for array set
  struct TestObj {
    TestObj() {
    }
    TestObj(const char* str)
      : m_str (str) {
    }
    TestObj& operator=(const TestObj& t) {
      m_str = t.m_str;
      return *this;
    }
    TestObj& operator=(TestObj&& t) {
      m_str = std::move(t.m_str);
      return *this;
    }
    std::string m_str;
  };
  // Main test entry
  void Test();
};
}
#endif
