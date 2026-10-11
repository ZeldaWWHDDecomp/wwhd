/**
 * d_a_hookshot.cpp (WWHD)
 * Item - Hookshot
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_hookshot.cpp) to the WWHD layout and code, verified against
 * cking.rpx under documented verification contracts. "HD:" marks GameCube differences.
 *
 * HD: the chain is drawn by a new renderer: daHookshot_shape grew from a 0x10-byte J3DPacket to
 * 0xD0A0 bytes (vertex buffers, a material, up to 300 chain links of 0xA8 bytes at +0x980), and
 * an HD object of 0xB0 bytes follows it at 0xD44C. The GameCube members follow at 0xD4FC
 * (GameCube offset + 0xD25C). The shape's constructor (0217718C), destructor (02177FD4) and GX2
 * draw (021783F4) are reconstructed in d_a_hookshot_shape.cpp with explicit proof scopes.
 */
#include "d/actor/d_a_hookshot.h"
namespace {
struct HookshotLinkage {
    u8 bytes[32];
};
// Every live guest local keeps the incoming EABI linkage below its own storage,
// including calls made by shared bindings.
template <class T> struct HookshotLocal {
    gabi::Local<T> value;
    gabi::Local<HookshotLinkage> linkage;
    T* get() const { return value.get(); }
    T* operator->() const { return get(); }
    T& operator*() const { return *get(); }
    operator T*() const { return get(); }
};
} // namespace

#define SAFESTRING_VTBL 0x10011560 /* this TU's sead::SafeString vtable */
#define HOOKSHOT_VTBL 0x100116A8   /* HD: daHookshot_c vtable */
static HookshotCpsSource l_at_cps_src;
static HookshotJ3DSysView l_j3dSys;
/* g_Counter.mTimer */
static u32 l_chainTimer;

enum {
    JA_SE_LK_HS_CHAIN = 0x206C,
    JA_SE_LK_HS_WIND_UP = 0x206D,
    JA_SE_LK_HS_WIND_UP_FIN = 0x286E,
    JA_SE_LK_HS_SPIKE = 0x286F,
    JA_SE_LK_HS_REBOUND = 0x287B,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 028E8DE8 PSVECSquareDistance(a, b) */
/* 028E8DAC PSVECSubtract(a, b, out) */
/* daPy_lk_c::getHookshotRootPos(): +0x72F0 (HD) */
static inline cXyz* link_hookshotRootPos(fopAc_ac_c* link) {
    return &hookshotView<HookshotPlayerView>(gabi::ea(link))->mHookshotRootPos;
}
/* daPy_lk_c::getBodyAngleX/Y (HD): +0x3D0 / +0x3D2 */
static inline u16 link_bodyAngleX(fopAc_ac_c* link) {
    return hookshotView<HookshotPlayerView>(gabi::ea(link))->mBodyAngleX;
}
static inline s16 link_bodyAngleY(fopAc_ac_c* link) {
    return hookshotView<HookshotPlayerView>(gabi::ea(link))->mBodyAngleY;
}
/* hookshotStayRoom(): s8 at 0x1047E6C8 */
static u8 l_stayRoom;
static inline u8 hookshotStayRoom() { return l_stayRoom; }
/* 024EF398 dBgS::ChkPolyHSStick (HD: out of line; the matcher calls it GetPolyId2) */
/* 024EF0F4 dBgS::GetAttributeCode(cBgS_PolyInfo&) */
/* 02008254 cBgS::ChkPolySafe, 024EEABC dBgS::ChkMoveBG, 024EFA38 dBgS::MoveBgTransPos */
/* 02017300 cM3d_CalcVecZAngle(const Vec&, csXyz*) */
/* dCcD_Cps: cM3dGCps::Set(start, end, r) (020181B0 on the cps at +0x118) + CalcAtVec (At vec +0x7C)
 */
static inline void cps_SetStartEnd(u32 cps, cXyz* s, cXyz* e, f32 r) {
    hs_setCpsEndpoints(cps + 0x118, s, e, r);
}
static inline void cps_CalcAtVec(u32 cps) {
    PSVECSubtract(gabi::at<cXyz>(cps + 0x124), gabi::at<cXyz>(cps + 0x118),
                  gabi::at<cXyz>(cps + 0x7C));
}
/* dCcMassS_Mng::Set (play+0x4EF8) */
static inline void dComIfG_Ccsp_SetMass(u32 obj, u8 type) {
    hs_setCollisionMass(dComIfGp_ea() + 0x4EF8, obj, type);
}
/* sead::SafeString equality (HD inline, as in d_a_kb) */
static inline bool SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b);
    if (s1 == b->mStringTop)
        return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i))
            return false;
        if (c == 0)
            return true;
    }
    return false;
}
static inline bool hookshotIsStartStage(const char* lit) {
    HookshotLocal<SafeString> a;
    a->mStringTop = gabi::guest_addr(lit);
    a->__vtbl = SAFESTRING_VTBL;
    HookshotLocal<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134;
    b->__vtbl = SAFESTRING_VTBL;
    return SafeString_eq(a, b);
}

static inline void fopAcM_SetParam(fopAc_ac_c* a, u32 v) { a->mParameters = v; }

static const dBgS_LinChk_vt LINCHK_VT = {0x10011668, 0x10011678, 0x10011698, 0x10011688};

static const dBgS_GndChk_vt GNDCHK_VT = {0x100115E8, 0x100115F8, 0x10011618, 0x10011608};

#define PROC_WAIT 0x021789B4u
#define PROC_SHOT 0x02178DDCu
#define PROC_PULL 0x02179888u
#define PROC_RETURN 0x02179B98u

// HD: Mtx returned by value (dst = src through FPRs)
/* 0217674C */
static void mtx_copy_value(Mtx34* dst, const Mtx34* src) {
    f32 v[12];
    for (int i = 0; i < 12; i++)
        v[i] = hookshotView<HookshotFloatMatrix>(gabi::ea(src))->m[i];
    for (int i = 0; i < 12; i++)
        hookshotView<HookshotFloatMatrix>(gabi::ea(dst))->m[i] = v[i];
}

// HD: GXColorS10 -> four floats / 255 (returned by value)
/* 021767EC */
static void colorS10_toF(f32* out, const s16* c) {
    auto* color = hookshotView<HookshotSignedColor>(gabi::ea(c));
    auto* result = hookshotView<HookshotFloatColor>(gabi::ea(out));
    f32 r = (f32)color->r / 255.0f;
    f32 g = (f32)color->g / 255.0f;
    f32 b = (f32)color->b / 255.0f;
    f32 a = (f32)color->a / 255.0f;
    result->r = r;
    result->g = g;
    result->b = b;
    result->a = a;
}

// daHookshot_shape::draw (HD: fills the chain links' matrices and the material colours;
// the GX2 draw itself is the virtual 021783F4)
/* 021768B0 */
static void daHookshot_shape_draw(u8* shape) {
    u32 sh = gabi::ea(shape);
    daHookshot_c* hookshot =
        gabi::at<daHookshot_c>(hookshotView<HookshotShapeView>(sh)->mUserArea); /* getUserArea() */
    u32 hs = gabi::ea(hookshot);
    s32 chain_count = hookshot->mChainCnt;
    if (chain_count <= 0)
        return;

    hs_prepareDraw();
    HookshotLocal<Mtx34> viewMtx;
    mtx_copy_value(viewMtx, &l_j3dSys.mViewMtx); /* j3dSys.getViewMtx() */
    u32 r5 = l_j3dSys.mDrawData;
    hs_setupMaterial(sh + 0x564, 0, viewMtx.get(), gabi::ea(l_j3dSys.mDrawContext), r5 + 0x240);

    u32 mat = hookshotView<HookshotShapeView>(sh)->mUniforms;
    HookshotLocal<f32[4]> c;
    colorS10_toF(*c, reinterpret_cast<s16*>(&hookshot->tevStr.mColorC0)); /* tevStr.mColorC0 */
    HookshotLocal<f32[4]> c1;
    hs_scaleColor(c1.get(), c.get(), hookshotView<HookshotActorRenderView>(hs)->mColorScale);
    for (int i = 0; i < 4; i++)
        hookshotView<HookshotMaterialView>(mat)->mColor[i] =
            hookshotView<be<u32>>(gabi::ea(c1.get()))[i];
    mat = hookshotView<HookshotShapeView>(sh)->mUniforms;
    colorS10_toF(
        *c, reinterpret_cast<s16*>(&hookshotView<HookshotActorRenderView>(hs)->mSecondaryColor));
    HookshotLocal<f32[4]> c2;
    hs_scaleColor(c2.get(), c.get(), hookshotView<HookshotActorRenderView>(hs)->mSecondaryScale);
    for (int i = 0; i < 4; i++)
        hookshotView<HookshotMaterialView>(mat)->mSecondaryColor[i] =
            hookshotView<be<u32>>(gabi::ea(c2.get()))[i];
    hs_flushMaterial(sh + 0x564, 0);

    /* tevStr.mColorC0 (s16) and mColorK0 (u8) as floats / 255 */
    f32 r = (f32)hookshot->tevStr.mColorC0.r / 255.0f;
    f32 g = (f32)hookshot->tevStr.mColorC0.g / 255.0f;
    f32 b = (f32)hookshot->tevStr.mColorC0.b / 255.0f;
    f32 a = (f32)hookshot->tevStr.mColorC0.a / 255.0f;
    hookshotView<HookshotShapeView>(sh)->mTevColor[1] = g;
    hookshotView<HookshotShapeView>(sh)->mTevColor[2] = b;
    hookshotView<HookshotShapeView>(sh)->mTevColor[0] = r;
    hookshotView<HookshotShapeView>(sh)->mTevColor[3] = a;
    f32 kr = (f32)hookshot->tevStr.mColorK0.r / 255.0f;
    f32 kg = (f32)hookshot->tevStr.mColorK0.g / 255.0f;
    f32 kb = (f32)hookshot->tevStr.mColorK0.b / 255.0f;
    f32 ka = (f32)hookshot->tevStr.mColorK0.a / 255.0f;
    hookshotView<HookshotShapeView>(sh)->mAmbientColor[1] = kg;
    hookshotView<HookshotShapeView>(sh)->mAmbientColor[0] = kr;
    hookshotView<HookshotShapeView>(sh)->mAmbientColor[2] = kb;
    hookshotView<HookshotShapeView>(sh)->mAmbientColor[3] = ka;
    hs_scaleAmbient(sh + 0x6DC, hookshotView<HookshotActorRenderView>(hs)->mAmbientScale);
    u32 m2 = hookshotView<HookshotShapeView>(sh)->mUniforms;
    for (int i = 0; i < 4; i++)
        hookshotView<HookshotShapeView>(sh)->mMaterialColor[i] =
            hookshotView<HookshotMaterialView>(m2)->mColor[i];
    hs_flushLink(sh + 0x618);

    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    HookshotLocal<cXyz> chain_pos;
    cXyz* root = link_hookshotRootPos(link);
    chain_pos->x = root->x;
    chain_pos->y = root->y;
    chain_pos->z = root->z;
    u16 sx = static_cast<u16>(hookshot->shape_angle.x),
        sy = static_cast<u16>(hookshot->shape_angle.y);
    HookshotLocal<cXyz> chain_offset;
    f32 cx = 7.0f * cM_scos(sx);
    chain_offset->x = cx * cM_ssin(sy);
    chain_offset->y = -7.0f * cM_ssin(sx);
    chain_offset->z = cx * cM_scos(sy);
    u32 timer = l_chainTimer;
    s16 z = (s16)((timer << 12) | ((timer >> 4) & 0xFFF));
    for (s32 i = 0; i < chain_count; i++) {
        u32 lnk = sh + 0x980 + i * 0xA8;
        PSMTXTrans(mDoMtx_stack_c::get(), chain_pos->x, chain_pos->y, chain_pos->z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), hookshot->shape_angle.x, hookshot->shape_angle.y, z);
        u32 t = l_chainTimer;
        z = (s16)(z + (((((t + i) << 12) | ((t >> ((i & 0xF) + 1)) & 0xFFF))) & 0x1FFF) + 0x3000);
        HookshotLocal<Mtx34> m;
        mtx_copy_value(m, mDoMtx_stack_c::get());
        PSMTXCopy(m, &hookshotView<HookshotRenderLinkView>(lnk)->mMatrix);
        hs_flushLink(lnk);
        PSVECAdd(chain_pos, chain_offset, chain_pos);
    }
}

// HD: the shape is entered, then an HD object and the chain matrices are updated
/* 02176C50 */
BOOL daHookshot_c::draw() {
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    hs_entryImm(gabi::load<u32>(dComIfGp_ea() + 0x5D58), mShape,
                0); /* dComIfGd_getOpaListP1()->entryImm */
    u32 p = gabi::ea(this) + 0xD44C;
    hookshotView<HookshotRenderObject>(p)->mTevStr = gabi::ea(&tevStr);
    hs_setupPlayerRender(dComIfGp_getLinkPlayer(), p, mShape);
    daHookshot_shape_draw(mShape);
    return TRUE;
}

/* 02176CDC */
static BOOL daHookshot_Draw(daHookshot_c* i_this) { return i_this->draw(); }

/* 02176CE0 */
static void daHookshot_rockLineCallback(fopAc_ac_c* hookshot_actor, dCcD_GObjInf* objInf,
                                        fopAc_ac_c* collided_actor, dCcD_GObjInf* unusedObjInf) {
    daHookshot_c* i_this = (daHookshot_c*)hookshot_actor;
    cXyz* hitPos = &objInf->mGObjAt.mHitPos; /* GetAtHitPosP() */
    f32 f31 = PSVECSquareDistance(&i_this->mObjSightCrossPos, &i_this->current.pos);
    f32 f1 = PSVECSquareDistance(hitPos, &i_this->current.pos);
    if (f31 > f1) {
        i_this->mObjSightCrossPos.copy(*hitPos);
        /* HD (USA): fopAcStts_UNK80000_e | UNK200000_e | UNK10000000_e */
        i_this->mObjHookFlg =
            (collided_actor->actor_status &
             (fopAcStts_UNK80000_e | fopAcStts_UNK200000_e | fopAcStts_UNK10000000_e)) != 0;
    }
}

// HD: the chain count is clamped to 300; the water effect is a free function
/* 02176D98 */
BOOL daHookshot_c::execute() {
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    if (fopAcM_GetParam(this) != Mode_Wait) {
        mDoAud_seStart(JA_SE_LK_HS_CHAIN, &link->current.pos, 0,
                       dComIfGp_getReverb(current.roomNo));
        if (fopAcM_GetParam(this) != Mode_Shot)
            mDoAud_seStart(JA_SE_LK_HS_WIND_UP, &link->current.pos, 0,
                           dComIfGp_getReverb(current.roomNo));
    }
    if (mCurrProcFunc.i != 0)
        (this->*mCurrProcFunc)();

    /* eyePos = attention_info.position = current.pos (word copies) */
    auto* positionWords = hookshotView<HookshotWordVector>(gabi::ea(&current.pos));
    u32 px = positionWords->x, py = positionWords->y, pz = positionWords->z;
    auto* attentionWords = hookshotView<HookshotWordVector>(gabi::ea(&attention_info.position));
    attentionWords->x = px;
    attentionWords->y = py;
    attentionWords->z = pz;
    auto* eyeWords = hookshotView<HookshotWordVector>(gabi::ea(&eyePos));
    eyeWords->x = px;
    eyeWords->y = py;
    eyeWords->z = pz;

    HookshotLocal<cXyz> root;
    cXyz* r = link_hookshotRootPos(link);
    root->x = r->x;
    root->y = r->y;
    root->z = r->z;
    HookshotLocal<cXyz> sp3C;
    cXyz_mi(&current.pos, sp3C, root);
    s32 cnt = gabi::ftoi(std_sqrtf(PSVECSquareMag(sp3C)) / 7.0f);
    if (cnt > 300)
        cnt = 300;
    mChainCnt = cnt;

    if (fopAcM_GetParam(this) != Mode_Pull || cnt > 18) {
        HookshotLocal<cXyz> xz;
        xz->x = sp3C->x;
        xz->y = 0.0f;
        xz->z = sp3C->z;
        f32 absXZ = std_sqrtf(PSVECSquareMag(xz));
        shape_angle.x = cM_atan2s(-sp3C->y, absXZ);
        shape_angle.y = cM_atan2s(sp3C->x, sp3C->z);
    } else {
        shape_angle.x = link->shape_angle.x;
        shape_angle.y = link->shape_angle.y;
    }

    if (fopAcM_GetParam(this) != Mode_Wait && !m2A0) {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, 0);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), -0x4000);
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    } else {
        m2A0 = false;
    }

    u8 roomNo = hookshotStayRoom();
    tevStr.mRoomNo = roomNo; /* tevStr.mRoomNo */
    mStts.mRoomId = roomNo;  /* mStts.SetRoomId */
    current.roomNo = roomNo; /* current.roomNo */

    if (fopAcM_GetParam(this) != Mode_Wait) {
        /* HD: setItemWaterEffect(actor, u8, int) is not a member of the player */
        m2A1 = hs_setItemWaterEffect(this, (u8)m2A1, 0);
        if (fopAcM_GetParam(this) == Mode_Shot) {
            speedF = std_sqrtf(PSVECSquareDistance(&current.pos, &old.pos));
            return TRUE;
        }
        if (fopAcM_GetParam(this) == Mode_Return || fopAcM_GetParam(this) == Mode_Pull) {
            speedF = -std_sqrtf(PSVECSquareDistance(&current.pos, &old.pos));
            return TRUE;
        }
    }
    speedF = 0.0f;
    return TRUE;
}

/* 021770EC */
static BOOL daHookshot_Execute(daHookshot_c* i_this) { return i_this->execute(); }

/* 021770F0 */
static BOOL daHookshot_IsDelete(daHookshot_c* i_this) { return TRUE; }

/* 021770F8 */
BOOL daHookshot_c::hookshot_delete() {
    if (mCarryActorID != 0xFFFFFFFFu) {
        fopAc_ac_c* carry_actor = fopAcM_SearchByID(mCarryActorID);
        if (carry_actor != nullptr) {
            u32 st = carry_actor->actor_status;
            if (st & fopAcStts_HOOK_CARRY_e /* fopAcStts_HOOK_CARRY_e */) {
                carry_actor->actor_status =
                    st & ~static_cast<u32>(fopAcStts_HOOK_CARRY_e); /* fopAcM_cancelHookCarryNow */
                mCarryActorID = 0xFFFFFFFFu;
            }
        }
    }
    return TRUE;
}

/* 02177168 */
static BOOL daHookshot_Delete(daHookshot_c* i_this) {
    i_this->hookshot_delete();
    return TRUE;
}

/* 02177C00 */
BOOL daHookshot_c::procWait_init(BOOL playSe) {
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    fopAcM_SetParam(this, Mode_Wait);
    setProc(PROC_WAIT);
    mChainCnt = 0;
    cXyz* r = link_hookshotRootPos(link);
    f32 x = r->x, y = r->y, z = r->z;
    current.pos.x = x;
    current.pos.y = y;
    current.pos.z = z;
    mObjHookFlg = FALSE;
    mCarryActorID = 0xFFFFFFFFu;
    if (playSe)
        mDoAud_seStart(JA_SE_LK_HS_WIND_UP_FIN, &link->current.pos, 0,
                       dComIfGp_getReverb(current.roomNo));
    return TRUE;
}

// HD: the actor gains a stage-specific setup in "GanonK"
/* 02177CB8 */
cPhs_State daHookshot_c::create() {
    u32 b = gabi::ea(this);
    /* fopAcM_ct(this, daHookshot_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (b != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = HOOKSHOT_VTBL;
            hs_shapeConstruct(mShape); /* daHookshot_shape::daHookshot_shape (HD) */
            hs_objectConstruct(mRenderObject);
            /* dBgS_RopeLinChk */
            dBgS_LinChk_ct(mLinChk, LINCHK_VT, false);
            hookshotLine(mLinChk)->mbRopeThrough = 1; /* rope */
            /* dBgS_ObjGndChk */
            dBgS_GndChk_ct(mGndChk, GNDCHK_VT, true);
            dCcD_Stts_ct(&mStts);
            /* dCcD_Cps */
            u32 c = sightCps();
            hs_collisionConstruct(c);                   /* dCcD_GObjInf::dCcD_GObjInf */
            hookshotCps(c)->mShapeVtable = 0x100015A8;  /* cCcD_ShapeAttr */
            hookshotCps(c)->mBoundsVtable = 0x10011578; /* cM3dGAab (this TU) */
            hs_capsuleConstruct(c + 0x118);             /* cM3dGCps::cM3dGCps */
            hookshotCps(c)->__vtbl_hitinf = 0x1004AF18;
            hookshotCps(c)->mCpsVtable = 0x1004AF60;
            hookshotCps(c)->mShapeVtable = 0x1004AF70;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    hookshotView<HookshotShapeView>(gabi::ea(mShape))->mUserArea = b; /* mShape.setUserArea(this) */
    hookshotLine(mLinChk)->mFlag &= ~0x20000000u;                     /* mLinChk.ClrSttsRoofOff() */
    procWait_init(FALSE);
    gravity = -5.0f;
    mStts.Init(10, 0xFF, this);
    hs_capsuleSet(sightCps(), gabi::ea(&l_at_cps_src)); /* mSightCps.Set(l_at_cps_src) */
    hookshotCps(sightCps())->mStts = &mStts;
    u8 roomNo = hookshotStayRoom();
    tevStr.mRoomNo = roomNo;
    mStts.mRoomId = roomNo;
    current.roomNo = roomNo;
    if (hookshotIsStartStage("GanonK"))
        hs_setupGanonRender(mRenderObject, 0);
    return cPhs_COMPLEATE_e;
}

/* 02177FD0 */
static cPhs_State daHookshot_Create(fopAc_ac_c* i_actor) {
    return reinterpret_cast<daHookshot_c*>(i_actor)->create();
}

// Preserve the recompiled lfs/stfs round-trip bits, including signaling NaNs.
static f32 copiedFloat(u32 bits) {
    f32 value;
    memcpy(&value, &bits, 4);
    return value;
}

/* 021789B4 */
BOOL daHookshot_c::procWait() {
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    cXyz* r = link_hookshotRootPos(link);
    f32 rz = r->z, rx = r->x, ry = r->y;
    current.pos.x = rx;
    current.pos.z = rz;
    current.pos.y = ry;
    /* daPy_lk_c::getModelJointMtx(8) (virtual +0xFC): cl_LhandA */
    Mtx34* leftHandMtx = gabi::call_ptr<Mtx34*>(gabi::load<u32>(link->__vtbl + 0xFC), link, 8);
    PSMTXCopy(leftHandMtx, &mMtx);
    mMtx.m[0][3] = copiedFloat(hookshotView<HookshotWordVector>(gabi::ea(&current.pos))->x);
    mMtx.m[1][3] = copiedFloat(hookshotView<HookshotWordVector>(gabi::ea(&current.pos))->y);
    mMtx.m[2][3] = copiedFloat(hookshotView<HookshotWordVector>(gabi::ea(&current.pos))->z);
    mObjHookFlg = FALSE;
    u32 c = sightCps();

    if (fopAcM_GetParam(this) == Mode_Shot) {
        s16 angleY = (s16)(link_bodyAngleY(link) + link->shape_angle.y);
        u16 angleX = link_bodyAngleX(link);
        mMoveVec.x = cM_ssin(angleY) * cM_scos(angleX);
        mMoveVec.y = -cM_ssin(angleX);
        mMoveVec.z = cM_scos(angleY) * cM_scos(angleX);
        hs_resetAtHit(c);                         /* mSightCps.ResetAtHit() */
        hookshotCps(c)->mObjAt.mSPrm &= ~0x10u;   /* OffAtNoTgHitInfSet */
        hookshotCps(c)->mGObjAt.mHitCallback = 0; /* SetAtHitCallback(NULL) */

        HookshotLocal<cXyz> mv;
        cXyz_ml(&mMoveVec, mv, 60.0f);
        HookshotLocal<cXyz> r1_3C;
        cXyz_mi(&current.pos, r1_3C, mv);
        dBgS_LinChk_Set(mLinChk, r1_3C, &current.pos, this);
        m2A0 = true;
        if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
            if (dBgS_ChkPolyHSStick(dComIfG_Bgsp(), mLinChk + 0x14)) {
                fopAcM_SetParam(this, Mode_Pull);
                mShipRideFlg = false;
                setProc(PROC_PULL);
                dBgS* bgs = dComIfG_Bgsp();
                cXyz* triPla = (cXyz*)cBgS_GetTriPla(bgs, hookshotLine(mLinChk)->mBgIndex,
                                                     hookshotLine(mLinChk)->mPolyIndex);
                if (triPla != nullptr) {
                    HookshotLocal<cXyz> xz;
                    xz->z = triPla->z;
                    xz->x = triPla->x;
                    xz->y = 0.0f;
                    f32 absXZ = std_sqrtf(PSVECSquareMag(xz));
                    mHookAngle.x = cM_atan2s(triPla->y, absXZ);
                    mHookAngle.y = cM_atan2s(-triPla->x, -triPla->z);
                    mHookAngle.z = 0;
                } else { /* HD: null-checked plane */
                    mHookAngle.x = 0;
                    mHookAngle.y = 0;
                    mHookAngle.z = 0;
                }
                mMoveVec.copy(*cXyz_Zero);
            } else {
                fopAcM_SetParam(this, Mode_Return);
                setProc(PROC_RETURN);
            }
        } else {
            PSVECScale(&mMoveVec, &mMoveVec, 7.0f);
            cps_SetStartEnd(c, r1_3C, &current.pos, 5.0f);
            cps_CalcAtVec(c);
            dComIfG_Ccsp_Set(gabi::at<void>(c));
            dComIfG_Ccsp_SetMass(c, 1);
            m2A3 = false;
            setProc(PROC_SHOT);
        }
    } else {
        hookshotCps(c)->mObjAt.mSPrm |= 0x10u; /* OnAtNoTgHitInfSet */
        u16 sy = link->shape_angle.y;
        u16 bx = link_bodyAngleX(link);
        f32 py = current.pos.y;
        f32 px = current.pos.x;
        f32 pz = current.pos.z;
        f32 ssy = cM_ssin(sy), csy = cM_scos(sy);
        f32 sbx = cM_ssin(bx), cbx = cM_scos(bx);
        mObjSightCrossPos.x = gabi::fmadds(1500.0f * ssy, cbx, px);
        mObjSightCrossPos.y = gabi::fnmsubs(1500.0f, sbx, py);
        mObjSightCrossPos.z = gabi::fmadds(1500.0f * csy, cbx, pz);
        cps_SetStartEnd(c, &current.pos, &mObjSightCrossPos, 5.0f);
        cps_CalcAtVec(c);
        hookshotCps(c)->mGObjAt.mHitCallback = gabi::guest_fn(
            reinterpret_cast<const void*>(daHookshot_rockLineCallback)); /* SetAtHitCallback */
        dComIfG_Ccsp_Set(gabi::at<void>(c));
    }
    return TRUE;
}

/* 02178DDC */
BOOL daHookshot_c::procShot() {
    if (fopAcM_GetParam(this) == Mode_Return) {
        setProc(PROC_RETURN);
        return TRUE;
    }
    u32 c = sightCps();
    if (hs_checkAtHit(c) /* mSightCps.ChkAtHit() */) {
        fopAc_ac_c* hit_ac = hs_getHitActor(c + 0x50); /* GetAtHitAc() */
        if (hit_ac != nullptr) {
            mCarryActorID = fopAcM_GetID(hit_ac);
            u32 st = hit_ac->actor_status;
            if (st & 0x00080000 /* fopAcStts_UNK80000_e */) {
                hit_ac->actor_status = st | fopAcStts_HOOK_CARRY_e; /* fopAcM_setHookCarryNow */
                HookshotLocal<cXyz> d;
                cXyz_mi(&hit_ac->current.pos, d, &current.pos);
                mCarryOffset.copy(*d);
                mDoAud_seStart(JA_SE_LK_HS_SPIKE, &current.pos, 0,
                               dComIfGp_getReverb(current.roomNo));
                HookshotLocal<cXyz> v;
                v->x = 0.0f;
                v->y = 1.0f;
                v->z = 0.0f;
                dComIfGp_getVibration_StartShock(4, -0x21, v);
                fopAcM_SetParam(this, Mode_Return);
                setProc(PROC_RETURN);
                return TRUE;
            } else if (st & 0x00200000 /* fopAcStts_UNK200000_e */) {
                current.pos.copy(*&hookshotCps(c)->mGObjAt.mHitPos); /* GetAtHitPosP() */
                HookshotLocal<cXyz> d;
                cXyz_mi(&current.pos, d, &hit_ac->current.pos);
                mCarryOffset.copy(*d);
                m2A4 = hit_ac->shape_angle.y;
                mHookAngle.x = shape_angle.x;
                mHookAngle.z = 0;
                mHookAngle.y = shape_angle.y;
                mMoveVec.copy(*cXyz_Zero);
                fopAcM_SetParam(this, Mode_Pull);
                mShipRideFlg = false;
                setProc(PROC_PULL);
                mDoAud_seStart(JA_SE_LK_HS_SPIKE, &current.pos, 0,
                               dComIfGp_getReverb(current.roomNo));
                HookshotLocal<cXyz> v;
                v->x = 0.0f;
                v->y = 1.0f;
                v->z = 0.0f;
                dComIfGp_getVibration_StartShock(4, -0x21, v);
                return TRUE;
            } else if (hookshotCps(c)->mGObjAt.mRPrm & 1 /* mSightCps.ChkAtShieldHit() */) {
                mDoAud_seStart(JA_SE_LK_HS_REBOUND, &current.pos, 0x20,
                               dComIfGp_getReverb(current.roomNo));
            }
        }
        fopAcM_SetParam(this, Mode_Return);
        setProc(PROC_RETURN);
        return TRUE;
    }

    /* m2A3 || current.pos.abs(root) >= 1500: m2A3 is read again after the distance */
    bool rebound = m2A3 != 0;
    bool stop = rebound;
    if (!rebound) {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        HookshotLocal<cXyz> root;
        cXyz* r = link_hookshotRootPos(link);
        root->x = r->x;
        root->y = r->y;
        root->z = r->z;
        if (!(std_sqrtf(PSVECSquareDistance(&current.pos, root)) < 1500.0f)) {
            stop = true;
            rebound = m2A3 != 0;
        }
    }
    if (stop) {
        if (rebound) {
            u16 id = m2A6;
            dPa_control_c* pa = dComIfGp_getParticle();
            if (id == 0x825F /* dPa_name::ID_IT_SN_LK_FKSHOT_SUNA00 */)
                dPa_control_set(pa, 1, id, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1,
                                nullptr, nullptr, nullptr);
            else
                dPa_control_set(pa, 1, id, &current.pos, &m2BA, nullptr, 0xFF, nullptr, -1, nullptr,
                                nullptr, nullptr);
            u32 snd = mMtrlSndId;
            mDoAud_seStart(JA_SE_LK_HS_REBOUND, &current.pos, snd,
                           dComIfGp_getReverb(current.roomNo));
        }
        fopAcM_SetParam(this, Mode_Return);
        setProc(PROC_RETURN);
        return TRUE;
    }

    HookshotLocal<cXyz> mv;
    cXyz_ml(&mMoveVec, mv, 15.0f);
    HookshotLocal<cXyz> sp8C;
    cXyz_pl(&current.pos, sp8C, mv);
    dBgS_LinChk_Set(mLinChk, &current.pos, sp8C, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
        sp8C->copy(*&hookshotLine(mLinChk)->mLin.mEnd); /* mLinChk.GetCross() */
        if (dBgS_ChkPolyHSStick(dComIfG_Bgsp(), mLinChk + 0x14)) {
            fopAcM_SetParam(this, Mode_Pull);
            mShipRideFlg = false;
            setProc(PROC_PULL);
            dBgS* bgs = dComIfG_Bgsp();
            cXyz* plane = (cXyz*)cBgS_GetTriPla(bgs, hookshotLine(mLinChk)->mBgIndex,
                                                hookshotLine(mLinChk)->mPolyIndex);
            if (plane != nullptr) {
                HookshotLocal<cXyz> xz;
                xz->y = 0.0f;
                xz->z = plane->z;
                xz->x = plane->x;
                f32 absXZ = std_sqrtf(PSVECSquareMag(xz));
                mHookAngle.x = cM_atan2s(plane->y, absXZ);
                mHookAngle.y = cM_atan2s(-plane->x, -plane->z);
                mHookAngle.z = 0;
            } else { /* HD: null-checked plane */
                mHookAngle.x = 0;
                mHookAngle.z = 0;
                mHookAngle.y = 0;
            }
            mMoveVec.copy(*cXyz_Zero);
            u32 snd = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo*)(mLinChk + 0x14));
            mDoAud_seStart(JA_SE_LK_HS_SPIKE, &current.pos, snd,
                           dComIfGp_getReverb(current.roomNo));
            HookshotLocal<cXyz> v;
            v->x = 0.0f;
            v->y = 1.0f;
            v->z = 0.0f;
            dComIfGp_getVibration_StartShock(4, -0x21, v);
        } else {
            dBgS* bgs = dComIfG_Bgsp();
            void* plane = cBgS_GetTriPla(bgs, hookshotLine(mLinChk)->mBgIndex,
                                         hookshotLine(mLinChk)->mPolyIndex);
            cM3d_CalcVecZAngle(plane, &m2BA);
            bool sand = false;
            /* HD: (y < 950 && (ground || sand attribute)) */
            if (hookshotIsStartStage("kazeB") && sp8C->y < 950.0f) {
                bgs = dComIfG_Bgsp();
                cXyz* pla = (cXyz*)cBgS_GetTriPla(bgs, hookshotLine(mLinChk)->mBgIndex,
                                                  hookshotLine(mLinChk)->mPolyIndex);
                if ((pla != nullptr && !(pla->y < 0.5f)) /* cBgW_CheckBGround */ ||
                    dBgS_GetAttributeCode(dComIfG_Bgsp(), mLinChk + 0x14) ==
                        0xB /* dBgS_Attr_SAND_e */)
                    sand = true;
            }
            m2A6 = sand ? 0x825F /* ID_IT_SN_LK_FKSHOT_SUNA00 */ : 0xC /* ID_AK_JN_NG */;
            mMtrlSndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo*)(mLinChk + 0x14));
            m2A3 = true;
        }
    }
    cps_SetStartEnd(c, &current.pos, sp8C, 5.0f);
    cps_CalcAtVec(c);
    dComIfG_Ccsp_Set(gabi::at<void>(c));
    dComIfG_Ccsp_SetMass(c, 1);
    current.pos.copy(*sp8C);
    return TRUE;
}

/* 02179888 */
BOOL daHookshot_c::procPlayerPull() {
    fopAc_ac_c* carry_actor = nullptr;
    if (mCarryActorID != 0xFFFFFFFFu) {
        carry_actor = fopAcM_SearchByID(mCarryActorID);
        if (carry_actor == nullptr || fopAcM_GetParam(this) == Mode_Return) {
            setProc(PROC_RETURN);
            mCarryActorID = 0xFFFFFFFFu;
            return TRUE;
        }
        if (carry_actor->shape_angle.y != m2A4) {
            mDoMtx_YrotS(mDoMtx_stack_c::get(), (s16)(carry_actor->shape_angle.y - m2A4));
            HookshotLocal<cXyz> sp4C;
            PSMTXMultVec(mDoMtx_stack_c::get(), &mCarryOffset, sp4C);
            mCarryOffset.copy(*sp4C);
            m2A4 = carry_actor->shape_angle.y;
        }
        HookshotLocal<cXyz> p;
        cXyz_pl(&carry_actor->current.pos, p, &mCarryOffset);
        current.pos.copy(*p);
    } else {
        if (fopAcM_GetParam(this) == Mode_Return ||
            !cBgS_ChkPolySafe(dComIfG_Bgsp(), mLinChk + 0x14)) {
            setProc(PROC_RETURN);
            return TRUE;
        }
        if (dBgS_ChkMoveBG(dComIfG_Bgsp(), mLinChk + 0x14))
            dBgS_MoveBgTransPos(dComIfG_Bgsp(), mLinChk + 0x14, true, &current.pos, &mHookAngle,
                                nullptr);
    }

    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    HookshotLocal<cXyz> root;
    cXyz* r = link_hookshotRootPos(link);
    root->x = r->x;
    root->y = r->y;
    root->z = r->z;
    HookshotLocal<cXyz> sp40;
    cXyz_mi(&current.pos, sp40, root);
    f32 f1 = std_sqrtf(PSVECSquareMag(sp40));
    s32 r5 = gabi::ftoi(f1 / 7.0f);
    if (r5 > 0) {
        s32 cnt = mChainCnt;
        r5 = (r5 > cnt ? s32(u32(r5) - u32(cnt)) : 0);
        s32 iVar6 = s32(u32(r5) + u32(cnt <= 9 ? cnt : 9));
        mChainCnt = s32(u32(cnt) - u32(iVar6));
        HookshotLocal<cXyz> mv;
        cXyz_ml(sp40, mv, (7.0f * (f32)iVar6) / f1);
        mMoveVec.copy(*mv);
        if (mChainCnt > 0)
            return TRUE;
    } else {
        mChainCnt = 0;
    }
    if (carry_actor != nullptr &&
        gabi::ea(carry_actor) == gabi::ea(dComIfGp_getShipActor()) /* dComIfGp_getShipActor() */)
        mShipRideFlg = true;
    procWait_init(TRUE);
    return TRUE;
}

/* 02179B98 */
BOOL daHookshot_c::procReturn() {
    fopAc_ac_c* carry_actor;
    if (mCarryActorID != 0xFFFFFFFFu) {
        carry_actor = fopAcM_SearchByID(mCarryActorID);
        if (carry_actor == nullptr ||
            !(carry_actor->actor_status & fopAcStts_HOOK_CARRY_e) /* fopAcM_checkHookCarryNow */) {
            mCarryActorID = 0xFFFFFFFFu;
            carry_actor = nullptr;
        }
    } else {
        carry_actor = nullptr;
    }

    bool done = mChainCnt <= 9;
    HookshotLocal<cXyz> sp4C;
    f32 f1 = 0.0f;
    if (!done) {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        HookshotLocal<cXyz> root;
        cXyz* r = link_hookshotRootPos(link);
        root->x = r->x;
        root->y = r->y;
        root->z = r->z;
        cXyz_mi(&current.pos, sp4C, root);
        f1 = std_sqrtf(PSVECSquareMag(sp4C));
        done = gabi::ftoi(f1 / 7.0f) <= 9;
    }
    if (done) {
        if (carry_actor != nullptr)
            carry_actor->actor_status =
                carry_actor->actor_status &
                ~static_cast<u32>(fopAcStts_HOOK_CARRY_e); /* fopAcM_cancelHookCarryNow */
        procWait_init(TRUE);
        procWait();
        return TRUE;
    }
    f32 s = 63.0f / f1;
    mChainCnt = mChainCnt - 9;
    HookshotLocal<cXyz> d;
    cXyz_ml(sp4C, d, s);
    PSVECSubtract(&current.pos, d, &current.pos);
    if (carry_actor != nullptr) {
        HookshotLocal<cXyz> p;
        cXyz_pl(&current.pos, p, &mCarryOffset);
        f32 x = p->x, y = p->y, z = p->z;
        carry_actor->current.pos.x = x;
        carry_actor->current.pos.y = y;
        carry_actor->current.pos.z = z;
        hookshotGround(mGndChk)->m_pos.x = x;
        hookshotGround(mGndChk)->m_pos.y = (y + 100.0f) + 100.0f;
        hookshotGround(mGndChk)->m_pos.z = z;
        f32 ground_y = cBgS_GroundCross(dComIfG_Bgsp(), mGndChk);
        if (ground_y > carry_actor->current.pos.y) {
            carry_actor->current.pos.y = ground_y;
            mCarryOffset.y = ground_y - current.pos.y;
        }
    }
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 02179D84 */
static void __sinit_d_a_hookshot_cpp() {
    sinit_header_statics(0x10464744, 0x101B76FC);
    l_at_cps_src.mRadius = 5.0f; /* l_at_cps_src radius */
}

// sead::SafeString deleting destructor (per-TU copy; trivial destructor)
/* 02179E28 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD shape sub-object (0x254 bytes) constructor, for __construct_array
/* 02179E3C */
u32 shapeBuf_ct(u32 p) {
    if (p == 0) {
        p = gabi::ea(operator_new(0x254));
        if (p == 0)
            return p;
    }
    hs_vertexBufferConstruct(p + 4);
    hs_uniformBufferConstruct(p + 0x158);
    hookshotView<HookshotGpuBuffer>(p)->mAllocation = 0;
    hookshotView<HookshotGpuBuffer>(p)->mAllocationCount = 0;
    return p;
}

// HD 0x10-byte element constructor (trivial), for __construct_array
/* 02179E98 */
u32 elem10_ct(u32 p) {
    if (p == 0)
        p = gabi::ea(operator_new(0x10));
    return p;
}

// HD chain link (0xA8 bytes) constructor, for __construct_array
/* 02179EC4 */
u32 chainLink_ct(u32 p) {
    if (p == 0) {
        p = gabi::ea(operator_new(0xA8));
        if (p == 0)
            return p;
    }
    hs_renderLinkConstruct(p);
    hookshotView<HookshotRenderLinkView>(p)->mVtable = 0x1016EF84;
    hs_zeroBuffer(p + 0x74, 0x34);
    if (p + 0x74 == 0) /* inline placement new of a member */
        operator_new(0x30);
    return p;
}

// HD chain link destructor
/* 02179F34 */
void chainLink_dt(u32 p, s32 flags) {
    if (p != 0) {
        hs_renderLinkDestruct(p, 0);
        if (flags & 1)
            operator_delete(gabi::at<void>(p));
    }
}

// HD shape sub-object (0x254 bytes) destructor
/* 02179F88 */
void shapeBuf_dt(u32 p, s32 flags) {
    if (p != 0) {
        hs_uniformBufferDestruct(p + 0x158, 2);
        hs_vertexBufferDestruct(p + 4, 2);
        if (flags & 1)
            operator_delete(gabi::at<void>(p));
    }
}

// empty virtual of daHookshot_shape (per-TU copy)
/* 02179FE8 */
static void daHookshot_shape_empty(void* p) {}

// daHookshot_c deleting destructor (inline member destructors)
/* 02179FEC */
static void daHookshot_c_dt(daHookshot_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        u32 b = gabi::ea(i_this);
        hs_collisionDestruct(b + 0xD630, 2); /* dCcD_GObjInf::~dCcD_GObjInf (mSightCps) */
        dCcD_Stts_dt(&i_this->mStts, 2);
        /* ~dBgS_ObjGndChk: this TU's vtables, then ~cBgS_Chk */
        u32 g = b + 0xD44C;
        hookshotGround(i_this->mGndChk)->cBgS_PolyInfo::__vtbl = 0x100115B8;
        hookshotGround(i_this->mGndChk)->dBgS_PolyPassChk::__vtbl = 0x100115D8;
        hookshotGround(i_this->mGndChk)->dBgS_GrpPassChk::__vtbl = 0x10011598;
        hs_groundDestruct(g + 0x154, 0);
        /* ~dBgS_RopeLinChk */
        hookshotLine(i_this->mLinChk)->dBgS_GrpPassChk::__vtbl = 0x10011598;
        hookshotLine(i_this->mLinChk)->cBgS_PolyInfo::__vtbl = 0x10011588;
        hookshotLine(i_this->mLinChk)->dBgS_PolyPassChk::__vtbl = 0x10011658;
        hs_lineDestruct(g + 0xE8, 0);
        hs_objectDestruct(g, 2);             /* HD object at 0xD44C */
        hs_shapeDestruct(i_this->mShape, 2); /* daHookshot_shape::~daHookshot_shape (HD) */
        hs_actorDestruct(i_this, 0);         /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

// sead::SafeString::assureTerminationImpl_ (per-TU copy; empty)
/* 0217A0F0 */
static void SafeString_assureTermination(void* p) {}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): Hookshot
ext 0255F84C void hs_prepareDraw()
ext 027FDA54 void hs_setupMaterial(u32 material, s32 index, Mtx34* view, u32 context, u32 matrices)
ext 0274D458 void hs_scaleColor(void* output, void* input, f32 scale)
ext 027FDFF4 void hs_flushMaterial(u32 material, s32 index)
ext 0274D2AC void hs_scaleAmbient(u32 color, f32 scale)
ext 027FB678 void hs_flushLink(u32 link)
ext 027F0E04 void hs_entryImm(u32 list, void* shape, s32 flags)
ext 02415A80 void hs_setupPlayerRender(fopAc_ac_c* player, u32 object, void* shape)
ext 02443208 u8 hs_setItemWaterEffect(daHookshot_c* actor, u8 state, s32 flags)
ext 0217718C void hs_shapeConstruct(void* shape)
ext 02080404 void hs_objectConstruct(void* object)
ext 02515FB8 void hs_collisionConstruct(u32 collision)
ext 02018150 void hs_capsuleConstruct(u32 capsule)
ext 025164C0 void hs_capsuleSet(u32 capsule, u32 source)
ext 0207FD38 void hs_setupGanonRender(void* object, s32 flags)
ext 02516138 void hs_resetAtHit(u32 collision)
ext 025160DC BOOL hs_checkAtHit(u32 collision)
ext 02515BBC fopAc_ac_c* hs_getHitActor(u32 hit)
ext 027B5BD8 void hs_vertexBufferConstruct(u32 buffer)
ext 027BF734 void hs_uniformBufferConstruct(u32 buffer)
ext 027FB40C void hs_renderLinkConstruct(u32 link)
ext 028F521C void hs_zeroBuffer(u32 buffer, u32 size)
ext 027FB528 void hs_renderLinkDestruct(u32 link, s32 flags)
ext 027BF880 void hs_uniformBufferDestruct(u32 buffer, s32 flags)
ext 027B5CBC void hs_vertexBufferDestruct(u32 buffer, s32 flags)
ext 02515980 void hs_collisionDestruct(u32 collision, s32 flags)
ext 02008DAC void hs_groundDestruct(u32 ground, s32 flags)
ext 02008B4C void hs_lineDestruct(u32 line, s32 flags)
ext 02082DDC void hs_objectDestruct(u32 object, s32 flags)
ext 02177FD4 void hs_shapeDestruct(void* shape, s32 flags)
ext 025D50BC void hs_actorDestruct(daHookshot_c* actor, s32 flags)
ext 028E8DE8 f32 PSVECSquareDistance(const cXyz* a, const cXyz* b)
ext 028E8DAC void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out)
ext 024EF398 BOOL dBgS_ChkPolyHSStick(dBgS* bgs, void* polyInfo)
ext 024EF0F4 s32 dBgS_GetAttributeCode(dBgS* bgs, void* polyInfo)
ext 02008254 BOOL cBgS_ChkPolySafe(dBgS* bgs, void* polyInfo)
ext 024EEABC BOOL dBgS_ChkMoveBG(dBgS* bgs, void* polyInfo)
ext 024EFA38 void dBgS_MoveBgTransPos(dBgS* bgs, void* polyInfo, bool b, cXyz* pos, csXyz* a, csXyz* scale)
ext 02017300 void cM3d_CalcVecZAngle(void* v, csXyz* out)
ext 020181B0 void hs_setCpsEndpoints(u32 capsule, cXyz* start, cXyz* end, f32 radius)
ext 02516C14 void hs_setCollisionMass(u32 manager, u32 object, u8 type)
data l_stayRoom 1047E6C8
data l_chainTimer 101FF560
data l_at_cps_src 101B76AC
data l_j3dSys 104B45C0
str 10011714 in 02177CB8
str 10011738 in 02178DDC
*/
// clang-format on
