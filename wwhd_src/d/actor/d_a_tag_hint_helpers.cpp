/* TU-local SafeString virtual functions for the WWHD hint trigger. */
#include "d/actor/d_a_tag_hint.h"

static void daTag_Hint_SafeString_destructor(void *string, s32 flags) {
  WWHD_FUNC(0x024AA534, void, string, flags);
  if (string && (flags & 1)) {
    gabi::call(0x0273AF40, string);
  }
}
VERIFY(0x024AA534, daTag_Hint_SafeString_destructor);

static void daTag_Hint_SafeString_prepare(void *string) {
  WWHD_FUNC(0x024AA59C, void, string);
}
VERIFY(0x024AA59C, daTag_Hint_SafeString_prepare);
