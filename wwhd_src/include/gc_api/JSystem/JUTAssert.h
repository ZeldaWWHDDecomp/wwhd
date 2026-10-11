/* JUT_ASSERT (HD release builds keep the call: 0273AA24(file, line, message)). The file name
 * and the condition text are host strings; the unit's glue maps them to its guest copies
 * (str lines; GHS printed NULL as "(0)"). */
#pragma once
#include "bindings.h"

namespace JUTAssertion {
inline void showAssert(const char* file, int line, const char* msg) { gabi::call(0x0273AA24, file, line, msg); }
}  // namespace JUTAssertion
inline void JUTAssertion_showAssert(const char* file, int line, const char* msg) { JUTAssertion::showAssert(file, line, msg); }
#define JUT_ASSERT(LINE, COND) ((COND) ? (void)0 : JUTAssertion::showAssert(__FILE_NAME__, LINE, #COND))
