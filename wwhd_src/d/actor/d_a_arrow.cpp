/**
 * d_a_arrow.cpp (WWHD)
 * Item - Arrow (Link's arrows, and Zelda's light arrows in the Ganondorf fight)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_arrow.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arc_name STR(0x1000749C) /* "Link" */
#define SAFESTRING_VTBL 0x10007210  /* this TU's sead::SafeString vtable */
#define ACT_VTBL 0x10007358         /* HD: daArrow_c vtable */

/* statics */
#define M_KEEP_TYPE 0x101D5F43      /* u8 daArrow_c::m_keep_type */
#define M_COUNT 0x1018FDE8          /* s16 daArrow_c::m_count */
#define M_AT_CPS_SRC 0x1018FE5C     /* dCcD_SrcCps daArrow_c::m_at_cps_src (.bss, filled by __sinit) */
#define M_CO_SPH_SRC 0x1018FE1C     /* dCcD_SrcSph daArrow_c::m_co_sph_src */
#define USE_MP_REST 0x1000738C      /* checkRestMp: static const s16 use_mp[4] */
#define ARROW_MAT 0x100073AC        /* setDrawShapeMaterial: {u32 atType; u8 atp; u16 tipJnt} [4] */
#define HEAP_SIZE 0x100073DC        /* _create: static const u32 heap_size[4] */
#define SHOOT_SE 0x1000740C         /* setArrowShootSe: static const s32 se[8] */
#define USE_MP 0x10007444           /* arrowUseMp: static const s16 use_mp[4] */

enum ArrowType { TYPE_NORMAL = 0, TYPE_FIRE = 1, TYPE_ICE = 2, TYPE_LIGHT = 3 };
enum {
    dRes_INDEX_LINK_BDL_ARROW_e = 0x37,
    dRes_INDEX_LINK_BDL_ARROWGLITTER_e = 0x38,
    fpcNm_ARROW_e = 0x1D8,
    fpcNm_ARROW_LIGHTEFF_e = 0x1DA,
    fpcNm_PZ_e = 0xD2,
    AT_TYPE_NORMAL_ARROW = 0x4000,
    AT_TYPE_LIGHT_ARROW = 0x100000,
    dItemNo_MAGIC_ARROW_e = 0x35,
    dItemNo_LIGHT_ARROW_e = 0x36,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
static inline void mDoMtx_MtxToRot(Mtx34* m, csXyz* rot) { gabi::call(0x025F232C, m, rot); }
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
/* fopAcM_SearchByID(id, fopAc_ac_c** out) (out of line) */
static inline BOOL fopAcM_SearchByID_out(u32 id, be<u32>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
static inline u32 fopAcM_createChild(s16 name, u32 parent, u32 prm, cXyz* pos, s32 room, csXyz* angle, cXyz* scale, s8 subtype,
                                     u32 createFunc) {
    return gabi::call<u32>(0x025D5A20, name, parent, prm, pos, room, angle, scale, subtype, createFunc);
}
static inline fopAc_ac_c* fopAcM_fastCreate(s16 name, u32 prm, cXyz* pos, s32 room, csXyz* angle, cXyz* scale, s8 subtype,
                                            u32 createFunc, u32 data) {
    return gabi::call<fopAc_ac_c*>(0x025D5928, name, prm, pos, room, angle, scale, subtype, createFunc, data);
}
static inline f32 cBgS_GroundCross_l(dBgS* bgs, void* chk) { return gabi::call<f32>(0x02008974, bgs, chk); }
static inline s32 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s32>(0x024EF130, bgs, poly); }
static inline s32 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<s32>(0x024EEEB8, bgs, poly); }
static inline void dCcD_Cps_Set(void* cps, u32 src) { gabi::call(0x025164C0, cps, src); }
static inline void dCcD_Sph_Set(void* sph, u32 src) { gabi::call(0x0251677C, sph, src); }
static inline void dCcMassS_Mng_Set(void* obj, u8 prio) { gabi::call(0x02516C14, gabi::at<void>(dComIfGp_ea() + PLAY_CCMASS), obj, prio); }
/* cM3dGCps::Set(const cXyz& start, const cXyz& end, f32 r) */
static inline void cM3dGCps_Set(void* cps, cXyz* start, cXyz* end, f32 r) { gabi::call(0x020181B0, cps, start, end, r); }
static inline void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* ab) { gabi::call(0x028E8DAC, a, b, ab); }
static inline void JPASetRMtxTVecfromMtx(const Mtx34* m, void* r, void* t) { gabi::call(0x028249B0, m, r, t); }
static inline f32 cM_rnd_l() { return gabi::call<f32>(0x02019788); }
/* save data: *(0x101F84DC) + 0x34 magic, + 0x68 the bow slot (dInvSlot_BOW_e) */
static inline u32 savep() { return gabi::load<u32>(0x101F84DC); }
static inline u8 dComIfGs_getMagic() { return gabi::load<u8>(savep() + 0x34); }
/* daPy_py_c virtuals through the HD vtable at +0xB4 */
static inline u32 vfunc(void* obj, u32 slot) { return gabi::load<u32>(gabi::load<u32>(gabi::ea(obj) + 0xB4) + slot); }
/* J3DModelData joint array behind the model (+0xAC: {?, u32 count, entries of 0x1C}); an index out
 * of range yields entry 0 (bounds-checked buffer accessor) */
static inline u32 joint_entry(u32 jointBuf, u32 idx) {
    u32 base = gabi::load<u32>(jointBuf + 8);
    if (idx < gabi::load<u32>(jointBuf + 4)) base += idx * 0x1C;
    return base;
}

struct daArrow_c : fopAc_ac_c {
    BOOL _createHeap();
    void _atHit(dCcD_GObjInf* thisObjInf, fopAc_ac_c* hitActor, dCcD_GObjInf* hitObjInf);
    void checkCreater();
    void setLightEffect();
    void setBlur();
    void createBlur();
    void setArrowShootSe();
    void setDrawShapeMaterial();
    void arrowShooting();
    void arrowUseMp();
    void ShieldReflect();
    bool check_water_in();
    daArrow_c* changeArrowType();
    void setRoomInfo();
    void setKeepMatrix();
    void setStopActorMatrix();
    BOOL procWait();
    BOOL procMove();
    BOOL procReturn();
    BOOL procStop_BG();
    BOOL procStop_Actor();
    BOOL procWater();
    void setTypeByPlayer();
    BOOL createInit();
    BOOL _execute();
    BOOL _draw();
    cPhs_State _create();
    BOOL _delete();

    void setProc(u32 fn) { /* GHS pointer to member {s16 delta 0, s16 -1 (not virtual), fn} */
        mCurrProcFunc_delta = 0;
        mCurrProcFunc_vidx = -1;
        mCurrProcFunc_fn = fn;
    }

    /* 0x3AC */ be<u8> mbSetByZelda;
    /* 0x3AD */ u8 _3AD[3];
    /* 0x3B0 */ gptr<J3DModel> mpModel;
    /* 0x3B4 */ be<u32> mpTipMat;          /* J3DMaterial* */
    /* 0x3B8 */ be<u32> mpBtk;             /* J3DAnmTextureSRTKey* */
    /* 0x3BC */ u8 mLinChk[0x6C];          /* dBgS_ArrowLinChk */
    /* 0x428 */ u8 mGndChk[0x54];          /* dBgS_ObjGndChk */
    /* 0x47C */ dCcD_Stts mStts;
    /* 0x4B8 */ dCcD_Cps mAtCps;
    /* 0x5F0 */ dCcD_Sph mCoSph;
    /* 0x71C */ be<u8> field_0x600;
    /* 0x71D */ be<u8> mArrowType;
    /* 0x71E */ be<s16> field_0x602;
    /* 0x720 */ be<s16> field_0x604;
    /* 0x722 */ u8 _722[2];
    /* 0x724 */ be<s16> mSparkleTimer;
    /* 0x726 */ u8 _726[2];
    /* 0x728 */ be<u32> mpSparkleEmitter;
    /* 0x72C */ be<u32> mHitActorProcID;
    /* 0x730 */ be<s32> mHitJointIndex;
    /* 0x734 */ cXyz field_0x618;
    /* 0x740 */ u8 field_0x624[0x77C - 0x740];
    /* 0x77C */ be<f32> mBtkFrame;
    /* 0x780 */ be<u8> mbLinkReflect;
    /* 0x781 */ u8 _781[3];
    /* 0x784 */ u8 mBlurFollowCb[0x14];    /* dPa_followEcallBack: vtable +0, emitter +4 */
    /* 0x798 */ csXyz mBlurAngle;
    /* 0x79E */ be<u8> field_0x682;
    /* 0x79F */ u8 _79F;
    /* 0x7A0 */ be<u32> mLightEffPID;
    /* 0x7A4 */ be<u8> mbHasLightEff;
    /* 0x7A5 */ u8 _7A5[3];
    /* 0x7A8 */ be<s16> mCurrProcFunc_delta; /* HD: GHS pointer to member, 8 bytes (GameCube 12) */
    /* 0x7AA */ be<s16> mCurrProcFunc_vidx;
    /* 0x7AC */ be<u32> mCurrProcFunc_fn;
    /* 0x7B0 */ be<u8> field_0x698;
    /* 0x7B1 */ be<u8> field_0x699;
    /* 0x7B2 */ be<u8> field_0x69a;
    /* 0x7B3 */ u8 _7B3;
    /* 0x7B4 */ be<s16> field_0x69c;
    /* 0x7B6 */ u8 _7B6[2];
    /* 0x7B8 */ be<s32> field_0x6a0;
    /* 0x7BC */ be<s32> mInWaterTimer;
    /* 0x7C0 */ cXyz field_0x6a8;
    /* 0x7CC */ Mtx34 field_0x6b4;
    /* 0x7FC */ be<u8> field_0x6e4;
    /* 0x7FD */ u8 _7FD;
    /* 0x7FE */ csXyz field_0x6e6;
    /* 0x804 */ be<u32> mpAtHitActor;
    /* 0x808 */ cXyz mAtHitPos;
    /* 0x814 */ be<f32> mNearestHitDist;
    /* 0x818 */ be<u8> mbHitActor;
    /* 0x819 */ u8 _819[3];
};
WWHD_OFFSET(daArrow_c, mLinChk, 0x3BC);
WWHD_OFFSET(daArrow_c, mAtCps, 0x4B8);
WWHD_OFFSET(daArrow_c, mCoSph, 0x5F0);
WWHD_OFFSET(daArrow_c, mArrowType, 0x71D);
WWHD_OFFSET(daArrow_c, mBtkFrame, 0x77C);
WWHD_OFFSET(daArrow_c, mCurrProcFunc_delta, 0x7A8);
WWHD_OFFSET(daArrow_c, field_0x6b4, 0x7CC);
WWHD_OFFSET(daArrow_c, mbHitActor, 0x818);
WWHD_SIZE(daArrow_c, 0x81C);

/* guest addresses of the member functions (pointers to member, callbacks) */
enum : u32 {
    A_procWait = 0x02053D24,
    A_procMove = 0x02054040,
    A_procStop_BG = 0x02054FE0,
    A_procStop_Actor = 0x020553C0,
    A_procReturn = 0x02055450,
    A_procWater = 0x02055734,
    A_createHeap_CB = 0x02052268,
    A_atHit_CB = 0x02052388,
};

static inline u32 self(const void* p) { return gabi::ea(p); }

/* 020521BC */
BOOL daArrow_c::_createHeap() {
    WWHD_FUNC(0x020521BC, BOOL, this);
    u16 modelFileIndex;
    if (mArrowType == TYPE_LIGHT) {
        modelFileIndex = dRes_INDEX_LINK_BDL_ARROWGLITTER_e;
    } else {
        modelFileIndex = dRes_INDEX_LINK_BDL_ARROW_e;
    }
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arc_name, modelFileIndex, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(190, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x10007368), 0xBE, STR(0x10007378));

    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
    mpModel = model;
    return model != nullptr;
}
VERIFY(0x020521BC, &daArrow_c::_createHeap);

/* 02052268 createHeap_CB: a branch to _createHeap */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02052268, BOOL, i_this);
    return static_cast<daArrow_c*>(i_this)->_createHeap();
}
VERIFY(0x02052268, createHeap_CB);

/* 0205226C */
void daArrow_c::_atHit(dCcD_GObjInf* thisObjInf, fopAc_ac_c* hitActor, dCcD_GObjInf* hitObjInf) {
    WWHD_FUNC(0x0205226C, void, this, thisObjInf, hitActor, hitObjInf);
    // Keep track of which actor this arrow hit as well as the position the hit occurred at.
    if (!hitActor) {
        return;
    }
    if (!fopAc_IsActor(hitActor)) {
        return;
    }
    if (hitActor->jntHit == 0) {
        return;
    }

    gabi::Local<cXyz> hitPos;
    hitPos->set(thisObjInf->mGObjAt.mHitPos.x, thisObjInf->mGObjAt.mHitPos.y, thisObjInf->mGObjAt.mHitPos.z);
    gabi::Local<cXyz> diff;
    cXyz_mi(hitPos, diff, &current.pos);
    f32 hitDist = std_sqrtf(PSVECSquareMag(diff));
    if (hitDist < mNearestHitDist) { // bge: unordered skips
        mNearestHitDist = hitDist;
        mHitActorProcID = fopAcM_GetID(hitActor);
        if (hitObjInf->mGObjTg.mSPrm & 1) { /* ChkTgShield */
            mbHitActor = false;
            mpAtHitActor = 0;
        } else {
            mpAtHitActor = gabi::ea(hitActor);
            mbHitActor = true;
        }
        mAtHitPos.copy(thisObjInf->mGObjAt.mHitPos);
    }
}
VERIFY(0x0205226C, &daArrow_c::_atHit);

/* 02052388 atHit_CB: a branch to _atHit */
static void atHit_CB(fopAc_ac_c* i_this, dCcD_GObjInf* thisObjInf, fopAc_ac_c* hitActor, dCcD_GObjInf* hitObjInf) {
    WWHD_FUNC(0x02052388, void, i_this, thisObjInf, hitActor, hitObjInf);
    static_cast<daArrow_c*>(i_this)->_atHit(thisObjInf, hitActor, hitObjInf);
}
VERIFY(0x02052388, atHit_CB);

/* 0205238C */
void daArrow_c::checkCreater() {
    WWHD_FUNC(0x0205238C, void, this);
    // Check if this arrow was fired by Princess Zelda (during the Ganondorf fight).
    gabi::Local<be<u32>> archer;
    if (fopAcM_SearchByID_out(parentActorID, archer)) {
        u32 a = *archer;
        if (a != 0 && gabi::load<s16>(a + 8) == fpcNm_PZ_e) { /* HD: null check */
            mbSetByZelda = true;
        }
    }
}
VERIFY(0x0205238C, &daArrow_c::checkCreater);

/* 020523E8 daArrow_c::checkRestMp (static; not named by the matcher) */
static void checkRestMp() {
    WWHD_FUNC(0x020523E8, void);
    u8 keep = gabi::load<u8>(M_KEEP_TYPE);
    if ((s32)dComIfGs_getMagic() < gabi::load<s16>(USE_MP_REST + 2 * keep)) {
        gabi::store<u8>(M_KEEP_TYPE, TYPE_NORMAL);
    }
}
VERIFY(0x020523E8, checkRestMp);

/* 02052420 (not named by the matcher) */
void daArrow_c::setTypeByPlayer() {
    WWHD_FUNC(0x02052420, void, this);
    checkRestMp();
    mArrowType = gabi::load<u8>(M_KEEP_TYPE);
}
VERIFY(0x02052420, &daArrow_c::setTypeByPlayer);

/* 0205244C */
void daArrow_c::setKeepMatrix() {
    WWHD_FUNC(0x0205244C, void, this);
    // Transform the arrow onto its archer's hand.
    Mtx34* mtx = mDoMtx_stack_c::get();
    if (mbSetByZelda) {
        gabi::Local<be<u32>> zelda;
        fopAcM_SearchByID_out(parentActorID, zelda);

        mDoMtx_stack_c::transS(0.7f, -0.07f, -0.2f);
        mDoMtx_XYZrotM(mtx, 0x238E, 0x2CDF, 0x29BE);
        u32 z = *zelda;
        if (z != 0) { /* HD: daPz_c::getRightHandMatrix() inline (null-checked) */
            u32 jnt = gabi::load<u32>(gabi::load<u32>(gabi::load<u32>(z + 0x44C) + 0x90) + 0x2C);
            u16 flags = gabi::load<u16>(jnt + 4);
            u32 m = gabi::load<u32>(jnt + 0x10);
            gabi::store<u16>(jnt + 4, flags | 0x10);
            PSMTXConcat(gabi::at<Mtx34>(m + 0x390), mtx, mtx);
        }
    } else {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        mDoMtx_stack_c::transS(7.6f, -0.8f, -0.5f);
        mDoMtx_XYZrotM(mtx, -0x4F49, 0x238E, -0x6333);
        /* daPy_py_c::getLeftHandMatrix(): virtual (slot 0x14) */
        Mtx34* hand = gabi::call_ptr<Mtx34*>(vfunc(player, 0x14), player);
        PSMTXConcat(hand, mtx, mtx);
    }
    J3DModel_setBaseTRMtx(mpModel, mtx);
    current.pos.set(mtx->m[0][3], mtx->m[1][3], mtx->m[2][3]);
    mDoMtx_MtxToRot(mtx, &shape_angle);
    current.angle.x = -shape_angle.x;
    current.angle.y = shape_angle.y;
}
VERIFY(0x0205244C, &daArrow_c::setKeepMatrix);

/* 020526A8 */
void daArrow_c::setDrawShapeMaterial() {
    WWHD_FUNC(0x020526A8, void, this);
    /* static const ArrowAttackInfo arrow_mat[4] = {{atType, atp, tipJointIdx}} */
    u32 e = ARROW_MAT + mArrowType * 8;
    mAtCps.SetAtType(gabi::load<u32>(e));
    mAtCps.SetAtAtp(gabi::load<u8>(e + 4));

    if (mbSetByZelda) {
        mAtCps.SetAtAtp(4);
        mAtCps.SetAtType(AT_TYPE_NORMAL_ARROW); /* arrow_mat[0].mAtType */
        mAtCps.OnAtSPrmBit(0xE);                /* cCcD_AtSPrm_GrpAll_e */
    }

    u16 tip = gabi::load<u16>(ARROW_MAT + mArrowType * 8 + 6);
    if (tip != 0) {
        u32 jnt = joint_entry(gabi::load<u32>(gabi::ea(mpModel) + 0xAC), tip);
        mpTipMat = gabi::load<u32>(jnt + 0x10); /* getMesh() */
    }
}
VERIFY(0x020526A8, &daArrow_c::setDrawShapeMaterial);

/* shape of a material: hide()/show() is the visible byte at +4 */
static inline void shape_show(u32 mat, u8 on) { gabi::store<u8>(gabi::load<u32>(mat + 8) + 4, on); }

/* 02052738 */
BOOL daArrow_c::createInit() {
    WWHD_FUNC(0x02052738, BOOL, this);
    u8 type = mArrowType;
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    if (type == TYPE_LIGHT) {
        mpBtk = gabi::ea(link) + 0x4720; /* daPy_lk_c::getLightArrowBtk() */
    } else {
        mpBtk = gabi::ea(link) + 0x46AC; /* daPy_lk_c::getIceArrowBtk() */
    }

    setProc(A_procWait);

    setKeepMatrix();
    cullMtx = mpModel ? gabi::ea(mpModel) + 0xC8 : 0; /* fopAcM_SetMtx(this, mpModel->getBaseTRMtx()) */
    fopAcM_SetMin(this, -6.0f, -6.0f, 0.0f);
    fopAcM_SetMax(this, 6.0f, 6.0f, 65.0f);

    mStts.Init(10, 0xFF, this);
    dCcD_Cps_Set(&mAtCps, M_AT_CPS_SRC);
    mAtCps.SetStts(&mStts);
    mAtCps.mGObjAt.mHitCallback = A_atHit_CB;
    dCcD_Sph_Set(&mCoSph, M_CO_SPH_SRC);
    mCoSph.SetStts(&mStts);

    field_0x602 = -1;

    if (mArrowType != TYPE_LIGHT) {
        u32 joints = gabi::load<u32>(gabi::ea(mpModel) + 0xAC);
        shape_show(gabi::load<u32>(joint_entry(joints, 4) + 0x10), 0); /* ARROW_JNT_TIPNOMAL_e */
        shape_show(gabi::load<u32>(joint_entry(joints, 2) + 0x10), 0); /* ARROW_JNT_TIPFIRE_e */
        shape_show(gabi::load<u32>(joint_entry(joints, 3) + 0x10), 0); /* ARROW_JNT_TIPICE_e */
        shape_show(gabi::load<u32>(gabi::load<u32>(joint_entry(joints, 3) + 0x10) + 4), 0); /* its next material */
    }
    setDrawShapeMaterial();

    field_0x698 = true;
    field_0x699 = 0;
    field_0x69a = false;
    field_0x69c = 0;
    field_0x6a0 = 300;
    mInWaterTimer = 0;
    field_0x6e4 = false;
    mbHasLightEff = false;
    mbLinkReflect = false;
    field_0x604 = 0;

    return TRUE;
}
VERIFY(0x02052738, &daArrow_c::createInit);

/* 0205295C */
cPhs_State daArrow_c::_create() {
    WWHD_FUNC(0x0205295C, cPhs_State, this);
    /* fopAcM_ct(this, daArrow_c): the HD constructor is inlined */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
            /* dBgS_ArrowLinChk mLinChk */
            u32 l = self(mLinChk);
            gabi::call(0x02008FEC, mLinChk); /* cBgS_LinChk::cBgS_LinChk */
            gabi::store<u8>(l + 0x5E, 0);
            gabi::store<u32>(l + 0x4, l + 0x64);
            gabi::store<u8>(l + 0x60, 0);
            gabi::store<u8>(l + 0x62, 0);
            gabi::store<u32>(l + 0x20, 0x10007328);
            gabi::store<u32>(l + 0x0, l + 0x58);
            gabi::store<u8>(l + 0x5D, 0);
            gabi::store<u32>(l + 0x58, 0x10007348);
            gabi::store<u8>(l + 0x61, 0);
            gabi::store<u32>(l + 0x64, 0x10007338);
            gabi::store<u32>(l + 0x68, 5);
            gabi::store<u8>(l + 0x5F, 1); /* dBgS_ArrowLinChk: the arrow pass flag */
            gabi::store<u32>(l + 0x10, 0x10007318);
            gabi::store<u8>(l + 0x5C, 0);
            /* dBgS_ObjGndChk mGndChk */
            dBgS_GndChk_ct(mGndChk, dBgS_GndChk_vt{0x10007298, 0x100072A8, 0x100072C8, 0x100072B8}, true);
            dCcD_Stts_ct(&mStts);
            /* dCcD_Cps mAtCps */
            u32 c = self(&mAtCps);
            gabi::call(0x02515FB8, &mAtCps); /* dCcD_GObjInf::dCcD_GObjInf */
            gabi::store<u32>(c + 0x114, 0x100015A8);
            gabi::store<u32>(c + 0x110, 0x10007228);
            gabi::call(0x02018150, mAtCps.mCps); /* cM3dGCps::cM3dGCps */
            mAtCps.__vtbl_hitinf = 0x1004AF18;
            gabi::store<u32>(c + 0x114, 0x1004AF70);
            gabi::store<u32>(c + 0x130, 0x1004AF60);
            gabi::call(0x025166F0, &mCoSph); /* dCcD_Sph::dCcD_Sph */
            gabi::call(0x025A5894, mBlurFollowCb, 0, 0); /* dPa_followEcallBack(0, 0) */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    checkCreater();

    if (mbSetByZelda) {
        mArrowType = TYPE_LIGHT;
    } else {
        setTypeByPlayer();
    }

    if (gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 8) { /* dComIfGp_getMiniGameType() */
        mArrowType = TYPE_NORMAL;
    }

    if (!fopAcM_entrySolidHeap(this, A_createHeap_CB, gabi::load<u32>(HEAP_SIZE + 4 * mArrowType))) {
        return cPhs_ERROR_e;
    }

    if (createInit()) {
        return cPhs_COMPLEATE_e;
    } else {
        return cPhs_ERROR_e;
    }
}
VERIFY(0x0205295C, &daArrow_c::_create);

/* 02052C08 daArrowCreate: a branch to _create */
static cPhs_State daArrowCreate(void* i_this) {
    WWHD_FUNC(0x02052C08, cPhs_State, i_this);
    return static_cast<daArrow_c*>(i_this)->_create();
}
VERIFY(0x02052C08, daArrowCreate);

/* 02052C0C */
BOOL daArrow_c::_delete() {
    WWHD_FUNC(0x02052C0C, BOOL, this);
    /* mBlurFollowCb.remove(): virtual (slot 0x44) */
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(self(mBlurFollowCb)) + 0x44), mBlurFollowCb);
    return TRUE;
}
VERIFY(0x02052C0C, &daArrow_c::_delete);

/* 02052C3C daArrowDelete: a branch to _delete */
static BOOL daArrowDelete(void* i_this) {
    WWHD_FUNC(0x02052C3C, BOOL, i_this);
    return static_cast<daArrow_c*>(i_this)->_delete();
}
VERIFY(0x02052C3C, daArrowDelete);

/* 02052C40 */
void daArrow_c::setLightEffect() {
    WWHD_FUNC(0x02052C40, void, this);
    u8 type = mArrowType;
    if (field_0x682 == type) {
        if (type == TYPE_NORMAL) {
            return;
        }
        if (!mbHasLightEff) {
            mLightEffPID = fopAcM_createChild(fpcNm_ARROW_LIGHTEFF_e, gabi::load<u32>(self(this) + 4), type, &field_0x6a8,
                                              current.roomNo, &shape_angle, nullptr, -1, 0);
            if (mLightEffPID != fpcM_ERROR_PROCESS_ID_e) {
                mbHasLightEff = true;
            }
        } else {
            field_0x682 = type;
            return;
        }
    } else {
        fopAcM_delete(fopAcM_SearchByID(mLightEffPID));
        mbHasLightEff = false;
    }
    field_0x682 = mArrowType;
}
VERIFY(0x02052C40, &daArrow_c::setLightEffect);

/* 02052D04 */
void daArrow_c::setRoomInfo() {
    WWHD_FUNC(0x02052D04, void, this);
    s32 roomNo;

    dBgS_GndChk_SetPos(mGndChk, &current.pos);
    f32 groundY = cBgS_GroundCross_l(dComIfG_Bgsp(), mGndChk);
    if (!(groundY == -1000000000.0f)) { /* -G_CM3D_F_INF */
        roomNo = dBgS_GetRoomId(dComIfG_Bgsp(), dBgS_GndChk_PolyInfo(mGndChk));
        s32 color = dBgS_GetPolyColor(dComIfG_Bgsp(), dBgS_GndChk_PolyInfo(mGndChk));
        current.roomNo = roomNo;
        tevStr.mRoomNo = roomNo;
        gabi::store<u8>(self(&tevStr) + 0xBA, color); /* mEnvrIdxOverride */
        mStts.mRoomId = roomNo;
    } else {
        roomNo = gabi::load<u8>(0x1047E6C8); /* dComIfGp_roomControl_getStayNo() */
        tevStr.mRoomNo = roomNo;
        mStts.mRoomId = roomNo;
        current.roomNo = roomNo;
    }
}
VERIFY(0x02052D04, &daArrow_c::setRoomInfo);

/* 02053188 */
BOOL daArrow_c::_draw() {
    WWHD_FUNC(0x02053188, BOOL, this);
    if (!field_0x698) {
        return TRUE;
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);

    if (mArrowType != TYPE_LIGHT) {
        shape_show(mpTipMat, 1);
        if (gabi::load<u32>(mpTipMat + 4) != 0) { /* getNext() */
            shape_show(gabi::load<u32>(mpTipMat + 4), 1);
        }
    }

    gabi::store<f32>(mpBtk + 4, mBtkFrame); /* mpBtk->setFrame(mBtkFrame) */
    /* dComIfGd_setListP1() */
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D58));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D60));
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();

    if (mArrowType != TYPE_LIGHT) {
        shape_show(mpTipMat, 0);
        if (gabi::load<u32>(mpTipMat + 4) != 0) {
            shape_show(gabi::load<u32>(mpTipMat + 4), 0);
        }
    }

    return TRUE;
}
VERIFY(0x02053188, &daArrow_c::_draw);

/* 020532B4 daArrowDraw: a branch to _draw */
static BOOL daArrowDraw(void* i_this) {
    WWHD_FUNC(0x020532B4, BOOL, i_this);
    return static_cast<daArrow_c*>(i_this)->_draw();
}
VERIFY(0x020532B4, daArrowDraw);

/* 02053628 */
void daArrow_c::setBlur() {
    WWHD_FUNC(0x02053628, void, this);
    u32 blurEmitter = gabi::load<u32>(self(mBlurFollowCb) + 4); /* mBlurFollowCb.getEmitter() */
    if (!blurEmitter) {
        return;
    }
    s32 alpha = gabi::load<u8>(blurEmitter + 0x247); /* getGlobalAlpha() */
    alpha -= 50;
    if (alpha <= 0) {
        dPa_followEcallBack_end(gabi::at<dPa_followEcallBack>(self(mBlurFollowCb))); /* remove() */
    } else {
        gabi::store<u8>(blurEmitter + 0x247, alpha); /* setGlobalAlpha() */
    }

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mBlurAngle.x, mBlurAngle.y, mBlurAngle.z);
    /* blurEmitter->setGlobalRTMatrix() */
    JPASetRMtxTVecfromMtx(mDoMtx_stack_c::get(), gabi::at<void>(blurEmitter + 0x1F0), gabi::at<void>(blurEmitter + 0x22C));
}
VERIFY(0x02053628, &daArrow_c::setBlur);

/* 02053700 */
void daArrow_c::createBlur() {
    WWHD_FUNC(0x02053700, void, this);
    if (!gabi::load<u32>(self(mBlurFollowCb) + 4)) {
        /* dComIfGp_particle_setP1(ID_IT_JN_ARW_BLUR00, &current.pos, NULL, NULL, 0xFF, &mBlurFollowCb) */
        dPa_control_set(dComIfGp_getParticle(), 1, 0x48, &current.pos, nullptr, nullptr, 0xFF,
                        gabi::at<dPa_levelEcallBack>(self(mBlurFollowCb)), -1, nullptr, nullptr, nullptr);
    }
}
VERIFY(0x02053700, &daArrow_c::createBlur);

/* HD: fopAcM_seStartCurrent without the null checks */
static inline void seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    mDoAud_seStart(id, &a->current.pos, param, reverb);
}

/* 02053770 */
void daArrow_c::setArrowShootSe() {
    WWHD_FUNC(0x02053770, void, this);
    u8 type = mArrowType;
    if (type == TYPE_NORMAL) {
        return;
    }
    seStartCurrent(this, gabi::load<s32>(SHOOT_SE + type * 8), 0);
    seStartCurrent(this, gabi::load<s32>(SHOOT_SE + mArrowType * 8 + 4), 0);
}
VERIFY(0x02053770, &daArrow_c::setArrowShootSe);

/* 02053AD0 */
void daArrow_c::arrowUseMp() {
    WWHD_FUNC(0x02053AD0, void, this);
    s16 use = gabi::load<s16>(USE_MP + 2 * mArrowType);
    /* dComIfGp_setItemMagicCount(use_mp[mArrowType]) */
    u32 p = dComIfGp_ea() + 0x5B60;
    gabi::store<s16>(p, (s16)(gabi::load<s16>(p) + use));
}
VERIFY(0x02053AD0, &daArrow_c::arrowUseMp);

/* 02053D24 */
BOOL daArrow_c::procWait() {
    WWHD_FUNC(0x02053D24, BOOL, this);
    speedF = 0.0f;
    setKeepMatrix();
    PSMTXCopy(mpModel ? gabi::at<Mtx34>(gabi::ea(mpModel) + 0xC8) : nullptr, &field_0x6b4);
    u16 sx = shape_angle.x, sy = shape_angle.y, sz = shape_angle.z;
    field_0x6e6.x = sx;
    field_0x6e6.y = sy;
    field_0x6e6.z = sz;

    if (fopAcM_GetParam(this) == 1) {
        if (!mbSetByZelda) {
            arrowUseMp();
            checkRestMp();
        }

        setProc(A_procMove);
        arrowShooting();
    }

    return TRUE;
}
VERIFY(0x02053D24, &daArrow_c::procWait);

/* 020553C0 */
BOOL daArrow_c::procStop_Actor() {
    WWHD_FUNC(0x020553C0, BOOL, this);
    speedF = 0.0f;
    if (field_0x600) {
        fopAcM_delete(this);
        return TRUE;
    }

    setBlur();

    if (fopAcM_SearchByID(mHitActorProcID)) {
        setStopActorMatrix();
    } else {
        field_0x600 = true;
    }

    return TRUE;
}
VERIFY(0x020553C0, &daArrow_c::procStop_Actor);

/* 02055734 */
BOOL daArrow_c::procWater() {
    WWHD_FUNC(0x02055734, BOOL, this);
    if (mInWaterTimer <= 0) {
        fopAcM_delete(this);
        return TRUE;
    }

    mInWaterTimer = mInWaterTimer - 1;
    return TRUE;
}
VERIFY(0x02055734, &daArrow_c::procWater);

/* 02055774 daArrow_c::changeArrowMp (static; not named by the matcher) */
static BOOL changeArrowMp() {
    WWHD_FUNC(0x02055774, BOOL);
    u8 magic = dComIfGs_getMagic();
    return magic >= 1;
}
VERIFY(0x02055774, changeArrowMp);

static inline bool bow_has_magic_arrows() {
    u8 item = gabi::load<u8>(savep() + 0x68); /* dComIfGs_getItem(dInvSlot_BOW_e) */
    return item == dItemNo_MAGIC_ARROW_e || item == dItemNo_LIGHT_ARROW_e;
}

/* 0205578C (static) */
static void changeArrowTypeNotReady() {
    WWHD_FUNC(0x0205578C, void);
    u8 keep = gabi::load<u8>(M_KEEP_TYPE);
    if (keep == TYPE_NORMAL) {
        if (dComIfGs_getMagic() == 0) {
            return;
        }
        if (bow_has_magic_arrows()) {
            gabi::store<u8>(M_KEEP_TYPE, TYPE_FIRE);
        }
    } else if (keep == TYPE_FIRE) {
        if (dComIfGs_getMagic() == 0) {
            return;
        }
        if (bow_has_magic_arrows()) {
            gabi::store<u8>(M_KEEP_TYPE, TYPE_ICE);
        }
    } else if (keep == TYPE_ICE) {
        u32 s = savep();
        if (gabi::load<u8>(s + 0x34) >= 2 && gabi::load<u8>(s + 0x68) == dItemNo_LIGHT_ARROW_e) {
            gabi::store<u8>(M_KEEP_TYPE, TYPE_LIGHT);
        } else {
            gabi::store<u8>(M_KEEP_TYPE, TYPE_NORMAL);
        }
    } else if (keep == TYPE_LIGHT) {
        gabi::store<u8>(M_KEEP_TYPE, TYPE_NORMAL);
    }
}
VERIFY(0x0205578C, changeArrowTypeNotReady);

/* 0205584C */
daArrow_c* daArrow_c::changeArrowType() {
    WWHD_FUNC(0x0205584C, daArrow_c*, this);
    u8 origArrowType = mArrowType;
    mBtkFrame = 0.0f;

    daArrow_c* ret = this;
    u8 type;

    if (origArrowType == TYPE_NORMAL) {
        if (dComIfGs_getMagic() == 0 || !bow_has_magic_arrows()) {
            mArrowType = type = TYPE_NORMAL;
        } else {
            mArrowType = type = TYPE_FIRE;
        }
    } else if (origArrowType == TYPE_FIRE) {
        if (dComIfGs_getMagic() == 0 || !bow_has_magic_arrows()) {
            mArrowType = type = TYPE_NORMAL;
        } else {
            mArrowType = type = TYPE_ICE;
        }
    } else if (origArrowType == TYPE_ICE) {
        u32 s = savep();
        if (gabi::load<u8>(s + 0x34) < 2 || gabi::load<u8>(s + 0x68) != dItemNo_LIGHT_ARROW_e) {
            mArrowType = type = TYPE_NORMAL;
        } else {
            mArrowType = type = TYPE_LIGHT;
        }
    } else if (origArrowType == TYPE_LIGHT) {
        mArrowType = type = TYPE_NORMAL;
    } else {
        type = origArrowType;
    }

    if (type != origArrowType) {
        gabi::store<u8>(M_KEEP_TYPE, type);
        daArrow_c* newNockedArrow =
            (daArrow_c*)fopAcM_fastCreate(fpcNm_ARROW_e, 0, &current.pos, fopAcM_GetRoomNo(this), nullptr, nullptr, -1, 0, 0);
        mArrowType = origArrowType;
        if (!newNockedArrow) {
            gabi::store<u8>(M_KEEP_TYPE, origArrowType);
            setDrawShapeMaterial();
            ret = this;
        } else {
            fopAcM_delete(this);
            ret = newNockedArrow;
        }
    }

    return ret;
}
VERIFY(0x0205584C, &daArrow_c::changeArrowType);

/* ---- local bindings (SHARED-CANDIDATE), second group ---- */
static inline u8 daPy_lk_c_setItemWaterEffect(fopAc_ac_c* a, s32 prev, s32 p) { return gabi::call<u8>(0x02443208, a, prev, p); }
static inline void dKy_arrowcol_chg_on(cXyz* pos, s32 type) { gabi::call(0x0255F3E8, pos, type); }
static inline BOOL fopAcM_getWaterY_l(const cXyz* pos, be<f32>* y) { return gabi::call<BOOL>(0x025D9F70, pos, y); }
static inline BOOL fopAcM_SearchByName(s16 name, be<u32>* out) { return gabi::call<BOOL>(0x025D5578, name, out); }
static inline BOOL dAttention_LockonTruth(void* att) { return gabi::call<BOOL>(0x024EDFCC, att); }
/* 024EC8D0: matcher name dAttention_c::ActionTarget; the GameCube source calls LockonTarget(0) here */
static inline u32 dAttention_LockonTarget(void* att, s32 i) { return gabi::call<u32>(0x024EC8D0, att, i); }
static inline s16 cLib_targetAngleX_l(cXyz* a, cXyz* b) { return gabi::call<s16>(0x0200F974, a, b); }
static inline void mDoMtx_XrotM_l(Mtx34* m, s16 x) { gabi::call(0x025F1BF4, m, x); }
static inline BOOL cBgS_ChkPolySafe(dBgS* bgs, void* poly) { return gabi::call<BOOL>(0x02008254, bgs, poly); }
static inline BOOL dBgS_ChkMoveBG(dBgS* bgs, void* poly) { return gabi::call<BOOL>(0x024EEABC, bgs, poly); }
static inline void dBgS_MoveBgTransPos(dBgS* bgs, void* poly, bool b, cXyz* pos, csXyz* angle, csXyz* shape) {
    gabi::call(0x024EFA38, bgs, poly, b, pos, angle, shape);
}
static inline BOOL dBgS_ChkGrpInf(dBgS* bgs, void* poly, u32 grp) { return gabi::call<BOOL>(0x024EEE30, bgs, poly, grp); }
static inline s32 dBgS_GetAttributeCode(dBgS* bgs, void* poly) { return gabi::call<s32>(0x024EF0F4, bgs, poly); }
static inline void C_VECReflect(const cXyz* in, const void* normal, cXyz* out) { gabi::call(0x028E9F74, in, normal, out); }
static inline void cM3d_CalcVecZAngle(const void* v, csXyz* out) { gabi::call(0x02017300, v, out); }
static inline s32 fopAcM_createItemForSimpleDemo(cXyz* pos, s32 item, s32 room, csXyz* angle, cXyz* scale, f32 sp, f32 spy) {
    return gabi::call<s32>(0x025D8F90, pos, item, room, angle, scale, sp, spy);
}
/* HD: mDoAud_seStart(id) without a position */
static inline void mDoAud_seStart_noPos(u32 id) { gabi::call(0x025E1988, id); }
static inline void cM3dGSph_SetC(void* sph, cXyz* c) { gabi::call(0x02018D40, sph, c); }
/* cLib_calcTimer<s16>: this TU's copy */
static s16 cLib_calcTimer(be<s16>* t);
/* dComIfGp_particle_setP1(id, pos, angle): dPa_control_c::set on group 1 */
static inline JPABaseEmitter* particle_setP1(u16 id, const cXyz* pos, const csXyz* angle = nullptr) {
    return dPa_control_set(dComIfGp_getParticle(), 1, id, pos, angle, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}
static inline f32 vec_abs(const cXyz* v) { return std_sqrtf(PSVECSquareMag(v)); }
/* speed.absXZ(): a temporary cXyz(x, 0, z) */
static inline f32 absXZ(f32 x, f32 z) {
    gabi::Local<cXyz> t;
    t->x = x;
    t->y = 0.0f;
    t->z = z;
    return vec_abs(t);
}
static inline void set_model_mtx(daArrow_c* a) {
    mDoMtx_stack_c::transS(a->current.pos.x, a->current.pos.y, a->current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), a->shape_angle.x, a->shape_angle.y, 0);
    J3DModel_setBaseTRMtx(a->mpModel, mDoMtx_stack_c::get());
}
static inline void at_cps_set(daArrow_c* a, cXyz* end) {
    cM3dGCps_Set(a->mAtCps.mCps, &a->current.pos, end, 5.0f); /* SetStartEnd + SetR(5.0f) */
    u32 c = self(&a->mAtCps);
    PSVECSubtract(gabi::at<cXyz>(c + 0x124), gabi::at<cXyz>(c + 0x118), &a->mAtCps.mGObjAt.mVec); /* CalcAtVec() */
    dComIfG_Ccsp_Set(&a->mAtCps);
    dCcMassS_Mng_Set(&a->mAtCps, 1);
}

/* 02053184 daArrowExecute: a branch to _execute */
static BOOL daArrowExecute(void* i_this);

/* 02053814 */
void daArrow_c::arrowShooting() {
    WWHD_FUNC(0x02053814, void, this);
    field_0x6a8.copy(current.pos);
    f32 xCos = cM_scos(current.angle.x);
    speed.x = 200.0f * cM_ssin(current.angle.y) * xCos;
    speed.y = 200.0f * cM_ssin(current.angle.x);
    speed.z = 200.0f * cM_scos(current.angle.y) * xCos;

    setArrowShootSe();

    if (mArrowType == TYPE_LIGHT && !mbSetByZelda) {
        /* HD: strcmp(dComIfGp_getStartStageName(), "GanonK") on sead::SafeStrings: cstr() is virtual
         * (slot 0x14) */
        gabi::Local<SafeString> name;
        name->mStringTop = 0x1000743C; /* "GanonK" */
        name->__vtbl = SAFESTRING_VTBL;
        gabi::Local<SafeString> stage;
        stage->mStringTop = dComIfGp_ea() + 0x5134;
        stage->__vtbl = SAFESTRING_VTBL;
        gabi::call_ptr(gabi::load<u32>(name->__vtbl + 0x14), name.get());
        gabi::call_ptr(gabi::load<u32>(name->__vtbl + 0x14), name.get());
        gabi::call_ptr(gabi::load<u32>(stage->__vtbl + 0x14), stage.get());
        u32 a = name->mStringTop;
        u32 b = stage->mStringTop;
        bool equal = true;
        if (a != b) {
            equal = false;
            for (u32 i = 0; i < 0x40001; i++) {
                u8 ca = gabi::load<u8>(a + i);
                u8 cb = gabi::load<u8>(b + i);
                if (ca != cb) break;
                if (ca == 0) {
                    equal = true;
                    break;
                }
            }
        }
        if (!equal) {
            // Not in Puppet Ganon's boss room.
            mAtCps.SetAtSpl(0xB); /* dCcG_At_Spl_UNKB */
        }
    }

    u16 bx = shape_angle.x, by = shape_angle.y, bz = shape_angle.z;
    mBlurAngle.x = bx;
    mBlurAngle.y = by;
    mBlurAngle.z = bz;

    createBlur();

    field_0x602 = gabi::load<s16>(M_COUNT);
    s16 count = gabi::load<s16>(M_COUNT) + 1;
    if (count == 5) {
        count = 0;
    }
    gabi::store<s16>(M_COUNT, count);

    gabi::Local<cXyz> step;
    cXyz_ml(&speed, step, 1.25f);
    gabi::Local<cXyz> sum;
    cXyz_pl(&current.pos, sum, step);
    gabi::Local<cXyz> end;
    end->copy(*sum);
    at_cps_set(this, end);

    mAtHitPos.copy(*end); /* clrAtHitNormal(); setAtHitPosBuff(&end) */
    mbHitActor = false;
    mNearestHitDist = 3.4028235e+38f; /* FLOAT_MAX */
}
VERIFY(0x02053814, &daArrow_c::arrowShooting);

/* 02053B18 */
void daArrow_c::ShieldReflect() {
    WWHD_FUNC(0x02053B18, void, this);
    f32 vel = vec_abs(&speed);

    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    s16 targetAngleY = link->shape_angle.y + gabi::load<s16>(gabi::ea(link) + 0x3D2); /* getBodyAngleY() */
    s16 targetAngleX = gabi::load<s16>(gabi::ea(link) + 0x3D0);                     /* getBodyAngleX() */

    gabi::Local<be<u32>> ganondorf;
    if (fopAcM_SearchByName(0xF6 /* fpcNm_GND_e */, ganondorf) && *ganondorf != 0) { /* HD: null check */
        dAttention_c* attention = dComIfGp_getAttention();
        if (dAttention_LockonTruth(attention) && dAttention_LockonTarget(attention, 0) == *ganondorf) {
            u32 g = *ganondorf;
            gabi::Local<cXyz> chest;
            fopAc_ac_c* gnd = gabi::at<fopAc_ac_c>(g);
            chest->x = gnd->current.pos.x;
            chest->y = gnd->current.pos.y;
            chest->z = gnd->current.pos.z;
            chest->y = gabi::fadds_ppc(REG_F(8, 0), 130.0f);
            s16 ax = cLib_targetAngleX_l(&link->current.pos, chest);
            gabi::store<u32>(*ganondorf + 0xB0, 0x23); /* fopAcM_SetParam(ganondorf, 0x23) */
            targetAngleX = -ax;
            mSparkleTimer = 15 + REG0_S(3);
            mpSparkleEmitter = gabi::ea(dComIfGp_particle_set(0x3EE /* ID_AK_JN_CCTHUNDER01 */, &link->current.pos));
        }
    }

    f32 sinY = cM_ssin(targetAngleY), cosX = cM_scos(targetAngleX), sinX = cM_ssin(targetAngleX), cosY = cM_scos(targetAngleY);
    f32 sx = (sinY * cosX) * vel;
    f32 sy = (-sinX) * vel;
    f32 sz = (cosY * cosX) * vel;
    speed.x = sx;
    speed.y = sy;
    speed.z = sz;
    f32 xz = absXZ(sx, sz); /* speed.absXZ() */

    shape_angle.x = cM_atan2s(-speed.y, -xz);
    shape_angle.y = cM_atan2s(-speed.x, -speed.z);
    shape_angle.z = 0;
}
VERIFY(0x02053B18, &daArrow_c::ShieldReflect);

/* 020532B8 */
bool daArrow_c::check_water_in() {
    WWHD_FUNC(0x020532B8, bool, this);
    u8 prev_field_0x699 = field_0x699;
    u8 now = daPy_lk_c_setItemWaterEffect(this, prev_field_0x699, 1);
    field_0x699 = now;
    if (prev_field_0x699 != 0 || now == 0) { /* HD: any non-zero result */
        return FALSE;
    }

    gabi::Local<be<f32>> waterY;
    fopAcM_getWaterY_l(&current.pos, waterY);

    f32 curY = current.pos.y;
    f32 deltaY = std::fabs(old.pos.y - curY);
    f32 waterDist = std::fabs(*waterY - curY);
    gabi::Local<cXyz> waterHitPos;
    if (deltaY < 1.0f) { // bge: unordered takes the else branch
        waterHitPos->x = current.pos.x;
        waterHitPos->z = current.pos.z;
        waterHitPos->y = curY;
    } else {
        f32 weight = waterDist / deltaY;
        if (weight > 1.0f) { // ble: unordered keeps the weight
            weight = 1.0f;
        }
        gabi::Local<cXyz> a;
        cXyz_ml(&old.pos, a, weight);
        gabi::Local<cXyz> b;
        cXyz_ml(&current.pos, b, 1.0f - weight);
        gabi::Local<cXyz> sum;
        cXyz_pl(a, sum, b);
        waterHitPos->copy(*sum);
    }

    setProc(A_procWater);
    mParameters = 4; /* fopAcM_SetParam(this, 4) */

    u8 type = mArrowType;
    if (type == TYPE_FIRE) {
        mInWaterTimer = 1;
        particle_setP1(0x35A /* ID_AK_JN_EVAPORATION00 */, waterHitPos);
        if (!field_0x6e4) {
            dKy_arrowcol_chg_on(&current.pos, 0);
        }
    } else if (type == TYPE_ICE) {
        mInWaterTimer = 10 * 30;
        fopAcM_createChild(0x1D9 /* fpcNm_ARROW_ICEEFF_e */, gabi::load<u32>(self(this) + 4), mArrowType, waterHitPos,
                           current.roomNo, &current.angle, nullptr, -1, 0);
        if (!field_0x6e4) {
            dKy_arrowcol_chg_on(&current.pos, 1);
        }
    } else if (type == TYPE_LIGHT) {
        particle_setP1(0x2A1 /* ID_IT_JN_ARWG_HITA00 */, waterHitPos);
        seStartCurrent(this, 0x69DC /* JA_SE_OBJ_LIGHT_ARW_EFF */, 0);
        if (!field_0x6e4) {
            dKy_arrowcol_chg_on(&current.pos, 2);
        }
        mInWaterTimer = 1;
    } else {
        mInWaterTimer = 1;
    }

    field_0x698 = false;

    return TRUE;
}
VERIFY(0x020532B8, &daArrow_c::check_water_in);

/* 02053DDC */
void daArrow_c::setStopActorMatrix() {
    WWHD_FUNC(0x02053DDC, void, this);
    s16 xRot = 0;
    if (cLib_calcTimer(&field_0x604) != 0) {
        s16 timer = field_0x604;
        f32 temp = (f32)timer / 40.0f;
        xRot = (s16)gabi::ftoi(((1024.0f * temp) * temp) * cM_ssin(timer * 0x52FB));
    }
    fopAc_ac_c* hitActor = fopAcM_SearchByID(mHitActorProcID);
    if (!hitActor) {
        return;
    }
    u32 jntHit = hitActor->jntHit;
    if (!jntHit) {
        return;
    }
    u32 hitModel = gabi::load<u32>(jntHit + 4); /* jntHit->getModel() */

    /* static cXyz offset_arrow_pos(0.0f, 0.0f, -50.0f) (guard 0x10461494) */
    if (gabi::load<u32>(0x10461494) == 0) {
        gabi::store<u32>(0x10461494, 1);
        gabi::store<f32>(0x10461488, 0.0f);
        gabi::store<f32>(0x1046148C, 0.0f);
        gabi::store<f32>(0x10461490, -50.0f);
    }
    cXyz* offset_arrow_pos = gabi::at<cXyz>(0x10461488);

    /* mDoMtx_stack_c::copy(hitModel->getAnmMtx(mHitJointIndex)) (HD: marks the joint matrices used) */
    s32 idx = mHitJointIndex;
    u32 mtxBuf = gabi::load<u32>(hitModel + 0x2C);
    u32 anm = gabi::load<u32>(mtxBuf + 0x10);
    gabi::store<u16>(mtxBuf + 4, gabi::load<u16>(mtxBuf + 4) | 0x10);
    Mtx34* stack = mDoMtx_stack_c::get();
    PSMTXCopy(gabi::at<Mtx34>(idx * 0x30 + anm), stack);
    gabi::Local<csXyz> hitJointRot;
    mDoMtx_MtxToRot(stack, hitJointRot);

    mDoMtx_stack_c::transM(field_0x618.x, field_0x618.y, field_0x618.z);
    mDoMtx_ZXYrotM(stack, field_0x6e6.x, field_0x6e6.y, field_0x6e6.z);

    f32 px = stack->m[0][3], py = stack->m[1][3], pz = stack->m[2][3]; /* multVecZero */
    current.pos.y = py;
    current.pos.x = px;
    current.pos.z = pz;

    mDoMtx_stack_c::transS(px, py, pz);
    mDoMtx_ZXYrotM(stack, hitJointRot->x, hitJointRot->y, hitJointRot->z);
    mDoMtx_ZXYrotM(stack, field_0x6e6.x, field_0x6e6.y, field_0x6e6.z);
    mDoMtx_XrotM_l(stack, xRot);
    mDoMtx_stack_c::transM(offset_arrow_pos->x, offset_arrow_pos->y, offset_arrow_pos->z);
    J3DModel_setBaseTRMtx(mpModel, stack);
}
VERIFY(0x02053DDC, &daArrow_c::setStopActorMatrix);

/* 02054FE0 */
BOOL daArrow_c::procStop_BG() {
    WWHD_FUNC(0x02054FE0, BOOL, this);
    speedF = 0.0f;
    void* poly = mLinChk + 0x14; /* mLinChk's cBgS_PolyInfo */

    if (!cBgS_ChkPolySafe(dComIfG_Bgsp(), poly)) {
        fopAcM_delete(this);
        return TRUE;
    }

    BOOL temp2 = FALSE;

    s16 t = field_0x604;
    if (t > 0) {
        t = t - 1;
        f32 a = (f32)t * (1 / 40.0f);
        f32 base = (f32)(s16)field_0x6e6.x;
        f32 amp = (1024.0f * a) * a;
        field_0x604 = t;
        s16 sx = (s16)gabi::ftoi(gabi::fmadds(amp, cM_ssin(t * 0x52FB), base));
        s16 sy = field_0x6e6.y;
        shape_angle.x = sx;
        shape_angle.y = sy;
        temp2 = TRUE;
    } else if (field_0x600) {
        fopAcM_delete(this);
        return TRUE;
    }

    setBlur();

    if (dBgS_ChkMoveBG(dComIfG_Bgsp(), poly)) {
        dBgS_MoveBgTransPos(dComIfG_Bgsp(), poly, true, &field_0x6a8, &current.angle, &shape_angle);
        temp2 = TRUE;
    }

    if (temp2) {
        f32 xCos = cM_scos(shape_angle.x);
        f32 ax = 50.0f * cM_ssin(shape_angle.y);
        current.pos.x = gabi::fnmsubs(ax, xCos, field_0x6a8.x);
        current.pos.y = gabi::fmadds(50.0f, cM_ssin(shape_angle.x), field_0x6a8.y);
        f32 az = 50.0f * cM_scos(shape_angle.y);
        current.pos.z = gabi::fnmsubs(az, xCos, field_0x6a8.z);

        set_model_mtx(this);
    }

    if (mArrowType == TYPE_NORMAL) {
        cM3dGSph_SetC(&mCoSph.mSph, &current.pos);
        dComIfG_Ccsp_Set(&mCoSph);

        s32 n = field_0x6a0;
        if (n == 0) {
            field_0x600 = true;
            field_0x698 = false;
        } else {
            n--;
            field_0x6a0 = n;

            if (n < 60) {
                if ((n & 1) == 0) {
                    field_0x698 = false;
                } else {
                    field_0x698 = true;
                }
            } else {
                field_0x698 = true;
            }
        }

        if (mCoSph.ChkCoHit()) {
            /* dComIfGp_setItemArrowNumCount(1) */
            u32 p = dComIfGp_ea() + 0x5B68;
            gabi::store<s16>(p, (s16)(gabi::load<s16>(p) + 1));
            fopAcM_createItemForSimpleDemo(&current.pos, 0x10 /* dItemNo_ARROW_10_e */, -1, nullptr, nullptr, 0.0f, 0.0f);
            mDoAud_seStart_noPos(0x827 /* JA_SE_CONSUMP_ITEM_GET */);
            fopAcM_delete(this);
            return TRUE;
        }
    }

    if (mbSetByZelda) {
        field_0x600 = true;
    }

    return TRUE;
}
VERIFY(0x02054FE0, &daArrow_c::procStop_BG);

/* 02055450 */
BOOL daArrow_c::procReturn() {
    WWHD_FUNC(0x02055450, BOOL, this);
    f32 sy = speed.y - 2.0f;
    speedF = 0.0f;
    speed.y = sy;
    PSVECAdd(&current.pos, &speed, &current.pos);
    shape_angle.x = shape_angle.x + field_0x69c;
    gabi::Local<cXyz> step;
    cXyz_ml(&speed, step, 0.25f);
    gabi::Local<cXyz> quarterStepPos;
    cXyz_pl(&current.pos, quarterStepPos, step);
    field_0x699 = daPy_lk_c_setItemWaterEffect(this, field_0x699, 1);
    dBgS_LinChk_Set(mLinChk, &old.pos, quarterStepPos, this);
    setBlur();

    if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
        void* triPla = dBgS_GetTriPla(dComIfG_Bgsp(), mLinChk + 0x14);
        if (triPla) { /* HD: null check */
            f32 temp2 = vec_abs(&speed);
            gabi::Local<cXyz> temp1;
            C_VECReflect(&speed, triPla, temp1);
            speed.y = gabi::fmuls_ppc(gabi::fmuls_ppc(temp1->y, temp2), 0.5f);
            speed.x = gabi::fmuls_ppc(gabi::fmuls_ppc(temp1->x, temp2), 0.5f);
            speed.z = gabi::fmuls_ppc(gabi::fmuls_ppc(temp1->z, temp2), 0.5f);

            s32 temp3 = -field_0x69c;
            field_0x69c = (s16)(temp3 / 2);
            if (!(gabi::load<f32>(gabi::ea(triPla) + 4) < 0.5f)) { /* cBgW_CheckBGround(triPla->GetNP()->y) */
                field_0x69a = true;
            }
        }
    } else if (field_0x69a && speed.y < 0.0f) { // bge: unordered skips
        fopAcM_delete(this);
    }

    set_model_mtx(this);

    return TRUE;
}
VERIFY(0x02055450, &daArrow_c::procReturn);

/* HD: mCurrProcFunc == &daArrow_c::procWater (pointer-to-member compare) */
static inline bool proc_is(daArrow_c* a, u32 fn) {
    return a->mCurrProcFunc_vidx == -1 && a->mCurrProcFunc_delta == 0 && a->mCurrProcFunc_fn == fn;
}
/* bounce off: speed *= -0.1f; speed.y += speed.absXZ(); current.pos = old.pos */
static inline void rebound(daArrow_c* a) {
    a->setProc(A_procReturn);
    a->mParameters = 3; /* fopAcM_SetParam(this, 3) */
    PSVECScale(&a->speed, &a->speed, -0.1f);
    f32 xz = absXZ(a->speed.x, a->speed.z);
    f32 sy = gabi::fadds_ppc(a->speed.y, xz);
    a->current.pos.copy(a->old.pos);
    a->field_0x69c = 0x2C00;
    a->speed.y = sy;
}

/* 02054040 */
BOOL daArrow_c::procMove() {
    WWHD_FUNC(0x02054040, BOOL, this);
    speedF = 100.0f;
    PSVECAdd(&current.pos, &speed, &current.pos);
    gabi::Local<cXyz> step;
    cXyz_ml(&speed, step, 0.25f);
    gabi::Local<cXyz> sum;
    cXyz_pl(&current.pos, sum, step);
    gabi::Local<cXyz> quarterStepPos;
    quarterStepPos->copy(*sum);
    dBgS_LinChk_Set(mLinChk, &old.pos, quarterStepPos, this);
    u16 sy = shape_angle.y, sx = shape_angle.x, sz = shape_angle.z;
    field_0x6e6.z = sz;
    field_0x6e6.x = sx;
    field_0x6e6.y = sy;

    PSMTXCopy(mpModel ? gabi::at<Mtx34>(gabi::ea(mpModel) + 0xC8) : nullptr, &field_0x6b4);
    mBlurAngle.z = mBlurAngle.z + 0x889;

    void* poly = mLinChk + 0x14;
    cXyz* crossP = gabi::at<cXyz>(self(mLinChk) + 0x30); /* mLinChk.GetCrossP() */
    dCcD_GObjAt* at = &mAtCps.mGObjAt;
    gabi::Local<cXyz> hitPos;
    s32 hitType = 0; // No hit
    bool bg_check = true; /* HD: the paths join at the background line check */

    if (mAtCps.ChkAtHit()) {
        fopAc_ac_c* ac;
        if (mArrowType == TYPE_LIGHT && !mbLinkReflect && (at->mRPrm & 1) /* ChkAtShieldHit */ &&
            (ac = gabi::call<fopAc_ac_c*>(0x02515BBC, at)) != nullptr && fpcM_GetName(ac) == 0xA8 /* fpcNm_PLAYER_e */) {
            gabi::call<fopAc_ac_c*>(0x02515BBC, at); /* mAtCps.GetAtHitAc() */
            hitPos->copy(at->mHitPos);
            mbLinkReflect = true;
            ShieldReflect(); // Reflected hit
        } else {
            fopAc_ac_c* hitActor;
            BOOL hitWasBlocked;
            if (mbHitActor) {
                hitPos->copy(mAtHitPos);
                hitActor = gabi::at<fopAc_ac_c>(mpAtHitActor);
                hitWasBlocked = FALSE;
            } else {
                hitActor = gabi::call<fopAc_ac_c*>(0x02515BBC, at);
                hitPos->copy(at->mHitPos);
                hitWasBlocked = at->mRPrm & 1;
            }

            if (hitActor) {
                u32 jntHit = hitActor->jntHit;
                if (mArrowType == TYPE_LIGHT) {
                    fopAc_ac_c* a;
                    bool ganon = ((a = gabi::call<fopAc_ac_c*>(0x02515BBC, at)) != nullptr && fpcM_GetName(a) == 0xF3) ||
                                 ((a = gabi::call<fopAc_ac_c*>(0x02515BBC, at)) != nullptr && fpcM_GetName(a) == 0xF4) ||
                                 ((a = gabi::call<fopAc_ac_c*>(0x02515BBC, at)) != nullptr && fpcM_GetName(a) == 0xF5);
                    if (ganon && hitWasBlocked) {
                        // Hit Puppet Ganon (fpcNm_BGN_e / BGN2 / BGN3).
                        field_0x6a8.copy(*hitPos);
                        gabi::Local<cXyz> back;
                        cXyz_ml(&speed, back, 0.25f);
                        gabi::Local<cXyz> pos;
                        cXyz_mi(hitPos, pos, back);
                        current.pos.copy(*pos);

                        if (!field_0x6e4) {
                            dKy_arrowcol_chg_on(&current.pos, 2);
                        }

                        f32 vx = speed.x, vz = speed.z;
                        actor_status = actor_status | 0x4000; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
                        setProc(A_procStop_BG);
                        mParameters = 2;
                        field_0x604 = 0x28;
                        /* HD: no GetTriPla call */

                        gabi::Local<csXyz> temp10;
                        f32 xz = absXZ(vx, vz);
                        temp10->x = cM_atan2s(speed.y, xz);
                        temp10->y = cM_atan2s(speed.x, speed.z);
                        temp10->z = 0;
                        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
                        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), temp10->x, temp10->y, 0);
                        PSMTXCopy(mDoMtx_stack_c::get(), &field_0x6b4);

                        particle_setP1(0x2A1 /* ID_IT_JN_ARWG_HITA00 */, &field_0x6a8, temp10);
                        seStartCurrent(this, 0x69DC /* JA_SE_OBJ_LIGHT_ARW_EFF */, 0);
                        fopAcM_delete(this);
                    }
                    // otherwise: no hit (pass through)
                } else if (hitWasBlocked) {
                    hitType = 1; // Blocked hit
                    field_0x604 = 0x28;
                    actor_status = actor_status | 0x4000;
                } else if (jntHit) {
                    gabi::Local<cXyz> temp12;
                    gabi::Local<csXyz> temp11;
                    s32 jnt = gabi::call<s32>(0x025538E4, jntHit, hitPos.get(), &shape_angle, temp12.get(), temp11.get());
                    mHitJointIndex = jnt;
                    if (jnt >= 0) {
                        u16 rx = temp11->x, ry = temp11->y;
                        field_0x6e6.x = rx;
                        field_0x6e6.y = ry;
                        u32 a = gabi::load<u32>(self(temp12.get()) + 4), b = gabi::load<u32>(self(temp12.get()) + 8);
                        gabi::store<u32>(self(&field_0x618) + 4, a);
                        gabi::store<u32>(self(&field_0x618) + 8, b);
                        field_0x6e6.z = temp11->z;
                        actor_status = actor_status | 0x4000;
                        field_0x604 = 0x28;
                        hitType = 2; // Hit a joint
                        gabi::store<u32>(self(&field_0x618), gabi::load<u32>(self(temp12.get())));
                    } else if (jnt == -3 /* JntHitIdx_DELETE_e */) {
                        fopAcM_delete(this);
                        return TRUE;
                    }
                }
            }
        }
    }

    if (hitType > 0) {
        bg_check = false;
        if (gabi::load<u32>(self(mBlurFollowCb) + 4)) {
            dPa_followEcallBack_end(gabi::at<dPa_followEcallBack>(self(mBlurFollowCb))); /* remove() */
        }

        if (hitType == 1) { // Blocked hit
            rebound(this);
            set_model_mtx(this);
            seStartCurrent(this, 0x287A /* JA_SE_LK_ARROW_REBOUND */, 0x20);
        } else if (hitType == 2) { // Hit a joint
            setProc(A_procStop_Actor);
            mParameters = 2;

            u8 type = mArrowType;
            if (type == TYPE_FIRE) {
                seStartCurrent(this, 0x69DA /* JA_SE_OBJ_FIRE_ARW_EFF */, 0);
                field_0x698 = false;
            } else if (type == TYPE_ICE) {
                seStartCurrent(this, 0x69DB /* JA_SE_OBJ_ICE_ARW_EFF */, 0);
            } else if (type == TYPE_LIGHT) {
                seStartCurrent(this, 0x69DC /* JA_SE_OBJ_LIGHT_ARW_EFF */, 0);
                field_0x698 = false;
            }

            setStopActorMatrix();
        }
    }

    if (bg_check) {
        if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
            field_0x6a8.copy(*crossP);
            gabi::Local<cXyz> back;
            cXyz_ml(&speed, back, 0.25f);
            gabi::Local<cXyz> pos;
            cXyz_mi(crossP, pos, back);
            current.pos.copy(*pos);

            if (!check_water_in()) {
                u8 type = mArrowType;
                if (type >= 1 && type <= 3 && !field_0x6e4) {
                    /* switch (mArrowType): FIRE 0, ICE 1, LIGHT 2, default -1 */
                    dKy_arrowcol_chg_on(&current.pos, gabi::load<u8>(0x10007477 + type));
                }

                setProc(A_procStop_BG);
                field_0x604 = 0x28;
                mParameters = 2;
                actor_status = actor_status | 0x4000;
                void* triPla = dBgS_GetTriPla(dComIfG_Bgsp(), poly);

                gabi::Local<csXyz> temp10;
                if (triPla) { /* HD: null check */
                    u32 n = gabi::ea(triPla);
                    f32 xz = absXZ(gabi::load<f32>(n + 0), gabi::load<f32>(n + 8));
                    temp10->x = cM_atan2s(-gabi::load<f32>(n + 4), -xz);
                    temp10->y = cM_atan2s(-gabi::load<f32>(n + 0), -gabi::load<f32>(n + 8));
                    temp10->z = 0;
                } else {
                    temp10->y = 0;
                    temp10->z = 0;
                    temp10->x = 0;
                }
                mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
                mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), temp10->x, temp10->y, 0);
                PSMTXCopy(mDoMtx_stack_c::get(), &field_0x6b4);

                type = mArrowType;
                if (type == TYPE_FIRE) {
                    particle_setP1(0x29A /* ID_IT_JN_ARWF_HITA00 */, &field_0x6a8, temp10);
                    particle_setP1(0x29B /* ID_IT_JN_ARWF_HITB00 */, &field_0x6a8, temp10);
                    seStartCurrent(this, 0x69DA /* JA_SE_OBJ_FIRE_ARW_EFF */, 0);
                    field_0x698 = false;
                } else if (type == TYPE_ICE) {
                    if (dBgS_ChkGrpInf(dComIfG_Bgsp(), poly, 0x200)) {
                        fopAcM_create(0x2A /* fpcNm_Obj_Magmarock_e */, 0, &field_0x6a8, current.roomNo, nullptr, nullptr, -1, 0);
                    } else {
                        particle_setP1(0x29E /* ID_IT_JN_ARWI_HITA00 */, &field_0x6a8, temp10);
                        fopAcM_createChild(0x1D9 /* fpcNm_ARROW_ICEEFF_e */, gabi::load<u32>(self(this) + 4), mArrowType,
                                           &field_0x6a8, current.roomNo, &field_0x6e6, nullptr, -1, 0);
                        seStartCurrent(this, 0x69DB /* JA_SE_OBJ_ICE_ARW_EFF */, 0);
                    }
                } else if (type == TYPE_LIGHT) {
                    particle_setP1(0x2A1 /* ID_IT_JN_ARWG_HITA00 */, &field_0x6a8, temp10);
                    seStartCurrent(this, 0x69DC /* JA_SE_OBJ_LIGHT_ARW_EFF */, 0);
                    field_0x698 = false;
                }

                u32 attribCode = dBgS_GetAttributeCode(dComIfG_Bgsp(), poly);
                u32 mtrlSndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo*)poly);

                if (mArrowType == TYPE_NORMAL &&
                    (attribCode == 3 /* dBgS_Attr_STONE_e */ || attribCode == 9 /* METAL */ || attribCode == 0xF /* ICE */ ||
                     (attribCode >= 0x14 && attribCode <= 0x15) /* DAMAGE, FREEZE */)) {
                    rebound(this);
                    gabi::Local<csXyz> temp9;
                    cM3d_CalcVecZAngle(triPla, temp9); /* *triPla->GetNP() */

                    particle_setP1(0xC /* ID_AK_JN_NG */, &field_0x6a8, temp9);
                    seStartCurrent(this, 0x287A /* JA_SE_LK_ARROW_REBOUND */, mtrlSndId);
                } else {
                    seStartCurrent(this, 0x2879 /* JA_SE_LK_ARROW_HIT */, mtrlSndId);
                }
            }
        } else {
            check_water_in();
        }
    }

    if (proc_is(this, A_procWater)) {
        set_model_mtx(this);
    } else {
        gabi::Local<cXyz> d;
        cXyz_mi(&current.pos, d, &field_0x6a8);
        if (vec_abs(d) > 25000.0f) { // ble: unordered skips
            f32 vy = speed.y - 2.0f;
            if (vy < -100.0f) { // bge
                vy = -100.0f;
            }
            f32 posY = current.pos.y;
            f32 startY = field_0x6a8.y;
            speed.y = vy;
            if (startY > posY) { // ble
                fopAcM_delete(this);
                return TRUE;
            }
        } else {
            gabi::Local<cXyz> d2;
            cXyz_mi(&current.pos, d2, &field_0x6a8);
            if (vec_abs(d2) > 20000.0f) { // ble
                field_0x6e4 = true;
            }
        }

        createBlur();

        set_model_mtx(this);

        gabi::Local<cXyz> step2;
        cXyz_ml(&speed, step2, 1.25f);
        cXyz_pl(&current.pos, d, step2);
        gabi::Local<cXyz> end;
        end->copy(*d);
        at_cps_set(this, end);
    }

    return TRUE;
}
VERIFY(0x02054040, &daArrow_c::procMove);

/* 02052DB8 */
BOOL daArrow_c::_execute() {
    WWHD_FUNC(0x02052DB8, BOOL, this);
    if (mbSetByZelda) {
        if (!mbLinkReflect) {
            fopAc_ac_c* link = dComIfGp_getLinkPlayer();
            /* daPy_py_c::checkPlayerGuard(): virtual (slot 0x3C) */
            if (gabi::call_ptr<BOOL>(vfunc(link, 0x3C), link)) {
                mAtCps.SetAtSpl(0); /* dCcG_At_Spl_UNK0 */
                mAtCps.SetAtType(AT_TYPE_NORMAL_ARROW);
            } else {
                mAtCps.SetAtSpl(0xB); /* dCcG_At_Spl_UNKB */
                mAtCps.SetAtType(AT_TYPE_LIGHT_ARROW);
            }
        } else {
            mAtCps.SetAtSpl(0);
            mAtCps.SetAtType(AT_TYPE_NORMAL_ARROW);
        }
    }

    s16 sparkle = mSparkleTimer;
    if (sparkle != 0) {
        mSparkleTimer = sparkle - 1;
        fopAc_ac_c* player = dComIfGp_getPlayer(0);

        gabi::Local<cXyz> offset;
        offset->x = 0.0f;
        offset->y = gabi::fadds_ppc(REG0_F(8), 45.0f);
        offset->z = gabi::fadds_ppc(REG0_F(9), 30.0f);
        mDoMtx_YrotS(calc_mtx(), player->shape_angle.y);
        gabi::Local<cXyz> offsetOut;
        MtxPosition(offset, offsetOut);
        gabi::Local<cXyz> pos;
        cXyz_pl(&player->current.pos, pos, offsetOut);
        f32 px = pos->x, py = pos->y, pz = pos->z;
        current.pos.y = py;
        current.pos.z = pz;
        current.pos.x = px;

        mDoMtx_stack_c::transS(px, py, pz);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, 0);
        J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());

        u32 e = mpSparkleEmitter;
        if (!e) {
            return TRUE;
        }
        s16 t = mSparkleTimer;
        if (t != 0) {
            f32 scaleMag = (f32)t;
            scaleMag = scaleMag + scaleMag; /* mSparkleTimer * 2.0f */
            if (scaleMag > 7.0f) {
                scaleMag = 7.0f;
            }
            /* mpSparkleEmitter->setGlobalTranslation(current.pos) (HD: y negated from version 7 on) */
            f32 tz = current.pos.z, ty = current.pos.y, tx = current.pos.x;
            if (gabi::load<u8>(e + 0x262) >= 7) {
                ty = -ty;
            }
            gabi::store<f32>(e + 0x22C, tx);
            gabi::store<f32>(e + 0x230, ty);
            gabi::store<f32>(e + 0x234, tz);
            /* setGlobalScale(scaleVec) */
            e = mpSparkleEmitter;
            gabi::store<f32>(e + 0x220, scaleMag);
            gabi::store<f32>(e + 0x224, scaleMag);
            gabi::store<f32>(e + 0x228, scaleMag);
            gabi::store<f32>(e + 0x238, scaleMag);
            gabi::store<f32>(e + 0x23C, scaleMag);
            gabi::store<f32>(e + 0x240, scaleMag);
            return TRUE;
        }
        /* becomeInvalidEmitter() */
        u32 flags = gabi::load<u32>(e + 0x254);
        gabi::store<s32>(e + 0x5C, -1);
        gabi::store<u32>(e + 0x254, flags | 1);
        mpSparkleEmitter = 0;
    }

    if ((u32)(s32)field_0x602 == (u32)(s32)gabi::load<s16>(M_COUNT)) {
        field_0x600 = true;
    }

    f32 frame = mBtkFrame;
    if (frame > 0.0f) { // ble: unordered takes the else branch
        frame = frame + 1.0f;
        mBtkFrame = frame;
        if (!(frame < (f32)gabi::load<s16>(mpBtk + 0xA))) { /* mpBtk->getFrameMax() */
            mBtkFrame = 0.0f;
        }
    } else {
        if (cM_rnd_l() < 0.02f) { // bge: unordered skips
            mBtkFrame = mBtkFrame + 1.0f;
        }
    }

    setLightEffect();

    /* if (mCurrProcFunc) (this->*mCurrProcFunc)() */
    s16 vidx = mCurrProcFunc_vidx;
    if (vidx != 0) {
        u32 p = self(this) + mCurrProcFunc_delta;
        if (vidx < 0) {
            gabi::call_ptr(mCurrProcFunc_fn, gabi::at<void>(p));
        } else {
            u32 vt = gabi::load<u32>(p + gabi::load<s16>(self(this) + 0x7AE));
            gabi::call_ptr(gabi::load<u32>(vt + (u32)vidx * 8 + 4), gabi::at<void>(p));
        }
    }

    u32 cx = gabi::load<u32>(self(&current.pos)), cy = gabi::load<u32>(self(&current.pos) + 4),
        cz = gabi::load<u32>(self(&current.pos) + 8);
    u32 ap = self(this) + 0x390; /* attention_info.position */
    gabi::store<u32>(ap, cx);
    gabi::store<u32>(ap + 4, cy);
    gabi::store<u32>(ap + 8, cz);
    gabi::store<u32>(self(&eyePos), cx);
    gabi::store<u32>(self(&eyePos) + 4, cy);
    gabi::store<u32>(self(&eyePos) + 8, cz);
    setRoomInfo();

    return TRUE;
}
VERIFY(0x02052DB8, &daArrow_c::_execute);

/* 02053184 daArrowExecute: a branch to _execute */
static BOOL daArrowExecute(void* i_this) {
    WWHD_FUNC(0x02053184, BOOL, i_this);
    return static_cast<daArrow_c*>(i_this)->_execute();
}
VERIFY(0x02053184, daArrowExecute);

/* 02055A80 daArrowIsDelete */
static BOOL daArrowIsDelete(void*) {
    WWHD_FUNC(0x02055A80, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02055A80, daArrowIsDelete);

/* 02055B64 cLib_calcTimer<s16> (this TU's copy) */
static s16 cLib_calcTimer(be<s16>* t) {
    WWHD_FUNC(0x02055B64, s16, t);
    s16 v = *t;
    if (v != 0) {
        v = v - 1;
        *t = v;
    }
    return v;
}
VERIFY(0x02055B64, cLib_calcTimer);

/* 02055B60: an empty function (compiler-generated, probably an empty virtual of a callback class) */
static void emptyFunc() {
    WWHD_FUNC(0x02055B60, void);
}
VERIFY(0x02055B60, emptyFunc);

/* 02055A6C: deleting destructor of an empty class (only operator delete) */
static void emptyClass_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02055A6C, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x02055A6C, emptyClass_dt);

/* 02055A88 daArrow_c::~daArrow_c (deleting destructor, HD virtual) */
static void daArrow_dt(daArrow_c* p, s32 flags) {
    WWHD_FUNC(0x02055A88, void, p, flags);
    if (p == nullptr) return;
    u32 a = self(p);
    gabi::call(0x02515AE8, gabi::at<void>(a + 0x5F0), 2); /* dCcD_Sph::~dCcD_Sph */
    gabi::call(0x02515980, gabi::at<void>(a + 0x4B8), 2); /* dCcD_Cps (dCcD_GObjInf::~dCcD_GObjInf) */
    gabi::call(0x02515860, gabi::at<void>(a + 0x47C), 2); /* dCcD_Stts::~dCcD_Stts */
    /* ~dBgS_ObjGndChk: this TU's vtables, then cBgS_Chk::~cBgS_Chk */
    gabi::store<u32>(a + 0x448, 0x10007268);
    gabi::store<u32>(a + 0x474, 0x10007248);
    gabi::store<u32>(a + 0x468, 0x10007288);
    gabi::call(0x02008DAC, gabi::at<void>(a + 0x428), 0);
    /* ~dBgS_ArrowLinChk */
    gabi::store<u32>(a + 0x420, 0x10007248);
    gabi::store<u32>(a + 0x414, 0x10007308);
    gabi::store<u32>(a + 0x3DC, 0x10007238);
    gabi::call(0x02008B4C, gabi::at<void>(a + 0x3BC), 0); /* cBgS_LinChk::~cBgS_LinChk */
    gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) {
        operator_delete(p);
    }
}
VERIFY(0x02055A88, daArrow_dt);

/* 020559B8 __sinit_d_a_arrow_cpp */
static void sinit_d_a_arrow() {
    WWHD_FUNC(0x020559B8, void);
    sinit_header_statics(0x1046146C, 0x1018FDC4);
    /* the radii of m_at_cps_src (5.0f) and m_co_sph_src (25.0f) */
    gabi::store<f32>(0x1018FEA4, 5.0f);
    gabi::store<f32>(0x1018FE58, 25.0f);
}
VERIFY(0x020559B8, sinit_d_a_arrow);
