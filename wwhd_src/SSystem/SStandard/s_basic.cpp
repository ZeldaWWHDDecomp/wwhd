/* s_basic: memory fill helpers (SSystem), WWHD. 
 * Translation unit 0201B664..0201B718: sBs_FillArea_s, sBs_ClearArea, and the header __sinit every
 * game TU gets. Ported from the GameCube s_basic.cpp. */
#include "bindings.h"

namespace s_basic_cpp {

/* 0201B664: fill (bytes / 2) halfwords with value (bdnz loop; nothing for fewer than 2 bytes) */
static void sBs_FillArea_s(void* pPtr, u32 pNumBytes, s16 pValue) {
    WWHD_FUNC(0x0201B664, void, pPtr, pNumBytes, pValue);
    u32 p = gabi::ea(pPtr);
    for (u32 i = 0; i < pNumBytes / 2; i++) {
        gabi::store<s16>(p, pValue);
        p += 2;
    }
}
VERIFY(0x0201B664, sBs_FillArea_s);

/* 0201B680: tail call of sBs_FillArea_s with value 0 */
static void sBs_ClearArea(void* pPtr, u32 pNumBytes) {
    WWHD_FUNC(0x0201B680, void, pPtr, pNumBytes);
    sBs_FillArea_s(pPtr, pNumBytes, 0);
}
VERIFY(0x0201B680, sBs_ClearArea);

/* 0201B688 __sinit: header statics block at 101FFC08 ({-pi, pi} from 10003758, two one-byte objects
 * at +8/+9, zeroed 16-byte object at +0xC), each registered for destruction (records 1018D484, +0xC, +0x18) */
static void s_basic_sinit() {
    WWHD_FUNC(0x0201B688, void);
    gabi::store<u32>(0x101FFC1C, 0); gabi::store<u32>(0x101FFC14, 0);
    gabi::store<u32>(0x101FFC20, 0); gabi::store<u32>(0x101FFC18, 0);
    gabi::call(0x028F026C, 0x1018D484u);
    f32 lo = gabi::load<f32>(0x10003758), hi = gabi::load<f32>(0x1000375C);
    gabi::store<f32>(0x101FFC08, lo); gabi::store<f32>(0x101FFC0C, hi);
    gabi::call(0x028ED6F8, 0x101FFC10u); gabi::call(0x028F026C, 0x1018D490u);
    gabi::call(0x028EAB2C, 0x101FFC11u); gabi::call(0x028F026C, 0x1018D49Cu);
}
VERIFY(0x0201B688, s_basic_sinit);

} // namespace s_basic_cpp
