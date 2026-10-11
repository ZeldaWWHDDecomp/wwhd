/**
 * d_a_tag_ghostship.cpp (WWHD)
 * Tag - Ghost Ship clear event
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_ghostship.cpp) to the WWHD layout and verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tag_ghostship.h"

const gc_tu_data gc_tu = {
    .headerStatics = {0x1046E2B8, 0x101D19E0, 0x1046E2D4},
};

static daTag_Gship_HIO_c l_HIO;

/* 024A8A70 */
// HD: GHS constructor allocates when this == NULL.
static daTag_Gship_HIO_c* daTag_Gship_HIO_ct(daTag_Gship_HIO_c* p) {
    if (p == nullptr) {
        p = (daTag_Gship_HIO_c*)operator_new(sizeof(daTag_Gship_HIO_c));
        if (p == nullptr)
            return p;
    }
    p->__vtbl = daTag_Gship_HIO_c::__vt;
    p->mNo = -1;
    p->field_0x05 = 0;
    return p;
}

/* 024A8B68 */
void daTag_Gship_c::modeClearWaitInit() {
    return;
}

/* 024A878C */
void daTag_Gship_c::modeClearWait() {
    if (dComIfGp_evmng_endCheck("DEFAULT_TREASURE") ||
        dComIfGp_evmng_endCheck("DEFAULT_TREASURE2") ||
        dComIfGp_evmng_endCheck("DEFAULT_TREASURE_A") || l_HIO.field_0x05) {
        modeProc(PROC_INIT_e, 1);
    }
}

/* 024A8828 */
void daTag_Gship_c::modeClearEventInit() {
    u8 reg = dComIfGs_getEventReg(dSv_event_flag_c::GHOST_SHIP);
    (void)reg;
    reg = 3;
    dComIfGs_setEventReg(dSv_event_flag_c::GHOST_SHIP, reg);
}

/* 024A8884 */
void daTag_Gship_c::modeClearEvent() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        int staffIdx = dComIfGp_evmng_getMyStaffId("PScnChg", nullptr, 0);
        const char* cut_name = dComIfGp_evmng_getMyNowCutName(staffIdx);
        if (cut_name == 0) {
            /* HD: JUT_ASSERT(0x69, cut_name != 0) (getMyNowCutName inline) */
            JUTAssertion::showAssert("d_a_tag_ghostship.cpp", 0x69, "cut_name != (0)");
        } else if (ghostship::strcmp(cut_name, "WARAIGOE") == 0) {
            if (dComIfGs_getEventReg(dSv_event_flag_c::GHOST_SHIP) == 3) {
                mDoAud_seStart(JA_SE_CV_G_SHIP_SCREAM, nullptr, 0, 0);
            } else {
                mDoAud_seStart(JA_SE_CV_G_SHIP_LAUGH, nullptr, 0, 0);
            }
            dComIfGp_evmng_cutEnd(staffIdx);
        }
        if (dComIfGp_evmng_endCheck("PSHIP_CLEAR")) {
            mDoAud_seStart(JA_SE_LK_WARP_TO_G_SHIP, nullptr, 0, 0);
            // HD: the saved room byte is sign-extended for the stage transition.
            s8 room = dComIfGs_getEventReg(dSv_event_flag_c::UNK_C3FF);
            s8 spawn = dComIfGs_getEventReg(dSv_event_flag_c::UNK_85FF);
            dKy_set_nexttime(120.0f);
            dComIfGp_setNextStage("sea", spawn, room, -1, 0.0f, 5, 1, 0);
        }
    } else {
        fopAcM_orderOtherEvent(this, "PSHIP_CLEAR", 1, 0xFFFF);
    }
}

/* 024A86A8 */
void daTag_Gship_c::modeProc(Proc_e proc, int param_2) {
    static ProcEntry mode_proc[] = {
        {&daTag_Gship_c::modeClearWaitInit, &daTag_Gship_c::modeClearWait, "CLEAR_WAIT"},
        {&daTag_Gship_c::modeClearEventInit, &daTag_Gship_c::modeClearEvent, "CLEAR_EVENT"},
    };
    if (proc == PROC_INIT_e) {
        mMode = param_2;
        (this->*mode_proc[param_2].init)();
    } else if (proc == PROC_EXEC_e) {
        (this->*mode_proc[mMode].run)();
    }
}

/* 024A8754 */
bool daTag_Gship_c::_execute() {
    modeProc(PROC_EXEC_e, 2);
    return true;
}

/* 024A85D8 */
void daTag_Gship_c::getArg() {
    u32 param = fopAcM_GetParam(this);
    field_0x294 = (s8)param;
    s32 bit = (param >> 8) & 0xFF;
    if (bit == 0xFF) {
        field_0x298 = 1000.0f;
    } else {
        field_0x298 = (f32)(bit * 100);
    }
}

/* 024A8634 */
cPhs_State daTag_Gship_c::_create() {
    fopAcM_ct(this, daTag_Gship_c);
    getArg();
    return cPhs_COMPLEATE_e;
}

/* 024A869C */
static cPhs_State daTag_GshipCreate(void* i_this) {
    return static_cast<daTag_Gship_c*>(i_this)->_create();
}

/* 024A86A0 */
// HD: _delete inlined
static BOOL daTag_GshipDelete(void* i_this) {
    return true;
}

/* 024A8780 */
static BOOL daTag_GshipExecute(void* i_this) {
    return static_cast<daTag_Gship_c*>(i_this)->_execute();
}

/* 024A8784 */
// HD: _draw inlined
static BOOL daTag_GshipDraw(void* i_this) {
    return true;
}

/* 024A8B60 */
static BOOL daTag_GshipIsDelete(void* i_this) {
    return true;
}

/* 024A8AC0 */
static void __sinit_d_a_tag_ghostship_cpp() {
    __sinit_header_statics();
    daTag_Gship_HIO_ct(&l_HIO);
}

/* 024A8B6C */
// HD: daTag_Gship_c deleting destructor
static void daTag_Gship_c_dt(daTag_Gship_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and existing local bindings
str 1003F160
str 1003F14C
str 1003F174
str 1003F190
str 1003F1B8
str 1003F19C
str 1003F1AC
str 1003F198
str 1003F1D0
data l_HIO 1046E2C4
data mode_proc 1003F10C
ext 0254457C BOOL dEvent_manager_endCheckOld(dEvent_manager_c*, const char*)
ext 02544830 const char* dEvent_manager_getMyNowCutName(dEvent_manager_c*, s32)
ext 02560728 void dKy_set_nexttime(f32)
ext 0252012C void dComIfGp_setNextStage(const char*, s16, s8, s8, f32, u32, s32, s8)
ext 025D77DC void fopAcM_orderOtherEvent(fopAc_ac_c*, const char*, u16, u16)
*/
