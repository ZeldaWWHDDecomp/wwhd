/* JUT_ASSERT (HD release builds keep the call: 0273AA24(file, line, message)). The file name
 * and the condition text are host strings; the unit's glue maps them to its guest copies
 * (str lines; GHS printed NULL as "(0)"). */
#pragma once
#include "bindings.h"

namespace JUTAssertion {
inline void showAssert(const char* file, int line, const char* msg) { gabi::call(0x0273AA24, file, line, msg); }
}  // namespace JUTAssertion
inline void JUTAssertion_showAssert(const char* file, int line, const char* msg) { JUTAssertion::showAssert(file, line, msg); }
/* the condition is stringized after macro expansion, as the original's two-level JUT_ASSERT did (its FALSE
 * printed as 0); our NULL expands to __null, so a NULL condition uses JUT_ASSERT_MSG with the original's text */
#define JUT_ASSERT_STR_(X) #X
#define JUT_ASSERT_XSTR_(X) JUT_ASSERT_STR_(X)
#define JUT_ASSERT(LINE, COND) ((COND) ? (void)0 : JUTAssertion::showAssert(__FILE_NAME__, LINE, JUT_ASSERT_XSTR_(COND)))
/* the same check with the original's message text where our condition's spelling differs from the original's
 * stringized condition (member names, NULL printed as (0) or 0): the guest gets the original's string */
#define JUT_ASSERT_MSG(LINE, COND, MSG) ((COND) ? (void)0 : JUTAssertion::showAssert(__FILE_NAME__, LINE, MSG))
