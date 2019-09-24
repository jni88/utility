#ifndef JNU_UNIT_TEST_H
#define JNU_UNIT_TEST_H

#include "jnu_defines.h"

#define JNU_UT_MSG_A(F, ...) printf(F "\n", ##__VA_ARGS__)
#define JNU_UT_MSG(F) JNU_UT_MSG_A("%s", F)

#define JNU_UT_OUTPUT_A(L, F, ...) JNU_UT_MSG_A("%s: In function '%s':\n" \
                                                "%s:%d: " #L ": " F,\
                                                __FILE__, \
                                                __PRETTY_FUNCTION__, \
                                                __FILE__, \
                                                __LINE__, \
                                                ##__VA_ARGS__)
#define JNU_UT_OUTPUT(L, F) JNU_UT_OUTPUT_A(L, "%s", F)

#define JNU_UT_INFO_A(F, ...) JNU_UT_OUTPUT_A(info, F, ##__VA_ARGS__);
#define JNU_UT_INFO(F) JNU_UT_OUTPUT(info, F);

#define JNU_UT_WARN_A(F, ...) JNU_UT_OUTPUT_A(warn, F, ##__VA_ARGS__);
#define JNU_UT_WARN(F) JNU_UT_OUTPUT(warn, F);

#define JNU_UT_ERROR_A(F, ...) JNU_UT_OUTPUT_A(error, F, ##__VA_ARGS__);
#define JNU_UT_ERROR(F) JNU_UT_OUTPUT(error, F);

#define JNU_UT_CHECK(E) if (!(E)) { JNU_UT_ERROR("Check failed: " #E); }

namespace jnu {
}

#endif
