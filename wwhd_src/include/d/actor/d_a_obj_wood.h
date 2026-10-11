/* Wood placement actor. GameCube members move by +0x11C in WWHD (size 0x524).
 * HD adds a virtual destructor in the actor base. */
#pragma once
#include "gc_api/gc_api.h"
#include "gc_api/d/d_wood.h"
namespace daObjWood {
class Act_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x10033898, AAB_VT = 0x10033888 };
    Act_c();
    inline cPhs_State _create();
    inline bool _delete() { return true; }
    inline bool _execute();
    inline bool _draw() { return true; }
    /* 0x3AC */ be<u32> field_0x290[3]; // Unused GameCube members.
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
};
WWHD_OFFSET(Act_c, mStts, 0x3B8);
WWHD_OFFSET(Act_c, mCyl, 0x3F4);
WWHD_SIZE(Act_c, 0x524);
}
