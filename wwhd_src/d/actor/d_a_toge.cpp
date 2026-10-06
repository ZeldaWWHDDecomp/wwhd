/**
 * d_a_toge.cpp (WWHD)
 * Object - Wind Temple - Floor spikes
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_toge.cpp) to the WWHD layout and verified against cking.rpx.
 * Layout: every GameCube member +0x11C (class size 0x5A4).
 */
#include "bindings.h"

/* guest string literals / data (.rodata, .data) */
#define M_arcname STR(0x1004083C)      /* "Htoge1" */
#define FILE_NAME STR(0x100407D4)      /* "d_a_toge.cpp" */
#define SAFESTRING_VTBL 0x1004079C     /* this TU's copy of the sead::SafeString vtable */
#define TOGE_VTBL 0x100407C4           /* daToge_c vtable (HD virtual destructor) */
#define TOGE_AAB_VTBL 0x100407B4       /* this TU's cM3dGAab vtable */
#define L_CYL_SRC gabi::at<dCcD_SrcCyl>(0x101D28B8)

enum {
    dRes_INDEX_HTOGE1_BDL_HTOGE1_e = 4,
    dRes_INDEX_HTOGE1_DZB_HTOGE1A_e = 7,
    dRes_INDEX_HTOGE1_DZB_HTOGE1B_e = 8,
};
enum {
    JA_SE_OBJ_TOGETOGE_OUT = 0x7840,
    JA_SE_OBJ_TOGETOGE_MOVE = 0x7841,
    JA_SE_OBJ_TOGETOGE_IN = 0x7842,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 024F2478 dBgW_NewSet(cBgD_t*, u32 flags, Mtx*) */
static inline dBgW* dBgW_NewSet(cBgD_t* data, u32 flags, Mtx34* mtx) { return gabi::call<dBgW*>(0x024F2478, data, flags, mtx); }
/* 0200ECD4 cLib_addCalc(f32* value, target, scale, maxStep, minStep) -> remaining distance */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* 0207A9A0 cLib_calcTimer<u8> (the copy the WWHD linker kept) */
static inline u8 cLib_calcTimer_u8(be<u8>* t) { return gabi::call<u8>(0x0207A9A0, t); }
/* 025D50BC fopAc_ac_c::~fopAc_ac_c */
static inline void fopAc_ac_c_dt(fopAc_ac_c* a, s32 flags) { gabi::call(0x025D50BC, a, flags); }
/* fopAcM_SearchByName (HD inline): the process name is passed by address to fpcSch_JudgeForPName */
static inline fopAc_ac_c* fopAcM_SearchByName(s16 name) {
    gabi::Local<be<s16>> key;
    *key = name;
    return fopAcIt_Judge(0x025E121C /* fpcSch_JudgeForPName */, key.get());
}
/* HD J3D: j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint matrix block at
 * model +0x2C (+4 flags, +0x10 matrices), user area +0xB8 (as d_a_mt) */
struct toge_J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ be<u32> mpMtx;
};
struct toge_J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<toge_J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ gptr<J3DModelData> mModelData;
    /* 0xB0 */ u8 _B0[0xB8 - 0xB0];
    /* 0xB8 */ be<u32> mUserArea;
};
static inline toge_J3DModel_l* toge_j3dSys_getModel() { return gabi::at<toge_J3DModel_l>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* toge_getAnmMtx(toge_J3DModel_l* model, u32 jntNo) {
    toge_J3DMtxBlock_l* blk = model->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(blk->mpMtx + jntNo * 0x30);
}
/* J3DModelData::getJointNodePointer(i) (HD inline): count +4, nodes (0x1C each) +8; out of range: the first */
static inline u32 toge_getJointNode(J3DModelData* md, u32 i) {
    u32 num = gabi::load<u32>(gabi::ea(md) + 4);
    u32 base = gabi::load<u32>(gabi::ea(md) + 8);
    return i < num ? base + i * 0x1C : base;
}
/* J3DModelData::getJointName (HD): 027F68FC returns the joint tree header; the JUTNameTab is a
 * self-relative offset at +0x10 (0: none). 027DF9B0 JUTNameTab::getIndex(name) */
static inline void* toge_getJointName(J3DModelData* md) {
    u32 hdr = gabi::call<u32>(0x027F68FC, md);
    u32 off = gabi::load<u32>(hdr + 0x10);
    return gabi::at<void>(off != 0 ? hdr + 0x10 + off : 0);
}
static inline s32 JUTNameTab_getIndex(void* tab, const char* name) { return gabi::call<s32>(0x027DF9B0, tab, name); }

struct daToge_c : fopAc_ac_c {
    BOOL _delete();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State _create();
    BOOL _execute();
    void set_collision();
    void search_wind();
    void toge_move();
    void toge_seStart(u32 i_seNum);
    BOOL _draw();
    void set_mtx() {
        J3DModel_setBaseScale(mpModel, &scale);
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(current.angle.y);
        J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
        PSMTXCopy(mDoMtx_stack_c::get(), &mtx1);
    }

    /* 0x3AC */ request_of_phase_process_class m_Phs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ gptr<dBgW> mpBgW1;
    /* 0x528 */ Mtx34 mtx1;
    /* 0x558 */ gptr<dBgW> mpBgW2;
    /* 0x55C */ Mtx34 mtx2;
    /* 0x58C */ be<f32> unk470;
    /* 0x590 */ be<f32> unk474;
    /* 0x594 */ be<s32> mSwitchNo;
    /* 0x598 */ u8 pad47C[4];
    /* 0x59C */ be<u32> mWindTagId;
    /* 0x5A0 */ be<u8> mEventState;
    /* 0x5A1 */ be<u8> unk485;
    /* 0x5A2 */ be<u8> unk486;
    /* 0x5A3 */ u8 _5A3;
};
WWHD_OFFSET(daToge_c, mCyl, 0x3F4);
WWHD_OFFSET(daToge_c, mpBgW1, 0x524);
WWHD_OFFSET(daToge_c, mtx2, 0x55C);
WWHD_OFFSET(daToge_c, mWindTagId, 0x59C);
WWHD_SIZE(daToge_c, 0x5A4);

/* daWindTag_c::mOffsY (+0x6F0 in WWHD) */
static inline f32 daWindTag_getOffsY(fopAc_ac_c* a) { return gabi::load<f32>(gabi::ea(a) + 0x6F0); }

/* 024C7338 */
BOOL daToge_c::_delete() {
    WWHD_FUNC(0x024C7338, BOOL, this);
    dComIfG_resDelete(&m_Phs, M_arcname);

    if (mpBgW1 != nullptr && dBgW_ChkUsed(mpBgW1)) {
        cBgS_Release(dComIfG_Bgsp(), mpBgW1);
    }

    if (mpBgW2 != nullptr && dBgW_ChkUsed(mpBgW2)) {
        cBgS_Release(dComIfG_Bgsp(), mpBgW2);
    }

    return TRUE;
}
VERIFY(0x024C7338, &daToge_c::_delete);

/* 024C6E74 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024C6E74, BOOL, i_this);
    return static_cast<daToge_c*>(i_this)->CreateHeap();
}
VERIFY(0x024C6E74, CheckCreateHeap);

/* 024C6D5C */
BOOL daToge_c::CreateHeap() {
    WWHD_FUNC(0x024C6D5C, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_HTOGE1_BDL_HTOGE1_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x11A, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x11A, STR(0x100407E4));

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000U, 0x11000002U);

    if (!mpModel) {
        return FALSE;
    }

    gabi::store<u32>(gabi::ea(mpModel.get()) + 0xB8, gabi::ea(this)); /* setUserArea */

    mpBgW1 = dBgW_NewSet((cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_HTOGE1_DZB_HTOGE1A_e, SAFESTRING_VTBL), 1 /* cBgW::MOVE_BG_e */, &mtx1);
    mpBgW2 = dBgW_NewSet((cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_HTOGE1_DZB_HTOGE1B_e, SAFESTRING_VTBL), 1, &mtx2);

    if (mpBgW1 == nullptr || mpBgW2 == nullptr) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x024C6D5C, &daToge_c::CreateHeap);

/* 024C6E78 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x024C6E78, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        toge_J3DModel_l* model = toge_j3dSys_getModel();
        daToge_c* i_this = gabi::at<daToge_c>(model->mUserArea);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(toge_getAnmMtx(model, jntNo), mDoMtx_stack_c::get());
            mDoMtx_stack_c::transM(0.0f, i_this->unk470, 0.0f);
            mtx_copy(toge_getAnmMtx(model, jntNo), mDoMtx_stack_c::get());

            PSMTXCopy(mDoMtx_stack_c::get(), gabi::at<Mtx34>(0x104B4868) /* j3dSys.mCurrentMtx */);
            PSMTXCopy(mDoMtx_stack_c::get(), &i_this->mtx2);
        }
    }

    return TRUE;
}
VERIFY(0x024C6E78, nodeCallBack);

/* 024C6FB0 */
BOOL daToge_c::Create() {
    WWHD_FUNC(0x024C6FB0, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */

    fopAcM_setCullSizeBox(this, -80.0f, 0.0f, -80.0f, 80.0f, 120.0f, 80.0f);
    mStts.Init(0xFF, 0xFF, this);

    mCyl.Set(L_CYL_SRC);
    mCyl.SetStts(&mStts);

    u8 sw = fopAcM_GetParam(this) & 0xFF; /* daToge_prm::getSwitchNo */
    mSwitchNo = sw;

    if (fopAcM_isSwitch(this, sw)) {
        unk470 = -150.0f; /* m_y_min */
        mEventState = 2;
    }

    set_mtx();
    PSMTXCopy(&mtx1, &mtx2); /* cMtx_copy */

    void* jointName = toge_getJointName(J3DModel_getModelData(mpModel));
    /* HD: JUTNameTab::getIndex instead of the GameCube strcmp loop */
    s32 idx = JUTNameTab_getIndex(jointName, STR(0x1004080C) /* "toge" */);
    if (idx >= 0) {
        u32 jnt = toge_getJointNode(J3DModel_getModelData(mpModel), (u16)idx);
        gabi::store<u32>(jnt + 8, 0x024C6E78); /* setCallBack(nodeCallBack) */
    }

    J3DModel_calc(mpModel);

    dBgS_Regist(dComIfG_Bgsp(), mpBgW1, this);
    dBgS_Regist(dComIfG_Bgsp(), mpBgW2, this);

    dBgW_Move(mpBgW1);
    dBgW_Move(mpBgW2);

    return TRUE;
}
VERIFY(0x024C6FB0, &daToge_c::Create);

/* 024C71FC */
cPhs_State daToge_c::_create() {
    WWHD_FUNC(0x024C71FC, cPhs_State, this);
    /* fopAcM_ct(this, daToge_c); HD: fopAc_ac_c has a vtable, the members' inline constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = TOGE_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, TOGE_AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&m_Phs, M_arcname);

    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x024C6E74 /* CheckCreateHeap */, 0x1400)) {
            return cPhs_ERROR_e;
        }

        Create();
    }

    return phase_state;
}
VERIFY(0x024C71FC, &daToge_c::_create);

/* 024C77F0 */
BOOL daToge_c::_execute() {
    WWHD_FUNC(0x024C77F0, BOOL, this);
    if (mSwitchNo != 0xFF) {
        if (fopAcM_isSwitch(this, mSwitchNo) != 0) {
            if (mEventState != 1) {
                unk485 = 1;
            }
        } else if (!fopAcM_isSwitch(this, mSwitchNo) && mEventState != 4) {
            mEventState = 3;
        }
    } else {
        search_wind();

        fopAc_ac_c* pActor = fopAcM_SearchByID(mWindTagId);

        if (pActor != nullptr) {
            if (daWindTag_getOffsY(pActor) > 0.0f) {
                if (mEventState != 4) {
                    mEventState = 3;
                }
            } else if (mEventState != 1) {
                unk485 = 1;
            }
        }
    }

    toge_move();

    set_mtx();

    dBgW_Move(mpBgW2);

    set_collision();

    return TRUE;
}
VERIFY(0x024C77F0, &daToge_c::_execute);

/* 024C7774 */
void daToge_c::set_collision() {
    WWHD_FUNC(0x024C7774, void, this);
    if (mEventState != 2) {
        gabi::Local<cXyz> center;
        f32 x = current.pos.x;
        f32 d = unk470 - 10.0f;
        center->x = x;
        center->y = current.pos.y + d;
        center->z = current.pos.z;

        mCyl.SetC(center.get());
        dComIfG_Ccsp_Set(&mCyl);
    }
}
VERIFY(0x024C7774, &daToge_c::set_collision);

/* 024C7464 */
void daToge_c::search_wind() {
    WWHD_FUNC(0x024C7464, void, this);
    fopAc_ac_c* pActor = fopAcM_SearchByName(0x187 /* fpcNm_WindTag_e */);

    if (pActor != nullptr) {
        mWindTagId = gabi::load<u32>(gabi::ea(pActor) + 4); /* fopAcM_GetID */
    } else {
        mWindTagId = fpcM_ERROR_PROCESS_ID_e;
    }
}
VERIFY(0x024C7464, &daToge_c::search_wind);

/* 024C7520 */
void daToge_c::toge_move() {
    WWHD_FUNC(0x024C7520, void, this);
    f32 f31 = 30.0f;
    f32 f30 = 15.0f;
    int timer = 0xA;
    bool r30 = true;
    switch (mEventState) {
    case 0:
        break;
    case 1:
        if (cLib_calcTimer_u8(&unk486) != 0)
            break;
        toge_seStart(JA_SE_OBJ_TOGETOGE_IN);
        mEventState = 2;
        // Fallthrough
    case 2:
        cLib_addCalc(&unk470, -150.0f, 0.1f, f31, f30);
        break;
    case 3:
        toge_seStart(JA_SE_OBJ_TOGETOGE_OUT);
        mEventState = 4;
        r30 = false;
        // Fallthrough
    case 4:
        if (cLib_addCalc(&unk470, unk474, 0.1f, f31, f30) == 0.0f) {
            if (unk470 < 0.0f) {
                unk474 = 0.0f;
            } else if (unk485 != 0) {
                unk486 = timer;
                mEventState = 1;
                unk485 = 0;
            } else {
                if (r30) {
                    toge_seStart(JA_SE_OBJ_TOGETOGE_MOVE);
                }
                unk474 = -60.0f;
            }
        }
        break;
    }
}
VERIFY(0x024C7520, &daToge_c::toge_move);

/* 024C74C0: toge_seStart (not named by the matcher). HD: no NULL checks in this fopAcM_seStart */
void daToge_c::toge_seStart(u32 i_seNum) {
    WWHD_FUNC(0x024C74C0, void, this, i_seNum);
    if ((fopAcM_GetParam(this) >> 8) & 0xF /* daToge_prm::getSeEnabled */) {
        mDoAud_seStart(i_seNum, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    }
}
VERIFY(0x024C74C0, &daToge_c::toge_seStart);

/* 024C73C8 */
BOOL daToge_c::_draw() {
    WWHD_FUNC(0x024C73C8, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);

    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();

    return TRUE;
}
VERIFY(0x024C73C8, &daToge_c::_draw);

/* 024C7334 */
static cPhs_State daToge_Create(void* i_this) {
    WWHD_FUNC(0x024C7334, cPhs_State, i_this);
    return static_cast<daToge_c*>(i_this)->_create();
}
VERIFY(0x024C7334, daToge_Create);

/* 024C73C4 */
static BOOL daToge_Delete(void* i_this) {
    WWHD_FUNC(0x024C73C4, BOOL, i_this);
    return static_cast<daToge_c*>(i_this)->_delete();
}
VERIFY(0x024C73C4, daToge_Delete);

/* 024C7460 */
static BOOL daToge_Draw(void* i_this) {
    WWHD_FUNC(0x024C7460, BOOL, i_this);
    return static_cast<daToge_c*>(i_this)->_draw();
}
VERIFY(0x024C7460, daToge_Draw);

/* 024C79B4 */
static BOOL daToge_Execute(void* i_this) {
    WWHD_FUNC(0x024C79B4, BOOL, i_this);
    return static_cast<daToge_c*>(i_this)->_execute();
}
VERIFY(0x024C79B4, daToge_Execute);

/* 024C7A60 */
static BOOL daToge_IsDelete(void*) {
    WWHD_FUNC(0x024C7A60, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024C7A60, daToge_IsDelete);

/* ---- compiler-generated (HD) ---- */

/* 024C79B8: __sinit_d_a_toge_cpp (the per-TU header statics only) */
static void __sinit_d_a_toge_cpp() {
    WWHD_FUNC(0x024C79B8, void);
    sinit_header_statics(0x1046E97C, 0x101D28FC);
}
VERIFY(0x024C79B8, __sinit_d_a_toge_cpp);

/* 024C7A4C: sead::SafeString deleting destructor (this TU's copy, SafeString vtable +0xC) */
static void SafeString_dt(SafeString* s, s32 flags) {
    WWHD_FUNC(0x024C7A4C, void, s, flags);
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}
VERIFY(0x024C7A4C, SafeString_dt);

/* 024C7A68: daToge_c deleting destructor (HD virtual destructor) */
static void daToge_c_dt(daToge_c* i_this, s32 flags) {
    WWHD_FUNC(0x024C7A68, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        fopAc_ac_c_dt(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024C7A68, daToge_c_dt);

/* 024C7AD4: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy) */
static void SafeString_assureTermination(SafeString*) {
    WWHD_FUNC(0x024C7AD4, void, (u32)0);
}
VERIFY(0x024C7AD4, SafeString_assureTermination);
