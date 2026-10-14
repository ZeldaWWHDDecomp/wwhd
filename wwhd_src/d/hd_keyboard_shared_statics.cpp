// HD message-component header statics emitted in the keyboard group. Private source.
#include "d/hd_keyboard_screen.h"
static hdKeyboardStatRange_c sMessageRange;
static hdKeyboardStatRegistrations_c sMessageRegistrations;
static const hdKeyboardStatLimits_c sMessageLimits;
/* 026BD8D0 */
static hdScreenType* keyboardMessageBaseType(hdScreenType* type) {
    return type->mParent;
}
/* 026BD9D0 */
static void initializeKeyboardMessageStatics() {
    initializeKeyboardStatics(&sMessageRange, &sMessageRegistrations, &sMessageLimits);
}
/* WWHD glue (tools/verify/autoglue.py)
 data sMessageRange 10498DA0
 data sMessageRegistrations 101F6768
 data sMessageLimits 100FF6A0
*/
