// By JNI
// General macros used throughout the project

#ifndef JNU_DEFINES_H
#define JNU_DEFINES_H

// Stringfy a macro define
#define JNU_XSTRF(X) JNU_STRF(X)
#define JNU_STRF(X) #X

// Define of Mod operation
#define JNU_MOD(X, Y) ((X) - (X) / (Y) * (Y))

#endif
