/**
 * d_a_obj_Itnak.cpp (WWHD)
 * Object - Unused - Darknut statue
 *
 * The GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_Itnak.cpp) has only "Nonmatching" stubs and no layout: every
 * function here is written from the WWHD code and verified against cking.rpx.
 */
#include "bindings.h"

#define M_arcname STR(0x1002B958)     /* "Itnak" */
#define SAFESTRING_VTBL 0x1002B868    /* this TU's sead::SafeString vtable */
#define ITNAK_VTBL 0x1002B890         /* HD: daObjItnak::Act_c vtable */
#define AAB_VTBL 0x1002B880           /* this TU's cM3dGAab vtable */
#define M_cyl_src gabi::at<dCcD_SrcCyl>(0x1002B8A0)

enum { dRes_INDEX_ITNAK_BDL_e = 3 };

namespace daObjItnak {
struct Act_c : fopAc_ac_c {
    u32 param_get_arg0();
    bool is_switch();
    u8 create_heap(); /* bool, returned as u8 (clrlwi): a tail call passes the byte through */
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    bool set_co_se(dCcD_Cyl* cyl);
    void manage_draw_flag();
    void set_collision();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ Mtx34 mMtx;
    /* 0x3E8 */ dCcD_Stts mStts0;
    /* 0x424 */ dCcD_Cyl mCyl0;
    /* 0x554 */ dCcD_Stts mStts1;
    /* 0x590 */ dCcD_Cyl mCyl1;
    /* 0x6C0 */ dCcD_Stts mStts2;
    /* 0x6FC */ dCcD_Cyl mCyl2;
    /* 0x82C */ be<s32> mArg0;
    /* 0x830 */ be<s32> mDrawFlag;
};
WWHD_OFFSET(Act_c, mStts0, 0x3E8);
WWHD_OFFSET(Act_c, mCyl2, 0x6FC);
WWHD_OFFSET(Act_c, mDrawFlag, 0x830);
WWHD_SIZE(Act_c, 0x834);
}  // namespace daObjItnak
using daObjItnak::Act_c;

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02349DFC daObj::PrmAbstract<int>(actor, width, shift): the copy Itnak calls (outside this TU) */
static inline u32 PrmAbstract_02349DFC(fopAc_ac_c* a, s32 width, s32 shift) { return gabi::call<u32>(0x02349DFC, a, width, shift); }
/* 023129C4 daObj::HitSeStart(const cXyz*, int roomNo, const dCcD_GObjInf*, u32 se) */
static inline void daObj_HitSeStart(const cXyz* pos, s32 room, dCcD_GObjInf* o, u32 se) { gabi::call(0x023129C4, pos, room, o, se); }
/* 02312E54 daObj::HitEff_hibana(const fopAc_ac_c*, const dCcD_Cyl*) */
static inline void daObj_HitEff_hibana(fopAc_ac_c* a, dCcD_Cyl* c) { gabi::call(0x02312E54, a, c); }
/* 0255F458 dKy_Sound_set(cXyz pos (by value: pointer to a copy), int, fpc_ProcID, int) */
static inline void dKy_Sound_set(cXyz* pos, s32 a, u32 id, s32 b) { gabi::call(0x0255F458, pos, a, id, b); }
/* 025D69FC fopAcM_rollPlayerCrash(actor, f32 dist, u32 flag) */
static inline BOOL fopAcM_rollPlayerCrash(fopAc_ac_c* a, f32 d, u32 f) { return gabi::call<BOOL>(0x025D69FC, a, d, f); }
/* dCcD_GObjInf::ClrTgHit through the vtable (slot +0x3C) */
static inline void ClrTgHit_v(dCcD_GObjInf* o) { gabi::call_ptr(gabi::load<u32>(o->__vtbl_hitinf + 0x3C), o); }
u32 Act_c::param_get_arg0() { return PrmAbstract_02349DFC(this, 8, 0); }
bool Act_c::is_switch() { return fopAcM_isSwitch(this, PrmAbstract_02349DFC(this, 8, 8)); }

/* 0236524C */
void Act_c::set_mtx() {
    WWHD_FUNC(0x0236524C, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    J3DModel_calc(mpModel);
}
VERIFY(0x0236524C, &Act_c::set_mtx);

/* 02365340 */
u8 Act_c::create_heap() {
    WWHD_FUNC(0x02365340, u8, this);
    J3DModelData* mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_ITNAK_BDL_e, SAFESTRING_VTBL);
    if (mdl_data == nullptr) {
        /* JUT_ASSERT(0x141, mdl_data != 0) */
        JUT_ASSERT_fail(STR(0x1002B8E4), 0x141, STR(0x1002B8F8));
    } else {
        mpModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11000002);
    }
    set_mtx();
    return mdl_data != nullptr && mpModel != nullptr;
}
VERIFY(0x02365340, &Act_c::create_heap);

/* 02365414: solidHeapCB */
static u8 solidHeapCB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02365414, u8, i_this);
    return ((Act_c*)i_this)->create_heap();
}
VERIFY(0x02365414, solidHeapCB);

static inline void init_cyl(Act_c* self, dCcD_Stts* stts, dCcD_Cyl* cyl) {
    stts->Init(0xFF, 0xFF, self);
    cyl->Set(M_cyl_src);
    cyl->SetStts(stts);
    /* SetTgVec(cXyz::Zero) (integer copy) and OnTgSPrmBit-like flag 4 of dCcD_GObjTg */
    u32 z = 0x101FFBA8;
    u32 o = gabi::ea(cyl);
    gabi::store<u32>(o + 0xB4, gabi::load<u32>(z));
    gabi::store<u32>(o + 0xB8, gabi::load<u32>(z + 4));
    gabi::store<u32>(o + 0xBC, gabi::load<u32>(z + 8));
    cyl->mGObjTg.mSPrm |= 4;
}

/* 02365418 */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x02365418, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ITNAK_VTBL;
            dCcD_Stts_ct(&mStts0);
            dCcD_Cyl_ct(&mCyl0, AAB_VTBL);
            dCcD_Stts_ct(&mStts1);
            dCcD_Cyl_ct(&mCyl1, AAB_VTBL);
            dCcD_Stts_ct(&mStts2);
            dCcD_Cyl_ct(&mCyl2, AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    mArg0 = param_get_arg0();
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x02365414 /* solidHeapCB */, 0)) {
            phase_state = cPhs_ERROR_e;
        } else {
            u32 mtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
            mDrawFlag = mArg0;
            cullMtx = mtx; /* fopAcM_SetMtx */
            fopAcM_setCullSizeBox(this, -120.0f, 0.0f, -100.0f, 120.0f, 280.0f, 150.0f);
            init_cyl(this, &mStts0, &mCyl0);
            init_cyl(this, &mStts1, &mCyl1);
            init_cyl(this, &mStts2, &mCyl2);
        }
    }
    return phase_state;
}
VERIFY(0x02365418, &Act_c::_create);

/* 023656F8 */
bool Act_c::_delete() {
    WWHD_FUNC(0x023656F8, bool, this);
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}
VERIFY(0x023656F8, &Act_c::_delete);

/* 02365728 */
void Act_c::manage_draw_flag() {
    WWHD_FUNC(0x02365728, void, this);
    if (mArg0 == 1) {
        if (mDrawFlag == 1 && is_switch()) {
            mDrawFlag = 0;
        }
    } else if (mArg0 == 0) {
        if (mDrawFlag == 0 && is_switch()) {
            mDrawFlag = 1;
        }
    } else {
        mDrawFlag = 1;
    }
}
VERIFY(0x02365728, &Act_c::manage_draw_flag);

/* 023657FC */
bool Act_c::set_co_se(dCcD_Cyl* cyl) {
    WWHD_FUNC(0x023657FC, bool, this, cyl);
    if (cyl->ChkTgHit()) {
        daObj_HitSeStart(&current.pos, current.roomNo, cyl, 0xD);
        gabi::Local<cXyz> pos;
        f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
        pos->x = x;
        pos->y = y;
        pos->z = z;
        dKy_Sound_set(pos.get(), 4, gabi::load<u32>(gabi::ea(this) + 4) /* fopAcM_GetID */, 100);
        daObj_HitEff_hibana(this, cyl);
        ClrTgHit_v(cyl);
        return true;
    }
    return false;
}
VERIFY(0x023657FC, &Act_c::set_co_se);

/* 023658C4 */
void Act_c::set_collision() {
    WWHD_FUNC(0x023658C4, void, this);
    if (mDrawFlag == 1) {
        if (!set_co_se(&mCyl0)) {
            mCyl0.SetR(68.0f);
            mCyl0.SetH(230.0f);
            mCyl0.SetC(&current.pos);
            dComIfG_Ccsp_Set(&mCyl0);
        }
        if (!set_co_se(&mCyl1)) {
            gabi::Local<cXyz> offset;
            gabi::Local<cXyz> pos;
            offset->x = 41.0f;
            offset->y = 44.0f;
            offset->z = 84.0f;
            PSMTXMultVec(&mMtx, offset.get(), pos.get());
            mCyl1.SetR(62.0f);
            mCyl1.SetH(121.0f);
            mCyl1.SetC(pos.get());
            dComIfG_Ccsp_Set(&mCyl1);
        }
        if (!set_co_se(&mCyl2)) {
            gabi::Local<cXyz> offset;
            gabi::Local<cXyz> pos;
            offset->x = -88.0f;
            offset->y = 83.0f;
            offset->z = 86.0f;
            PSMTXMultVec(&mMtx, offset.get(), pos.get());
            mCyl2.SetR(47.0f);
            mCyl2.SetH(205.0f);
            mCyl2.SetC(pos.get());
            dComIfG_Ccsp_Set(&mCyl2);
        }
        fopAcM_rollPlayerCrash(this, 68.0f, 0xD);
    }
}
VERIFY(0x023658C4, &Act_c::set_collision);

/* 02365A90 */
bool Act_c::_execute() {
    WWHD_FUNC(0x02365A90, bool, this);
    set_mtx();
    manage_draw_flag();
    set_collision();
    return true;
}
VERIFY(0x02365A90, &Act_c::_execute);

/* 02365AD0 (not named by the matcher) */
bool Act_c::_draw() {
    WWHD_FUNC(0x02365AD0, bool, this);
    if (mDrawFlag != 0) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
        setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
        mDoExt_modelUpdateDL(mpModel);
    }
    return true;
}
VERIFY(0x02365AD0, &Act_c::_draw);

/* method table (HD: tail branches) */
/* 02365B38 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x02365B38, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x02365B38, Mthd_Create);
/* 02365B3C */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x02365B3C, bool, i_this);
    return ((Act_c*)i_this)->_delete();
}
VERIFY(0x02365B3C, Mthd_Delete);
/* 02365B40 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x02365B40, bool, i_this);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x02365B40, Mthd_Execute);
/* 02365B44 */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x02365B44, bool, i_this);
    return ((Act_c*)i_this)->_draw();
}
VERIFY(0x02365B44, Mthd_Draw);
/* 02365C90 */
static BOOL Mthd_IsDelete(void*) {
    WWHD_FUNC(0x02365C90, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02365C90, Mthd_IsDelete);

/* 02365B48 */
static void __sinit_d_a_obj_Itnak_cpp() {
    WWHD_FUNC(0x02365B48, void, (u32)0);
    sinit_header_statics(0x1046A300, 0x101CA530);
}
VERIFY(0x02365B48, __sinit_d_a_obj_Itnak_cpp);

/* 02365BDC: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02365BDC, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x02365BDC, SafeString_dt);

/* 02365BF0: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x02365BF0, void, (u32)0);
}
VERIFY(0x02365BF0, SafeString_assureTerminationImpl);

/* 02365BF4: daObjItnak::Act_c deleting destructor (compiler-generated) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x02365BF4, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl2, 2);
        dCcD_Stts_dt(&i_this->mStts2, 2);
        dCcD_Cyl_dt(&i_this->mCyl1, 2);
        dCcD_Stts_dt(&i_this->mStts1, 2);
        dCcD_Cyl_dt(&i_this->mCyl0, 2);
        dCcD_Stts_dt(&i_this->mStts0, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02365BF4, Act_c_dt);
