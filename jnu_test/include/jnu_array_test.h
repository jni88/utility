// By JNI
// Test cases of array

#ifndef JNU_ARRAY_TEST_H
#define JNU_ARRAY_TEST_H

#include "jnu_unit_test.h"
#include "jnu_array.h"
#include <utility>
#include <string>

namespace jnu_test {
// Template for test different types array
template<typename C>
class ArrayTestImp : public jnu::TestCase {
  // Main test entry
  void Test();
};
// Array tests
class ArrayTest : public jnu::TestCase {
  // Test object for array
  struct TestObj {
    TestObj() {
      // Dynamic allocate
      // To test if objects'
      // constructor and deconstructor are
      // properly called
      m_ptr = malloc(128);
    }
    TestObj(const char* str)
      : m_str (str) {
      // Dynamic allocate
      m_ptr = malloc(128);
    }
    TestObj& operator=(const TestObj& t) {
      m_str = t.m_str;
      return *this;
    }
    TestObj& operator=(TestObj&& t) {
      m_str = std::move(t.m_str);
      return *this;
    }
    ~TestObj() {
      // Free memory
      // If freed multiple times, it will crash
      free (m_ptr);
    }
    std::string m_str;
    void* m_ptr;
  };
  // Main test entry
  void Test();
};
}

#endif
