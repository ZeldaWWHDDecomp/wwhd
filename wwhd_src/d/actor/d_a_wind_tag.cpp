/**
 * d_a_wind_tag.cpp (WWHD)
 * Tag - Wind column (Yaflw00 / Ybgaf00) with a point wind, collision and a water splash emitter.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_wind_tag.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x10042ABC /* this TU's sead::SafeString vtable */
#define WINDTAG_VTBL 0x10042B0C
#define FILE_NAME STR(0x10042B4C)
#define l_cps_src 0x101D33A0

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void J3DFrameCtrl_init(void* p, s16 end) { gabi::call(0x027F2BC0, p, end); }
static inline void mDoExt_btkAnm_entryModel(void* a, J3DModel* m, f32 frame) { gabi::call(0x025E8048, a, m, frame); } /* HD: takes the model */
static inline void cM3dGCps_Set(void* cps, void* src) { gabi::call(0x020181FC, cps, src); }
static inline void PSVECSubtract(void* a, void* b, void* out) { gabi::call(0x028E8DAC, a, b, out); }
static inline void dCcD_Cps_Set(void* cps, u32 src) { gabi::call(0x025164C0, cps, src); }
static inline void dPointWind_set_pwind_init(void* pw, void* cps) { gabi::call(0x025AB3F8, pw, cps); }
static inline void dPointWind_set_pwind_move(void* pw) { gabi::call(0x025AB454, pw); }
static inline void dPointWind_set_pwind_delete(void* pw) { gabi::call(0x025AB710, pw); }
static inline u32 fopKyM_create(s16 name, u32 prm, cXyz* pos, cXyz* scale, u32 fn) { return gabi::call<u32>(0x025DADA4, name, prm, pos, scale, fn); }
static inline void* fopKyM_SearchByID(u32 id) { return gabi::call<void*>(0x025DAC50, id); }
static inline BOOL cLib_chasePos(cXyz* pos, cXyz* target, f32 speed) { return gabi::call<BOOL>(0x0200F62C, pos, target, speed); }
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
static inline u32 dKy_get_schbit() { return gabi::call<u32>(0x025602A8); }
static inline f32 dBgS_ObjGndChk_Wtr_Func(cXyz* pos) { return gabi::call<f32>(0x024F1478, pos); }
static inline void cM3d_CalcVecAngle(void* v, be<s16>* x, be<s16>* z) { gabi::call(0x02017264, v, x, z); }
static inline void JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, u32 mtx) { gabi::call(0x028245AC, x, y, z, mtx); }
static inline s32 fopAcM_cullingCheck_call(fopAc_ac_c* a) { return gabi::call<s32>(0x025D6CE8, a); }
static inline s16 fpcLf_GetPriority(void* a) { return gabi::call<s16>(0x025DF2B8, a); }
static inline void fopDwTg_ToDrawQ(u32 tag, s16 prio) { gabi::call(0x025DA874, tag, prio); }
static inline void dKy_tevstr_init_l(void* t, s8 roomNo, u8 p) { gabi::call(0x0255FFF4, t, roomNo, p); }
/* HD material colour helpers (as in d_a_obj_swlight / d_a_obj_ice) */
static inline void color_convert(void* out, void* in, f32 f) { gabi::call(0x0274D458, out, in, f); }
static inline u32 material_uniform(u32 p, s32 n) { return gabi::call<u32>(0x027F9F0C, p, n); }
static inline void matpacket_getTevColor(void* out, u32 packet, s32 idx) { gabi::call(0x027FA160, out, packet, idx); }
/* J3DTevBlock virtuals (vtable at +4 of the block at material+0x18) */
static inline u32 tevblock_vfn(u32 mat, u32 slot) {
    u32 tb = gabi::load<u32>(mat + 0x18);
    return gabi::load<u32>(gabi::load<u32>(tb + 4) + slot);
}
/* J3DModelData::getMaterialNodePointer(i) */
static inline u32 j3d_material(u32 data, u16 i) {
    u32 count = gabi::load<u32>(data + 0xC);
    u32 mat = gabi::load<u32>(data + 0x10);
    if (i < count) mat += i * 0x39C;
    return mat;
}

namespace daWindTag {

struct daWindTag_c : fopAc_ac_c {
    bool _delete();
    BOOL CreateHeap();
    void CreateInit();
    void set_wind_angle();
    cPhs_State _create();
    void set_mtx();
    bool checkSizeSpecialBig();
    void set_wind_se_sub(u32, cXyz*);
    void set_wind_se();
    bool _execute();
    void path_move();
    void set_next_pnt();
    bool _draw();
    void MoveEmitter();

    /* 0x3AC */ u8 mStts[0x3C];       /* dCcD_Stts */
    /* 0x3E8 */ u8 mCps[0x118];       /* dCcD_Cps: dCcD_GObjInf ... */
    /* 0x500 */ u8 mCpsCps[0x20];     /*   its cM3dGCps (start 0x500, end 0x50C) */
    /* 0x520 */ cXyz mCpsS_mStart;    /* cM3dGCpsS mCpsS */
    /* 0x52C */ cXyz mCpsS_mEnd;
    /* 0x538 */ be<f32> mCpsS_mRadius;
    /* 0x53C */ request_of_phase_process_class mPhs;
    /* 0x544 */ gptr<J3DModel> mpModel;
    /* 0x548 */ mDoExt_btkAnm mBtkAnm0;
    /* 0x5BC */ mDoExt_btkAnm mBtkAnm1;
    /* 0x630 */ mDoExt_bckAnm mBckAnm;
    /* 0x6BC */ u8 mPointWind[0x30];  /* dPointWind_c (mWind.mStrength at +0x20) */
    /* 0x6EC */ be<u8> mType;
    /* 0x6ED */ u8 _6ED[3];
    /* 0x6F0 */ be<f32> mOffsY;
    /* 0x6F4 */ be<f32> field_0x49c;
    /* 0x6F8 */ be<u32> mpEmitter;
    /* 0x6FC */ be<u8> mEfColor[4];
    /* 0x700 */ dKy_tevstr_c mEfTevStr;
    /* 0x8C8 */ cXyz mSePos;
    /* 0x8D4 */ cXyz mTargetPos;
    /* 0x8E0 */ be<u32> mpPath;
    /* 0x8E4 */ be<u8> mPathId;
    /* 0x8E5 */ be<s8> mCurPathPoint;
    /* 0x8E6 */ be<s8> mPathPointDir;
    /* 0x8E7 */ be<u8> field_0x577;
    /* 0x8E8 */ be<s32> mSwNo;
    /* 0x8EC */ be<u8> field_0x57c;
    /* 0x8ED */ be<u8> field_0x57d;
    /* 0x8EE */ be<u8> mbDraw;
    /* 0x8EF */ be<u8> field_0x57f;
    /* 0x8F0 */ be<u32> mLevelSeID;
};
WWHD_OFFSET(daWindTag_c, mCpsS_mStart, 0x520);
WWHD_OFFSET(daWindTag_c, mBckAnm, 0x630);
WWHD_OFFSET(daWindTag_c, mType, 0x6EC);
WWHD_OFFSET(daWindTag_c, mEfTevStr, 0x700);
WWHD_OFFSET(daWindTag_c, mLevelSeID, 0x8F0);
WWHD_SIZE(daWindTag_c, 0x8F4);

/* static tables (.data / .rodata) */
static u32 m_arcname(u32 type) { return gabi::load<u32>(0x101D3410 + type * 4); }
static s16 m_bdlidx(u32 type) { return gabi::load<s16>(0x10042BC0 + type * 2); }
static s16 m_heapsize(u32 type) { return gabi::load<s16>(0x10042BC4 + type * 2); }
static s16 m_bckidx(u32 type) { return gabi::load<s16>(0x10042BC8 + type * 2); }
static s16 m_btkidx(u32 type) { return gabi::load<s16>(0x10042BCC + type * 2); }
static s16 m_btkidx2(u32 type) { return gabi::load<s16>(0x10042BD0 + type * 2); }
static f32 mData(u32 i) { return gabi::load<f32>(0x1004BD3C + i * 4); }

namespace daWindTag_prm {
inline u8 getSch(daWindTag_c* i_this) { return fopAcM_GetParam(i_this) & 0xFF; }
inline u8 getPathId(daWindTag_c* i_this) { return (fopAcM_GetParam(i_this) >> 8) & 0xFF; }
inline u8 getSpeed(daWindTag_c* i_this) { return (fopAcM_GetParam(i_this) >> 16) & 0x1F; }
inline u8 getType(daWindTag_c* i_this) { return (fopAcM_GetParam(i_this) >> 21) & 0x03; }
inline u8 getSwType(daWindTag_c* i_this) { return (fopAcM_GetParam(i_this) >> 23) & 0x01; }
inline u8 getSwitchNo(daWindTag_c* i_this) { return (fopAcM_GetParam(i_this) >> 24) & 0xFF; }
inline u8 getSplashFlag(daWindTag_c* i_this) { return (i_this->home.angle.z & 0x0F) == 1; }
inline u8 getDrawFlag(daWindTag_c* i_this) { return (i_this->home.angle.z >> 4) & 0x0F; }
}  // namespace daWindTag_prm

/* sead::SafeString(lit) == the stage name (play+0x5134) (HD inline: cstr() through the vtable,
 * the left operand twice, pointer compare, bounded strcmp) */
static bool isStage(u32 lit) {
    gabi::Local<SafeString> a;
    a->__vtbl = SAFESTRING_VTBL;
    a->mStringTop = lit;
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<SafeString> b;
    b->__vtbl = SAFESTRING_VTBL;
    b->mStringTop = stage;
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 pa = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 pb = b->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        if (ca != gabi::load<u8>(pb + i)) return false;
        if (ca == 0) return true;
    }
    return false;
}

struct ColorF { be<f32> r, g, b, a; };

/* 024DFB00: GXColor (u8 x4) -> normalised float colour (this TU's copy) */
static void color_normalize(ColorF* out, be<u8>* in) {
    WWHD_FUNC(0x024DFB00, void, out, in);
    f32 r = (f32)(u8)in[0] / 255.0f;
    f32 g = (f32)(u8)in[1] / 255.0f;
    f32 b = (f32)(u8)in[2] / 255.0f;
    f32 a = (f32)(u8)in[3] / 255.0f;
    gabi::Local<ColorF> t;
    t->r = r;
    t->g = g;
    t->b = b;
    t->a = a;
    u32 s = gabi::ea(t.get()), d = gabi::ea(out);
    for (u32 i = 0; i < 16; i += 4) gabi::store<u32>(d + i, gabi::load<u32>(s + i));
}
VERIFY(0x024DFB00, color_normalize);

/* GXColorS10 (s16 x4) / 255 as a word-copied float colour */
static void color_s10_normalize(ColorF* out, u32 c) {
    out->r = (f32)gabi::load<s16>(c + 0) / 255.0f;
    out->g = (f32)gabi::load<s16>(c + 2) / 255.0f;
    out->b = (f32)gabi::load<s16>(c + 4) / 255.0f;
    out->a = (f32)gabi::load<s16>(c + 6) / 255.0f;
}

/* writes a converted colour (rgb) and alpha into the material's uniform block */
static void store_uniform(u32 u, ColorF* conv, f32 alpha) {
    f32 r = conv->r, g = conv->g, b = conv->b;
    gabi::store<f32>(u + 4, g);
    gabi::store<f32>(u + 8, b);
    gabi::store<f32>(u + 0, r);
    gabi::store<f32>(u + 0xC, alpha);
}

/* 024DFA88 */
bool daWindTag_c::_delete() {
    WWHD_FUNC(0x024DFA88, bool, this);
    if (mpEmitter != 0) {
        u32 e = mpEmitter; /* becomeInvalidEmitter() */
        gabi::store<s32>(e + 0x5C, -1);
        gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) | 1);
        mpEmitter = 0;
    }

    dComIfG_resDelete(&mPhs, gabi::at<const char>(m_arcname(mType))); /* dComIfG_resDeleteDemo */
    dPointWind_set_pwind_delete(mPointWind);
    return true;
}
VERIFY(0x024DFA88, &daWindTag_c::_delete);

/* 024DF33C */
static BOOL CheckCreateHeap(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x024DF33C, BOOL, i_ac);
    return ((daWindTag_c*)i_ac)->CreateHeap();
}
VERIFY(0x024DF33C, CheckCreateHeap);

/* 024DEE3C */
BOOL daWindTag_c::CreateHeap() {
    WWHD_FUNC(0x024DEE3C, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(gabi::at<const char>(m_arcname(mType)), m_bdlidx(mType), SAFESTRING_VTBL);
    if (modelData == NULL) /* JUT_ASSERT(362, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x16A, STR(0x10042B60));

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000222);
    if (!mpModel)
        return FALSE;

    J3DAnmTextureSRTKey* pbtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(gabi::at<const char>(m_arcname(mType)), m_btkidx(mType), SAFESTRING_VTBL);
    if (pbtk == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0x17A, STR(0x10042B34));
    if (!mBtkAnm0.init(modelData, pbtk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
        return FALSE;

    pbtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(gabi::at<const char>(m_arcname(mType)), m_btkidx2(mType), SAFESTRING_VTBL);
    if (pbtk == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0x186, STR(0x10042B34));
    if (!mBtkAnm1.init(modelData, pbtk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
        return FALSE;

    J3DAnmTransform* pbck = (J3DAnmTransform*)dComIfG_getObjectRes(gabi::at<const char>(m_arcname(mType)), m_bckidx(mType), SAFESTRING_VTBL);
    if (pbck == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0x192, STR(0x10042B40));
    if (!mBckAnm.init(modelData, pbck, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false))
        return FALSE;

    /* HD: on the "sea" stage, each material's TEV colour 2 is taken from its packet and written,
     * converted, into the material's uniform block */
    if (isStage(0x10042B30 /* "sea" */)) {
        for (s32 i = 0; i < gabi::load<u16>(gabi::ea(mpModel) + 0x2A); i++) {
            u32 model = gabi::ea(mpModel);
            u32 data = gabi::load<u32>(model + 0xAC);
            u32 mat = j3d_material(data, (u16)i);
            u32 packets = gabi::load<u32>(model + 0x34);
            gabi::Local<be<s16>[4]> col;
            matpacket_getTevColor(col.get(), packets + i * 0x3C, 2);
            gabi::call_ptr(tevblock_vfn(mat, 0x24), gabi::load<u32>(mat + 0x18), 2, col.get()); /* setTevColor(2, col) */
            gabi::Local<ColorF> norm;
            color_s10_normalize(norm.get(), gabi::ea(col.get()));
            gabi::Local<ColorF> conv;
            color_convert(conv.get(), norm.get(), 1.0f);
            gabi::store<u32>(mat + 0xA0, gabi::load<u32>(mat + 0xA0) | 0x40);
            u32 u = material_uniform(mat + 0xA0, 6);
            store_uniform(u, conv.get(), (f32)gabi::load<s16>(gabi::ea(col.get()) + 6) / 255.0f);
        }
    }
    return TRUE;
}
VERIFY(0x024DEE3C, &daWindTag_c::CreateHeap);

/* 024DF534 */
bool daWindTag_c::checkSizeSpecialBig() {
    WWHD_FUNC(0x024DF534, bool, this);
    if (mType == 1)
        return true;
    return false;
}
VERIFY(0x024DF534, &daWindTag_c::checkSizeSpecialBig);

/* 024DF548 */
void daWindTag_c::CreateInit() {
    WWHD_FUNC(0x024DF548, void, this);
    mbDraw = true;
    if (daWindTag_prm::getDrawFlag(this) == 1) {
        mbDraw = false;
    }

    field_0x57f = false;
    if (daWindTag_prm::getSplashFlag(this) == 1) {
        field_0x57f = true;
    }

    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    f32 maxXZ = mData(mType * 2) * scale.x;
    f32 minXZ = -maxXZ;
    f32 maxY = mData(mType * 2 + 1) * scale.y;
    fopAcM_setCullSizeBox(this, minXZ, 0.0f, minXZ, maxXZ, maxY, maxXZ);
    cullSizeFar = 4.0f; /* fopAcM_setCullSizeFar(this, m_cullsize_far) */

    gabi::call(0x02515F14, mStts, 0xFF, 0xFF, this); /* mStts.Init(0xFF, 0xFF, this) */
    dCcD_Cps_Set(mCps, l_cps_src);
    field_0x49c = scale.y;
    gabi::store<u32>(gabi::ea(this) + 0x42C, gabi::ea(mStts)); /* mCps.SetStts(&mStts) */
    mSwNo = daWindTag_prm::getSwitchNo(this);
    field_0x57c = (u8)fopAcM_isSwitch(this, mSwNo);

    u8 r28 = mSwNo == 0xFF;
    if (daWindTag_prm::getSwType(this) == 0) {
        r28 |= (mSwNo != 0xFF && !fopAcM_isSwitch(this, mSwNo));
    } else {
        r28 |= (mSwNo != 0xFF && fopAcM_isSwitch(this, mSwNo));
    }

    if (r28 != 0) {
        mOffsY = mData(mType * 2 + 1) * field_0x49c;
        mBtkAnm1.mFrameCtrl.setFrame((f32)mBtkAnm1.mFrameCtrl.getEnd());
    } else {
        mOffsY = 0.0f;
        mBtkAnm1.mFrameCtrl.setFrame(0.0f);
    }

    set_wind_angle();
    mPathId = daWindTag_prm::getPathId(this);
    if (mPathId != 0xFF) {
        mpPath = gabi::ea(dPath_GetRoomPath(mPathId, fopAcM_GetRoomNo(this)));
        if (mpPath != 0) {
            mPathPointDir = 1;
            mCurPathPoint = 1;
            /* HD: the point index is the constant 1 */
            u32 pnt = gabi::load<u32>(mpPath + 8) + 1 * 0x10;
            mTargetPos.x = gabi::load<f32>(pnt + 4);
            mTargetPos.y = gabi::load<f32>(pnt + 8);
            mTargetPos.z = gabi::load<f32>(pnt + 0xC);
            current.pos.x = gabi::load<f32>(gabi::load<u32>(mpPath + 8) + 4);
            current.pos.y = gabi::load<f32>(gabi::load<u32>(mpPath + 8) + 8);
            speedF = 10.0f + daWindTag_prm::getSpeed(this);
            current.pos.z = gabi::load<f32>(gabi::load<u32>(mpPath + 8) + 0xC);
        } else {
            mPathId = 0xFF;
        }
    }
    dPointWind_set_pwind_init(mPointWind, &mCpsS_mStart);
    u32 seNum = 0x701D; /* JA_SE_OBJ_WIND_TAG */
    if (checkSizeSpecialBig()) {
        seNum = 0x701E; /* JA_SE_OBJ_WIND_TAG_L */
    }
    mLevelSeID = fopKyM_create(0x18 /* fpcNm_LEVEL_SE_e */, seNum, &eyePos, nullptr, 0);
    mpEmitter = 0;
    f32 efScale = scale.x; /* cXyz efScale(scale.x, scale.x, scale.x) */
    if (field_0x57f) {
        settingTevStruct(dKy_getEnvlight(), 2 /* TEV_TYPE_BG1 */, &current.pos, &mEfTevStr);
        u32 c0 = gabi::ea(&mEfTevStr) + 0x90; /* mColorC0 (GXColorS10) */
        mEfColor[0] = (u8)gabi::load<s16>(c0 + 0);
        mEfColor[1] = (u8)gabi::load<s16>(c0 + 2);
        mEfColor[2] = (u8)gabi::load<s16>(c0 + 4);
        mEfColor[3] = (u8)gabi::load<s16>(c0 + 6);
        s8 roomNo = fopAcM_GetRoomNo(this);
        mpEmitter = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 0, 0x8290 /* ID_AK_SN_AIRFLOWSPLASH00 */, &current.pos,
                                             nullptr, &scale, 0xFF, nullptr, roomNo,
                                             (const GXColor*)mEfColor, nullptr, nullptr));
        if (mpEmitter != 0) {
            u32 e = mpEmitter; /* setGlobalScale(efScale) (HD: two copies) */
            gabi::store<f32>(e + 0x220, efScale);
            gabi::store<f32>(e + 0x224, efScale);
            gabi::store<f32>(e + 0x228, efScale);
            gabi::store<f32>(e + 0x238, efScale);
            gabi::store<f32>(e + 0x23C, efScale);
            gabi::store<f32>(e + 0x240, efScale);
        }
    }

    current.angle.z = 0;
    home.angle.z = 0;
    dKy_tevstr_init_l(&mEfTevStr, fopAcM_GetRoomNo(this), 0xFF);
}
VERIFY(0x024DF548, &daWindTag_c::CreateInit);

/* 024DF4A8 */
void daWindTag_c::set_wind_angle() {
    WWHD_FUNC(0x024DF4A8, void, this);
    mCpsS_mStart.copy(current.pos);
    mCpsS_mRadius = mData(mType * 2) * scale.x;
    set_mtx();
    cM3dGCps_Set(mCpsCps, &mCpsS_mStart); /* mCps.cM3dGCps::Set(mCpsS) */
    /* mCps.CalcAtVec(): at vector = end - start */
    PSVECSubtract(mCpsCps + 0xC, mCpsCps, gabi::at<u8>(gabi::ea(this) + 0x464));
}
VERIFY(0x024DF4A8, &daWindTag_c::set_wind_angle);

/* 024DEBB0: daWindTag_c::daWindTag_c() (fopAcM_ct; GHS allocates when this == NULL) */
static daWindTag_c* daWindTag_ct(daWindTag_c* p) {
    WWHD_FUNC(0x024DEBB0, daWindTag_c*, p);
    if (p == nullptr) {
        p = (daWindTag_c*)operator_new(0x8F4);
        if (p == nullptr)
            return nullptr;
    }
    u32 a = gabi::ea(p);
    fopAc_ac_c_ct(p);
    p->__vtbl = WINDTAG_VTBL;
    /* dCcD_Stts mStts */
    gabi::call(0x0200BD2C, p->mStts);                       /* cCcD_Stts::cCcD_Stts */
    gabi::call(0x02515DA0, gabi::at<u8>(a + 0x3C8));        /* dCcD_GStts::dCcD_GStts */
    gabi::store<u32>(a + 0x3C4, 0x1004AE88);
    gabi::store<u32>(a + 0x3C8, 0x1004AEC0);
    /* dCcD_Cps mCps */
    gabi::call(0x02515FB8, p->mCps);                        /* dCcD_GObjInf::dCcD_GObjInf */
    gabi::store<u32>(a + 0x4FC, 0x100015A8);
    gabi::store<u32>(a + 0x4F8, 0x10042AD4);
    gabi::call(0x02018150, p->mCpsCps);                     /* cM3dGCps::cM3dGCps */
    gabi::store<u32>(a + 0x424, 0x1004AF18);
    gabi::store<u32>(a + 0x4FC, 0x1004AF70);
    gabi::store<u32>(a + 0x518, 0x1004AF60);
    mDoExt_btkAnm::ct(&p->mBtkAnm0);
    mDoExt_btkAnm::ct(&p->mBtkAnm1);
    /* mDoExt_bckAnm mBckAnm */
    J3DFrameCtrl_init(&p->mBckAnm, 0);
    gabi::store<u32>(a + 0x640, 0x1016E54C);
    gabi::call(0x027DA984, gabi::at<u8>(a + 0x644));
    gabi::store<u32>(a + 0x6AC, 0);
    gabi::store<u32>(a + 0x6B0, 0);
    gabi::store<u32>(a + 0x688, 0);
    gabi::store<u32>(a + 0x678, 0x1016D820);
    gabi::store<u32>(a + 0x6B4, 0);
    gabi::store<u32>(a + 0x640, 0x10042AE4);
    gabi::store<u32>(a + 0x6B8, 0);
    /* dKy_tevstr_c mEfTevStr: three lights initialised from the 0x44-byte template 0x1016E414 */
    const u32 T = 0x1016E414;
    f32 f0[6];
    for (int i = 0; i < 6; i++) f0[i] = gabi::load<f32>(T + 4 * i);
    u8 b[4];
    for (int i = 0; i < 4; i++) b[i] = gabi::load<u8>(T + 0x18 + i);
    s16 h[4];
    for (int i = 0; i < 4; i++) h[i] = gabi::load<s16>(T + 0x1C + 2 * i);
    f32 f1[8];
    for (int i = 0; i < 8; i++) f1[i] = gabi::load<f32>(T + 0x24 + 4 * i);
    static const u32 dst[3] = {0x700, 0x7C0, 0x844};
    for (int k = 0; k < 3; k++) {
        u32 d = a + dst[k];
        for (int i = 0; i < 6; i++) gabi::store<f32>(d + 4 * i, f0[i]);
        for (int i = 0; i < 4; i++) gabi::store<u8>(d + 0x18 + i, b[i]);
        for (int i = 0; i < 4; i++) gabi::store<s16>(d + 0x1C + 2 * i, h[i]);
        for (int i = 0; i < 8; i++) gabi::store<f32>(d + 0x24 + 4 * i, f1[i]);
    }
    return p;
}
VERIFY(0x024DEBB0, daWindTag_ct);

/* 024DF9AC */
cPhs_State daWindTag_c::_create() {
    WWHD_FUNC(0x024DF9AC, cPhs_State, this);
    /* fopAcM_ct(this, daWindTag_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            daWindTag_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    mType = daWindTag_prm::getType(this);
    cPhs_State rt = dComIfG_resLoad(&mPhs, gabi::at<const char>(m_arcname(daWindTag_prm::getType(this))));
    if (rt == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x024DF33C /* CheckCreateHeap */, (u32)(s32)m_heapsize(mType)))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return rt;
}
VERIFY(0x024DF9AC, &daWindTag_c::_create);

/* 024DF340 */
void daWindTag_c::set_mtx() {
    WWHD_FUNC(0x024DF340, void, this);
    f32 offsY = mOffsY;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    mDoMtx_stack_c::transM(0.0f, offsY, 0.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(0x101FFBA8) /* cXyz::Zero */, &mCpsS_mEnd);
    if (current.angle.x == 0) {
        mCpsS_mEnd.x = current.pos.x;
        mCpsS_mEnd.z = current.pos.z;
    }
    u32 m = gabi::ea(mpModel); /* setBaseScale(scale) */
    gabi::store<f32>(m + 0xBC, scale.x);
    gabi::store<f32>(m + 0xC0, scale.y);
    gabi::store<f32>(m + 0xC4, scale.z);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x024DF340, &daWindTag_c::set_mtx);

/* 024E0158 */
void daWindTag_c::set_wind_se_sub(u32 p1, cXyz* pos) {
    WWHD_FUNC(0x024E0158, void, this, p1, pos);
    if (mLevelSeID != 0xFFFFFFFF) {
        u32 se = gabi::ea(fopKyM_SearchByID(mLevelSeID));
        if (se != 0) {
            s8 reverb = (s8)dComIfGp_getReverb(fopAcM_GetRoomNo(this));
            /* se->setReverb(100, reverb) */
            gabi::store<s8>(se + 0x100, reverb);
            gabi::store<u32>(se + 0xFC, 100);
            u8 flag = gabi::load<u8>(se + 0x101) | 4;
            gabi::store<u8>(se + 0x101, flag);
            gabi::at<cXyz>(se + 0xE0)->copy(*pos); /* se->mPos = *pos */
            if (mOffsY < 1.0f)
                gabi::store<u8>(se + 0x101, flag | 0x08);
            else
                gabi::store<u8>(se + 0x101, flag & ~0x08);
        }
    }
}
VERIFY(0x024E0158, &daWindTag_c::set_wind_se_sub);

/* 024E0228 */
void daWindTag_c::set_wind_se() {
    WWHD_FUNC(0x024E0228, void, this);
    f32 radius = mData(mType * 2) * scale.x;
    u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8); /* dComIfGp_getCamera(0) */
    cXyz* eye = gabi::at<cXyz>(camera + 0xDC);            /* view.mLookat.mEye */

    gabi::Local<u8[0x20]> cps;
    gabi::call(0x02018150, cps.get()); /* cM3dGCps::cM3dGCps */
    gabi::Local<u8[0x14]> camSph;
    gabi::call(0x02018C40, camSph.get()); /* cM3dGSph::cM3dGSph */
    gabi::Local<cXyz> cross;
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> end;
    start->set(mCpsS_mStart.x, mCpsS_mStart.y, mCpsS_mStart.z);
    end->set(mCpsS_mEnd.x, mCpsS_mEnd.y, mCpsS_mEnd.z);
    gabi::call(0x020181B0, cps.get(), start.get(), end.get(), radius); /* cps.Set(start, end, radius) */
    gabi::call(0x02018E88, camSph.get(), eye, 1.0f);                    /* camSph.Set(eye, 1.0f) */
    if (gabi::call<BOOL>(0x020168D4, cps.get(), camSph.get(), cross.get())) { /* cM3d_Cross_CpsSph */
        mSePos.copy(*eye);
        set_wind_se_sub(0, &mSePos);
    } else {
        gabi::Local<cXyz> e;
        e->set(eye->x, eye->y, eye->z);
        gabi::call(0x020170F4, cps.get(), e.get(), &mSePos); /* cps.NearPos(eye, &mSePos) */
        set_wind_se_sub(0, &mSePos);
    }
    gabi::call(0x0201819C, cps.get(), 2); /* cM3dGCps::~cM3dGCps */
}
VERIFY(0x024E0228, &daWindTag_c::set_wind_se);

/* 024E065C */
bool daWindTag_c::_execute() {
    WWHD_FUNC(0x024E065C, bool, this);
    u32 r26 = dKy_get_schbit();
    u8 r29 = daWindTag_prm::getSch(this);
    speedF = daWindTag_prm::getSpeed(this) + 10.0f;
    f32 f31;
    f32 f30 = 0.1f;

    path_move();

    u32 r27v = r29 & r26;
    u8 r27 = r27v != 0 || r29 == 0;

    u8 r26_2 = mSwNo == 0xFF;
    u8 r24;
    if (daWindTag_prm::getSwType(this) == 0) {
        r24 = mSwNo != 0xFF && !fopAcM_isSwitch(this, mSwNo);
        r26_2 |= r24;
    } else {
        r24 = mSwNo != 0xFF && fopAcM_isSwitch(this, mSwNo);
        r26_2 |= r24;
    }

    if (mSwNo != 0xFF && r29 != 0) {
        if (field_0x57d != 0 && r27v == 0) {
            field_0x577 = 0;
        }
        u32 sw = (u32)fopAcM_isSwitch(this, mSwNo);
        if ((u32)field_0x57c != sw && r24 != 0) {
            field_0x577 = 1;
        }
    }

    be<f32>* strength = gabi::at<be<f32>>(gabi::ea(mPointWind) + 0x20); /* mPointWind.mWind.mStrength */
    if ((r27 != 0 && r26_2 != 0) || field_0x577 != 0) {
        cLib_addCalc(strength, 1.0f, 0.5f, 0.5f, 0.01f);
        f31 = mData(mType * 2 + 1) * field_0x49c;
        mBtkAnm1.mFrameCtrl.setRate(1.0f);
    } else {
        cLib_addCalc(strength, 0.0f, 0.5f, 0.5f, 0.01f);
        f31 = 0.0f;
        mBtkAnm1.mFrameCtrl.setRate(-1.0f);
    }

    set_wind_se();

    f32 len = mData(mType * 2 + 1) * field_0x49c;
    if (daWindTag_prm::getSwType(this) == 0) {
        f32 f3 = len * 0.1f;
        cLib_addCalc(&mOffsY, f31, f30, f3, f3 * 0.5f);
    } else {
        cLib_chaseF(&mOffsY, f31, len / 20.0f);
    }

    set_wind_angle();

    if (mOffsY < 50.0f) {
        fopAcM_offDraw(this);
    } else {
        fopDwTg_ToDrawQ(gabi::ea(this) + 0xDC, fpcLf_GetPriority(this)); /* fopAcM_onDraw */
        cM3dGCps_Set(mCpsCps, &mCpsS_mStart);
        cCcS_Set(dComIfG_Ccsp(), mCps);
        gabi::call(0x02516C14, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), mCps, 2); /* SetMass(&mCps, 2) */
    }

    mBtkAnm0.play();
    mBckAnm.play();
    mBtkAnm1.play();
    field_0x57d = (u8)r27v;
    field_0x57c = (u8)fopAcM_isSwitch(this, mSwNo);
    MoveEmitter();
    dPointWind_set_pwind_move(mPointWind);

    return true;
}
VERIFY(0x024E065C, &daWindTag_c::_execute);

/* 024E0104 */
void daWindTag_c::path_move() {
    WWHD_FUNC(0x024E0104, void, this);
    if (mPathId != 0xFF && cLib_chasePos(&current.pos, &mTargetPos, speedF))
        set_next_pnt();
}
VERIFY(0x024E0104, &daWindTag_c::path_move);

/* 024E0020 */
void daWindTag_c::set_next_pnt() {
    WWHD_FUNC(0x024E0020, void, this);
    if (mPathId == 0xFF)
        return;

    s8 cur = (s8)((u8)mCurPathPoint + (u8)mPathPointDir);
    mCurPathPoint = cur;
    if (gabi::load<u8>(mpPath + 5) & 1) { /* dPath_ChkClose */
        if (cur > (s8)gabi::load<u16>(mpPath) - 1) {
            cur = 0;
            mCurPathPoint = cur;
        } else if (cur < 0) {
            cur = (s8)(gabi::load<u16>(mpPath) - 1);
            mCurPathPoint = cur;
        }
    } else {
        if (cur > gabi::load<u16>(mpPath) - 1) {
            mPathPointDir = -1;
            cur = (s8)(gabi::load<u16>(mpPath) - 2);
            mCurPathPoint = cur;
        } else if (cur < 0) {
            mPathPointDir = 1;
            cur = 1;
            mCurPathPoint = cur;
        }
    }

    u32 pnt = gabi::load<u32>(mpPath + 8) + cur * 0x10;
    mTargetPos.x = gabi::load<f32>(pnt + 4);
    mTargetPos.y = gabi::load<f32>(pnt + 8);
    mTargetPos.z = gabi::load<f32>(pnt + 0xC);
}
VERIFY(0x024E0020, &daWindTag_c::set_next_pnt);

/* 024DFBB4 */
bool daWindTag_c::_draw() {
    WWHD_FUNC(0x024DFBB4, bool, this);
    if (!mbDraw)
        return TRUE;

    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mDoExt_btkAnm_entryModel(&mBtkAnm0, mpModel, mBtkAnm0.mFrameCtrl.getFrame());
    mDoExt_btkAnm_entryModel(&mBtkAnm1, mpModel, mBtkAnm1.mFrameCtrl.getFrame());
    mBckAnm.entry(J3DModel_getModelData(mpModel), mBckAnm.mFrameCtrl.getFrame());

    /* HD: on the "sea" stage, each material gets TEV konst colour 1 with alpha 50 and TEV colour 2
     * from the actor's light colour, both written (converted) into the material's uniform blocks */
    if (isStage(0x10042B8C /* "sea" */)) {
        for (s32 i = 0; i < gabi::load<u16>(gabi::ea(mpModel) + 0x2A); i++) {
            u32 data = gabi::load<u32>(gabi::ea(mpModel) + 0xAC);
            u32 mat = j3d_material(data, (u16)i);
            u32 kcol = gabi::call_ptr<u32>(tevblock_vfn(mat, 0x4C), gabi::load<u32>(mat + 0x18), 1); /* getTevKColor(1) */
            gabi::store<u8>(kcol + 3, 0x32);
            gabi::call_ptr(tevblock_vfn(mat, 0x3C), gabi::load<u32>(mat + 0x18), 1, kcol); /* setTevKColor(1, kcol) */
            gabi::Local<ColorF> norm;
            color_normalize(norm.get(), gabi::at<be<u8>>(kcol));
            gabi::Local<ColorF> conv;
            color_convert(conv.get(), norm.get(), 1.0f);
            gabi::store<u32>(mat + 0xA0, gabi::load<u32>(mat + 0xA0) | 0x100);
            u32 u = material_uniform(mat + 0xA0, 8);
            store_uniform(u, conv.get(), (f32)gabi::load<u8>(kcol + 3) / 255.0f);

            u32 col = gabi::call_ptr<u32>(tevblock_vfn(mat, 0x34), gabi::load<u32>(mat + 0x18), 2); /* getTevColor(2) */
            u32 t = gabi::ea(&tevStr);
            gabi::store<s16>(col + 0, gabi::load<u8>(t + 0x98));
            gabi::store<s16>(col + 2, gabi::load<u8>(t + 0x99));
            gabi::store<s16>(col + 4, gabi::load<u8>(t + 0x9A));
            u32 fn = tevblock_vfn(mat, 0x24);
            f32 f29 = gabi::load<f32>(t + 0x24);
            gabi::call_ptr(fn, gabi::load<u32>(mat + 0x18), 2, col); /* setTevColor(2, col) */
            gabi::Local<ColorF> norm2;
            color_s10_normalize(norm2.get(), col);
            gabi::Local<ColorF> conv2;
            color_convert(conv2.get(), norm2.get(), f29);
            gabi::store<u32>(mat + 0xA0, gabi::load<u32>(mat + 0xA0) | 0x40);
            u = material_uniform(mat + 0xA0, 6);
            store_uniform(u, conv2.get(), (f32)gabi::load<s16>(col + 6) / 255.0f);
        }
    }
    mDoExt_modelUpdateDL(mpModel);

    if (mpEmitter != 0) {
        settingTevStruct(dKy_getEnvlight(), 2 /* TEV_TYPE_BG1 */, &current.pos, &mEfTevStr);
        u32 c0 = gabi::ea(&mEfTevStr) + 0x90;
        u8 r = (u8)gabi::load<s16>(c0 + 0);
        mEfColor[0] = r;
        u8 g = (u8)gabi::load<s16>(c0 + 2);
        u8 b = gabi::load<u8>(c0 + 5);
        mEfColor[1] = g;
        mEfColor[2] = b;
        u32 e = mpEmitter; /* setGlobalPrmColor(r, g, b) */
        gabi::store<u8>(e + 0x244, r);
        gabi::store<u8>(e + 0x245, g);
        gabi::store<u8>(e + 0x246, b);
    }

    return TRUE;
}
VERIFY(0x024DFBB4, &daWindTag_c::_draw);

/* 024E0394 */
void daWindTag_c::MoveEmitter() {
    WWHD_FUNC(0x024E0394, void, this);
    f32 px = current.pos.x, pz = current.pos.z; /* cXyz sp4C = current.pos */
    gabi::Local<csXyz> sp08;
    sp08->x = 0;
    sp08->y = 0;
    sp08->z = 0;
    f32 py = dBgS_GetWaterHeight(&current.pos);
    if (daSea_ChkArea(current.pos.x, current.pos.z)) {
        f32 f30 = daSea_calcWave(current.pos.x - 50.0f, current.pos.z - 50.0f);
        f32 f31 = daSea_calcWave(current.pos.x - 50.0f, current.pos.z + 50.0f);
        f32 f1 = daSea_calcWave(current.pos.x + 50.0f, current.pos.z - 50.0f);
        f32 x = current.pos.x, z = current.pos.z;
        gabi::Local<cXyz> sp40, sp34, sp28;
        sp40->set(x - 50.0f, f30, z - 50.0f);
        sp34->set(x - 50.0f, f31, z + 50.0f);
        sp28->set(x + 50.0f, f1, z - 50.0f);
        gabi::Local<u8[0x3C]> sp58;
        gabi::call(0x020190B8, sp58.get(), sp40.get(), sp34.get(), sp28.get()); /* cM3dGTri::cM3dGTri */
        cM3d_CalcVecAngle(sp58.get() /* GetNP() */, &sp08->x, &sp08->z);
    } else {
        gabi::Local<cXyz> sp1C;
        sp1C->x = current.pos.x;
        sp1C->y = current.pos.y + 200.0f;
        sp1C->z = current.pos.z;
        py = dBgS_ObjGndChk_Wtr_Func(sp1C);
    }

    if (mpEmitter != 0) {
        f32 s = scale.x; /* setGlobalScale(cXyz(scale.x, scale.x, scale.x)) */
        u32 e = mpEmitter;
        gabi::store<f32>(e + 0x224, s);
        gabi::store<f32>(e + 0x238, s);
        gabi::store<f32>(e + 0x240, s);
        gabi::store<f32>(e + 0x23C, s);
        gabi::store<f32>(e + 0x228, s);
        gabi::store<f32>(e + 0x220, s);
        JPAGetXYZRotateMtx(sp08->x, sp08->y, sp08->z, mpEmitter + 0x1F0); /* setGlobalRotation(sp08) */
        if (py == -1000000000.0f) { /* -G_CM3D_F_INF */
            py = current.pos.y;
        }
        e = mpEmitter; /* setGlobalTranslation(sp4C) */
        u8 k = gabi::load<u8>(e + 0x262);
        gabi::store<f32>(e + 0x22C, px);
        gabi::store<f32>(e + 0x234, pz);
        gabi::store<f32>(e + 0x230, py);
        if (k >= 7) { /* HD */
            gabi::store<f32>(e + 0x230, -gabi::load<f32>(e + 0x230));
        }
    }

    cullSizeFar = 2.0f; /* fopAcM_setCullSizeFar(this, m_ef_cullsize_far) */

    if (mOffsY < 100.0f || (actor_condition & 4) || fopAcM_cullingCheck_call(this)) {
        if (mpEmitter != 0) {
            u32 e = mpEmitter;
            gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) | 1); /* stopCreateParticle */
        }
    } else {
        if (mpEmitter != 0) {
            u32 e = mpEmitter;
            gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) & ~1u); /* playCreateParticle */
        }
    }

    cullSizeFar = 4.0f; /* fopAcM_setCullSizeFar(this, m_cullsize_far) */
}
VERIFY(0x024E0394, &daWindTag_c::MoveEmitter);

}  // namespace daWindTag
using daWindTag::daWindTag_c;

/* 024DFA84 */
static cPhs_State daWindTag_Create(void* i_ac) {
    WWHD_FUNC(0x024DFA84, cPhs_State, i_ac);
    return ((daWindTag_c*)i_ac)->_create();
}
VERIFY(0x024DFA84, daWindTag_Create);

/* 024DFAFC */
static BOOL daWindTag_Delete(void* i_ac) {
    WWHD_FUNC(0x024DFAFC, bool, i_ac);
    return ((daWindTag_c*)i_ac)->_delete();
}
VERIFY(0x024DFAFC, daWindTag_Delete);

/* 024E001C */
static BOOL daWindTag_Draw(void* i_ac) {
    WWHD_FUNC(0x024E001C, bool, i_ac);
    return ((daWindTag_c*)i_ac)->_draw();
}
VERIFY(0x024E001C, daWindTag_Draw);

/* 024E0A70 */
static BOOL daWindTag_Execute(void* i_ac) {
    WWHD_FUNC(0x024E0A70, bool, i_ac);
    return ((daWindTag_c*)i_ac)->_execute();
}
VERIFY(0x024E0A70, daWindTag_Execute);

/* 024E0B1C */
static BOOL daWindTag_IsDelete(void*) {
    WWHD_FUNC(0x024E0B1C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024E0B1C, daWindTag_IsDelete);

/* 024E0A74: __sinit_d_a_wind_tag_cpp (compiler-generated: header statics only) */
static void __sinit_d_a_wind_tag_cpp() {
    WWHD_FUNC(0x024E0A74, void);
    sinit_header_statics(0x1046EBE4, 0x101D33EC);
}
VERIFY(0x024E0A74, __sinit_d_a_wind_tag_cpp);

/* 024E0B08: sead::SafeString deleting destructor (this TU's copy, vtable 0x10042ABC) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024E0B08, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024E0B08, SafeString_dt);

/* 024E0B24: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x024E0B24, void, (u32)0);
}
VERIFY(0x024E0B24, SafeString_assureTerminationImpl);

/* 024E0B28: daWindTag_c deleting destructor (vtable 0x10042B0C) */
static void daWindTag_c_dt(daWindTag_c* p, s32 flags) {
    WWHD_FUNC(0x024E0B28, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x027F3628, gabi::at<u8>(gabi::ea(p) + 0x640), 0); /* mBckAnm's member destructor */
        gabi::call(0x02515980, p->mCps, 2);                            /* dCcD_GObjInf::~dCcD_GObjInf */
        gabi::call(0x02515860, p->mStts, 2);                           /* dCcD_Stts::~dCcD_Stts */
        gabi::call(0x025D50BC, p, 0);                                  /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x024E0B28, daWindTag_c_dt);
