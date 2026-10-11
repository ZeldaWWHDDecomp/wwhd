/**
 * d_a_obj_Ygush00.cpp (WWHD)
 * Object - Spring water gush (Ygush00)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_Ygush00.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_Ygush00.h"

const gc_tu_data gc_tu = {.safeString=0x10033A2C};
static const char* l_arcname = "Ygush00";
enum { TEV_TYPE_BG1=2, JA_SE_OBJ_SPRING=0x61FE, fpcNm_Obj_Gryw00_e=0x8E };
namespace dSv_event_flag_c { enum { COLORS_IN_HYRULE=0x3802 }; }
static u32 mdl_table[] = { 10, 9, 9, 9 };
static u32 btk_table[] = { 14, 13, 13, 13 };
static u32 bck_table[] = { 6, 5, 5, 5 };
/* fopAcM_SearchByName(name): HD inline, fopAcIt_Judge(fpcSch_JudgeForPName 025E121C, &name) */
static inline fopAc_ac_c* fopAcM_SearchByName_l(s16 name) {
    gabi::Local<be<s16>> key;
    *key = name;
    return fopAcIt_Judge(0x025E121C, key.get());
}
/* fopAcM_seStartCurrent: HD inline (no null checks) */
static inline void fopAcM_seStartCurrent_l(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    mDoAud_seStart(id, &a->current.pos, param, reverb);
}

// HD: GHS expands bck construction inline and stores this TU's vtable.
daObjYgush00_c::daObjYgush00_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x10033A6C;
    mDoExt_btkAnm::ct(&mBtkAnm);
    YgushBckLayout* bck = reinterpret_cast<YgushBckLayout*>(&mBckAnm);
    YgushFrameCtrl_init(&bck->mFrameCtrl, 0);
    bck->mVtable = 0x1016E54C;
    YgushMtxCalc_construct(bck->mCalcBase);
    bck->mOne[0]=0;
    bck->mOne[1]=0;
    bck->mCalcAnm=0;
    bck->mMayaVtable=0x1016D820;
    bck->mVtable=0x10033A44;
    bck->mOne[3]=0;
    bck->mOne[2]=0;
}

// HD: daObj::PrmAbstract<daObjYgush00_c::Param_e>(actor, width, shift) (HD: out of line, per TU)
/* 023BDAC0 */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}

int daObjYgush00_c::param_get_arg() { return PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S); }

/* 023BD414 */
bool daObjYgush00_c::create_heap() {
    bool ret = true;
    J3DModelData* pModelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname, mdl_table[mType]);
    J3DAnmTextureSRTKey* pBtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(l_arcname, btk_table[mType]);
    J3DAnmTransform* pBck = (J3DAnmTransform*)dComIfG_getObjectRes(l_arcname, bck_table[mType]);

    if (!pModelData || !pBtk || !pBck) {
        JUT_ASSERT(207, FALSE);
        ret = false;
    } else {
        mpModel = mDoExt_J3DModel__create(pModelData, 0x80000, 0x11000222);
        s32 btkRet = mBtkAnm.init(pModelData, pBtk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);
        s32 bckRet = mBckAnm.init(pModelData, pBck, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false);
        if (!mpModel || !btkRet || !bckRet)
            ret = false;
    }
    return ret;
}

/* 023BD5BC */
static BOOL solidHeapCB(fopAc_ac_c* ac) {
    return ((daObjYgush00_c*)ac)->create_heap();
}

/* 023BD5C0 */
cPhs_State daObjYgush00_c::_create() {
    fopAcM_ct(this, daObjYgush00_c);

    /* fopAcM_IsFirstCreating: base_process_class::mInitState (+0xC) == 0 */
    if (mInitState == 0) {
        u32 type = param_get_arg();
        /* HD: one store; GameCube stores, then resets out-of-range values */
        if (type < 4)
            mType = type;
        else
            mType = 0;
    }

    cPhs_State ret = dComIfG_resLoad(&mPhase, l_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, solidHeapCB, 0x740) != 0) { /* HD: != 0 (GameCube == 1) */
            mpModel->setBaseScale(scale);
            mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
            mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
            fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
            /* HD: the min y is 0.0f * scale.y */
            fopAcM_setCullSizeBox(this,
                -80.0f * scale.x, 0.0f * scale.y, -80.0f * scale.z,
                80.0f * scale.x, 125.0f * scale.y, 80.0f * scale.z);
        } else {
            ret = cPhs_ERROR_e;
        }
    }
    return ret;
}

/* 023BD814 */
bool daObjYgush00_c::_delete() {
    dComIfG_resDelete(&mPhase, l_arcname);
    return true;
}

/* 023BD848 */
bool daObjYgush00_c::_execute() {
    if (mType != 3 || dComIfGs_isEventBit(dSv_event_flag_c::COLORS_IN_HYRULE) == 1) {
        mBtkAnm.play();
        mBckAnm.play();
    }

    if (mType == 1) {
        if (mpGryw00 != nullptr) {
            daObjGryw00_c* gryw = mpGryw00;
            /* GameCube: if (lv <= current.pos.y) se; HD branches on lv > y (NaN plays the sound) */
            if (!(gryw->get_draw_water_lv(gryw) > current.pos.y)) {
                fopAcM_seStartCurrent_l(this, JA_SE_OBJ_SPRING, 0);
            }
        } else {
            mpGryw00 = (daObjGryw00_c*)fopAcM_SearchByName_l(fpcNm_Obj_Gryw00_e);
        }
    } else {
        fopAcM_seStartCurrent_l(this, JA_SE_OBJ_SPRING, 0);
    }
    return true;
}

/* 023BD924 */
bool daObjYgush00_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG1, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mBtkAnm.entry(mpModel->getModelData(), mBtkAnm.getFrame());
    mBckAnm.entry(mpModel->getModelData(), mBckAnm.getFrame());
    mDoExt_modelUpdateDL(mpModel);
    return true;
}

/* method table entries (HD: tail branches) */
/* 023BD810 */
static cPhs_State daObjYgush00_Create(daObjYgush00_c* i_this) {
    return i_this->_create();
}
/* 023BD844 */
static BOOL daObjYgush00_Delete(daObjYgush00_c* i_this) {
    return i_this->_delete();
}
/* 023BD920 */
static BOOL daObjYgush00_Execute(daObjYgush00_c* i_this) {
    return i_this->_execute();
}
/* 023BD9A8 */
static BOOL daObjYgush00_Draw(daObjYgush00_c* i_this) {
    return i_this->_draw();
}
/* 023BD9AC */
static BOOL daObjYgush00_IsDelete(daObjYgush00_c* i_this) {
    return TRUE;
}

/* ---- compiler-generated (HD) ---- */

/* 023BD9B4 */
static void __sinit_d_a_obj_Ygush00_cpp() {
    sinit_header_statics(0x1046CA6C, 0x101CE020);
}

// HD: deleting destructor of a class with a trivial destructor
/* 023BDA48 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: daObjYgush00_c deleting destructor (mBckAnm's J3DMtxCalc member at +0x43C has a destructor)
/* 023BDA5C */
static void daObjYgush00_c_dt(daObjYgush00_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        YgushMtxCalc_destruct(&reinterpret_cast<YgushBckLayout*>(&i_this->mBckAnm)->mVtable, 0);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: an empty virtual (per-TU copy)
/* 023BDABC */
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 10033A20
str 10033A84
str 10033A80 "FALSE" renamed # HD: assertion spells zero.
data mdl_table 101CDFF0
data btk_table 101CE000
data bck_table 101CE010
ext 0234C5C4 f32 daObjGryw00_c::get_draw_water_lv(fopAc_ac_c*)
ext 027F2BC0 void YgushFrameCtrl_init(J3DFrameCtrl*, s32)
ext 027DA984 void YgushMtxCalc_construct(void*)
ext 027F3628 void YgushMtxCalc_destruct(be<u32>*, s32)
*/
