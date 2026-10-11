/**
 * d_a_obj_msdan.cpp
 * Object - staircase spawner and switch-driven Msdan event.
 *
 * WWHD, verified against the recompiled original (tools/verify, d_a_obj_msdan).
 * GameCube methods are placeholders; interface names follow its header.
 */
#include "d/actor/d_a_obj_msdan.h"

// HD: this TU's common header statics.
const gc_tu_data gc_tu = {
    .headerStatics = {0x1046B90C, 0x101CBC10, 0x1046B918},
};

// HD: archive and event names have distinct guest copies of "Msdan".
static const char archiveName[6] = {0x4D, 0x73, 0x64, 0x61, 0x6E, 0};
static const char* M_arcname = reinterpret_cast<const char*>(archiveName);

using daObjMsdan::Act_c;

/* inlined into Mthd_Create */
Act_c::Act_c() {
    // HD: GHS constructs the actor base and stores this class's vtable.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* 0237B360 */
// HD: out-of-line daObj::PrmAbstract<Act_c::Prm_e> in this TU.
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
u32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }
u32 Act_c::prm_get_size() { return PrmAbstract(this, PRM_SIZE_W, PRM_SIZE_S); }
u32 Act_c::prm_get_sound() { return PrmAbstract(this, PRM_SOUND_W, PRM_SOUND_S); }
u32 Act_c::prm_get_evId() { return PrmAbstract(this, PRM_EVID_W, PRM_EVID_S); }

/* 0237AD28 */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        /* the steps go up in a spiral of radius 50 around the actor */
        s16 angY = current.angle.y;
        cXyz pos;
        csXyz angle;
        angle.x = current.angle.x;
        pos.x = current.pos.x;
        angle.y = (s16)(angY - 0x8000);
        f32 y = current.pos.y;
        pos.y = y;
        angle.z = current.angle.z;
        pos.z = current.pos.z;
        s32 count;
        u32 sub;
        if (prm_get_size() != 0) {
            pos.y = y + 800.0f;
            count = 31;
            sub = 0;
        } else {
            pos.y = y + 400.0f;
            count = 15;
            sub = 0x1000;
        }
        for (;;) {
            pos.x = fmadds(cM_ssin(angY), 50.0f, pos.x);
            pos.z = fmadds(cM_scos(angY), 50.0f, pos.z);
            u32 sw = prm_get_swSave();
            u32 size = prm_get_size();
            fopAcM_create(fpcNm_Obj_MsdanSub_e, sw + sub + (size << 16), &pos, current.roomNo, &angle, nullptr, -1, 0);
            sub += 0x100;
            if (--count == 0)
                break;
            angY = current.angle.y;
        }
        u8 evId = (u8)prm_get_evId();
        if (evId == 0xFF) {
            mEventIdx = dComIfGp_evmng_getEventIdx("Msdan", 0xFF);
        } else {
            mEventIdx = dComIfGp_evmng_getEventIdx(nullptr, evId);
        }
        mMode = fopAcM_isSwitch(this, prm_get_swSave()) ? MODE_END_e : MODE_WAIT_e;
    }
    return phase_state;
}

/* 0237B0C8 */
BOOL Act_c::Mthd_Execute() {
    switch ((u32)mMode) {
    case MODE_WAIT_e:
        if (fopAcM_isSwitch(this, prm_get_swSave())) {
            if (prm_get_size() != 0 || (u8)prm_get_sound() != 0) {
                mMode = MODE_END_e;
            } else if (mEventIdx == -1) {
                mDoAud_seStart(JA_SE_READ_RIDDLE_1);
                mMode = MODE_END_e;
            } else {
                fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
                mMode = MODE_ORDER_e;
            }
        }
        break;
    case MODE_ORDER_e:
        if (eventInfo.checkCommandDemoAccrpt()) {
            mMode = MODE_EVENT_e;
            mDoAud_seStart(JA_SE_READ_RIDDLE_1);
        } else {
            fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
        }
        break;
    case MODE_EVENT_e:
        if (dComIfGp_evmng_endCheck(mEventIdx)) {
            dComIfGp_event_reset();
            mMode = MODE_END_e;
        }
        break;
    }
    return TRUE;
}

/* 0237B098 */
BOOL Act_c::Mthd_Delete() {
    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return TRUE;
}

/* method table entries (HD: tail branches) */
/* 0237B25C */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 0237B260 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 0237B264 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Execute();
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0237B268 */
static void __sinit_d_a_obj_msdan_cpp() {
    __sinit_header_statics();
}

/* 0237B2FC */
// HD: compiler-generated deleting destructor
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0237B350 */
static BOOL Mthd_Draw(void* i_this) {
    return TRUE;
}
/* 0237B358 */
static BOOL Mthd_IsDelete(void* i_this) {
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data archiveName 1002D820
str 1002D80C
ext 025E1988 void mDoAud_seStart(u32 id)
*/
