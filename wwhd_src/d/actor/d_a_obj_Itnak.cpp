/**
 * d_a_obj_Itnak.cpp (WWHD)
 * Object - Unused - Darknut statue
 *
 * The GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_Itnak.cpp) has only "Nonmatching" stubs and no layout: every
 * function here is written from the WWHD code and verified against cking.rpx.
 */
#include "d/actor/d_a_obj_Itnak.h"
// HD: discard the unrelated legacy tsubo macro so the GameCube Itnak name is available.
#undef M_cyl_src

const gc_tu_data gc_tu = {
    .safeString = 0x1002B868,
    .headerStatics = {0x1046A300, 0x101CA530},
};
const char Act_c::M_arcname[] = "Itnak";
// HD: preserve the cylinder source image record (shared source type remains opaque).
static dCcD_SrcCyl M_cyl_src;

u32 Act_c::param_get_arg0() { return PrmAbstract(this, 8, 0); }
bool Act_c::is_switch() { return fopAcM_isSwitch(this, PrmAbstract(this, 8, 8)); }

/* 02365414 */
// HD: solidHeapCB
static u8 solidHeapCB(fopAc_ac_c* i_this) {
    return ((Act_c*)i_this)->create_heap();
}

static inline void init_cyl(Act_c* self, dCcD_Stts* stts, dCcD_Cyl* cyl) {
    stts->Init(0xFF, 0xFF, self);
    cyl->Set(&M_cyl_src);
    cyl->SetStts(stts);
    // HD: copy the three zero-vector words without floating point conversion.
    cyl->mGObjTg.mVec.copy(cXyz::Zero);
    cyl->mGObjTg.mSPrm |= 4;
}

/* 02365340 */
u8 Act_c::create_heap() {
    J3DModelData* mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_ITNAK_BDL_e);
    if (mdl_data == nullptr) {
        /* JUT_ASSERT(0x141, mdl_data != 0) */
        JUT_ASSERT(0x141, mdl_data != 0);
    } else {
        mpModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11000002);
    }
    set_mtx();
    return mdl_data != nullptr && mpModel != nullptr;
}

/* 02365418 */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    mArg0 = param_get_arg0();
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, (BOOL (*)(fopAc_ac_c*))solidHeapCB, 0)) {
            phase_state = cPhs_ERROR_e;
        } else {
            Mtx34* mtx = mpModel->getBaseTRMtx();
            mDrawFlag = mArg0;
            fopAcM_SetMtx(this, mtx);
            fopAcM_setCullSizeBox(this, -120.0f, 0.0f, -100.0f, 120.0f, 280.0f, 150.0f);
            init_cyl(this, &mStts0, &mCyl0);
            init_cyl(this, &mStts1, &mCyl1);
            init_cyl(this, &mStts2, &mCyl2);
        }
    }
    return phase_state;
}

/* 023656F8 */
bool Act_c::_delete() {
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}

/* 0236524C */
void Act_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &mMtx);
    J3DModel_calc(mpModel);
}

/* 023657FC */
bool Act_c::set_co_se(dCcD_Cyl* cyl) {
    if (cyl->ChkTgHit()) {
        daObj_HitSeStart(&current.pos, current.roomNo, cyl, 0xD);
        cXyz pos;
        f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        dKy_Sound_set(&pos, 4, mBsPcId, 100);
        daObj_HitEff_hibana(this, cyl);
        ClrTgHit_v(cyl);
        return true;
    }
    return false;
}

/* 02365728 */
void Act_c::manage_draw_flag() {
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

/* 023658C4 */
void Act_c::set_collision() {
    if (mDrawFlag == 1) {
        if (!set_co_se(&mCyl0)) {
            mCyl0.SetR(68.0f);
            mCyl0.SetH(230.0f);
            mCyl0.SetC(&current.pos);
            dComIfG_Ccsp_Set(&mCyl0);
        }
        if (!set_co_se(&mCyl1)) {
            cXyz offset;
            cXyz pos;
            offset.x = 41.0f;
            offset.y = 44.0f;
            offset.z = 84.0f;
            PSMTXMultVec(&mMtx, &offset, &pos);
            mCyl1.SetR(62.0f);
            mCyl1.SetH(121.0f);
            mCyl1.SetC(&pos);
            dComIfG_Ccsp_Set(&mCyl1);
        }
        if (!set_co_se(&mCyl2)) {
            cXyz offset;
            cXyz pos;
            offset.x = -88.0f;
            offset.y = 83.0f;
            offset.z = 86.0f;
            PSMTXMultVec(&mMtx, &offset, &pos);
            mCyl2.SetR(47.0f);
            mCyl2.SetH(205.0f);
            mCyl2.SetC(&pos);
            dComIfG_Ccsp_Set(&mCyl2);
        }
        fopAcM_rollPlayerCrash(this, 68.0f, 0xD);
    }
}

/* 02365A90 */
bool Act_c::_execute() {
    set_mtx();
    manage_draw_flag();
    set_collision();
    return true;
}

/* 02365AD0 */
bool Act_c::_draw() {
    if (mDrawFlag != 0) {
        g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
        g_env_light.setLightTevColorType(mpModel, &tevStr);
        mDoExt_modelUpdateDL(mpModel);
    }
    return true;
}

/* method table (HD: tail branches) */
/* 02365B38 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}
/* 02365B3C */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->_delete();
}
/* 02365B40 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}
/* 02365B44 */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->_draw();
}
/* 02365C90 */
static BOOL Mthd_IsDelete(void*) {
    return TRUE;
}

/* 02365B48 */
static void __sinit_d_a_obj_Itnak_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 02365BDC */
// HD: sead::SafeString deleting destructor (this TU's copy)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 02365BF0 */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
static void SafeString_assureTerminationImpl(void*) {
}

/* 02365BF4 */
// HD: daObjItnak::Act_c deleting destructor (compiler-generated)
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl2, 2);
        dCcD_Stts_dt(&i_this->mStts2, 2);
        dCcD_Cyl_dt(&i_this->mCyl1, 2);
        dCcD_Stts_dt(&i_this->mStts1, 2);
        dCcD_Cyl_dt(&i_this->mCyl0, 2);
        dCcD_Stts_dt(&i_this->mStts0, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
str 1002B958
str 1002B8E4
str 1002B8F8 "mdl_data != 0"
data M_cyl_src 1002B8A0
ext 02349DFC u32 PrmAbstract(fopAc_ac_c*, s32, s32)
ext 023129C4 void daObj_HitSeStart(const cXyz*, s32, dCcD_GObjInf*, u32)
ext 02312E54 void daObj_HitEff_hibana(fopAc_ac_c*, dCcD_Cyl*)
ext 0255F458 void dKy_Sound_set(cXyz*, s32, u32, s32)
ext 025D69FC BOOL fopAcM_rollPlayerCrash(fopAc_ac_c*, f32, u32)
*/
