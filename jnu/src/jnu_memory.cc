#include <stdlib.h>
#include <stdint.h>
#include "jnu_defines.h"
#include "jnu_memory.h"

using namespace jnu;
using namespace memory;

void* Standard::Malloc(const Align& al, size_t sz) {
  return sz ? aligned_alloc(al, sz) : NULL;
}
void Standard::Free(void* ptr) {
  free(ptr);
}
void* Custom::Malloc(const Align& al, size_t sz) {
  if (al <= 0) {
    return NULL;
  }
  size_t mem_sz = al + sizeof(SZ_T);
  if (mem_sz < al) {
    return NULL;
  }
  mem_sz += sz;
  if (mem_sz < sz) {
    return NULL;
  }
  if (char* ptr = (char*)malloc(mem_sz)) {
    char* t = ptr + sizeof(SZ_T);
    t += (al - JNU_MOD((uintptr_t)t, al));
    *((SZ_T*)(t - sizeof(SZ_T))) = (SZ_T)(t - ptr);
    return t;
  }
  return NULL;
}
void Custom::Free(void* ptr) {
  if (ptr) {
    SZ_T offset = *((SZ_T*)((char*)ptr - sizeof(SZ_T)));
    free((char*)ptr - offset);
  }
}
