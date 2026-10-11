/**
 * d_level_se.cpp - Level sound emitter.
 * GameCube-style source verified against WWHD (tools/verify, d_level_se).
 */
#include "d/d_level_se.h"

static const u32 levelSeVtable = 0x100505F4;

/* 02586F8C */
static BOOL dLevelSe_Execute(dLevelSe_c* i_this) {
    u8 flag = i_this->mFlag;
    u32 soundId = i_this->mParam;
    if (flag & dLevelSe_c::STOPPED) {
        return TRUE;
    }
    if (flag & dLevelSe_c::CUSTOM_PARAM) {
        mDoAud_seStart(soundId, &i_this->mPos, i_this->mSoundParam);
    } else if (flag & dLevelSe_c::CUSTOM_REVERB) {
        s32 reverb = i_this->mReverb;
        mDoAud_seStart(soundId, &i_this->mPos, i_this->mSoundParam, reverb);
    } else {
        mDoAud_seStart(soundId, &i_this->mPos);
    }
    return TRUE;
}

/* 02587120 */
static BOOL dLevelSe_IsDelete() {
    // HD: this entry has no actor argument.
    return TRUE;
}

/* 02587000 */
static BOOL dLevelSe_Delete(dLevelSe_c* i_this) {
    mDoAud_seDeleteObject(&i_this->mPos);
    // HD: GHS also destroys the process base, without freeing its storage.
    if (i_this) {
        base_process_class__dt(i_this, 0);
    }
    return TRUE;
}

/* 02587048 */
static s32 dLevelSe_Create(dLevelSe_c* i_this) {
    // HD: placement construction initializes the base and this TU's vtable.
    if (i_this) {
        base_process_class__ct(i_this);
        i_this->__vtbl = levelSeVtable;
    }
    return cPhs_COMPLEATE_e;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0258708C */
static void __sinit_d_level_se_cpp() {
    sinit_header_statics_rodata(0x1047762C, 0x101E9BC8, 0x10477638, 0x1005060C);
}

/* WWHD glue
ext 025E19CC void mDoAud_seStart(u32 soundId, cXyz* pos)
ext 025E1A04 void mDoAud_seStart(u32 soundId, cXyz* pos, u32 param)
ext 025DD5F0 void base_process_class__ct(base_process_class* i_this)
ext 025DD630 void base_process_class__dt(base_process_class* i_this, u32 flags)
*/
