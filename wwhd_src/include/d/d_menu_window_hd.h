/** Place-name state identity companion. */
#pragma once
#include "d/hd_parts.h"
struct hdPlaceNameState_c { static u32 stateKey(hdPartsStateId_c*); };

/* Allocation/header layout only; the binary establishes no container role.
 * The constructor clears this eight-byte header and leaves storage untouched. */
struct hdPlaceNameWork_c {
    be<u32> mHeaderWord;
    be<u16> mHeaderLowerHalf;
    be<u16> mHeaderUpperHalf;
    u8 mStorage[0xA0];
    static hdPlaceNameWork_c* construct(hdPlaceNameWork_c* work);
};
WWHD_OFFSET(hdPlaceNameWork_c, mHeaderWord, 0);
WWHD_OFFSET(hdPlaceNameWork_c, mHeaderLowerHalf, 4);
WWHD_OFFSET(hdPlaceNameWork_c, mHeaderUpperHalf, 6);
WWHD_OFFSET(hdPlaceNameWork_c, mStorage, 8);
WWHD_SIZE(hdPlaceNameWork_c, 0xA8);
