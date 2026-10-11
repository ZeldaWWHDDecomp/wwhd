/**
 * d_a_wind_tag.cpp (WWHD)
 * Tag - Wind column (Yaflw00 / Ybgaf00) with a point wind, collision and a
 * water splash emitter.
 *
 * Ported from the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_wind_tag.cpp) to the WWHD layout
 * and code, verified against cking.rpx. "HD:" marks where WWHD's code differs
 * from the GameCube source.
 */
#include "d/actor/d_a_wind_tag.h"

const gc_tu_data gc_tu = {
    .safeString = 0x10042ABC,
    .safeStringAssure = 0x024E0B24,
    .headerStatics = {0x1046EBE4, 0x101D33EC, 0},
};

#define SAFESTRING_VTBL gc_tu.safeString
#define WINDTAG_VTBL WindTagHD::ActorVtable
#define FILE_NAME "d_a_wind_tag.cpp"
static WindTagHD::CapsuleSource l_cps_src = {
    0, 0x200000, 0, 13, 0, 0, 0, 0, 2, 0, 4, 0, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 200.0f,
};

/* J3DTevBlock virtuals (vtable at +4 of the block at material+0x18) */
static inline u32 tevblock_vfn(WindTagHD::Material* mat, u32 slot) {
    WindTagHD::TevBlock* tb = mat->mpTevBlock;
    return tb->__vtbl->slots[slot / 4].get();
}
static inline WindTagHD::Material* j3d_material(WindTagHD::MaterialData* data, u16 i) {
    u32 count = data->mMaterialNum;
    WindTagHD::Material* mat = data->mpMaterials;
    if (i < count)
        mat += i;
    return mat;
}

using namespace daWindTag;

static WindTagHD::LightTemplate initialLight = {
    {0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f},
    {255, 255, 255, 255},
    {255, 255, 255, 255},
    {1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
};

/* static tables (.data / .rodata) */
static const char* m_arcname[] = {"Yaflw00", "Ybgaf00"};
static s16 m_bdlidx[] = {8, 8};
static s16 m_heapsize[] = {3136, 3136};
static s16 m_bckidx[] = {5, 5};
static s16 m_btkidx[] = {11, 11};
static s16 m_btkidx2[] = {12, 12};
static f32 mData[] = {150.0f, 1000.0f, 1000.0f, 7000.0f};

namespace daWindTag_prm {
inline u8 getSch(daWindTag_c* i_this) { return fopAcM_GetParam(i_this) & 0xFF; }
inline u8 getPathId(daWindTag_c* i_this) { return (fopAcM_GetParam(i_this) >> 8) & 0xFF; }
inline u8 getSpeed(daWindTag_c* i_this) { return (fopAcM_GetParam(i_this) >> 16) & 0x1F; }
inline u8 getType(daWindTag_c* i_this) { return (fopAcM_GetParam(i_this) >> 21) & 0x03; }
inline u8 getSwType(daWindTag_c* i_this) { return (fopAcM_GetParam(i_this) >> 23) & 0x01; }
inline u8 getSwitchNo(daWindTag_c* i_this) { return (fopAcM_GetParam(i_this) >> 24) & 0xFF; }
inline u8 getSplashFlag(daWindTag_c* i_this) { return (i_this->home.angle.z & 0x0F) == 1; }
inline u8 getDrawFlag(daWindTag_c* i_this) { return (i_this->home.angle.z >> 4) & 0x0F; }
} // namespace daWindTag_prm

/* sead::SafeString(lit) == the stage name (play+0x5134) (HD inline: cstr()
 * through the vtable, the left operand twice, pointer compare, bounded strcmp)
 */
static bool isStage(const char* lit) {
    gabi::Local<PointWindSafeString> a;
    a->__vtbl.v = gc_tu.safeString;
    a->mStringTop.v = gabi::guest_addr(lit);
    const char* stage = dComIfGp_getStartStageName();
    gabi::Local<PointWindSafeString> b;
    b->__vtbl.v = gc_tu.safeString;
    b->mStringTop = reinterpret_cast<const be<char>*>(stage);
    a->assureTermination();
    a->assureTermination();
    const be<u8>* lhs = reinterpret_cast<const be<u8>*>(a->mStringTop.get());
    b->assureTermination();
    const be<u8>* rhs = reinterpret_cast<const be<u8>*>(b->mStringTop.get());
    if (lhs == rhs)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = lhs[i];
        if (ca != (u8)rhs[i])
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}

using WindTagHD::ColorF;

// HD: GXColor (u8 x4) -> normalised float colour (this TU's copy)
/* 024DFB00 */
static void color_normalize(ColorF* out, be<u8>* in) {
    f32 r = (f32)(u8)in[0] / 255.0f;
    f32 g = (f32)(u8)in[1] / 255.0f;
    f32 b = (f32)(u8)in[2] / 255.0f;
    f32 a = (f32)(u8)in[3] / 255.0f;
    gabi::Local<ColorF> t;
    t->r = r;
    t->g = g;
    t->b = b;
    t->a = a;
    auto* src = reinterpret_cast<WindTagHD::ColorWords*>(t.get());
    auto* dst = reinterpret_cast<WindTagHD::ColorWords*>(out);
    for (u32 i = 0; i < 4; i++)
        dst->rgba[i] = (u32)src->rgba[i];
}

/* GXColorS10 (s16 x4) / 255 as a word-copied float colour */
static void color_s10_normalize(ColorF* out, GXColorS10* c) {
    out->r = (f32)(s16)c->r / 255.0f;
    out->g = (f32)(s16)c->g / 255.0f;
    out->b = (f32)(s16)c->b / 255.0f;
    out->a = (f32)(s16)c->a / 255.0f;
}

/* writes a converted colour (rgb) and alpha into the material's uniform block
 */
static void store_uniform(ColorF* u, ColorF* conv, f32 alpha) {
    f32 r = conv->r, g = conv->g, b = conv->b;
    u->g = g;
    u->b = b;
    u->r = r;
    u->a = alpha;
}

/* 024DFA88 */
bool daWindTag::daWindTag_c::_delete() {
    if (mpEmitter != 0) {
        WindTagHD::Emitter* e = mpEmitter.get(); /* becomeInvalidEmitter() */
        e->mMaxFrame = -1;
        e->mFlags = (u32)e->mFlags | WindTagHD::StopCreateParticle;
        mpEmitter = 0;
    }

    dComIfG_resDelete(&mPhs, m_arcname[mType]); /* dComIfG_resDeleteDemo */
    mPointWind.set_pwind_delete();
    return true;
}

/* 024DF33C */
static BOOL CheckCreateHeap(fopAc_ac_c* i_ac) { return ((daWindTag_c*)i_ac)->CreateHeap(); }

/* 024DEE3C */
BOOL daWindTag::daWindTag_c::CreateHeap() {
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes(m_arcname[mType], m_bdlidx[mType], SAFESTRING_VTBL);
    if (modelData == NULL) /* JUT_ASSERT(362, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x16A, "modelData != (0)");

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000222);
    if (!mpModel)
        return FALSE;

    J3DAnmTextureSRTKey* pbtk =
        (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arcname[mType], m_btkidx[mType], SAFESTRING_VTBL);
    if (pbtk == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0x17A, "pbtk != (0)");
    if (!mBtkAnm0.init(modelData, pbtk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
        return FALSE;

    pbtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arcname[mType], m_btkidx2[mType], SAFESTRING_VTBL);
    if (pbtk == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0x186, "pbtk != (0)");
    if (!mBtkAnm1.init(modelData, pbtk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
        return FALSE;

    J3DAnmTransform* pbck =
        (J3DAnmTransform*)dComIfG_getObjectRes(m_arcname[mType], m_bckidx[mType], SAFESTRING_VTBL);
    if (pbck == NULL)
        JUT_ASSERT_fail(FILE_NAME, 0x192, "pbck != (0)");
    if (!mBckAnm.init(modelData, pbck, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false))
        return FALSE;

    /* HD: on the "sea" stage, each material's TEV colour 2 is taken from its
   * packet and written, converted, into the material's uniform block */
    if (isStage("sea")) {
        for (s32 i = 0; i < reinterpret_cast<WindTagHD::Model*>(mpModel.get())->mMaterialNum; i++) {
            WindTagHD::Model* model = reinterpret_cast<WindTagHD::Model*>(mpModel.get());
            WindTagHD::MaterialData* data = model->mpModelData;
            WindTagHD::Material* mat = j3d_material(data, (u16)i);
            u32 packets = (u32)model->mMatPackets;
            gabi::Local<be<s16>[4]> col;
            matpacket_getTevColor(col.get(), packets + i * 0x3C, 2);
            gabi::call_ptr(tevblock_vfn(mat, WindTagHD::SetTevColor), gabi::ea(mat->mpTevBlock), 2,
                           col.get()); /* setTevColor(2, col) */
            gabi::Local<ColorF> norm;
            color_s10_normalize(norm.get(), reinterpret_cast<GXColorS10*>(col.get()));
            gabi::Local<ColorF> conv;
            color_convert(conv.get(), norm.get(), 1.0f);
            mat->mUniformFlags |= WindTagHD::TevColorUniform;
            ColorF* u = material_uniform(&mat->mUniformFlags, 6);
            store_uniform(u, conv.get(), (f32)(s16)(*col.get())[3] / 255.0f);
        }
    }
    return TRUE;
}

/* 024DF534 */
bool daWindTag::daWindTag_c::checkSizeSpecialBig() {
    if (mType == 1)
        return true;
    return false;
}

/* 024DF548 */
void daWindTag::daWindTag_c::CreateInit() {
    mbDraw = true;
    if (daWindTag_prm::getDrawFlag(this) == 1) {
        mbDraw = false;
    }

    field_0x57f = false;
    if (daWindTag_prm::getSplashFlag(this) == 1) {
        field_0x57f = true;
    }

    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    f32 maxXZ = mData[mType * 2] * scale.x;
    f32 minXZ = -maxXZ;
    f32 maxY = mData[mType * 2 + 1] * scale.y;
    fopAcM_setCullSizeBox(this, minXZ, 0.0f, minXZ, maxXZ, maxY, maxXZ);
    cullSizeFar = 4.0f; /* fopAcM_setCullSizeFar(this, m_cullsize_far) */

    mStts.Init(0xFF, 0xFF, this); /* mStts.Init(0xFF, 0xFF, this) */
    dCcD_Cps_Set(&mCps, gabi::ea(&l_cps_src));
    field_0x49c = scale.y;
    mCps.mpStts = &mStts; /* mCps.SetStts(&mStts) */
    mSwNo = daWindTag_prm::getSwitchNo(this);
    field_0x57c = (u8)fopAcM_isSwitch(this, mSwNo);

    u8 r28 = mSwNo == 0xFF;
    if (daWindTag_prm::getSwType(this) == 0) {
        r28 |= (mSwNo != 0xFF && !fopAcM_isSwitch(this, mSwNo));
    } else {
        r28 |= (mSwNo != 0xFF && fopAcM_isSwitch(this, mSwNo));
    }

    if (r28 != 0) {
        mOffsY = mData[mType * 2 + 1] * field_0x49c;
        mBtkAnm1.mFrameCtrl.setFrame((f32)mBtkAnm1.mFrameCtrl.getEnd());
    } else {
        mOffsY = 0.0f;
        mBtkAnm1.mFrameCtrl.setFrame(0.0f);
    }

    set_wind_angle();
    mPathId = daWindTag_prm::getPathId(this);
    if (mPathId != 0xFF) {
        mpPath = reinterpret_cast<path_layout::dPath*>(dPath_GetRoomPath(mPathId, fopAcM_GetRoomNo(this)));
        if (mpPath != 0) {
            mPathPointDir = 1;
            mCurPathPoint = 1;
            /* HD: the point index is the constant 1 */
            path_layout::dPnt* pnt = &mpPath->m_points[1];
            mTargetPos.x = (f32)pnt->m_position.x;
            mTargetPos.y = (f32)pnt->m_position.y;
            mTargetPos.z = (f32)pnt->m_position.z;
            current.pos.x = (f32)mpPath->m_points[0].m_position.x;
            current.pos.y = (f32)mpPath->m_points[0].m_position.y;
            speedF = 10.0f + daWindTag_prm::getSpeed(this);
            current.pos.z = (f32)mpPath->m_points[0].m_position.z;
        } else {
            mPathId = 0xFF;
        }
    }
    mPointWind.set_pwind_init(&mCpsS);
    u32 seNum = JA_SE_OBJ_WIND_TAG;
    if (checkSizeSpecialBig()) {
        seNum = JA_SE_OBJ_WIND_TAG_L;
    }
    mLevelSeID = fopKyM_create(fpcNm_LEVEL_SE_e, seNum, &eyePos, nullptr, 0);
    mpEmitter = 0;
    f32 efScale = scale.x; /* cXyz efScale(scale.x, scale.x, scale.x) */
    if (field_0x57f) {
        settingTevStruct(dKy_getEnvlight(), 2 /* TEV_TYPE_BG1 */, &current.pos, &mEfTevStr);
        mEfColor[0] = (u8)(s16)mEfTevStr.mColorC0.r;
        mEfColor[1] = (u8)(s16)mEfTevStr.mColorC0.g;
        mEfColor[2] = (u8)(s16)mEfTevStr.mColorC0.b;
        mEfColor[3] = (u8)(s16)mEfTevStr.mColorC0.a;
        s8 roomNo = fopAcM_GetRoomNo(this);
        mpEmitter = reinterpret_cast<WindTagHD::Emitter*>(
            dPa_control_set(dComIfGp_getParticle(), 0, ID_AK_SN_AIRFLOWSPLASH00, &current.pos, nullptr,
                            &scale, 0xFF, nullptr, roomNo, (const GXColor*)mEfColor, nullptr, nullptr));
        if (mpEmitter != 0) {
            WindTagHD::Emitter* e = mpEmitter.get(); /* setGlobalScale(efScale) (HD: two copies) */
            e->mGlobalScale.x = efScale;
            e->mGlobalScale.y = efScale;
            e->mGlobalScale.z = efScale;
            e->mGlobalParticleScale.x = efScale;
            e->mGlobalParticleScale.y = efScale;
            e->mGlobalParticleScale.z = efScale;
        }
    }

    current.angle.z = 0;
    home.angle.z = 0;
    dKy_tevstr_init_l(&mEfTevStr, fopAcM_GetRoomNo(this), 0xFF);
}

/* 024DF4A8 */
void daWindTag::daWindTag_c::set_wind_angle() {
    mCpsS.mStart.copy(current.pos);
    mCpsS.mRadius = mData[mType * 2] * scale.x;
    set_mtx();
    cM3dGCps_Set(&mCps.mCps, &mCpsS); /* mCps.cM3dGCps::Set(mCpsS) */
    /* mCps.CalcAtVec(): at vector = end - start */
    PSVECSubtract(&mCps.mCps.mEnd, &mCps.mCps.mStart, &mCps.mAtVec);
}

static daWindTag_c* daWindTag_ct(daWindTag_c* p);

/* 024DF9AC */
cPhs_State daWindTag::daWindTag_c::_create() {
    /* fopAcM_ct(this, daWindTag_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            daWindTag_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    mType = daWindTag_prm::getType(this);
    cPhs_State rt = dComIfG_resLoad(&mPhs, m_arcname[daWindTag_prm::getType(this)]);
    if (rt == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, (u32)(s32)m_heapsize[mType]))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return rt;
}

/* 024DF340 */
void daWindTag::daWindTag_c::set_mtx() {
    f32 offsY = mOffsY;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    mDoMtx_stack_c::transM(0.0f, offsY, 0.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), &cXyz::Zero, &mCpsS.mEnd);
    if (current.angle.x == 0) {
        mCpsS.mEnd.x = current.pos.x;
        mCpsS.mEnd.z = current.pos.z;
    }
    WindTagHD::Model* model = reinterpret_cast<WindTagHD::Model*>(mpModel.get());
    model->mBaseScale.x = scale.x;
    model->mBaseScale.y = scale.y;
    model->mBaseScale.z = scale.z;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 024E0158 */
void daWindTag::daWindTag_c::set_wind_se_sub(u32 p1, cXyz* pos) {
    if (mLevelSeID != 0xFFFFFFFF) {
        dLevelSe_c* se = reinterpret_cast<dLevelSe_c*>(fopKyM_SearchByID(mLevelSeID));
        if (se != 0) {
            s8 reverb = (s8)dComIfGp_getReverb(fopAcM_GetRoomNo(this));
            /* se->setReverb(100, reverb) */
            se->mReverb = reverb;
            se->mSoundParam = 100;
            u8 flag = (u8)se->mFlag | dLevelSe_c::CUSTOM_REVERB;
            se->mFlag = flag;
            se->mPos.copy(*pos); /* se->mPos = *pos */
            if (mOffsY < 1.0f)
                se->mFlag = flag | dLevelSe_c::STOPPED;
            else
                se->mFlag = flag & ~dLevelSe_c::STOPPED;
        }
    }
}

/* 024E0228 */
void daWindTag::daWindTag_c::set_wind_se() {
    f32 radius = mData[mType * 2] * scale.x;
    auto* camera = reinterpret_cast<WindTagHD::Camera*>(dComIfGp_getCamera(0));
    cXyz* eye = &camera->mEye;

    gabi::Local<u8[0x20]> cps;
    cM3dGCps_ct(cps.get()); /* cM3dGCps::cM3dGCps */
    gabi::Local<u8[0x14]> camSph;
    cM3dGSph_ct(camSph.get()); /* cM3dGSph::cM3dGSph */
    gabi::Local<cXyz> cross;
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> end;
    start->set(mCpsS.mStart.x, mCpsS.mStart.y, mCpsS.mStart.z);
    end->set(mCpsS.mEnd.x, mCpsS.mEnd.y, mCpsS.mEnd.z);
    cM3dGCps_SetStartEnd(cps.get(), start.get(), end.get(), radius); /* cps.Set(start, end, radius) */
    cM3dGSph_SetCenterRadius(camSph.get(), eye, 1.0f);               /* camSph.Set(eye, 1.0f) */
    if (cM3d_Cross_CpsSph(cps.get(), camSph.get(), cross.get())) {   /* cM3d_Cross_CpsSph */
        mSePos.copy(*eye);
        set_wind_se_sub(0, &mSePos);
    } else {
        gabi::Local<cXyz> e;
        e->set(eye->x, eye->y, eye->z);
        cM3dGCps_NearPos(cps.get(), e.get(), &mSePos); /* cps.NearPos(eye, &mSePos) */
        set_wind_se_sub(0, &mSePos);
    }
    cM3dGCps_dt(cps.get(), 2); /* cM3dGCps::~cM3dGCps */
}

/* 024E065C */
bool daWindTag::daWindTag_c::_execute() {
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

    be<f32>* strength = &mPointWind.mWind.mStrength; /* mPointWind.mWind.mStrength */
    if ((r27 != 0 && r26_2 != 0) || field_0x577 != 0) {
        cLib_addCalc(strength, 1.0f, 0.5f, 0.5f, 0.01f);
        f31 = mData[mType * 2 + 1] * field_0x49c;
        mBtkAnm1.mFrameCtrl.setRate(1.0f);
    } else {
        cLib_addCalc(strength, 0.0f, 0.5f, 0.5f, 0.01f);
        f31 = 0.0f;
        mBtkAnm1.mFrameCtrl.setRate(-1.0f);
    }

    set_wind_se();

    f32 len = mData[mType * 2 + 1] * field_0x49c;
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
        fopDwTg_ToDrawQ(gabi::ea(&draw_tag), fpcLf_GetPriority(this)); /* fopAcM_onDraw */
        cM3dGCps_Set(&mCps.mCps, &mCpsS);
        cCcS_Set(dComIfG_Ccsp(), &mCps);
        dCcS_SetMass(reinterpret_cast<WindTagHD::PlayMass*>(dComIfGp_get())->mMass, &mCps,
                     2); /* SetMass(&mCps, 2) */
    }

    mBtkAnm0.play();
    mBckAnm.play();
    mBtkAnm1.play();
    field_0x57d = (u8)r27v;
    field_0x57c = (u8)fopAcM_isSwitch(this, mSwNo);
    MoveEmitter();
    mPointWind.set_pwind_move();

    return true;
}

/* 024E0104 */
void daWindTag::daWindTag_c::path_move() {
    if (mPathId != 0xFF && cLib_chasePos(&current.pos, &mTargetPos, speedF))
        set_next_pnt();
}

/* 024E0020 */
void daWindTag::daWindTag_c::set_next_pnt() {
    if (mPathId == 0xFF)
        return;

    s8 cur = (s8)((u8)mCurPathPoint + (u8)mPathPointDir);
    mCurPathPoint = cur;
    if ((u8)mpPath->m_closed & 1) { /* dPath_ChkClose */
        if (cur > (s8)(u16)mpPath->m_num - 1) {
            cur = 0;
            mCurPathPoint = cur;
        } else if (cur < 0) {
            cur = (s8)((u16)mpPath->m_num - 1);
            mCurPathPoint = cur;
        }
    } else {
        if (cur > (u16)mpPath->m_num - 1) {
            mPathPointDir = -1;
            cur = (s8)((u16)mpPath->m_num - 2);
            mCurPathPoint = cur;
        } else if (cur < 0) {
            mPathPointDir = 1;
            cur = 1;
            mCurPathPoint = cur;
        }
    }

    path_layout::dPnt* pnt = &mpPath->m_points[cur];
    mTargetPos.x = (f32)pnt->m_position.x;
    mTargetPos.y = (f32)pnt->m_position.y;
    mTargetPos.z = (f32)pnt->m_position.z;
}

/* 024DFBB4 */
bool daWindTag::daWindTag_c::_draw() {
    if (!mbDraw)
        return TRUE;

    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mDoExt_btkAnm_entryModel(&mBtkAnm0, mpModel, mBtkAnm0.mFrameCtrl.getFrame());
    mDoExt_btkAnm_entryModel(&mBtkAnm1, mpModel, mBtkAnm1.mFrameCtrl.getFrame());
    mBckAnm.entry(J3DModel_getModelData(mpModel), mBckAnm.mFrameCtrl.getFrame());

    /* HD: on the "sea" stage, each material gets TEV konst colour 1 with alpha 50
   * and TEV colour 2 from the actor's light colour, both written (converted)
   * into the material's uniform blocks */
    if (isStage("sea")) {
        for (s32 i = 0; i < reinterpret_cast<WindTagHD::Model*>(mpModel.get())->mMaterialNum; i++) {
            WindTagHD::MaterialData* data = reinterpret_cast<WindTagHD::Model*>(mpModel.get())->mpModelData;
            WindTagHD::Material* mat = j3d_material(data, (u16)i);
            GXColor* kcol = gabi::call_ptr<GXColor*>(tevblock_vfn(mat, WindTagHD::GetTevKColor),
                                                     gabi::ea(mat->mpTevBlock), 1); /* getTevKColor(1) */
            kcol->a = 0x32;
            gabi::call_ptr(tevblock_vfn(mat, WindTagHD::SetTevKColor), gabi::ea(mat->mpTevBlock), 1,
                           kcol); /* setTevKColor(1, kcol) */
            gabi::Local<ColorF> norm;
            color_normalize(norm.get(), reinterpret_cast<be<u8>*>(kcol));
            gabi::Local<ColorF> conv;
            color_convert(conv.get(), norm.get(), 1.0f);
            mat->mUniformFlags |= WindTagHD::KonstColorUniform;
            ColorF* u = material_uniform(&mat->mUniformFlags, 8);
            store_uniform(u, conv.get(), (f32)(u8)kcol->a / 255.0f);

            GXColorS10* col = gabi::call_ptr<GXColorS10*>(tevblock_vfn(mat, WindTagHD::GetTevColor),
                                                          gabi::ea(mat->mpTevBlock), 2); /* getTevColor(2) */
            col->r = (u8)tevStr.mColorK0.r;
            col->g = (u8)tevStr.mColorK0.g;
            col->b = (u8)tevStr.mColorK0.b;
            u32 fn = tevblock_vfn(mat, WindTagHD::SetTevColor);
            f32 f29 = reinterpret_cast<WindTagHD::LightTemplate*>(tevStr.mLightObj)->mAttenuation[0];
            gabi::call_ptr(fn, gabi::ea(mat->mpTevBlock), 2, col); /* setTevColor(2, col) */
            gabi::Local<ColorF> norm2;
            color_s10_normalize(norm2.get(), col);
            gabi::Local<ColorF> conv2;
            color_convert(conv2.get(), norm2.get(), f29);
            mat->mUniformFlags |= WindTagHD::TevColorUniform;
            u = material_uniform(&mat->mUniformFlags, 6);
            store_uniform(u, conv2.get(), (f32)(s16)col->a / 255.0f);
        }
    }
    mDoExt_modelUpdateDL(mpModel);

    if (mpEmitter != 0) {
        settingTevStruct(dKy_getEnvlight(), 2 /* TEV_TYPE_BG1 */, &current.pos, &mEfTevStr);
        u8 r = (u8)(s16)mEfTevStr.mColorC0.r;
        mEfColor[0] = r;
        u8 g = (u8)(s16)mEfTevStr.mColorC0.g;
        u8 b = (u8)(s16)mEfTevStr.mColorC0.b;
        mEfColor[1] = g;
        mEfColor[2] = b;
        WindTagHD::Emitter* e = mpEmitter.get(); /* setGlobalPrmColor(r, g, b) */
        e->mPrmColor[0] = r;
        e->mPrmColor[1] = g;
        e->mPrmColor[2] = b;
    }

    return TRUE;
}

/* 024E0394 */
void daWindTag::daWindTag_c::MoveEmitter() {
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
        cM3dGTri_ct(sp58.get(), sp40.get(), sp34.get(), sp28.get()); /* cM3dGTri::cM3dGTri */
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
        WindTagHD::Emitter* e = mpEmitter.get();
        e->mGlobalScale.y = s;
        e->mGlobalParticleScale.x = s;
        e->mGlobalParticleScale.z = s;
        e->mGlobalParticleScale.y = s;
        e->mGlobalScale.z = s;
        e->mGlobalScale.x = s;
        JPAGetXYZRotateMtx(sp08->x, sp08->y, sp08->z,
                           gabi::ea(&mpEmitter.get()->mGlobalRotation)); /* setGlobalRotation(sp08) */
        if (py == -1000000000.0f) {                                      /* -G_CM3D_F_INF */
            py = current.pos.y;
        }
        e = mpEmitter.get(); /* setGlobalTranslation(sp4C) */
        u8 k = (u8)e->mDrawTimes;
        e->mGlobalTranslation.x = px;
        e->mGlobalTranslation.z = pz;
        e->mGlobalTranslation.y = py;
        if (k >= 7) { /* HD */
            e->mGlobalTranslation.y = -(f32)e->mGlobalTranslation.y;
        }
    }

    cullSizeFar = 2.0f; /* fopAcM_setCullSizeFar(this, m_ef_cullsize_far) */

    if (mOffsY < 100.0f || (actor_condition & 4) || fopAcM_cullingCheck_call(this)) {
        if (mpEmitter != 0) {
            WindTagHD::Emitter* e = mpEmitter.get();
            e->mFlags = (u32)e->mFlags | WindTagHD::StopCreateParticle; /* stopCreateParticle */
        }
    } else {
        if (mpEmitter != 0) {
            WindTagHD::Emitter* e = mpEmitter.get();
            e->mFlags = (u32)e->mFlags & ~(u32)WindTagHD::StopCreateParticle; /* playCreateParticle */
        }
    }

    cullSizeFar = 4.0f; /* fopAcM_setCullSizeFar(this, m_cullsize_far) */
}

using daWindTag::daWindTag_c;

/* 024DFA84 */
static cPhs_State daWindTag_Create(void* i_ac) { return ((daWindTag_c*)i_ac)->_create(); }

/* 024DFAFC */
static bool daWindTag_Delete(void* i_ac) { return ((daWindTag_c*)i_ac)->_delete(); }

/* 024E001C */
static bool daWindTag_Draw(void* i_ac) { return ((daWindTag_c*)i_ac)->_draw(); }

/* 024E0A70 */
static bool daWindTag_Execute(void* i_ac) { return ((daWindTag_c*)i_ac)->_execute(); }

/* 024E0B1C */
static BOOL daWindTag_IsDelete(void*) { return TRUE; }

// HD: compiler-generated and template code of this translation unit

// HD: daWindTag::daWindTag_c::daWindTag_c() (fopAcM_ct; GHS allocates when this
// == NULL)
/* 024DEBB0 */
static daWindTag_c* daWindTag_ct(daWindTag_c* p) {
    if (p == nullptr) {
        p = (daWindTag_c*)operator_new(0x8F4);
        if (p == nullptr)
            return nullptr;
    }
    fopAc_ac_c_ct(p);
    p->__vtbl = WINDTAG_VTBL;
    /* dCcD_Stts mStts */
    cCcD_Stts_ct(&p->mStts);               /* cCcD_Stts::cCcD_Stts */
    dCcD_GStts_ct(&p->mStts.__vtbl_gstts); /* dCcD_GStts::dCcD_GStts */
    p->mStts.__vtbl = WindTagHD::CollisionStatusVtable;
    p->mStts.__vtbl_gstts = WindTagHD::GameCollisionStatusVtable;
    /* dCcD_Cps mCps */
    dCcD_GObjInf_ct(&p->mCps); /* dCcD_GObjInf::dCcD_GObjInf */
    p->mCps.mShapeVtable = WindTagHD::ShapeBaseVtable;
    p->mCps.mAabVtable = WindTagHD::AabVtable;
    cM3dGCps_ct(&p->mCps.mCps); /* cM3dGCps::cM3dGCps */
    p->mCps.mHitInfoVtable = WindTagHD::HitInfoVtable;
    p->mCps.mShapeVtable = WindTagHD::CapsuleShapeVtable;
    p->mCps.mCps.__vtbl = WindTagHD::CapsuleVtable;
    mDoExt_btkAnm::ct(&p->mBtkAnm0);
    mDoExt_btkAnm::ct(&p->mBtkAnm1);
    /* mDoExt_bckAnm mBckAnm */
    auto* bck = reinterpret_cast<WindTagHD::BckConstruction*>(&p->mBckAnm);
    J3DFrameCtrl_init(&p->mBckAnm, 0);
    bck->mVtable = WindTagHD::BaseAnimationVtable;
    J3DAnmTransform_ct(bck->mTransformCache);
    bck->mCacheCount = 0;
    bck->mData = 0;
    bck->mCacheData = 0;
    bck->mCacheVtable = WindTagHD::TransformCacheVtable;
    bck->mResource = 0;
    bck->mVtable = WindTagHD::BckAnimationVtable;
    bck->mFlags = 0;
    /* dKy_tevstr_c mEfTevStr: three lights initialised from the 0x44-byte
   * template 0x1016E414 */

    f32 f0[6];
    for (int i = 0; i < 6; i++)
        f0[i] = (f32)initialLight.mPositionDirection[i];
    u8 b[4];
    for (int i = 0; i < 4; i++)
        b[i] = (u8)initialLight.mColor[i];
    s16 h[4];
    for (int i = 0; i < 4; i++)
        h[i] = (s16)initialLight.mSpot[i];
    f32 f1[8];
    for (int i = 0; i < 8; i++)
        f1[i] = (f32)initialLight.mAttenuation[i];
    WindTagHD::LightTemplate* dst[3] = {
        reinterpret_cast<WindTagHD::LightTemplate*>(p->mEfTevStr.mLightObj),
        reinterpret_cast<WindTagHD::LightTemplate*>(p->mEfTevStr.mHdLightParam),
        reinterpret_cast<WindTagHD::LightTemplate*>(p->mEfTevStr.mHdLightParam + 0x84)};
    for (int k = 0; k < 3; k++) {
        WindTagHD::LightTemplate* d = dst[k];
        for (int i = 0; i < 6; i++)
            d->mPositionDirection[i] = f0[i];
        for (int i = 0; i < 4; i++)
            d->mColor[i] = b[i];
        for (int i = 0; i < 4; i++)
            d->mSpot[i] = h[i];
        for (int i = 0; i < 8; i++)
            d->mAttenuation[i] = f1[i];
    }
    return p;
}

// HD: __sinit_d_a_wind_tag_cpp (compiler-generated: header statics only)
/* 024E0A74 */
static void __sinit_d_a_wind_tag_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

// HD: sead::SafeString deleting destructor (this TU's copy, vtable 0x10042ABC)
/* 024E0B08 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
/* 024E0B24 */
static void SafeString_assureTerminationImpl(void*) {}

// HD: daWindTag_c deleting destructor (vtable WindTagHD::ActorVtable)
/* 024E0B28 */
static void daWindTag_c_dt(daWindTag_c* p, s32 flags) {
    if (p != nullptr) {
        J3DAnmTransform_dt(&reinterpret_cast<WindTagHD::BckConstruction*>(&p->mBckAnm)->mVtable,
                           0);        /* mBckAnm's member destructor */
        dCcD_GObjInf_dt(&p->mCps, 2); /* dCcD_GObjInf::~dCcD_GObjInf */
        dCcD_Stts_dt(&p->mStts, 2);   /* dCcD_Stts::~dCcD_Stts */
        fopAc_ac_c_dt(p, 0);          /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
data l_cps_src 101D33A0
str 10042B30 in 024DEE3C
str 10042B8C in 024DFBB4
str 10042B4C
str 10042B60
str 10042B34
str 10042B40
data initialLight 1016E414
ext 025D50BC void fopAc_ac_c_dt(void* p, s32 flags)
ext 02515860 void dCcD_Stts_dt(void* p, s32 flags)
ext 02515980 void dCcD_GObjInf_dt(void* p, s32 flags)
ext 027F3628 void J3DAnmTransform_dt(void* p, s32 flags)
ext 020190B8 void cM3dGTri_ct(void* p, cXyz* a, cXyz* b, cXyz* c)
ext 02516C14 void dCcS_SetMass(void* p, void* shape, s32 mass)
ext 0201819C void cM3dGCps_dt(void* p, s32 flags)
ext 020170F4 void cM3dGCps_NearPos(void* p, cXyz* eye, cXyz* nearest)
ext 020168D4 BOOL cM3d_Cross_CpsSph(void* cps, void* sph, cXyz* cross)
ext 02018E88 void cM3dGSph_SetCenterRadius(void* p, cXyz* center, f32 radius)
ext 020181B0 void cM3dGCps_SetStartEnd(void* p, cXyz* start, cXyz* end, f32 radius)
ext 02018C40 void cM3dGSph_ct(void* p)
ext 027DA984 void J3DAnmTransform_ct(void* p)
ext 02018150 void cM3dGCps_ct(void* p)
ext 02515FB8 void dCcD_GObjInf_ct(void* p)
ext 02515DA0 void dCcD_GStts_ct(void* p)
ext 0200BD2C void cCcD_Stts_ct(void* p)
ext 025AB3F8 void dPointWind_c::set_pwind_init(PointWind::cM3dGCpsS* pCps)
ext 025AB454 void dPointWind_c::set_pwind_move()
ext 025AB710 void dPointWind_c::set_pwind_delete()
data m_arcname 101D3410
data m_bdlidx 10042BC0
data m_heapsize 10042BC4
data m_bckidx 10042BC8
data m_btkidx 10042BCC
data m_btkidx2 10042BD0
data mData 1004BD3C
ext 027F2BC0 void J3DFrameCtrl_init(void* p, s16 end)
ext 025E8048 void mDoExt_btkAnm_entryModel(void* a, J3DModel* m, f32 frame)
ext 020181FC void cM3dGCps_Set(void* cps, void* src)
ext 028E8DAC void PSVECSubtract(void* a, void* b, void* out)
ext 025164C0 void dCcD_Cps_Set(void* cps, u32 src)
ext 025DADA4 u32 fopKyM_create(s16 name, u32 prm, cXyz* pos, cXyz* scale, u32 fn)
ext 025DAC50 void* fopKyM_SearchByID(u32 id)
ext 0200F62C BOOL cLib_chasePos(cXyz* pos, cXyz* target, f32 speed)
ext 0200ECD4 f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep)
ext 025602A8 u32 dKy_get_schbit()
ext 024F1478 f32 dBgS_ObjGndChk_Wtr_Func(cXyz* pos)
ext 02017264 void cM3d_CalcVecAngle(void* v, be<s16>* x, be<s16>* z)
ext 028245AC void JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, u32 mtx)
ext 025D6CE8 s32 fopAcM_cullingCheck_call(fopAc_ac_c* a)
ext 025DF2B8 s16 fpcLf_GetPriority(void* a)
ext 025DA874 void fopDwTg_ToDrawQ(u32 tag, s16 prio)
ext 0255FFF4 void dKy_tevstr_init_l(void* t, s8 roomNo, u8 p)
ext 0274D458 void color_convert(void* out, void* in, f32 f)
ext 027F9F0C WindTagHD::ColorF* material_uniform(void* p, s32 n)
ext 027FA160 void matpacket_getTevColor(void* out, u32 packet, s32 idx)
*/
