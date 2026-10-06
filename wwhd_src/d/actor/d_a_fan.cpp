/**
 * d_a_fan.cpp (WWHD)
 * Object - Fan (wind fan in the Wind Temple / Tower of the Gods; moving BG with a wind Cps)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fan.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1000E904 /* this TU's sead::SafeString vtable */
#define FAN_VTBL 0x1000EA70
#define FILE_NAME STR(0x1000E958) /* "d_a_fan.cpp" */

/* static tables (.rodata / .data) */
static const char* m_arcname(s32 type) { return gabi::at<const char>(gabi::load<u32>(0x101B4BC4 + type * 4)); }
#define m_arcname2 STR(0x1000E9C0) /* "Yaflw00" */
static s16 m_bdlidx(s32 type) { return gabi::load<s16>(0x1000E9C8 + type * 2); }
static s16 m_dzbidx(s32 type) { return gabi::load<s16>(0x1000E9D0 + type * 2); }
static s16 m_fan_speed(s32 type) { return gabi::load<s16>(0x1000E9D8 + type * 2); }
static f32 m_wind_length(s32 type) { return gabi::load<f32>(0x1000E9E0 + type * 4); }
static u32 m_heapsize(s32 type) { return gabi::load<u32>(0x1000E9EC + type * 4); }
static cXyz* m_cull_min(s32 type) { return gabi::at<cXyz>(0x1000E9F8 + type * 0xC); }
static cXyz* m_cull_max(s32 type) { return gabi::at<cXyz>(0x1000EA1C + type * 0xC); }
static f32 m_wind_r(s32 type) { return gabi::load<f32>(0x1000EA40 + type * 4); }
static cXyz* m_wind_model_scale(s32 type) { return gabi::at<cXyz>(0x1000EA4C + type * 0xC); }
#define l_cps_src gabi::at<dCcD_SrcCps>(0x101B4B24)

enum {
    dRes_INDEX_YAFLW00_BCK_YAFLW00_e = 5,
    dRes_INDEX_YAFLW00_BDL_YAFLW00_e = 8,
    dRes_INDEX_YAFLW00_BTK_YAFLW00_01_e = 0xB,
    dRes_INDEX_YAFLW00_BTK_YAFLW00_02_e = 0xC,
};
enum { JA_SE_OBJ_WIND_TAG = 0x701D };
enum { fpcNm_LEVEL_SE_e = 0x18 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025DAC50 fopKyM_SearchByID(id) / 025DAD48 fopKyM_Delete(p) */
static inline void* fopKyM_SearchByID(u32 id) { return gabi::call<void*>(0x025DAC50, id); }
static inline BOOL fopKyM_Delete(void* p) { return gabi::call<BOOL>(0x025DAD48, p); }
/* 025DADA4 fopKyM_create(procName, param, pos, scale, createFunc) */
static inline u32 fopKyM_create(s16 name, s32 prm, cXyz* pos, cXyz* scale, u32 fn) {
    return gabi::call<u32>(0x025DADA4, name, prm, pos, scale, fn);
}
/* 0200F564 cLib_chaseS(s16* value, s16 target, s16 step) */
static inline bool cLib_chaseS(be<s16>* v, s16 target, s16 step) { return gabi::call<bool>(0x0200F564, v, target, step); }
/* 025164C0 dCcD_Cps::Set(const dCcD_SrcCps&); 020181FC cM3dGCps::Set(const cM3dGCpsS&) */
static inline void dCcD_Cps_Set(void* cps, const dCcD_SrcCps* src) { gabi::call(0x025164C0, cps, src); }
static inline void cM3dGCps_Set(void* cps, void* src) { gabi::call(0x020181FC, cps, src); }
/* 028E8DAC PSVECSubtract(a, b, out) */
static inline void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x028E8DAC, a, b, out); }
/* 025E8048 mDoExt_btkAnm::entry (HD variant taking the J3DModel*, not its model data) */
static inline void mDoExt_btkAnm_entryModel(mDoExt_btkAnm* a, J3DModel* m, f32 frame) { gabi::call(0x025E8048, a, m, frame); }
/* J3DModelData (HD) joint names: 027F68FC returns the joint name table header (as in d_a_npc_people) */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s32 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
/* HD J3D (as in d_a_kb): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint matrix block at model+0x2C */
struct J3DMtxBlock_fan_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_fan_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_fan_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xB8 - 0x30];
    /* 0xB8 */ be<u32> mUserArea;
};
static inline Mtx34* getAnmMtx(J3DModel_fan_l* m, s32 jnt) {
    J3DMtxBlock_fan_l* blk = m->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
static inline J3DModel_fan_l* j3dSys_getModel() { return gabi::at<J3DModel_fan_l>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }

/* cM3dGCpsS: start, end, radius */
struct cM3dGCpsS_l {
    /* 0x00 */ cXyz mStart;
    /* 0x0C */ cXyz mEnd;
    /* 0x18 */ be<f32> mRadius;
};

struct daFan_c : dBgS_MoveBgActor {
    cPhs_State _create();
    BOOL Delete();
    BOOL CreateHeap();
    BOOL Create();
    void set_mtx();
    void set_wind_length(f32);
    void set_cps(f32);
    BOOL Execute(Mtx34** mtxP);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;     /* GameCube 0x2C8 */
    /* 0x3E8 */ request_of_phase_process_class mWindPhs;
    /* 0x3F0 */ gptr<J3DModel> mModel;
    /* 0x3F4 */ dCcD_Stts mStts;
    /* 0x430 */ dCcD_Cps mCps;                            /* cM3dGCps at 0x548 */
    /* 0x568 */ u8 field_0x568[0x34];
    /* 0x59C */ mDoExt_btkAnm mBtkAnm;
    /* 0x610 */ be<u32> mSwitchNo;
    /* 0x614 */ be<u8> mType;
    /* 0x615 */ u8 _615[3];
    /* 0x618 */ cM3dGCpsS_l mCpsS;
    /* 0x634 */ be<s16> mFanAngle;
    /* 0x636 */ be<s16> mFanSpeed;
    /* 0x638 */ be<u32> field_0x638;
    /* 0x63C */ gptr<J3DModel> mWindModel;
    /* 0x640 */ mDoExt_btkAnm mWindBtkAnm0;
    /* 0x6B4 */ mDoExt_btkAnm mWindBtkAnm1;
    /* 0x728 */ mDoExt_bckAnm mWindBckAnm;
    /* 0x7B4 */ cXyz mWindScale;
    /* 0x7C0 */ be<u32> mWindSePId;

    u32 ea() { return gabi::ea(this); }
    void* cps_shape() { return gabi::at<u8>(ea() + 0x548); } /* mCps's cM3dGCps */
};
WWHD_OFFSET(daFan_c, mModel, 0x3F0);
WWHD_OFFSET(daFan_c, mStts, 0x3F4);
WWHD_OFFSET(daFan_c, mCps, 0x430);
WWHD_OFFSET(daFan_c, mBtkAnm, 0x59C);
WWHD_OFFSET(daFan_c, mType, 0x614);
WWHD_OFFSET(daFan_c, mCpsS, 0x618);
WWHD_OFFSET(daFan_c, mWindModel, 0x63C);
WWHD_OFFSET(daFan_c, mWindBckAnm, 0x728);
WWHD_OFFSET(daFan_c, mWindSePId, 0x7C0);
WWHD_SIZE(daFan_c, 0x7C4);

namespace daFan_prm {
/* HD: the type is clamped to 0 when it is 3 */
inline u8 getType(daFan_c* ac) {
    u32 t = (fopAcM_GetParam(ac) >> 8) & 0x03;
    if (t >= 3) t = 0;
    return t;
}
inline u8 getSwitchNo(daFan_c* ac) { return (fopAcM_GetParam(ac) >> 0) & 0xFF; }
};  // namespace daFan_prm

/* 02132BDC */
BOOL daFan_c::Delete() {
    WWHD_FUNC(0x02132BDC, BOOL, this);
    dComIfG_resDelete(&mPhs, m_arcname(mType));
    dComIfG_resDelete(&mWindPhs, m_arcname2);
    if (mWindSePId != (u32)-1 /* fpcM_ERROR_PROCESS_ID_e */) {
        void* se = fopKyM_SearchByID(mWindSePId);
        if (se != nullptr)
            fopKyM_Delete(se);
    }
    return TRUE;
}
VERIFY(0x02132BDC, &daFan_c::Delete);

/* 02132310 */
BOOL daFan_c::CreateHeap() {
    WWHD_FUNC(0x02132310, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname(mType), m_bdlidx(mType), SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x15e, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x15E, STR(0x1000E97C));

    mModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (!mModel)
        return FALSE;
    gabi::store<u32>(gabi::ea(mModel.get()) + 0xB8, gabi::ea(this)); /* setUserArea */

    modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname2, dRes_INDEX_YAFLW00_BDL_YAFLW00_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x17f, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x17F, STR(0x1000E97C));
    mWindModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000222);
    if (!mWindModel)
        return FALSE;

    J3DAnmTextureSRTKey* pbtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arcname2, dRes_INDEX_YAFLW00_BTK_YAFLW00_01_e, SAFESTRING_VTBL);
    if (pbtk == nullptr) /* JUT_ASSERT(400, pbtk != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 400, STR(0x1000E964));
    if (!mWindBtkAnm0.init(modelData, pbtk, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
        return FALSE;

    pbtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arcname2, dRes_INDEX_YAFLW00_BTK_YAFLW00_02_e, SAFESTRING_VTBL);
    if (pbtk == nullptr) /* JUT_ASSERT(0x19c, pbtk != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x19C, STR(0x1000E964));
    if (!mWindBtkAnm1.init(modelData, pbtk, TRUE, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
        return FALSE;

    J3DAnmTransform* pbck = (J3DAnmTransform*)dComIfG_getObjectRes(m_arcname2, dRes_INDEX_YAFLW00_BCK_YAFLW00_e, SAFESTRING_VTBL);
    if (pbck == nullptr) /* JUT_ASSERT(0x1a9, pbck != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x1A9, STR(0x1000E970));
    if (!mWindBckAnm.init(modelData, pbck, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false))
        return FALSE;

    return TRUE;
}
VERIFY(0x02132310, &daFan_c::CreateHeap);

/* 02132C4C */
BOOL daFan_c::Create() {
    WWHD_FUNC(0x02132C4C, BOOL, this);
    f32 wind_len = m_wind_length(mType);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModel)); /* fopAcM_SetMtx */
    cXyz* cullMin = m_cull_min(mType);
    cXyz* cullMax = m_cull_max(mType);
    fopAcM_setCullSizeBox(this, cullMin->x, cullMin->y, cullMin->z, cullMax->x, cullMax->y, cullMax->z);
    mStts.Init(0xFF, 0xFF, this);
    dCcD_Cps_Set(&mCps, l_cps_src);
    gabi::store<u32>(ea() + 0x430 + 0x44, gabi::ea(&mStts)); /* mCps.SetStts(&mStts) */
    cXyz* ws = m_wind_model_scale(mType);
    mWindScale.x = ws->x;
    mWindScale.y = ws->y;
    mWindScale.z = ws->z;
    set_cps(wind_len);
    cM3dGCps_Set(cps_shape(), &mCpsS);
    /* mCps.CalcAtVec(): at vector = end - start */
    PSVECSubtract(gabi::at<cXyz>(ea() + 0x554), gabi::at<cXyz>(ea() + 0x548), gabi::at<cXyz>(ea() + 0x4AC));
    set_mtx();
    mSwitchNo = daFan_prm::getSwitchNo(this);

    /* HD: the joint loop became JUTNameTab::getIndex("puro") */
    u32 jointName = J3DModelData_getJointName(J3DModel_getModelData(mModel));
    s32 idx = JUTNameTab_getIndex(jointName, STR(0x1000E9B0) /* "puro" */);
    if (idx >= 0) {
        /* getJointNodePointer(i)->setCallBack(nodeCallBack): nodes of 0x1C from +8, index checked against +4 */
        u32 data = gabi::ea(J3DModel_getModelData(mModel));
        u32 n = gabi::load<u32>(data + 4);
        u32 p = gabi::load<u32>(data + 8);
        if ((u32)(u16)idx < n)
            p += (u16)idx * 0x1C;
        gabi::store<u32>(p + 8, 0x02131FF8 /* nodeCallBack */);
    }

    J3DModel_calc(mModel);
    mWindSePId = fopKyM_create(fpcNm_LEVEL_SE_e, JA_SE_OBJ_WIND_TAG, &eyePos, nullptr, 0);
    return TRUE;
}
VERIFY(0x02132C4C, &daFan_c::Create);

/* 02131FF8 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02131FF8, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_fan_l* model = j3dSys_getModel();
        daFan_c* i_this = gabi::at<daFan_c>(model->mUserArea);
        if (i_this != nullptr) {
            s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
            i_this->mFanAngle = (s16)(i_this->mFanAngle + i_this->mFanSpeed);
            PSMTXCopy(getAnmMtx(model, jntNo), mDoMtx_stack_c::get());
            mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->mFanAngle);
            mtx_copy(getAnmMtx(model, jntNo), mDoMtx_stack_c::get()); /* setAnmMtx */
            PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
        }
    }
    return TRUE;
}
VERIFY(0x02131FF8, nodeCallBack);

/* 0213212C */
cPhs_State daFan_c::_create() {
    WWHD_FUNC(0x0213212C, cPhs_State, this);
    /* fopAcM_ct(this, daFan_c): base constructor, vtable, inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = FAN_VTBL;
            dCcD_Stts_ct(&mStts);
            /* dCcD_Cps */
            u32 c = ea() + 0x430;
            gabi::call(0x02515FB8, gabi::at<u8>(c)); /* dCcD_GObjInf::dCcD_GObjInf */
            gabi::store<u32>(c + 0x114, 0x100015A8); /* cCcD_ShapeAttr */
            gabi::store<u32>(c + 0x110, 0x1000E91C); /* cM3dGAab (this TU) */
            gabi::call(0x02018150, gabi::at<u8>(c + 0x118)); /* cM3dGCps::cM3dGCps */
            gabi::store<u32>(c + 0x3C, 0x1004AF18);
            gabi::store<u32>(c + 0x114, 0x1004AF70);
            gabi::store<u32>(c + 0x130, 0x1004AF60);
            mDoExt_btkAnm::ct(&mBtkAnm);
            mDoExt_btkAnm::ct(&mWindBtkAnm0);
            mDoExt_btkAnm::ct(&mWindBtkAnm1);
            /* mDoExt_bckAnm (inline): J3DFrameCtrl, anm vtables */
            u32 b = ea() + 0x728;
            gabi::call(0x027F2BC0, gabi::at<u8>(b), 0); /* J3DFrameCtrl::init(0) */
            gabi::store<u32>(b + 0x10, 0x1016E54C);
            gabi::call(0x027DA984, gabi::at<u8>(b + 0x14));
            gabi::store<u32>(b + 0x58, 0);
            gabi::store<u32>(b + 0x84, 0);
            gabi::store<u32>(b + 0x10, 0x1000E92C);
            gabi::store<u32>(b + 0x80, 0);
            gabi::store<u32>(b + 0x48, 0x1016D820);
            gabi::store<u32>(b + 0x7C, 0);
            gabi::store<u32>(b + 0x88, 0);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    mType = daFan_prm::getType(this);
    cPhs_State rt1 = dComIfG_resLoad(&mPhs, m_arcname(mType));
    if (rt1 != cPhs_COMPLEATE_e)
        return rt1;

    cPhs_State rt2 = dComIfG_resLoad(&mWindPhs, m_arcname2);
    if (rt2 != cPhs_COMPLEATE_e)
        return rt2;

    /* HD: MoveBGCreate's result is returned directly */
    return MoveBGCreate(m_arcname(mType), m_dzbidx(mType), 0x024EE708 /* dBgS_MoveBGProc_TypicalRotY */, m_heapsize(mType));
}
VERIFY(0x0213212C, &daFan_c::_create);

/* 021326B0 */
void daFan_c::set_mtx() {
    WWHD_FUNC(0x021326B0, void, this);
    J3DModel_setBaseScale(mModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    J3DModel_setBaseTRMtx(mModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mBgMtx);

    J3DModel_setBaseScale(mWindModel, &mWindScale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), (s16)(current.angle.x + 0x4000), current.angle.y, current.angle.z);
    J3DModel_setBaseTRMtx(mWindModel, mDoMtx_stack_c::get());
}
VERIFY(0x021326B0, &daFan_c::set_mtx);

/* 02132698 */
void daFan_c::set_wind_length(f32 h) {
    WWHD_FUNC(0x02132698, void, this, h);
    f32 len = m_wind_length(mType);
    len *= h;
    set_cps(len);
}
VERIFY(0x02132698, &daFan_c::set_wind_length);

/* 0213258C */
void daFan_c::set_cps(f32 h) {
    WWHD_FUNC(0x0213258C, void, this, h);
    mCpsS.mRadius = m_wind_r(mType);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), (s16)(current.angle.x + 0x4000), current.angle.y, current.angle.z);
    mDoMtx_stack_push();
    mDoMtx_stack_c::transM(0.0f, mCpsS.mRadius, 0.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), cXyz_Zero, &mCpsS.mStart);
    mDoMtx_stack_pop();
    mDoMtx_stack_c::transM(0.0f, mCpsS.mRadius + h, 0.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), cXyz_Zero, &mCpsS.mEnd);
}
VERIFY(0x0213258C, &daFan_c::set_cps);

/* 0213284C */
BOOL daFan_c::Execute(Mtx34** mtxP) {
    WWHD_FUNC(0x0213284C, BOOL, this, mtxP);
    s16 speed = m_fan_speed(mType);
    f32 len = (f32)(s16)mFanSpeed / (f32)speed;

    if ((mType == 0 && !fopAcM_isSwitch(this, mSwitchNo)) || (mType == 1 && fopAcM_isSwitch(this, mSwitchNo))) {
        cLib_chaseS(&mFanSpeed, speed, 100);
        mWindBtkAnm1.mFrameCtrl.setRate(1.0f); /* setPlaySpeed */
    } else if (mType == 2) {
        len = 1.0f;
        cLib_chaseS(&mFanSpeed, speed, 100);
        mWindBtkAnm1.mFrameCtrl.setRate(1.0f);
    } else {
        cLib_chaseS(&mFanSpeed, 0, 100);
        mWindBtkAnm1.mFrameCtrl.setRate(-1.0f);
    }

    if (mWindSePId != (u32)-1) {
        u32 se = gabi::ea(fopKyM_SearchByID(mWindSePId));
        if (se != 0) {
            /* dLevelSe_c::setReverb(u32 vol, s8 reverb) (HD inline): vol +0xFC, reverb +0x100, flag 4 at +0x101 */
            f32 v = len * 100.0f;
            u32 vol;
            if (v >= 2147483648.0f)
                vol = (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u;
            else
                vol = (u32)gabi::ftoi(v);
            s32 reverb = dComIfGp_getReverb(current.roomNo);
            gabi::store<u32>(se + 0xFC, vol);
            gabi::store<u8>(se + 0x100, (u8)reverb);
            gabi::store<u8>(se + 0x101, gabi::load<u8>(se + 0x101) | 4);
        }
    }

    if (len > 0.1f) {
        set_wind_length(len * len);
        cM3dGCps_Set(cps_shape(), &mCpsS);
        cCcS_Set(dComIfG_Ccsp(), &mCps);
        mWindBtkAnm0.play();
        mWindBtkAnm1.play();
        mWindBckAnm.play();
    }

    set_mtx();
    gabi::store<u32>(gabi::ea(mtxP), gabi::ea(&mBgMtx));
    return TRUE;
}
VERIFY(0x0213284C, &daFan_c::Execute);

/* 02132AF4 */
BOOL daFan_c::Draw() {
    WWHD_FUNC(0x02132AF4, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mWindModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel);
    dComIfGd_setList();
    mWindBckAnm.entry(J3DModel_getModelData(mWindModel), mWindBckAnm.getFrame());
    mDoExt_btkAnm_entryModel(&mWindBtkAnm0, mWindModel, mWindBtkAnm0.getFrame());
    mDoExt_btkAnm_entryModel(&mWindBtkAnm1, mWindModel, mWindBtkAnm1.getFrame());
    mDoExt_modelUpdateDL(mWindModel);
    return TRUE;
}
VERIFY(0x02132AF4, &daFan_c::Draw);

/* 021322F4 */
static cPhs_State daFan_Create(void* i_this) {
    WWHD_FUNC(0x021322F4, cPhs_State, i_this);
    return ((daFan_c*)i_this)->_create();
}
VERIFY(0x021322F4, daFan_Create);

/* 021322F8 */
static BOOL daFan_Delete(void* i_this) {
    WWHD_FUNC(0x021322F8, BOOL, i_this);
    return ((daFan_c*)i_this)->MoveBGDelete();
}
VERIFY(0x021322F8, daFan_Delete);

/* 021322FC: MoveBGDraw() is the virtual Draw (vtable +0x2C) */
static BOOL daFan_Draw(void* i_this) {
    WWHD_FUNC(0x021322FC, BOOL, i_this);
    return ((daFan_c*)i_this)->Draw_v();
}
VERIFY(0x021322FC, daFan_Draw);

/* 0213230C */
static BOOL daFan_Execute(void* i_this) {
    WWHD_FUNC(0x0213230C, BOOL, i_this);
    return ((daFan_c*)i_this)->MoveBGExecute();
}
VERIFY(0x0213230C, daFan_Execute);

/* 02132E9C */
static BOOL daFan_IsDelete(void* i_this) {
    WWHD_FUNC(0x02132E9C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02132E9C, daFan_IsDelete);

/* 02132EB8: this TU's copy of dBgS_MoveBgActor::IsDelete (virtual) */
static BOOL dBgS_MoveBgActor_IsDelete(void* i_this) {
    WWHD_FUNC(0x02132EB8, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02132EB8, dBgS_MoveBgActor_IsDelete);

/* 02132E08: __sinit_d_a_fan_cpp (HD header statics only) */
static void __sinit_d_a_fan_cpp() {
    WWHD_FUNC(0x02132E08, void, (u32)0);
    sinit_header_statics(0x10463CF8, 0x101B4B70);
}
VERIFY(0x02132E08, __sinit_d_a_fan_cpp);

/* 02132EA4: deleting destructor of an empty class (this TU's copy) */
static void deleting_dtor_empty(void* p, s32 flags) {
    WWHD_FUNC(0x02132EA4, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02132EA4, deleting_dtor_empty);

/* 02132EC0: daFan_c::~daFan_c (deleting destructor) */
static void daFan_dtor(daFan_c* p, s32 flags) {
    WWHD_FUNC(0x02132EC0, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x027F3628, gabi::at<u8>(gabi::ea(p) + 0x738), 0); /* mWindBckAnm's anm member destructor */
        gabi::call(0x02515980, &p->mCps, 2);                          /* dCcD_GObjInf::~dCcD_GObjInf */
        dCcD_Stts_dt(&p->mStts, 2);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02132EC0, daFan_dtor);

/* 02132F38: empty virtual (next to 02132EA4 in the same per-TU vtable) */
static void fan_empty_virtual(void* p) {
    WWHD_FUNC(0x02132F38, void, p);
}
VERIFY(0x02132F38, fan_empty_virtual);
