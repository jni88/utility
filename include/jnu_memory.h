// By JNI
// Implementation of memory management interface
// The interface can either be a wrap of buildin
// functions (posix_memalign), or
// it can be built from custom-made aligned memory
// allocation functions
// In addition, an application-level memory allocation
// mechanism is developed in order to minimizing
// system call

#ifndef JNU_MEMORY_H
#define JNU_MEMORY_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <new>
#include "jnu_defines.h"

namespace jnu {
namespace memory {
typedef size_t Align;  // Define memory alignment
// Check if a pointer is aligned
// Input: al - the required alignment (pow of 2)
//        ptr - the pointer need to be checked
// Return: true - aligned, false - not aligned
static bool IsAligned(const Align& al, void* ptr) {
  // If alignment is less than 1, always true
  // Then check if pointer can be divided by alignment
  return al <= 1 || JNU_MOD((uintptr_t)ptr, al) == 0;
}
// Base of memory manage interface
class MMBase {
public:
  // Virtual of memory allocate
  virtual void* Malloc(const Align& al, size_t sz) = 0;
  // Virtual of memory free
  virtual void Free(void* ptr) = 0;
protected:
  // Base constructor
  MMBase() {}
  // Virtual of deconstructor
  virtual ~MMBase() {}
};
// Template of memory manage interface
// Template arguments:
// C: Implementation of memory manage functions
// A...: Arguments for construct object C
template<typename C, typename... A>
class MM : public MMBase {
public:
  typedef C Type;  // Memory manage implementation class
  // Constructor, construct object of C (m_mm)
  MM(A... arg)
    : m_mm (arg...) {
  }
  // Deconstructor
  ~MM() {
  }
  // Implementation of memory allocation
  void* Malloc(const Align& al, size_t sz) {
    return m_mm.Malloc(al, sz);
  }
  // Implementation of memory free
  void Free(void* ptr) {
    m_mm.Free(ptr);
  }
  // Access underline implementation object
  C& GetImp() {
    return m_mm;
  }
private:
  C m_mm;  // Implementation object
};
// Implementation using buildin methods
class Buildin {
  template<typename C, typename... A> friend class MM;
public:
  // Aligned memory allocation using 'posix_memalign'
  // Input: al - required memory alignment
  //        sz - required memory size
  static void* Malloc(const Align& al, size_t sz) {
    // Check memory size and alignment
    // Need non-empty size and alignment has to be pow of 2
    if (!sz || !JNU_IS_POW_2(al)) {
      return NULL;
    }
    // Alignment less (equal) 1, using malloc
    if (al <= 1) {
      return malloc(sz);
    }
    // All else, using posix_memalign
    void* ptr;
    if (posix_memalign(&ptr, al < JNU_PTR_SZ ? JNU_PTR_SZ : al, sz) != 0) {
      return NULL;  // posix_memalign failed
    }
    return ptr;  // On success, return the memory pointer
  }
  // Free memory, directly call 'free'
  static void Free(void* ptr) {
    free(ptr);
  }
private:
  // Buildin class remains static
  Buildin() {}
};
// Custom implementation of aligned memory allocation
// Template arguments:
// C - The class implementing 'Malloc' and 'Free'
// A... - Arguments for constructing C
template<typename C, typename... A>
class Custom {
  typedef size_t SZ_T;  // Define of memory offset size type
public:
  // Constructor, constructing object of C (m_imp)
  Custom(A... arg)
    : m_imp(arg...) {
  }
  // Deconstructor
  ~Custom() {
  }
  // Aligned memory allocation
  // Input: al - memory alignment
  //        sz - memory size
  void* Malloc(const Align& al, size_t sz) {
    // Check memory size and alignment
    if (!sz || !JNU_IS_POW_2(al)) {
      return NULL;
    }
    // In case of zero alignment,
    // make it equal to size of pointer
    Align r_a = al == 0 ? JNU_PTR_SZ : al;
    // Calculate memory size, need include extra memory
    // for storing offset size and for doing memory
    // align
    size_t mem_sz = r_a + sizeof(SZ_T);
    if (mem_sz < r_a) {  // Check overflow
      return NULL;
    }
    mem_sz += sz;  // Real size for allocation
    if (mem_sz < sz) {  // Check overflow
      return NULL;
    }
    // Allocation using underline implementation
    if (char* ptr = (char*)m_imp.Malloc(mem_sz)) {
      // Leave space for storing offset value
      // t is pointing to the memory return to user
      char* t = ptr + sizeof(SZ_T);
      // Make t aligned
      t += (al - JNU_MOD((uintptr_t)t, r_a));
      // Store the offset between client memory pointer
      // and the Malloc memory pointer
      *((SZ_T*)(t - sizeof(SZ_T))) = (SZ_T)(t - ptr);
      return t;  // Return client memory pointer
    }
    return NULL;  // On fail
  }
  // Free memory
  void Free(void* ptr) {
    if (ptr) {  // Check inpt client memory pointer
      // Get the pointer offset value
      SZ_T offset = *((SZ_T*)((char*)ptr - sizeof(SZ_T)));
      // Apply the offset and free memory
      m_imp.Free((char*)ptr - offset);
    }
  }
private:
  C m_imp;  // Underline implementation
};
// Default implemention for custom interface
class CustomDef {
public:
  // Using 'malloc' for memory allocation
  static void* Malloc(size_t sz) {
    return malloc(sz);
  }
  // Using 'free' for memory free
  static void Free(void* ptr) {
    free(ptr);
  }
};
// Buildin memory manage
typedef MM<Buildin> MMBuildin;
// Default custom memory manage
typedef MM<Custom<CustomDef>> MMCustomDef;
// Memory object (unique), it maintains allocated memory
// automatically deallocate it when finishing using
// it
class Mem {
public:
  // Constructor:
  // mm is the pointer of memory manage (buildin or custom)
  Mem(MMBase* mm = NULL)
    : m_mm (mm) {
    Reset();  // Initialize
  }
  // Deconstructor
  ~Mem() {
    Free();  // Free allocated memory
  }
  // Keep unique, no copy constructor allowed
  Mem(const Mem& r) = delete;
  // Keep unique, no assign operator allowed
  Mem& operator=(const Mem& r) = delete;
  // Move constructor
  Mem(Mem&& r) {
    Reset();  // Initialize
    Move(r);  // Move r to this
  }
  // Move operator
  Mem& operator=(Mem&& r) {
    Move(r);  // Move r to this
    return *this;
  }
  // Bool operator, check underline memory is available
  operator bool() const {
    return Ptr() != NULL;
  }
  // Access underline memory
  void* Ptr() const {
    return m_ptr;
  }
  // Underline memory size
  size_t Size() const {
    return m_sz;
  }
  // Check if underline memory is aligned
  bool IsAligned(const Align& al) const {
    return jnu::memory::IsAligned(al, m_ptr);
  }
  // Allocate memory
  // Input: mm - memory manage interface used
  //        al - memory alignment required
  //        sz - required memory size
  bool Malloc(MMBase* mm, const Align& al, size_t sz) {
    if (!sz) {  // Empty size
      Free();  // Free current memory
      return true;
    }
    if (mm) {  // Valid memory manage interface
      if (void* ptr = mm->Malloc(al, sz)) {  // Allocate
        Free();  // Free current memory
        m_mm = mm;  // Assign memory manage interface
        m_ptr = ptr;  // Assign current allocated memory
        m_sz = sz;  // Assign memory size
        return true;
      }
    }
    return false;  // Fail
  }
  // Allocate using remembered memory manage interface
  bool Malloc(const Align& al, size_t sz) {
    return Malloc(m_mm, al, sz);
  }
  // Allocate and reset memory to zero
  // Input: mm - memory manage interface used
  //        al - memory alignment required
  //        n - number of memory units
  //        sz - size of each units
  bool Calloc(MMBase* mm, const Align& al, size_t n, size_t sz) {
    if (!n || !sz) {  // Empty memory
      Free();  // Free
      return true;
    }
    size_t mem_sz = n * sz;  // Calculate memory size
    // Check overflow and do malloc
    if ((mem_sz / sz) == n && Malloc(mm, al, mem_sz)) {
      memset(m_ptr, 0, mem_sz);  // Success, reset memory
      return true;
    }
    return false;  // Fail
  }
  // Allocate and reset using current memory manage interface
  bool Calloc(const Align& al, size_t n, size_t sz) {
    return Calloc(m_mm, al, n, sz);
  }
  // Re-allocate
  // Input: mm - memory manage interface used
  //        al - memory alignment required
  //        sz - new memory size
  bool Realloc(MMBase* mm, const Align& al, size_t sz) {
    if (!sz) {  // Empty new size
      Free();  // Free memory
      return true;
    }
    // new size smaller than current size
    if (sz <= m_sz && IsAligned(al) && mm == m_mm) {
      m_sz = sz;  // Just update size
      return true;
    }
    Mem r(mm);  // Need re-allocate
    if (r.Malloc(al, sz)) {  // Allocate new memory
      memcpy(r.m_ptr, m_ptr, m_sz);  // Copy content
      Move(r);  // Move new memory to this
      return true;
    }
    return false;
  }
  // Re-allocate using existing memory manage interface
  bool Realloc(const Align& al, size_t sz) {
    return Realloc(m_mm, al, sz);
  }
  // Free memory
  void Free() {
    if (m_ptr) {  // Check underline memory
      m_mm->Free(m_ptr);  // Free
      Reset();  // Initialize
    }
  }
protected:
  // Access underline memory manage interface
  MMBase* GetMM() const {
    return m_mm;
  }
  // Move memory
  void Move(Mem& r) {
    // Cannot move it to itself
    if (this != &r) {
      Free();  // Free current one
      m_mm = r.m_mm;  // Copy memory interface
      m_ptr = r.m_ptr;  // Copy memory pointer
      m_sz = r.m_sz;  // Copy memory size
      r.Reset();  // Initialize input
    }
  }
private:
  // Initialize
  void Reset() {
    m_ptr = NULL;  // Reset memory pointer
    m_sz = 0;  // Reset size
  }
  MMBase* m_mm;  // Memory manage interface
  void* m_ptr;  // Allocated memory pointer
  size_t m_sz;  // Allocated memory size
};
// Object record maintains create objects
// using 'new' and 'delete'
// template argument: C - object type
// It inherits 'Mem'
template<typename C>
class Obj : private Mem {
public:
  // Constructor
  // mm - default memory interface
  Obj(MMBase* mm = NULL)
    : Mem (mm) {
    Reset();  // Initialize
  }
  // Deconstructor
  ~Obj() {
    Finalize();  // Clean up objects (deconstructor)
  }
  // Copy constructor not allowed
  Obj(const Obj& o) = delete;
  // Assign operator not allowed
  Obj& operator=(const Obj& o) = delete;
  // Move constructor
  Obj(Obj&& o) {
    Reset();  // Initialize
    Move(o);  // Move
  }
  // Move operator
  Obj& operator=(Obj&& o) {
    Move(o);  // Move
    return *this;
  }
  // Bool operator, has objects allocated
  operator bool() const {
    return Size() > 0;
  }
  // Access underline object pointer
  C* Ptr() const {
    return (C*)Mem::Ptr();
  }
  // Get number of objects allocated
  size_t Size() const {
    return m_sz;
  }
  // Check if memory is aligned
  bool IsAligned(const Align& al) const {
    return Mem::IsAligned(al);
  }
  // New single object
  // template arguments A: for construct object of C
  // Input: mm - memory manage interface
  //        al - memory alignment
  //        arg - C's constructor arguments
  template<typename... A>
  bool New(MMBase* mm, const Align& al, A... arg) {
    Obj t(mm, 1);  // Template object
    if (t.Malloc(al, sizeof(C))) {  // Allocate memory
      ::new (t.Ptr()) C(arg...);  // New object
      Move(t);  // Move to this
      return true;
    }
    return false;  // Fail
  }
  // New single object using existing memory manage interface
  template<typename... A>
  bool New(const Align& al, A... arg) {
    return New(Mem::GetMM(), al, arg...);
  }
  // New array of objects
  // Input: mm - memory manage interface
  //        al - memory alignment
  //        sz - number of objects
  bool NewArr(MMBase* mm, const Align& al, size_t sz) {
    if (DoNewArr(mm, al, sz)) {  // Allocate memory
      ::new (Ptr()) C[sz];  // New objects using default constructor
      return true;
    }
    return false;  // Fail
  }
  // New array of objects using existing memory manage interface
  bool NewArr(const Align& al, size_t sz) {
    return NewArr(Mem::GetMM(), al, sz);
  }
  // New array of objects
  // using specific constructors
  // template argument A: C's constructor argument list
  template<typename... A>
  bool NewArr(MMBase* mm, const Align& al, size_t sz, A... arg) {
    if (DoNewArr(mm, al, sz)) {  // allocate memory
      for (size_t i = 0; i < sz; ++i) {  // Constructor each object
        ::new (Ptr() + i) C(arg...);
      }
      return true;
    }
    return false;  // Fail
  }
  // New array of objects using existing memory manage interface
  template<typename... A>
  bool NewArr(const Align& al, size_t sz, A... arg) {
    return NewArr(Mem::GetMM(), al, sz, arg...);
  }
  // Delete object
  void Delete() {
    Finalize();  // Call objects' deconstructor
    Mem::Free();  // Free underline memory
  }
private:
  // Private constructor
  Obj(MMBase* mm, size_t sz)
    : Mem (mm),
      m_sz (sz) {
  }
  // Move object
  void Move(Obj& o) {
    if (this != &o) {  // Not move to itself
      Finalize();  // Delete current objects
      Mem::Move(o);  // Move memory from o to this
      m_sz = o.m_sz;  // Update number of objects
      o.Reset();  // Initialize input object
    }
  }
  // Object reset (reset object count)
  void Reset() {
    m_sz = 0;
  }
  // Delete object
  void Finalize() {
    size_t sz = m_sz;  // Number of objects
    // Deconstructor each objects
    for (size_t i = 0; i < sz; ++i) {
      Ptr()[i].~C();
    }
  }
  // Allocate memory for array of objects
  bool DoNewArr(MMBase* mm, const Align& al, size_t sz) {
    if (!sz) {  // No objects
      Delete();  // Delete all objects allocated
      return true;
    }
    Obj t(mm, sz);  // Template object record
    size_t mem_sz = sz * sizeof(C);  // Calculate memory size
    // Check overflow and allocate memory
    if (mem_sz / sz == sizeof(C) && t.Malloc(al, mem_sz)) {
      Move(t);  // Move to this
      return true;
    }
    return false;  // Fail
  }
  size_t m_sz;  // Number of allocated objects
};
}
}

#endif
