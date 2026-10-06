/**
 * d_a_obj_wood.cpp (WWHD)
 * Object - Wood (registers a tree unit with the dWood packet and deletes itself).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_wood.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define WOOD_VTBL 0x10033898     /* daObjWood::Act_c vtable (HD virtual destructor) */
#define WOOD_AAB_VTBL 0x10033888 /* this TU's cM3dGAab vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
WWHD_OPAQUE(dWood_Packet_c);
/* 02524EC0 dComIfG_play_c::createWood() (HD: `this` is play+0x12A0) -> dWood::Packet_c* */
static inline void* dComIfGp_createWood() { return gabi::call<void*>(0x02524EC0, dComIfGp_ea() + 0x12A0); }
/* dComIfGp_getWood(): play+0x5AC0 (GameCube mpWood) */
static inline dWood_Packet_c* dComIfGp_getWood() { return gabi::at<dWood_Packet_c>(gabi::load<u32>(dComIfGp_ea() + 0x5AC0)); }
/* 025D077C dWood::Packet_c::put_unit(const cXyz&, int roomNo) */
static inline s32 dWood_Packet_put_unit(dWood_Packet_c* p, cXyz* pos, s32 roomNo) { return gabi::call<s32>(0x025D077C, p, pos, roomNo); }

namespace daObjWood {
struct Act_c : fopAc_ac_c {
    /* 0x3AC */ be<u32> field_0x290[3];
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
};
}
using daObjWood::Act_c;
WWHD_OFFSET(Act_c, mStts, 0x3B8);
WWHD_OFFSET(Act_c, mCyl, 0x3F4);
WWHD_SIZE(Act_c, 0x524);

/* 023BBCE4: daObjWood::Method::Create (_create inlined) */
static cPhs_State daObjWood_Method_Create(void* i_ptr) {
    WWHD_FUNC(0x023BBCE4, cPhs_State, i_ptr);
    Act_c* i_this = (Act_c*)i_ptr;
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = WOOD_VTBL;
            dCcD_Stts_ct(&i_this->mStts);
            dCcD_Cyl_ct(&i_this->mCyl, WOOD_AAB_VTBL);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    if (dComIfGp_createWood() != nullptr)
        dWood_Packet_put_unit(dComIfGp_getWood(), &i_this->current.pos, fopAcM_GetRoomNo(i_this));
    return cPhs_ERROR_e;
}
VERIFY(0x023BBCE4, daObjWood_Method_Create);

/* 023BBDE4: daObjWood::Method::Delete */
static BOOL daObjWood_Method_Delete(void* i_this) {
    WWHD_FUNC(0x023BBDE4, BOOL, i_this);
    return true;
}
VERIFY(0x023BBDE4, daObjWood_Method_Delete);

/* 023BBDEC: daObjWood::Method::Execute */
static BOOL daObjWood_Method_Execute(void* i_ptr) {
    WWHD_FUNC(0x023BBDEC, BOOL, i_ptr);
    Act_c* i_this = (Act_c*)i_ptr;
    i_this->mCyl.SetC(&i_this->current.pos);
    return true;
}
VERIFY(0x023BBDEC, daObjWood_Method_Execute);

/* 023BBE18: daObjWood::Method::Draw */
static BOOL daObjWood_Method_Draw(void* i_this) {
    WWHD_FUNC(0x023BBE18, BOOL, i_this);
    return true;
}
VERIFY(0x023BBE18, daObjWood_Method_Draw);

/* 023BBF20: daObjWood::Method::IsDelete */
static BOOL daObjWood_Method_IsDelete(void* i_this) {
    WWHD_FUNC(0x023BBF20, BOOL, i_this);
    return TRUE;
}
VERIFY(0x023BBF20, daObjWood_Method_IsDelete);

/* 023BBE20: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_obj_wood_cpp() {
    WWHD_FUNC(0x023BBE20, void, (u32)0);
    sinit_header_statics(0x1046CA18, 0x101CDE3C);
}
VERIFY(0x023BBE20, __sinit_d_a_obj_wood_cpp);

/* 023BBEB4: daObjWood::Act_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daObjWood_Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x023BBEB4, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x023BBEB4, daObjWood_Act_c_dt);
