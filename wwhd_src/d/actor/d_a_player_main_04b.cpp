/**
 * d_a_player_main_04b.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #04, second half (..): the create phases,
 * constructor, playerInit, makeBgWait, the emitter callbacks, and the first move/wait/ladder procs.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Canonical GameCube methods use the shared Link declarations; HD adapters retain the fixed GHS ABI.
 */
#include "d/actor/d_a_player_main_04b.h"
#include "d/d_com_lib_game.h"

static void* daPy_lk_ct(void*);
static void* daPy_sightPacket_ct(void*);
static void* daPy_swBlur_ct(void*);


// HD: Picto Box selfie expressions and pad directions. This source owns their sole definitions.
static player04b::SelfiePose l_selfie_pose[11] = {
    {0x0, 0.0f, 1},
    {0x69, 12.0f, 0},
    {0x36, 0.0f, 0},
    {0x42, 0.0f, 0},
    {0x99, 20.0f, 0},
    {0x27, 0.0f, 0},
    {0x69, 20.0f, 0},
    {0x7E, 0.0f, 0},
    {0x90, 0.0f, 0},
    {0x94, 0.0f, 0},
    {0x92, 0.0f, 1},
};
static GXColor n_color0 = {0xFF, 0xFF, 0xFF, 0x96};
static GXColor y_color0 = {0xFF, 0xFF, 0x7B, 0x96};
static GXColor g_color0 = {0x5A, 0xFF, 0x5A, 0x96};
static u32 l_selfie_pad[4] = {0x110000u, 0x220000u, 0x880000u, 0x440000u};

static cPhs__Handler l_method[4] = {{0x0240EE2C}, {0x02412884}, {0x02413E68}, {0}};
// HD: these guarded function statics occupy guest BSS and retain their original initialization order.
static u32 l_tail_scale_guard;
static cXyz l_tail_scale;
static u32 l_right_dir_guard;
static cXyz l_right_dir;

// HD: fixed ABI helpers for the renderer and explicit GHS construction.
#define LK_SAFESTRING_VTBL 0x10034B24u /* sead::SafeString vtable of this translation unit */

/* sead::SafeString {const char*; vtable}; assureTermination is virtual (+0x14) */
static inline void lk_ss_assure(player04b::SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
/* inline sead::SafeString::operator== */
static inline bool lk_ss_cmp(player04b::SafeString* a, player04b::SafeString* b) {
    lk_ss_assure(a);
    lk_ss_assure(a);
    u32 pa = a->mpString;
    lk_ss_assure(b);
    u32 pb = b->mpString;
    if (pa == pb)
        return true;
    pa = a->mpString;
    pb = b->mpString;
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::load<u8>(pa + n);
        u8 cb = gabi::load<u8>(pb + n);
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (HD: SafeString comparison; start stage name at play + 0x5134) */
static inline bool lk_isStartStage(u32 lit) {
    gabi::Local<player04b::SafeString> a;
    a->__vtbl = LK_SAFESTRING_VTBL;
    a->mpString = lit;
    u32 play = dComIfGp_ea();
    gabi::Local<player04b::SafeString> b;
    b->__vtbl = LK_SAFESTRING_VTBL;
    b->mpString = play + 0x5134;
    return lk_ss_cmp(a.get(), b.get());
}
/* bits of an f32 value */
static inline u32 lk_fbits(f32 f) {
    u32 v;
    memcpy(&v, &f, 4);
    return v;
}
/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void copyFloatBits(u32 dst, u32 src) { gmem_stf32(dst, gmem_ld32(src)); }
static inline void copyFloatBits(be<f32>& dst, const be<f32>& src) {
    copyFloatBits(gabi::ea(&dst), gabi::ea(&src));
}

/* daPy_lk_c members in the opaque blocks of the shared header (offsets measured in this range) */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define LK_demoMode() LK_FIELD(u32, 0x430) /* mDemo.getDemoMode() (GameCube 0x314) */

/* ---- daPy_lk_c functions of other ranges (by address) ---- */
enum : u32 {
    LK_getDirectionFromCurrentAngle = 0x023E3398,
    LK_setSpeedAndAngleNormal = 0x0241650C,
    LK_setNormalSpeedF = 0x02416230,
    LK_setBlendMoveAnime = 0x023E14A0,
    LK_setBlendAtnMoveAnime = 0x023E81E4,
    LK_setBlendAtnBackMoveAnime = 0x023E7E5C,
    LK_checkAtnWaitAnime = 0x023F0CE4,
    LK_checkNextMode = 0x023F14E0,
    LK_changeFrontWallTypeProc = 0x02418E00,
    LK_checkIceSlipFall = 0x024197A0,
    LK_checkBowAnime = 0x023D6A18,
    LK_checkNextActionBowReady = 0x023EB208,
    LK_itemTrigger = 0x023EAB40,
    LK_getReadyItem = 0x023EAA80,
    LK_setBowReadyAnime = 0x023E7BEC,
    LK_checkNextActionBoomerangReady = 0x023EB510,
    LK_setActAnimeUpper = 0x023DE7E8,
    LK_setShipRidePosUseItem = 0x023E2DC4,
    LK_procShipPaddle_init = 0x023F1E60,
    LK_commonProcInit = 0x023DFDD8,
    LK_setShipRidePos = 0x023E287C,
    LK_setSingleMoveAnime = 0x023E0A04,
    LK_getSwimTimerRate = 0x023F8858,
    LK_deleteEquipItem = 0x023DC7AC,
    LK_getDirectionFromShapeAngle = 0x023E4E18,
    LK_checkHeavyStateOn = 0x023DBC24,
    LK_getSlidePolygon = 0x023E4A58,
    LK_changeSlideProc = 0x023E4CD0,
    LK_procCrouch_init = 0x023EE2B8,
    LK_procCrouchDefense_init = 0x023EE510,
    LK_changeWaitProc = 0x023E32AC,
    LK_getDirectionFromAngle = 0x023E3358,
    LK_setTextureAnime = 0x023DD768,
    LK_procWait_init = 0x023E2FF4,
    LK_checkRestHPAnime = 0x023DD12C,
    LK_setOldRootQuaternion = 0x023E278C,
    LK_checkEquipAnime = 0x023D794C,
    LK_resetActAnimeUpper = 0x023DC6A4,
    LK_setAnimeUnequip = 0x023ECEF0,
    LK_setFrontWallType = 0x023E3850,
    LK_procFall_init = 0x023F6564,
    LK_checkShipRideUseItem = 0x023E26EC,
    LK_initShipRideUseItem = 0x023E2E18,
};
/* play + 0x5CD8: dComIfGp_checkPlayerStatus0(0, flag) */

/* play + 0x5CD8: dComIfGp_setPlayerStatus0(0, flag) */
/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */
#define LK_voiceStart(n) gabi::call_ptr(gabi::load<u32>(__vtbl + 0xE4), this, (u32)(n))
/* camera attention status of a camera info index: play + 0x5B00 + idx * 0x34 */
/*  cLib_addCalc(f32* value, f32 target, f32 scale, f32 maxStep, f32 minStep) */
static inline f32 cLib_addCalc_l(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return player04bAddCalc(v, target, scale, maxStep, minStep);
}
/* cBgS::GetTriPla(bg, poly) of a cBgS_PolyInfo; the cM3dGPla normal at +0 */
static inline u32 dBgS_GetTriPla_l(u32 poly) {
    return player04bGetTrianglePlane(dComIfG_Bgsp(), (u32)gabi::load<u16>(poly + 2), (u32)gabi::load<u16>(poly + 0));
}
static inline s32 dBgS_GetSpecialCode_l(u32 poly) { return player04bGetSpecialCode(dComIfG_Bgsp(), poly); }
#define mLinkLinChkPoly (gabi::ea(this) + 0x9E4) /* mLinkLinChk's cBgS_PolyInfo */
#define mLinkLinChkCross (*gabi::at<cXyz>(gabi::ea(this) + 0xA00)) /* mLinkLinChk.GetCross() (+0x30) */
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline Mtx34* lk_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30);
}
#define mBodyAngleX LK_FIELD(s16, 0x3D0) /* daPy_py_c mBodyAngle.x (GameCube 0x2B4) */
#define mGndChkPos (*gabi::at<cXyz>(gabi::ea(this) + 0xB38)) /* mGndChk's position (dBgS_GndChk + 0x24) */
/* daPy_lk_c::checkAttentionLock(): mpAttention->LockonTruth() or its flag  (+0x20) */
static inline bool lk_checkAttentionLock(daPy_lk_c* p) {
    u32 atn = p->mpAttention;
    return player04bAttentionLockonTruth(atn) || (gabi::load<u32>(atn + 0x20) & 0x20000000);
}
/*  mDoExt_MtxCalcOldFrame::initOldFrameMorf(f32 morf, u16 start, u16 end) on m_old_fdata */
static inline void lk_initOldFrameMorf(daPy_lk_c* p, f32 morf, u32 start, u32 end) {
    player04bInitOldFrameMorf((u32)p->m_old_fdata, morf, start, end);
}
/* mEquipItem == getReadyItem() (the item is loaded after the call) */
static inline bool lk_equipIsReadyItem(daPy_lk_c* p) {
    u32 ready = player04bGetReadyItemRegister(p);
    return (u32)p->mEquipItem == ready;
}
static inline bool JPA_isEnableDeleteEmitter(u32 e) {
    return gabi::at<player04b::Emitter>(e)->isEnableDeleteEmitter();
}

/* daPy_followEcallBack_c (HD: vtable at +0) */


/* daPy_swimTailEcallBack_c (HD: vtable at +0) */

/* daPy_fanSwingEcallBack_c (HD: vtable at +0) */

/* daPy_mtxPosFollowEcallBack_c (HD: vtable at +0) */


/* 0240EBB0 */
static BOOL daPy_Execute(daPy_lk_c* i_this) {
    return i_this->execute();
}

/* 0240EBB4 */
static BOOL daPy_IsDelete(daPy_lk_c*) {
    return TRUE;
}

/* 0240EBBC */
void daPy_swimTailEcallBack_c::remove() {
    if (mpEmitter != 0) {
        gabi::at<player04b::Emitter>(mpEmitter)->mpEmitterCallBack = 0; /* setEmitterCallBackPtr(NULL) */
        u32 e = mpEmitter;                        /* becomeInvalidEmitter (HD: also a -1 at +0x5C) */
        gabi::at<player04b::Emitter>(e)->mMaxFrame = -1;
        gabi::at<player04b::Emitter>(e)->mFlags = gabi::at<player04b::Emitter>(e)->mFlags | 1;
        mpEmitter = 0;
    }
}

/* 0240EE28 */
static BOOL daPy_Delete(daPy_lk_c* i_this) {
    return i_this->playerDelete();
}

/* 0240EE2C */
static cPhs_State phase_1(daPy_lk_c* i_this) {
    player04b::playerRegistry().mpPlayer[0] = i_this; /* dComIfGp_setPlayer(0, i_this) */
    player04b::playerRegistry().mpLinkPlayer = i_this; /* dComIfGp_setLinkPlayer(i_this) */
    fopAcM_setStageLayer(i_this);
    i_this->attention_info.flags = 0xFFFFFFFF; /* attention_info.flags */
    i_this->attention_info.position.x = i_this->current.pos.x; /* attention_info.position */
    i_this->attention_info.position.y = i_this->current.pos.y + 125.0f;
    i_this->attention_info.position.z = i_this->current.pos.z;
    return cPhs_NEXT_e;
}

/* 02412884 */
static cPhs_State phase_2(daPy_lk_c* i_this) {
    s32 camId = dComIfGp_getPlayerCameraID(0); /* dComIfGp_getPlayerCameraID(0) */
    if (dComIfGp_getCamera(camId) == nullptr) { /* dComIfGp_getCamera(..) */
        return cPhs_INIT_e;
    }
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) { /* fopAcM_ct(i_this, daPy_lk_c) */
        if (i_this != NULL) {
            daPy_lk_ct(i_this);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    i_this->playerInit();
    // HD: notify the HD system object's registered child
    u32 sys = gabi::load<u32>(0x101F8344);
    if (sys != 0) {
        u32 o = gabi::load<u32>(sys + 0x218);
        if (o != 0) {
            player04bNotifySystemChild(o);
        }
    }
    return cPhs_NEXT_e;
}

/* 02413E68 */
static cPhs_State phase_3(daPy_lk_c* i_this) {
    return (cPhs_State)i_this->makeBgWait();
}

/* 02413E6C */
static cPhs_State daPy_Create(fopAc_ac_c* i_this) {
    auto* player = static_cast<daPy_lk_c*>(i_this);
    return dComLbG_PhaseHandler(&player->mPhase, l_method, player);
}

/* 024140F8 */
void daPy_followEcallBack_c::execute(JPABaseEmitter* emitter) {
    auto* effect = player04b::emitterView(emitter);
    f32 x = mPos.x;
    f32 y = mPos.y;
    f32 z = mPos.z;
    if (effect->mResourceVersion >= 7) { /* HD: the emitter's Y is mirrored for these resource versions */
        y = -y;
    }
    effect->mGlobalTranslation.y = y; /* setGlobalTranslation */
    effect->mGlobalTranslation.x = x;
    effect->mGlobalTranslation.z = z;
    /* setGlobalRotation */
    JPAGetXYZRotateMtx(mAngle.x, mAngle.y, mAngle.z, &effect->mGlobalRotation);
}

/* 0241413C */
void daPy_followEcallBack_c::setup(JPABaseEmitter* emitter, const cXyz*, const csXyz*, s8) {
    mpEmitter = gabi::ea(emitter);
}

/* 02414144 */
void daPy_waterDropEcallBack_c::execute(JPABaseEmitter* emitter) {
    daPy_followEcallBack_c::execute(emitter);
    if (JPA_isEnableDeleteEmitter(gabi::ea(emitter))) {
        end();
    }
}

/* 024141A0 */
void daPy_waterDropEcallBack_c::setup(JPABaseEmitter* emitter, const cXyz*, const csXyz*, s8) {
    mpEmitter = gabi::ea(emitter);
    auto* effect = player04b::emitterView(emitter);
    effect->mFlags = effect->mFlags | 0x40;
}

/* 024141B4 */
void daPy_fanSwingEcallBack_c::execute(JPABaseEmitter* emitter) {
    auto* effect = player04b::emitterView(emitter);
    u32 e = gabi::ea(emitter);
    if (JPA_isEnableDeleteEmitter(e)) {
        effect->mFlags = effect->mFlags & ~0x40u; /* quitImmortalEmitter */
        effect->mpEmitterCallBack = 0;
        mpEmitter = 0;
    } else if (mAlphaOutFlg != FALSE) {
        gabi::Local<be<s16>> alpha;
        *alpha = (s16)effect->mGlobalPrmColor.a;
        cLib_chaseS(alpha.get(), 0, 50);
        s16 a = *alpha;
        effect->mGlobalPrmColor.a = (u8)a;
        if (a == 0) {
            effect->mFlags = effect->mFlags & ~0x40u;
            effect->mpEmitterCallBack = 0;
            mpEmitter = 0;
        }
    }
}

/* 02414DD8 */
void daPy_swimTailEcallBack_c::getMaxWaterY(cXyz* pos) {
    if (daSea_ChkArea(pos->x, pos->z)) {
        pos->y = daSea_calcWave(pos->x, pos->z) + 2.0f;
        if (mWaterFlatY > pos->y) {
            pos->y = mWaterFlatY + 2.0f;
        }
    } else if (mWaterFlatY != -1000000000.0f /* -G_CM3D_F_INF */) {
        pos->y = mWaterFlatY + 2.0f;
    } else {
        pos->y = mWaterY;
    }
}

/* 02415134 */
void daPy_swimTailEcallBack_c::draw(JPABaseEmitter*) {
    // HD: empty (GameCube: GXSetZMode when dPa_control_c::isStatus(1))
}

// HD: the material parameter block passed to the emitter by the HD setup (0281E5A8)

/* 02415138 */
void daPy_swimTailEcallBack_c::setup(JPABaseEmitter* emitter, const cXyz* pos, const csXyz* angle, s8) {
    mpEmitter = gabi::ea(emitter);
    field_0x20 = gabi::ea(angle);
    field_0x05 = 0;
    mbEnd = 0;
    // HD: the emitter's material parameter "jparticle_type_b"
    gabi::Local<player04b::EmitterMaterialParam> prm;
    prm->mName = gabi::guest_addr("jparticle_type_b");
    prm->__vtbl = LK_SAFESTRING_VTBL;
    prm->mValues[0] = 1.0f;
    prm->mValues[1] = 1.0f;
    prm->mValues[2] = 1.0f;
    prm->mValues[3] = 1.0f;
    prm->mOptions[0] = 1;
    prm->mOptions[1] = 0;
    prm->mOptions[2] = 0;
    prm->mOptions[3] = 0;
    player04bSetEmitterMaterial(emitter, prm.get());
}

/* 024151B8 */
void daPy_mtxPosFollowEcallBack_c::execute(JPABaseEmitter* emitter) {
    Mtx34* mtx = gabi::at<Mtx34>(mpMtx);
    mDoMtx_stack_c::transS(mtx->m[0][3], mtx->m[1][3], mtx->m[2][3]);
    if (mpAngle != 0) {
        mDoMtx_stack_c::YrotM(gabi::load<s16>(mpAngle + 2));
    }
    /* setGlobalRTMatrix */
    auto* effect = player04b::emitterView(emitter);
    JPASetRMtxTVecfromMtx(mDoMtx_stack_c::get(), &effect->mGlobalRotation, &effect->mGlobalTranslation);
}

/* 02417FF4 */
f32 daPy_lk_c::getLadderMoveAnmSpeed() {
    return 0.5f + mStickDistance; /* HD: getAnmSpeedStickRate(HIO) folded */
}

/* 02412F2C */
f32 daPy_lk_c::getCrawlMoveAnmSpeed() {
    if (mProcVar6 != 0) {
        return gabi::fmadds(1.5f, mStickDistance, 0.5f); /* getAnmSpeedStickRate(0.5f, 2.0f) */
    }
    f32 s = mStickDistance;
    return 1.0f + (s + s); /* HD: getAnmSpeedStickRate(HIO) folded */
}

/* 0241699C */
void daPy_lk_c::setSpeedAndAngleAtnBack() {
    f32 f1;
    if (mStickDistance > 0.05f) {
        if (getDirectionFromCurrentAngle() == player04b::DIR_BACKWARD) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            mNormalSpeed = -mNormalSpeed;
        }
        s16 origAngleY = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, m34E8, 6, 0xBB8, 0x7D0); /* HD: HIO folded */
        f1 = (2.5f * mStickDistance) * cM_scos((s16)(current.angle.y - origAngleY));
    } else {
        f1 = 0.0f;
    }
    shape_angle.y = m34E6;
    setNormalSpeedF(f1, 0.5f, 8.0f, 2.0f);
}

/* : HD, new: the direction/angle part of setSpeedAndAngleAtn without the speed (used by
 * procSubjectivity) */
/* 02416AB4 */
void daPy_lk_c::setAngleAtnHD() {
    u8 dir = mDirection;
    if (dir == player04b::DIR_FORWARD) {
        setSpeedAndAngleNormal(0xBB8);
        return;
    }
    if (dir == player04b::DIR_BACKWARD) {
        setSpeedAndAngleAtnBack();
        return;
    }
    if (mStickDistance > 0.05f) {
        int d = getDirectionFromCurrentAngle();
        s16 target = m34E8;
        if (d == 1) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            mNormalSpeed = -mNormalSpeed;
        }
        cLib_addCalcAngleS(&current.angle.y, target, 6, 0xBB8, 0x7D0);
    }
}

/* : HD, new (the matcher's "setSpeedAndAngleAtnActor"; that one is ): the
 * attention move of setSpeedAndAngleAtnActor without the shape angle */
/* 02416B70 */
void daPy_lk_c::setSpeedAndAngleAtnNoShapeHD() {
    if (mStickDistance > 0.05f) {
        if (getDirectionFromCurrentAngle() == 1) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            mNormalSpeed = -mNormalSpeed;
        }
        s16 origAngleY = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, m34E8, 6, 0xBB8, 0x7D0);
        f32 f1 = (5.0f * mStickDistance) * cM_scos((s16)(current.angle.y - origAngleY));
        setNormalSpeedF(f1, 0.5f, 7.5f, 4.0f);
    } else {
        setNormalSpeedF(0.0f, 0.5f, 7.5f, 4.0f);
    }
}

/* : HD, new: the move animation of the subjective (first person) mode */
/* 02416FA8 */
void daPy_lk_c::setBlendSubjectMoveAnimeHD() {
    u32 flg = mModeFlg;
    if (std::fabs((f32)mNormalSpeed) < 0.001f) {
        mModeFlg = flg | player04b::ModeFlg_00000001;
    } else {
        mModeFlg = flg & ~(u32)player04b::ModeFlg_00000001;
    }
    s32 a = m34DC;
    if (a < 0) {
        a = -a;
    }
    if (a >= 0x2000) {
        setBlendAtnBackMoveAnime(2.4f);
    } else {
        setBlendMoveAnime(2.4f);
    }
}

/* 02417388 */
BOOL daPy_lk_c::procControllWait() {
    if (dComIfGp_checkPlayerStatus0(0, player04b::daPyStts0_SHIP_RIDE_e)) {
        setShipRidePosUseItem();
        if (gabi::ea(player04b::playerRegistry().mpPlayer[0].get()) == gabi::ea(this)) { /* daPy_getPlayerActorClass() == this */
            procShipPaddle_init();
        }
    } else {
        checkNextMode(0);
    }
    return TRUE;
}

/* 02417538 */
void daPy_lk_c::setSpeedAndAngleAtn() {
    if (mDirection == player04b::DIR_FORWARD) {
        setSpeedAndAngleNormal(0xBB8); /* HD: m_HIO->mMove.m.field_0x0 folded */
        return;
    }
    if (mDirection == player04b::DIR_BACKWARD) {
        setSpeedAndAngleAtnBack();
        return;
    }
    f32 f1;
    if (mStickDistance > 0.05f) {
        if (getDirectionFromCurrentAngle() == 1) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            mNormalSpeed = -mNormalSpeed;
        }
        s16 origAngleY = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, m34E8, 6, 0xBB8, 0x7D0);
        f1 = (5.0f * mStickDistance) * cM_scos((s16)(current.angle.y - origAngleY));
    } else {
        f1 = 0.0f;
    }
    shape_angle.y = m34E6;
    setNormalSpeedF(f1, 0.5f, 7.5f, 4.0f);
}

/* 02419BF0 */
BOOL daPy_lk_c::procAtnMove() {
    setSpeedAndAngleAtn();
    if (!checkNextMode(0) &&
        (mDirection != player04b::DIR_FORWARD || !changeFrontWallTypeProc()) &&
        !checkIceSlipFall()) {
        setBlendAtnMoveAnime(-1.0f);
    }
    return TRUE;
}

/* 02419C70 */
void daPy_lk_c::setShapeAngleToAtnActor() {
    if (mpAttnActorLockOn != NULL) {
        s16 targetAngle = cLib_targetAngleY(&current.pos, &mpAttnActorLockOn->eyePos);
        cLib_addCalcAngleS(&shape_angle.y, targetAngle, 2, 0x2000, 0x800);
    }
}

/*  (not named by the matcher, which put the name on ) */
/* 02419CC8 */
void daPy_lk_c::setSpeedAndAngleAtnActor() {
    f32 f1;
    if (mStickDistance > 0.05f) {
        if (getDirectionFromCurrentAngle() == player04b::DIR_BACKWARD) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            mNormalSpeed = -mNormalSpeed;
        }
        s16 origAngleY = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, m34E8, 6, 0xBB8, 0x7D0);
        f1 = (5.0f * mStickDistance) * cM_scos((s16)(current.angle.y - origAngleY));
    } else {
        f1 = 0.0f;
    }
    setShapeAngleToAtnActor();
    setNormalSpeedF(f1, 0.5f, 7.5f, 4.0f);
}

/* 02419E00 */
BOOL daPy_lk_c::procAtnActorWait() {
    setSpeedAndAngleAtnActor();
    if (!checkNextMode(0)) {
        if (checkAtnWaitAnime()) {
            setBlendAtnMoveAnime(-1.0f);
        } else {
            setBlendMoveAnime(-1.0f);
        }
    }
    return TRUE;
}

/* 02419E70 */
BOOL daPy_lk_c::procAtnActorMove() {
    setSpeedAndAngleAtnActor();
    if (!checkNextMode(0) && !checkIceSlipFall()) {
        setBlendAtnMoveAnime(-1.0f);
    }
    return TRUE;
}

/* 02419ED4 */
void daPy_lk_c::checkNextActionBowFly() {
    if (checkBowAnime()) {
        checkNextActionBowReady();
    } else if (itemTrigger() && lk_equipIsReadyItem(this)) {
        setBowReadyAnime();
        m355E = 0;
    }
}

/* 02419F54 */
void daPy_lk_c::checkNextActionBoomerangFly() {
    if ((u16)gabi::at<player04b::AnmHeap>(gabi::ea(m_anm_heap_upper))[2].mIdx == 0x35 /* checkBoomerangReadyAnime(): BOOMWAIT */) {
        checkNextActionBoomerangReady();
    } else if (itemTrigger() && lk_equipIsReadyItem(this)) {
        setActAnimeUpper(player04b::dRes_INDEX_LKANM_BCK_BOOMWAIT_e, player04b::UPPER_MOVE2_e, 0.8f, 0.0f, -1, 2.4f);
    }
}

/* 02412B7C */
BOOL daPy_lk_c::procShipRestart_init() {
    u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C); /* dComIfGp_getShipActor() */
    commonProcInit(player04b::daPyProc_SHIP_RESTART_e);
    gravity = 0.0f;
    gabi::store<u8>(ship + 0x636, 2); /* ship->setPaddleMove() */
    setShipRidePos(1);
    setSingleMoveAnime(0xD5 /* ANM_SEARESET */, 1.0f, 0.0f, -1, -1.0f);
    dComIfGp_setPlayerStatus0(0, 0x10000 /* daPyStts0_SHIP_RIDE_e */);
    return TRUE;
}

/* 02412C30 */
void daPy_lk_c::setSwimMoveAnime(int swimMoveAnm) {
    f32 rate = 0.6f + (0.5f * std::fabs((f32)mNormalSpeed)) / mMaxNormalSpeed /* mMaxNormalSpeed */; /* HD: HIO folded */
    f32 endFrame = mFrameCtrlUnder[0].getFrame() * (f32)mFrameCtrlUnder[0].getEnd();
    f32 timer = getSwimTimerRate();
    setSingleMoveAnime(swimMoveAnm, gabi::fmadds(timer, 1.0f, rate), 0.0f, -1, 5.3f);
    f32 frame = endFrame * (f32)mFrameCtrlUnder[0].getEnd();
    mFrameCtrlUnder[0].setFrame(frame);
    gabi::at<player04b::AnimationValue>(gabi::at<player04b::AnmRatio>(gabi::ea(mAnmRatioUnder))->mpAnmTransform)->mFrame = frame; /* mAnmRatioUnder[0].getAnmTransform()->setFrame() */
}

/* 02412D54 */
void daPy_lk_c::setSwimTail() {
    /* static JGeometry::TVec3<f32> tail_scale(1.0f, 1.0f, -1.0f) (HD: initialised on first use, unused) */
    if ((u32)l_tail_scale_guard == 0) {
        l_tail_scale.x = 1.0f;
        l_tail_scale.z = -1.0f;
        l_tail_scale.y = 1.0f;
        l_tail_scale_guard = 1;
    }
    auto* cb0 = gabi::at<daPy_swimTailEcallBack_c>(gabi::ea(this) + 0x66A8); /* mSwimTailEcallBack[0] */
    if (cb0->mpEmitter == 0) {
        /* dComIfGp_particle_setShipTail(dPa_name::ID_IT_JN_LK_SWIMK_L, &cb.getPos(), &current.angle, &scale, 0xFF, &cb) */
        dPa_control_set(dComIfGp_getParticle(), 5, 0x3A, &cb0->mPos, &current.angle, &scale, 0xFF,
                        gabi::at<dPa_levelEcallBack>(gabi::ea(cb0)), -1, nullptr, nullptr, nullptr);
        cb0->field_0x05 = 1; /* onRightFlg() */
    }
    auto* cb1 = gabi::at<daPy_swimTailEcallBack_c>(gabi::ea(this) + 0x66D0); /* mSwimTailEcallBack[1] */
    if (cb1->mpEmitter == 0) {
        dPa_control_set(dComIfGp_getParticle(), 5, 0x3A, &cb1->mPos, &current.angle, &scale, 0xFF,
                        gabi::at<dPa_levelEcallBack>(gabi::ea(cb1)), -1, nullptr, nullptr, nullptr);
    }
}

/* 02412E58 */
BOOL daPy_lk_c::procSwimMove_init(int param_1) {
    commonProcInit(player04b::daPyProc_SWIM_MOVE_e);
    if (!param_1) {
        mFrameCtrlUnder[0].setFrame(0.0f);
    }
    gravity = 0.0f;
    setSwimMoveAnime(0x83 /* ANM_SWIMING */);
    mDirection = 0; /* DIR_FORWARD */
    if (mNoResetFlg0 & 0x100 /* daPyFlg0_UNK100 */) {
        if (mEquipItem != 0x100 /* daPyItem_NONE_e */) {
            deleteEquipItem(1);
        }
        f32 y = mWaterY;
        speed.y = 0.0f;
        current.pos.y = y;
        setSwimTail();
    }
    dComIfGp_setPlayerStatus0(0, 0x100000 /* daPyStts0_SWIM_e */);
    mProcVar6 = 0;
    m35C4 = 0.0f; /* HD: HIO folded */
    return TRUE;
}

/* 02412F68 */
BOOL daPy_lk_c::procCrawlMove_init(s16 param_0, s16 param_1) {
    BOOL var_r29 = mCurProc != 0x11 /* daPyProc_CRAWL_AUTO_MOVE_e */;
    BOOL var_r27 = dComIfGp_checkPlayerStatus0(0, 0x2000 /* daPyStts0_SUBJECT_e */) != 0;
    commonProcInit(player04b::daPyProc_CRAWL_MOVE_e);
    if (var_r29 != 0) {
        f32 dVar4 = getCrawlMoveAnmSpeed();
        if (getDirectionFromShapeAngle() == 1 /* DIR_BACKWARD */) {
            dVar4 = -dVar4;
        }
        current.angle.y = shape_angle.y;
        setSingleMoveAnime(0x41 /* ANM_LIEFORWARD */, dVar4, 0.0f, -1, 5.0f);
    } else {
        setResetFlg0(resetFlg0() | 0x1000); /* daPyRFlg0_CRAWL_AUTO_MOVE */
    }
    m35A0 = -1.0f;
    mProcVar6 = var_r29 ^ 1;
    shape_angle.x = param_0;
    shape_angle.z = param_1;
    m35E4 = 1.0f;
    dComIfGp_setPlayerStatus0(0, 0x8000000 /* daPyStts0_CRAWL_e */);
    if (var_r27 != 0) {
        dComIfGp_setPlayerStatus0(0, 0x2000 /* daPyStts0_SUBJECT_e */);
    }
    return TRUE;
}

/* 024130B0 */
BOOL daPy_lk_c::procSmallJump_init(int param_1) {
    commonProcInit(player04b::daPyProc_SMALL_JUMP_e);
    setSingleMoveAnime(0x3C /* ANM_JMPST */, 0.2f, 0.0f, 6, 0.0f); /* HD: HIO folded */
    if (param_1 == 0) {
        f32 t = ((m3724.y - current.pos.y) + 45.0f) * gravity;
        mNormalSpeed = 5.0f;
        speed.y = std_sqrtf(-(t + t));
        mProcVar6 = 2;
    } else {
        mNormalSpeed = 10.0f;
        speed.y = 20.0f;
        mProcVar6 = 0;
    }
    if (checkHeavyStateOn()) {
        gravity = gravity * 0.44444445f;
    }
    LK_voiceStart(5);
    return TRUE;
}

/* 02415EF0 */
BOOL daPy_lk_c::checkSubjectEnd(BOOL i_playSound) {
    if (dComIfGp_event_runCheck() ||
        (mItemTrigger & 3 /* BTN_A | BTN_B */) ||
        (mItemButton & 0x20 /* BTN_L */) ||
        (dComIfGp_getCameraAttentionStatus(mCameraInfoIdx) & 0x2000)) {
        if (i_playSound) {
            mDoAud_seStart(player04b::JA_SE_SUBJ_VIEW_OUT);
        }
        setResetFlg0(resetFlg0() | daPyRFlg0_UNK10000000);
        return TRUE;
    }
    return FALSE;
}

/* 02416160 */
BOOL daPy_lk_c::procCrawlStart_init() {
    commonProcInit(player04b::daPyProc_CRAWL_START_e);
    m3700.copy(*cXyz_Zero);
    m34C2 = 1;
    setSingleMoveAnime(player04b::ANM_LIE, 1.0f, 0.0f, 9, 3.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    current.angle.y = shape_angle.y;
    m35A0 = 0.14285715f; /* 1.0f / (HIO 0x28 - 0x24) */
    m35E4 = 0.0f;
    dComIfGp_setPlayerStatus0(0, player04b::daPyStts0_CRAWL_e);
    return TRUE;
}

/* 02415FB0 */
int daPy_lk_c::getCrawlMoveVec(cXyz* start, cXyz* end, cXyz* moveVec) {
    dBgS_LinChk_Set(mLinkLinChk, start, end, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        u32 triPla = dBgS_GetTriPla_l(gabi::ea(static_cast<cBgS_PolyInfo*>(&player04b::backgroundChecks(this)->mLinkLinChk)));
        if (triPla == 0) { /* HD: NULL check */
            return FALSE;
        }
        int specialCode = dBgS_GetSpecialCode_l(gabi::ea(static_cast<cBgS_PolyInfo*>(&player04b::backgroundChecks(this)->mLinkLinChk)));
        f32 temp = gabi::at<cM3dGPla>(triPla)->mNormal.y;
        if (temp < 0.5f && !(temp < -0.8f)) { /* cBgW_CheckBWall(temp) */
            gabi::Local<cXyz> crossDelta;
            cXyz_mi(end, crossDelta, &player04b::backgroundChecks(this)->mLinkLinChk.mLin.mEnd);
            gabi::Local<cXyz> crossDeltaXZ; /* absXZ() */
            crossDeltaXZ->x = crossDelta->x;
            crossDeltaXZ->y = 0.0f;
            crossDeltaXZ->z = crossDelta->z;
            f32 negativeXZDistance = -std_sqrtf(PSVECSquareMag(crossDeltaXZ));
            moveVec->y = crossDelta->y;
            moveVec->x = negativeXZDistance * gabi::at<cM3dGPla>(triPla)->mNormal.x;
            moveVec->z = negativeXZDistance * gabi::at<cM3dGPla>(triPla)->mNormal.z;
            return TRUE;
        } else if (specialCode == 1 || (temp < 0.643832f && specialCode == 2)) {
            gabi::Local<cXyz> moveDelta;
            cXyz_mi(end, moveDelta, &player04b::backgroundChecks(this)->mLinkLinChk.mLin.mEnd);
            moveVec->copy(*moveDelta);
            return TRUE;
        }
    }
    return FALSE;
}

/* 02416230 */
void daPy_lk_c::setNormalSpeedF(f32 acceleration, f32 decelerationRate, f32 maxDeceleration, f32 minDeceleration) {
    f32 maxSpeed;
    if (dComIfGp_event_runCheck() ||
        checkHeavyStateOn() ||
        field_0x2b0 < 0.0f /* checkGrabWear() */) {
        maxSpeed = mMaxNormalSpeed * mStickDistance;
    } else {
        f32 st = mStickDistance;
        maxSpeed = (mMaxNormalSpeed * st) * st;
    }
    if (mDemo.mDemoMode == daPy_demo_c::DEMO_KEEP_e || getSlidePolygon() != 0) {
        return;
    }
    if (mAcch.m_flags & dBgS_Acch::WALL_HIT) {
        s16 wallAngleDelta = 0;
        for (int i = 0; i < 3; i++) {
            dBgS_AcchCir& cir = mAcchCir[i];
            if (cir.ChkWallHit()) {
                wallAngleDelta = (s16)((current.angle.y + 0x8000) - cir.GetWallAngleY());
                break;
            }
        }
        s32 a = wallAngleDelta;
        if (a < 0) {
            a = -a;
        }
        if (a < 0x4000) {
            maxSpeed *= gabi::fnmsubs(0.6f, cM_scos(wallAngleDelta), 1.0f); /* 1.0f - cM_scos(wallAngleDelta) * HIO */
        }
    }
    f32 deceleration;
    f32 targetSpeed;
    f32 normalSpeed = mNormalSpeed;
    if (maxSpeed < normalSpeed) {
        deceleration = normalSpeed - maxSpeed;
        if (deceleration > maxDeceleration) {
            deceleration = maxDeceleration;
        }
        if (deceleration < minDeceleration) {
            deceleration = minDeceleration;
        }
        acceleration = 0.0f;
        targetSpeed = maxSpeed;
    } else {
        deceleration = maxDeceleration;
        targetSpeed = 0.0f;
    }
    if (!(std::fabs(acceleration) < 3.8146973e-06f)) { /* !cM3d_IsZero(acceleration) */
        f32 s = normalSpeed + acceleration;
        if (s > maxSpeed) {
            mNormalSpeed = maxSpeed;
        } else {
            mNormalSpeed = s;
        }
    } else {
        cLib_addCalc_l(&mNormalSpeed, targetSpeed, decelerationRate, deceleration, minDeceleration);
    }
}

/* 02416C78 */
s16 daPy_lk_c::checkBodyAngleX(s16 angleX) {
    gabi::Local<cXyz> bodyPos;
    gabi::Local<cXyz> probePos;
    Mtx34* bodyMtx = lk_getAnmMtx(mpCLModel, player04b::CL_JNT_BODY_CHN_e);
    bodyPos->x = bodyMtx->m[0][3]; /* mDoMtx_multVecZero */
    bodyPos->y = bodyMtx->m[1][3];
    bodyPos->z = bodyMtx->m[2][3];
    /* static Vec top_vec = {0.0f, 70.0f, 0.0f} () */
    cXyz* top_vec = &player04b::top_vec();
    if (angleX >= 0) {
        top_vec->z = 25.0f;
    } else {
        top_vec->z = -25.0f;
    }
    mDoMtx_stack_c::transS(bodyPos->x, bodyPos->y, bodyPos->z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), angleX, shape_angle.y, 0);
    PSMTXMultVec(mDoMtx_stack_c::get(), top_vec, probePos);
    dBgS_LinChk_Set(mLinkLinChk, bodyPos, probePos, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        gabi::Local<cXyz> hitDelta;
        cXyz_mi(&player04b::backgroundChecks(this)->mLinkLinChk.mLin.mEnd, hitDelta, bodyPos);
        f32 s = cM_ssin(angleX);
        f32 sinAngle = s * std_sqrtf(PSVECSquareMag(hitDelta));
        sinAngle = sinAngle / std_sqrtf(5525.0f); /* SQUARE(70.0f) + SQUARE(25.0f) */
        if (sinAngle > 1.0f) {
            sinAngle = 1.0f;
        } else if (sinAngle < -1.0f) {
            sinAngle = -1.0f;
        }
        f32 cosAngle = std_sqrtf(gabi::fnmsubs(sinAngle, sinAngle, 1.0f));
        angleX = cM_atan2s(sinAngle, cosAngle);
    }
    return angleX;
}

/* 02416E90 */
BOOL daPy_lk_c::setBodyAngleToCamera() {
    if (dComIfGp_getCameraAttentionStatus(mCameraInfoIdx) & player04b::dCamAttnStts_00000010_e) {
        gabi::Local<be<s16>> angleX; /* x */
        gabi::Local<be<s16>> angleY; /* y */
        u32 body = player04bGetCameraBody();
        BOOL hasSubjectAngle = player04bCalculateSubjectAngle(body, angleX.get(), angleY.get());
        if (hasSubjectAngle != 0) {
            shape_angle.y = *angleY; /* HD: current.angle.y is not set */
            u32 previousAngleX = player04bCheckBodyAngleRegister(this, (s32)mBodyAngle.x);
            mBodyAngle.x = (s16)previousAngleX;
            u32 subjectAngleX = player04bCheckBodyAngleRegister(this, (s32)*angleX);
            if (subjectAngleX == (u32)(s32)*angleX) {
                mBodyAngle.x = (s16)subjectAngleX;
            } else {
                mBodyAngle.x = (s16)previousAngleX;
            }
        } else {
            s32 idx = mCameraInfoIdx;
            camera_class* cam = dComIfGp_getCamera(idx); /* dComIfGp_getCamera(mCameraInfoIdx) */
            s16 y = cam->mAngle.y;
            shape_angle.y = y;
            current.angle.y = y;
            mBodyAngle.x = cam->mAngle.x;
        }
        return hasSubjectAngle;
    }
    return FALSE;
}

/* 0241701C */
BOOL daPy_lk_c::procSubjectivity() {
    dComIfGp_setAStatus(7); /* dComIfGp_setAStatus(dActStts_RETURN_e) */
    bool end;
    if (checkSubjectEnd(FALSE)) {
        end = true;
    } else if (mProcVar6 != 0 && !(mItemButton & 0x40) /* spActionButton(): R released while crouching */) {
        end = true;
    } else {
        // HD: a slide polygon also ends the subjective mode
        end = changeSlideProc() != 0;
    }
    if (end) {
        mDoAud_seStart(player04b::JA_SE_SUBJ_VIEW_OUT);
        if (mProcVar6 != 0) {
            if (mEquipItem == player04b::daPyItem_NONE_e) {
                return procCrouch_init();
            }
            return procCrouchDefense_init();
        }
        changeWaitProc();
        return TRUE;
    }
    if (mProcVar6 == 0) {
        // HD: the subjective view moves Link (attention move without the shape angle)
        setSpeedAndAngleAtnNoShapeHD();
        setBodyAngleToCamera();
        setBlendSubjectMoveAnimeHD();
        return TRUE;
    }
    // HD: crouching in the subjective view with the stick held starts crawling
    if ((mItemButton & 0x40) && gabi::at<player04b::OldFrame>(m_old_fdata)->mOldFrameRate < 0.01f && mStickDistance > 0.05f &&
        !(mWaterY > current.pos.y + 15.0f) && mEquipItem == player04b::daPyItem_NONE_e) {
        gabi::Local<cXyz> crawlFront;
        gabi::Local<cXyz> crawlTop;
        gabi::Local<cXyz> crawlMove;
        f32 y = current.pos.y;
        mDoMtx_stack_c::transS(current.pos.x, y, current.pos.z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), m34E2, shape_angle.y, 0);
        PSMTXMultVec(mDoMtx_stack_c::get(), &player04b::l_crawl_start_front_offset(), crawlFront);
        PSMTXMultVec(mDoMtx_stack_c::get(), &player04b::l_crawl_top_offset(), crawlTop);
        if (getCrawlMoveVec(crawlTop, crawlFront, crawlMove)) {
            gabi::Local<cXyz> groundPos;
            cXyz_mi(&current.pos, groundPos, crawlMove);
            f32 gy = groundPos->y + 5.0f;
            player04b::backgroundChecks(this)->mGndChk.m_pos.z = groundPos->z;
            player04b::backgroundChecks(this)->mGndChk.m_pos.x = groundPos->x;
            groundPos->y = gy;
            player04b::backgroundChecks(this)->mGndChk.m_pos.y = gy;
            groundPos->y = cBgS_GroundCross(dComIfG_Bgsp(), mGndChk);
            gabi::Local<cXyz> groundDelta;
            cXyz_mi(&current.pos, groundDelta, groundPos);
            gabi::Local<cXyz> groundDeltaXZ; /* absXZ() */
            groundDeltaXZ->x = groundDelta->x;
            groundDeltaXZ->y = 0.0f;
            groundDeltaXZ->z = groundDelta->z;
            f32 xz = std_sqrtf(PSVECSquareMag(groundDeltaXZ));
            s16 a = cM_atan2s(-groundDelta->y, xz);
            if (cLib_distanceAngleS(a, m34E2) > 0x100) {
                return TRUE;
            }
        }
        return procCrawlStart_init();
    }
    setAngleAtnHD();
    setBodyAngleToCamera();
    setBlendSubjectMoveAnimeHD();
    return TRUE;
}

/* 024172D4 */
BOOL daPy_lk_c::procCall() {
    if (gabi::ea(dComIfGp_getCb1Player()) != 0) {
        fopAc_ac_c* cb1 = dComIfGp_getCb1Player();
        s16 targetAngle = cLib_targetAngleY(&current.pos, &cb1->eyePos);
        cLib_addCalcAngleS(&shape_angle.y, targetAngle, 2, 0x2000, 0x800);
        current.angle.y = shape_angle.y;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (gabi::ea(dComIfGp_getCb1Player()) != 0) {
            u32 partner = gabi::ea(dComIfGp_getCb1Player()); /* partner->onNpcCallCommand() */
            gabi::at<daPy_py_c>(partner)->onNoResetFlg1(daPyFlg1_NPC_CALL_COMMAND);
        }
        checkNextMode(0);
    }
    return TRUE;
}

/* 024173F4 */
BOOL daPy_lk_c::procIceSlipAlmostFall_init() {
    commonProcInit(player04b::daPyProc_ICE_SLIP_ALMOST_FALL_e);
    current.angle.y = cM_atan2s(m36AC.x, m36AC.z);
    int direction = getDirectionFromAngle((s32)(s16)(current.angle.y - shape_angle.y));
    int anm;
    if (direction == player04b::DIR_BACKWARD) {
        anm = player04b::ANM_DAMF;
    } else if (direction == player04b::DIR_LEFT) {
        anm = player04b::ANM_DAMR;
    } else if (direction == player04b::DIR_RIGHT) {
        anm = player04b::ANM_DAML;
    } else {
        anm = player04b::ANM_DAMB;
    }
    setSingleMoveAnime(anm, 0.6f, 1.0f, 9, 1.0f); /* HD: HIO folded */
    setTextureAnime(6, 0);
    mNormalSpeed = 0.0f;
    return TRUE;
}

/* 024176A8 */
BOOL daPy_lk_c::procFreeWait_init() {
    commonProcInit(player04b::daPyProc_FREE_WAIT_e);
    mNormalSpeed = 0.0f;
    f32 animationChoice = cM_rnd();
    int anm;
    if (animationChoice < 0.3333f) {
        anm = player04b::ANM_FREEA;
        mProcVar6 = 0;
    } else if (animationChoice < 0.6666f) {
        anm = player04b::ANM_FREEB;
        mProcVar6 = 1;
    } else {
        anm = player04b::ANM_FREED;
        mProcVar6 = 0;
    }
    setSingleMoveAnime(anm, 1.0f, 0.0f, -1, 5.0f);
    mDirection = player04b::DIR_NONE;
    current.angle.y = shape_angle.y;
    return TRUE;
}

/* 02417BFC */
BOOL daPy_lk_c::procFreeWait() {
    if (lk_checkAttentionLock(this)) {
        setSpeedAndAngleAtn();
    } else {
        setSpeedAndAngleNormal(0xBB8); /* HD: m_HIO->mMove.m.field_0x0 folded */
    }
    if (mProcVar6 != 0) {
        if (mFrameCtrlUnder[0].checkPass(168.0f)) {
            LK_voiceStart(48);
        } else if (mFrameCtrlUnder[0].checkPass(105.0f)) {
            LK_voiceStart(47);
        }
    }
    if (!checkNextMode(0) && dComIfGp_event_runCheck()) {
        mFrameCtrlUnder[0].setRate(0.0f);
        procWait_init();
    }
    return TRUE;
}

/* 024177A8 */
BOOL daPy_lk_c::procWait() {
    gabi::Local<cXyz> xz0; /* m36A0.abs2XZ() */
    xz0->y = 0.0f;
    xz0->x = m36A0.x;
    xz0->z = m36A0.z;
    if (!(PSVECSquareMag(xz0) > 1.0000001e-06f)) {
        gabi::Local<cXyz> xz1; /* m36AC.abs2XZ() */
        xz1->x = m36AC.x;
        xz1->y = 0.0f;
        xz1->z = m36AC.z;
        if (!(PSVECSquareMag(xz1) < 25.0f)) {
            return procIceSlipAlmostFall_init();
        }
    }
    if (!dComIfGp_event_runCheck() &&
        mDemo.mDemoType == 0 /* !checkPlayerDemoMode() */ &&
        (mItemTrigger & 0x20) /* spLTrigger() */ &&
        (mAcch.m_flags & dBgS_Acch::WALL_HIT)) {
        for (int i = 0; i < 3; i++) {
            dBgS_AcchCir& cir = mAcchCir[i];
            if (cir.ChkWallHit()) {
                s16 wallFacingAngle = (s16)(cir.GetWallAngleY() + 0x8000);
                if (cLib_distanceAngleS(shape_angle.y, wallFacingAngle) <= 0x2000) {
                    shape_angle.y = wallFacingAngle;
                    current.angle.y = wallFacingAngle;
                    m34E6 = wallFacingAngle;
                }
                break;
            }
        }
    }
    s16 previousShapeAngle = shape_angle.y;
    if (lk_checkAttentionLock(this)) {
        setSpeedAndAngleAtn();
    } else {
        setSpeedAndAngleNormal(0xBB8); /* HD: m_HIO->mMove.m.field_0x0 folded */
    }
    m35A0 = 0.0f;
    if (checkNextMode(0)) {
        return TRUE;
    }
    if (m34C3 == 0) {
        if (mFrameCtrlUnder[0].getRate() < 0.01f || !checkRestHPAnime()) {
            setBlendMoveAnime(2.4f); /* HD: m_HIO->mBasic.m.field_0xC folded */
            mModeFlg = (mModeFlg & ~(u32)player04b::ModeFlg_00000400) | player04b::ModeFlg_00000100;
        }
    } else if (checkRestHPAnime() && (u16)gabi::at<player04b::AnmHeap>(gabi::ea(m_anm_heap_under))[0].mIdx != player04b::dRes_INDEX_LKANM_BCK_WAITB_e) {
        BOOL hasRestFace = (u16)gabi::at<player04b::AnmHeap>(gabi::ea(m_tex_anm_heap))->mIdx == 0x234; /* m_tex_anm_heap.mIdx == mTexAnmIndexTable[daPyFace_TMABAF].mBtpIdx */
        u16 textureVariant = m3530;
        setSingleMoveAnime(player04b::ANM_WAITATOB, 0.6f, 0.0f, 0xC, 6.0f);
        if (hasRestFace == 0) {
            mModeFlg = (mModeFlg | player04b::ModeFlg_00000400) & ~(u32)player04b::ModeFlg_00000100;
        } else {
            setTextureAnime(0xE, (u32)textureVariant);
        }
    } else {
        m35A0 = 0.005f * (f32)(s16)(shape_angle.y - previousShapeAngle);
        setBlendMoveAnime(-1.0f);
    }
    if (mDemo.mDemoMode == daPy_demo_c::DEMO_KM_WAIT_e) {
        if (mProcVar0 != 0) {
            mProcVar0 = mProcVar0 - 1;
        } else if (cM_rnd() < 0.05f) {
            LK_voiceStart(38);
            mProcVar0 = 20;
        }
    }
    if (!dComIfGp_event_runCheck() &&
        (u16)gabi::at<player04b::AnmHeap>(gabi::ea(m_anm_heap_under))[0].mIdx == player04b::dRes_INDEX_LKANM_BCK_WAITS_e &&
        (u16)gabi::at<player04b::AnmHeap>(gabi::ea(m_anm_heap_upper))[2].mIdx == 0xFFFF /* checkNoUpperAnime() */ &&
        (u8)player04b::eyeMoveFlag() == 0 /* daPy_matAnm_c::getEyeMoveFlg() */ &&
        m3564.y == 0 && m3564.z == 0 && m3564.x == 0) {
        mProcVar1 = mProcVar1 - 1;
        if (mProcVar1 == 0) {
            procFreeWait_init();
        }
    } else {
        mProcVar1 = (s16)gabi::ftoi(cM_rndF(150.0f) + 300.0f);
    }
    return TRUE;
}

/* 02418178 */
BOOL daPy_lk_c::procLadderUpEnd_init(int leftFoot) {
    commonProcInit(player04b::daPyProc_LADDER_UP_END_e);
    int anm = leftFoot != 0 ? player04b::ANM_LADDERUPEDL : player04b::ANM_LADDERUPEDR;
    setSingleMoveAnime(anm, 0.9f, 0.0f, -1, 3.0f); /* HD: HIO folded */
    speedF = 0.0f;
    gravity = 0.0f;
    m34C2 = 7;
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    dComIfGp_setPlayerStatus0(0, player04b::daPyStts0_UNK2000000_e);
    return TRUE;
}

/* 024184C8 */
void daPy_lk_c::procClimbUpStart_init_sub() {
    setSingleMoveAnime(player04b::ANM_LADDERUPST, 1.0f, 0.0f, -1, 1.0f); /* HD: HIO folded */
    mProcVar6 = 1;
    m34C2 = 4;
    dComIfGp_setPlayerStatus1(0, player04b::daPyStts1_UNK10000_e);
}

/* 024186A4 */
void daPy_lk_c::procLadderUpStart_init_sub() {
    setSingleMoveAnime(player04b::ANM_LADDERUPST, 1.0f, 0.0f, -1, 1.0f); /* HD: HIO folded */
    mProcVar6 = 1;
    m34C2 = 4;
    dComIfGp_setPlayerStatus0(0, player04b::daPyStts0_UNK2000000_e);
}

/* 024188F0 */
void daPy_lk_c::procLadderDownStart_init_sub() {
    setSingleMoveAnime(player04b::ANM_LADDERDWST, 1.3f, 0.0f, -1, 3.0f); /* HD: HIO folded */
    mProcVar6 = 1;
    m34C2 = 4;
    dComIfGp_setPlayerStatus0(0, player04b::daPyStts0_UNK2000000_e);
    setOldRootQuaternion(0, -0x8000, 0);
    shape_angle.y = (s16)(shape_angle.y - 0x8000);
    current.angle.y = shape_angle.y;
}

/* 02418D58 */
BOOL daPy_lk_c::procVerticalJump_init() {
    commonProcInit(player04b::daPyProc_VERTICAL_JUMP_e);
    setSingleMoveAnime(player04b::ANM_VJMP, 1.0f, 0.0f, 0xD, 1.0f); /* HD: HIO folded */
    mProcVar0 = 0;
    mProcVar2 = (s16)(m352C + 0x8000);
    mProcVar6 = mFrontWallType == 9 ? 1 : 0;
    mNormalSpeed = 0.0f;
    speed.y = 0.0f;
    return TRUE;
}

/* 02418008 */
BOOL daPy_lk_c::procClimbMoveUpDown_init(int fromBelow) {
    f32 animationSpeed = getLadderMoveAnmSpeed();
    commonProcInit(player04b::daPyProc_CLIMB_MOVE_UP_DOWN_e);
    current.angle.y = shape_angle.y;
    if (mDirection == player04b::DIR_BACKWARD) {
        animationSpeed = -animationSpeed;
    }
    int anm;
    if (fromBelow != 0) {
        mProcVar6 = 0;
        anm = mDirection == player04b::DIR_FORWARD ? player04b::ANM_LADDERLTOR : player04b::ANM_LADDERRTOL;
    } else {
        mProcVar6 = 1;
        anm = mDirection == player04b::DIR_FORWARD ? player04b::ANM_LADDERRTOL : player04b::ANM_LADDERLTOR;
    }
    setSingleMoveAnime(anm, animationSpeed, 0.0f, -1, 0.0f); /* HD: HIO folded */
    if (mDirection == 0) {
        setTextureAnime(10, 0);
    } else {
        setTextureAnime(11, 0);
    }
    gravity = 0.0f;
    speed.y = 0.0f;
    mProcVar0 = 1;
    speedF = 0.0f;
    mNormalSpeed = 0.0f;
    m34C2 = 7;
    dComIfGp_setPlayerStatus1(0, player04b::daPyStts1_UNK10000_e);
    return TRUE;
}

/* 02418530 */
BOOL daPy_lk_c::procClimbUpStart_init() {
    commonProcInit(player04b::daPyProc_CLIMB_UP_START_e);
    speed.y = 0.0f;
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    speedF = 0.0f;
    if (mEquipItem == player04b::daPyItem_NONE_e) {
        if (checkEquipAnime()) {
            resetActAnimeUpper(player04b::UPPER_MOVE2_e, -1.0f);
        }
        procClimbUpStart_init_sub();
    } else {
        setBlendMoveAnime(2.4f); /* HD: m_HIO->mBasic.m.field_0xC folded */
        setAnimeUnequip();
        mProcVar6 = 0;
        m3598 = 0.0f;
    }
    u16 a = (u16)m352C;
    f32 wallSin = cM_ssin(a);
    f32 wallCos = cM_scos(a);
    current.pos.x = gabi::fmadds(25.0f, wallSin, m3724.x);
    current.pos.z = gabi::fmadds(25.0f, wallCos, m3724.z);
    shape_angle.y = (s16)(a + 0x8000);
    current.angle.y = shape_angle.y;
    mProcVar2 = 0;
    return TRUE;
}

/* 02418230 */
BOOL daPy_lk_c::procLadderMove_init(int fromMidair, int direction, cXyz* ladderPos) {
    int wasMidair = (mModeFlg & player04b::ModeFlg_MIDAIR) ? 1 : 0;
    f32 animationSpeed = getLadderMoveAnmSpeed();
    commonProcInit(player04b::daPyProc_LADDER_MOVE_e);
    f32 y = ladderPos->y;
    if (wasMidair == 0) {
        if (direction == player04b::DIR_FORWARD) {
            m370C.y = y + 37.5f;
        } else {
            m370C.y = y - 37.5f;
            animationSpeed = -animationSpeed;
        }
    } else {
        m370C.y = y;
    }
    m370C.x = ladderPos->x;
    m370C.z = ladderPos->z;
    int anm;
    if (fromMidair != 0) {
        if (wasMidair != 0) {
            mProcVar6 = 1;
        } else {
            mProcVar6 = 0;
            u16 a = (u16)shape_angle.y;
            f32 c = cM_scos(a);
            m370C.x = m370C.x - (c + c);
            m370C.z = gabi::fnmsubs(-2.0f, cM_ssin(a), m370C.z);
        }
        anm = animationSpeed < 0.0f ? player04b::ANM_LADDERRTOL : player04b::ANM_LADDERLTOR;
    } else {
        mProcVar6 = 1;
        anm = animationSpeed < 0.0f ? player04b::ANM_LADDERLTOR : player04b::ANM_LADDERRTOL;
        u16 a = (u16)shape_angle.y;
        f32 c = cM_scos(a);
        m370C.x = m370C.x + (c + c);
        m370C.z = gabi::fmadds(-2.0f, cM_ssin(a), m370C.z);
    }
    setSingleMoveAnime(anm, animationSpeed, 0.0f, -1, 0.0f); /* HD: HIO folded */
    mDirection = direction;
    if (direction == 0) {
        setTextureAnime(10, 0);
    } else {
        setTextureAnime(11, 0);
    }
    gravity = 0.0f;
    speed.y = 0.0f;
    mProcVar0 = 1;
    speedF = 0.0f;
    mNormalSpeed = 0.0f;
    m34C2 = 7;
    dComIfGp_setPlayerStatus0(0, player04b::daPyStts0_UNK2000000_e);
    return TRUE;
}

/* 0241870C */
BOOL daPy_lk_c::procLadderUpStart_init() {
    commonProcInit(player04b::daPyProc_LADDER_UP_START_e);
    speed.y = 0.0f;
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    speedF = 0.0f;
    if (mEquipItem == player04b::daPyItem_NONE_e) {
        if (checkEquipAnime()) {
            resetActAnimeUpper(player04b::UPPER_MOVE2_e, -1.0f);
        }
        procLadderUpStart_init_sub();
    } else {
        setBlendMoveAnime(2.4f); /* HD: m_HIO->mBasic.m.field_0xC folded */
        setAnimeUnequip();
        m3598 = 0.0f;
        mProcVar6 = 0;
    }
    u16 a = (u16)m352C;
    f32 wallSin = cM_ssin(a);
    f32 wallCos = cM_scos(a);
    f32 x = m3724.x;
    f32 y = m3724.y;
    f32 z = m3724.z;
    current.pos.x = gabi::fmadds(25.0f, wallSin, x);
    current.pos.y = y;
    current.pos.z = gabi::fmadds(25.0f, wallCos, z);
    shape_angle.y = (s16)(a + 0x8000);
    current.angle.y = shape_angle.y;
    m370C.x = gabi::fmadds(20.5f, wallSin, x) - wallCos;
    m370C.y = y + 37.5f;
    m370C.z = gabi::fmadds(20.5f, wallCos, z) + wallSin;
    mProcVar2 = 0;
    return TRUE;
}

/* 02418980 */
BOOL daPy_lk_c::procLadderDownStart_init() {
    commonProcInit(player04b::daPyProc_LADDER_DOWN_START_e);
    s16 a0 = m352C;
    gravity = 0.0f;
    speed.y = 0.0f;
    speedF = 0.0f;
    shape_angle.y = (s16)(a0 + 0x8000);
    current.angle.y = a0;
    mNormalSpeed = 0.0f;
    if (mEquipItem == player04b::daPyItem_NONE_e) {
        if (checkEquipAnime()) {
            resetActAnimeUpper(player04b::UPPER_MOVE2_e, -1.0f);
        }
        procLadderDownStart_init_sub();
    } else {
        setBlendMoveAnime(2.4f); /* HD: m_HIO->mBasic.m.field_0xC folded */
        setAnimeUnequip();
        mProcVar6 = 0;
        m3598 = 0.0f;
    }
    u16 a = (u16)m352C;
    f32 wallSin = cM_ssin(a);
    f32 wallCos = cM_scos(a);
    f32 x = m3724.x;
    f32 y = m3724.y;
    f32 z = m3724.z;
    current.pos.y = y;
    current.pos.x = gabi::fmadds(25.0f, wallSin, x);
    current.pos.z = gabi::fmadds(25.0f, wallCos, z);
    m370C.x = gabi::fnmsubs(30.5f, wallSin, x) + wallCos;
    m370C.y = y - 112.5f;
    m370C.z = gabi::fnmsubs(30.5f, wallCos, z) - wallSin;
    return TRUE;
}

/* 02418B48 */
BOOL daPy_lk_c::procHangWallCatch_init() {
    gabi::Local<cXyz> wallPosXZ;
    gabi::Local<cXyz> playerPosXZ;
    wallPosXZ->set(m3724.x, 0.0f, m3724.z);
    playerPosXZ->set(current.pos.x, 0.0f, current.pos.z);
    f32 dist = std_sqrtf(player04bSquareDistance(wallPosXZ.get(), playerPosXZ.get()));
    if (dist > mAcchCir[0].m_wall_r + 20.0f) {
        return FALSE;
    }
    u16 a = (u16)m352C;
    f32 x = gabi::fnmsubs(1.5f, cM_ssin(a), m3724.x);
    f32 y = m3724.y + 10.0f;
    f32 z = gabi::fnmsubs(1.5f, cM_scos(a), m3724.z);
    player04b::backgroundChecks(this)->mGndChk.m_pos.y = y; /* mGndChk.SetPos(&local_1c) */
    player04b::backgroundChecks(this)->mGndChk.m_pos.x = x;
    player04b::backgroundChecks(this)->mGndChk.m_pos.z = z;
    f64 gy = player04bGroundCrossWide(dComIfG_Bgsp(), gabi::at<dBgS_GndChk>(gabi::ea(mGndChk)));
    if (std::fabs((f32)(gy - (f64)(f32)m3724.y)) > 30.1f) {
        return FALSE;
    }
    commonProcInit(player04b::daPyProc_HANG_WALL_CATCH_e);
    setSingleMoveAnime(player04b::ANM_VJMPCHB, 0.8f, 0.0f, 5, 2.5f); /* HD: HIO folded */
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    current.pos.y = (f32)gy;
    shape_angle.y = (s16)(m352C + 0x8000);
    current.angle.y = shape_angle.y;
    current.pos.z = z;
    current.pos.x = x;
    dComIfGp_setPlayerStatus0(0, player04b::daPyStts0_HANG_e);
    return TRUE;
}

/* 02417D30 */
BOOL daPy_lk_c::procHangStart_init() {
    if (mCurProc != player04b::daPyProc_ROPE_UP_HANG_e && mCurProc != player04b::daPyProc_ROPE_UP_e) {
        gabi::Local<cXyz> wallPosXZ;
        gabi::Local<cXyz> playerPosXZ;
        wallPosXZ->set(m3724.x, 0.0f, m3724.z);
        playerPosXZ->set(current.pos.x, 0.0f, current.pos.z);
        f32 dist = std_sqrtf(player04bSquareDistance(wallPosXZ.get(), playerPosXZ.get()));
        if (dist > mAcchCir[0].m_wall_r + 20.0f) {
            return FALSE;
        }
    }
    s32 fromRope = 0;
    f64 y;
    f32 x, z;
    if (mCurProc != player04b::daPyProc_ROPE_UP_HANG_e && mCurProc != player04b::daPyProc_ROPE_UP_e) {
        u16 a = (u16)m352C;
        f32 y0 = m3724.y + 10.0f;
        x = gabi::fnmsubs(1.5f, cM_ssin(a), m3724.x);
        z = gabi::fnmsubs(1.5f, cM_scos(a), m3724.z);
        player04b::backgroundChecks(this)->mGndChk.m_pos.y = y0; /* mGndChk.SetPos(&local_1c) */
        player04b::backgroundChecks(this)->mGndChk.m_pos.x = x;
        player04b::backgroundChecks(this)->mGndChk.m_pos.z = z;
        y = player04bGroundCrossWide(dComIfG_Bgsp(), gabi::at<dBgS_GndChk>(gabi::ea(mGndChk)));
        if (std::fabs((f32)(y - (f64)(f32)m3724.y)) > 30.1f) {
            return FALSE;
        }
    } else {
        u16 a = (u16)m352C;
        x = gabi::fnmsubs(2.25f, cM_ssin(a), m3724.x);
        y = m3724.y + 10.0f;
        z = gabi::fnmsubs(2.25f, cM_scos(a), m3724.z);
        /* !strcmp(dComIfGp_getStartStageName(), "GanonK") */
        u32 p = dComIfGp_ea() + 0x5134;
        u32 q = gabi::guest_addr("GanonK");
        for (;;) {
            u8 c0 = gabi::load<u8>(p++);
            u8 c1 = gabi::load<u8>(q++);
            if (c0 != c1) {
                break;
            }
            if (c0 == 0) {
                fromRope = 1;
                break;
            }
        }
    }
    commonProcInit(player04b::daPyProc_HANG_START_e);
    mProcVar6 = fromRope;
    setSingleMoveAnime(player04b::ANM_VJMPCHA, 0.8f, 1.0f, 7, 2.0f); /* HD: HIO folded */
    current.pos.x = x;
    current.pos.y = (f32)y;
    mNormalSpeed = 0.0f;
    current.angle.y = (s16)(m352C + 0x8000);
    speed.y = 0.0f;
    shape_angle.y = current.angle.y;
    current.pos.z = z;
    dComIfGp_setPlayerStatus0(0, player04b::daPyStts0_HANG_e);
    mHangGroundH = mAcch.m_ground_h;
    return TRUE;
}

/* 02419514 */
BOOL daPy_lk_c::procIceSlipFall_init() {
    commonProcInit(player04b::daPyProc_ICE_SLIP_FALL_e);
    s16 a = cM_atan2s(-m36A0.x, -m36A0.z);
    int direction = getDirectionFromAngle((s32)(s16)(a - shape_angle.y));
    if (direction == player04b::DIR_FORWARD) {
        mProcVar6 = player04b::ANM_DAMFB;
        mProcVar2 = 0x3FFF;
        mProcVar3 = 1;
    } else if (direction == player04b::DIR_RIGHT) {
        mProcVar6 = player04b::ANM_DAMFL;
        mProcVar2 = 0x3FFF;
        mProcVar3 = 0;
    } else if (direction == player04b::DIR_LEFT) {
        mProcVar6 = player04b::ANM_DAMFR;
        mProcVar2 = -0x3FFF;
        mProcVar3 = 0;
    } else {
        mProcVar6 = player04b::ANM_DAMFF;
        mProcVar2 = -0x3FFF;
        mProcVar3 = 1;
    }
    setSingleMoveAnime((s32)mProcVar6, 1.0f, 0.0f, -1, 3.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    speed.y = 12.0f;
    mBodyAngle.z = 0; /* mBodyAngle */
    mBodyAngle.y = 0;
    mBodyAngle.x = 0;
    LK_voiceStart(36);
    return TRUE;
}

/* 024197A0 */
BOOL daPy_lk_c::checkIceSlipFall() {
    s16 slipAngle = cM_atan2s(m36A0.x, m36A0.z);
    f32 minSpeed;
    f32 minSlipSpeedSquared;
    if (m34C3 == 1) {
        minSpeed = 15.0f;
        minSlipSpeedSquared = 169.0f; /* SQUARE(13.0f) */
    } else {
        minSpeed = 10.0f;
        minSlipSpeedSquared = 49.0f; /* SQUARE(7.0f) */
    }
    if (cLib_distanceAngleS(slipAngle, current.angle.y) > 0x7000 && !(mNormalSpeed < minSpeed)) {
        gabi::Local<cXyz> xz; /* m36A0.abs2XZ() */
        xz->x = m36A0.x;
        xz->y = 0.0f;
        xz->z = m36A0.z;
        if (!(PSVECSquareMag(xz) < minSlipSpeedSquared)) {
            mProcVar0 = mProcVar0 - 1;
            if (mProcVar0 == 0) {
                return procIceSlipFall_init();
            }
            return FALSE;
        }
    }
    mProcVar0 = 20;
    return FALSE;
}

/* 024198D4 */
BOOL daPy_lk_c::procMove() {
    setSpeedAndAngleNormal(0xBB8); /* HD: m_HIO->mMove.m.field_0x0 folded */
    if (!checkNextMode(0) && !changeFrontWallTypeProc() &&
        !checkIceSlipFall()) {
        if (mDemo.mDemoMode == daPy_demo_c::DEMO_N_WALK_e) {
            f32 lim = mMaxNormalSpeed * 0.5f;
            if (mNormalSpeed > lim) {
                mNormalSpeed = lim;
            }
            // HD: walking at the height 1750..1755 is faster in room 0 of MajyuE and in room 1 of the sea
            f32 y = current.pos.y;
            if (1750.0f < y && y < 1755.0f) {
                if (lk_isStartStage(gabi::guest_addr("MajyuE")) && current.roomNo == 0) {
                    mNormalSpeed = mNormalSpeed * 1.5f;
                }
                if (lk_isStartStage(gabi::guest_addr("sea")) && current.roomNo == 1) {
                    mNormalSpeed = mNormalSpeed * 1.5f;
                }
            }
        }
        setBlendMoveAnime(-1.0f);
    }
    return TRUE;
}

/* 0241650C */
void daPy_lk_c::setSpeedAndAngleNormal(s16 maxAngleStep) {
    f32 acceleration = 0.0f;
    if (mStickDistance > 0.05f) {
        f32 st = mStickDistance;
        f32 stickRateSquared = st * st;
        BOOL heavy = checkHeavyStateOn();
        f32 wear = field_0x2b0;
        if (heavy) {
            stickRateSquared *= 4.0f; /* 1.0f / SQUARE(m_HIO->mMove.m.field_0x80) */
        }
        if (wear < 0.0f /* checkGrabWear() */) {
            stickRateSquared *= 1.5624999f; /* 1.0f / SQUARE(m_HIO->mMove.m.field_0x74) */
        }
        bool turn = false;
        if (!lk_checkAttentionLock(this) && cLib_distanceAngleS(m34E8, current.angle.y) > 0x7800 &&
            mCurProc != player04b::daPyProc_MOVE_TURN_e) {
            turn = true;
        }
        bool cosPart = true;
        if (turn) {
            if (mModeFlg & player04b::ModeFlg_00000001) {
                return;
            }
            if (mCurProc == player04b::daPyProc_MOVE_e) {
                if (speedF / mMaxNormalSpeed > 0.6f) { /* m_HIO->mSlip.m.field_0x4 */
                    if (getDirectionFromAngle((s32)(s16)(m34EA - m34DC)) == player04b::DIR_BACKWARD) {
                        return;
                    }
                    cosPart = false; // Keep zero acceleration while reversing at high speed.
                } else {
                    cLib_addCalcAngleS(&current.angle.y, m34E8, 5, maxAngleStep, 100); /* HD: HIO folded */
                    return;
                }
            } else {
                cLib_addCalcAngleS(&current.angle.y, m34E8, 5, maxAngleStep, 100);
            }
        } else {
            s16 angleStep;
            s16 minAngleStep;
            if (dComIfGp_event_runCheck()) {
                angleStep = maxAngleStep;
                minAngleStep = 100;
            } else {
                angleStep = (s16)gabi::ftoi((f32)maxAngleStep * stickRateSquared);
                if (angleStep < 10) {
                    angleStep = 10;
                }
                minAngleStep = (s16)gabi::ftoi(100.0f * stickRateSquared);
                if (!(minAngleStep > 0)) {
                    minAngleStep = 1;
                }
            }
            cLib_addCalcAngleS(&current.angle.y, m34E8, 5, angleStep, minAngleStep);
        }
        if (cosPart) {
            f32 c = cM_scos((u16)(m34E8 - current.angle.y));
            f32 half = mMaxNormalSpeed * 0.5f;
            if (mNormalSpeed > half) {
                if (c < 0.7f) {
                    c = 0.7f;
                }
            } else {
                c = c >= 0.0f ? c : 0.0f; /* fsel */
            }
            if (dComIfGp_event_runCheck()) {
                acceleration = (3.5f * mStickDistance) * c; /* HD: m_HIO->mMove.m.field_0x14 folded */
            } else {
                f32 r = mNormalSpeed / mMaxNormalSpeed;
                f32 accelerationThreshold = gabi::fnmsubs(std::fabs(r), 0.5f, 0.5f); /* 0.5f - 0.5f * abs(..) */
                BOOL heavy2 = checkHeavyStateOn();
                f32 wear2 = field_0x2b0;
                f32 st2 = mStickDistance;
                if (heavy2) {
                    accelerationThreshold *= 0.5f;
                }
                if (wear2 < 0.0f) {
                    accelerationThreshold *= 0.8f;
                }
                if (st2 > accelerationThreshold) {
                    acceleration = (3.5f * stickRateSquared) * c;
                }
            }
        }
    }
    if (!lk_checkAttentionLock(this) && mCurProc != player04b::daPyProc_MOVE_TURN_e && mStickDistance > 0.05f) {
        s16 angleStep = shape_angle.y;
        cLib_addCalcAngleS(&shape_angle.y, m34E8, 5, (s16)(maxAngleStep << 1), 200);
        s16 cur = current.angle.y;
        s32 temp = (s16)(angleStep - cur);
        s32 temp2 = (s16)(shape_angle.y - cur);
        if (!(temp * temp2 > 0)) {
            shape_angle.y = cur;
        }
    }
    setNormalSpeedF(acceleration, 0.6f, 2.5f, 1.8f); /* HD: HIO folded */
}

/* 02418E00 */
BOOL daPy_lk_c::changeFrontWallTypeProc() {
    setFrontWallType();
    f32 st = mStickDistance;
    u32 modeFlg = mModeFlg;
    s16 previousWallDelay = m3544;
    m3544 = 0;
    u32 midair = modeFlg & player04b::ModeFlg_MIDAIR;
    if (!(st > 0.05f) && midair == 0) {
        return FALSE;
    }
    u8 type = mFrontWallType;
    if (midair != 0) {
        if (type == 7) {
            LK_voiceStart(10);
            return procHangStart_init();
        }
        if (noResetFlg1() & daPyFlg1_VINE_CATCH) {
            return FALSE;
        }
        if (type == 3) {
            deleteEquipItem(1);
            shape_angle.y = (s16)(m352C + 0x8000);
            procClimbMoveUpDown_init(1);
            m35E0 = 43.67353f;
            lk_initOldFrameMorf(this, 5.0f, 0, 0x2A);
            mFrameCtrlUnder[0].setRate(0.0f);
            return TRUE;
        }
        if (type == 4) {
            f32 y = current.pos.y;
            s16 a0 = m352C;
            u16 a = (u16)a0;
            int ladderStep = (s32)((u32)gabi::ftoi((y - m3724.y) / 37.5f) - 1u); /* wraps like the original */
            f32 wallSin = cM_ssin(a);
            f32 wallCos = cM_scos(a);
            if (ladderStep < 1) {
                return procFall_init(1, 6.0f); /* HD: m_HIO->mWallCatch.m.field_0x54 folded */
            }
            f32 z = gabi::fmadds(20.5f, wallCos, m3724.z) + wallSin;
            f32 x = gabi::fmadds(20.5f, wallSin, m3724.x) - wallCos;
            shape_angle.y = (s16)(a0 + 0x8000);
            f32 ny = gabi::fmadds(37.5f, (f32)ladderStep, m3724.y);
            current.angle.y = (s16)(a0 + 0x8000);
            current.pos.x = x;
            current.pos.y = ny;
            current.pos.z = z;
            deleteEquipItem(1);
            f32 lim = m35F8 - 150.0f;
            m35E0 = 43.67353f;
            if (current.pos.y > lim) {
                current.pos.y = lim;
                m370C.x = current.pos.x; /* m370C = current.pos */
                m370C.y = lim;
                m370C.z = current.pos.z;
                procLadderUpEnd_init(1);
                lk_initOldFrameMorf(this, 5.0f, 0, 0x2A);
            } else {
                procLadderMove_init(1, 0, &current.pos);
                mFrameCtrlUnder[0].setRate(0.0f);
                lk_initOldFrameMorf(this, 5.0f, 0, 0x2A);
            }
            return TRUE;
        }
        return FALSE;
    }
    // HD: no daPyRFlg0_UNK8 check here; it lengthens the wall catch delay below
    if (type == 3) {
        if (noResetFlg1() & daPyFlg1_VINE_CATCH) {
            return FALSE;
        }
        if (modeFlg & player04b::ModeFlg_SWIM) {
            f32 y = current.pos.y - 60.0f;
            shape_angle.y = (s16)(m352C + 0x8000);
            current.pos.y = y;
            procClimbMoveUpDown_init(1);
            m35E0 = 43.67353f;
            lk_initOldFrameMorf(this, 5.0f, 0, 0x2A);
            mFrameCtrlUnder[0].setRate(0.0f);
            return TRUE;
        }
        return procClimbUpStart_init();
    }
    if (type == 4) {
        if (noResetFlg1() & daPyFlg1_VINE_CATCH) {
            return FALSE;
        }
        if (modeFlg & player04b::ModeFlg_SWIM) {
            f32 ly = m3724.y;
            int ladderStep = (s32)((u32)gabi::ftoi((current.pos.y - ly) / 37.5f) - 2u);
            u16 a = (u16)m352C;
            f32 wallSin = cM_ssin(a);
            f32 wallCos = cM_scos(a);
            f32 x = gabi::fmadds(20.5f, wallSin, m3724.x) - wallCos;
            f32 y = gabi::fmadds(37.5f, (f32)ladderStep, ly);
            f32 z = gabi::fmadds(20.5f, wallCos, m3724.z) + wallSin;
            shape_angle.y = (s16)(a + 0x8000);
            current.angle.y = (s16)(a + 0x8000);
            f32 d = mWaterY - y;
            current.pos.x = x;
            current.pos.y = y;
            current.pos.z = z;
            while (d > 90.0f) { /* m_HIO->mSwim.m.field_0x24 */
                y = y + 37.5f;
                d = mWaterY - y;
                current.pos.y = y;
            }
            procLadderMove_init(1, 0, &current.pos);
            m35E0 = 43.67353f;
            lk_initOldFrameMorf(this, 5.0f, 0, 0x2A);
            return TRUE;
        }
        return procLadderUpStart_init();
    }
    if (type == 5) {
        if (noResetFlg1() & daPyFlg1_VINE_CATCH) {
            return FALSE;
        }
        return procLadderDownStart_init();
    }
    type = mFrontWallType;
    s16 cnt = (s16)(previousWallDelay + 1);
    m3544 = cnt;
    if (type == 6) {
        if (cnt > 4) { /* m_HIO->mSmallJump.m.field_0x2 */
            return procSmallJump_init(0);
        }
        return FALSE;
    }
    // HD: the wall catch waits 14 frames instead of 7 while daPyRFlg0_UNK8 is set
    if (resetFlg0() & 8) {
        if (!(cnt > 0xE)) {
            return FALSE;
        }
    } else if (!(cnt > 7)) {
        return FALSE;
    }
    if (type == 7) {
        return procHangWallCatch_init();
    }
    if (type == 8 || type == 9) {
        return procVerticalJump_init();
    }
    m3544 = 0;
    return FALSE;
}

/* 02412948 */
BOOL daPy_lk_c::procTactPlayEnd_init(int r30) {
    int r28 = checkShipRideUseItem(0);
    commonProcInit(player04b::daPyProc_TACT_PLAY_END_e);
    dComIfGp_setPlayerStatus1(0, 1 /* daPyStts1_WIND_WAKER_CONDUCT_e */);
    initShipRideUseItem(r28, 2);
    mProcVar6 = r30;
    mNormalSpeed = 0.0f;
    setBlendMoveAnime(2.4f); /* HD: m_HIO->mBasic.m.field_0xC folded */
    if (r30 == -1) {
        u32 body = player04bGetCameraBody();
        player04bEndEventCamera(body, fopAcM_GetID(this));
        fopAc_ac_c* partner = fopAcM_SearchByID(mTactZevPartnerId);
        player04bOrderChangeEvent(this, gabi::ea(partner), (u32)m3494, 0, 0xFFFF);
        mDemo.mDemoType = 2; /* mDemo.setSystemDemoType() */
    } else if (r30 == 0) {
        u32 body = player04bGetCameraBody();
        player04bEndEventCamera(body, fopAcM_GetID(this));
        /* fopAcM_create(fpcNm_WBIRD_e, NULL, &current.pos, dComIfGp_roomControl_getStayNo()) */
        fopAcM_create(0xC4, 0, &current.pos, gabi::load<s8>(0x1047E6C8), nullptr, nullptr, -1, 0);
    } else if (r30 == 2) {
        u32 body = player04bGetCameraBody();
        player04bEndEventCamera(body, fopAcM_GetID(this));
        /* dComIfGp_event_setTalkPartner(dComIfGp_getCb1Player()) */
        u32 cb1 = gabi::load<u32>(dComIfGp_ea() + 0x5B38);
        u32 evt = dComIfGp_ea() + 0x51D0;
        u32 pid = player04bGetEventProcessId(evt, cb1);
        gabi::store<u32>(evt + 0xCC, pid);
        u32 cb1b = gabi::load<u32>(dComIfGp_ea() + 0x5B38);
        player04bOrderChangeEvent(this, cb1b, gabi::ea(player04b::l_tact_event_label()), 0, 0xFFFF);
        mDoAud_seStart(0x885 /* JA_SE_CTRL_LINK_TO_NPC */);
    } else if (r30 == 5) {
        u32 body = player04bGetCameraBody();
        player04bEndEventCamera(body, fopAcM_GetID(this));
        player04bOrderNamedEvent(this, player04b::l_tact_night_event_label(), 0, 0xFFFF);
    }
    mProcVar7 = 0;
    mProcVar3 = (s16)(shape_angle.y + 0x4000);
    return TRUE;
}

/* daPy_waterDropPcallBack_c (HD: vtable at +0) */

/* 02414048 */
void daPy_waterDropPcallBack_c::execute(JPABaseEmitter*, void* ptcl) {
    if (mbWaterMark) {
        auto* particle = gabi::at<JPABaseParticle>(gabi::ea(ptcl));
        f32 pz = (f32)particle->mGlobalPosition.z;
        f32 py = (f32)particle->mGlobalPosition.y;
        f32 px = (f32)particle->mGlobalPosition.x;
        f32 y = py + 25.0f;
        mGndChk.m_pos.z = pz; /* mGndChk.SetPos(&pos) */
        mGndChk.m_pos.x = px;
        mGndChk.m_pos.y = y;
        gabi::Local<cXyz> pos;
        pos->x = px;
        pos->z = pz;
        pos->y = y;
        // HD: compare the unrounded f64 return before storing its f32 image.
        f64 gy = player04bGroundCrossWide(dComIfG_Bgsp(), &mGndChk);
        pos->y = (f32)gy;
        if (gy > (f64)py) {
            player04bCreateEnvironmentActor(0x1D4 /* fpcNm_WATER_MARK_e */, 1, pos.get(), 0, 0);
        }
    }
}

/* daPy_dmEcallBack_c (HD: vtable at +0) */

/* 02415238 */
void daPy_dmEcallBack_c::execute(JPABaseEmitter* emitter) {
    auto* effect = player04b::emitterView(emitter);
    u32 mtx = mpMtx;
    if ((u16)player04b::dmType() == 0 /* checkFlame(): m_type == 0 */) {
        f32 oz = effect->mGlobalTranslation.z; /* getGlobalTranslation */
        f32 ox = effect->mGlobalTranslation.x;
        f32 ty = gabi::load<f32>(mtx + 0x1C);
        f32 tz = gabi::load<f32>(mtx + 0x2C);
        f32 tx = gabi::load<f32>(mtx + 0x0C);
        if (effect->mResourceVersion >= 7) { /* HD: mirrored Y (see daPy_followEcallBack_c::execute) */
            ty = -ty;
        }
        effect->mGlobalTranslation.y = ty; /* setGlobalTranslation */
        effect->mGlobalTranslation.z = tz;
        effect->mGlobalTranslation.x = tx;
        u32 m2 = mpMtx;
        f32 x = (gabi::load<f32>(m2 + 0x0C) - ox) * -0.05f;
        f32 z = (gabi::load<f32>(m2 + 0x2C) - oz) * -0.05f;
        f32 sq = gabi::fmadds(x, x, z * z);
        if (sq > 1.0f) {
            f32 distFrac = 1.0f / std_sqrtf(sq);
            z = z * distFrac;
            x = x * distFrac;
        }
        effect->mEmitterDir.z = z; /* setDirection(x, 0.6f, z) (HD +0x28) */
        effect->mEmitterDir.y = 0.6f;
        effect->mEmitterDir.x = x;
    } else {
        /* setGlobalRTMatrix(mpMtx) */
        JPASetRMtxTVecfromMtx(gabi::at<Mtx34>(mtx), &effect->mGlobalRotation, &effect->mGlobalTranslation);
    }
}

/* 02414E8C */
void daPy_swimTailEcallBack_c::execute(JPABaseEmitter* emitter) {
    auto* effect = player04b::emitterView(emitter);
    /* static JGeometry::TVec3<f32> right_dir(-1.0f, 0.0f, 0.0f) (HD: initialised on first use) */
    if ((u32)l_right_dir_guard == 0) {
        l_right_dir.y = 0.0f;
        l_right_dir_guard = 1;
        l_right_dir.x = -1.0f;
        l_right_dir.z = 0.0f;
    }
    gabi::Local<GXColor> ambColor;
    gabi::Local<GXColor> difColor;
    player04bGetSeaColor(ambColor.get(), difColor.get());
    effect->mGlobalPrmColor.r = ambColor->r; /* setGlobalPrmColor(r, g, b) */
    effect->mGlobalPrmColor.g = ambColor->g;
    effect->mGlobalPrmColor.b = ambColor->b;
    if (mbEnd) {
        gabi::Local<be<s16>> sp8;
        *sp8 = (s16)effect->mGlobalPrmColor.a;
        cLib_chaseS(sp8.get(), 0, 0x14);
        s16 a = *sp8;
        effect->mGlobalPrmColor.a = (u8)a;
        if (a == 0) {
            effect->mMaxFrame = -1; /* becomeInvalidEmitter() */
            effect->mpEmitterCallBack = 0;
            effect->mFlags = effect->mFlags | 1;
            mpEmitter = 0;
            return;
        }
    }
    if (field_0x20 != 0) {
        f32 x = mPos.x;
        f32 z = mPos.z;
        f32 y = mPos.y;
        if (effect->mResourceVersion >= 7) { /* HD: mirrored Y */
            y = -y;
        }
        effect->mGlobalTranslation.z = z; /* setGlobalTranslation */
        effect->mGlobalTranslation.x = x;
        effect->mGlobalTranslation.y = y;
        /* setGlobalRotation(TVec3<s16>(0, field_0x20->y, 0)) */
        JPAGetXYZRotateMtx(0, gabi::at<csXyz>(field_0x20)->y, 0, &effect->mGlobalRotation);
    } else {
        gabi::Local<cXyz> sp20;
        copyFloatBits(sp20->z, effect->mGlobalTranslation.z); /* getGlobalTranslation */
        copyFloatBits(sp20->y, effect->mGlobalTranslation.y);
        copyFloatBits(sp20->x, effect->mGlobalTranslation.x);
        getMaxWaterY(sp20.get());
        copyFloatBits(effect->mGlobalTranslation.y, sp20->y); /* setGlobalTranslation */
        copyFloatBits(effect->mGlobalTranslation.x, sp20->x);
        copyFloatBits(effect->mGlobalTranslation.z, sp20->z);
        if (effect->mResourceVersion >= 7) { /* HD: mirrored Y */
            effect->mGlobalTranslation.y = -effect->mGlobalTranslation.y;
        }
        cLib_chaseF(&mSpeedRate, 0.0f, 0.08f);
    }
    effect->mInitialVelDir = gabi::fmadds(12.0f, mSpeedRate, 1.0f); /* setDirectionalSpeed(1.0f + 12.0f * mSpeedRate) */
    if (field_0x05) {
        effect->mEmitterDir.x = (f32)l_right_dir.x; /* setDirection(right_dir) */
        effect->mEmitterDir.y = (f32)l_right_dir.y;
        effect->mEmitterDir.z = (f32)l_right_dir.z;
    }
    u32 link = effect->mpFirstParticle; /* getParticleList()->getFirst() */
    gabi::Local<cXyz> offsetPos;
    while (link != 0) {
        auto* node = gabi::at<player04b::ParticleLink>(link);
        JPABaseParticle* particle = node->mpParticle;
        u32 next = node->mpNext;
        /* getOffsetPosition */
        copyFloatBits(offsetPos->x, particle->mOffsetPosition.x);
        copyFloatBits(offsetPos->y, particle->mOffsetPosition.y);
        copyFloatBits(offsetPos->z, particle->mOffsetPosition.z);
        getMaxWaterY(offsetPos.get());
        copyFloatBits(particle->mOffsetPosition.x, offsetPos->x); /* setOffsetPosition */
        copyFloatBits(particle->mOffsetPosition.y, offsetPos->y);
        copyFloatBits(particle->mOffsetPosition.z, offsetPos->z);
        link = next;
    }
}

/* : HD: the eye texture offset goes to the material's "texmtx1" SRT (found by name) and
 * marks the material's texture matrix dirty, instead of the GameCube loop over 8 texture matrices */
/* 02413E80 */
void player04b::daPy_matAnm_c::calc(player04b::Material* mat) {
    u32 md = mat->mpResource;
    s32 off = (s32)gabi::at<player04b::MaterialResource>(md)->mNameTableOffset;
    u32 nameTab = off != 0 ? md + 0x38 + off : 0;
    s32 idx = gabi::at<JUTNameTab>(nameTab)->getIndex("texmtx1");
    u32 md2 = (u32)mat->mpResource;
    s32 off2 = (s32)gabi::at<player04b::MaterialResource>(md2)->mSrtTableOffset;
    u32 e = (off2 != 0 ? md2 + 0x34 + off2 : 0) + idx * 0x14;
    if ((s32)gabi::at<player04b::MaterialSrtEntry>(e)->mUniformIndex >= 0) {
        mat->mDirtyFlags = mat->mDirtyFlags | 4;
        u32 a = (u32)mat->mpDirtySrtBits + ((idx >> 5) << 2);
        gabi::store<u32>(a, gabi::load<u32>(a) | (1u << (idx & 31)));
        md2 = (u32)mat->mpResource;
    }
    s32 off3 = (s32)gabi::at<player04b::MaterialResource>(md2)->mSrtTableOffset;
    u32 i2 = (u16)gabi::at<player04b::MaterialSrtEntry>(e)->mParentIndex;
    u32 e2 = (off3 != 0 ? md2 + 0x34 + off3 : 0) + i2 * 0x14;
    if ((s32)gabi::at<player04b::MaterialSrtEntry>(e2)->mUniformIndex >= 0) {
        mat->mDirtyFlags = mat->mDirtyFlags | 4;
        u32 a = (u32)mat->mpDirtySrtBits + ((i2 >> 5) << 2);
        gabi::store<u32>(a, gabi::load<u32>(a) | (1u << (i2 & 31)));
    }
    u32 srt = (u32)mat->mpSrtValues + (u16)gabi::at<player04b::MaterialSrtEntry>(e)->mValueOffset;
    auto* textureSrt = gabi::at<player04b::MaterialSrt>(srt);
    u8 morf = (u8)player04b::morphFrame(); /* m_morf_frame */
    if (morf != 0) {
        f32 temp = 1.0f / (f32)(morf + 1);
        f32 inv = 1.0f - temp;
        f32 sx = (f32)textureSrt->mOffsetX;
        f32 x = gabi::fmadds((f32)mOldOffset.x /* mOldOffset.x */, inv, sx * temp);
        f32 sy = (f32)textureSrt->mOffsetY;
        textureSrt->mOffsetX = x;
        textureSrt->mOffsetY = gabi::fmadds(mOldOffset.y, inv, sy * temp);
    } else if ((u8)player04b::eyeMoveFlag() != 0 /* getEyeMoveFlg() */) {
        copyFloatBits(textureSrt->mOffsetX, mNowOffset.x); /* mNowOffset */
        copyFloatBits(textureSrt->mOffsetY, mNowOffset.y);
    }
    copyFloatBits(mOldOffset.x, textureSrt->mOffsetX); /* mOldOffset */
    copyFloatBits(mOldOffset.y, textureSrt->mOffsetY);
}

/* HD: the J3D animation objects carry a sead::FixedSafeString<0x20> name at +0xC (inline
 * constructor: buffer at +0xC, length 0x20, terminator at +0x2B; vtables , then
 *  and ). GHS constructors allocate when called with NULL. */
static inline void lk_fixedSafeString32_ct(u32 s) {
    if (s == 0) {
        s = gabi::ea(operator_new(0x2C));
        if (s == 0) {
            return;
        }
    }
    u32 base = s; /* sead::BufferedSafeString part (allocates 0xC when NULL) */
    if (base == 0) {
        base = gabi::ea(operator_new(0xC));
    }
    if (base != 0) {
        gabi::store<u32>(base + 4, 0x10034B3C);
        gabi::store<u32>(base + 0, s + 0xC);
        gabi::store<u32>(base + 8, 0x20);
        gabi::store<u8>(s + 0x2B, 0);
    }
    u32 buf = gabi::load<u32>(s + 0);
    gabi::store<u32>(s + 4, 0x10034B54);
    gabi::store<u8>(buf, 0);
    gabi::store<u32>(s + 4, 0x10034B6C);
}
/* J3DAnmTransform-like HD base: frame 0.0f at +0, vtable  at +4, 0 at +8, name at +0xC */
static inline void lk_anmTransformBase_ct(u32 obj) {
    gabi::store<f32>(obj + 0, 0.0f);
    gabi::store<u32>(obj + 4, 0x10034B84);
    gabi::store<u32>(obj + 8, 0);
    lk_fixedSafeString32_ct(obj + 0xC);
}

/* 02410888 */
void daPy_lk_c::createAnimeHeap(JKRSolidHeap** pHeap, int heapType) {
    u32 heapSize;
    if (heapType == player04b::HEAP_TYPE_ITEM_ANIME_e) {
        heapSize = 0x50;
    } else if (heapType == player04b::HEAP_TYPE_UNDER_UPPER_e || heapType == player04b::HEAP_TYPE_TEXTURE_ANIME_e) {
        heapSize = 0x40;
    } else {
        heapSize = 0xA0;
    }
    u32 pp = gabi::ea(pHeap);
    u32 heap = gabi::ea(mDoExt_createSolidHeapFromGameToCurrent(heapSize, 0x20));
    gabi::store<u32>(pp, heap);
    if (heap == 0) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5E7D, "*i_heap != (0)" /* "*i_heap != (0)" */);
        heap = gabi::load<u32>(pp);
        if (heap == 0) {
            return;
        }
    }
    // HD: the heap gets a name (+0x10)
    if (heapType == player04b::HEAP_TYPE_ITEM_ANIME_e) {
        gabi::store<u32>(heap + 0x10, gabi::guest_addr("daPy_lk_c::AnimeHeap TRANS_BAS_ANM")); /* "daPy_lk_c::AnimeHeap TRANS_BAS_ANM" */
        u32 obj = gabi::ea(operator_new(0x3C)); /* new mDoExt_transAnmBas(NULL) */
        if (obj != 0) {
            lk_anmTransformBase_ct(obj);
            gabi::store<u32>(obj + 0x38, 0);
            gabi::store<u32>(obj + 4, 0x10034BE4);
            mDoExt_restoreCurrentHeap();
            mDoExt_adjustSolidHeap(gabi::at<JKRSolidHeap>(gabi::load<u32>(pp)));
            return;
        }
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5E84, "tmp_trans_bas != (0)");
    } else if (heapType == player04b::HEAP_TYPE_UNDER_UPPER_e) {
        gabi::store<u32>(heap + 0x10, gabi::guest_addr("daPy_lk_c::AnimeHeap TRANS_ANM")); /* "daPy_lk_c::AnimeHeap TRANS_ANM" */
        u32 obj = gabi::ea(operator_new(0x38)); /* new J3DAnmTransformKey */
        if (obj != 0) {
            player04bClearMemory(obj, 0x38);
            lk_anmTransformBase_ct(obj);
            gabi::store<u32>(obj + 4, 0x10034BCC);
        } else {
            JUT_ASSERT_fail("d_a_player_main.cpp", 0x5E8A, "tmp_trans != (0)");
        }
    } else if (heapType == player04b::HEAP_TYPE_TEXTURE_ANIME_e) {
        gabi::store<u32>(heap + 0x10, gabi::guest_addr("daPy_lk_c::AnimeHeap TEX_PATTERN")); /* "daPy_lk_c::AnimeHeap TEX_PATTERN" */
        u32 obj = gabi::ea(operator_new(0x14)); /* new J3DAnmTexPattern */
        if (obj != 0) {
            gabi::store<u32>(obj + 0x10, 0);
            gabi::store<f32>(obj + 0, 0.0f);
            gabi::store<u32>(obj + 4, 0x1016E4EC);
            gabi::store<u32>(obj + 0xC, 0);
            gabi::store<u32>(obj + 8, 0);
            mDoExt_restoreCurrentHeap();
            mDoExt_adjustSolidHeap(gabi::at<JKRSolidHeap>(gabi::load<u32>(pp)));
            return;
        }
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5E90, "tmp_tp != (0)");
    } else {
        gabi::store<u32>(heap + 0x10, gabi::guest_addr("daPy_lk_c::AnimeHeap TEX_SCROLL")); /* "daPy_lk_c::AnimeHeap TEX_SCROLL" */
        u32 obj = gabi::ea(operator_new(0x10)); /* new J3DAnmTextureSRTKey */
        if (obj != 0) {
            gabi::store<u32>(obj + 8, 0);
            gabi::store<u32>(obj + 0xC, 0);
            gabi::store<f32>(obj + 0, 0.0f);
            gabi::store<u32>(obj + 4, 0x1016E4AC);
            mDoExt_restoreCurrentHeap();
            mDoExt_adjustSolidHeap(gabi::at<JKRSolidHeap>(gabi::load<u32>(pp)));
            return;
        }
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5E96, "tmp_tk != (0)");
    }
    mDoExt_restoreCurrentHeap();
    mDoExt_adjustSolidHeap(gabi::at<JKRSolidHeap>(gabi::load<u32>(pp)));
}

/* : the out-of-line copy of the inline matrix assignment (twelve FPR loads to a stack
 * temporary, then word copies; bit-exact) */
/* 02414250 */
static void lk_mtxCopy(Mtx34* dst, const Mtx34* src) {
    u32 t[12];
    for (u32 i = 0; i < 12; i++) {
        t[i] = lk_fbits(gabi::load<f32>(gabi::ea(src) + i * 4)); /* lfs (quiets a signalling NaN) */
    }
    for (u32 i = 0; i < 12; i++) {
        gabi::store<u32>(gabi::ea(dst) + i * 4, t[i]); /* word copies from the stack temporary */
    }
}

/* : HD, new (called by other actors through dComIfGp_getLinkPlayer()): while the mirror
 * reflection list is enabled (+0x4BE4), store the object's value (+0x9C) and append the object to the
 * list (count +0x4B58, capacity +0x4B5C, array +0x4B60) */
/* 02415A80 */
void daPy_lk_c::entryMirrorObj9CHD(void* obj, u32 v) {
    if (player04b::itemModels(this)->mMirrorEntries.mEnabled == 0) {
        return;
    }
    gabi::at<player04b::MirrorObject>(gabi::ea(obj))->mMirrorValue = v;
    s32 n = player04b::itemModels(this)->mMirrorEntries.mCount;
    if (n < player04b::itemModels(this)->mMirrorEntries.mCapacity) {
        gabi::store<u32>(player04b::itemModels(this)->mMirrorEntries.mpEntries + n * 4, gabi::ea(obj));
        player04b::itemModels(this)->mMirrorEntries.mCount = player04b::itemModels(this)->mMirrorEntries.mCount + 1;
    }
}

/* : HD, new: the same with the value at +0xA0 */
/* 02415ABC */
void daPy_lk_c::entryMirrorObjA0HD(void* obj, u32 v) {
    if (player04b::itemModels(this)->mMirrorEntries.mEnabled == 0) {
        return;
    }
    gabi::at<player04b::MirrorObject>(gabi::ea(obj))->mMirrorMask = v;
    s32 n = player04b::itemModels(this)->mMirrorEntries.mCount;
    if (n < player04b::itemModels(this)->mMirrorEntries.mCapacity) {
        gabi::store<u32>(player04b::itemModels(this)->mMirrorEntries.mpEntries + n * 4, gabi::ea(obj));
        player04b::itemModels(this)->mMirrorEntries.mCount = player04b::itemModels(this)->mMirrorEntries.mCount + 1;
    }
}

/* : HD, new: the end check of the HD selfie pose proc (): the action status is
 * 0xB and B was pressed (or daPyFlg0 0x80000 is set) */
/* 02415AF8 */
BOOL daPy_lk_c::checkSelfieEndHD() {
    if (gabi::load<u8>(dComIfGp_ea() + 0x5BB3) != 0xB) {
        return FALSE;
    }
    if (!(mItemTrigger & 2 /* BTN_B */) && !(mNoResetFlg0 & 0x80000)) {
        return FALSE;
    }
    if (dComIfGp_checkPlayerStatus0(0, 0x200000) || (mItemTrigger & 2)) {
        mDoAud_seStart(0x824);
        u32 a = dComIfGp_ea() + 0x5CDC;
        gabi::store<u32>(a, gabi::load<u32>(a) & ~0x100000u);
        setNoResetFlg1(noResetFlg1() & ~0x100u);
    }
    return TRUE;
}

/* : HD, new proc (proc table entry before procSubjectivity): probably the Picto Box selfie
 * pose. Link faces the camera; the stick/pad direction (*() + 0x124) picks one of eleven
 * face expressions (table : face, frame, keep flag) for the face texture animations. */
/* 02415BA4 */
BOOL daPy_lk_c::procSelfieHD() {
    if (checkSelfieEndHD()) {
        u32 a = dComIfGp_ea() + 0x5CDC;
        gabi::store<u32>(a, gabi::load<u32>(a) & ~0x100000u);
        procWait_init();
        mNoResetFlg0 = mNoResetFlg0 & ~0x80000u;
        return TRUE;
    }
    if (dComIfGp_getCameraAttentionStatus(mCameraInfoIdx) & 0x10) {
        u32 st1 = gabi::load<u32>(dComIfGp_ea() + 0x5CDC);
        s32 idx = mCameraInfoIdx;
        if (st1 & 0x100000) {
            dComIfGp_get(); /* HD: an unused accessor call (inline dComIfGp_getCamera) */
            idx = mCameraInfoIdx;
            u32 cam = gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8);
            gabi::Local<be<s16>> ang;
            u32 p = player04bConstructSignedAngle(ang.get(), (s32)(s16)(gabi::load<s16>(cam + 0x236) + 0x1500));
            s16 inv = player04bInvertSignedAngle(p);
            shape_angle.y = inv;
            current.angle.y = inv;
            u32 pad = gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x124);
            s32 pose = (pad & 4) ? 5 : 0;
            for (int k = 1; k <= 4; k++) {
                if (l_selfie_pad[k - 1] & pad) {
                    pose += k;
                    break;
                }
            }
            if (pad & 0x80) {
                pose = 10;
            }
            auto* entry = &l_selfie_pose[pose];
            u32 face = entry->mFace;
            mFace = face;
            f32 frame = entry->mFrame;
            setTextureAnime(face & 0xFFFF, gabi::ftoi(frame));
            gabi::store<f32>(mpAnmTexPatternData, (f32)(u32)m3530);
            gabi::store<f32>(mpTexScrollResData, (f32)(u32)m3532);
            // HD: the three animation objects at 0x644/0x69C/0x6F4 (value = fn(ctx, frame, a, b))
            auto* channels = gabi::at<player04b::AnimationChannel>(gabi::ea(m644));
            for (int i = 0; i < 3; i++) {
                f32 f = (f32)(u32)(i < 2 ? m3530 : m3532);
                u32 o = channels[i].mpAnimation;
                auto* value = gabi::at<player04b::AnimationValue>(o);
                f32 r = gabi::call_ptr<f32>(value->mpInterpolate, (u32)value->mpContext, f,
                                          (f32)value->mStart, (f32)value->mEnd);
                value->mFrame = r;
            }
            u8 keep = entry->mKeep;
            u32 mf = mModeFlg;
            if (keep != 0) {
                mModeFlg = mf | 0x100 | 0x08000080;
            } else {
                m3530 = (u16)gabi::ftoi(frame);
                mModeFlg = (mf & ~0x100u) | 0x08000080;
            }
            if (face == 0x92) {
                setNoResetFlg1(noResetFlg1() | 0x100);
            } else {
                setNoResetFlg1(noResetFlg1() & ~0x100u);
            }
        } else {
            u32 cam = gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8);
            mModeFlg = mModeFlg & 0xF7FFFE7F;
            s16 y = gabi::load<s16>(cam + 0x236);
            shape_angle.y = y;
            current.angle.y = y;
        }
    }
    mNoResetFlg0 = mNoResetFlg0 & ~0x80000u;
    return TRUE;
}

/* HD: the inline constructor of the render shape object shared by the sight packet and the sword
 * blur (sub-objects at +0, +0xC, +0xB4; material block at +0x128 with its identity matrices) */
static inline void lk_hdShape_ct(u32 sub) {
    auto* shape = gabi::at<player04b::Shape>(sub);
    player04bConstructShaderParameters(sub);
    player04bConstructUniformBlock(sub + 0xC);
    shape->mVertexMaterialVtable = 0x1016EF84;
    player04bClearMemory(gabi::ea(&shape->mModelMtx), 0x34);
    if (sub + 0xC + 0x74 == 0) {
        operator_new(0x30);
    }
    player04bConstructUniformBlock(sub + 0xB4);
    shape->mPixelMaterialVtable = 0x1016EFB4;
    player04bClearMemory(gabi::ea(&shape->mUniformValues[0]), 0x2F0);
    {
        /* the material's matrices and colours: 1.0 at these offsets, 0.0 elsewhere in 0x128..0x1D4 */
        constexpr u16 ones[] = {0x154, 0x144, 0x1A4, 0x164, 0x1B4, 0x184, 0x194, 0x134, 0x174, 0x1C4, 0x1D4};
        for (u32 o = 0x128; o <= 0x1D4; o += 4) {
            bool one = false;
            for (u32 k = 0; k < sizeof(ones) / sizeof(ones[0]); k++) {
                if (ones[k] == o) {
                    one = true;
                }
            }
            gabi::store<f32>(sub + o, one ? 1.0f : 0.0f);
        }
    }
    player04bConstructArray(sub + 0x1D8, 2, 0x10, 0x024443B8u);
    player04bConstructArray(sub + 0x1F8, 2, 0x10, 0x024443B8u);
    player04bConstructArray(sub + 0x218, 2, 0x10, 0x024443B8u);
    {
        /* inline member constructors (allocate when their address is NULL) */
        constexpr u16 offs30[] = {0x110, 0x140, 0x170, 0x1A0, 0x1D0, 0x200, 0x230, 0x260};
        constexpr u16 offs10[] = {0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0};
        for (u32 k = 0; k < 8; k++) {
            if (gabi::ea(&shape->mUniformValues[0]) + offs30[k] == 0) {
                operator_new(0x30);
            }
        }
        for (u32 k = 0; k < 6; k++) {
            if (gabi::ea(&shape->mUniformValues[0]) + offs10[k] == 0) {
                operator_new(0x10);
            }
        }
    }
}
// HD: "name" material lookup in the material manager (027FFCBC); loads it on first use
static inline u32 lk_hdFindMaterial(u32 nameStr) {
    gabi::Local<player04b::SafeString> name;
    name->__vtbl = LK_SAFESTRING_VTBL;
    name->mpString = nameStr;
    u32 mgr = player04bGetMaterialManager();
    s32 idx = player04bFindMaterial( (u32)gabi::at<player04b::MaterialManager>(mgr)->mpArchive, name.get());
    if (idx < 0) {
        return 0;
    }
    u32 n = (u32)gabi::at<player04b::MaterialManager>(mgr)->mCount;
    u32 arr = (u32)gabi::at<player04b::MaterialManager>(mgr)->mpMaterials;
    u32 e = (u32)idx < n ? arr + idx * sizeof(player04b::MaterialEntry) : arr;
    if ((u8)gabi::at<player04b::MaterialEntry>(e)->mReady == 0) {
        u32 m = (u32)gabi::at<player04b::MaterialManager>(mgr)->mpArchive;
        u32 src = (u32)idx < (u32)gabi::at<player04b::MaterialArchive>(m)->mCount ? (u32)gabi::at<player04b::MaterialArchive>(m)->mpMaterials + idx * sizeof(player04b::MaterialSource) : 0;
        player04bLoadMaterial( e, src, 0);
        n = (u32)gabi::at<player04b::MaterialManager>(mgr)->mCount;
        arr = (u32)gabi::at<player04b::MaterialManager>(mgr)->mpMaterials;
    }
    return (u32)idx < n ? arr + idx * sizeof(player04b::MaterialEntry) : arr;
}
// HD: a vertex buffer slot (+0) allocated from the graphics heap when empty, then bound
static inline void lk_hdVtxBuf(u32 slot, u32 fmt, u32 mat, u32 curMat) {
    auto* buffer = gabi::at<player04b::VertexSlot>(slot);
    u32 v = (u32)buffer->mpVertices;
    if (v == 0) {
        u32 heap = player04bGetCurrentHeap(gabi::load<u32>(0x101F8B4C));
        u32 p = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(heap + 0xC) + 0x34), heap, 0x50, 0x40);
        if (p != 0) {
            buffer->mpAllocation = p;
            buffer->mAllocationType = 4;
        }
        v = (u32)buffer->mpAllocation;
        buffer->mpVertices = v;
    }
    player04bSetVertexBuffer(gabi::ea(&buffer->mBuffer), v, 4, fmt);
    if (mat != 0 && mat != gabi::load<u32>(curMat)) {
        player04bBindVertexArray(mat, gabi::ea(buffer->mVertexArray), gabi::ea(&buffer->mBuffer), fmt, 0);
    }
}

/* : daPy_sightPacket_c::daPy_sightPacket_c (HD: a 0xC08 render object with its own shape and
 * material data: two 0x254 vertex buffers at +0x54, the "default_sight" material, the Always 0x71 and
 * Link 0x72 sight textures). GHS: allocates when called with NULL. */
/* 0240EE9C */
static void* daPy_sightPacket_ct(void* i_self) {
    u32 self = gabi::ea(i_self);
    if (self == 0) {
        self = gabi::ea(operator_new(0xC08));
        if (self == 0) {
            return nullptr;
        }
    }
    auto* packet = gabi::at<player04b::daPy_sightPacket_c>(self);
    player04bConstructDrawListBase(self);
    u32 buf = gabi::ea(&packet->mVertices);
    auto* vertices = gabi::at<player04b::VertexPair>(buf);
    gabi::store<u32>(self, 0x10037C18); /* vtable */
    u32 r30 = buf;
    if (buf == 0) {
        r30 = gabi::ea(operator_new(0x4C0));
    }
    if (r30 != 0) {
        player04bConstructArray(r30, 2, 0x254, 0x0244435Cu);
        gabi::store<u32>(r30 + 0x4A8, 0);
        gabi::store<u32>(r30 + 0x4AC, 0);
        gabi::store<u32>(r30 + 0x4B8, 0);
        gabi::store<u32>(r30 + 0x4B0, 0x14);
        gabi::store<u8>(r30 + 0x4BC, 0);
        gabi::store<u32>(r30 + 0, 0);
        gabi::store<u32>(r30 + 0x254, 0);
    }
    lk_hdShape_ct(gabi::ea(&packet->mShape));
    packet->mShapeReady = 0;
    player04bConstructDrawCall(gabi::ea(&packet->mDrawCall));
    player04bConstructSampler(gabi::ea(&packet->mSampler));
    player04bConstructTexture(gabi::ea(&packet->mLockTexture));
    player04bConstructTexture(gabi::ea(&packet->mSightTexture));
    packet->mpMaterial = lk_hdFindMaterial(0x10035B78 /* "default_sight" */);
    vertices->mFormat = 0x11;
    vertices->mpFormatData = 0x10036240;
    u32 r26 = (u32)packet->mpMaterial;
    for (u32 i = 0; i < 2; i++) {
        lk_hdVtxBuf(buf + i * 0x254, gabi::ea(&vertices->mFormat), r26, gabi::ea(&vertices->mpMaterial));
    }
    vertices->mpMaterial = r26;
    vertices->mReady = 1;
    player04bCreateShape(gabi::ea(&packet->mShape), 1, 0);
    packet->mIndices[0] = 0;
    packet->mIndices[1] = 2;
    packet->mIndices[2] = 1;
    packet->mIndices[3] = 3;
    player04bSetDrawCallIndices(gabi::ea(&packet->mDrawCall), gabi::ea(&packet->mIndices[0]), 4, 4);
    packet->mDrawCall.mPrimitiveType = 6;
    {
        u32 vb = gabi::load<u32>(buf + (u32)vertices->mCurrent * 0x254); /* the current vertex buffer: four corners */
        constexpr s8 vals[20] = {-1, 1, 0, 0, 0, 1, 1, 0, 1, 0, -1, -1, 0, 0, 1, 1, -1, 0, 1, 1};
        for (u32 k = 0; k < 20; k++) {
            gabi::store<f32>(vb + k * 4, (f32)vals[k]);
        }
    }
    {
        u32 cur = buf + (u32)vertices->mCurrent * 0x254;
        player04bFlushVertexBuffer(cur + 4, 0, gabi::load<u32>(cur + 0x150));
    }
    vertices->mCurrent = (u32)vertices->mCurrent == 0 ? 1 : 0;
    player04bLockGraphics(gabi::load<u32>(0x101F8B18));
    gabi::Local<player04b::SafeString> arc1;
    arc1->__vtbl = LK_SAFESTRING_VTBL;
    arc1->mpString = 0x10035B70; /* "Always" */
    u32 img = player04bGetResource(gabi::load<u32>(0x101F4F28), arc1.get(), 0x71);
    if (img == 0) {
        JUT_ASSERT_fail("d_a_player_eff.inc" /* "d_a_player_eff.inc" */, 0x124, "tmp_img != (0)" /* "tmp_img != (0)" */);
    }
    packet->mpSightTex = img + gabi::load<u32>(img + 0x1C); /* setSightTex */
    player04bLoadTextureImage(gabi::ea(&packet->mSightTexture), gabi::load<u32>(img + 0x20));
    gabi::Local<player04b::SafeString> arc2;
    arc2->__vtbl = LK_SAFESTRING_VTBL;
    arc2->mpString = 0x101CEB48; /* l_arcName "Link" */
    img = player04bGetResource(gabi::load<u32>(0x101F4F28), arc2.get(), 0x72);
    if (img == 0) {
        JUT_ASSERT_fail("d_a_player_eff.inc", 0x12E, "tmp_img != (0)");
    }
    packet->mpLockTex = img + gabi::load<u32>(img + 0x1C); /* setLockTex */
    player04bLoadTextureImage(gabi::ea(&packet->mLockTexture), gabi::load<u32>(img + 0x20));
    packet->mpImg = img; /* setImage */
    player04bUnlockGraphics(gabi::load<u32>(0x101F8B18));
    packet->mSampler.mWrapZ = 2;
    packet->mSampler.mWrapY = 2;
    packet->mSampler.mDirtyFlags = (u8)packet->mSampler.mDirtyFlags | 2;
    packet->mSampler.mWrapX = 2;
    return gabi::at<void>(self);
}

/* : daPy_swBlur_c::daPy_swBlur_c (HD: the sword blur became a 0x1243C render object: 120 vertex
 * buffers of 0x254 at +0x660 (60 trail segments, double buffered), its own shape, the "blur_default"
 * material and the Link 0x70 blur texture). GHS: allocates when called with NULL. */
/* 0240F564 */
static void* daPy_swBlur_ct(void* i_self) {
    u32 self = gabi::ea(i_self);
    if (self == 0) {
        self = gabi::ea(operator_new(0x1243C));
        if (self == 0) {
            return nullptr;
        }
    }
    auto* packet = gabi::at<player04b::daPy_swBlur_c>(self);
    player04bConstructPacket(self);
    packet->mBlurColorType = 0;
    packet->mBlurTopRate = 0.0f;
    packet->mpMaterial = 0;
    packet->__vtbl = 0x10037BE8; /* vtable */
    packet->mpPosBuffer = 0;
    packet->mSampleIndex = 0;
    packet->mpTex = 0;
    packet->mPointCount = 0;
    u32 buf = gabi::ea(&packet->mVertices);
    auto* vertices = gabi::at<player04b::VertexArray>(buf);
    u32 r28 = buf;
    if (buf == 0) {
        r28 = gabi::ea(operator_new(0x11778));
    }
    if (r28 != 0) {
        player04bConstructArray(r28, 0x78, 0x254, 0x0244463Cu);
        gabi::store<u32>(r28 + 0x11760, 0);
        gabi::store<u8>(r28 + 0x11774, 0);
        gabi::store<u32>(r28 + 0x11768, 0x14);
        gabi::store<u32>(r28 + 0x11764, 0);
        gabi::store<u32>(r28 + 0x11770, 0);
        for (u32 i = 0; i < 2; i++) {
            for (u32 j = 0; j < 0x3C; j++) {
                gabi::store<u32>(r28 + i * 0x254 + j * 0x4A8, 0);
            }
        }
    }
    u32 shape = gabi::ea(&packet->mShape);
    lk_hdShape_ct(shape);
    gabi::store<u8>(shape + 0x418, 0);
    u32 disp = gabi::ea(&packet->mDrawCall);
    player04bConstructDrawCall(disp);
    u32 idxs = gabi::ea(&packet->mIndices[0]);
    player04bClearMemory(idxs, 8);
    player04bConstructSampler(gabi::ea(&packet->mSampler));
    player04bConstructTexture(gabi::ea(&packet->mTexture));
    packet->mpMaterial = lk_hdFindMaterial(0x10035BAC /* "blur_default" */);
    u32 fmt = gabi::ea(&vertices->mFormat);
    gabi::store<u32>(fmt, 0x11);
    vertices->mpFormatData = 0x10036238;
    u32 mat = (u32)packet->mpMaterial;
    for (u32 i = 0; i < 2; i++) {
        for (u32 j = 0; j < 0x3C; j++) {
            lk_hdVtxBuf(buf + i * 0x254 + j * 0x4A8, fmt, mat, gabi::ea(&vertices->mpMaterial));
        }
    }
    vertices->mpMaterial = mat;
    vertices->mReady = 1;
    player04bCreateShape(shape, 1, 0);
    for (u32 k = 0; k < 4; k++) {
        gabi::store<s16>(idxs + k * 2, (s16)k);
    }
    player04bSetDrawCallIndices(disp, idxs, 4, 4);
    gabi::store<u32>(disp + 4, 6);
    /* clear the current buffer of every segment: a quad strip edge (1.0 in the colour/alpha slots) */
    for (u32 k = 0; k < 0x3C; k++) {
        u32 idx = (u32)vertices->mCurrent;
        u32 vb = gabi::load<u32>(buf + (k * 2 + idx) * 0x254);
        u32 end = vb + 0x40;
        if (vb < end) {
            for (u32 q = vb; q < end; q += 0x20) { /* dcbz */
                for (u32 b = 0; b < 0x20; b += 4) {
                    gabi::store<u32>((q & ~31u) + b, 0);
                }
            }
            idx = (u32)vertices->mCurrent;
        }
        vb = gabi::load<u32>(buf + (k * 2 + idx) * 0x254);
        for (u32 o = 0; o < 0x50; o += 4) {
            bool one = o == 0x38 || o == 0x48 || o == 0x4C || o == 0x20;
            gabi::store<f32>(vb + o, one ? 1.0f : 0.0f);
        }
    }
    /* copy it into the other buffer of the segment */
    {
        u32 idx = (u32)vertices->mCurrent;
        u32 other = buf + (idx == 0 ? 1u : 0u) * 0x254;
        for (u32 k = 0; k < 0x3C; k++) {
            u32 src = buf + (k * 2 + idx) * 0x254;
            u32 dst = other + k * 0x4A8;
            for (u32 g = 0; g < 4; g++) {
                u32 s = gabi::load<u32>(src);
                u32 d = gabi::load<u32>(dst);
                for (u32 w = 0; w < 5; w++) {
                    gabi::store<f32>(d + g * 0x14 + w * 4, gabi::load<f32>(s + g * 0x14 + w * 4));
                }
            }
            idx = (u32)vertices->mCurrent;
        }
        u32 cur = buf + idx * 0x254 + 4;
        for (u32 k = 0; k < 0x3C; k++) {
            player04bFlushVertexBuffer(cur + k * 0x4A8, 0, gabi::load<u32>(cur + k * 0x4A8 + 0x14C));
        }
    }
    vertices->mCurrent = (u32)vertices->mCurrent == 0 ? 1 : 0;
    player04bLockGraphics(gabi::load<u32>(0x101F8B18));
    gabi::Local<player04b::SafeString> arc;
    arc->__vtbl = LK_SAFESTRING_VTBL;
    arc->mpString = 0x101CEB48; /* l_arcName "Link" */
    u32 img = player04bGetResource(gabi::load<u32>(0x101F4F28), arc.get(), 0x70);
    if (img == 0) {
        JUT_ASSERT_fail("d_a_player_eff.inc", 0x312, "tmp_img != (0)");
    }
    u32 tex = gabi::ea(&packet->mTexture);
    auto* texture = &packet->mTexture;
    player04bLoadTextureImage(tex, gabi::load<u32>(img + 0x20));
    player04bUnlockGraphics(gabi::load<u32>(0x101F8B18));
    /* take the texture object over into the sampler ( unless only the image changed) */
    u32 smp = gabi::ea(&packet->mSampler);
    auto* sampler = gabi::at<player04b::Sampler>(smp);
    constexpr be<u32> player04b::Texture::* descriptorFields[] = {
        &player04b::Texture::mDimension, &player04b::Texture::mWidth, &player04b::Texture::mHeight, &player04b::Texture::mDepth, &player04b::Texture::mMipCount, &player04b::Texture::mFormat, &player04b::Texture::mSwizzle, &player04b::Texture::mTileMode, &player04b::Texture::mAaMode
    };
    bool same = true;
    for (u32 k = 0; k < sizeof(descriptorFields) / sizeof(descriptorFields[0]); k++) {
        if ((u32)(sampler->mTexture.*descriptorFields[k]) != (u32)(texture->*descriptorFields[k])) {
            same = false;
            break;
        }
    }
    if (!same) {
        player04bAssignTexture(smp, tex);
    } else {
        u32 a = (u32)texture->mpImage;
        u32 b = (u32)texture->mpMipmaps;
        sampler->mTexture.mpImage = a;
        sampler->mpBoundMipmaps = b;
        sampler->mTexture.mpMipmaps = b;
        sampler->mpBoundImage = a;
    }
    sampler->mWrapY = 2;
    sampler->mWrapX = 2;
    sampler->mWrapZ = 2;
    sampler->mDirtyFlags = (u8)sampler->mDirtyFlags | 2;
    return gabi::at<void>(self);
}

/* : daPy_lk_c::daPy_lk_c (member constructors in layout order; HD: the sword blur is allocated
 * inside a named heap scope "(剣ブラー)daPy_swBlur_c" (/) with 32-byte alignment).
 * GHS: allocates when called with NULL; the inline member constructors allocate when their address is NULL. */
static inline void lk_ct_listInit(u32 p, u32 n, u32 nullSize) {
    if (p == 0) {
        p = gabi::ea(operator_new(nullSize));
        if (p == 0) {
            return;
        }
    }
    gabi::store<u32>(p + 0, 0);
    gabi::store<u32>(p + 4, 0);
    gabi::store<u32>(p + 8, 0);
    player04bSetPointerArrayBuffer(p, (s32)n, p + 0xC);
}
/* 0240FE40 */
static void* daPy_lk_ct(void* i_self) {
    u32 self = gabi::ea(i_self);
    if (self == 0) {
        self = gabi::ea(operator_new(0x8284));
        if (self == 0) {
            return nullptr;
        }
    }
    auto* player = gabi::at<daPy_lk_c>(self);
    auto* packets = player04b::modelPackets(player);
    auto* items = player04b::itemModels(player);
    auto* background = player04b::backgroundChecks(player);
    auto* renderAnimations = gabi::at<player04b::AnimationChannel>(gabi::ea(player->m644));
    player04bConstructPlayerBase(self);
    player->__vtbl = 0x10037CF0; /* vtable */
    lk_ct_listInit(gabi::ea(&player->m458), 0xA, 0x34);
    {
        u32 p = gabi::ea(&player->m48C);
        if (p == 0) {
            p = gabi::ea(operator_new(0x168));
        }
        if (p != 0) {
            player04bConstructArray(p, 0xA, 0x24, 0x02444280u);
        }
    }
    player04bConstructAnimationBackend(gabi::ea(&renderAnimations[0]));
    renderAnimations[0].mFlags = 0x0;
    renderAnimations[0].__vtbl = 0x1016D860;
    player04bConstructAnimationBackend(gabi::ea(&renderAnimations[1]));
    renderAnimations[1].__vtbl = 0x1016D860;
    renderAnimations[1].mFlags = 0x0;
    player04bConstructAnimationBackend(gabi::ea(&renderAnimations[2]));
    renderAnimations[2].mFlags = 0x0;
    renderAnimations[2].__vtbl = 0x1016D7E0;
    player04bConstructArray(gabi::ea(&player->mAcchCir), 0x3, 0x40, 0x24EFE94u);
    player04bConstructAcch(gabi::ea(&player->mAcch));
    player->mAcch.cBgS_Chk::__vtbl = 0x10034DA4;
    player->mAcch.dBgS_GrpPassChk::__vtbl = 0x10034DB4;
    player->mAcch.dBgS_PolyPassChk::__vtbl = 0x10034DC4;
    player->mAcch.dBgS_PolyPassChk::mbLinkThrough = 0x1;
    player04bConstructLineCheck(gabi::ea(&background->mLinkLinChk));
    background->mLinkLinChk.dBgS_GrpPassChk::mGrp = 0x1;
    background->mLinkLinChk.dBgS_PolyPassChk::mbLinkThrough = 0x1;
    background->mLinkLinChk.dBgS_PolyPassChk::mbBombThrough = 0x0;
    background->mLinkLinChk.cBgS_Chk::mPolyPassChk = gabi::ea(&background->mLinkLinChk.dBgS_PolyPassChk::__vtbl);
    background->mLinkLinChk.dBgS_PolyPassChk::mbCamThrough = 0x0;
    background->mLinkLinChk.dBgS_PolyPassChk::mbObjThrough = 0x0;
    background->mLinkLinChk.dBgS_GrpPassChk::__vtbl = 0x10034E34;
    background->mLinkLinChk.dBgS_PolyPassChk::mbBoomerangThrough = 0x0;
    background->mLinkLinChk.dBgS_PolyPassChk::mbRopeThrough = 0x0;
    background->mLinkLinChk.cBgS_Chk::__vtbl = 0x10034E14;
    background->mLinkLinChk.dBgS_PolyPassChk::__vtbl = 0x10034E44;
    background->mLinkLinChk.cBgS_Chk::mGrpPassChk = gabi::ea(&background->mLinkLinChk.dBgS_GrpPassChk::__vtbl);
    background->mLinkLinChk.dBgS_PolyPassChk::mbArrowThrough = 0x0;
    background->mLinkLinChk.cBgS_PolyInfo::__vtbl = 0x10034E24;
    player04bConstructLineCheck(gabi::ea(&background->mRopeLinChk));
    background->mRopeLinChk.dBgS_PolyPassChk::mbCamThrough = 0x0;
    background->mRopeLinChk.dBgS_PolyPassChk::mbLinkThrough = 0x0;
    background->mRopeLinChk.dBgS_PolyPassChk::mbObjThrough = 0x0;
    background->mRopeLinChk.dBgS_GrpPassChk::mGrp = 0x1;
    background->mRopeLinChk.cBgS_Chk::mPolyPassChk = gabi::ea(&background->mRopeLinChk.dBgS_PolyPassChk::__vtbl);
    background->mRopeLinChk.dBgS_PolyPassChk::mbRopeThrough = 0x1;
    background->mRopeLinChk.cBgS_Chk::__vtbl = 0x10034F14;
    background->mRopeLinChk.dBgS_GrpPassChk::__vtbl = 0x10034F34;
    background->mRopeLinChk.dBgS_PolyPassChk::mbArrowThrough = 0x0;
    background->mRopeLinChk.dBgS_PolyPassChk::mbBombThrough = 0x0;
    background->mRopeLinChk.cBgS_Chk::mGrpPassChk = gabi::ea(&background->mRopeLinChk.dBgS_GrpPassChk::__vtbl);
    background->mRopeLinChk.dBgS_PolyPassChk::__vtbl = 0x10034F44;
    background->mRopeLinChk.dBgS_PolyPassChk::mbBoomerangThrough = 0x0;
    background->mRopeLinChk.cBgS_PolyInfo::__vtbl = 0x10034F24;
    player04bConstructLineCheck(gabi::ea(&background->mBoomerangLinChk));
    background->mBoomerangLinChk.dBgS_PolyPassChk::mbRopeThrough = 0x0;
    background->mBoomerangLinChk.dBgS_PolyPassChk::mbObjThrough = 0x0;
    background->mBoomerangLinChk.dBgS_PolyPassChk::mbBoomerangThrough = 0x1;
    background->mBoomerangLinChk.dBgS_PolyPassChk::mbBombThrough = 0x0;
    background->mBoomerangLinChk.dBgS_GrpPassChk::__vtbl = 0x10034EF4;
    background->mBoomerangLinChk.dBgS_PolyPassChk::__vtbl = 0x10034F04;
    background->mBoomerangLinChk.cBgS_Chk::__vtbl = 0x10034ED4;
    background->mBoomerangLinChk.dBgS_PolyPassChk::mbCamThrough = 0x0;
    background->mBoomerangLinChk.cBgS_Chk::mPolyPassChk = gabi::ea(&background->mBoomerangLinChk.dBgS_PolyPassChk::__vtbl);
    background->mBoomerangLinChk.cBgS_Chk::mGrpPassChk = gabi::ea(&background->mBoomerangLinChk.dBgS_GrpPassChk::__vtbl);
    background->mBoomerangLinChk.dBgS_GrpPassChk::mGrp = 0x1;
    background->mBoomerangLinChk.dBgS_PolyPassChk::mbArrowThrough = 0x0;
    background->mBoomerangLinChk.cBgS_PolyInfo::__vtbl = 0x10034EE4;
    background->mBoomerangLinChk.dBgS_PolyPassChk::mbLinkThrough = 0x0;
    player04bConstructGroundCheck(gabi::ea(&background->mGndChk));
    background->mGndChk.dBgS_PolyPassChk::mbBoomerangThrough = 0x0;
    background->mGndChk.dBgS_PolyPassChk::mbRopeThrough = 0x0;
    background->mGndChk.dBgS_GrpPassChk::mGrp = 0x1;
    background->mGndChk.dBgS_PolyPassChk::mbCamThrough = 0x0;
    background->mGndChk.dBgS_PolyPassChk::mbBombThrough = 0x0;
    background->mGndChk.dBgS_GrpPassChk::__vtbl = 0x10034C94;
    background->mGndChk.cBgS_Chk::mGrpPassChk = gabi::ea(&background->mGndChk.dBgS_GrpPassChk::__vtbl);
    background->mGndChk.dBgS_PolyPassChk::mbLinkThrough = 0x1;
    background->mGndChk.cBgS_Chk::__vtbl = 0x10034C74;
    background->mGndChk.cBgS_Chk::mPolyPassChk = gabi::ea(&background->mGndChk.dBgS_PolyPassChk::__vtbl);
    background->mGndChk.dBgS_PolyPassChk::mbObjThrough = 0x0;
    background->mGndChk.cBgS_PolyInfo::__vtbl = 0x10034C84;
    background->mGndChk.dBgS_PolyPassChk::__vtbl = 0x10034CA4;
    background->mGndChk.dBgS_PolyPassChk::mbArrowThrough = 0x0;
    player04bConstructRoofCheck(gabi::ea(&background->mRoofChk));
    background->mRoofChk.cBgS_PolyInfo::__vtbl = 0x10034D64;
    background->mRoofChk.cBgS_Chk::__vtbl = 0x10034D74;
    background->mRoofChk.dBgS_PolyPassChk::mbLinkThrough = 0x1;
    background->mRoofChk.dBgS_GrpPassChk::__vtbl = 0x10034D84;
    background->mRoofChk.dBgS_PolyPassChk::__vtbl = 0x10034D94;
    player04bConstructLineCheck(gabi::ea(&background->mArrowLinChk));
    background->mArrowLinChk.dBgS_PolyPassChk::mbBoomerangThrough = 0x0;
    background->mArrowLinChk.dBgS_PolyPassChk::mbBombThrough = 0x0;
    background->mArrowLinChk.dBgS_PolyPassChk::mbCamThrough = 0x0;
    background->mArrowLinChk.dBgS_PolyPassChk::__vtbl = 0x10034E84;
    background->mArrowLinChk.dBgS_PolyPassChk::mbLinkThrough = 0x0;
    background->mArrowLinChk.dBgS_PolyPassChk::mbObjThrough = 0x0;
    background->mArrowLinChk.cBgS_Chk::__vtbl = 0x10034E54;
    background->mArrowLinChk.cBgS_Chk::mGrpPassChk = gabi::ea(&background->mArrowLinChk.dBgS_GrpPassChk::__vtbl);
    background->mArrowLinChk.dBgS_GrpPassChk::__vtbl = 0x10034E74;
    background->mArrowLinChk.cBgS_Chk::mPolyPassChk = gabi::ea(&background->mArrowLinChk.dBgS_PolyPassChk::__vtbl);
    background->mArrowLinChk.cBgS_PolyInfo::__vtbl = 0x10034E64;
    background->mArrowLinChk.dBgS_PolyPassChk::mbRopeThrough = 0x0;
    background->mArrowLinChk.dBgS_PolyPassChk::mbArrowThrough = 0x1;
    background->mArrowLinChk.dBgS_GrpPassChk::mGrp = 0x5;
    player04bConstructLineCheck(gabi::ea(&background->mMirLightLinChk));
    background->mMirLightLinChk.dBgS_PolyPassChk::mbCamThrough = 0x0;
    background->mMirLightLinChk.dBgS_PolyPassChk::mbBoomerangThrough = 0x0;
    background->mMirLightLinChk.dBgS_PolyPassChk::mbObjThrough = 0x0;
    background->mMirLightLinChk.dBgS_GrpPassChk::mGrp = 0x1F;
    background->mMirLightLinChk.cBgS_Chk::__vtbl = 0x10034E94;
    background->mMirLightLinChk.dBgS_PolyPassChk::mbBombThrough = 0x0;
    background->mMirLightLinChk.dBgS_PolyPassChk::mbLinkThrough = 0x0;
    background->mMirLightLinChk.dBgS_GrpPassChk::__vtbl = 0x10034EB4;
    background->mMirLightLinChk.dBgS_PolyPassChk::mbArrowThrough = 0x1;
    background->mMirLightLinChk.cBgS_Chk::mPolyPassChk = gabi::ea(&background->mMirLightLinChk.dBgS_PolyPassChk::__vtbl);
    background->mMirLightLinChk.dBgS_PolyPassChk::__vtbl = 0x10034EC4;
    background->mMirLightLinChk.dBgS_PolyPassChk::mbRopeThrough = 0x0;
    background->mMirLightLinChk.cBgS_PolyInfo::__vtbl = 0x10034EA4;
    background->mMirLightLinChk.cBgS_Chk::mGrpPassChk = gabi::ea(&background->mMirLightLinChk.dBgS_GrpPassChk::__vtbl);
    player04bConstructGroundCheck(gabi::ea(&background->mLavaGndChk));
    background->mLavaGndChk.dBgS_PolyPassChk::mbObjThrough = 0x1;
    background->mLavaGndChk.dBgS_PolyPassChk::mbArrowThrough = 0x0;
    background->mLavaGndChk.dBgS_PolyPassChk::mbBombThrough = 0x0;
    background->mLavaGndChk.dBgS_PolyPassChk::mbCamThrough = 0x0;
    background->mLavaGndChk.dBgS_PolyPassChk::mbLinkThrough = 0x0;
    background->mLavaGndChk.dBgS_PolyPassChk::__vtbl = 0x10034D24;
    background->mPolyInfo.mpBgW = 0x0;
    background->mLavaGndChk.cBgS_Chk::mPolyPassChk = gabi::ea(&background->mLavaGndChk.dBgS_PolyPassChk::__vtbl);
    background->mLavaGndChk.cBgS_Chk::__vtbl = 0x10034CF4;
    background->mLavaGndChk.dBgS_PolyPassChk::mbRopeThrough = 0x0;
    background->mPolyInfo.mPolyIndex = 0xFFFF;
    background->mLavaGndChk.cBgS_Chk::mGrpPassChk = gabi::ea(&background->mLavaGndChk.dBgS_GrpPassChk::__vtbl);
    background->mLavaGndChk.dBgS_GrpPassChk::mGrp = 0xE;
    background->mPolyInfo.mActorId = 0xFFFFFFFF;
    background->mLavaGndChk.dBgS_PolyPassChk::mbBoomerangThrough = 0x0;
    background->mLavaGndChk.cBgS_PolyInfo::__vtbl = 0x10034D04;
    background->mPolyInfo.mBgIndex = 0x100;
    background->mLavaGndChk.dBgS_GrpPassChk::__vtbl = 0x10034D14;
    background->mPolyInfo.__vtbl = 0x10034BAC;
    player->mSwgripmsabBckAnim.mFrameCtrl.init(0);
    player04b::bckAnm(&player->mSwgripmsabBckAnim)->__vtbl = 0x1016E54C;
    player04bConstructAnimationBackend(gabi::ea(&player04b::bckAnm(&player->mSwgripmsabBckAnim)->mBackend));
    player04b::bckAnm(&player->mSwgripmsabBckAnim)->field_0x84 = 0x0;
    player04b::bckAnm(&player->mSwgripmsabBckAnim)->mBackend.mFlags = 0x0;
    player04b::bckAnm(&player->mSwgripmsabBckAnim)->mBackend.__vtbl = 0x1016D820;
    player04b::bckAnm(&player->mSwgripmsabBckAnim)->field_0x88 = 0x0;
    player04b::bckAnm(&player->mSwgripmsabBckAnim)->field_0x80 = 0x0;
    player04b::bckAnm(&player->mSwgripmsabBckAnim)->field_0x7C = 0x0;
    player04b::bckAnm(&player->mSwgripmsabBckAnim)->__vtbl = 0x10034BFC;
    mDoExt_brkAnm::ct(&items->mpTswgripmsabBrk);
    mDoExt_btkAnm::ct(&items->mpTswgripmsBtk);
    items->mAtngshaBck.mFrameCtrl.init(0);
    player04b::bckAnm(&items->mAtngshaBck)->__vtbl = 0x1016E54C;
    player04bConstructAnimationBackend(gabi::ea(&player04b::bckAnm(&items->mAtngshaBck)->mBackend));
    player04b::bckAnm(&items->mAtngshaBck)->field_0x80 = 0x0;
    player04b::bckAnm(&items->mAtngshaBck)->field_0x7C = 0x0;
    player04b::bckAnm(&items->mAtngshaBck)->field_0x84 = 0x0;
    player04b::bckAnm(&items->mAtngshaBck)->mBackend.__vtbl = 0x1016D820;
    player04b::bckAnm(&items->mAtngshaBck)->mBackend.mFlags = 0x0;
    player04b::bckAnm(&items->mAtngshaBck)->__vtbl = 0x10034BFC;
    player04b::bckAnm(&items->mAtngshaBck)->field_0x88 = 0x0;
    mDoExt_btkAnm::ct(&items->mpTshmsBtk);
    player04bConstructMirrorPacket(self + 0xF8C);
    mDoExt_btkAnm::ct(&items->mpYmsls00Btk);
    items->mSwordAnim.mFrameCtrl.init(0);
    player04b::bckAnm(&items->mSwordAnim)->__vtbl = 0x1016E54C;
    player04bConstructAnimationBackend(gabi::ea(&player04b::bckAnm(&items->mSwordAnim)->mBackend));
    player04b::bckAnm(&items->mSwordAnim)->mBackend.__vtbl = 0x1016D820;
    player04b::bckAnm(&items->mSwordAnim)->field_0x84 = 0x0;
    player04b::bckAnm(&items->mSwordAnim)->__vtbl = 0x10034BFC;
    player04b::bckAnm(&items->mSwordAnim)->field_0x7C = 0x0;
    player04b::bckAnm(&items->mSwordAnim)->field_0x80 = 0x0;
    player04b::bckAnm(&items->mSwordAnim)->field_0x88 = 0x0;
    player04b::bckAnm(&items->mSwordAnim)->mBackend.mFlags = 0x0;
    mDoExt_brkAnm::ct(&items->mpBombBrk);
    mDoExt_brkAnm::ct(&items->mpGwp00BrkData);
    mDoExt_btkAnm::ct(&items->mpGwp00BtkData);
    mDoExt_btkAnm::ct(&items->mpGicer00Btk);
    mDoExt_btkAnm::ct(&items->mpIceArrowBtk);
    mDoExt_btkAnm::ct(&items->mpLightArrowBtk);
    mDoExt_btkAnm::ct(&items->mpGicer01Btk);
    mDoExt_btkAnm::ct(&items->mpEquipItemBtk);
    mDoExt_btkAnm::ct(&items->mpSwordBtk);
    mDoExt_brkAnm::ct(&items->mpEquipItemBrk);
    mDoExt_bpkAnm::ct(&items->mpCutfBpk);
    mDoExt_btkAnm::ct(&items->mpCutfBtk);
    mDoExt_brkAnm::ct(&items->mpCutfBrk);
    mDoExt_brkAnm::ct(&items->m2EFC);
    lk_ct_listInit(gabi::ea(&items->mMirrorEntries), 0x20, 0x8C);
    player04bConstructModelPacket(gabi::ea(&packets->mBeforeArray[0]));
    player04bConstructModelPacket(gabi::ea(&packets->mBeforeArray[1]));
    player04bConstructModelPacket(gabi::ea(&packets->mBeforeArray[2]));
    player04bConstructModelPacket(gabi::ea(&packets->mBeforeArray[3]));
    player04bConstructModelPacket(gabi::ea(&packets->mBeforeArray[4]));
    player04bConstructModelPacket(gabi::ea(&packets->mBeforeArray[5]));
    player04bConstructArray(gabi::ea(packets->mArray), 0x2, 0xB0, 0x2080404u);
    player04bConstructModelPacket(gabi::ea(&packets->mAfterArray[0]));
    player04bConstructModelPacket(gabi::ea(&packets->mAfterArray[1]));
    player04bConstructModelPacket(gabi::ea(&packets->mAfterArray[2]));
    mDoExt_btkAnm::ct(&items->mpSuimenMunyaBtk);
    items->mYuchw00Bck.mFrameCtrl.init(0);
    player04b::bckAnm(&items->mYuchw00Bck)->__vtbl = 0x1016E54C;
    player04bConstructAnimationBackend(gabi::ea(&player04b::bckAnm(&items->mYuchw00Bck)->mBackend));
    player04b::bckAnm(&items->mYuchw00Bck)->mBackend.mFlags = 0x0;
    player04b::bckAnm(&items->mYuchw00Bck)->mBackend.__vtbl = 0x1016D820;
    player04b::bckAnm(&items->mYuchw00Bck)->field_0x88 = 0x0;
    player04b::bckAnm(&items->mYuchw00Bck)->field_0x84 = 0x0;
    player04b::bckAnm(&items->mYuchw00Bck)->__vtbl = 0x10034BFC;
    player04b::bckAnm(&items->mYuchw00Bck)->field_0x7C = 0x0;
    player04b::bckAnm(&items->mYuchw00Bck)->field_0x80 = 0x0;
    mDoExt_btkAnm::ct(&items->mpYuchw00Btk);
    mDoExt_brkAnm::ct(&items->mpYuchw00Brk);
    mDoExt_btkAnm::ct(&items->mpYbafo00Btk);
    mDoExt_brkAnm::ct(&items->mYaura00rBrk);
    mDoExt_btkAnm::ct(&items->mpYaura00Btk);
    mDoExt_brkAnm::ct(&items->mYmgcs00Brk);
    mDoExt_btkAnm::ct(&items->mpYmgcs00Btk);
    player04bConstructArray(gabi::ea(&player->mAnmRatioUnder), 0x2, 0x10, 0x24442C0u);
    player04bConstructArray(gabi::ea(&player->mAnmRatioUpper), 0x3, 0x10, 0x24442C0u);
    player04bConstructArray(gabi::ea(&player->mFrameCtrlUnder), 0x2, 0x10, 0x2444310u);
    player04bConstructArray(gabi::ea(&player->mFrameCtrlUpper), 0x3, 0x10, 0x2444310u);
    daPy_sightPacket_ct(&player->mSightPacket);
    player04bConstructAnimeSound(gabi::ea(&player->mJAIZelAnime));
    player->m_sanm_buffer = 0x0;
    gabi::at<player04b::AnimeSoundPrefix>(gabi::ea(player->mJAIZelAnime))->mVtable = 0x100037A0;
    player04bConstructArray(gabi::ea(&player->mFootEffect), 0x2, 0x4C, 0x24443E4u);
    player04bConstructRippleCallback(gabi::ea(&player->m3280));
    player04bConstructArray(gabi::ea(&player->mSwimTailEcallBack), 0x2, 0x28, 0x2444440u);
    player->m32F0.mVtable = 0x100347F0;
    player->m32E4.mVtable = 0x100347F0;
    player04bConstructSmokeCallback(gabi::ea(&player->mSmokeEcallBack), 0x1);
    gabi::at<player04b::CutTurnCallback>(gabi::ea(player->m332C))->__vtbl = 0x10052228;
    gabi::at<player04b::CutTurnCallback>(gabi::ea(player->m331C))->__vtbl = 0x10052228;
    gabi::at<daPy_waterDropEcallBack_c>(gabi::ea(player->m334C))->__vtbl = 0x10037B68;
    gabi::at<daPy_waterDropEcallBack_c>(gabi::ea(player->m336C))->__vtbl = 0x10037B68;
    gabi::at<daPy_mtxPosFollowEcallBack_c>(gabi::ea(player->m33A8))->__vtbl = 0x10037CB0;
    gabi::at<player04b::CutTurnCallback>(gabi::ea(player->m333C))->__vtbl = 0x10052228;
    gabi::at<daPy_followEcallBack_c>(gabi::ea(player->m338C))->__vtbl = 0x10037B28;
    player04bConstructArray(gabi::ea(&player->mDmEcallBack), 0x4, 0xC, 0x2444480u);
    player->m33E8.mVtable = 0x100347F0;
    gabi::at<daPy_followEcallBack_c>(gabi::ea(player->m3438))->__vtbl = 0x10037B28;
    gabi::at<daPy_followEcallBack_c>(gabi::ea(player->m3410))->__vtbl = 0x10037B28;
    player->m342C.mVtable = 0x100347F0;
    gabi::at<daPy_fanSwingEcallBack_c>(gabi::ea(player->mFanSwingCb))->__vtbl = 0x10037BA8;
    gabi::at<daPy_mtxPosFollowEcallBack_c>(gabi::ea(player->m3400))->__vtbl = 0x10037CB0;
    player->m3454.mVtable = 0x100347F0;
    player04bConstructArray(gabi::ea(&player->m3460), 0x2, 0x10, 0x24444C0u);
    player->m68D0 = 1.0f;
    {
        u32 p = gabi::ea(&player->mpSeAnm); /* SafeString member */
        if (p == 0) {
            p = gabi::ea(operator_new(8));
        }
        if (p != 0) {
            gabi::at<player04b::SafeString>(p)->__vtbl = LK_SAFESTRING_VTBL;
            gabi::at<player04b::SafeString>(p)->mpString = 0x10000160;
        }
    }
    player04bConstructArray(gabi::ea(&player->mFootData), 0x2, 0x118, 0x2444500u);
    player04bConstructCollisionStatus(gabi::ea(&player->mStts));
    player04bConstructCollisionGlobalStatus(self + 0x763C);
    player->mStts.__vtbl = 0x1004AE88;
    player->mStts.__vtbl_gstts = 0x1004AEC0;
    player04bConstructCollisionObjectInfo(gabi::ea(&player->mCyl));
    player04b::shapeAttribute(player->mCyl).mVtable = 0x100015A8;
    player04b::shapeAttribute(player->mCyl).mAab.mVtable = 0x10034B9C;
    player04bConstructCylinder(gabi::ea(&player->mCyl.mCyl));
    player->mCyl.__vtbl_hitinf = 0x1004B108;
    player04b::shapeAttribute(player->mCyl).mVtable = 0x1004B160;
    player->mCyl.mCyl.__vtbl = 0x1004B150;
    player04bConstructCollisionObjectInfo(gabi::ea(&player->mWindCyl));
    player04b::shapeAttribute(player->mWindCyl).mAab.mVtable = 0x10034B9C;
    player04b::shapeAttribute(player->mWindCyl).mVtable = 0x100015A8;
    player04bConstructCylinder(gabi::ea(&player->mWindCyl.mCyl));
    player->mWindCyl.mCyl.__vtbl = 0x1004B150;
    player04b::shapeAttribute(player->mWindCyl).mVtable = 0x1004B160;
    player->mWindCyl.__vtbl_hitinf = 0x1004B108;
    player04bConstructCollisionObjectInfo(gabi::ea(&player->mAtCyl));
    player04b::shapeAttribute(player->mAtCyl).mAab.mVtable = 0x10034B9C;
    player04b::shapeAttribute(player->mAtCyl).mVtable = 0x100015A8;
    player04bConstructCylinder(gabi::ea(&player->mAtCyl.mCyl));
    player->mAtCyl.__vtbl_hitinf = 0x1004B108;
    player->mAtCyl.mCyl.__vtbl = 0x1004B150;
    player04b::shapeAttribute(player->mAtCyl).mVtable = 0x1004B160;
    player04bConstructCollisionObjectInfo(gabi::ea(&player->mLightCyl));
    player04b::shapeAttribute(player->mLightCyl).mAab.mVtable = 0x10034B9C;
    player04b::shapeAttribute(player->mLightCyl).mVtable = 0x100015A8;
    player04bConstructCylinder(gabi::ea(&player->mLightCyl.mCyl));
    player->mLightCyl.mCyl.__vtbl = 0x1004B150;
    player->mLightCyl.__vtbl_hitinf = 0x1004B108;
    player04b::shapeAttribute(player->mLightCyl).mVtable = 0x1004B160;
    player04bConstructArray(gabi::ea(&player->mAtCps[0]), 0x3, 0x138, 0x24445B0u);
    player04bConstructCollisionObjectInfo(gabi::ea(&player->mFanWindCps));
    player04b::shapeAttribute(player->mFanWindCps).mVtable = 0x100015A8;
    player04b::shapeAttribute(player->mFanWindCps).mAab.mVtable = 0x10034B9C;
    player04bConstructCapsule(gabi::ea(player->mFanWindCps.mCps));
    gabi::at<player04b::CapsuleGeometry>(gabi::ea(player->mFanWindCps.mCps))->__vtbl = 0x1004AF60;
    player->mFanWindCps.__vtbl_hitinf = 0x1004AF18;
    player04b::shapeAttribute(player->mFanWindCps).mVtable = 0x1004AF70;
    player04bConstructSphere(gabi::ea(&player->mFanWindSph));
    u32 cyl = gabi::ea(&player->mFanLightCps);
    player04bConstructCollisionObjectInfo(cyl);
    player04b::shapeAttribute(player->mFanLightCps).mVtable = 0x100015A8;
    player04b::shapeAttribute(player->mFanLightCps).mAab.mVtable = 0x10034B9C;
    player04bConstructCapsule(gabi::ea(player->mFanLightCps.mCps));
    player->mFanLightCps.__vtbl_hitinf = 0x1004AF18;
    gabi::at<player04b::CapsuleGeometry>(gabi::ea(player->mFanLightCps.mCps))->__vtbl = 0x1004AF60;
    player04b::shapeAttribute(player->mFanLightCps).mVtable = 0x1004AF70;
    gabi::Local<u32> heapScope;
    player04bBeginNamedHeap(heapScope.get(), "(\214\225\203u\203\211\201[)daPy_swBlur_c", 0x22700, 0);
    u32 blur = player04bAllocateAligned(0x1243C, 0x20);
    if (blur != 0) {
        blur = gabi::ea(daPy_swBlur_ct(gabi::at<void>(blur)));
    }
    player->mpSwBlur = blur;
    player04bEndNamedHeap(heapScope.get(), 2);
    return gabi::at<void>(self);
}

/* dBgS_GndChk (stack object, 0x54; HD layout, this TU's vtables) */
/* model->setBaseTRMtx(src) (twelve FPR loads, then the stores) */
static inline void lk_setBaseTRMtx_b(u32 model, u32 src) {
    u32 t[12];
    for (u32 i = 0; i < 12; i++) {
        t[i] = lk_fbits(gabi::load<f32>(src + i * 4));
    }
    for (u32 i = 0; i < 12; i++) {
        gmem_stf32(model + 0xC8 + i * 4, t[i]);
    }
}
/* if (changeSwimProc()) { procSwimWait_init(0); m34C2 = 0; } else procWait_init(); */
static inline void lk_swimWaitOrWait(daPy_lk_c* p) {
    if (p->changeSwimProc()) {
        p->procSwimWait_init(0);
        p->m34C2 = 0;
    } else {
        p->procWait_init();
    }
}

/* : daPy_lk_c::makeBgWait (HD: the treasure chest check became a ground check under Link (a
 * moving ground of actor 0x124 lowers Link onto it and waits while there is no ground below); the start
 * speeds are constants; the hat animation and  run before the model update) */
/* 024131D0 */
BOOL daPy_lk_c::makeBgWait() {
    u32 self = gabi::ea(this);
    if (m352E != 0) { /* m352E */
        m352E = (s16)(m352E - 1);
    }
    mAcch.CrrPos(dComIfG_Bgsp());
    if (mAcch.m_ground_h == -1000000000.0f) { /* mAcch.GetGroundH() == -G_CM3D_F_INF */
        return 0;
    }
    if (m352E != 0 && player04bCheckMovingBackground(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd))) {
        u32 ap = player04bGetBackgroundActor(dComIfG_Bgsp(), (u32)mAcch.m_gnd.mBgIndex);
        if (ap != 0 && gabi::load<s16>(ap + 8) == 0x124) {
            gabi::Local<dBgS_GndChk> chk;
            player04bConstructGroundCheck(gabi::ea(chk.get()));
            chk->dBgS_GrpPassChk::__vtbl = 0x10034C54;
            copyFloatBits(gabi::ea(chk.get()) + 0x2C, self + 0x31C);
            chk->mbObjThrough = 0;
            chk->cBgS_Chk::__vtbl = 0x10034C34;
            chk->mPolyPassChk = gabi::ea(chk.get()) + 0x40;
            chk->mbBombThrough = 0;
            chk->mGrpPassChk = gabi::ea(chk.get()) + 0x4C;
            copyFloatBits(gabi::ea(chk.get()) + 0x24, self + 0x314);
            chk->cBgS_PolyInfo::__vtbl = 0x10034C44;
            u32 gh = gmem_ld32(gabi::ea(&mAcch.m_ground_h));
            chk->mbLinkThrough = 0;
            chk->mGrp = 1;
            gmem_stf32(gabi::ea(&current.pos.y), gh); /* current.pos.y = mAcch.GetGroundH() */
            chk->dBgS_PolyPassChk::__vtbl = 0x10034C64;
            chk->mbBoomerangThrough = 0;
            chk->mbArrowThrough = 0;
            gmem_stf32(gabi::ea(chk.get()) + 0x28, gh);
            chk->mbCamThrough = 0;
            chk->mbRopeThrough = 0;
            f32 y = (f32)player04bGroundCrossWide(dComIfG_Bgsp(), chk.get());
            chk->cBgS_PolyInfo::__vtbl = 0x10034C44;
            chk->dBgS_PolyPassChk::__vtbl = 0x10034C64;
            chk->dBgS_GrpPassChk::__vtbl = 0x10034C24;
            player04bDestroyBackgroundCheck(gabi::ea(chk.get()), 0);
            if (y == -1000000000.0f) {
                return 0;
            }
        }
    }
    if ((mParameters & 0x80) && m352E != 0) {
        if (!player04bCheckMovingBackground(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd))) {
            return 0;
        }
        u32 ap = player04bGetBackgroundActor(dComIfG_Bgsp(), (u32)mAcch.m_gnd.mBgIndex);
        if (ap == 0 || gabi::load<s16>(ap + 8) != 0x44 /* fpcNm_OBJ_IKADA_e */) {
            return 0;
        }
        u32 gh = gmem_ld32(gabi::ea(&mAcch.m_ground_h));
        m352E = 0;
        gmem_stf32(gabi::ea(&current.pos.y), gh);
    }
    setWaterY();
    u32 tactStart = mParameters & 0x40;
    if (tactStart == 0) {
        /* dComIfGs_setRestartRoom(current.pos, shape_angle.y, getStartRoomNo()) */
        player04bSetRestartRoom(gabi::load<u32>(0x101F84DC) + 0x1148, &current.pos,
                   (s32)shape_angle.y, mParameters & 0x3F);
    }
    setRoomInfo();
    u32 startMode = (mParameters >> 12) & 0xF;
    u32 sceneMode = gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1177); /* dComIfGs_getLastSceneMode() */
    bool ship;
    if (startMode == 2 || startMode == 9 || sceneMode == 6 || (startMode == 5 && sceneMode == 1)) {
        ship = true;
    } else if (sceneMode != 5) {
        ship = false;
    } else if (lk_isStartStage(gabi::guest_addr("Pjavdou")) || lk_isStartStage(gabi::guest_addr("ShipD"))) {
        ship = true;
    } else if (lk_isStartStage(gabi::guest_addr("Siren"))) {
        ship = current.roomNo == 0 && current.pos.z > 1400.0f && current.pos.x < 400.0f &&
               current.pos.x > -400.0f;
    } else {
        ship = false;
    }
    if (ship) {
        if (!gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644)->isEventBit(0xF80 /* MET_KORL */)) {
            lk_swimWaitOrWait(this);
        } else {
            u32 shipAc = gabi::ea(dComIfGp_getShipActor()); /* dComIfGp_getShipActor() */
            if (shipAc == 0) {
                return 0;
            }
            if (gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644)->isEventBit(0x3E10)) {
                gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644)->onEventBit(0x3E01);
                gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644)->onEventBit(0x3F80);
            }
            s16 ang = shape_angle.y;
            if (tactStart != 0) {
                m34DE = ang; /* m34DE */
                player04bInitShipStartPosition(shipAc, &current.pos, (s32)(s16)(ang + 0x4000));
            } else {
                player04bInitShipStartPosition(shipAc, &current.pos, (s32)ang);
            }
            if (gabi::load<u32>(shipAc + 0x644) & 0x200) { /* getSailOn() */
                procShipSteer_init();
            } else {
                procShipPaddle_init();
            }
            if (tactStart != 0) {
                procTactPlayEnd_init(-2);
            } else if (sceneMode == 6) {
                gabi::store<u8>(shipAc + 0x636, 0x10); /* setStartModeThrow() */
            } else if (sceneMode == 4 || sceneMode == 5) {
                mDemo.mDemoType = 4; /* mDemo.setStartDemoType() */
                mDoAud_subBgmStop();
                procShipRestart_init();
            } else if (startMode == 9) {
                gabi::store<u8>(shipAc + 0x636, 0xD); /* setStartModeWarp() */
                gabi::Local<cXyz> pos;
                f32 y = current.pos.y;
                s8 room = current.roomNo;
                copyFloatBits(gabi::ea(pos.get()) + 0, self + 0x314);
                copyFloatBits(gabi::ea(pos.get()) + 8, self + 0x31C);
                pos->y = gabi::fadds_ppc(y, 1500.0f);
                u32 id = fopAcM_create( 0x1BB /* fpcNm_TORNADO_e */, 2, pos.get(), (s32)room, 0, 0, -1, 0);
                gabi::store<u32>(shipAc + 0x714, id); /* setTactWarpID */
            }
        }
    } else if (startMode == 4) {
        mDemo.mDemoType = 4;
        mDoAud_bgmStart(0x8000000Fu /* JA_BGM_I_MAJU_JAIL */);
        procLargeDamage_init(-7, 0, 0, 0);
    } else if (sceneMode == 4 || sceneMode == 5) {
        if (changeSwimProc()) {
            procSwimWait_init(0);
            m34C2 = 0;
        } else {
            gmem_stf32(gabi::ea(&current.pos.y), gmem_ld32(gabi::ea(&mAcch.m_ground_h)));
            mDemo.mDemoType = 4;
            mDoAud_subBgmStop();
            procLargeDamageUp_init(sceneMode == 4 ? -1 : -2, 1, 0, 0);
        }
    } else if (startMode == 1 || startMode == 5) {
        if (mEventIdx == 0xFF) { /* mEventIdx */
            u32 save = gabi::load<u32>(0x101F84DC);
            s16 ang = current.angle.y;
            u32 speed = gmem_ld32(save + 0x1170); /* dComIfGs_getLastSceneSpeedF() */
            mDemo.mDemoMoveAngle = ang;      /* mDemo.setMoveAngle */
            mDemo.mTimer = 0x23;     /* mDemo.setTimer */
            mDemo.mDemoType = 4;        /* mDemo.setStartDemoType */
            mDemo.mDemoMode = 0xE;      /* DEMO_KEEP_e */
            gmem_stf32(gabi::ea(&mNormalSpeed), speed); /* mNormalSpeed */
            if (changeSwimProc()) {
                if (startMode == 1) {
                    mNormalSpeed = 9.0f;
                    speedF = 9.0f;
                } else {
                    gmem_stf32(gabi::ea(&speedF), gmem_ld32(gabi::ea(&mNormalSpeed)));
                }
                procSwimMove_init(0);
            } else if (startMode == 1) {
                mNormalSpeed = 8.5f;
                speedF = 8.5f;
                procMove_init();
            } else if (sceneMode == 2) {
                m34E8 = shape_angle.y; /* m34E8 */
                procCrawlMove_init(0, 0);
            } else if (sceneMode == 3) {
                s16 a = (s16)(shape_angle.y - 0x8000);
                m34DE = a;
                current.angle.y = a;
                shape_angle.y = a;
                m34E8 = (s16)(a + 0x8000);
                procCrawlMove_init(0, 0);
            } else {
                u32 v = gmem_ld32(gabi::ea(&mNormalSpeed));
                f32 f;
                memcpy(&f, &v, 4);
                if (f > 17.0f) {
                    mNormalSpeed = 17.0f;
                    v = lk_fbits(17.0f);
                }
                gmem_stf32(gabi::ea(&speedF), v);
                procMove_init();
            }
        } else {
            lk_swimWaitOrWait(this);
        }
    } else if (startMode == 7) {
        s16 ang = shape_angle.y;
        u32 idx = (u32)(u16)ang >> 3;
        f32 x = current.pos.x;
        f32 y = current.pos.y;
        u32 prm = mParameters;
        f32 s = gabi::load<f32>(0x104A44F8 + idx * 8);
        f32 z = current.pos.z;
        f32 c = gabi::load<f32>(0x104A44F8 + idx * 8 + 4);
        gabi::Local<cXyz> pos;
        pos->x = gabi::fmadds(200.0f, s, x);
        pos->y = gabi::fadds_ppc(y, 150.0f);
        pos->z = gabi::fmadds(200.0f, c, z);
        player04bSetRestartRoom(gabi::load<u32>(0x101F84DC) + 0x1148, pos.get(), (s32)ang, prm & 0x3F);
        procVomitJump_init(1);
        mAcch.m_flags = mAcch.m_flags & ~0x20u; /* mAcch.ClrGroundHit() */
    } else if (startMode == 0xD) {
        procSmallJump_init(1);
    } else if (startMode == 0xF) {
        procSlowFall_init();
    } else if (startMode == 0xE) {
        mDemo.mDemoType = 4;
        mDemo.mDemoMode = 1; /* DEMO_N_WAIT_e */
        procWait_init();
        gabi::Local<csXyz> ang;
        player04bConstructAngleVector(ang.get(), 0, 0, 0);
        s32 id = player04bCreateChildActor(0x77 /* fpcNm_FM_e */, (u32)mBsPcId, 0,
                                 &current.pos, (s32)current.roomNo, ang.get(), 0, -1, 0);
        if (id != -1) {
            mNoResetFlg0 = mNoResetFlg0 | daPyFlg0_NO_DRAW;
        }
    } else if (changeSwimProc()) {
        procSwimWait_init(0);
        m34C2 = 0;
    } else if (current.pos.y - mAcch.m_ground_h > 30.1f) {
        procFall_init(1, 0.0f);
    } else {
        procWait_init();
    }
    lk_initOldFrameMorf(this, 0.0f, 0, 0x2A);
    setWorldMatrix();
    checkOriginalHatAnimation();
    animeUpdate();
    setAnimeRatioHD();
    mpCLModel->calc();
    {
        u32 m = gabi::ea(lk_getAnmMtx(mpCLModel.get(), 0xF /* CL_JNT_HEAD_JNT_e */));
        lk_setBaseTRMtx_b(mpKatsuraModel.v, m); /* mpKatsuraModel */
    }
    mpKatsuraModel->calc();
    {
        u32 m = gabi::ea(lk_getAnmMtx(mpCLModel.get(), 0xF));
        lk_setBaseTRMtx_b(mpYamuModel.v, m); /* mpYamuModel */
    }
    mpYamuModel->calc();
    setItemModel();
    player04bSetCylinderCenter(&mCyl.mCyl, &current.pos);
    setAttentionPos();
    u32 code = player04bGetGroundCode(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
    m3580 = code;
    m357C = code;
    if (sceneMode == 4) {
        u32 m = gabi::ea(lk_getAnmMtx(mpCLModel.get(), 0 /* CL_JNT_LINK_ROOT_e */));
        gabi::Local<cXyz> pos;
        copyFloatBits(gabi::ea(pos.get()) + 0, m + 0xC);
        copyFloatBits(gabi::ea(pos.get()) + 4, m + 0x1C);
        copyFloatBits(gabi::ea(pos.get()) + 8, m + 0x2C);
        /* dComIfGp_particle_setP1(dPa_name::ID_AK_SN_OSHIRIKUROKOGE, &pos) */
        dPa_control_set(gabi::at<dPa_control_c>(gabi::load<u32>(dComIfGp_ea() + 0x5AB0)), 1, 0x8089, pos, nullptr, nullptr, 0xFF,
                        nullptr, -1, nullptr, nullptr, nullptr);
    }
    if (gabi::load<u32>(gabi::load<u32>(0x101F84DC) + 0x1174) & 0x8000) {
        changeDragonShield(0);
    } else {
        mTinkleShieldTimer = (u16)(gabi::load<u32>(gabi::load<u32>(0x101F84DC) + 0x1174) >> 16); /* mTinkleShieldTimer */
    }
    if (gabi::load<u32>(gabi::load<u32>(0x101F84DC) + 0x1174) & 0x4000) {
        mNoResetFlg1 = mNoResetFlg1 | daPyFlg1_SOUP_POWER_UP;
    }
    // HD: integer copies preserve the exact guest float/angle bits in the shared debug statics.
    for (u32 k = 0; k < 3; k++) {
        gabi::at<be<u32>>(gabi::ea(&player04b::l_debug_keep_pos()))[k] = gabi::at<be<u32>>(gabi::ea(&current.pos))[k];
    }
    for (u32 k = 0; k < 3; k++) {
        gabi::at<be<u16>>(gabi::ea(&player04b::l_debug_shape_angle()))[k] = gabi::at<be<u16>>(gabi::ea(&shape_angle))[k];
    }
    for (u32 k = 0; k < 3; k++) {
        gabi::at<be<u16>>(gabi::ea(&player04b::l_debug_current_angle()))[k] = gabi::at<be<u16>>(gabi::ea(&current.angle))[k];
    }
    return 2; /* cPhs_NEXT_e */
}

/* HD: bind a material's shader for drawing (the inline cache update at 's state: +0 the
 * current program, +4 the current shader) */
static inline void lk_hdBindShader(u32 cache, u32 sh) {
    auto* cacheState = gabi::at<player04b::ShaderCache>(cache);
    auto* shader = gabi::at<player04b::Shader>(sh);
    if (sh == (u32)cacheState->mpShader) {
        return;
    }
    u8 fl = (u8)shader->mFlags;
    u32 cur = (u32)cacheState->mpProgram;
    if (fl & 2) {
        shader->mFlags = fl & ~2;
        player04bUpdateShaderProgram(sh, 0);
    }
    u32 prog = (u32)gabi::at<player04b::ShaderResource>(shader->mpResource)->mpProgram;
    if (cur != prog) {
        player04bBindShaderGroup(prog);
    }
    u32 n = (u32)shader->mDisplayListSize;
    if (n != 0) {
        GX2CallDisplayList((u32)shader->mpDisplayList, n);
        cacheState->mpProgram = prog;
        cacheState->mpShader = sh;
    } else {
        player04bBindShaderProgram(sh);
        cacheState->mpShader = sh;
        cacheState->mpProgram = prog;
    }
}
/* HD GX2 state block on the stack ( constructs,  applies) */
/* : daPy_swBlur_c::draw (HD: GX2 rendering. Binds the "blur_default" material, sets the colour
 * (by blur type at +0xA4: three colour words at ) as material constants, fills the strip of
 * quads from the two point lists at +0xBC and +0x38C with alpha fading along the blur, then draws each
 * segment) */
/* 024142F0 */
void player04b::daPy_swBlur_c::draw() {
    auto* packet = this;
    u32 cache = player04bGetActiveShader(0x104B45C0u);
    lk_hdBindShader(cache, gabi::load<u32>((u32)packet->mpMaterial));
    gabi::Local<Mtx34> mtx;
    lk_mtxCopy(mtx.get(), gabi::at<Mtx34>(0x104B45F8));
    u32 shape = gabi::ea(&packet->mShape);
    auto* renderShape = gabi::at<player04b::Shape>(shape);
    player04bSetMaterialView(shape, 0, mtx.get(), gabi::at<void>(0x104B470C), gabi::load<u32>(0x104B4708) + 0x240);
    player04bUpdateShape(shape, 0);
    {
        u32 s = (u32)renderShape->mpShapeData;
        u32 mat = (u32)packet->mpMaterial;
        u32 e = s + 0x10 + (u32)gabi::at<player04b::ShapeResource>(s)->mUniformBlockIndex * 0x1C;
        u32 t = (u32)gabi::at<player04b::DrawMaterial>(mat)->mUniformBlockCount != 0 ? (u32)gabi::at<player04b::DrawMaterial>(mat)->mpUniformBlocks : 0;
        s32 a = (s16)gabi::at<player04b::UniformBinding>(t)->mVertexLocation;
        u32 r24 = (u32)gabi::at<player04b::UniformBlock>(e)->mSize;
        s32 b = (s16)gabi::at<player04b::UniformBinding>(t)->mPixelLocation;
        u32 r29 = (u32)gabi::at<player04b::UniformBlock>(e)->mpData;
        s32 c = (s16)gabi::at<player04b::UniformBinding>(t)->mGeometryLocation;
        if (b != -1) {
            GX2SetPixelUniformBlock(b, r24, r29);
        }
        if (a != -1) {
            GX2SetVertexUniformBlock(a, r24, r29);
        }
        if (c != -1) {
            GX2SetGeometryUniformBlock(c, r24, r29);
        }
    }
    {
        u32 type = (u32)packet->mBlurColorType;
        u32 col = type == player04b::daPy_swBlur_c::NORMAL_SLASH_BLUR ? gabi::load<u32>(gabi::ea(&n_color0)) : type == player04b::daPy_swBlur_c::ELIXIR_SOUP_SLASH_BLUR ? gabi::load<u32>(gabi::ea(&y_color0)) : gabi::load<u32>(gabi::ea(&g_color0));
        renderShape->mUniformValues[16] = (f32)(col >> 24) / 255.0f;
        renderShape->mUniformValues[17] = (f32)((col >> 16) & 0xFF) / 255.0f;
        renderShape->mUniformValues[18] = (f32)((col >> 8) & 0xFF) / 255.0f;
        renderShape->mUniformValues[19] = (f32)(col & 0xFF) / 255.0f;
    }
    player04bFlushMaterialUniforms(shape + 0xB4);
    gabi::call_ptr(gabi::load<u32>((u32)renderShape->mPixelMaterialVtable + 0x2C), shape + 0xB4, (u32)packet->mpMaterial);
    player04bFlushMaterialUniforms(shape + 0xC);
    gabi::call_ptr(gabi::load<u32>((u32)renderShape->mVertexMaterialVtable + 0x2C), shape + 0xC, (u32)packet->mpMaterial);
    {
        u32 mat = (u32)packet->mpMaterial;
        u32 t = (u32)gabi::at<player04b::DrawMaterial>(mat)->mSamplerCount != 0 ? (u32)gabi::at<player04b::DrawMaterial>(mat)->mpSamplerBindings : 0;
        player04bBindTexture(gabi::ea(&packet->mSampler), t + 4, -1, 0);
    }
    {
        gabi::Local<player04b::RenderState> st;
        u32 sp = gabi::ea(st.get());
        player04bConstructRenderState(sp);
        st.get()->mCullEnable = 0;
        st.get()->mDepthWrite = 0;
        st.get()->mWriteRed = 1;
        st.get()->mWriteBlue = 1;
        st.get()->mDepthFunc = 2;
        st.get()->mWriteGreen = 1;
        st.get()->mDepthTest = 1;
        st.get()->mBlendEnable = 1;
        st.get()->mWriteAlpha = 0;
        player04bApplyRenderState(sp);
    }
    u32 buf = gabi::ea(&packet->mVertices);
    auto* vertices = gabi::at<player04b::VertexArray>(buf);
    u32 idxp = gabi::ea(&vertices->mCurrent);
    s32 n = (s32)packet->mPointCount;
    f32 step = (255.0f / (f32)(s32)((n >> 1) + 1)) / 255.0f;
    f32 prev = 0.0f;
    f32 cur = step;
    if (n >= 0) {
        u32 cnt = (u32)n + 1;
        for (s32 i = n; ; i--) {
            u32 vb = gabi::load<u32>(buf + ((u32)i * 2 + gabi::load<u32>(idxp)) * 0x254);
            u32 end = vb + 0x40;
            if (vb < end) {
                for (u32 q = vb; q < end; q += 0x20) { /* dcbz */
                    for (u32 b = 0; b < 0x20; b += 4) {
                        gabi::store<u32>((q & ~31u) + b, 0);
                    }
                }
            }
            vb = gabi::load<u32>(buf + ((u32)i * 2 + gabi::load<u32>(idxp)) * 0x254);
            u32 pa = gabi::ea(&packet->mTopPos[0]) + (u32)i * 0xC;
            u32 pb = gabi::ea(&packet->mTopPos[1]) + (u32)i * 0xC;
            u32 pc = gabi::ea(&packet->mBottomPos[0]) + (u32)i * 0xC;
            u32 pd = gabi::ea(&packet->mBottomPos[1]) + (u32)i * 0xC;
            f32 ay = gabi::load<f32>(pa + 4), ax = gabi::load<f32>(pa), az = gabi::load<f32>(pa + 8);
            gabi::store<f32>(vb + 0, ax);
            gabi::store<f32>(vb + 4, ay);
            gabi::store<f32>(vb + 0xC, cur);
            gabi::store<f32>(vb + 0x10, 0.0f);
            gabi::store<f32>(vb + 8, az);
            f32 bx = gabi::load<f32>(pb), by = gabi::load<f32>(pb + 4), bz = gabi::load<f32>(pb + 8);
            gabi::store<f32>(vb + 0x18, by);
            gabi::store<f32>(vb + 0x14, bx);
            gabi::store<f32>(vb + 0x1C, bz);
            gabi::store<f32>(vb + 0x24, 0.0f);
            gabi::store<f32>(vb + 0x20, prev);
            f32 cx = gabi::load<f32>(pc), cy = gabi::load<f32>(pc + 4), cz = gabi::load<f32>(pc + 8);
            gabi::store<f32>(vb + 0x2C, cy);
            gabi::store<f32>(vb + 0x30, cz);
            gabi::store<f32>(vb + 0x28, cx);
            gabi::store<f32>(vb + 0x34, cur);
            gabi::store<f32>(vb + 0x38, 1.0f);
            f32 dy = gabi::load<f32>(pd + 4), dx = gabi::load<f32>(pd), dz = gabi::load<f32>(pd + 8);
            gabi::store<f32>(vb + 0x3C, dx);
            gabi::store<f32>(vb + 0x48, prev);
            gabi::store<f32>(vb + 0x40, dy);
            prev = cur;
            gabi::store<f32>(vb + 0x4C, 1.0f);
            gabi::store<f32>(vb + 0x44, dz);
            cur = gabi::fadds_ppc(cur, step);
            if (--cnt == 0) {
                break;
            }
        }
    }
    {
        u32 c = buf + gabi::load<u32>(idxp) * 0x254 + 4;
        for (u32 k = 0; k < 0x3C; k++) {
            player04bFlushVertexBuffer(c + k * 0x4A8, 0, gabi::load<u32>(c + k * 0x4A8 + 0x14C));
        }
    }
    u32 idx = gabi::load<u32>(idxp) == 0 ? 1 : 0;
    gabi::store<u32>(idxp, idx);
    s32 m = (s32)packet->mPointCount;
    if (m >= 0) {
        u32 disp = gabi::ea(&packet->mDrawCall);
        u32 cnt = (u32)m + 1;
        for (s32 i = m; ; i--) {
            u32 other = idx == 0 ? 1 : 0;
            player04bBindAttributeBuffer(buf + ((u32)i * 2 + other) * 0x254 + 0x158);
            u32 nIdx = gabi::load<u32>(disp + 0xC);
            if (nIdx != 0) {
                GX2DrawIndexedEx(gabi::load<u32>(disp + 4), nIdx, gabi::load<u32>(disp), gabi::load<u32>(disp + 8), 0, 1);
            }
            if (--cnt == 0) {
                break;
            }
            idx = gabi::load<u32>(idxp);
        }
    }
}

// HD: set a shape's uniform blocks from the material's shader program (s16 slots -1 = unused)
static inline void lk_hdUniformBlocks(u32 shapeData, u32 mat) {
    u32 e = shapeData + 0x10 + (u32)gabi::at<player04b::ShapeResource>(shapeData)->mUniformBlockIndex * 0x1C;
    u32 t = (u32)gabi::at<player04b::DrawMaterial>(mat)->mUniformBlockCount != 0 ? (u32)gabi::at<player04b::DrawMaterial>(mat)->mpUniformBlocks : 0;
    s32 a = (s16)gabi::at<player04b::UniformBinding>(t)->mVertexLocation;
    u32 data = (u32)gabi::at<player04b::UniformBlock>(e)->mpData;
    s32 b = (s16)gabi::at<player04b::UniformBinding>(t)->mPixelLocation;
    u32 size = (u32)gabi::at<player04b::UniformBlock>(e)->mSize;
    s32 c = (s16)gabi::at<player04b::UniformBinding>(t)->mGeometryLocation;
    if (b != -1) {
        GX2SetPixelUniformBlock(b, size, data);
    }
    if (a != -1) {
        GX2SetVertexUniformBlock(a, size, data);
    }
    if (c != -1) {
        GX2SetGeometryUniformBlock(c, size, data);
    }
}
/* the sampler at smp takes the texture object over ( unless only the image changed) */
static inline void lk_hdSamplerSet(u32 smp, u32 tex) {
    auto* sampler = gabi::at<player04b::Sampler>(smp);
    auto* texture = gabi::at<player04b::Texture>(tex);
    constexpr be<u32> player04b::Texture::* descriptorFields[] = {
        &player04b::Texture::mDimension, &player04b::Texture::mWidth, &player04b::Texture::mHeight, &player04b::Texture::mDepth, &player04b::Texture::mMipCount, &player04b::Texture::mFormat, &player04b::Texture::mSwizzle, &player04b::Texture::mTileMode, &player04b::Texture::mAaMode
    };
    bool same = true;
    for (u32 k = 0; k < sizeof(descriptorFields) / sizeof(descriptorFields[0]); k++) {
        if ((u32)(sampler->mTexture.*descriptorFields[k]) != (u32)(texture->*descriptorFields[k])) {
            same = false;
            break;
        }
    }
    if (!same) {
        player04bAssignTexture(smp, tex);
    } else {
        u32 a = (u32)texture->mpImage;
        u32 b = (u32)texture->mpMipmaps;
        sampler->mpBoundImage = a;
        sampler->mpBoundMipmaps = b;
        sampler->mTexture.mpImage = a;
        sampler->mTexture.mpMipmaps = b;
    }
}
/* : daPy_sightPacket_c::draw (HD: GX2 rendering of the bow/hookshot sight: the "default_sight"
 * material, the lock-on texture (+0xAE8, red-orange with the alpha at +7 while locked (+5)) or the
 * normal one (+0xB78, black), the quad from the vertex buffer at +0x54 and the matrix at +0x14) */
/* 02414834 */
void player04b::daPy_sightPacket_c::draw() {
    u32 self = gabi::ea(this);
    auto* packet = this;
    u32 cache = player04bGetActiveShader(0x104B45C0u);
    lk_hdBindShader(cache, gabi::load<u32>((u32)packet->mpMaterial));
    {
        gabi::Local<player04b::RenderState> st;
        u32 sp = gabi::ea(st.get());
        player04bConstructRenderState(sp);
        st.get()->mDepthWrite = 0;
        st.get()->mWriteAlpha = 0;
        st.get()->mWriteBlue = 1;
        st.get()->mCullEnable = 0;
        st.get()->mWriteGreen = 1;
        st.get()->mDepthTest = 0;
        st.get()->mBlendEnable = 1;
        st.get()->mDepthFunc = 2;
        st.get()->mWriteRed = 1;
        player04bApplyRenderState(sp);
    }
    gabi::Local<Mtx34> mtx;
    gabi::Local<player04b::ProjectionMatrix> proj;
    lk_mtxCopy(mtx.get(), gabi::at<Mtx34>(0x104B45F8));
    PSMTXIdentity(mtx.get());
    u32 p = player04bGetCameraProjection(0x104B465Cu);
    player04bCopyProjection(p, proj.get());
    player04bSetMaterialView(gabi::ea(&packet->mShape), 0, mtx.get(), proj.get(), gabi::load<u32>(0x104B4708) + 0x240);
    player04bUpdateShape(gabi::ea(&packet->mShape), 0);
    lk_hdUniformBlocks((u32)packet->mShape.mpShapeData, (u32)packet->mpMaterial);
    u32 g, b, a;
    if ((u8)packet->mLockFlag != 0) { /* locked on */
        g = 0xFF;
        b = 0x32;
        a = (u8)packet->mLockAlpha;
        lk_hdSamplerSet(gabi::ea(&packet->mSampler), gabi::ea(&packet->mLockTexture));
    } else {
        g = 0;
        b = 0;
        a = 0xFF;
        lk_hdSamplerSet(gabi::ea(&packet->mSampler), gabi::ea(&packet->mSightTexture));
    }
    {
        u32 mat = (u32)packet->mpMaterial;
        u32 t = (u32)gabi::at<player04b::DrawMaterial>(mat)->mSamplerCount != 0 ? (u32)gabi::at<player04b::DrawMaterial>(mat)->mpSamplerBindings : 0;
        player04bBindTexture(gabi::ea(&packet->mSampler), t + 4, -1, 0);
    }
    u32 cb = gabi::ea(&packet->mShape.mUniformValues[0]); /* material constants: the colour (1, g, b, a) twice, then a fixed colour */
    gabi::store<f32>(cb + 0x44, (f32)g / 255.0f);
    gabi::store<u32>(cb + 0x40, 0x3F800000);
    gabi::store<u32>(cb + 0x50, gabi::load<u32>(cb + 0x40));
    gabi::store<u32>(cb + 0x54, gabi::load<u32>(cb + 0x44));
    gabi::store<f32>(cb + 0x48, (f32)b / 255.0f);
    gabi::store<u32>(cb + 0x58, gabi::load<u32>(cb + 0x48));
    gabi::store<f32>(cb + 0x4C, (f32)a / 255.0f);
    gabi::store<u32>(cb + 0x5C, gabi::load<u32>(cb + 0x4C));
    for (u32 k = 0; k < 4; k++) {
        gabi::store<u32>(cb + 0x20 + k * 4, gabi::load<u32>(0x104A01EC + k * 4));
    }
    player04bFlushMaterialUniforms(self + 0x5C8);
    gabi::call_ptr(gabi::load<u32>((u32)packet->mShape.mPixelMaterialVtable + 0x2C), self + 0x5C8, (u32)packet->mpMaterial);
    {
        gabi::Local<Mtx34> m;
        u32 t[12];
        for (u32 k = 0; k < 12; k++) {
            t[k] = lk_fbits(gabi::load<f32>(gabi::ea(&packet->mMtx) + k * 4));
        }
        for (u32 k = 0; k < 12; k++) {
            gmem_stf32(gabi::ea(m.get()) + k * 4, t[k]);
        }
        PSMTXCopy(m.get(), &packet->mShape.mModelMtx);
    }
    player04bFlushMaterialUniforms(self + 0x520);
    gabi::call_ptr(gabi::load<u32>((u32)packet->mShape.mVertexMaterialVtable + 0x2C), self + 0x520, (u32)packet->mpMaterial);
    player04bBindAttributeBuffer(gabi::ea(&packet->mVertices) + ((u32)packet->mVertices.mCurrent == 0 ? 1u : 0u) * 0x254 + 0x158);
    u32 n = (u32)packet->mDrawCall.mIndexCount;
    if (n != 0) {
        GX2DrawIndexedEx((u32)packet->mDrawCall.mPrimitiveType, n, gabi::load<u32>(gabi::ea(&packet->mDrawCall)),
                   (u32)packet->mDrawCall.mpIndices, 0, 1);
    }
}

/* HD: release a vertex buffer slot (0x254): its GX2 buffer object (+0x158) and, when allocated, the
 * memory from the graphics heap (+0x24C state, +0x250 pointer) */
static inline void lk_hdVtxFree(u32 slot) {
    auto* buffer = gabi::at<player04b::VertexSlot>(slot);
    player04bReleaseAttributeBuffer(slot + 0x158);
    u32 p = (u32)buffer->mpAllocation;
    gabi::store<u32>(slot, 0);
    if (p != 0) {
        u32 heap = player04bFindContainingHeap(gabi::load<u32>(0x101F8B4C), p); /* (GHS: the loaded pointer stays in r4) */
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(heap + 0xC) + 0x3C), heap, (u32)buffer->mpAllocation);
        buffer->mAllocationType = 0;
        buffer->mpAllocation = 0;
    }
}
// HD: the destructor of the double vertex buffer (two slots and the current index at +0x4B8)
static inline void lk_hdVtxBufPair_dt(u32 b, u32 elemDtor) {
    auto* vertices = gabi::at<player04b::VertexPair>(b);
    lk_hdVtxFree(b);
    lk_hdVtxFree(gabi::ea(&vertices->mSlots[1]));
    vertices->mpMaterial = 0;
    player04bDestroyArray(b, 2, 0x254, elemDtor, 0, 0);
}
// HD: the destructor of the render shape object (see lk_hdShape_ct)
static inline void lk_hdShape_dt(u32 sub) {
    auto* shape = gabi::at<player04b::Shape>(sub);
    for (s32 i = 0; i < gabi::load<s32>(sub); i++) {
        u32 arr = (u32)shape->mpShapeData;
        if ((u32)i < gabi::load<u32>(sub)) {
            arr += (u32)i * 0x23C;
        }
        for (u32 k = 0; k < 2; k++) {
            player04bReleaseUniformBuffer(arr + 0x10 + k * 0x1C);
        }
    }
    for (u32 k = 0; k < 2; k++) {
        player04bReleaseUniformBuffer(sub + 0x1C + k * 0x1C);
    }
    for (u32 k = 0; k < 2; k++) {
        player04bReleaseUniformBuffer(sub + 0xC4 + k * 0x1C);
    }
    player04bDestroyTextureSampler(sub + 0x43C, 2);
    player04bDestroyIndexBuffer(sub + 0x41C, 2);
    player04bDestroyUniformBlock(sub + 0xB4, 0);
    player04bDestroyUniformBlock(sub + 0xC, 0);
    player04bDestroyShaderParameters(sub, 2);
}
/* : daPy_lk_c::~daPy_lk_c (members destroyed in reverse order; HD: the sword blur is deleted
 * through its virtual destructor and freed from its named heap (); the sight packet and the HD
 * render members have their own GX2 resources) */
/* 02415384 */
static void daPy_lk_dt(void* i_self, s32 flags) {
    u32 self = gabi::ea(i_self);
    if (self == 0) {
        return;
    }
    auto* player = gabi::at<daPy_lk_c>(self);
    auto* items = player04b::itemModels(player);
    auto* packets = player04b::modelPackets(player);
    auto* background = player04b::backgroundChecks(player);
    player->__vtbl = 0x10037CF0; /* vtable */
    u32 blur = (u32)player->mpSwBlur;
    if (blur != 0) {
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(blur + 0xC) + 0xC), blur, 2);
        player->mpSwBlur = 0;
        player04bFreeHeapAllocation(blur);
    }
    player04bDestroyCollisionObjectInfo(gabi::ea(&player->mFanLightCps), 2);
    player04bDestroyCollisionSphere(gabi::ea(&player->mFanWindSph), 2);
    player04bDestroyCollisionObjectInfo(gabi::ea(&player->mFanWindCps), 2);
    player04bDestroyArray(gabi::ea(&player->mAtCps[0]), 3, 0x138, 0x02515980u, 0, 0);
    player04bDestroyCollisionCylinder(gabi::ea(&player->mLightCyl), 2);
    player04bDestroyCollisionCylinder(gabi::ea(&player->mAtCyl), 2);
    player04bDestroyCollisionCylinder(gabi::ea(&player->mWindCyl), 2);
    player04bDestroyCollisionCylinder(gabi::ea(&player->mCyl), 2);
    player04bDestroyCollisionStatus(gabi::ea(&player->mStts), 2);
    player04bDestroyArray(gabi::ea(&player->mFootData), 2, 0x118, 0x02444868u, 0, 0);
    player04bDestroyArray(gabi::ea(&player->m3460), 2, 0x10, 0x02444D38u, 0, 0);
    player04bDestroyArray(gabi::ea(&player->mDmEcallBack), 4, 0xC, 0x02444D4Cu, 0, 0);
    player04bDestroyArray(gabi::ea(&player->mSwimTailEcallBack), 2, 0x28, 0x02444D24u, 0, 0);
    player04bDestroyArray(gabi::ea(&player->mFootEffect), 2, 0x4C, 0x024448E0u, 0, 0);
    /* daPy_sightPacket_c::~daPy_sightPacket_c (inline) */
    {
        u32 sight = gabi::ea(&player->mSightPacket);
        u32 b = sight + 0x54;
        gabi::store<u32>(sight, 0x10037C18);
        lk_hdVtxFree(b);
        lk_hdVtxFree(b + 0x254);
        gabi::at<player04b::daPy_sightPacket_c>(sight)->mVertices.mpMaterial = 0;
        lk_hdShape_dt(sight + 0x514);
        if (b != 0) {
            lk_hdVtxBufPair_dt(b, 0x02444CC4u);
        }
        player04bDestroyDrawListBase(sight, 0);
    }
    player04bDestroyAnimationTransform(gabi::ea(&player04b::bckAnm(&items->mYuchw00Bck)->__vtbl), 0);
    player04bDestroyModelPacket(gabi::ea(&packets->mAfterArray[2]), 2);
    player04bDestroyModelPacket(gabi::ea(&packets->mAfterArray[1]), 2);
    player04bDestroyModelPacket(gabi::ea(&packets->mAfterArray[0]), 2);
    player04bDestroyArray(gabi::ea(packets->mArray), 2, 0xB0, 0x02082DDCu, 0, 0);
    player04bDestroyModelPacket(gabi::ea(&packets->mBeforeArray[5]), 2);
    player04bDestroyModelPacket(gabi::ea(&packets->mBeforeArray[4]), 2);
    player04bDestroyModelPacket(gabi::ea(&packets->mBeforeArray[3]), 2);
    player04bDestroyModelPacket(gabi::ea(&packets->mBeforeArray[2]), 2);
    player04bDestroyModelPacket(gabi::ea(&packets->mBeforeArray[1]), 2);
    player04bDestroyModelPacket(gabi::ea(&packets->mBeforeArray[0]), 2);
    player04bDestroyAnimationTransform(gabi::ea(&player04b::bckAnm(&items->mSwordAnim)->__vtbl), 0);
    player04b::mirrorLineCheck(player)->dBgS_PolyPassChk::__vtbl = 0x10034E04;
    player04b::mirrorLineCheck(player)->dBgS_GrpPassChk::__vtbl = 0x10034C24;
    player04b::mirrorLineCheck(player)->cBgS_PolyInfo::__vtbl = 0x10034BBC;
    player04bDestroyPolyCheck(gabi::ea(player04b::mirrorLineCheck(player)), 0);
    /* the HD render member at +0xF8C: its shape (+0x2FA0) and vertex buffers (+0x2AE0) */
    player04bDestroyUniformBlock(self + 0x3FE0, 0);
    player04bDestroyUniformBlock(self + 0x3F38, 0);
    player04bDestroyShaderParameters(self + 0x3F2C, 2);
    if (self + 0x3A6C != 0) {
        lk_hdVtxBufPair_dt(self + 0x3A6C, 0x024447E8u);
    }
    player04bDestroyTextureSampler(self + 0x38D0, 2);
    player04bDestroyTextureSampler(self + 0x3738, 2);
    player04bDestroyTextureSampler(self + 0x35A0, 2);
    player04bDestroyPacket(self + 0xF8C, 0);
    player04bDestroyAnimationTransform(gabi::ea(&player04b::bckAnm(&items->mAtngshaBck)->__vtbl), 0);
    player04bDestroyAnimationTransform(gabi::ea(&player04b::bckAnm(&player->mSwgripmsabBckAnim)->__vtbl), 0);
    /* the ground/line check members (dBgS_*Chk destructors: this TU's vtables, then the base) */
    background->mLavaGndChk.dBgS_GrpPassChk::__vtbl = 0x10034C24;
    background->mLavaGndChk.cBgS_PolyInfo::__vtbl = 0x10034C44;
    background->mLavaGndChk.dBgS_PolyPassChk::__vtbl = 0x10034C64;
    player04bDestroyBackgroundCheck(gabi::ea(&background->mLavaGndChk), 0);
    background->mMirLightLinChk.dBgS_PolyPassChk::__vtbl = 0x10034E04;
    background->mMirLightLinChk.cBgS_PolyInfo::__vtbl = 0x10034BBC;
    background->mMirLightLinChk.dBgS_GrpPassChk::__vtbl = 0x10034C24;
    player04bDestroyPolyCheck(gabi::ea(&background->mMirLightLinChk), 0);
    background->mArrowLinChk.cBgS_PolyInfo::__vtbl = 0x10034BBC;
    background->mArrowLinChk.dBgS_GrpPassChk::__vtbl = 0x10034C24;
    background->mArrowLinChk.dBgS_PolyPassChk::__vtbl = 0x10034E04;
    player04bDestroyPolyCheck(gabi::ea(&background->mArrowLinChk), 0);
    background->mRoofChk.dBgS_GrpPassChk::__vtbl = 0x10034C24;
    background->mRoofChk.cBgS_Chk::__vtbl = 0x10034D34;
    background->mRoofChk.dBgS_PolyPassChk::__vtbl = 0x10034D54;
    player04bDestroyPolyCheck(gabi::ea(&background->mRoofChk.cBgS_Chk::mPolyPassChk), 0);
    background->mGndChk.dBgS_GrpPassChk::__vtbl = 0x10034C24;
    background->mGndChk.dBgS_PolyPassChk::__vtbl = 0x10034C64;
    background->mGndChk.cBgS_PolyInfo::__vtbl = 0x10034C44;
    player04bDestroyBackgroundCheck(gabi::ea(&background->mGndChk), 0);
    background->mBoomerangLinChk.cBgS_PolyInfo::__vtbl = 0x10034BBC;
    background->mBoomerangLinChk.dBgS_PolyPassChk::__vtbl = 0x10034E04;
    background->mBoomerangLinChk.dBgS_GrpPassChk::__vtbl = 0x10034C24;
    player04bDestroyPolyCheck(gabi::ea(&background->mBoomerangLinChk), 0);
    background->mRopeLinChk.dBgS_PolyPassChk::__vtbl = 0x10034E04;
    background->mRopeLinChk.dBgS_GrpPassChk::__vtbl = 0x10034C24;
    background->mRopeLinChk.cBgS_PolyInfo::__vtbl = 0x10034BBC;
    player04bDestroyPolyCheck(gabi::ea(&background->mRopeLinChk), 0);
    background->mLinkLinChk.cBgS_PolyInfo::__vtbl = 0x10034BBC;
    background->mLinkLinChk.dBgS_PolyPassChk::__vtbl = 0x10034E04;
    background->mLinkLinChk.dBgS_GrpPassChk::__vtbl = 0x10034C24;
    player04bDestroyPolyCheck(gabi::ea(&background->mLinkLinChk), 0);
    player->mAcch.dBgS_GrpPassChk::__vtbl = 0x10034DB4;
    player->mAcch.dBgS_PolyPassChk::__vtbl = 0x10034DC4;
    player04bDestroyAcch(gabi::ea(&player->mAcch), 0);
    player04bDestroyArray(gabi::ea(&player->mAcchCir), 3, 0x40, 0x02444794u, 0, 0);
    player04bDestroyPlayerBase(self, 0);
    if (flags & 1) {
        operator_delete(gabi::at<void>(self));
    }
}

/* HD: a SafeString literal compared with a string (SafeString ==, the literal made terminated by
 * ) */
static inline bool lk_ssEq(u32 lit, u32 str) {
    gabi::Local<player04b::SafeString> a;
    gabi::Local<player04b::SafeString> b;
    a->__vtbl = LK_SAFESTRING_VTBL;
    a->mpString = lit;
    b->__vtbl = LK_SAFESTRING_VTBL;
    b->mpString = str;
    player04bTerminateSafeString(a.get());
    lk_ss_assure(a.get());
    u32 pa = a->mpString;
    lk_ss_assure(b.get());
    u32 pb = b->mpString;
    if (pa == pb) {
        return true;
    }
    pa = a->mpString;
    pb = b->mpString;
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::load<u8>(pa + n);
        u8 cb = gabi::load<u8>(pb + n);
        if (ca != cb) {
            return false;
        }
        if (ca == 0) {
            return true;
        }
    }
    return false;
}
/* a material's name (J3DMaterial +0: name table entry, relative offset at +4) */
static inline u32 lk_mtlName(u32 mtl) {
    u32 nm = gabi::at<player04b::MaterialNode>(mtl)->mpName;
    u32 off = gabi::at<player04b::NameEntry>(nm)->mNameOffset;
    return off != 0 ? nm + 4 + off : 0;
}
static inline bool lk_mtlNameIs4(u32 mtl, u32 l0, u32 l1, u32 l2, u32 l3) {
    return lk_ssEq(l0, lk_mtlName(mtl)) || lk_ssEq(l1, lk_mtlName(mtl)) || lk_ssEq(l2, lk_mtlName(mtl)) || lk_ssEq(l3, lk_mtlName(mtl));
}
/* J3DModelData joint node j (0x1C each; the first one when out of range) */
static inline u32 lk_jointNode(u32 md, u32 j) {
    u32 n = gabi::at<J3DModelData>(md)->mJointNum;
    u32 base = gabi::at<J3DModelData>(md)->mJointNodes;
    return j < n ? base + j * 0x1C : base;
}
// HD: ResTIMG assignment is nine ordered integer words, including the renderer pointer.
static inline void lk_copy24(u32 dst, u32 src) {
    for (u32 k = 0; k < 0x24; k += 4) {
        gabi::store<u32>(dst + k, gabi::load<u32>(src + k));
    }
}
/* dEvent_exception_c::setStartDemo, then the rest of the init (HD: start mode 0xA also sets
 * daPyFlg0 ) */
// HD: explicit load/store order follows GHS; the two light fields retain the HD ordering.
static inline void lk_playerInitTail(u32 self, u32 demo, u32 extraFlg) {
    auto* player = gabi::at<daPy_lk_c>(self);
    u32 event = player04bSetStartDemo(dComIfGp_ea() + 0x52E8, demo);
    u32 flags = player->mNoResetFlg0;
    player->mEventIdx = event;
    player->mNoResetFlg0 = flags | extraFlg | 0x100;
    player->mDemo.mStick = 1.0f;
    auto& lava = player04b::backgroundChecks(player)->mLavaGndChk;
    lava.dBgS_GrpPassChk::mGrp = lava.dBgS_GrpPassChk::mGrp & ~2u;
    player->endDamageEmitter();
    player04bInitMirrorPacket(gabi::ea(player04b::mirrorPacket(player)), 0);
    auto* light = gabi::at<player04b::LightInfluence>(gabi::ea(player->mLightInfluence));
    light->mColor.r = 600;
    light->mColor.g = 400;
    light->mColor.b = 120;
    light->mFluctuation = 250.0f;
    // HD: GHS assigns the position through integer words, preserving signaling-NaN payloads.
    for (u32 k = 0; k < 3; k++) {
        gabi::at<be<u32>>(gabi::ea(&light->mPos))[k] = gabi::at<be<u32>>(gabi::ea(&player->current.pos))[k];
    }
    light->mPower = 0.0f;
    player04bRegisterPointLight(gabi::ea(light));
    player->m352E = 900;
    player->m34DE = player->shape_angle.y;
    player->mFace = 0xAD;
}

/* : daPy_lk_c::playerInit (HD: the eye/eyebrow shapes are sorted by material name instead of
 * z-mode; the hands model's joints are hidden and their matrices zeroed; no item heap dummies or blur
 * texture; every "linktexS3TC" texture is replaced by the casual one, the originals kept in the
 * object at +0x458 (copies in +0x48C) and swapped back for the hero's clothes) */
/* 02410BE8 */
void daPy_lk_c::playerInit() {
    auto* items = player04b::itemModels(this);
    u32 b = gabi::ea(this);
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)&player04bCreateHeap), 0)) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5F3B, "0");
    }
    {
        u32 m = mpCLModel.v;
        cullMtx = gabi::ea(gabi::at<J3DModel>(m)->getBaseTRMtx()); /* fopAcM_SetMtx */
        gabi::at<J3DModel>(m)->mUserArea = b; /* setUserArea */
        gabi::at<J3DModel>(items->mpYmgcs00Model)->mUserArea = b;
    }
    for (u16 j = 0; j < gabi::at<J3DModel>(items->mpYmgcs00Model)->getModelData()->getJointNum(); j = (u16)(j + 1)) {
        u32 node = lk_jointNode(gabi::ea(gabi::at<J3DModel>(items->mpYmgcs00Model)->getModelData()), j);
        gabi::at<J3DJoint>(node)->mCallBack = 0x023D6980u; /* daPy_auraCallback */
    }
    {
        constexpr u8 jnts[] = {2, 14, 5, 9, 13, 29, 15, 1};
        for (u32 k = 0; k < sizeof(jnts); k++) {
            gabi::at<J3DJoint>(lk_jointNode(mpCLModelData.v, jnts[k]))->mCallBack = 0x023D80F0u; /* daPy_jointCallback0 */
        }
    }
    for (u32 k = 0; k < 2; k++) { /* m_pbCalc */
        u32 c = (u32)m_pbCalc[k];
        gabi::at<player04b::MatrixCalculator>(c)->mUserArea = b;
        gabi::at<player04b::MatrixCalculator>((u32)m_pbCalc[k])->mJointBeforeCallback = 0x023D7194u; /* daPy_jointBeforeCallback */
        gabi::at<player04b::MatrixCalculator>((u32)m_pbCalc[k])->mJointAfterCallback = 0x023D78E0u; /* daPy_jointAfterCallback */
    }
    s32 zon = 0, zoffNone = 0, zoffBlend = 0;
    {
        u32 mtl = (u32)gabi::at<player04b::Joint>(lk_jointNode(mpCLModelData.v, 0x13 /* CL_JNT_CL_EYE_e */))->mMesh;
        for (u32 pass = 0; pass < 2; pass++) {
            while (mtl != 0) {
                if (lk_mtlNameIs4(mtl, gabi::guest_addr("eyeLdamA"), gabi::guest_addr("eyeRdamA"), gabi::guest_addr("mayuLdamA"), gabi::guest_addr("mayuRdamA"))) {
                    mpZOnShape[zon] = gabi::ea(gabi::at<player04b::MaterialNode>(mtl)->mShape.get()); /* mpZOnShape */
                    zon++;
                    if (zon > 4) {
                        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5F96, "zon_cnt <= 4");
                    }
                } else if (lk_mtlNameIs4(mtl, gabi::guest_addr("eyeLdamB"), gabi::guest_addr("eyeRdamB"), gabi::guest_addr("mayuLdamB"), gabi::guest_addr("mayuRdamB"))) {
                    mpZOffNoneShape[zoffNone] = gabi::ea(gabi::at<player04b::MaterialNode>(mtl)->mShape.get()); /* mpZOffNoneShape */
                    zoffNone++;
                    if (zoffNone > 4) {
                        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5F9E, "zoff_none_cnt <= 4");
                    }
                } else if (lk_mtlNameIs4(mtl, gabi::guest_addr("eyeL"), gabi::guest_addr("eyeR"), gabi::guest_addr("mayuL"), gabi::guest_addr("mayuR"))) {
                    mpZOffBlendShape[zoffBlend] = gabi::ea(gabi::at<player04b::MaterialNode>(mtl)->mShape.get()); /* mpZOffBlendShape */
                    zoffBlend++;
                    if (zoffBlend > 4) {
                        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5FA6, "zoff_blend_cnt <= 4");
                    }
                }
                mtl = (u32)gabi::at<player04b::MaterialNode>(mtl)->mNext; /* getNext() */
            }
            mtl = (u32)gabi::at<player04b::Joint>(lk_jointNode(mpCLModelData.v, 0x15 /* CL_JNT_CL_MAYU_e */))->mMesh;
        }
    }
    if (zon != 4) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5FAF, "zon_cnt == 4");
    }
    if (zoffNone != 4) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5FB0, "zoff_none_cnt == 4");
    }
    if (zoffBlend != 4) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x5FB1, "zoff_blend_cnt == 4");
    }
    mpLhandShape = gabi::ea(gabi::at<player04b::MaterialNode>(gabi::at<player04b::Joint>(lk_jointNode(mpCLModelData.v, 8))->mMesh)->mShape.get()); /* mpLhandShape */
    mpRhandShape = gabi::ea(gabi::at<player04b::MaterialNode>(gabi::at<player04b::Joint>(lk_jointNode(mpCLModelData.v, 12))->mMesh)->mShape.get()); /* mpRhandShape */
    {
        u32 hmd = gabi::ea(mpHandsModel->getModelData()); /* mpHandsModel->getModelData() */
        gabi::Local<Mtx34> zero;
        u32 z = gabi::ea(zero.get());
        for (u32 j = 1; j <= 10; j++) { /* HANDS_JNT_CL_LHANDB_e .. CL_RHANDE: hidden, matrices zeroed */
            u32 node = lk_jointNode(hmd, j);
            gabi::at<player04b::MaterialNode>(gabi::at<player04b::Joint>(node)->mMesh)->mShape->hide();
            for (u32 k = 0; k < 12; k++) {
                gabi::store<u32>(z + k * 4, 0);
            }
            u32 m = gabi::ea(lk_getAnmMtx(mpHandsModel.get(), (s32)j));
            for (u32 k = 0; k < 12; k++) {
                gabi::store<f32>(m + k * 4, gabi::load<f32>(z + k * 4));
            }
        }
    }
    {
        u32 buf = gabi::at<player04b::AnmHeap>(gabi::ea(m_anm_heap_under))->m_buffer; /* m_anm_heap_under[UNDER_MOVE0_e].m_buffer */
        gabi::at<player04b::AnmHeap>(gabi::ea(m_anm_heap_under + 1 * sizeof(player04b::AnmHeap)))->m_buffer = buf + 0x2400;
        gabi::at<player04b::AnmHeap>(gabi::ea(m_anm_heap_upper))->m_buffer = buf + 0x4800;
        for (u32 i = 1; i <= 2; i++) {
            gabi::at<player04b::AnmHeap>(gabi::ea(m_anm_heap_upper))[i].m_buffer = gabi::at<player04b::AnmHeap>(gabi::ea(m_anm_heap_upper))->m_buffer + i * 0x2400;
        }
    }
    mAcch.Set(&current.pos, &old.pos, this, 3, &mAcchCir[0], &speed, &current.angle, &shape_angle);
    mAcch.m_flags = ((mAcch.m_flags & ~0x400u) | 0x2000) & ~8u; /* ClrWaterNone, OnLineCheck, ClrRoofNone */
    mMaxNormalSpeed = 17.0f;   /* mMaxNormalSpeed */
    maxFallSpeed = -175.0f; /* maxFallSpeed */
    gravity = -2.5f;   /* gravity */
    mAcch.m_wtr_check_offset = 500.0f;  /* SetWaterCheckOffset */
    mAcch.m_roof_crr_height = 125.0f;  /* SetRoofCrrHeight */
    mAcchCir[0].SetWall(30.1f, 35.0f);
    mAcchCir[1].SetWall(89.9f, 35.0f);
    mAcchCir[2].SetWall(125.0f, 35.0f);
    mHeight = 125.0f; /* mHeight */
    mKeepItem = 0x100; /* mKeepItem = daPyItem_NONE_e */
    mHeadTopPos.z = 0.0f;   /* mHeadTopPos */
    mEquipItem = 0x100; /* mEquipItem */
    mHeadTopPos.y = 0.0f;
    mHeadTopPos.x = 0.0f;
    mCameraInfoIdx = dComIfGp_getPlayerCameraID(0); /* mCameraInfoIdx */
    {
        u32 att = dComIfGp_ea() + 0x5804;
        u32 stts = gabi::ea(&mStts);
        mpAttention = att; /* mpAttention */
        mStts.Init(0x78, 0xFF, this);
        mCyl.Set(&player04b::l_cyl_src());
        s16 by = mBodyAngle.y;
        mCyl.mGObjTg.mpShieldFrontRangeYAngle = b + 0x6996; /* SetTgShieldFrontRangeYAngle */
        u32 t = mCyl.mGObjTg.mSPrm;
        s16 sy = shape_angle.y;
        mCyl.mGObjTg.mSPrm = t | 8; /* OnTgShieldFrontRange */
        mCyl.mStts.v = stts;
        mShieldFrontRangeYAngle = (s16)(sy + by); /* mShieldFrontRangeYAngle */
        mWindCyl.Set(&player04b::l_wind_cyl_src());
        mWindCyl.mStts.v = stts;
        mLightCyl.Set(&player04b::l_wind_cyl_src());
        mLightCyl.mStts.v = stts;
        mLightCyl.mTgType = 0x00800000; /* SetTgType(AT_TYPE_LIGHT) */
        for (u32 i = 0; i < 3; i++) {
            player04bSetCapsule(gabi::ea(&mAtCps[i]), gabi::ea(player04b::l_at_cps_src()));
            mAtCps[i].mStts.v = stts;
        }
        mAtCyl.Set(&player04b::l_at_cyl_src());
        mAtCyl.mStts.v = stts;
        player04bSetCapsule(gabi::ea(&mFanWindCps), gabi::ea(player04b::l_fan_wind_cps_src()));
        mFanWindCps.mStts.v = stts;
        gabi::at<player04b::CapsuleGeometry>(gabi::ea(mFanWindCps.mCps))->mRadius = 70.0f;
        mFanWindSph.Set(&player04b::l_fan_wind_sph_src());
        mFanWindSph.mStts.v = stts;
        player04bSetCapsule(gabi::ea(&mFanLightCps), gabi::ea(player04b::l_fan_wind_cps_src()));
        mFanLightCps.mAtType = 0x00800000; /* SetAtType(AT_TYPE_LIGHT) */
        mFanLightCps.mStts.v = stts;
        gabi::at<player04b::CapsuleGeometry>(gabi::ea(mFanLightCps.mCps))->mRadius = 20.0f;
    }
    for (u32 i = 0; i < 2; i++) { /* m_anm_heap_under */
        auto* heap = &gabi::at<player04b::AnmHeap>(gabi::ea(m_anm_heap_under))[i];
        createAnimeHeap(gabi::at<JKRSolidHeap*>(gabi::ea(&heap->mpAnimeHeap)), player04b::HEAP_TYPE_UNDER_UPPER_e);
        heap->mIdx = -1;
        heap->field_0x2 = -1;
        heap->field_0x4 = -1;
    }
    for (u32 i = 0; i < 3; i++) { /* m_anm_heap_upper */
        auto* heap = &gabi::at<player04b::AnmHeap>(gabi::ea(m_anm_heap_upper))[i];
        createAnimeHeap(gabi::at<JKRSolidHeap*>(gabi::ea(&heap->mpAnimeHeap)), player04b::HEAP_TYPE_UNDER_UPPER_e);
        heap->mIdx = -1;
        heap->field_0x2 = -1;
        heap->field_0x4 = -1;
    }
    createAnimeHeap(gabi::at<JKRSolidHeap*>(gabi::ea(&gabi::at<player04b::AnmHeap>(gabi::ea(m_tex_anm_heap))->mpAnimeHeap)), player04b::HEAP_TYPE_TEXTURE_ANIME_e); /* m_tex_anm_heap */
    gabi::at<player04b::AnmHeap>(gabi::ea(m_tex_anm_heap))->field_0x6 = -1;
    gabi::at<player04b::AnmHeap>(gabi::ea(m_tex_anm_heap))->field_0x4 = -1;
    gabi::at<player04b::AnmHeap>(gabi::ea(m_tex_anm_heap))->mIdx = -1;
    gabi::at<player04b::AnmHeap>(gabi::ea(m_tex_anm_heap))->field_0x2 = -1;
    createAnimeHeap(gabi::at<JKRSolidHeap*>(gabi::ea(&gabi::at<player04b::AnmHeap>(gabi::ea(m_tex_scroll_heap))->mpAnimeHeap)), player04b::HEAP_TYPE_TEXTURE_SCROLL_e); /* m_tex_scroll_heap */
    gabi::at<player04b::AnmHeap>(gabi::ea(m_tex_scroll_heap))->field_0x2 = -1;
    gabi::at<player04b::AnmHeap>(gabi::ea(m_tex_scroll_heap))->mIdx = -1;
    m34F0 = -1; /* mSeAnmIdx */
    mpSeAnm = gabi::guest_addr(""); /* (HD SafeString member: "") */
    gabi::at<player04b::AnmHeap>(gabi::ea(m_tex_scroll_heap))->field_0x4 = -1;
    gabi::at<player04b::AnmHeap>(gabi::ea(m_tex_scroll_heap))->field_0x6 = -1;
    for (u32 i = 0; i < 2; i++) { /* mpItemHeaps (HD: named, no dummy allocation) */
        u32 h = gabi::ea(mDoExt_createSolidHeapFromGameToCurrent(0x1E600, 0x20));
        items->mpItemHeaps[i] = h;
        if (h == 0) {
            JUT_ASSERT_fail("d_a_player_main.cpp", i == 0 ? 0x605A : 0x6064, i == 0 ? "m_item_heap[0] != (0)" : "m_item_heap[1] != (0)");
            h = items->mpItemHeaps[i];
        }
        gabi::at<player04b::NamedHeap>(h)->mpName = i == 0 ? gabi::guest_addr("\203A\203C\203e\203\200\227p\227\314\210\346[0]") : gabi::guest_addr("\203A\203C\203e\203\200\227p\227\314\210\346[1]"); /* heap name */
        mDoExt_restoreCurrentHeap();
    }
    createAnimeHeap(gabi::at<JKRSolidHeap*>(gabi::ea(&items->mpItemAnimeHeap)), 3); /* mpItemAnimeHeap */
    player04bClearActorKeep(gabi::ea(&mActorKeepEquip));
    player04bClearActorKeep(gabi::ea(&mActorKeepThrow));
    player04bClearActorKeep(gabi::ea(&mActorKeepGrab));
    player04bClearActorKeep(gabi::ea(&mActorKeepRope));
    mWhirlId = -1; /* mWhirlId */
    current.angle.z = 0;   /* current.angle.z */
    shape_angle.z = 0;   /* shape_angle.z */
    m3630 = -1;
    m3526 = 8;  /* m3526 */
    mGameOverId = -1;
    mTactZevPartnerId = -1;
    gabi::at<Quaternion>(gabi::ea(m3648))->w = 1.0f; /* m3648.w */
    resetSeAnime();
    {
        gabi::Local<player04b::SafeString> arc;
        arc->__vtbl = LK_SAFESTRING_VTBL;
        arc->mpString = gabi::guest_addr("Always"); /* "Always" */
        u32 tex = player04bGetResource(gabi::load<u32>(0x101F4F28), arc.get(), 0x71);
        if (tex == 0) {
            JUT_ASSERT_fail("d_a_player_main.cpp", 0x6087, "tmp_tex != (0)");
        }
        gabi::at<player04b::daPy_sightPacket_c>(gabi::ea(mSightPacket))->mpSightTex = tex; /* mSightPacket.setSightTex */
    }
    {
        gabi::Local<player04b::SafeString> arc;
        arc->__vtbl = LK_SAFESTRING_VTBL;
        arc->mpString = gabi::guest_addr("Link"); /* l_arcName "Link" */
        u32 img = player04bGetResource(gabi::load<u32>(0x101F4F28), arc.get(), 0x72);
        if (img == 0) {
            JUT_ASSERT_fail("d_a_player_main.cpp", 0x608D, "tmp_img != (0)");
        }
        u32 off = (u32)gabi::at<player04b::TextureHeader>(img)->imageOffset;
        gabi::at<player04b::daPy_sightPacket_c>(gabi::ea(mSightPacket))->mpImg = img;       /* mSightPacket.setImage */
        gabi::at<player04b::daPy_sightPacket_c>(gabi::ea(mSightPacket))->mpLockTex = img + off; /* setLockTex */
    }
    {
        u32 startMode = (mParameters >> 12) & 0xF;
        u32 prm = mParameters;
        u8 lastMode = gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1177);
        if (startMode == 0xE) {
            lk_playerInitTail(b, 0xD4, 0);
        } else if (startMode == 0xC) {
            lk_playerInitTail(b, 0xD0, 0);
        } else if (startMode == 6) {
            lk_playerInitTail(b, 0xCF, 0);
        } else if (startMode == 0xA) {
            lk_playerInitTail(b, 0xD2, 0x08000000);
        } else if (startMode == 0xB) {
            lk_playerInitTail(b, 0xD3, 0);
        } else if (startMode == 9 || lastMode == 6) {
            lk_playerInitTail(b, 0xCB, 0);
        } else if (startMode == 2 && (lastMode == 4 || lastMode == 5)) {
            lk_playerInitTail(b, 0xC9, 0);
        } else if (startMode == 0xF) {
            lk_playerInitTail(b, 0xD5, 0);
        } else {
            lk_playerInitTail(b, prm >> 24 /* getStartEvent() */, 0);
        }
    }
    /* the Link texture: casual clothes */
    u32 casual;
    {
        gabi::Local<player04b::SafeString> arc;
        arc->__vtbl = LK_SAFESTRING_VTBL;
        arc->mpString = gabi::guest_addr("Link");
        casual = player04bGetResource(gabi::load<u32>(0x101F4F28), arc.get(), 0x71);
    }
    mpCurrLinktex = gabi::ea(m458);
    u32 texture = (u32)gabi::at<player04b::ModelData>(mpCLModelData.v)->mTexture; /* getTexture() */
    if (texture == 0) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x60DA, "texture != (0)");
    }
    u32 names = (u32)gabi::at<player04b::ModelData>(mpCLModelData.v)->mTextureName; /* getTextureName() */
    if (names == 0) {
        JUT_ASSERT_fail("d_a_player_main.cpp", 0x60DC, "textureName != (0)");
    }
    for (u16 i = 0; i < (u16)gabi::at<player04b::TextureTable>(texture)->mNum; i = (u16)(i + 1)) {
        u32 nm = player04bGetName(names, (u32)i);
        if (nm == 0) {
            continue;
        }
        bool same;
        {
            u32 k = 0;
            u8 c1, c2;
            do {
                c1 = gabi::load<u8>(nm + k);
                c2 = gabi::load<u8>(gabi::guest_addr("linktexS3TC") + k);
                k++;
            } while (c1 == c2 && c1 != 0);
            same = c1 == c2;
        }
        if (!same) {
            continue;
        }
        u32 res = (u32)gabi::at<player04b::TextureTable>(texture)->mpRes + (u32)i * 0x24;
        {
            s32 cnt = (s32)gabi::at<player04b::TextureOriginals>(gabi::ea(m458))->mCount;
            if (cnt < (s32)gabi::at<player04b::TextureOriginals>(gabi::ea(m458))->mCapacity) { /* the originals list: push_back */
                gabi::store<u32>((u32)gabi::at<player04b::TextureOriginals>(gabi::ea(m458))->mpHeaders + (u32)cnt * 4, res);
                cnt = (s32)gabi::at<player04b::TextureOriginals>(gabi::ea(m458))->mCount + 1;
                gabi::at<player04b::TextureOriginals>(gabi::ea(m458))->mCount = cnt;
            }
            u32 last = (u32)cnt - 1;
            u32 v = mpCurrLinktex;
            u32 n = gabi::load<u32>(v);
            u32 dst = gabi::ea(m48C) + (last < 10 ? last * 0x24 : 0);
            u32 src = last < n ? gabi::load<u32>(gabi::load<u32>(v + 8) + last * 4) : 0;
            lk_copy24(dst, src); /* keep a copy of the original header */
        }
        lk_copy24((u32)gabi::at<player04b::TextureTable>(texture)->mpRes + (u32)i * 0x24, casual); /* texture->setResTIMG(i, *casual) */
        {
            u32 r = (u32)gabi::at<player04b::TextureTable>(texture)->mpRes + (u32)i * 0x24;
            gabi::at<player04b::TextureHeader>(r)->imageOffset = (u32)gabi::at<player04b::TextureHeader>(r)->imageOffset + casual - r;
            r = (u32)gabi::at<player04b::TextureTable>(texture)->mpRes + (u32)i * 0x24;
            gabi::at<player04b::TextureHeader>(r)->paletteOffset = (u32)gabi::at<player04b::TextureHeader>(r)->paletteOffset + casual - r;
            r = (u32)gabi::at<player04b::TextureTable>(texture)->mpRes + (u32)i * 0x24;
            gabi::at<player04b::TextureHeader>(r)->mHDTexture = (u32)gabi::at<player04b::TextureHeader>(casual)->mHDTexture;
        }
        {
            u32 hi = (u32)gabi::at<player04b::TextureTable>(texture)->mDirtyMaskSeed[0], lo = (u32)gabi::at<player04b::TextureTable>(texture)->mDirtyMaskSeed[1];
            auto slw = [](u32 v, u32 s) -> u32 { s &= 0x3F; return s >= 32 ? 0 : v << s; };
            auto srw = [](u32 v, u32 s) -> u32 { s &= 0x3F; return s >= 32 ? 0 : v >> s; };
            if (i < 0x40) {
                u32 sh = i;
                u32 h = slw(hi, sh) | srw(lo, 32 - sh) | slw(lo, sh + 32);
                gabi::at<player04b::TextureTable>(texture)->mDirtyTexMask[0] = (u32)gabi::at<player04b::TextureTable>(texture)->mDirtyTexMask[0] | h;
                gabi::at<player04b::TextureTable>(texture)->mDirtyTexMask[1] = (u32)gabi::at<player04b::TextureTable>(texture)->mDirtyTexMask[1] | slw(lo, sh);
            } else {
                u32 sh = i - 0x40;
                u32 w10 = (u32)gabi::at<player04b::TextureTable>(texture)->mDirtyTexMask[2];
                u32 h = slw(hi, sh) | srw(lo, 32 - sh) | slw(lo, sh + 32);
                u32 w14 = (u32)gabi::at<player04b::TextureTable>(texture)->mDirtyTexMask[3] | slw(lo, sh);
                gabi::at<player04b::TextureTable>(texture)->mDirtyTexMask[3] = w14;
                gabi::at<player04b::TextureTable>(texture)->mDirtyTexMask[2] = w10 | h;
            }
        }
    }
    {
        u32 save = gabi::load<u32>(0x101F84DC);
        if (!gabi::at<dSv_event_c>(save + 0x644)->isEventBit(0x2A80) || gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1C0) != 0) {
            mNoResetFlg1 = mNoResetFlg1 | daPyFlg1_CASUAL_CLOTHES;
        }
    }
    if (!(mNoResetFlg1 & daPyFlg1_CASUAL_CLOTHES)) {
        /* the hero's clothes: swap the original headers back */
        struct Tmp {
            u8 b[0x168];
        };
        gabi::Local<Tmp> tmp;
        u32 t = gabi::ea(tmp.get());
        u32 v = mpCurrLinktex;
        player04bConstructArray(t, 0xA, 0x24, 0x02444280u);
        for (u32 k = 0; k < 10; k++) {
            u32 src = k < gabi::load<u32>(v) ? gabi::load<u32>(gabi::load<u32>(v + 8) + k * 4) : 0;
            lk_copy24(t + k * 0x24, src);
        }
        player04bCopyTextureList((u32)mpCurrLinktex, gabi::ea(m48C));
        for (u32 k = 0; k < 10; k++) {
            lk_copy24(gabi::ea(m48C) + k * 0x24, t + k * 0x24);
        }
    }
    player04b::actorState(this).field_0x8282 = 0;
    player04b::actorState(this).mBottleThrowPending = 0;
    player04b::actorState(this).field_0x8280 = 0;
    player04b::actorState(this).field_0x827C = 0;
    player04b::actorState(this).mThrowActor = 0;
    player04b::actorState(this).mTingleDemoOrdered = 0;
    {
        u32 play = dComIfGp_ea();
        gabi::store<u32>(play + 0x5CDC, gabi::load<u32>(play + 0x5CDC) & ~0x100000u);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
ext 023EAA80 u32 player04bGetReadyItemRegister(daPy_lk_c*)
ext 02506964 BOOL player04bCalculateSubjectAngle(u32, be<s16>*, be<s16>*)
ext 02416C78 u32 player04bCheckBodyAngleRegister(daPy_lk_c*, s32)
ext 028E8DE8 f32 player04bSquareDistance(cXyz*, cXyz*)
str 10035F50 "MajyuE" in 024198D4
str 10035F4C "sea" in 024198D4
str 10035F30 "GanonK" in 02417D30
ext 023E4CD0 int daPy_lk_c::changeSlideProc()
ext 023E32AC BOOL daPy_lk_c::changeWaitProc()
ext 023F0CE4 BOOL daPy_lk_c::checkAtnWaitAnime()
ext 023D6A18 BOOL daPy_lk_c::checkBowAnime()
ext 023EB510 BOOL daPy_lk_c::checkNextActionBoomerangReady()
ext 023EB208 BOOL daPy_lk_c::checkNextActionBowReady()
ext 023F14E0 BOOL daPy_lk_c::checkNextMode(int)
ext 023DD12C BOOL daPy_lk_c::checkRestHPAnime()
ext 023E4A58 u32 daPy_lk_c::getSlidePolygon()
ext 023EAB40 BOOL daPy_lk_c::itemTrigger()
ext 023EE510 BOOL daPy_lk_c::procCrouchDefense_init()
ext 023EE2B8 BOOL daPy_lk_c::procCrouch_init()
ext 023DC6A4 BOOL daPy_lk_c::resetActAnimeUpper(int, f32)
ext 023DE7E8 BOOL daPy_lk_c::setActAnimeUpper(u32, int, f32, f32, int, f32)
ext 023ECEF0 void daPy_lk_c::setAnimeUnequip()
ext 023E7E5C void daPy_lk_c::setBlendAtnBackMoveAnime(f32)
ext 023E81E4 void daPy_lk_c::setBlendAtnMoveAnime(f32)
ext 023E7BEC void daPy_lk_c::setBowReadyAnime()
ext 023E3850 void daPy_lk_c::setFrontWallType()
ext 023E278C void daPy_lk_c::setOldRootQuaternion(s16, s16, s16)
ext 023E2DC4 void daPy_lk_c::setShipRidePosUseItem()
ext 025E3F3C void player04bInitOldFrameMorf(u32, f32, u32, u32)
ext 024EDFCC BOOL player04bAttentionLockonTruth(u32)
ext 024EF09C s32 player04bGetSpecialCode(dBgS*, u32)
ext 020084C8 u32 player04bGetTrianglePlane(dBgS*, u32, u32)
ext 0200ECD4 f32 player04bAddCalc(be<f32>*, f32, f32, f32, f32)
str 10035ECC "Siren" in 024131D0
str 10035EC4 "ShipD" in 024131D0
str 10035EBC "Pjavdou" in 024131D0
ext 028E9098 void PSMTXIdentity(Mtx34*)
ext 025DADA4 void player04bCreateEnvironmentActor(s32, u32, cXyz*, u32, u32)
ext 025D79F4 void player04bOrderChangeEvent(daPy_lk_c*, u32, u32, u32, u32)
ext 025D78FC void player04bOrderNamedEvent(daPy_lk_c*, const char*, u32, u32)
ext 025D5A20 s32 player04bCreateChildActor(s32, u32, u32, cXyz*, s32, csXyz*, u32, s32, u32)
ext 025B9810 void player04bSetRestartRoom(u32, cXyz*, s32, u32)
ext 025602F0 void player04bGetSeaColor(GXColor*, GXColor*)
ext 0253F124 u32 player04bGetEventProcessId(u32, u32)
ext 0253E860 void player04bEndEventCamera(u32, u32)
ext 024F8044 u32 player04bGetCameraBody()
ext 024EF0BC u32 player04bGetGroundCode(dBgS*, cBgS_PolyInfo*)
ext 024EEABC BOOL player04bCheckMovingBackground(dBgS*, cBgS_PolyInfo*)
ext 024832E0 void player04bInitShipStartPosition(u32, cXyz*, s32)
ext 0201A478 void player04bConstructAngleVector(csXyz*, s32, s32, s32)
ext 020182E0 void player04bSetCylinderCenter(cM3dGCyl*, cXyz*)
ext 02008974 f64 player04bGroundCrossWide(dBgS*, dBgS_GndChk*)
ext 02008438 u32 player04bGetBackgroundActor(dBgS*, u32)
ext 02006804 s16 player04bInvertSignedAngle(u32)
ext 0200658C u32 player04bConstructSignedAngle(be<s16>*, s32)
ext 023FC06C void daPy_lk_c::setAnimeRatioHD()
str 10035CFC "mayuR" in 02410BE8
str 10035CF4 "mayuL" in 02410BE8
str 10035D10 "eyeR" in 02410BE8
str 10035D08 "eyeL" in 02410BE8
str 10035D8C "mayuRdamB" in 02410BE8
str 10035D80 "mayuLdamB" in 02410BE8
str 10035E24 "eyeRdamB" in 02410BE8
str 10035E18 "eyeLdamB" in 02410BE8
str 10035D74 "mayuRdamA" in 02410BE8
str 10035D68 "mayuLdamA" in 02410BE8
str 10035DFC "eyeRdamA" in 02410BE8
str 10035DF0 "eyeLdamA" in 02410BE8
str 10035DDC "\203A\203C\203e\203\200\227p\227\314\210\346[1]" in 02410BE8
str 10035DB0 "\203A\203C\203e\203\200\227p\227\314\210\346[0]" in 02410BE8
str 10035DC4 "m_item_heap[1] != (0)" in 02410BE8
str 10035D98 "m_item_heap[0] != (0)" in 02410BE8
str 10035D5C "linktexS3TC" in 02410BE8
str 101CEB48 "Link" in 02410BE8
str 10035D18 "Always" in 02410BE8
str 10035D04 "" in 02410BE8
ext 028F521C void player04bClearMemory(u32, u32)
ext 028E8970 void player04bCopyProjection(u32, void*)
ext 027FD764 void player04bDestroyShaderParameters(u32, s32)
ext 027FD6F4 void player04bConstructShaderParameters(u32)
ext 027FB528 void player04bDestroyUniformBlock(u32, s32)
ext 027FB40C void player04bConstructUniformBlock(u32)
ext 027F3628 void player04bDestroyAnimationTransform(u32, s32)
ext 027F13DC void player04bDestroyPacket(u32, u32)
ext 027F1278 void player04bConstructPacket(u32)
ext 027ED1F0 u32 player04bGetName(u32, u32)
ext 027BFE5C void player04bBindAttributeBuffer(u32)
ext 027BF7E8 void player04bReleaseAttributeBuffer(u32)
ext 027BEBEC void player04bReleaseUniformBuffer(u32)
ext 027BE2B0 void player04bDestroyTextureSampler(u32, s32)
ext 027B54A0 void player04bDestroyIndexBuffer(u32, s32)
ext 02756140 u32 player04bGetCurrentHeap(u32)
ext 02755FEC u32 player04bFindContainingHeap(u32, u32)
ext 0274FCCC void player04bUnlockGraphics(u32)
ext 0274FBF8 void player04bLockGraphics(u32)
ext 0274D80C u32 player04bGetCameraProjection(u32)
ext 0273AE48 u32 player04bAllocateAligned(u32, s32)
ext 0272B8B8 void player04bCopyTextureList(u32, u32)
ext 0268899C void player04bNotifySystemChild(u32)
ext 026066C4 u32 player04bGetResource(u32, player04b::SafeString*, u32)
ext 025F0148 void player04bFreeHeapAllocation(u32)
ext 025A5B18 void player04bConstructSmokeCallback(u32, u32)
ext 025564B4 void player04bRegisterPointLight(u32)
ext 02543534 u32 player04bSetStartDemo(u32, u32)
ext 0252D6E8 void player04bInitMirrorPacket(u32, u32)
ext 0252CCFC void player04bDestroyDrawListBase(u32, u32)
ext 0252CCBC void player04bConstructDrawListBase(u32)
ext 025164C0 void player04bSetCapsule(u32, u32)
ext 02515AE8 void player04bDestroyCollisionSphere(u32, s32)
ext 02515A70 void player04bDestroyCollisionCylinder(u32, s32)
ext 02515860 void player04bDestroyCollisionStatus(u32, s32)
ext 02444F48 void player04bTerminateSafeString(player04b::SafeString*)
ext 023DC63C void player04bClearActorKeep(u32)
ext 02082DDC void player04bDestroyModelPacket(u32, s32)
ext 02008B4C void player04bDestroyPolyCheck(u32, s32)
ext 027FFCBC u32 player04bGetMaterialManager()
ext 027B90AC s32 player04bFindMaterial(u32, player04b::SafeString*)
ext 02800B0C void player04bLoadMaterial(u32, u32, s32)
ext 027F29D4 u32 player04bGetActiveShader(u32)
ext 027FDA54 void player04bSetMaterialView(u32, s32, const Mtx34*, const void*, u32)
ext 027FDFF4 void player04bUpdateShape(u32, s32)
ext 027FB678 void player04bFlushMaterialUniforms(u32)
ext 027BE53C void player04bBindTexture(u32, u32, s32, s32)
ext 0274FDBC void player04bConstructRenderState(u32)
ext 0274FEB0 void player04bApplyRenderState(u32)
ext 027BDEB4 void player04bAssignTexture(u32, u32)
ext 027BB9E0 void player04bUpdateShaderProgram(u32, s32)
ext 027B9F68 void player04bBindShaderGroup(u32)
ext 027BB7CC void player04bBindShaderProgram(u32)
ext C00060E0 void GX2CallDisplayList(u32, u32)
ext C0006178 void GX2DrawIndexedEx(u32, u32, u32, u32, u32, u32)
ext C00068A8 void GX2SetGeometryUniformBlock(s32, u32, u32)
ext C0006900 void GX2SetPixelUniformBlock(s32, u32, u32)
ext C0006A38 void GX2SetVertexUniformBlock(s32, u32, u32)
str 10035ED4 "texmtx1"
ext 025E3630 JKRSolidHeap* mDoExt_createSolidHeapFromGameToCurrent(u32, u32)
ext 025E37D8 void mDoExt_restoreCurrentHeap()
ext 025E18EC void mDoAud_bgmStart(u32)
ext 025E1928 void mDoAud_subBgmStop()
ext 025E1988 void mDoAud_seStart(u32)
ext 0200F564 BOOL cLib_chaseS(be<s16>*, s16, s16)
ext 023DBC24 BOOL daPy_lk_c::checkHeavyStateOn()
ext 023E26EC int daPy_lk_c::checkShipRideUseItem(int)
ext 023DFDD8 BOOL daPy_lk_c::commonProcInit(int)
ext 023DC7AC void daPy_lk_c::deleteEquipItem(BOOL)
ext 023F8858 f32 daPy_lk_c::getSwimTimerRate()
ext 023E2E18 void daPy_lk_c::initShipRideUseItem(int, int)
ext 023E14A0 void daPy_lk_c::setBlendMoveAnime(f32)
ext 023E287C void daPy_lk_c::setShipRidePos(int)
ext 023E0A04 BOOL daPy_lk_c::setSingleMoveAnime(int, f32, f32, int, f32)
ext 023DD768 void daPy_lk_c::setTextureAnime(u32, int)
data l_selfie_pose 10035064
data l_selfie_pad 10035EF4
ext 023D6184 BOOL player04bCreateHeap(fopAc_ac_c*) callback
ext 0281E5A8 void player04bSetEmitterMaterial(JPABaseEmitter*, player04b::EmitterMaterialParam*)
ext 025F01D8 void player04bBeginNamedHeap(void*, const char*, u32, u32)
ext 025F0270 void player04bEndNamedHeap(void*, s32)
str 10035EDC "jparticle_type_b" in 02415138
str 10035BE0 "(\214\225\203u\203\211\201[)daPy_swBlur_c" in 0240FE40
ext 027DA984 u32 player04bConstructAnimationBackend(u32)
ext 0273B560 void player04bSetPointerArrayBuffer(u32, s32, u32)
ext 0200BD2C u32 player04bConstructCollisionStatus(u32)
ext 02018590 u32 player04bConstructCylinder(u32)
ext 02018150 u32 player04bConstructCapsule(u32)
ext 025166F0 u32 player04bConstructSphere(u32)
ext 025A9084 u32 player04bConstructRippleCallback(u32)
ext 02080404 u32 player04bConstructModelPacket(u32)
ext 028EFFD0 void player04bConstructArray(u32, u32, u32, u32)
ext 028F0164 void player04bDestroyArray(u32, u32, u32, u32, u32, u32)
ext 023D47E8 void player04bConstructPlayerBase(u32)
ext 023D483C void player04bDestroyPlayerBase(u32, s32)
ext 02008FEC void player04bConstructLineCheck(u32)
ext 02008E0C void player04bConstructGroundCheck(u32)
ext 02008DAC void player04bDestroyBackgroundCheck(u32, s32)
ext 024F0474 void player04bConstructAcch(u32)
ext 024EFD9C void player04bDestroyAcch(u32, s32)
ext 024EE7AC void player04bConstructRoofCheck(u32)
ext 0252CE74 void player04bConstructMirrorPacket(u32)
ext 02801DA0 void player04bConstructAnimeSound(u32)
ext 02515DA0 void player04bConstructCollisionGlobalStatus(u32)
ext 02515FB8 void player04bConstructCollisionObjectInfo(u32)
ext 02515980 void player04bDestroyCollisionObjectInfo(u32, s32)
ext 027B5430 void player04bConstructDrawCall(u32)
ext 027BDF7C void player04bConstructSampler(u32)
ext 027BE6B8 void player04bConstructTexture(u32)
ext 027FE084 void player04bCreateShape(u32, s32, s32)
ext 027B54E0 void player04bSetDrawCallIndices(u32, u32, s32, s32)
ext 027FF478 void player04bSetVertexBuffer(u32, u32, s32, s32)
ext 027FF530 void player04bBindVertexArray(u32, u32, u32, s32, s32)
ext 02773798 void player04bLoadTextureImage(u32, u32)
ext 027B5E94 void player04bFlushVertexBuffer(u32, u32, u32)
data n_color0 101CEEC0
data y_color0 101CEEC4
data g_color0 101CEEC8
str 10035B88 "d_a_player_eff.inc" in 0240EE9C
str 10035B9C "tmp_img != (0)" in 0240EE9C
str 10035BBC "d_a_player_eff.inc" in 0240F564
str 10035BD0 "tmp_img != (0)" in 0240F564
str 10035C18 "d_a_player_main.cpp" in 02410888
str 10035C2C "tmp_tp != (0)" in 02410888
str 10035C3C "tmp_tk != (0)" in 02410888
str 10035C4C "*i_heap != (0)" in 02410888
str 10035C80 "tmp_trans_bas != (0)" in 02410888
str 10035CB8 "tmp_trans != (0)" in 02410888
str 10035CF0 "0" in 02410BE8
str 10035D20 "d_a_player_main.cpp" in 02410BE8
str 10035D34 "zoff_blend_cnt <= 4" in 02410BE8
str 10035D48 "zoff_blend_cnt == 4" in 02410BE8
str 10035E08 "zon_cnt <= 4" in 02410BE8
str 10035E30 "zoff_none_cnt <= 4" in 02410BE8
str 10035E44 "zon_cnt == 4" in 02410BE8
str 10035E54 "zoff_none_cnt == 4" in 02410BE8
str 10035E68 "tmp_tex != (0)" in 02410BE8
str 10035E78 "tmp_img != (0)" in 02410BE8
str 10035E88 "texture != (0)" in 02410BE8
str 10035E98 "textureName != (0)" in 02410BE8
str 10035C5C "daPy_lk_c::AnimeHeap TRANS_BAS_ANM" in 02410888
str 10035C98 "daPy_lk_c::AnimeHeap TRANS_ANM" in 02410888
str 10035CCC "daPy_lk_c::AnimeHeap TEX_PATTERN" in 02410888
str 10035BF8 "daPy_lk_c::AnimeHeap TEX_SCROLL" in 02410888
data l_method 101CEEB0
data l_tail_scale_guard 1046D1E4
data l_tail_scale 1046D1E8
data l_right_dir_guard 1046D1F4
data l_right_dir 1046D1F8
ext 02525FE4 cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class*, cPhs__Handler*, void*)
ext 023DCA08 void daPy_lk_c::endDamageEmitter()
ext 023DD054 void daPy_lk_c::animeUpdate()
ext 023E2FF4 BOOL daPy_lk_c::procWait_init()
ext 023E47D4 BOOL daPy_lk_c::procVomitJump_init(int)
ext 023E9D50 BOOL daPy_lk_c::changeDragonShield(int)
ext 023F1154 BOOL daPy_lk_c::procMove_init()
ext 023F1D84 BOOL daPy_lk_c::procShipSteer_init()
ext 023F1E60 BOOL daPy_lk_c::procShipPaddle_init()
ext 023F54E8 BOOL daPy_lk_c::procLargeDamage_init(int, int, s16, s16)
ext 023F6020 BOOL daPy_lk_c::procLargeDamageUp_init(int, int, s16, s16)
ext 023F6564 BOOL daPy_lk_c::procFall_init(int, f32)
ext 023F6794 BOOL daPy_lk_c::procSlowFall_init()
ext 023F8B00 BOOL daPy_lk_c::procSwimWait_init(int)
ext 023F8F80 BOOL daPy_lk_c::changeSwimProc()
ext 023FE224 void daPy_lk_c::setWaterY()
ext 023FE960 int daPy_lk_c::setRoomInfo()
ext 023FF6A0 void daPy_lk_c::setWorldMatrix()
*/
