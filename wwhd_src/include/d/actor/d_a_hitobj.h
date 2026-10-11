/* Hit object: WWHD layout. GameCube members move by +0x11C with the actor base. */
#pragma once
#include "gc_api/gc_api.h"

class hitobj_class : public fopAc_ac_c {
public:
    hitobj_class();
    enum : u32 { __vt = 0x10011324 }; // HD: virtual destructor
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<u8> mUnusedParam;
    /* 0x3B5 */ u8 _3B5;
    /* 0x3B6 */ be<s16> mTimer;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Sph mSph;
};
WWHD_OFFSET(hitobj_class, mStts, 0x3B8);
WWHD_OFFSET(hitobj_class, mSph, 0x3F4);
WWHD_SIZE(hitobj_class, 0x520);


BOOL fopKyM_Delete(void*);
void dCcD_Sph_construct(dCcD_Sph*);
void dCcD_Sph_destroy(dCcD_Sph*, int);
