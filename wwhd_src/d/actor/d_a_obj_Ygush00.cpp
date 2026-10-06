/**
 * d_a_obj_Ygush00.cpp (WWHD)
 * Object - Spring water gush (Ygush00)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_Ygush00.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define l_arcname STR(0x10033A20) /* "Ygush00" */
#define SAFESTRING_VTBL 0x10033A2C
#define YGUSH00_VTBL 0x10033A6C /* HD: daObjYgush00_c vtable */

/* static u32 mdl_table[4], btk_table[4], bck_table[4] (.data) */
#define MDL_TABLE 0x101CDFF0
#define BTK_TABLE 0x101CE000
#define BCK_TABLE 0x101CE010

enum { TEV_TYPE_BG1_l = 2 };
enum { JA_SE_OBJ_SPRING = 0x61FE };
enum { fpcNm_Obj_Gryw00_e = 0x8E };
enum { COLORS_IN_HYRULE = 0x3802 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dComIfGs_isEventBit: dSv_event_c at *(0x101F84DC) + 0x644 (as in d_a_npc_ds1 / d_a_kb) */
static inline BOOL dComIfGs_isEventBit_l(u16 f) {
    return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f);
}
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
/* 0234C5C4 daObjGryw00_c::get_draw_water_lv(this, actor) -> f32 */
static inline f32 daObjGryw00_get_draw_water_lv(void* self, void* actor) { return gabi::call<f32>(0x0234C5C4, self, actor); }

struct daObjYgush00_c : fopAc_ac_c {
    cPhs_State _create();
    bool _delete();
    bool _execute();
    bool _draw();
    bool create_heap();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_btkAnm mBtkAnm;
    /* 0x42C */ mDoExt_bckAnm mBckAnm;
    /* 0x4B8 */ be<s32> mType;
    /* 0x4BC */ gptr<fopAc_ac_c> mpGryw00;
};
WWHD_OFFSET(daObjYgush00_c, mBtkAnm, 0x3B8);
WWHD_OFFSET(daObjYgush00_c, mBckAnm, 0x42C);
WWHD_OFFSET(daObjYgush00_c, mType, 0x4B8);
WWHD_SIZE(daObjYgush00_c, 0x4C0);

/* 023BDAC0: daObj::PrmAbstract<daObjYgush00_c::Param_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x023BDAC0, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x023BDAC0, PrmAbstract);

/* 023BD414 */
bool daObjYgush00_c::create_heap() {
    WWHD_FUNC(0x023BD414, bool, this);
    bool ret = true;
    J3DModelData* pModelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname, gabi::load<u32>(MDL_TABLE + 4 * mType), SAFESTRING_VTBL);
    J3DAnmTextureSRTKey* pBtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(l_arcname, gabi::load<u32>(BTK_TABLE + 4 * mType), SAFESTRING_VTBL);
    J3DAnmTransform* pBck = (J3DAnmTransform*)dComIfG_getObjectRes(l_arcname, gabi::load<u32>(BCK_TABLE + 4 * mType), SAFESTRING_VTBL);

    if (!pModelData || !pBtk || !pBck) {
        JUT_ASSERT_fail(STR(0x10033A84), 0xCF, STR(0x10033A80)); /* JUT_ASSERT(207, FALSE) */
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
VERIFY(0x023BD414, &daObjYgush00_c::create_heap);

/* 023BD5BC (tail call) */
static BOOL solidHeapCB(fopAc_ac_c* ac) {
    WWHD_FUNC(0x023BD5BC, BOOL, ac);
    return ((daObjYgush00_c*)ac)->create_heap();
}
VERIFY(0x023BD5BC, solidHeapCB);

/* 023BD5C0 */
cPhs_State daObjYgush00_c::_create() {
    WWHD_FUNC(0x023BD5C0, cPhs_State, this);
    /* fopAcM_ct(this, daObjYgush00_c): HD vtable, inline mDoExt_btkAnm / mDoExt_bckAnm constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = YGUSH00_VTBL;
            mDoExt_btkAnm::ct(&mBtkAnm);
            u32 self = gabi::ea(this);
            gabi::call(0x027F2BC0, &mBckAnm, 0); /* J3DFrameCtrl::init */
            gabi::store<u32>(self + 0x43C, 0x1016E54C);
            gabi::call(0x027DA984, gabi::at<void>(self + 0x440));
            gabi::store<u32>(self + 0x4A8, 0);
            gabi::store<u32>(self + 0x4AC, 0);
            gabi::store<u32>(self + 0x484, 0);
            gabi::store<u32>(self + 0x474, 0x1016D820);
            gabi::store<u32>(self + 0x43C, 0x10033A44);
            gabi::store<u32>(self + 0x4B4, 0);
            gabi::store<u32>(self + 0x4B0, 0);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    /* fopAcM_IsFirstCreating: base_process_class::mInitState (+0xC) == 0 */
    if (gabi::load<u8>(gabi::ea(this) + 0xC) == 0) {
        u32 type = PrmAbstract(this, 3, 0);
        /* HD: one store; GameCube stores, then resets out-of-range values */
        if (type < 4)
            mType = type;
        else
            mType = 0;
    }

    cPhs_State ret = dComIfG_resLoad(&mPhase, l_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, 0x023BD5BC /* solidHeapCB */, 0x740) != 0) { /* HD: != 0 (GameCube == 1) */
            J3DModel_setBaseScale(mpModel, &scale);
            mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
            J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
            cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
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
VERIFY(0x023BD5C0, &daObjYgush00_c::_create);

/* 023BD814 */
bool daObjYgush00_c::_delete() {
    WWHD_FUNC(0x023BD814, bool, this);
    dComIfG_resDelete(&mPhase, l_arcname);
    return true;
}
VERIFY(0x023BD814, &daObjYgush00_c::_delete);

/* 023BD848 */
bool daObjYgush00_c::_execute() {
    WWHD_FUNC(0x023BD848, bool, this);
    if (mType != 3 || dComIfGs_isEventBit_l(COLORS_IN_HYRULE) == 1) {
        mBtkAnm.play();
        mBckAnm.play();
    }

    if (mType == 1) {
        if (mpGryw00 != nullptr) {
            fopAc_ac_c* gryw = mpGryw00;
            /* GameCube: if (lv <= current.pos.y) se; HD branches on lv > y (NaN plays the sound) */
            if (!(daObjGryw00_get_draw_water_lv(gryw, gryw) > current.pos.y)) {
                fopAcM_seStartCurrent_l(this, JA_SE_OBJ_SPRING, 0);
            }
        } else {
            mpGryw00 = fopAcM_SearchByName_l(fpcNm_Obj_Gryw00_e);
        }
    } else {
        fopAcM_seStartCurrent_l(this, JA_SE_OBJ_SPRING, 0);
    }
    return true;
}
VERIFY(0x023BD848, &daObjYgush00_c::_execute);

/* 023BD924 */
bool daObjYgush00_c::_draw() {
    WWHD_FUNC(0x023BD924, bool, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG1_l, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mBtkAnm.entry(J3DModel_getModelData(mpModel), mBtkAnm.getFrame());
    mBckAnm.entry(J3DModel_getModelData(mpModel), mBckAnm.getFrame());
    mDoExt_modelUpdateDL(mpModel);
    return true;
}
VERIFY(0x023BD924, &daObjYgush00_c::_draw);

/* method table entries (HD: tail branches) */
/* 023BD810 */
static cPhs_State daObjYgush00_Create(daObjYgush00_c* i_this) {
    WWHD_FUNC(0x023BD810, cPhs_State, i_this);
    return i_this->_create();
}
VERIFY(0x023BD810, daObjYgush00_Create);
/* 023BD844 */
static BOOL daObjYgush00_Delete(daObjYgush00_c* i_this) {
    WWHD_FUNC(0x023BD844, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x023BD844, daObjYgush00_Delete);
/* 023BD920 */
static BOOL daObjYgush00_Execute(daObjYgush00_c* i_this) {
    WWHD_FUNC(0x023BD920, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x023BD920, daObjYgush00_Execute);
/* 023BD9A8 */
static BOOL daObjYgush00_Draw(daObjYgush00_c* i_this) {
    WWHD_FUNC(0x023BD9A8, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x023BD9A8, daObjYgush00_Draw);
/* 023BD9AC */
static BOOL daObjYgush00_IsDelete(daObjYgush00_c* i_this) {
    WWHD_FUNC(0x023BD9AC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x023BD9AC, daObjYgush00_IsDelete);

/* ---- compiler-generated (HD) ---- */

/* 023BD9B4 */
static void __sinit_d_a_obj_Ygush00_cpp() {
    WWHD_FUNC(0x023BD9B4, void, (u32)0);
    sinit_header_statics(0x1046CA6C, 0x101CE020);
}
VERIFY(0x023BD9B4, __sinit_d_a_obj_Ygush00_cpp);

/* 023BDA48: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023BDA48, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x023BDA48, trivial_dt);

/* 023BDA5C: daObjYgush00_c deleting destructor (mBckAnm's J3DMtxCalc member at +0x43C has a destructor) */
static void daObjYgush00_c_dt(daObjYgush00_c* i_this, s32 flags) {
    WWHD_FUNC(0x023BDA5C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x027F3628, gabi::ea(i_this) + 0x43C, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x023BDA5C, daObjYgush00_c_dt);

/* 023BDABC: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x023BDABC, void, p);
}
VERIFY(0x023BDABC, empty_virtual);
