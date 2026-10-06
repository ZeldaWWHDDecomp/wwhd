/**
 * d_a_tag_ghostship.cpp (WWHD)
 * Tag - Ghost Ship clear event
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_ghostship.cpp) to the WWHD layout and verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define GSHIP_VTBL 0x1003F0EC     /* daTag_Gship_c vtable (HD virtual destructor) */
#define GSHIP_HIO_VTBL 0x1003F0FC /* daTag_Gship_HIO_c vtable */
#define MODE_PROC 0x1003F10C      /* mode_proc[2]: {ptmf init, ptmf run, name}, 0x14 bytes each */
#define L_HIO 0x1046E2C4          /* l_HIO */

enum {
    JA_SE_CV_G_SHIP_LAUGH = 0x4925,
    JA_SE_CV_G_SHIP_SCREAM = 0x4926,
    JA_SE_LK_WARP_TO_G_SHIP = 0x2889,
};
enum {
    EVREG_GHOST_SHIP = 0x8803,
    EVREG_UNK_85FF = 0x85FF,
    EVREG_UNK_C3FF = 0xC3FF,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dComIfGs_event(): save info + 0x644 (as in d_a_npc_people.h) */
static inline dSv_event_c* gs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline u8 dComIfGs_getEventReg(u16 reg) { return dSv_event_getEventReg(gs_event(), reg); }
static inline void dComIfGs_setEventReg(u16 reg, u8 v) { dSv_event_setEventReg(gs_event(), reg, v); }
/* 0254457C dEvent_manager_c::endCheckOld(const char*) (HD: dComIfGp_evmng_endCheck(name)) */
static inline BOOL dComIfGp_evmng_endCheckOld(const char* name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
/* 02544830 dEvent_manager_c::getMyNowCutName(staffIdx) */
static inline u32 dComIfGp_evmng_getMyNowCutName(s32 staffIdx) { return gabi::call<u32>(0x02544830, dComIfGp_getPEvtManager(), staffIdx); }
/* 02560728 dKy_set_nexttime(f32) */
static inline void dKy_set_nexttime(f32 t) { gabi::call(0x02560728, t); }
/* 0252012C dComIfGp_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe) (as in d_a_npc_kf1.h) */
static inline void dComIfGp_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 speed, u32 mode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, speed, mode, setPoint, wipe);
}
/* 025D77DC fopAcM_orderOtherEvent2(actor, name, flag, hind) (HD: fopAcM_orderOtherEvent inline) */
static inline void fopAcM_orderOtherEvent2(fopAc_ac_c* a, const char* name, u16 flag, u16 hind) {
    gabi::call(0x025D77DC, a, name, flag, hind);
}
/* inline strcmp (byte loop, as in d_a_npc_tc_cut.cpp) */
static inline int inl_strcmp(u32 a, u32 b) {
    u8 ca, cb;
    do {
        ca = gabi::load<u8>(a++);
        cb = gabi::load<u8>(b++);
    } while (ca == cb && ca != 0);
    return (int)ca - (int)cb;
}

/* HD layout: vtable at +0xC */
struct daTag_Gship_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> field_0x05;
    /* 0x02 */ u8 _02[0xC - 0x2];
    /* 0x0C */ be<u32> __vtbl;
};
WWHD_SIZE(daTag_Gship_HIO_c, 0x10);
#define l_HIO (*gabi::at<daTag_Gship_HIO_c>(L_HIO))

struct daTag_Gship_c : fopAc_ac_c {
    enum Proc_e {
        PROC_INIT_e = 0,
        PROC_EXEC_e = 1,
    };
    void modeClearWaitInit();
    void modeClearWait();
    void modeClearEventInit();
    void modeClearEvent();
    void modeProc(Proc_e, int);
    bool _execute();
    void getArg();
    cPhs_State _create();

    /* 0x3AC */ be<s32> mMode;
    /* 0x3B0 */ be<s8> field_0x294;
    /* 0x3B1 */ u8 _3B1[3];
    /* 0x3B4 */ be<f32> field_0x298;
};
WWHD_OFFSET(daTag_Gship_c, mMode, 0x3AC);
WWHD_OFFSET(daTag_Gship_c, field_0x298, 0x3B4);

/* 024A8A70: daTag_Gship_HIO_c::daTag_Gship_HIO_c (HD: allocates when this == NULL) */
static daTag_Gship_HIO_c* daTag_Gship_HIO_ct(daTag_Gship_HIO_c* p) {
    WWHD_FUNC(0x024A8A70, daTag_Gship_HIO_c*, p);
    if (p == nullptr) {
        p = gabi::call<daTag_Gship_HIO_c*>(0x0273AD10, 0x10); /* operator new */
        if (p == nullptr)
            return p;
    }
    p->__vtbl = GSHIP_HIO_VTBL;
    p->mNo = -1;
    p->field_0x05 = 0;
    return p;
}
VERIFY(0x024A8A70, daTag_Gship_HIO_ct);

/* 024A8B68 */
void daTag_Gship_c::modeClearWaitInit() {
    WWHD_FUNC(0x024A8B68, void, this);
    return;
}
VERIFY(0x024A8B68, &daTag_Gship_c::modeClearWaitInit);

/* 024A878C */
void daTag_Gship_c::modeClearWait() {
    WWHD_FUNC(0x024A878C, void, this);
    if (dComIfGp_evmng_endCheckOld(STR(0x1003F160) /* "DEFAULT_TREASURE" */) ||
        dComIfGp_evmng_endCheckOld(STR(0x1003F14C) /* "DEFAULT_TREASURE2" */) ||
        dComIfGp_evmng_endCheckOld(STR(0x1003F174) /* "DEFAULT_TREASURE_A" */) || l_HIO.field_0x05) {
        modeProc(PROC_INIT_e, 1);
    }
}
VERIFY(0x024A878C, &daTag_Gship_c::modeClearWait);

/* 024A8828 */
void daTag_Gship_c::modeClearEventInit() {
    WWHD_FUNC(0x024A8828, void, this);
    u8 reg = dComIfGs_getEventReg(EVREG_GHOST_SHIP);
    (void)reg;
    reg = 3;
    dComIfGs_setEventReg(EVREG_GHOST_SHIP, reg);
}
VERIFY(0x024A8828, &daTag_Gship_c::modeClearEventInit);

/* 024A8884 */
void daTag_Gship_c::modeClearEvent() {
    WWHD_FUNC(0x024A8884, void, this);
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2) { /* eventInfo.checkCommandDemoAccrpt() */
        int staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1003F190) /* "PScnChg" */, nullptr, 0);
        u32 cut_name = dComIfGp_evmng_getMyNowCutName(staffIdx);
        if (cut_name == 0) {
            /* HD: JUT_ASSERT(0x69, cut_name != 0) (getMyNowCutName inline) */
            JUT_ASSERT_fail(STR(0x1003F1B8), 0x69, STR(0x1003F19C));
        } else if (inl_strcmp(cut_name, 0x1003F1D0 /* "WARAIGOE" */) == 0) {
            if (dComIfGs_getEventReg(EVREG_GHOST_SHIP) == 3) {
                mDoAud_seStart(JA_SE_CV_G_SHIP_SCREAM, nullptr, 0, 0);
            } else {
                mDoAud_seStart(JA_SE_CV_G_SHIP_LAUGH, nullptr, 0, 0);
            }
            dComIfGp_evmng_cutEnd(staffIdx);
        }
        if (dComIfGp_evmng_endCheckOld(STR(0x1003F1AC) /* "PSHIP_CLEAR" */)) {
            mDoAud_seStart(JA_SE_LK_WARP_TO_G_SHIP, nullptr, 0, 0);
            s8 room = dComIfGs_getEventReg(EVREG_UNK_C3FF);
            s8 spawn = dComIfGs_getEventReg(EVREG_UNK_85FF);
            dKy_set_nexttime(120.0f);
            dComIfGp_setNextStage(STR(0x1003F198) /* "sea" */, spawn, room, -1, 0.0f, 5, 1, 0);
        }
    } else {
        fopAcM_orderOtherEvent2(this, STR(0x1003F1AC) /* "PSHIP_CLEAR" */, 1, 0xFFFF);
    }
}
VERIFY(0x024A8884, &daTag_Gship_c::modeClearEvent);

/* 024A86A8 */
void daTag_Gship_c::modeProc(Proc_e proc, int param_2) {
    WWHD_FUNC(0x024A86A8, void, this, proc, param_2);
    if (proc == PROC_INIT_e) {
        mMode = param_2;
        ptmf_call(MODE_PROC + param_2 * 0x14, this); /* (this->*mode_proc[mMode].init)() */
    } else if (proc == PROC_EXEC_e) {
        ptmf_call(MODE_PROC + mMode * 0x14 + 8, this); /* (this->*mode_proc[mMode].run)() */
    }
}
VERIFY(0x024A86A8, &daTag_Gship_c::modeProc);

/* 024A8754 */
bool daTag_Gship_c::_execute() {
    WWHD_FUNC(0x024A8754, bool, this);
    modeProc(PROC_EXEC_e, 2);
    return true;
}
VERIFY(0x024A8754, &daTag_Gship_c::_execute);

/* 024A85D8 */
void daTag_Gship_c::getArg() {
    WWHD_FUNC(0x024A85D8, void, this);
    u32 param = fopAcM_GetParam(this);
    field_0x294 = (s8)param;
    s32 bit = (param >> 8) & 0xFF;
    if (bit == 0xFF) {
        field_0x298 = 1000.0f;
    } else {
        field_0x298 = (f32)(bit * 100);
    }
}
VERIFY(0x024A85D8, &daTag_Gship_c::getArg);

/* 024A8634 */
cPhs_State daTag_Gship_c::_create() {
    WWHD_FUNC(0x024A8634, cPhs_State, this);
    /* fopAcM_ct(this, daTag_Gship_c); HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = GSHIP_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    getArg();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024A8634, &daTag_Gship_c::_create);

/* 024A869C */
static cPhs_State daTag_GshipCreate(void* i_this) {
    WWHD_FUNC(0x024A869C, cPhs_State, i_this);
    return static_cast<daTag_Gship_c*>(i_this)->_create();
}
VERIFY(0x024A869C, daTag_GshipCreate);

/* 024A86A0: _delete inlined */
static BOOL daTag_GshipDelete(void* i_this) {
    WWHD_FUNC(0x024A86A0, BOOL, i_this);
    return true;
}
VERIFY(0x024A86A0, daTag_GshipDelete);

/* 024A8780 */
static BOOL daTag_GshipExecute(void* i_this) {
    WWHD_FUNC(0x024A8780, BOOL, i_this);
    return static_cast<daTag_Gship_c*>(i_this)->_execute();
}
VERIFY(0x024A8780, daTag_GshipExecute);

/* 024A8784: _draw inlined */
static BOOL daTag_GshipDraw(void* i_this) {
    WWHD_FUNC(0x024A8784, BOOL, i_this);
    return true;
}
VERIFY(0x024A8784, daTag_GshipDraw);

/* 024A8B60 */
static BOOL daTag_GshipIsDelete(void* i_this) {
    WWHD_FUNC(0x024A8B60, BOOL, i_this);
    return true;
}
VERIFY(0x024A8B60, daTag_GshipIsDelete);

/* 024A8AC0 */
static void __sinit_d_a_tag_ghostship_cpp() {
    WWHD_FUNC(0x024A8AC0, void, (u32)0);
    sinit_header_statics_z(0x1046E2B8, 0x101D19E0, 0x1046E2D4);
    daTag_Gship_HIO_ct(gabi::at<daTag_Gship_HIO_c>(L_HIO)); /* l_HIO */
}
VERIFY(0x024A8AC0, __sinit_d_a_tag_ghostship_cpp);

/* 024A8B6C: daTag_Gship_c deleting destructor */
static void daTag_Gship_c_dt(daTag_Gship_c* i_this, s32 flags) {
    WWHD_FUNC(0x024A8B6C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A8B6C, daTag_Gship_c_dt);
