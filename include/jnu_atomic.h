// By JNI
// Wrap of gnu atomic buildins

#ifndef JNU_ATOMIC_H
#define JNU_ATOMIC_H

#include <stdint.h>

namespace jnu {
namespace atomic {
// Define of memory order type
typedef unsigned int MemoryOrder;
// Supported memory order types
const static MemoryOrder MO_RELAXED = __ATOMIC_RELAXED;
const static MemoryOrder MO_CONSUME = __ATOMIC_CONSUME;
const static MemoryOrder MO_ACQUIRE = __ATOMIC_ACQUIRE;
const static MemoryOrder MO_RELEASE = __ATOMIC_RELEASE;
const static MemoryOrder MO_ACQ_REL = __ATOMIC_ACQ_REL;
const static MemoryOrder MO_SEQ_CST = __ATOMIC_SEQ_CST;
// Base class for wrapping atomic buildins
// Template parameters:
// C: underline date type (1 - 8 bytes)
// MO_R, MO_W, MO_RW: Default memory orders for
//                    read, write, and read/write
template<typename C,
         MemoryOrder MO_R = MO_RELAXED,
         MemoryOrder MO_W = MO_RELAXED,
         MemoryOrder MO_RW = MO_RELAXED>
class Base {
public:
  typedef C Type;  // Define of underline data type
  // Default constructor
  Base() {
  }
  // Copy constructors
  Base(const C& val) {
    *this = val;
  }
  Base(const Base& t) {
    *this = t;
  }
  // Assign operators
  C operator=(const C& val) {
    Store(val);  // Atomic store
    return val;
  }
  C operator=(const Base& t) {
    return *this = (C) t;
  }
  // Type operator
  operator C() const {
    return Load();  // Atomic load
  }
  // Add assign operator
  C operator+=(const C& val) {
    return AddFetch(val);
  }
  // Sub assign operator
  C operator-=(const C& val) {
    return SubFetch(val);
  }
  // And assign operator
  C operator&=(const C& val) {
    return AndFetch(val);
  }
  // Or assign operator
  C operator|=(const C& val) {
    return OrFetch(val);
  }
  // Xor assign operator
  C operator^=(const C& val) {
    return XorFetch(val);
  }
  // Left hand increment operator
  C operator++() {
    return AddFetch(1);
  }
  // Left hand decrement operator
  C operator--() {
    return SubFetch(1);
  }
  // Right hand increment operator
  C operator++(int) {
    return FetchAdd(1);
  }
  // Right hand decrement operator
  C operator--(int) {
    return FetchSub(1);
  }
  // Atomic load
  C Load(const MemoryOrder& mo = MO_R) const {
    return __atomic_load_n(&m_val, mo);
  }
  // Atomic store
  void Store(const C& val, const MemoryOrder& mo = MO_W) {
    __atomic_store_n(&m_val, val, mo);
  }
  // Atomic exchange
  C Exchange(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_exchange_n(&m_val, val, mo);
  }
  // Atomic compare and exchange
  // Input: expected : the value for comparison
  //        desired: store the value if equal to expected
  //        mo_succ: for comparison success (read/write operation)
  //        mo_fail: for comparison fail (read only operation)
  bool CompareExchange(const C& expected, const C& desired,
                       const MemoryOrder& mo_succ = MO_RW,
                       const MemoryOrder& mo_fail = MO_R) {
    C t = expected;
    return __atomic_compare_exchange_n(&m_val, &t, desired,
                                       true, mo_succ, mo_fail);
  }
  // Atomic add fetch
  C AddFetch(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_add_fetch(&m_val, val, mo);
  }
  // Atomic sub fetch
  C SubFetch(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_sub_fetch(&m_val, val, mo);
  }
  // Atomic and fetch
  C AndFetch(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_and_fetch(&m_val, val, mo);
  }
  // Atomic or fetch
  C OrFetch(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_or_fetch(&m_val, val, mo);
  }
  // Atomic xor fetch
  C XorFetch(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_xor_fetch(&m_val, val, mo);
  }
  // Atomic nand fetch
  C NandFetch(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_nand_fetch(&m_val, val, mo);
  }
  // Atomic fetch add
  C FetchAdd(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_fetch_add(&m_val, val, mo);
  }
  // Atomic fetch sub
  C FetchSub(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_fetch_sub(&m_val, val, mo);
  }
  // Atomic fetch and
  C FetchAnd(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_fetch_and(&m_val, val, mo);
  }
  // Atomic fetch or
  C FetchOr(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_fetch_or(&m_val, val, mo);
  }
  // Atomic fetch xor
  C FetchXor(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_fetch_xor(&m_val, val, mo);
  }
  // Atomic fetch nand
  C FetchNand(const C& val, const MemoryOrder& mo = MO_RW) {
    return __atomic_fetch_nand(&m_val, val, mo);
  }
  // Atomic test and set (only for byte data types)
  bool TestAndSet(const MemoryOrder& mo = MO_RW) {
    static_assert(sizeof(C) == 1,
                  "function 'TestAndSet' only allows byte types");
    return __atomic_test_and_set(&m_val, mo);
  }
  // Atomic clear (only for byte data types)
  void Clear(const MemoryOrder& mo = MO_W) {
    static_assert(sizeof(C) == 1,
                  "function 'Clear' only allows byte types");
    return __atomic_clear(&m_val, mo);
  }
private:
  C m_val;  // Underline data
};
// Define of supported data types
// MO_R, MO_W, MO_RW: Default memory orders for
//                    read, write, and read/write
template<MemoryOrder MO_R = MO_RELAXED,
         MemoryOrder MO_W = MO_RELAXED,
         MemoryOrder MO_RW = MO_RELAXED>
class Type {
public:
  using Bool = Base<bool, MO_R, MO_W, MO_RW>;
  using Char =  Base<char, MO_R, MO_W, MO_RW>;
  using UChar = Base<unsigned char, MO_R, MO_W, MO_RW>;
  using Short = Base<short, MO_R, MO_W, MO_RW>;
  using UShort = Base<unsigned short, MO_R, MO_W, MO_RW>;
  using Int = Base<int, MO_R, MO_W, MO_RW>;
  using UInt = Base<unsigned int, MO_R, MO_W, MO_RW>;
  using Long = Base<long, MO_R, MO_W, MO_RW>;
  using ULong = Base<unsigned long, MO_R, MO_W, MO_RW>;
  using LongLong = Base<long long, MO_R, MO_W, MO_RW>;
  using ULongLong = Base<unsigned long long, MO_R, MO_W, MO_RW>;
  using Size_T = Base<size_t, MO_R, MO_W, MO_RW>;
  using Int8 = Base<int8_t, MO_R, MO_W, MO_RW>;
  using UInt8 = Base<uint8_t, MO_R, MO_W, MO_RW>;
  using Int16 = Base<int16_t, MO_R, MO_W, MO_RW>;
  using UInt16 = Base<uint16_t, MO_R, MO_W, MO_RW>;
  using Int32 = Base<int32_t, MO_R, MO_W, MO_RW>;
  using UInt32 = Base<uint32_t, MO_R, MO_W, MO_RW>;
  using Int64 = Base<int64_t, MO_R, MO_W, MO_RW>;
  using UInt64 = Base<uint64_t, MO_R, MO_W, MO_RW>;
private:
  // Private constructor, make sure it is static only
  Type() {}
};
// Apply thread fence
static void ThreadFence(const MemoryOrder& mo) {
  __atomic_thread_fence(mo);
}
// Apply signal fence
static void SignalFence(const MemoryOrder& mo) {
  __atomic_signal_fence(mo);
}
}
}

#endif
