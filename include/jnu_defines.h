// By JNI
// General macros used throughout the project

#ifndef JNU_DEFINES_H
#define JNU_DEFINES_H

// Stringfy a macro define
#define JNU_XSTRF(X) JNU_STRF(X)
#define JNU_STRF(X) #X

// Define of Mod operation
#define JNU_MOD(X, Y) ((X) - (X) / (Y) * (Y))
#define JNU_MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define JNU_MIN(X, Y) ((Y) > (X) ? (X) : (Y))

// Define of pow_2 operations
#define JNU_IS_POW_2(X) ((((X) - 1) & (X)) == 0)
#define JNU_POW_2(X) (1 << (X))

// Get size of pointer
#define JNU_PTR_SZ sizeof(void*)

#endif
