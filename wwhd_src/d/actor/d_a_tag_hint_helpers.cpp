/* TU-local SafeString virtual functions for the WWHD hint trigger. */
#include "d/actor/d_a_tag_hint.h"

/* 024AA534 */
static void daTag_Hint_SafeString_destructor(void* string, s32 flags) {
    if (string && (flags & 1)) {
        operator_delete(string);
    }
}

/* 024AA59C */
static void daTag_Hint_SafeString_prepare(void* string) {}

/* WWHD glue
 */
