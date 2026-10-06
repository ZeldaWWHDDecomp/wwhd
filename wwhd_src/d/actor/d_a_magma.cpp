/**
 * d_a_magma.cpp (WWHD)
 * Magma floor: registers a magma floor with the play object's dMagma_packet_c and deletes itself.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_magma.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define MAGMA_VTBL 0x1001437C /* daMagma_c vtable (HD virtual destructor) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02524C10 dComIfG_play_c::createMagma() (this = dComIfGp_get() + 0x12A0) */
static inline BOOL dComIfGp_createMagma() { return gabi::call<BOOL>(0x02524C10, dComIfGp_ea() + 0x12A0); }
/* dComIfGp_getMagma(): dMagma_packet_c* at play+0x5AB4 */
static inline u32 dComIfGp_getMagma() { return gabi::load<u32>(dComIfGp_ea() + 0x5AB4); }
/* 0258CC64 dMagma_packet_c::newFloor(cXyz& pos, cXyz& scale, s32 roomNo, s16 pathNo) */
static inline void dMagma_packet_newFloor(u32 magma, cXyz* pos, cXyz* scale, s32 roomNo, s16 pathNo) {
    gabi::call(0x0258CC64, magma, pos, scale, roomNo, pathNo);
}

struct daMagma_c : fopAc_ac_c {
    s16 getPathNo() { return (s16)fopAcM_GetParam(this); }

    /* 0x3AC */ request_of_phase_process_class mPhs;
};
WWHD_SIZE(daMagma_c, 0x3B4);

/* 021B834C */
static BOOL daMagma_IsDelete(daMagma_c* i_this) {
    WWHD_FUNC(0x021B834C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B834C, daMagma_IsDelete);

/* 021B8354: HD: the destructor is not called here (it is virtual, see 021B84B0) */
static BOOL daMagma_Delete(daMagma_c* i_this) {
    WWHD_FUNC(0x021B8354, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B8354, daMagma_Delete);

/* 021B835C: daMagma_c::create() inlined */
static cPhs_State daMagma_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x021B835C, cPhs_State, i_ac);
    daMagma_c* i_this = (daMagma_c*)i_ac;
    /* fopAcM_ct_Retail(this, daMagma_c) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr) {
            fopAc_ac_c_ct(i_ac);
            i_ac->__vtbl = MAGMA_VTBL;
        }
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }

    cPhs_State result = dComIfG_resLoad(&i_this->mPhs, STR(0x1001436C) /* "Magma" */);
    if (result != cPhs_COMPLEATE_e) {
        return result;
    }

    if (dComIfGp_createMagma()) {
        u32 magma = dComIfGp_getMagma();
        dMagma_packet_newFloor(magma, &i_this->current.pos, &i_this->scale, fopAcM_GetRoomNo(i_this), i_this->getPathNo());
    }

    return cPhs_ERROR_e;
}
VERIFY(0x021B835C, daMagma_Create);

/* 021B841C: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_magma_cpp() {
    WWHD_FUNC(0x021B841C, void, (u32)0);
    sinit_header_statics(0x10464EA4, 0x101B92BC);
}
VERIFY(0x021B841C, __sinit_d_a_magma_cpp);

/* 021B84B0: daMagma_c deleting destructor (~daMagma_c: dComIfG_resDeleteDemo(&mPhs, "Magma"),
 * HD: resDelete, virtual). The matcher names it daMagma_Delete. */
static void daMagma_c_dt(daMagma_c* i_this, s32 flags) {
    WWHD_FUNC(0x021B84B0, void, i_this, flags);
    if (i_this != nullptr) {
        i_this->__vtbl = MAGMA_VTBL;
        dComIfG_resDelete(&i_this->mPhs, STR(0x10014374) /* "Magma" */);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021B84B0, daMagma_c_dt);
