// By JNI
// Implement of static, dynamic and hybrid array
// static array: allocate on stack
// dynamic array: allocate on heap
// hybrid array: for small size, allocate directly on stack
//               when size grow bigger, allocate on heap

#ifndef JNU_ARRAY_H
#define JNU_ARRAY_H

#include <utility>
#include "jnu_defines.h"
#include "jnu_memory.h"
#include "jnu_iterator.h"

namespace jnu {
// The following defines different
// memory/object models.
// Each one defines:
// Initialize - initialize memory/object
// destroy - destroy memory/object
// copy/move - copy or move memory/object
// Users are responsible to choose the right one
// for the array to achieve best performance while
// remain correct.

// Model for memory block (e.g. structs without
// constructor or deconstructor)
class ARR_MEM_ALLOC {
public:
  // Initialize memory, do nothing
  template<typename T>
  static void Initialize(T* ptr, size_t sz) {
  }
  // Destroy memory, do nothing
  template<typename T>
  static void Destroy(T* ptr, size_t sz) {
  }
  // Memory copy
  // T - data type
  // Input: dst - destination address
  //        src - source address
  //        sz - the size of array to copy
  template<typename T>
  static void Copy(T* dst, const T* src, size_t sz) {
    memmove(dst, src, sz * sizeof(T));  // Memory move
  }
  // Memory move (use copy)
  template<typename T>
  static void Move(T* dst, T* src, size_t sz) {
    Copy<T>(dst, src, sz);
  }
private:
  ARR_MEM_ALLOC() {}  // Make sure static class
};
// Model for objects
// Need to take care of the constructor and deconstructor
class ARR_OBJ_ALLOC {
public:
  // Object initialize
  // T - object type
  // Input: ptr - start address
  //        sz - array length
  template<typename T>
  static void Initialize(T* ptr, size_t sz) {
    ::new (ptr) T[sz];  // Call object's default constructor
  }
  // Object destroy
  template<typename T>
  static void Destroy(T* ptr, size_t sz) {
    // For each object, call its deconstructor
    for(size_t i = 0; i < sz; ++i) {
      ptr[i].~T();
    }
  }
  // Copy objects
  // Input: dst - destination address
  //        src - source address
  //        sz - array length
  template<typename T>
  static void Copy(T* dst, const T* src, size_t sz) {
    if (src > dst) {
      // For the case source address bigger than destination
      // Copy from source start to source end
      for (size_t i = 0; i < sz; ++i) {
        dst[i] = src[i];  // Use object's assign operator
      }
    } else if (src < dst) {
      // For the case source address smaller than destination
      // Copy reverse order from source end to source start
      for (size_t i = sz; i > 0; --i) {
        dst[i - 1] = src[i - 1];  // Use object's assign operator
      }
    }
  }
  // Move objects
  // Input: dst - destination address
  //        src - source address
  //        sz - array length
  template<typename T>
  static void Move(T* dst, T* src, size_t sz) {
    if (src > dst) {
      // For the case source address bigger than destination
      // Move from source start to source end
      for (size_t i = 0; i < sz; ++i) {
        dst[i] = std::move(src[i]);  // Using object's move operator
      }
    } else if (src < dst) {
      // For the case source address smaller than destination
      // Move reverse order from source end to source start
      for (size_t i = sz; i > 0; --i) {
        dst[i - 1] = std::move(src[i - 1]);  // Move operator
      }
    }
  }
private:
  ARR_OBJ_ALLOC() {}  // Make sure static class
};
// Model for objects safe for memory copy or move
class ARR_OBJ_MV_ALLOC {
public:
  // Object initialize, need do it properly
  // by calling objects' constructor
  template<typename T>
  static void Initialize(T* ptr, size_t sz) {
    ARR_OBJ_ALLOC::Initialize(ptr, sz);
  }
  // Object destroy, need do it properly
  // by calling objects' deconstructor
  template<typename T>
  static void Destroy(T* ptr, size_t sz) {
    ARR_OBJ_ALLOC::Destroy(ptr, sz);
  }
  // Copy object, safe to do memory copy
  // for fast execution
  template<typename T>
  static void Copy(T* dst, const T* src, size_t sz) {
    ARR_MEM_ALLOC::Copy(dst, src, sz);
  }
  // Move object, safe to do memory move
  // for fast execution
  template<typename T>
  static void Move(T* dst, T* src, size_t sz) {
    ARR_MEM_ALLOC::Move(dst, src, sz);
  }
private:
  ARR_OBJ_MV_ALLOC() {}  // Make sure static class
};
// Interface of array implementation
// C - the type of array define: static, dynamic or hybrid
template<typename C>
class ArrayImp : private C {
  typedef typename C::Type T;  // Underline element type
  typedef typename C::Alloc A;  // Memory/object model
public:
  typedef T Type;  // Underline element type
  typedef Iterator<T*, (T*)NULL> Iter;  // Define of iterator
  typedef Iterator<const T*, (T*)NULL> IterC;  // Const iterator
  // Constructor
  // Input: mm - memory manager (for malloc and free)
  //        rsv_sz - size of reserved memory
  ArrayImp(size_t rsv_sz = 0,
           memory::MMBase* mm = &memory::MM_BUILDIN)
    : C (mm),  // Set memory manager
      m_sz (0) {  // Initialize array size
    Reserve(rsv_sz);  // Reserve memory
  }
  // Deconstructor
  ~ArrayImp() {
    Free();  // Free all elements and recycle memory
  }
  // Constructor with raw array input
  // Input: arr - raw array
  //        sz - raw array length
  ArrayImp(const T* arr, size_t sz,
           memory::MMBase* mm = &memory::MM_BUILDIN)
    : C (mm),  // Set memory manager
      m_sz (0) {  // Initialize array size
    Copy(arr, sz);  // Copy from input raw array
  }
  // Copy constructor
  ArrayImp(const ArrayImp& arr)
    : C (arr.GetMM()),  // Set memory manager
      m_sz (0) {  // Initialize array size
    *this = arr;  // Assign from input array
  }
  // Move constructor
  ArrayImp(ArrayImp&& arr)
    : C(arr.GetMM()),  // Set memory manager
      m_sz (0) {  // Initialize array size
    *this = std::move(arr);  // Move from input array
  }
  // Template for assign operator
  // Allows different types input
  // H - input array type
  template<typename H>
  ArrayImp& operator=(const H& arr) {
    Copy(arr.Data(), arr.Size());  // Copy from input
    return *this;
  }
  // Assign operator
  ArrayImp& operator=(const ArrayImp& arr) {
    // Using the assign template defined above
    return operator=<ArrayImp>((const ArrayImp&)arr);
  }
  // Reserve extra memory (for insert)
  bool Reserve(size_t rsv_sz) {
    // Reserve memory, also have to tell
    // the current array size in order to
    // keep the array elements untouched
    return C::Reserve(m_sz, rsv_sz);
  }
  // Free all array elements
  void Free() {
    Clear();  // Clear array, destroy all
    Recycle();  // Recycle (free) allocated memory
  }
  // Clear array
  void Clear() {
    A::Destroy(Data(), m_sz);  // Destroy elements
    m_sz = 0;  // Reset array size
  }
  // Recyce (free) all unused memory
  bool Recycle() {
    // Free all extra memory but keep the array elements
    return C::FreeTill(m_sz);
  }
  // Copy items from input raw array
  // Input: arr - input raw array
  //        sz - array length
  bool Copy(const T* arr, size_t sz) {
    if (!arr || !sz) {  // Input not valid or empty
      Clear();  // Clear array
      return true;  // Copy success
    }
    if (sz > m_sz) {  // Input array is longer
      if (!Reserve(sz)) {  // Try to reserve enough space
        return false;  // Reserve fail
      }
      // Initialize extra space
      A::Initialize(Data() + m_sz, sz - m_sz);
    } else {  // Input array is shorter
      // Destroy extra elements
      A::Destroy(Data() + sz, m_sz - sz);
    }
    A::Copy(Data(), arr, sz);  // Array copy
    m_sz = sz;  // Update array size
    return true;
  }
  // Move operator
  ArrayImp& operator=(ArrayImp&& arr) {
    Move(arr);  // Array move
    return *this;
  }
  // Array (with same type) move
  bool Move(ArrayImp& arr) {
    if (this == &arr) {  // Same array
      return true;  // Do nothing
    }
    if (C::Move(m_sz, arr, arr.m_sz)) {  // Underline array move
      arr.Clear();  // Clear the input array
      return true;  // Success
    }
    return false;  // Move fail
  }
  // Move from raw array
  bool Move(T* arr, size_t sz) {
    if (!arr || !sz) {  // Invalid or empty array
      Clear();  // Clear array
      return true;
    }
    if (sz > m_sz) {  // Input is longer
      if (!Reserve(sz)) {  // Reserve enough space
        return false;  // Reserve fail
      }
      // Initialize extra reserved space
      A::Initialize(Data() + m_sz, sz - m_sz);
    } else {  // Input is shorter
      // Destroy extra elements
      A::Destroy(Data() + sz, m_sz - sz);
    }
    A::Move(Data(), arr, sz);  // Array move
    m_sz = sz;  // Update array size
    return true;
  }
  // Move from constructed array
  // H - type of array
  template<typename H>
  bool Move(H& arr) {
    bool res = false;  // Result
    if (this != &arr) {  // Not same array
      // Move from input's raw array
      res = Move(arr.Data(), arr.Size());
      arr.Clear();  // Clear input array
    }
    return res;  // Result
  }
  // Access underline array
  const T* Data() const {
    return C::Data();
  }
  // Access underline array
  T* Data() {
    return C::Data();
  }
  // Access array size
  size_t Size() const {
    return m_sz;
  }
  // Check if array is empty
  operator bool() const {
    return m_sz;
  }
  // Expand array at certain position
  // Input: p - the position expand happens
  //        t_sz - expand size
  // Return: success - iterator indicating the position of
  //         expand
  //         fail - invalid iterator
  Iter Expand(const Iter& p, size_t t_sz) {
    size_t sz = m_sz;  // Current array size
    size_t new_sz = sz + t_sz;  // New size after expand
    // Check overflow and reserve memory
    if (new_sz < sz || !Reserve(new_sz)) {
      return Iter();  // Fail, return invalid iterator
    }
    T* pos = Adjust(p);  // Adjust expand position
    T* end = End();  // End of array
    A::Initialize(end, t_sz);  // Initialize newly reserved memory
    // Move elements to make space
    A::Move(pos + t_sz, pos, end - pos);
    m_sz = new_sz;  // Update array size
    return Iter(pos);  // Return the position of expand
  }
  // Insert the same element multiple times into certain position
  // Input: p - insert position
  //        t - the insert element
  //        t_sz - repeated times
  // Return: success - the position of insert
  //         fail - invalid iterator
  Iter Insert(const Iter& p, const T& t, size_t t_sz) {
    Iter pos = Expand(p, t_sz);  // Expand array
    if (pos) {  // Expand success
      Iter end = pos + t_sz;  // End position
      // Repeat the input element
      for (Iter i = pos; i < end; ++i) {
        *i = t;
      }
    }
    return pos;  // Return insert position
  }
  // Insert a raw array into certain position
  // Input: p - insert position
  //        t, t_sz - input array and length
  // Return: success - the position of insert
  //         fail - invalid iterator
  Iter Insert(const Iter& p, const T* t, size_t t_sz) {
    Iter pos = Expand(p, t_sz);  // Expand array
    if (pos) {  // Expand success
      A::Copy(pos, t, t_sz);  // Copy raw array
    }
    return pos;  // Return insert position
  }
  // Insert array with any type
  template<typename H>
  Iter Insert(const Iter& p, const H& t) {
    return Insert(p, t.Data(), t.Size());
  }
  // Inject (move) raw array into certain position
  // Input: p - inject position
  //        t, t_sz - input array and length
  // Return: success - the position of inject
  //         fail - invalid iterator
  Iter Inject(const Iter& p, T* t, size_t t_sz) {
    Iter pos = Expand(p, t_sz);  // Expand array
    if (pos) {  // Expand success
      A::Move((T*)pos, t, t_sz);  // Move raw array
    }
    return pos;  // Return inject position
  }
  // Inject subarray
  // H: type of array
  // Input: p - inject position
  //        t - input array
  //        t_start - the start of subarray
  //        t_sz - the length of subarray
  // Return: success - the position of inject
  //         fail - invalid iterator
  template<typename H>
  Iter Inject(const Iter& p, H& t, T* t_start, size_t t_sz) {
    // Adjust input subarray
    t_start = Adjust<H>(t_start, t_sz, t);
    // Inject raw array of input
    Iter pos = Inject(p, t_start, t_sz);
    if (pos) {  // Inject success
      t.Delete(t_start, t_sz);  // Delete injected items
    }
    return pos;
  }
  // Inject constructed array
  // H - type of array
  template<typename H>
  Iter Inject(const Iter& p, H& t) {
    return Inject(p, t, t.Begin(), t.Size());  // Inject
  }
  // Delete items from array
  // Shift the tail into deleted position
  // Input: p - start position
  //        t_sz - number of items to delete
  // Return: the position of delete
  Iter Delete(const Iter& p, size_t t_sz) {
    T* pos = Adjust(p, t_sz);  // Adjust delete position and size
    T* end = End();  // End of array
    size_t sz = end - pos - t_sz;  // Remain tail size after delete
    A::Move(pos, pos + t_sz, sz);  // Shift remain tail
    A::Destroy(pos + sz, t_sz);  // Destroy unused part of tail
    m_sz -= t_sz;  // Update array size
    return Iter(pos);  // Return delete position
  }
  // Delete items from array
  // Flip tail into deleted position
  // In most of the cases, flip delete results
  // less memory move
  // It can be used for the cases that item order
  // does not matter
  // Input: p - start position
  //        t_sz - number of items to delete
  // Return: the position of delete
  Iter DeleteFlip(const Iter& p, size_t t_sz) {
    T* pos = Adjust(p, t_sz);  // Adjust delete position and size
    T* end = End();  // End of array
    size_t sz = end - pos - t_sz;  // Remain tail size after delete
    if (sz > t_sz) {  // Adjust flip size
      sz = t_sz;
    }
    A::Move(pos, end - sz, sz);  // Flip the tail
    A::Destroy(end + sz, t_sz);  // Destroy unused part of tail
    m_sz -= t_sz;  // Update array size
    return Iter(pos);  // Return the position of delete
  }
  // Access begin of array
  Iter Begin() {
    return Iter(Data());
  }
  // Constant access begin of array
  IterC Begin() const {
    return IterC(Data());
  }
  // Access end of array
  Iter End() {
    return Iter(Data() + m_sz);
  }
  // Constant access end of array
  IterC End() const {
    return IterC(Data() + m_sz);
  }
  // Access reversed begin
  Iter RBegin() {
    return Iter(End() - 1);
  }
  // Constant access reversed begin
  IterC RBegin() const {
    return IterC(End() - 1);
  }
  // Access reversed end
  Iter REnd() {
    return Iter(Begin() - 1);
  }
  // Constant access reversed end
  IterC REnd() const {
    return IterC(Begin() - 1);
  }
  // Access item
  T& operator[](size_t i) {
    return Data()[i];
  }
  // Constant access item
  const T& operator[](size_t i) const {
    return Data()[i];
  }
private:
  // Adjust array start p,
  // make sure it is in range [begin, end)
  static T* Adjust(T* p, T* begin, T* end) {
    return p < begin ? begin : (p > end ? end : p);
  }
  // Adjust array start p,
  // make sure it is in range [begin, end)
  // Also adjust array length sz,
  // make sure p + sz also in range [begin, end)
  static T* Adjust(T* p, size_t& sz, T* begin, T* end) {
    T* p_end = p + sz > end ? end : p + sz;  // Array end
    p = Adjust(p, begin, end);  // Adjust array start
    sz = p < p_end ? p_end - p : 0;  // Adjust array size
    return p;  // Return adjusted array start
  }
  // Adjust with respect input array t
  template<typename H>
  static T* Adjust(T* p, size_t&sz, H& t) {
    return Adjust(p, sz, t.Begin(), t.End());
  }
  // Adjust with respect the current array
  T* Adjust(T* p, size_t& sz) {
    return Adjust(p, sz, *this);
  }
  // Adjust array start only
  // with respect the current array
  T* Adjust(T* p) {
    return Adjust(p, Begin(), End());
  }
  size_t m_sz;  // Array size
};
// Define of static array
// Template arguments:
// T - array element type
// S - size of static array
// A - Memory allocate type
template<typename T, size_t S, typename A>
class SArrayDef {
protected:
  typedef T Type;  // Define of item
  typedef A Alloc;  // Memory allocate type
  // Constructor
  SArrayDef(memory::MMBase* mm) {
  }
  // Access underline array
  T* Data() {
    return (T*) m_data;
  }
  // Constant access underline array
  const T* Data() const {
    return (T*) m_data;
  }
  // Reserve memory
  // Since for static array, memory cannot
  // be expanded, the function only
  // check if the required exceeds the static
  // array size
  bool Reserve(size_t sz, size_t rsv_sz) {
    return rsv_sz <= S;
  }
  // Memory free (empty)
  bool FreeTill(size_t sz) {
    return false;
  }
  // Array move:
  // For example move array a to b
  // after move b == a, and a becomes empty
  // Input: sz - original array size
  //        arr - input array
  //        a_sz - input array size
  bool Move(size_t& sz, SArrayDef& arr, size_t& a_sz) {
    if (a_sz < sz) {  // current array is bigger
      // Destroy extra items in current array
      A::Destroy(Data() + a_sz, sz - a_sz);
    } else {  // Input array is bigger
      // Intialize extra space in current array
      A::Initialize(Data() + sz, a_sz - sz);
    }
    A::Move(Data(), arr.Data(), a_sz);  // Move from input to current
    sz = a_sz;  // Assign input array size to current
    return true;
  }
  // Get underline memory manager
  memory::MMBase* GetMM() const {
    return NULL;
  }
private:
  char m_data[S * sizeof(T)];  // Static array
};
// Define of dynamic array
// Template arguments:
// T - array element type
// A - memory allocation type
// R - memory expansion round
// AL - memory alignment
template<typename T, typename A, size_t R, memory::Align AL>
class DArrayDef {
  const static size_t ROUND = R ? R : 1;  // Adjust memory round
protected:
  typedef T Type;  // Array element type
  typedef A Alloc;  // Memory allocation type
  // Constructor, assign memory manager
  DArrayDef(memory::MMBase* mm)
    : m_data (NULL),
      m_mem_sz (0),
      m_mm (mm) {
  }
  // Access underline dynamic array
  T* Data() {
    return m_data;
  }
  // Constant access dynamic array
  const T* Data() const {
    return m_data;
  }
  // Reserve memory
  // Input: sz - current array size
  //        rsv_sz - required memory size
  bool Reserve(size_t sz, size_t rsv_sz) {
    if (rsv_sz <= m_mem_sz) {  // Has enough memory
      return true;  // Success
    }
    if (!m_mm) {  // No valid memory manager
      return false;  // Fail
    }
    size_t n_sz = rsv_sz * sizeof(T);  // Required memory size
    if (n_sz / rsv_sz != sizeof(T)) {  // Check overflow
      return false;
    }
    size_t r_sz = Round(n_sz);  // Round memory size
    T* ptr = (T*)m_mm->Malloc(AL, r_sz);  // Allocate memory
    if (!ptr) {  // Allocation failed
      if (r_sz <= n_sz) {  // Rounded size is real size
        return false;  // Fail
      }
      r_sz = n_sz;  // Try real size, it is smaller
      ptr = (T*)m_mm->Malloc(AL, r_sz);  // Allocate real size
      if (!ptr) {  // Still fail
        return false;  // Fail
      }
    }
    A::Initialize(ptr, sz);  // Initialize allocated memory
    A::Move(ptr, m_data, sz);  // Move items to newly allocated memory
    A::Destroy(m_data, sz);  // Destroy items in old array
    m_mm->Free(m_data);  // Free old memory
    m_data = ptr;  // Assign new array
    m_mem_sz = r_sz;  // Update reserved memory size
    return true;  // Success
  }
  // Free memory
  // After free, the array contains
  // 'sz' number of memory slots
  bool FreeTill(size_t sz) {
    if (sz >= m_mem_sz) {  // No extra memory to free
      return true;  // Do nothing and return
    }
    if (!sz) {  // After free, the array will be empty
      if (m_data) {  // Valid array memory
        m_mm->Free(m_data);  // Free all memory
        m_data = NULL;  // Null array head
        m_mem_sz = 0;  // Reset reserved memory size
      }
      return true;  // Success
    }
    // Otherwise, allocate new (smaller) size
    if (T* ptr = (T*)m_mm->Malloc(AL, sz)) {
      A::Initialize(ptr, sz);  // Initialize newly allocated memory
      A::Move(ptr, m_data, sz);  // Move items from old to new
      A::Destroy(m_data, sz);  // Destroy items in old array
      m_mm->Free(m_data);  // Free old array memory
      m_data = ptr;  // Connect with new array
      m_mem_sz = sz;  // Update reserved size
      return true;  // Success
    }
    return false;  // Fail
  }
  // Move dynamic array
  // After move the current == input,
  // and the input will be cleaned.
  // Input: sz - current array size
  //        arr - input array
  //        a_sz - input array size
  bool Move(size_t& sz, DArrayDef& arr, size_t& a_sz) {
    if (m_data) {  // Valid current array
      A::Destroy(m_data, sz);  // Destroy all items
      m_mm->Free(m_data);  // Free memory
    }
    m_data = arr.m_data;  // Copy input memory pointer
    m_mem_sz = arr.m_mem_sz;  // Copy input memory size
    m_mm = arr.m_mm;  // Copy input memory manager
    sz = a_sz;  // Copy input array size
    arr.m_data = NULL;  // Null input memory pointer
    arr.m_mem_sz = 0;  // Reset input memory size
    a_sz = 0;  // Reset input array size
    return true;  // Success
  }
  // Access underline memory manger
  memory::MMBase* GetMM() const {
    return m_mm;
  }
private:
  // Round memory size
  // After round the memory size 'sz' will be
  // multiple of ROUND
  size_t Round(size_t sz) {
    if (size_t r = JNU_MOD(sz, ROUND)) {  // Has mod
      size_t n_sz = ROUND - r + sz;  // Round
      sz = JNU_MAX(n_sz, sz);  // Prevent overflow
    }
    return sz;  // Return rounded size
  }
  T* m_data;  // Dynamic array
  size_t m_mem_sz;  // Reserved memory size
  memory::MMBase* m_mm;  // Memory manager
};
// Define of hybrid array
// If array size is small, it is static array
// If the size becomes bigger, it uses dynamic array
// Template arguments:
// T - array element type
// S - static array size
// A - memory allocator type
// R - memory round for dynamic array
// AL - memory alignment for dynamic array
template<typename T, size_t S, typename A, size_t R, memory::Align AL>
class HArrayDef : private SArrayDef<T, S, A>,
                  private DArrayDef<T, A, R, AL> {
  typedef SArrayDef<T, S, A> SArr;  // Static array
  typedef DArrayDef<T, A, R, AL> DArr;  // Dynamic array
public:
  typedef T Type;  // Array element type
  typedef A Alloc;  // Memory allocator type
  // Constructor
  HArrayDef(memory::MMBase* mm)
    : SArr (mm),
      DArr (mm) {
  }
  // Access underline array
  T* Data() {
    // If dynamic array is allocated, then use dynamic array
    // otherwise use static array
    return DArr::Data() ? DArr::Data() : SArr::Data();
  }
  // Constant access underline array
  const T* Data() const {
    return DArr::Data() ? DArr::Data() : SArr::Data();
  }
  // Reserve memory
  // Input: sz - current array size
  //        rsv_sz - required memory size
  bool Reserve(size_t sz, size_t rsv_sz) {
    if (DArr::Data()) {  // Has dynamic array
      return DArr::Reserve(sz, rsv_sz);  // Dynamic allocate
    }
    if (SArr::Reserve(sz, rsv_sz)) {  // Try static array
      return true;
    }
    // In this case, dynamic array is
    // not allocated before
    if (DArr::Reserve(0, rsv_sz)) {  // Allocate dynamic array
      A::Initialize(DArr::Data(), sz);  // Initialize array
      A::Move(DArr::Data(), SArr::Data(), sz);  // Move from static array
      A::Destroy(SArr::Data(), sz);  // Destroy items from static array
      return true;  // Success
    }
    return false;  // Fail
  }
  // Free memory
  // After free the memory has size 'sz'
  bool FreeTill(size_t sz) {
    if (!DArr::Data()) {  // No dynamic array
      return SArr::FreeTill(sz);  // Only free static array
    }
    // Has dynamic array
    if (sz > S) {  // After free, size still unfit for static array
      return DArr::FreeTill(sz);  // Just free in dynamic array
    }
    // After free, size fit for static array
    A::Initialize(SArr::Data(), sz);  // Initialize static array
    A::Move(SArr::Data(), DArr::Data(), sz);  // Move from dynamic array
    A::Destroy(DArr::Data(), sz);  // Destroy items in dynamic array
    return DArr::FreeTill(0);  // Free entire dynamic array
  }
  // Move hybrid array
  // Input: sz - current array size
  //        arr - input array
  //        a_sz - input array size
  bool Move(size_t& sz, HArrayDef& arr, size_t& a_sz) {
    if (!DArr::Data()) {  // Current is static
      if (!arr.DArr::Data()) {  // Input is static
        return SArr::Move(sz, (SArr&) arr, a_sz);  // Static move
      }
      // Input is dynamic
      A::Destroy(SArr::Data(), sz);  // Destroy static array
      return DArr::Move(sz, (DArr&) arr, a_sz);  // Move to dynamic array
    }
    // Current is dynamic
    if (arr.DArr::Data()) {  // Input is dynamic as well
      return DArr::Move(sz, (DArr&) arr, a_sz);  // Dynamic move
    }
    // Input is static
    A::Destroy(DArr::Data(), sz);  // Destroy current dynamic array
    DArr::FreeTill(0);  // Free dynamic array
    sz = 0;  // Reset array size
    return SArr::Move(sz, (SArr&) arr, a_sz);  // Static move
  }
  // Access under line memory manager
  memory::MMBase* GetMM() const {
    return DArr::GetMM();
  }
};
// Define of static array
template<typename T, size_t S, typename A>
using SArray = ArrayImp<SArrayDef<T, S, A>>;
// Define of dynamic array
template<typename T, typename A, size_t R, memory::Align AL = 8>
using DArray = ArrayImp<DArrayDef<T, A, R, AL>>;
// Define of hybrid array
template<typename T, size_t S, typename A, size_t R, memory::Align AL = 8>
using HArray = ArrayImp<HArrayDef<T, S, A, R, AL>>;
}

#endif
