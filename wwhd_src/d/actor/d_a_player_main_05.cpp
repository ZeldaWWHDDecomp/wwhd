/**
 * d_a_player_main_05.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #05 (02419FEC..02423D0F): side step, crouch, slides,
 * rolls, jumps and landings, rope swing, damage procs, boots/not-use, and the first demo procs
 * (d_a_player_dproc.inc: tool, talk, get item, dead, look around, ...).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * "HIO folded" marks m_HIO parameters that HD compiled in as constants.
 * Functions of other ranges are called by address (gabi::call), not as C++ members.
 */
#include "d/actor/d_a_player_main.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* play + 0x5CD8 / 0x5CDC: dComIfGp_set/checkPlayerStatus0/1(0, flag) */
static inline void dComIfGp_onPlayerStatus0_l(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CD8;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
static inline u32 dComIfGp_checkPlayerStatus0_l(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & flag; }
static inline void dComIfGp_onPlayerStatus1_l(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CDC;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
static inline u32 dComIfGp_checkPlayerStatus1_l(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & flag; }
/* play + 0x5BB5 / 0x5BB7: dComIfGp_setRStatus / setDoStatus */
static inline void dComIfGp_setRStatus_l(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB5, s); }
static inline void dComIfGp_setDoStatus_l(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB7, s); }
/* play + 0x5292: dComIfGp_event_runCheck() */
static inline u8 dComIfGp_event_runCheck_l() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
/* play + 0x5B60: dComIfGp_setItemMagicCount(n) (adds) */
static inline void dComIfGp_setItemMagicCount_l(s16 n) {
    u32 a = dComIfGp_ea() + 0x5B60;
    gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n));
}
/* camera attention status of a camera info index: play + 0x5B00 + idx * 0x34 */
static inline u32 dComIfGp_getCameraAttentionStatus_l(s32 idx) { return gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5B00); }
/* dComIfGs: the save data object *(0x101F84DC); magic at +0x34, shield at +0x2F */
static inline u32 dComIfGs_base_l() { return gabi::load<u32>(0x101F84DC); }
/* daPy_dmEcallBack_c::checkCurse(): the static curse type at 0x101CEF16 */
static inline BOOL daPy_checkCurse_l() { return gabi::load<u16>(0x101CEF16) == 1; }
/* cLib_addCalc(f32* value, f32 target, f32 scale, f32 maxStep, f32 minStep): f1 as returned */
static inline f64 cLib_addCalc_l(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f64>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* cBgS::GroundCross (play + 0x12A0): f1 as returned */
static inline f64 dBgS_GroundCross_l(void* chk) { return gabi::call<f64>(0x02008974, dComIfG_Bgsp(), chk); }
static inline f64 PSVECSquareMag_d(const cXyz* v) { return gabi::call<f64>(0x028E8DD0, v); }
static inline f64 std_sqrtf_d(f64 x) { return gabi::call<f64>(0x028F4384, x); }
/* dAttention_c::LockonTruth */
static inline BOOL dAttention_LockonTruth_l(u32 att) { return gabi::call<BOOL>(0x024EDFCC, att); }
/* JAIZelBasic::seStartSystem */
static inline void mDoAud_seStartSystem_l(u32 id) { gabi::call(0x025E1988, id); }

/* daPy_lk_c members in the opaque blocks of the shared header (offsets measured in this range) */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define mBodyAngleX LK_FIELD(s16, 0x3D0) /* daPy_py_c mBodyAngle.x (GameCube 0x2B4) */
#define mBodyAngleY LK_FIELD(s16, 0x3D2) /* daPy_py_c mBodyAngle.y */
#define mpParachuteFanMorf LK_FIELD(u32, 0x44D0) /* mDoExt_McaMorf* (GameCube 0x...) */
#define mpEquipItemModel LK_FIELD(u32, 0x4440) /* J3DModel* */
#define mDemoMode LK_FIELD(u32, 0x430) /* mDemo.getDemoMode() (GameCube 0x314) */

/* ---- daPy_lk_c functions of other ranges (by address) ---- */
enum : u32 {
    LK_checkNextActionBowFly = 0x02419ED4,
    LK_checkNextActionBoomerangFly = 0x02419F54,
    LK_commonProcInit = 0x023DFDD8,
    LK_setSingleMoveAnime = 0x023E0A04,
    LK_checkHeavyStateOn = 0x023DBC24,
    LK_itemTrigger = 0x023EAB40,
    LK_getReadyItem = 0x023EAA80,
    LK_procJumpCut_init = 0x023ED5A4,
    LK_getItemAnimeResource = 0x023DDEEC, /* unnamed by the matcher */
    LK_setItemHeap = 0x023DDF64,
    LK_deleteEquipItem = 0x023DC7AC,
    LK_checkSetItemTrigger = 0x023E9AF4,
    LK_checkItemChangeFromButton = 0x023EFB68,
    LK_procFall_init = 0x023F6564,
    LK_resetFootEffect = 0x023DF9C0,
    LK_procSubjectivity_init = 0x023EEA30,
    LK_procCrouchDefense_init = 0x023EE510,
    LK_checkGuardAccept = 0x023EE328,
    LK_getCrawlMoveVec = 0x02415FB0,
    LK_procCrawlStart_init = 0x02416160,
    LK_changeSlideProc = 0x023E4CD0,
    LK_checkNextActionFromButton = 0x023EFF20,
    LK_setSpeedAndAngleNormal = 0x0241650C,
    LK_setBlendMoveAnime = 0x023E14A0,
    LK_getSlidePolygon = 0x023E4A58,
    LK_setMoveAnime = 0x023E0E50,
};
/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void fcpy_l(u32 dst, u32 src) { gmem_stf32(dst, gmem_ld32(src)); }
/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */
#define LK_voiceStart(n) gabi::call_ptr(gabi::load<u32>(__vtbl + 0xE4), this, (u32)(n))
#define LK_doTrigger() (mItemTrigger & 1)
#define LK_swordTrigger() (mItemTrigger & 2)
#define LK_spActionButton() (mItemButton & 0x40)
#define LK_cutEnd() dComIfGp_evmng_cutEnd(mStaffIdx)
/* m_old_fdata->getOldFrameRate() */
#define LK_oldFrameRate() gabi::load<f32>(m_old_fdata + 0xC)
/* checkAttentionLock(): dAttention_c::LockonTruth() or the attention's lock flag (+0x20 bit 0x20000000) */
#define LK_checkAttentionLock(att) (dAttention_LockonTruth_l(att) || (gabi::load<u32>((att) + 0x20) & 0x20000000))
/* checkNextMode 023F14E0 (declared in phase 1, defined in another range) */
#define LK_checkNextMode(n) gabi::call<BOOL>(0x023F14E0, this, (s32)(n))

/* 02419FEC */
void daPy_lk_c::checkNextActionItemFly() {
    WWHD_FUNC(0x02419FEC, void, this);
    if (!daPy_checkCurse_l()) {
        u16 item = mEquipItem;
        if (checkBowItem(item)) {
            gabi::call(LK_checkNextActionBowFly, this);
        } else if (item == 0x2D /* dItemNo_BOOMERANG_e */) {
            gabi::call(LK_checkNextActionBoomerangFly, this);
        }
    }
}
VERIFY(0x02419FEC, &daPy_lk_c::checkNextActionItemFly);

/* 0241A05C */
BOOL daPy_lk_c::procSideStepLand_init() {
    WWHD_FUNC(0x0241A05C, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0xB /* daPyProc_SIDE_STEP_LAND_e */);
    mNormalSpeed = 0.0f;
    int anm = mDirection == 2 /* DIR_LEFT */ ? 0x12 /* ANM_ATNJLLAND */ : 0x13 /* ANM_ATNJRLAND */;
    gabi::call(LK_setSingleMoveAnime, this, anm, 0.85f, 1.0f, 5, 0.0f); /* HD: HIO folded */
    mFootEffectPosType = 3;
    setResetFlg0(resetFlg0() | 0xC00); /* RIGHT/LEFT_FOOT_ON_GROUND */
    current.angle.y = (s16)(current.angle.y + 0x8000);
    if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x31, v);
    }
    return TRUE;
}
VERIFY(0x0241A05C, &daPy_lk_c::procSideStepLand_init);

/* 0241A154 */
BOOL daPy_lk_c::checkJumpCutFromButton() {
    WWHD_FUNC(0x0241A154, BOOL, this);
    u16 item = mEquipItem;
    if ((item == 0x103 /* daPyItem_SWORD_e */ && ((resetFlg0() & 0x80) || LK_swordTrigger())) ||
        (item == 0x101 /* daPyItem_BOKO_e */ && LK_doTrigger() /* HD: the A trigger */) ||
        (item == 0x33 /* dItemNo_SKULL_HAMMER_e */ && gabi::call<BOOL>(LK_itemTrigger, this) &&
         gabi::call<s32>(LK_getReadyItem, this) == 0x33)) {
        return gabi::call<BOOL>(LK_procJumpCut_init, this, 1);
    }
    return FALSE;
}
VERIFY(0x0241A154, &daPy_lk_c::checkJumpCutFromButton);

/* 0241A204 */
/* modelData->getJointNodePointer(jnt)->setCallBack(daPy_parachuteJointCallback): HD joint table of
 * 0x1C entries at +8, count at +4 (out of range: entry 0) */
static inline void lk_setJointCallBack_l(u32 modelData, u32 jnt) {
    u32 cnt = gabi::load<u32>(modelData + 4);
    u32 node = gabi::load<u32>(modelData + 8);
    if (cnt > jnt) {
        node += jnt * 0x1C;
    }
    gabi::store<u32>(node + 8, 0x023D6850 /* daPy_parachuteJointCallback */);
}
void daPy_lk_c::setParachuteFanModel(f32 frame) {
    WWHD_FUNC(0x0241A204, void, this, frame);
    u32 bck = gabi::call<u32>(LK_getItemAnimeResource, this, 0x84 /* dRes_INDEX_LKANM_BCK_FANBA_e */);
    u32 oldHeap = gabi::call<u32>(LK_setItemHeap, this);
    u32 modelData = gabi::ea(dComIfG_getObjectRes(STR(0x101CEB48) /* l_arcName "Link" */, 0x1A /* dRes_INDEX_LINK_BDL_FANB_e */, 0x10034B24));
    u32 morf = gabi::ea(mDoExt_McaMorf::create(nullptr, gabi::at<J3DModelData>(modelData), nullptr, nullptr,
                                               gabi::at<J3DAnmTransform>(bck), 0 /* EMode_NONE */, 0.5f /* HD: HIO folded */,
                                               gabi::ftoi(frame), 0xD, 0, nullptr, 0, 0x11020203));
    mpParachuteFanMorf = morf;
    if (morf == 0 || gabi::load<u32>(morf + 0x90) == 0) {
        JUT_ASSERT_fail(STR(0x10035F5C), 0x156, STR(0x10035F58));
        morf = mpParachuteFanMorf;
    }
    mpEquipItemModel = gabi::load<u32>(morf + 0x90);
    gabi::call(0x025E3570 /* mDoExt_setCurrentHeap */, oldHeap);
    gabi::store<u32>(mpEquipItemModel + 0xB8, gabi::ea(this)); /* setUserArea */
    lk_setJointCallBack_l(modelData, 1 /* FANB_JNT_LROOT_e */);
    lk_setJointCallBack_l(modelData, 7 /* FANB_JNT_RROOT_e */);
    lk_setJointCallBack_l(modelData, 3 /* FANB_JNT_LARMB_e */);
    lk_setJointCallBack_l(modelData, 9 /* FANB_JNT_RARMB_e */);
}
VERIFY(0x0241A204, &daPy_lk_c::setParachuteFanModel);

/* 0241A3C4 */
BOOL daPy_lk_c::procFanGlide_init(int param_0) {
    WWHD_FUNC(0x0241A3C4, BOOL, this, param_0);
    if (mCurProc == 0x93 /* daPyProc_FAN_GLIDE_e */) { /* HD */
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x93);
    gabi::call(LK_deleteEquipItem, this, 0);
    f32 start = param_0 != 0 ? 3.0f : 1.0f; /* HD: HIO folded */
    gabi::call(LK_setSingleMoveAnime, this, 0xA2 /* ANM_USEFANB */, 0.5f, start, 0xD, 0.0f);
    mEquipItem = 0x102 /* daPyItem_UNK102_e */;
    setParachuteFanModel(start);
    mProcVar3 = 0;
    mProcVar4 = 0;
    mProcVar2 = 0;
    mProcVar6 = 0;
    m3600 = 0.0f;
    m3604 = 0.0f;
    mProcVar0 = 0;
    mProcVar5 = 0;
    m3730.copy(*gabi::at<cXyz>(0x101FFBA8) /* cXyz::Zero */);
    LK_voiceStart(5);
    seStartOnlyReverb(0x2845 /* JA_SE_LK_FAN_CHUTE_OPEN */);
    mProcVar7 = 0x28;
    if (!gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        dComIfGp_setItemMagicCount_l(-1);
    }
    dComIfGp_onPlayerStatus1_l(0x20 /* daPyStts1_DEKU_LEAF_FLY_e */);
    mProcVar1 = 0x14;
    return TRUE;
}
VERIFY(0x0241A3C4, &daPy_lk_c::procFanGlide_init);

/* 0241A578 */
BOOL daPy_lk_c::checkFanGlideProc(int param_0) {
    WWHD_FUNC(0x0241A578, BOOL, this, param_0);
    if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x34 /* dItemNo_DEKU_LEAF_e */, 1)) {
        if (gabi::load<u8>(dComIfGs_base_l() + 0x34) != 0) { /* dComIfGs_getMagic() >= 1 */
            return procFanGlide_init(param_0);
        }
        mDoAud_seStartSystem_l(0x883 /* JA_SE_ITEM_TARGET_OUT */);
    }
    return FALSE;
}
VERIFY(0x0241A578, &daPy_lk_c::checkFanGlideProc);

/* 0241A604 */
BOOL daPy_lk_c::procSideStep() {
    WWHD_FUNC(0x0241A604, BOOL, this);
    /* HD: m_HIO->mSideStep.m.field_0x0 folded (on) */
    fopAc_ac_c* lockOn = mpAttnActorLockOn;
    if (lockOn != nullptr) {
        s16 a = fopAcM_searchActorAngleY(this, lockOn);
        cLib_addCalcAngleS(&shape_angle.y, a, 5, 0x5E8, 0x13C);
    }
    if (mDirection == 2 /* DIR_LEFT */) {
        current.angle.y = (s16)(shape_angle.y + 0x4000);
    } else {
        current.angle.y = (s16)(shape_angle.y - 0x4000);
    }
    checkNextActionItemFly();
    if (mAcch.ChkGroundHit()) {
        procSideStepLand_init();
    } else {
        if (checkJumpCutFromButton()) {
            return TRUE;
        }
        if (checkFanGlideProc(0)) {
            return TRUE;
        }
        if (current.pos.y < m3688.y - 50.0f) {
            gabi::call(LK_procFall_init, this, 2, 14.0f);
        }
    }
    gabi::call(LK_checkItemChangeFromButton, this);
    return TRUE;
}
VERIFY(0x0241A604, &daPy_lk_c::procSideStep);

/* 0241A718 */
BOOL daPy_lk_c::procSideStepLand() {
    WWHD_FUNC(0x0241A718, BOOL, this);
    gabi::call(LK_resetFootEffect, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_checkNextMode(0);
    } else if (mFrameCtrlUnder[0].getFrame() > 1.5f) {
        LK_checkNextMode(1);
    }
    return TRUE;
}
VERIFY(0x0241A718, &daPy_lk_c::procSideStepLand);

/* 0241A798 */
BOOL daPy_lk_c::procCrouchDefense() {
    WWHD_FUNC(0x0241A798, BOOL, this);
    dComIfGp_setRStatus_l(0x36 /* dActStts_DEFEND_e */);
    if (gabi::call<BOOL>(0x02514E24 /* dCamera_c::ChangeModeOK */, gabi::call<u32>(0x024F8044 /* dCam_getBody */), 4) &&
        !(current.pos.y < mWaterY)) {
        setResetFlg0(resetFlg0() | 0x4000000 /* daPyRFlg0_SUBJECT_ACCEPT */);
        if ((dComIfGp_getCameraAttentionStatus_l(mCameraInfoIdx) & 0x1000) && !dComIfGp_event_runCheck_l()) {
            return gabi::call<BOOL>(LK_procSubjectivity_init, this, 1);
        }
    }
    cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.6f, 2.5f, 1.8f); /* HD: HIO folded */
    u32 att = mpAttention;
    if (LK_checkAttentionLock(att) || !LK_spActionButton()) {
        mDirection = 3 /* DIR_RIGHT */;
        current.angle.y = shape_angle.y;
        LK_checkNextMode(0);
        return TRUE;
    }
    u16 a = (u16)(m34E8 - shape_angle.y);
    f32 f = mStickDistance * cM_scos(a);
    if (gabi::load<u8>(gabi::call<u32>(0x027200D0, dComIfGs_base_l() + 0x12C0) + 2) == 0) { /* HD: camera option */
        f = -f;
    }
    if (!(f < 0.0f)) {
        cLib_addCalcAngleS(&mBodyAngleX, (s16)gabi::ftoi(2500.0f * f), 4, 0xC00, 0x180);
    } else {
        cLib_addCalcAngleS(&mBodyAngleX, (s16)gabi::ftoi(8500.0f * f), 4, 0xC00, 0x180);
    }
    cLib_addCalcAngleS(&mBodyAngleY, (s16)gabi::ftoi(10000.0f * mStickDistance * cM_ssin(a)), 4, 0xC00, 0x180);
    /* HD: turn the body with the stick held sideways */
    f32 stick = mStickDistance;
    if (stick > 0.9f) {
        f32 s = cM_ssin(a);
        if (std::fabs(s) > 0.91f || std::fabs(cM_ssin(m34DC)) > 0.95f) {
            s16 cur = shape_angle.y;
            cLib_addCalcAngleS(&shape_angle.y, (s16)(cur + (s16)gabi::ftoi(512.0f * (stick - 0.5f) * s)), 4, 0x40, 0x10);
        }
    }
    return TRUE;
}
VERIFY(0x0241A798, &daPy_lk_c::procCrouchDefense);

/* 0241AB04 */
BOOL daPy_lk_c::procNockBackEnd_init() {
    WWHD_FUNC(0x0241AB04, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x20 /* daPyProc_NOCK_BACK_END_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x33 /* ANM_ROLLFMIS */, 0.8f, 8.0f, 0x18, 3.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x0241AB04, &daPy_lk_c::procNockBackEnd_init);

/* 0241AB6C */
BOOL daPy_lk_c::procCrouchDefenseSlip() {
    WWHD_FUNC(0x0241AB6C, BOOL, this);
    dComIfGp_setRStatus_l(0x36 /* dActStts_DEFEND_e */);
    if (mProcVar0 > 0) {
        mProcVar0 = mProcVar0 - 1;
    }
    if (mEquipItem == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        fcpy_l(gabi::ea(&m35EC), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    } else {
        fcpy_l(gabi::ea(&m35E8), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    }
    f64 r = cLib_addCalc_l(&mNormalSpeed, 0.0f, m35A0, m35A4, m35A8);
    if (!(r > 0.001f)) {
        if (mProcVar6 != 0) {
            return procNockBackEnd_init();
        }
        if (mProcVar0 == 0) {
            return gabi::call<BOOL>(LK_procCrouchDefense_init, this);
        }
    } else if (mNormalSpeed < m35AC) {
        gabi::call(LK_resetFootEffect, this);
    }
    return TRUE;
}
VERIFY(0x0241AB6C, &daPy_lk_c::procCrouchDefenseSlip);

/* 0241AC94 */
BOOL daPy_lk_c::procCrouch() {
    WWHD_FUNC(0x0241AC94, BOOL, this);
    dComIfGp_setRStatus_l(0xF /* dActStts_CROUCH_e */);
    if (gabi::call<BOOL>(0x02514E24 /* dCamera_c::ChangeModeOK */, gabi::call<u32>(0x024F8044 /* dCam_getBody */), 4) &&
        !(current.pos.y < mWaterY)) {
        setResetFlg0(resetFlg0() | 0x4000000 /* daPyRFlg0_SUBJECT_ACCEPT */);
        if ((dComIfGp_getCameraAttentionStatus_l(mCameraInfoIdx) & 0x1000) && !dComIfGp_event_runCheck_l()) {
            return gabi::call<BOOL>(LK_procSubjectivity_init, this, 1);
        }
    }
    cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.6f, 2.5f, 1.8f); /* HD: HIO folded */
    if (!LK_spActionButton()) {
        LK_checkNextMode(0);
        return TRUE;
    }
    {
        u32 att = mpAttention;
        if (LK_checkAttentionLock(att) && gabi::load<u8>(dComIfGs_base_l() + 0x2F) != 0xFF /* checkShieldEquip() */ &&
            gabi::call<BOOL>(LK_checkGuardAccept, this)) {
            LK_checkNextMode(0);
            return TRUE;
        }
    }
    if (LK_oldFrameRate() < 0.01f && mStickDistance > 0.05f && !(mWaterY > current.pos.y + 15.0f) /* !checkCrawlWaterIn() */) {
        gabi::Local<cXyz> front, top, vec;
        PSMTXTrans(gabi::at<Mtx34>(0x1048D0CC) /* mDoMtx_stack_c */, current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_ZXYrotM(gabi::at<Mtx34>(0x1048D0CC), m34E2, shape_angle.y, 0);
        PSMTXMultVec(gabi::at<Mtx34>(0x1048D0CC), gabi::at<cXyz>(0x101CEBC0) /* l_crawl_start_front_offset */, front);
        PSMTXMultVec(gabi::at<Mtx34>(0x1048D0CC), gabi::at<cXyz>(0x101CEBE4) /* l_crawl_top_offset */, top);
        if (gabi::call<BOOL>(LK_getCrawlMoveVec, this, top.get(), front.get(), vec.get())) {
            gabi::Local<cXyz> p, d, xz;
            cXyz_mi(&current.pos, p, vec);
            f32 y = p->y + 5.0f;
            gabi::store<f32>(gabi::ea(this) + 0xB40, p->z); /* mGndChk.SetPos(&p) */
            gabi::store<f32>(gabi::ea(this) + 0xB38, p->x);
            p->y = y;
            gabi::store<f32>(gabi::ea(this) + 0xB3C, y);
            p->y = (f32)dBgS_GroundCross_l(gabi::at<void>(gabi::ea(this) + 0xB14));
            cXyz_mi(&current.pos, d, p);
            xz->x = d->x;
            xz->y = 0.0f;
            xz->z = d->z;
            f64 len = std_sqrtf_d(PSVECSquareMag_d(xz));
            s16 ang = gabi::call<s16>(0x020195B0 /* cM_atan2s */, -d->y, len);
            if (cLib_distanceAngleS(ang, m34E2) > 0x100) {
                return TRUE;
            }
        }
        gabi::call(LK_procCrawlStart_init, this);
    }
    return TRUE;
}
VERIFY(0x0241AC94, &daPy_lk_c::procCrouch);

/* 0241AF64 */
BOOL daPy_lk_c::procWaitTurn() {
    WWHD_FUNC(0x0241AF64, BOOL, this);
    cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.6f, 2.5f, 1.8f); /* HD: HIO folded */
    if (gabi::call<BOOL>(LK_changeSlideProc, this)) {
        return TRUE;
    }
    s16 r = cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 0x1E, 0x3CDF, 0x1F40); /* HD: HIO folded */
    current.angle.y = shape_angle.y;
    if (gabi::call<BOOL>(LK_checkNextActionFromButton, this)) {
        return TRUE;
    }
    if (r == 0) {
        if (dComIfGp_event_runCheck_l() && mDemoMode == 5 /* daPy_demo_c::DEMO_WAIT_TURN_e */) {
            LK_cutEnd();
            return TRUE;
        }
        LK_checkNextMode(0);
    }
    return TRUE;
}
VERIFY(0x0241AF64, &daPy_lk_c::procWaitTurn);

/* 0241B050 */
BOOL daPy_lk_c::procMoveTurn() {
    WWHD_FUNC(0x0241B050, BOOL, this);
    gabi::call(LK_setSpeedAndAngleNormal, this, 0xBB8); /* HD: HIO folded */
    cLib_addCalcAngleS(&shape_angle.y, current.angle.y, mProcVar0, mProcVar2, mProcVar3);
    if (!LK_checkNextMode(0)) {
        gabi::call(LK_setBlendMoveAnime, this, -1.0f);
    }
    return TRUE;
}
VERIFY(0x0241B050, &daPy_lk_c::procMoveTurn);

/* 0241B0C0 */
BOOL daPy_lk_c::procSlip() {
    WWHD_FUNC(0x0241B0C0, BOOL, this);
    f64 r = cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.6f, 1.25f, 0.1875f); /* HD: HIO folded */
    if (!(r > 0.001f)) {
        if (mStickDistance > 0.05f) {
            s16 a = shape_angle.y;
            current.angle.y = (s16)(a + 0x8000);
            shape_angle.y = (s16)(a + 0x100);
            mNormalSpeed = LK_FIELD(f32, 0x3C4) /* mMaxNormalSpeed */ * 0.5f;
            gabi::call(0x023F0FD0 /* procMoveTurn_init */, this, 0);
        } else {
            LK_checkNextMode(0);
        }
        return TRUE;
    }
    if (!(mAcch.m_flags & 0x10) /* !ChkWallHit() */) {
        if (mNormalSpeed < 2.5f) {
            gabi::call(LK_resetFootEffect, this);
        }
        u16 ang = current.angle.y;
        f32 y = current.pos.y + 2.5f;
        gabi::Local<cXyz> start, end;
        start->z = current.pos.z;
        start->y = y;
        f32 sz = current.pos.z;
        f32 sx = current.pos.x;
        end->y = y;
        start->x = sx;
        end->z = gabi::fmadds(50.0f, cM_scos(ang), sz);
        end->x = gabi::fmadds(50.0f, cM_ssin(ang), sx);
        gabi::call(0x024F1AFC /* dBgS_LinChk::Set */, gabi::ea(this) + 0x9D0, start.get(), end.get(), this);
        if (!gabi::call<BOOL>(0x02008860 /* cBgS::LineCross */, dComIfG_Bgsp(), gabi::ea(this) + 0x9D0)) {
            return TRUE;
        }
        u32 pla = gabi::call<u32>(0x020084C8 /* cBgS::GetTriPla */, dComIfG_Bgsp(), (u32)gabi::load<u16>(gabi::ea(this) + 0x9E6),
                                  (u32)gabi::load<u16>(gabi::ea(this) + 0x9E4));
        if (pla == 0) {
            return TRUE;
        }
        f32 ny = gabi::load<f32>(pla + 4);
        if (!(ny < 0.5f) || ny < -0.8f) { /* !cBgW_CheckBWall(ny) */
            return TRUE;
        }
    }
    mNormalSpeed = 0.0f;
    LK_checkNextMode(0);
    return TRUE;
}
VERIFY(0x0241B0C0, &daPy_lk_c::procSlip);

/* 0241B2BC */
BOOL daPy_lk_c::procSlideFrontLand_init() {
    WWHD_FUNC(0x0241B2BC, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x1C /* daPyProc_SLIDE_FRONT_LAND_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x36 /* ANM_SLIDEFLAND */, 0.8f, 2.0f, 9, 5.0f); /* HD: HIO folded */
    mNormalSpeed = mNormalSpeed * 0.5f;
    return TRUE;
}
VERIFY(0x0241B2BC, &daPy_lk_c::procSlideFrontLand_init);

/* HD: on the test stage "ITest62", the slide direction is turned by g_Counter.mTimer (0x101FF560; debug
 * code) times 0x400; the stage name compare is a sead::SafeString strcmp */
static inline s16 lk_slideAngle_l(s16 ang, u32 testName) {
    gabi::Local<SafeString> name;
    name->mStringTop = testName; /* "ITest62" */
    name->__vtbl = 0x10034B24;
    gabi::Local<SafeString> stage;
    stage->mStringTop = dComIfGp_ea() + 0x5134; /* dComIfGp_getStartStageName() */
    stage->__vtbl = 0x10034B24;
    gabi::call_ptr(gabi::load<u32>(name->__vtbl + 0x14), name.get());
    gabi::call_ptr(gabi::load<u32>(name->__vtbl + 0x14), name.get());
    u32 a = name->mStringTop;
    gabi::call_ptr(gabi::load<u32>(stage->__vtbl + 0x14), stage.get());
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
    if (equal) {
        ang = (s16)(ang + (gabi::load<u32>(0x101FF560) << 10));
    }
    return ang;
}

/* 0241B32C */
BOOL daPy_lk_c::procSlideFront() {
    WWHD_FUNC(0x0241B32C, BOOL, this);
    u32 pla = gabi::call<u32>(LK_getSlidePolygon, this);
    if (pla == 0) {
        procSlideFrontLand_init();
        return TRUE;
    }
    s16 ang = cM_atan2s(gabi::load<f32>(pla + 0), gabi::load<f32>(pla + 8));
    ang = lk_slideAngle_l(ang, 0x10035F88);
    cLib_addCalcAngleS(&current.angle.y, ang, 4, 0x1000, 0x400);
    cLib_addCalcAngleS(&shape_angle.y, current.angle.y, 4, 0x1000, 0x400);
    f32 ny = gabi::load<f32>(pla + 4);
    s16 cur = current.angle.y;
    f32 k = 3.0f /* HD: HIO folded */ * gabi::fmadds(0.5f, 1.0f - ny, 1.0f);
    f32 c = cM_scos((s16)(cur - ang));
    f32 spd = gabi::fmadds(k, c, mNormalSpeed);
    f32 max = LK_FIELD(f32, 0x3C4); /* mMaxNormalSpeed */
    mNormalSpeed = spd;
    if (spd > max) {
        mNormalSpeed = max;
    }
    seStartMapInfo(0x205A /* JA_SE_LK_SLIP_SUS */);
    return TRUE;
}
VERIFY(0x0241B32C, &daPy_lk_c::procSlideFront);

/* 0241B51C */
BOOL daPy_lk_c::procSlideBackLand_init() {
    WWHD_FUNC(0x0241B51C, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x1D /* daPyProc_SLIDE_BACK_LAND_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x38 /* ANM_SLIDEBLAND */, 1.0f, 2.0f, 8, 5.0f); /* HD: HIO folded */
    return TRUE;
}
VERIFY(0x0241B51C, &daPy_lk_c::procSlideBackLand_init);

/* 0241B578 */
BOOL daPy_lk_c::procSlideBack() {
    WWHD_FUNC(0x0241B578, BOOL, this);
    u32 pla = gabi::call<u32>(LK_getSlidePolygon, this);
    if (pla == 0) {
        procSlideBackLand_init();
        return TRUE;
    }
    s16 ang = cM_atan2s(gabi::load<f32>(pla + 0), gabi::load<f32>(pla + 8));
    ang = lk_slideAngle_l(ang, 0x10035F90);
    cLib_addCalcAngleS(&current.angle.y, ang, 4, 0x1000, 0x400);
    cLib_addCalcAngleS(&shape_angle.y, (s16)(current.angle.y + 0x8000), 4, 0x1000, 0x400);
    s16 cur = current.angle.y;
    f32 ny = gabi::load<f32>(pla + 4);
    f32 k = 3.0f /* HD: HIO folded */ * gabi::fmadds(0.5f, 1.0f - ny, 1.0f);
    f32 c = cM_scos((s16)(cur - ang));
    f32 spd = gabi::fmadds(k, c, mNormalSpeed);
    f32 max = LK_FIELD(f32, 0x3C4); /* mMaxNormalSpeed */
    mNormalSpeed = spd;
    if (spd > max) {
        mNormalSpeed = max;
    }
    seStartMapInfo(0x205A /* JA_SE_LK_SLIP_SUS */);
    return TRUE;
}
VERIFY(0x0241B578, &daPy_lk_c::procSlideBack);

/* 0241B774 */
BOOL daPy_lk_c::procSlideFrontLand() {
    WWHD_FUNC(0x0241B774, BOOL, this);
    cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.5f, 5.0f, 1.0f);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        current.angle.y = shape_angle.y;
        LK_checkNextMode(0);
    } else if (mFrameCtrlUnder[0].getFrame() > 6.0f /* HD: HIO folded */) {
        s16 a = shape_angle.y;
        s16 old = current.angle.y;
        current.angle.y = a;
        if (!LK_checkNextMode(1)) {
            current.angle.y = old;
        }
    }
    return TRUE;
}
VERIFY(0x0241B774, &daPy_lk_c::procSlideFrontLand);

/* 0241B83C */
BOOL daPy_lk_c::procSlideBackLand() {
    WWHD_FUNC(0x0241B83C, BOOL, this);
    cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.5f, 5.0f, 1.0f);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        current.angle.y = shape_angle.y;
        LK_checkNextMode(0);
    } else if (mFrameCtrlUnder[0].getFrame() > 6.0f /* HD: HIO folded */) {
        s16 a = shape_angle.y;
        s16 old = current.angle.y;
        current.angle.y = a;
        if (!LK_checkNextMode(1)) {
            current.angle.y = old;
        }
    }
    return TRUE;
}
VERIFY(0x0241B83C, &daPy_lk_c::procSlideBackLand);

/* 0241B904 */
BOOL daPy_lk_c::procFrontRollCrash_init() {
    WWHD_FUNC(0x0241B904, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x1F /* daPyProc_FRONT_ROLL_CRASH_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x33 /* ANM_ROLLFMIS */, 0.0f, 6.0f, 0x18, 1.0f); /* HD: HIO folded */
    mNoResetFlg0 = mNoResetFlg0 & ~8u; /* offNoResetFlg0(daPyFlg0_UNK8) */
    current.angle.y = (s16)(current.angle.y - 0x8000);
    mNormalSpeed = speedF * 0.4f;
    speed.y = 7.0f;
    {
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x31, v);
    }
    LK_voiceStart(8);
    gabi::call(0x025E1A40 /* mDoAud_seStart */, 0x282F /* JA_SE_LK_BODYATTACK */, &current.pos, (u32)m3620, (s32)(s8)mReverb);
    setResetFlg0(resetFlg0() | 0x2000 /* daPyRFlg0_FRONT_ROLL_CRASH */);
    {
        gabi::Local<cXyz> pos;
        fcpy_l(gabi::ea(&pos->z), gabi::ea(&current.pos.z));
        fcpy_l(gabi::ea(&pos->y), gabi::ea(&current.pos.y));
        fcpy_l(gabi::ea(&pos->x), gabi::ea(&current.pos.x));
        gabi::call(0x0255F458 /* dKy_Sound_set */, pos.get(), 100, gabi::load<u32>(gabi::ea(this) + 4) /* fopAcM_GetID */, 5);
    }
    if ((mAcch.m_flags & 0x10) && (mAcchCir[0].m_flags & 2)) { /* ChkWallHit() */
        u32 actor = gabi::call<u32>(0x02008438 /* cBgS::GetActorPointer */, dComIfG_Bgsp(), (u32)gabi::load<u16>(gabi::ea(this) + 0x74E));
        if (actor != 0 && gabi::load<s16>(actor + 8) == 0x2B /* fpcNm_Obj_Movebox_e */) {
            /* daObjMovebox::Act_c::set_rollCrash() */
            s32 mode = gabi::load<s32>(actor + 0x710);
            if (mode == 0 || mode == 5) {
                gabi::store<u32>(actor + 0x7D4, 1);
            }
        }
    }
    return TRUE;
}
VERIFY(0x0241B904, &daPy_lk_c::procFrontRollCrash_init);

/* 0241BAB8 */
BOOL daPy_lk_c::procFrontRoll() {
    WWHD_FUNC(0x0241BAB8, BOOL, this);
    if (mFrameCtrlUnder[0].getFrame() > 6.0f) {
        mFootEffectPosType = 4;
        gabi::call(0x023E4E2C /* endFlameDamageEmitter */, this);
    }
    if (gabi::call<u32>(LK_getSlidePolygon, this) != 0) {
        cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.5f, 2.5f, 0.1f);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (!(mStickDistance > 0.05f)) {
            mNormalSpeed = mNormalSpeed - 5.0f; /* HD: HIO folded */
        }
        LK_checkNextMode(0);
    } else if (mFrameCtrlUnder[0].getFrame() > 17.0f) {
        LK_checkNextMode(1);
    } else if (!(speedF < 10.0f) && !(noResetFlg1() & 0x100000)) {
        if (!(mNoResetFlg0 & 8)) {
            if (mProcVar6 == 0 || !(mAcch.m_flags & 0x10) || !(mAcchCir[0].m_flags & 2)) {
                return TRUE;
            }
            if (cLib_distanceAngleS((s16)(current.angle.y + 0x8000), gabi::load<s16>(gabi::ea(this) + 0x788) /* mAcchCir[0].GetWallAngleY() */) > 0x1388) {
                return TRUE;
            }
            f32 frame = mFrameCtrlUnder[0].getFrame();
            if (frame < 6.0f || frame > 15.0f) {
                return TRUE;
            }
            if (!(mNoResetFlg0 & 8)) {
                m3620 = gabi::call<u32>(0x024EECAC /* dBgS::GetMtrlSndId */, dComIfG_Bgsp(), gabi::ea(this) + 0x74C);
            }
        }
        procFrontRollCrash_init();
    }
    return TRUE;
}
VERIFY(0x0241BAB8, &daPy_lk_c::procFrontRoll);

/* 0241BC90 */
BOOL daPy_lk_c::procFrontRollCrash() {
    WWHD_FUNC(0x0241BC90, BOOL, this);
    if (!(mModeFlg & 2 /* ModeFlg_MIDAIR */)) {
        if (mFrameCtrlUnder[0].getRate() < 0.01f) {
            LK_checkNextMode(0);
        } else if (mFrameCtrlUnder[0].getFrame() > 20.0f) {
            LK_checkNextMode(1);
        }
    }
    if (mAcch.ChkGroundHit() && (mModeFlg & 2)) {
        mNormalSpeed = 0.0f;
        mFrameCtrlUnder[0].setRate(0.7f);
        LK_voiceStart(9);
        mModeFlg = (mModeFlg & ~2u) | 0x8000;
    }
    return TRUE;
}
VERIFY(0x0241BC90, &daPy_lk_c::procFrontRollCrash);

/* 0241BD70 */
BOOL daPy_lk_c::procNockBackEnd() {
    WWHD_FUNC(0x0241BD70, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_checkNextMode(0);
    } else if (mFrameCtrlUnder[0].getFrame() > 16.0f) {
        LK_checkNextMode(1);
    }
    return TRUE;
}
VERIFY(0x0241BD70, &daPy_lk_c::procNockBackEnd);

/* 0241BDD0 */
BOOL daPy_lk_c::procSideRoll_init() {
    WWHD_FUNC(0x0241BDD0, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x21 /* daPyProc_SIDE_ROLL_e */);
    int anm = (s16)(current.angle.y - shape_angle.y) > 0 ? 0x6E /* ANM_MROLLL */ : 0x6F /* ANM_MROLLR */;
    gabi::call(LK_setSingleMoveAnime, this, anm, 1.3f, 6.0f, 0x16, 6.0f); /* HD: HIO folded */
    mNormalSpeed = 26.0f;
    if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        mNormalSpeed = mNormalSpeed * 0.5f;
    }
    mFootEffectPosType = 4;
    return TRUE;
}
VERIFY(0x0241BDD0, &daPy_lk_c::procSideRoll_init);

/* 0241BE78 */
BOOL daPy_lk_c::procSideRoll() {
    WWHD_FUNC(0x0241BE78, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (frameCtrl.getRate() < 0.01f) {
        current.angle.y = shape_angle.y;
        if (!(mStickDistance > 0.05f)) {
            mNormalSpeed = 0.0f;
        }
        LK_checkNextMode(0);
    } else if (frameCtrl.checkPass(10.0f)) {
        gabi::call(0x023E4E2C /* endFlameDamageEmitter */, this);
    } else if (frameCtrl.getFrame() > 19.0f /* HD: HIO folded */) {
        s16 a = shape_angle.y;
        u32 spd = gmem_ld32(gabi::ea(&mNormalSpeed));
        s16 old = current.angle.y;
        current.angle.y = a;
        if (!LK_checkNextMode(1)) {
            gmem_stf32(gabi::ea(&mNormalSpeed), spd);
            current.angle.y = old;
        }
    }
    return TRUE;
}
VERIFY(0x0241BE78, &daPy_lk_c::procSideRoll);

/* 0241BF80 */
BOOL daPy_lk_c::procBackJumpLand_init() {
    WWHD_FUNC(0x0241BF80, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x23 /* daPyProc_BACK_JUMP_LAND_e */);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0x3E /* ANM_ROLLBLAND */, 0.8f, 0.0f, 5, 0.0f); /* HD: HIO folded */
    mFootEffectPosType = 3;
    setResetFlg0(resetFlg0() | 0xC00);
    current.angle.y = shape_angle.y;
    if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x31, v);
    }
    u16 item = mEquipItem;
    if ((item == 0x103 /* daPyItem_SWORD_e */ || item == 0x101 /* daPyItem_BOKO_e */) && !daPy_checkCurse_l() &&
        gabi::load<u16>(gabi::ea(this) + 0x5888) == 0xFFFF /* checkNoUpperAnime() */) {
        mProcVar6 = 1;
    } else {
        mProcVar6 = 0;
    }
    return TRUE;
}
VERIFY(0x0241BF80, &daPy_lk_c::procBackJumpLand_init);

/* 0241C094 */
BOOL daPy_lk_c::procBackJump() {
    WWHD_FUNC(0x0241C094, BOOL, this);
    if (mAcch.ChkGroundHit() && mFrameCtrlUnder[0].getRate() < 0.01f) {
        procBackJumpLand_init();
    } else {
        if (checkFanGlideProc(0)) {
            return TRUE;
        }
        if (current.pos.y < m3688.y - 30.0f) {
            gabi::call(LK_procFall_init, this, 2, 10.0f); /* HD: HIO folded */
        }
    }
    gabi::call(LK_checkItemChangeFromButton, this);
    return TRUE;
}
VERIFY(0x0241C094, &daPy_lk_c::procBackJump);

/* 0241C140 */
BOOL daPy_lk_c::procBackJumpLand() {
    WWHD_FUNC(0x0241C140, BOOL, this);
    gabi::call(LK_resetFootEffect, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (dComIfGp_event_runCheck_l()) {
            LK_cutEnd();
        } else {
            LK_checkNextMode(0);
        }
    } else if (mFrameCtrlUnder[0].getFrame() > 2.0f) {
        LK_checkNextMode(1);
    } else if (mProcVar6 != 0) {
        s32 v = m3578;
        mProcVar6 = 0;
        if ((v < 0 ? -v : v) > 0xF800 && mEquipItem == 0x103 /* daPyItem_SWORD_e */) {
            gabi::call(0x023E6380 /* procCutTurn_init */, this, 1);
        }
    }
    return TRUE;
}
VERIFY(0x0241C140, &daPy_lk_c::procBackJumpLand);

/* 0241C244 */
BOOL daPy_lk_c::procShipJumpRide_init() {
    WWHD_FUNC(0x0241C244, BOOL, this);
    u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C); /* dComIfGp_getShipActor() */
    if (ship == 0 || !(gabi::load<u32>(ship + 0x39C) & 0x80 /* fopAc_Attn_ACTION_SHIP_e */)) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x87 /* daPyProc_SHIP_JUMP_RIDE_e */);
    gabi::call(LK_deleteEquipItem, this, 1);
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    speed.y = 0.0f;
    gabi::store<u8>(ship + 0x636, 2);                                 /* ship->setPaddleMove() */
    gabi::store<u32>(ship + 0x644, gabi::load<u32>(ship + 0x644) | 0x2000); /* ship->onJumpRideFlg() */
    gabi::call(LK_setSingleMoveAnime, this, 0xA9 /* ANM_SLIPICE */, 1.0f, 0.0f, -1, 5.0f);
    gabi::call(0x023E287C /* setShipRidePos */, this, 0);
    mNoResetFlg0 = mNoResetFlg0 & ~0x200u; /* offNoResetFlg0(daPyFlg0_SHIP_DROP) */
    dComIfGp_onPlayerStatus1_l(0x80);
    seStartOnlyReverb(0x3838 /* JA_SE_LK_FT_JUMP_SHIP */);
    return TRUE;
}
VERIFY(0x0241C244, &daPy_lk_c::procShipJumpRide_init);

/* 0241C344 */
BOOL daPy_lk_c::checkJumpRideShip() {
    WWHD_FUNC(0x0241C344, BOOL, this);
    if (gabi::call<BOOL>(0x02516464 /* dCcD_GObjInf::ChkCoHit */, &mCyl)) {
        u32 ac = gabi::call<u32>(0x02515BBC /* GetCoHitAc */, gabi::ea(this) + 0x7738);
        if (ac != 0 && gabi::load<s16>(ac + 8) == 0xA5 /* fpcNm_SHIP_e */) {
            /* ship->getBodyMtx(): the body model's base matrix (NULL-preserving) */
            u32 model = gabi::load<u32>(gabi::load<u32>(ac + 0x3B4) + 0x90);
            gabi::Local<cXyz> p, d, xz;
            PSMTXMultVec(gabi::at<Mtx34>(model ? model + 0xC8 : 0), gabi::at<cXyz>(0x10034F84) /* l_ship_offset */, p);
            cXyz_mi(&old.pos, d, p);
            if (d->y > 5.0f) {
                xz->x = d->x;
                xz->z = d->z;
                xz->y = 0.0f;
                if (PSVECSquareMag_d(xz) < 10000.0f) {
                    return procShipJumpRide_init();
                }
            }
        }
    }
    return FALSE;
}
VERIFY(0x0241C344, &daPy_lk_c::checkJumpRideShip);

/* 0241C448 */
BOOL daPy_lk_c::procLandDamage_init(int param_1) {
    WWHD_FUNC(0x0241C448, BOOL, this, param_1);
    gabi::Local<cXyz> v;
    if (param_1 == 2) {
        if (!dComIfGp_event_runCheck_l()) {
            gabi::call(0x023F51D0 /* setDamagePoint */, this, -2.0f);
        }
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        dComIfGp_getVibration_StartShock(7, -0x21, v);
    } else {
        u32 play = dComIfGp_ea();
        if (param_1 == 1) {
            if (!gabi::load<u8>(play + 0x5292)) {
                gabi::call(0x023F51D0 /* setDamagePoint */, this, -1.0f);
            }
            v->x = 0.0f;
            v->y = 1.0f;
            v->z = 0.0f;
            dComIfGp_getVibration_StartShock(5, -0x21, v);
        } else {
            v->x = 0.0f;
            v->y = 1.0f;
            v->z = 0.0f;
            gabi::call<BOOL>(0x025CB374 /* dVibration_c::StartShock */, play + 0x599C, 2, -0x21, v.get());
        }
    }
    gabi::call(LK_commonProcInit, this, 0x26 /* daPyProc_LAND_DAMAGE_e */);
    mNormalSpeed = 0.0f;
    if (param_1 == 0) {
        mProcVar0 = 3;
        mModeFlg = mModeFlg & ~8u; /* offModeFlg(ModeFlg_DAMAGE) */
    } else {
        mProcVar0 = 0x1E;
        mDamageWaitTimer = 0x1E;
    }
    gabi::call(LK_setSingleMoveAnime, this, 0x30 /* ANM_LANDDAMA */, 0.8f, 4.0f, 9, 0.0f); /* HD: HIO folded */
    mProcVar6 = 0;
    seStartOnlyReverb(0x2816 /* JA_SE_LK_FALL_DAMAGE */);
    LK_voiceStart(13);
    mFootEffectPosType = 5;
    return TRUE;
}
VERIFY(0x0241C448, &daPy_lk_c::procLandDamage_init);

/* 0241C69C */
BOOL daPy_lk_c::procVomitLand_init() {
    WWHD_FUNC(0x0241C69C, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x99 /* daPyProc_VOMIT_LAND_e */);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0x30 /* ANM_LANDDAMA */, 1.0f, 4.0f, 9, 0.0f); /* HD: HIO folded */
    mProcVar0 = 7;
    mFootEffectPosType = 5;
    mProcVar6 = 0;
    return TRUE;
}
VERIFY(0x0241C69C, &daPy_lk_c::procVomitLand_init);

/* 0241C714 */
BOOL daPy_lk_c::procLand_init(f32 param_1, int param_2) {
    WWHD_FUNC(0x0241C714, BOOL, this, param_1, param_2);
    gabi::call(LK_commonProcInit, this, 0x25 /* daPyProc_LAND_e */);
    mNormalSpeed = 0.0f;
    if (param_2 != 0) {
        gabi::call(LK_setSingleMoveAnime, this, 0xD /* ANM_JMPEDS */, param_1, 2.0f, 0xC, 5.0f); /* HD: HIO folded */
    } else {
        gabi::call(LK_setSingleMoveAnime, this, 0xD /* ANM_JMPEDS */, param_1, 3.0f, 0xB, 0.0f);
    }
    mFootEffectPosType = 3;
    setResetFlg0(resetFlg0() | 0xC00);
    s16 cur = current.angle.y;
    s16 shp = shape_angle.y;
    if (cur != shp) {
        if (cLib_distanceAngleS(cur, shp) > 0x6000) {
            current.angle.y = shape_angle.y;
        } else {
            current.angle.y = (s16)(current.angle.y - 0x8000);
        }
    }
    if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x31, v);
    }
    return TRUE;
}
VERIFY(0x0241C714, &daPy_lk_c::procLand_init);

/* 0241C898 */
BOOL daPy_lk_c::changeLandProc(f32 param_1) {
    WWHD_FUNC(0x0241C898, BOOL, this, param_1);
    f32 posY = current.pos.y;
    f32 dist = m35F0 - posY;
    if (mCurrAttributeCode != 5 /* dBgS_Attr_GIANT_FLOWER_e */ && !(dist < 2000.0f)) { /* HD: HIO folded */
        if (!(dist < 6000.0f)) {
            return procLandDamage_init(2);
        }
        return procLandDamage_init(1);
    }
    int direction = getDirectionFromAngle((s16)(current.angle.y - shape_angle.y));
    s32 proc = mCurProc;
    BOOL bVar2;
    if (proc == 0x49 /* daPyProc_CUT_EX_MJ_e */ && (m34C5 != 0 || (mNoResetFlg0 & 4))) {
        bVar2 = TRUE;
    } else {
        bVar2 = FALSE;
        u16 anm = gabi::load<u16>(gabi::ea(this) + 0x5888);
        if (anm != 0x95 && anm != 0x96 /* !checkGrabAnime() */ && mStickDistance > 0.5f && direction != 1 /* DIR_BACKWARD */ &&
            (proc == 0x49 || !(m3688.y - posY < 300.0f)) && !(noResetFlg1() & 0x8000000) &&
            getDirectionFromCurrentAngle() == 0 /* DIR_FORWARD */) {
            LK_voiceStart(7);
            if (direction == 0) {
                speedF = 17.0f; /* HD: HIO folded */
                gabi::call(0x023EE86C /* procFrontRoll_init */, this, 6.0f);
            } else {
                procSideRoll_init();
            }
            return TRUE;
        }
    }
    if (!(dist < 1000.0f)) {
        if (!gabi::call<BOOL>(0x023EB6C8 /* checkBossGomaStage */, this) && mCurrAttributeCode != 5) {
            procLandDamage_init(0);
            return TRUE;
        }
        proc = mCurProc;
    }
    if (proc == 0x98 /* daPyProc_VOMIT_JUMP_e */) {
        procVomitLand_init();
    } else if (bVar2) {
        LK_checkNextMode(0);
    } else {
        procLand_init(param_1, 0);
    }
    return TRUE;
}
VERIFY(0x0241C898, &daPy_lk_c::changeLandProc);

/* 0241CB30 */
BOOL daPy_lk_c::checkSpecialRope() {
    WWHD_FUNC(0x0241CB30, BOOL, this);
    /* strcmp(dComIfGp_getStartStageName(), "GanonK") (inlined) */
    u32 a = dComIfGp_ea() + 0x5134;
    u32 b = 0x10035FA4;
    u8 ca, cb;
    do {
        ca = gabi::load<u8>(a++);
        cb = gabi::load<u8>(b++);
    } while (ca == cb && ca != 0);
    if (ca == cb && mActorKeepEquip.mActor == nullptr) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0241CB30, &daPy_lk_c::checkSpecialRope);

/* 0241CBB0 */
void daPy_lk_c::setBlendRopeMoveAnime(int param_0) {
    WWHD_FUNC(0x0241CBB0, void, this, param_0);
    f32 f30 = m35A4 * m35A8;
    f32 f2 = std::fabs(f30);
    gabi::Local<be<f32>> sp08;
    int anm;
    f32 dVar6;
    if (mModeFlg & 0x400) {
        *sp08 = 0.0f;
        anm = 0x77; /* ANM_ROPESWINGB */
    } else {
        /* m_pbCalc[PART_UNDER_e]->getRatio(1) */
        u32 pb = m_pbCalc[0];
        fcpy_l(sp08.a, gabi::load<u32>(gabi::load<u32>(pb + 0x7C) + 0x1C));
        s16 pv0 = mProcVar0;
        if (pv0 != 0) {
            anm = pv0 == -1 ? 0x76 /* ANM_ROPESWINGF */ : 0x77;
            dVar6 = 1.0f;
            cLib_chaseF(sp08.get(), 1.0f, 0.05f);
            pv0 = mProcVar0;
            if ((pv0 == -1 && 1.5707964f < f30) || (pv0 == 1 && -1.5707964f < f30)) {
                mProcVar0 = 0;
            }
        } else {
            if (param_0 == 0 && mStickDistance > 0.05f && !LK_spActionButton()) {
                if (1.5707964f > f30 && !(0.31415927f > f30)) {
                    gabi::call(LK_setMoveAnime, this, 0.0f, 1.0f, 1.0f, 0x75 /* ANM_ROPECATCH */, 0x76, 7, 7.0f);
                    mProcVar0 = -1;
                    return;
                }
                if (-1.5707964f > f30 && !(-2.8274333f > f30)) {
                    gabi::call(LK_setMoveAnime, this, 0.0f, 1.0f, 1.0f, 0x75 /* ANM_ROPECATCH */, 0x77, 7, 7.0f);
                    mProcVar0 = 1;
                    return;
                }
            }
            anm = f2 > 1.5707964f ? 0x76 : 0x77;
            dVar6 = (0.6f * (f32)(s16)mProcVar2) / 12000.0f; /* HD: HIO folded */
            if (checkSpecialRope()) {
                dVar6 = dVar6 * 8.0f;
            }
            if (dVar6 > 0.6f) {
                dVar6 = 0.6f;
            }
        }
        u32 data = gabi::call<u32>(0x023E048C /* getAnmData */, this, anm);
        if (gabi::load<u16>(data) != gabi::load<u16>(gabi::ea(this) + 0x5858) /* m_anm_heap_under[UNDER_MOVE1_e].mIdx */) {
            cLib_addCalc_l(sp08.get(), -dVar6, 0.5f, 0.05f, 0.005f);
            if (!(*sp08 > 0.0f)) {
                *sp08 = 0.0f;
            } else {
                anm = anm == 0x77 ? 0x76 : 0x77;
            }
        } else if (mProcVar0 == 0) {
            cLib_addCalc_l(sp08.get(), dVar6, 0.5f, 0.05f, 0.005f);
        }
    }
    f32 f4 = param_0 != 0 ? 3.0f : -1.0f; /* HD: HIO folded */
    gabi::call(LK_setMoveAnime, this, (f32)*sp08, 1.0f, 1.0f, 0x75 /* ANM_ROPECATCH */, anm, 7, f4);
}
VERIFY(0x0241CBB0, &daPy_lk_c::setBlendRopeMoveAnime);

/* the rope's distance check of procRopeSwing_init and changeRopeSwingProc: |v| - the hanging offset
 * (a float at 0x1046CCEC, GameCube 95.0f) */
static inline f64 lk_ropeLen_l(gabi::Local<cXyz>& v) {
    return std_sqrtf_d(PSVECSquareMag_d(v));
}

/* 0241D040 */
BOOL daPy_lk_c::procRopeSwing_init(fopAc_ac_c* rope_actor, s16 param_1) {
    WWHD_FUNC(0x0241D040, BOOL, this, rope_actor, param_1);
    fopAc_ac_c* i_rope_actor = rope_actor;
    gabi::call(LK_commonProcInit, this, 0x78 /* daPyProc_ROPE_SWING_e */);
    gabi::Local<cXyz> v10, v1c, v28, v34;
    if (rope_actor != nullptr) {
        gabi::call(0x023DE638 /* daPy_actorKeep_c::setData */, &mActorKeepRope, rope_actor);
        if (fpcM_GetName(rope_actor) == 0x1BF /* fpcNm_HIMO3_e */) {
            gabi::store<u32>(gabi::ea(rope_actor) + 0xB0, 1); /* fopAcM_SetParam(rope_actor, 1) */
            gabi::call(LK_deleteEquipItem, this, 1);
            mEquipItem = 0x25 /* dItemNo_GRAPPLING_HOOK_e */;
            cXyz_mi(&rope_actor->current.pos, v1c, &current.pos);
            v10->copy(*v1c);
            f64 len = lk_ropeLen_l(v10);
            f32 d = (f32)(len - (f64)gabi::load<f32>(0x1046CCEC));
            f32 maxLen = gabi::load<f32>(gabi::ea(rope_actor) + 0x1914); /* rope->getPlayerMoveLength() */
            if (d > maxLen) {
                d = maxLen;
            }
            gabi::call(0x0201B31C /* cXyz::normalize */, v10.get(), v1c.get());
            cXyz_ml(v10, v34, d);
            cXyz_mi(&rope_actor->current.pos, v1c, v34);
            current.pos.copy(*v1c);
        }
    } else {
        rope_actor = mActorKeepRope.mActor;
    }
    cXyz_mi(&rope_actor->current.pos, v28, &current.pos);
    v10->copy(*v28);
    f64 r = lk_ropeLen_l(v10);
    m35A0 = (f32)r;
    f64 s = std_sqrtf_d((f32)(2.0 / r));
    m35A4 = (f32)s;
    f32 dVar11 = (f32)((f64)1.5707964f / s);
    f32 f31;
    if (i_rope_actor != nullptr) {
        mProcVar2 = 0x2EE0; /* HD: HIO folded */
        if (checkSpecialRope()) {
            mProcVar2 = (s16)gabi::ftoi((f32)(s16)mProcVar2 * 0.125f);
        }
        u16 sa = shape_angle.y;
        f32 z = v10->z;
        f32 sn = cM_ssin(sa);
        f32 cs = cM_scos(sa);
        f32 x = v10->x;
        f32 y = v10->y;
        f32 lz = gabi::fmadds(sn, x, cs * z);  /* local_78.z */
        f32 lx = gabi::fmsubs(cs, x, sn * z);  /* local_78.x */
        s32 sVar4 = cM_atan2s(-lz, y);
        s32 pv2 = mProcVar2;
        f32 ratio;
        if (sVar4 > pv2) {
            ratio = (f32)pv2 / (f32)pv2;
        } else {
            if (sVar4 < -pv2) {
                sVar4 = (s16)-pv2;
            }
            ratio = (f32)sVar4 / (f32)pv2;
        }
        f64 sq = std_sqrtf_d(gabi::fnmsubs(ratio, ratio, 1.0f));
        f64 at = gabi::call<f64>(0x0201971C /* cM_atan2f */, ratio, sq);
        m35A8 = (f32)(at / (f64)(f32)m35A4);
        f64 h = std_sqrtf_d(gabi::fmadds(y, y, lz * lz));
        s16 a3 = gabi::call<s16>(0x020195B0 /* cM_atan2s */, -lx, h);
        mProcVar3 = a3;
        f31 = a3 > 0 ? dVar11 : -dVar11;
    } else {
        s32 p = param_1;
        mProcVar2 = (s16)(p < 0 ? -p : p);
        m35A8 = param_1 < 0 ? dVar11 : -dVar11;
        f31 = 0.0f;
        mProcVar3 = 0;
    }
    current.angle.y = shape_angle.y;
    m35AC = f31;
    mNormalSpeed = 0.0f;
    speed.y = 0.0f;
    gravity = 0.0f;
    gabi::at<cXyz>(gabi::ea(this) + 0x408)->copy(rope_actor->current.pos); /* mRopePos (daPy_py_c, GameCube 0x2EC) */
    dComIfGp_onPlayerStatus0_l(0x800000 /* daPyStts0_UNK800000_e */);
    if (i_rope_actor == nullptr || mActorKeepEquip.mActor != nullptr) {
        mModeFlg = (mModeFlg & ~0x400u) | 0x100;
        gabi::call(0x023DD768 /* setTextureAnime */, this, 8, 0);
        if (i_rope_actor == nullptr) {
            gabi::call(0x025E3F3C /* mDoExt_MtxCalcOldFrame::initOldFrameMorf */, (u32)m_old_fdata, 15.0f, 0, 0x2A);
        }
    } else {
        setBlendRopeMoveAnime(1);
    }
    /* HD: setStart also sets the frame */
    mFrameCtrlUnder[0].mFrame = 0.0f;
    mFrameCtrlUnder[0].mStart = 0;
    mFrameCtrlUnder[0].mEnd = 7; /* HD: HIO folded */
    if (i_rope_actor != nullptr && mActorKeepEquip.mActor == nullptr) {
        /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform()->setFrame(frame) */
        fcpy_l(gabi::load<u32>(gabi::ea(this) + 0x57FC), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    }
    shape_angle.x = param_1;
    mProcVar0 = 0;
    LK_FIELD(s16, 0x6922) = 0; /* HD-only */
    return TRUE;
}
VERIFY(0x0241D040, &daPy_lk_c::procRopeSwing_init);

/* 0241D5F0 */
int daPy_lk_c::changeRopeSwingProc() {
    WWHD_FUNC(0x0241D5F0, int, this);
    if (gabi::call<BOOL>(0x02516464 /* dCcD_GObjInf::ChkCoHit */, &mCyl)) {
        fopAc_ac_c* hit = gabi::call<fopAc_ac_c*>(0x02515BBC /* GetCoHitAc */, gabi::ea(this) + 0x7738);
        if (hit != nullptr && fpcM_GetName(hit) == 0x1BF /* fpcNm_HIMO3_e */) {
            gabi::Local<cXyz> d;
            cXyz_mi(&hit->current.pos, d, &current.pos);
            f64 len = lk_ropeLen_l(d);
            if (!((f32)(len - (f64)gabi::load<f32>(0x1046CCEC)) < 100.0f) && hit->current.pos.y > current.pos.y) {
                return procRopeSwing_init(hit, 0x1800);
            }
        }
    }
    return FALSE;
}
VERIFY(0x0241D5F0, &daPy_lk_c::changeRopeSwingProc);

/* dComIfGs_isEventBit: dSv_event_c at save + 0x644 */
static inline BOOL dComIfGs_isEventBit_l(u16 flag) { return gabi::call<BOOL>(0x025B8B94, dComIfGs_base_l() + 0x644, (u32)flag); }

/* 0241D6CC */
void daPy_lk_c::setFallVoice() {
    WWHD_FUNC(0x0241D6CC, void, this);
    u32 flg = mNoResetFlg0;
    if (flg & 0x40000 /* daPyFlg0_NO_FALL_VOICE */) {
        return;
    }
    if (flg & 0x80 /* daPyFlg0_UNK80 */) {
        f32 gh = mAcch.GetGroundH();
        if (!(mWaterY < gh) && m3580 != 4 && gh != -1000000000.0f) {
            return;
        }
    }
    f32 y = current.pos.y;
    if (m35F0 - y > 500.0f && !(y - mAcch.GetGroundH() < 2000.0f) /* HD: HIO folded */) {
        LK_voiceStart(12);
        mNoResetFlg0 = mNoResetFlg0 | 0x40000;
        gabi::call(0x023DD768 /* setTextureAnime */, this, 0x56, 0);
    }
}
VERIFY(0x0241D6CC, &daPy_lk_c::setFallVoice);

/* 0241D79C */
BOOL daPy_lk_c::procAutoJump() {
    WWHD_FUNC(0x0241D79C, BOOL, this);
    if (mProcVar0 == 0) {
        f32 stick = mStickDistance;
        if (stick > 0.05f) {
            if (getDirectionFromCurrentAngle() == 1 /* DIR_BACKWARD */) {
                cLib_chaseF(&mNormalSpeed, 0.0f, 0.8f * stick); /* HD: HIO folded */
            } else {
                cLib_chaseF(&mNormalSpeed, 17.0f, 0.4f);
            }
        } else {
            cLib_chaseF(&mNormalSpeed, 0.0f, 0x1.47ae16p-4f); /* 0.1f * 0.8f (HD: HIO folded, rounded) */
        }
    } else {
        cLib_chaseF(&mNormalSpeed, 17.0f, 0.4f);
    }
    checkNextActionItemFly();
    if (gabi::call<BOOL>(0x023F13A0 /* checkJumpFlower */, this) || checkJumpRideShip()) {
        return TRUE;
    }
    if (mAcch.ChkGroundHit()) {
        return changeLandProc(1.3f);
    }
    if (checkFanGlideProc(0)) {
        return TRUE;
    }
    u16 anm = gabi::load<u16>(gabi::ea(this) + 0x5888);
    if (anm != 0x95 && anm != 0x96 /* !checkGrabAnime() */ &&
        (gabi::call<BOOL>(0x02418E00 /* changeFrontWallTypeProc */, this) || changeRopeSwingProc() || checkJumpCutFromButton() ||
         gabi::call<BOOL>(LK_checkItemChangeFromButton, this))) {
        return TRUE;
    }
    m34C2 = 1;
    s16 pv0 = mProcVar0;
    if (pv0 > 0) {
        mProcVar0 = (s16)(pv0 - 1);
        dComIfGp_setDoStatus_l(6 /* dActStts_LET_GO_e */);
        if ((mAcch.m_flags & 0x10) || gabi::call<BOOL>(0x02516464 /* dCcD_GObjInf::ChkCoHit */, &mCyl)) {
            mProcVar1 = mProcVar1 - 1;
        }
        if (LK_doTrigger()) {
            u32 grab = gabi::ea(mActorKeepGrab.mActor.get());
            if (grab != 0) {
                fcpy_l(grab + 0x370, gabi::ea(&speedF));
            }
            gabi::call(0x023DCF8C /* freeGrabItem */, this);
            mProcVar0 = 0;
            gravity = -2.5f;
        } else if (mProcVar1 == 0) {
            gabi::call(0x023DCF8C /* freeGrabItem */, this);
            mProcVar0 = 0;
            gravity = -2.5f;
        } else if (mProcVar0 == 0) {
            gravity = -2.5f; /* HD: HIO folded */
        }
        f32 sy = speed.y;
        if (sy > 0.0f) {
            f32 n = sy - 0.5f;
            if (n > 0.0f) {
                speed.y = n;
            } else {
                speed.y = 0.0f;
            }
        }
        setFallVoice();
        return TRUE;
    }
    if (speed.y < -gravity && mProcVar6 != 2) {
        u32 grab = gabi::ea(mActorKeepGrab.mActor.get());
        if (grab != 0 && gabi::load<s16>(grab + 8) == 0x16F /* fpcNm_NPC_MD_e */ && dComIfGs_isEventBit_l(0x1620)) {
            gravity = 0.0f;
            mProcVar0 = 0x64;
            speed.y = 5.0f;
        }
        gabi::call(LK_setSingleMoveAnime, this, 0xD /* ANM_JMPEDS */, 0.0f, 0.0f, -1, 20.0f);
        m34C2 = 2;
        mProcVar6 = 2;
        mModeFlg = mModeFlg & ~0x400u;
        gabi::call(0x023DD768 /* setTextureAnime */, this, 0x37, 0);
        resetSeAnime();
    } else if (m3688.y > current.pos.y && gabi::load<u16>(gabi::ea(this) + 0x65D0) != 0x22B /* m_tex_anm_heap.mIdx */) {
        gabi::call(0x023DD768 /* setTextureAnime */, this, 0x37, 0);
    }
    setFallVoice();
    return TRUE;
}
VERIFY(0x0241D79C, &daPy_lk_c::procAutoJump);

/* 0241DB84 */
BOOL daPy_lk_c::procLand() {
    WWHD_FUNC(0x0241DB84, BOOL, this);
    gabi::call(LK_resetFootEffect, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (mDemoMode == 0x40 /* daPy_demo_c::DEMO_SFALL_e */) {
            LK_cutEnd();
        } else {
            LK_checkNextMode(0);
        }
    } else if (mFrameCtrlUnder[0].getFrame() > 7.0f /* HD: HIO folded */) {
        LK_checkNextMode(1);
    }
    return TRUE;
}
VERIFY(0x0241DB84, &daPy_lk_c::procLand);

/* 0241DC30 */
BOOL daPy_lk_c::procLandDamage() {
    WWHD_FUNC(0x0241DC30, BOOL, this);
    gabi::call(LK_resetFootEffect, this);
    s32 pv6 = mProcVar6;
    f32 rate = mFrameCtrlUnder[0].getRate();
    if (pv6 != 0) {
        if (rate < 0.01f) {
            LK_checkNextMode(0);
        } else if (mFrameCtrlUnder[0].getFrame() > 9.0f /* HD: HIO folded */) {
            LK_checkNextMode(1);
        }
    } else if (rate < 0.01f) {
        s16 pv0 = mProcVar0;
        if (pv0 > 0) {
            mProcVar0 = (s16)(pv0 - 1);
        } else {
            f32 r = (mModeFlg & 8 /* ModeFlg_DAMAGE */) ? 0.3f : 1.2f;
            gabi::call(LK_setSingleMoveAnime, this, 0x31 /* ANM_LANDDAMAST */, r, 1.0f, 9, 5.0f);
            mProcVar6 = 1;
        }
    }
    return TRUE;
}
VERIFY(0x0241DC30, &daPy_lk_c::procLandDamage);

/* 0241DD5C */
BOOL daPy_lk_c::procFall() {
    WWHD_FUNC(0x0241DD5C, BOOL, this);
    if (mProcVar6 == 0 && m3688.y > current.pos.y && gabi::load<u16>(gabi::ea(this) + 0x65D0) != 0x22B /* m_tex_anm_heap.mIdx */) {
        gabi::call(0x023DD768 /* setTextureAnime */, this, 0x37, 0);
        mProcVar6 = 1;
    }
    if (mProcVar3 != 0) {
        cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.2f, 1.0f, 0.1f);
    }
    checkNextActionItemFly();
    if (gabi::call<BOOL>(0x023F13A0 /* checkJumpFlower */, this) || (mProcVar7 != 0 && checkJumpRideShip())) {
        return TRUE;
    }
    if (mAcch.ChkGroundHit()) {
        changeLandProc(1.3f);
    } else {
        if (checkFanGlideProc(0)) {
            return TRUE;
        }
        setFallVoice();
        s16 pv0 = mProcVar0;
        if (pv0 == 1) {
            s16 pv1 = mProcVar1;
            if (pv1 > 0) {
                pv1 = (s16)(pv1 - 1);
                mProcVar1 = pv1;
            }
            if (mStickDistance > 0.05f && getDirectionFromShapeAngle() == 0 && pv1 == 0 &&
                gabi::call<BOOL>(0x02418E00 /* changeFrontWallTypeProc */, this)) {
                return TRUE;
            }
        } else if (pv0 == 2) {
            if (gabi::call<BOOL>(0x02418E00 /* changeFrontWallTypeProc */, this) || changeRopeSwingProc()) {
                return TRUE;
            }
        }
        if (mProcVar2 != 0 && checkJumpCutFromButton()) {
            return TRUE;
        }
    }
    gabi::call(LK_checkItemChangeFromButton, this);
    return TRUE;
}
VERIFY(0x0241DD5C, &daPy_lk_c::procFall);

/* 0241DF20 */
BOOL daPy_lk_c::procSlowFall() {
    WWHD_FUNC(0x0241DF20, BOOL, this);
    if (current.pos.y - mAcch.GetGroundH() < 200.0f) {
        cLib_chaseF(gabi::at<be<f32>>(gabi::ea(this) + 0x378) /* maxFallSpeed */, -5.0f, 1.0f);
    }
    if (mAcch.ChkGroundHit()) {
        procLand_init(0.6f, 1); /* HD: HIO folded */
    }
    return TRUE;
}
VERIFY(0x0241DF20, &daPy_lk_c::procSlowFall);

/* 0241DFA0 */
BOOL daPy_lk_c::procSmallJump() {
    WWHD_FUNC(0x0241DFA0, BOOL, this);
    if (mAcch.ChkGroundHit()) {
        changeLandProc(1.3f);
    } else if (checkFanGlideProc(0)) {
        return TRUE;
    } else if (speed.y < -gravity) {
        gabi::call(LK_procFall_init, this, (s32)mProcVar6, 7.0f); /* HD: HIO folded */
        gabi::call(0x023DD768 /* setTextureAnime */, this, 0x37, 0);
    }
    return TRUE;
}
VERIFY(0x0241DFA0, &daPy_lk_c::procSmallJump);

/* 0241E034 */
BOOL daPy_lk_c::procVerticalJump() {
    WWHD_FUNC(0x0241E034, BOOL, this);
    cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 2, 0x1000, 0x400);
    current.angle.y = shape_angle.y;
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        s16 a = mProcVar2;
        current.angle.y = a;
        shape_angle.y = a;
        BOOL r;
        if (mProcVar6 != 0) {
            r = gabi::call<BOOL>(0x02417D30 /* procHangStart_init */, this);
        } else {
            r = gabi::call<BOOL>(0x02418B48 /* procHangWallCatch_init */, this);
        }
        if (r == 0) {
            gabi::call(0x023E2FF4 /* procWait_init */, this);
        }
    }
    return TRUE;
}
VERIFY(0x0241E034, &daPy_lk_c::procVerticalJump);

/* 0241E0D4 */
BOOL daPy_lk_c::procGuardCrash() {
    WWHD_FUNC(0x0241E0D4, BOOL, this);
    cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.5f, 1.25f, 0.25f);
    s16 pv0 = mProcVar0;
    if (pv0 > 0) {
        mProcVar0 = (s16)(pv0 - 1);
    } else {
        LK_checkNextMode(0);
    }
    return TRUE;
}
VERIFY(0x0241E0D4, &daPy_lk_c::procGuardCrash);

/* 0241E150 */
BOOL daPy_lk_c::procDamage() {
    WWHD_FUNC(0x0241E150, BOOL, this);
    f32 dVar6 = mFrameCtrlUnder[0].getFrame() - 1.5f; /* HD: HIO folded */
    cLib_addCalcAngleS(&m3564.y, 0, 4, 0x800, 0x100);
    f32 fVar1, fVar2;
    if (dVar6 < 0.0f) {
        s16 u = (s16)gabi::ftoi((16384.0f * mFrameCtrlUnder[0].getFrame()) / 1.5f);
        fVar1 = cM_ssin(u);
        if (u < 0x2000) {
            fVar2 = cM_ssin((u << 1) & 0xFFFE);
        } else {
            fVar2 = cM_ssin(0x4000);
        }
    } else {
        f32 len = (f32)mFrameCtrlUnder[0].getEnd() - 1.5f;
        s16 u = (s16)gabi::ftoi(16384.0f * (1.0f - dVar6 / len));
        fVar1 = 1.0f - cM_scos(u);
        s16 u2 = u < 0x2000 ? 0 : (s16)((u << 1) - 0x4000);
        fVar2 = 1.0f - cM_scos(u2);
    }
    f32 pv2 = (f32)(s16)mProcVar2;
    f32 pv3 = (f32)(s16)mProcVar3;
    s16 ax = (s16)gabi::ftoi(pv2 * fVar1);
    s16 az = (s16)gabi::ftoi(pv3 * fVar1);
    s16 bx = (s16)gabi::ftoi(pv2 * fVar2);
    s16 bz = (s16)gabi::ftoi(pv3 * fVar2);
    m3564.x = ax;
    m3564.y = 0;
    mBodyAngleX = bx;
    m3564.z = az;
    LK_FIELD(s16, 0x3D4) /* mBodyAngle.z */ = bz;
    mBodyAngleY = 0;
    f64 r = cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.5f, 1.2f, 0.25f); /* HD: HIO folded */
    if (!(r > 0.001f) && mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_checkNextMode(0);
    } else if (mNormalSpeed < 12.0f) {
        gabi::call(LK_resetFootEffect, this);
    }
    return TRUE;
}
VERIFY(0x0241E150, &daPy_lk_c::procDamage);

/* 0241E464 */
BOOL daPy_lk_c::procLargeDamageWall_init(int param_1, int param_2, s16 param_3, s16 param_4) {
    WWHD_FUNC(0x0241E464, BOOL, this, param_1, param_2, param_3, param_4);
    if (mCurProc == 0x6A /* daPyProc_LARGE_DAMAGE_WALL_e */) {
        return FALSE;
    }
    u32 pla = gabi::call<u32>(0x020084C8 /* cBgS::GetTriPla */, dComIfG_Bgsp(), (u32)gabi::load<u16>(gabi::ea(this) + 0x9E6),
                              (u32)gabi::load<u16>(gabi::ea(this) + 0x9E4));
    if (pla == 0) { /* HD */
        return FALSE;
    }
    s16 sVar6 = cM_atan2s(gabi::load<f32>(pla + 0), gabi::load<f32>(pla + 8));
    if (param_1 < 0) {
        return FALSE;
    }
    f32 ny = gabi::load<f32>(pla + 4);
    if (!(ny < 0.5f) || ny < -0.8f) { /* !cBgW_CheckBWall(ny) */
        return FALSE;
    }
    if (cLib_distanceAngleS(sVar6, (s16)(current.angle.y - 0x8000)) > 0x1555) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x6A);
    gabi::Local<cXyz> xz;
    fcpy_l(gabi::ea(&xz->z), pla + 8);
    fcpy_l(gabi::ea(&xz->x), pla + 0);
    xz->y = 0.0f;
    f64 len = std_sqrtf_d(PSVECSquareMag_d(xz));
    s16 sVar7 = gabi::call<s16>(0x020195B0 /* cM_atan2s */, gabi::load<f32>(pla + 4), len);
    fcpy_l(gabi::ea(&current.pos.x), gabi::ea(this) + 0xA00); /* mLinkLinChk.GetCrossP() */
    fcpy_l(gabi::ea(&current.pos.z), gabi::ea(this) + 0xA08);
    current.angle.y = sVar6;
    gabi::call(0x023E278C /* setOldRootQuaternion */, this, (s32)param_3, 0, (s32)param_4);
    /* HD: HIO folded (the same values for param_2 on and off) */
    if (param_1 == 0x5C) {
        mProcVar6 = 0x60; /* ANM_DAMFBUP */
        m34F2 = (s16)(sVar7 - 0x4000);
        mProcVar0 = 0;
        gabi::call(LK_setSingleMoveAnime, this, 0x60, 1.2f, 0.0f, 2, 1.0f);
        mNormalSpeed = 0.0f;
        speed.y = 0.0f;
        gravity = 0.0f;
    } else if (param_1 == 0x59) {
        mProcVar6 = 0x5D; /* ANM_DAMFLUP */
        m34F4 = (s16)(sVar7 - 0x4000);
        mProcVar0 = 1;
        gabi::call(LK_setSingleMoveAnime, this, 0x5D, 1.3f, 0.0f, 2, 1.0f);
        mNormalSpeed = 0.0f;
        speed.y = 0.0f;
        gravity = 0.0f;
    } else if (param_1 == 0x5A) {
        mProcVar6 = 0x5E; /* ANM_DAMFRUP */
        s32 anm = mProcVar6;
        m34F4 = (s16)(0x4000 - sVar7);
        mProcVar0 = 1;
        gabi::call(LK_setSingleMoveAnime, this, anm, 1.3f, 0.0f, 2, 1.0f);
        mNormalSpeed = 0.0f;
        gravity = 0.0f;
        speed.y = 0.0f;
    } else {
        mProcVar6 = 0x5F; /* ANM_DAMFFUP */
        mProcVar0 = 0;
        m34F2 = (s16)(0x4000 - sVar7);
        gabi::call(LK_setSingleMoveAnime, this, 0x5F, 1.5f, 18.0f, 0x13, 1.0f);
        mNormalSpeed = 0.0f;
        speed.y = 0.0f;
        gravity = 0.0f;
    }
    {
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        dComIfGp_getVibration_StartShock(6, -0x31, v);
    }
    mProcVar0 = (s16)param_2;
    return TRUE;
}
VERIFY(0x0241E464, &daPy_lk_c::procLargeDamageWall_init);

/* 0241E7D4 */
BOOL daPy_lk_c::procLargeDamage() {
    WWHD_FUNC(0x0241E7D4, BOOL, this);
    s16 pv0 = mProcVar0;
    if (pv0 > 0) {
        pv0 = (s16)(pv0 - 1);
        mProcVar0 = pv0;
        if (pv0 != 0) {
            return TRUE;
        }
        mNormalSpeed = 25.0f; /* HD: HIO folded */
        speed.y = 60.0f;
    }
    s16 pv3 = mProcVar3;
    s16 pv2 = mProcVar2;
    s16 pv4 = mProcVar4;
    if (pv3 & 4) {
        cLib_chaseAngleS(&m34F2, pv2, pv4);
    } else {
        cLib_chaseAngleS(&m34F4, pv2, pv4);
    }
    pv3 = mProcVar3;
    if (!(pv3 & 1)) {
        mProcVar3 = (s16)(pv3 | 1);
        return TRUE;
    }
    u32 acchFlags = mAcch.m_flags;
    if (acchFlags & 0x20 /* ChkGroundHit() */) {
        if (gabi::call<BOOL>(LK_changeSlideProc, this)) {
            mNormalSpeed = 0.0f;
            return TRUE;
        }
        pv3 = mProcVar3;
        if (pv3 & 2) {
            LK_voiceStart(2);
            mDoAud_seStartSystem_l(0x842 /* JA_SE_MAJUTOU_JAIL_DOOR */);
            mAcch.m_flags = mAcch.m_flags & ~0x4004u; /* ClrWallNone(), OffLineCheckNone() */
            pv3 = mProcVar3;
        }
        s16 a2 = m34F2;
        s32 anm = mProcVar6;
        s16 a4 = m34F4;
        gabi::call(0x023F6020 /* procLargeDamageUp_init */, this, anm, (s32)((pv3 >> 3) & 1), (s32)a2, (s32)a4);
        return TRUE;
    }
    if ((acchFlags & 0x10 /* ChkWallHit() */) && mNormalSpeed > m35A0) {
        for (int i = 0; i < 3; i++) {
            u32 cir = gabi::ea(&mAcchCir[0]) + i * 0x40;
            if (!(gabi::load<u32>(cir + 0x10) & 2 /* ChkWallHit() */)) {
                continue;
            }
            gabi::Local<cXyz> start, end;
            f32 wallH = gabi::load<f32>(cir + 0x30);
            f32 py = current.pos.y;
            f32 pz = current.pos.z;
            f32 y = py + wallH;
            u16 ang = current.angle.y;
            f32 px = current.pos.x;
            start->y = y;
            fcpy_l(gabi::ea(&start->x), gabi::ea(&current.pos.x));
            fcpy_l(gabi::ea(&start->z), gabi::ea(&current.pos.z));
            f32 r = gabi::load<f32>(cir + 0x34) + 25.0f;
            end->y = y;
            end->x = gabi::fmadds(cM_ssin(ang), r, px);
            end->z = gabi::fmadds(cM_scos(ang), r, pz);
            gabi::call(0x024F1AFC /* dBgS_LinChk::Set */, gabi::ea(this) + 0x9D0, start.get(), end.get(), this);
            if (gabi::call<BOOL>(0x02008860 /* cBgS::LineCross */, dComIfG_Bgsp(), gabi::ea(this) + 0x9D0)) {
                s16 p3 = mProcVar3;
                s16 a2 = m34F2;
                s32 anm = mProcVar6;
                s16 a4 = m34F4;
                return procLargeDamageWall_init(anm, (p3 >> 3) & 1, a2, a4);
            }
        }
    }
    return TRUE;
}
VERIFY(0x0241E7D4, &daPy_lk_c::procLargeDamage);

/* 0241EA50 */
BOOL daPy_lk_c::procLargeDamageUp() {
    WWHD_FUNC(0x0241EA50, BOOL, this);
    m35E4 = (m35A0 - mFrameCtrlUnder[0].getFrame()) * m35A4;
    gabi::call(LK_resetFootEffect, this);
    if (mProcVar0 > 0) {
        s16 pv0;
        if (!gabi::call<BOOL>(0x025445B8 /* dEvent_manager_c::startCheckOld */, dComIfGp_getPEvtManager(), STR(0x10035FC4) /* "ICE_FAILED" */)) {
            pv0 = (s16)(mProcVar0 - 1);
            mProcVar0 = pv0;
        } else {
            LK_cutEnd();
            pv0 = mProcVar0;
        }
        if (pv0 == 0 || (mProcVar6 == -4 && gabi::call<f64>(0x020079B4 /* CPad_GET_STICK_VALUE */, 0) > 0.05f)) {
            mFrameCtrlUnder[0].setRate(0.5f);
        }
    } else if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (dComIfGp_event_runCheck_l()) {
            LK_cutEnd();
        } else {
            LK_checkNextMode(0);
        }
    } else if (mFrameCtrlUnder[0].getFrame() > m35A0) {
        LK_checkNextMode(1);
    }
    return TRUE;
}
VERIFY(0x0241EA50, &daPy_lk_c::procLargeDamageUp);

/* 0241EBB4 */
BOOL daPy_lk_c::procLargeDamageWall() {
    WWHD_FUNC(0x0241EBB4, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        gabi::call(0x023F54E8 /* procLargeDamage_init */, this, (s32)mProcVar6, (s32)mProcVar0, (s32)m34F2, (s32)m34F4);
    }
    return TRUE;
}
VERIFY(0x0241EBB4, &daPy_lk_c::procLargeDamageWall);

/* 0241EBFC */
BOOL daPy_lk_c::procLavaDamage() {
    WWHD_FUNC(0x0241EBFC, BOOL, this);
    if (mAcch.ChkGroundHit()) {
        int direction = getDirectionFromAngle((s16)(current.angle.y - shape_angle.y));
        int anm;
        if (direction == 0 /* DIR_FORWARD */) {
            anm = 0x5C; /* ANM_DAMFB */
        } else if (direction == 1 /* DIR_BACKWARD */) {
            anm = 0x5B; /* ANM_DAMFF */
        } else if (direction == 2 /* DIR_LEFT */) {
            anm = 0x5A; /* ANM_DAMFR */
        } else {
            anm = 0x59; /* ANM_DAMFL */
        }
        gabi::call(0x023F6020 /* procLargeDamageUp_init */, this, anm, 1, 0, 0);
    }
    return TRUE;
}
VERIFY(0x0241EBFC, &daPy_lk_c::procLavaDamage);

/* 0241ECB0 */
BOOL daPy_lk_c::procElecDamage() {
    WWHD_FUNC(0x0241ECB0, BOOL, this);
    gabi::call(0x023E2DC4 /* setShipRidePosUseItem */, this);
    s16 pv0 = mProcVar0;
    if (pv0 > 0) {
        mProcVar0 = (s16)(pv0 - 1);
    } else {
        u32 ev = dComIfGp_ea() + 0x52B8; /* dComIfGp_event_reset() */
        gabi::store<u16>(ev, gabi::load<u16>(ev) | 8);
        gabi::call(0x023F2048 /* endDemoMode */, this);
        if (mProcVar3 == 0 && mAcch.ChkGroundHit()) {
            LK_checkNextMode(0);
        } else {
            if (mProcVar3 != 0) {
                current.angle.y = cM_atan2s(m370C.x, m370C.z);
            } else {
                current.angle.y = (s16)(shape_angle.y - 0x8000);
            }
            gabi::call(0x023F54E8 /* procLargeDamage_init */, this, -10, 1, 0, 0);
        }
    }
    setResetFlg0(resetFlg0() | 0x80000000); /* HD */
    return TRUE;
}
VERIFY(0x0241ECB0, &daPy_lk_c::procElecDamage);

/* 0241EDB8 */
BOOL daPy_lk_c::procGuardSlip() {
    WWHD_FUNC(0x0241EDB8, BOOL, this);
    s16 pv0 = mProcVar0;
    if (pv0 > 0) {
        mProcVar0 = (s16)(pv0 - 1);
    }
    gabi::call(0x02419C70 /* setShapeAngleToAtnActor */, this);
    if (mEquipItem == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        fcpy_l(gabi::ea(&m35EC), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    } else {
        fcpy_l(gabi::ea(&m35E8), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    }
    f64 r = cLib_addCalc_l(&mNormalSpeed, 0.0f, m35A0, m35A4, m35A8);
    if (!(r > 0.001f)) {
        if (mProcVar6 != 0) {
            return procNockBackEnd_init();
        }
        if (mProcVar0 == 0) {
            /* HD: m_pbCalc[PART_UPPER_e]: every blend ratio of the upper body set to 1.0 */
            u32 pb = m_pbCalc[1];
            u32 res = gabi::call<u32>(0x027F3F94 /* matcher: __nw (an offset-pointer accessor) */, gabi::load<u32>(gabi::load<u32>(pb + 0x80) + 0xAC));
            s32 n = gabi::call<s32>(0x027E0174, res, 0);
            for (s32 i = 0; i < n; i++) {
                gabi::store<f32>(gabi::load<u32>(gabi::load<u32>(pb + 0x7C) + 0x2C) + i * 4, 1.0f);
            }
            LK_checkNextMode(0);
        }
    } else if (mNormalSpeed < m35AC) {
        gabi::call(LK_resetFootEffect, this);
    }
    return TRUE;
}
VERIFY(0x0241EDB8, &daPy_lk_c::procGuardSlip);

/* 0241EF2C */
BOOL daPy_lk_c::procIceSlipFallUp_init(int param_1, s16 param_2, s16 param_3) {
    WWHD_FUNC(0x0241EF2C, BOOL, this, param_1, param_2, param_3);
    gabi::call(LK_commonProcInit, this, 0x9F /* daPyProc_ICE_SLIP_FALL_UP_e */);
    mFootEffectPosType = 6;
    {
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->z = 0.0f;
        v->y = 1.0f;
        dComIfGp_getVibration_StartShock(3, -0x3F, v);
    }
    seStartMapInfo(0x3811 /* JA_SE_LK_FALL_DOWN */);
    /* HD: HIO folded */
    int anm;
    f32 fVar1, fVar2, fVar3, len;
    if (param_1 == 0x5C /* ANM_DAMFB */) {
        anm = 0x60; fVar1 = 0.8f; fVar2 = 3.0f; fVar3 = 2.0f; len = 33.0f;
    } else if (param_1 == 0x59 /* ANM_DAMFL */) {
        anm = 0x5D; fVar1 = 0.7f; fVar2 = 7.0f; fVar3 = 0.0f; len = 30.0f;
    } else if (param_1 == 0x5A /* ANM_DAMFR */) {
        anm = 0x5E; fVar1 = 0.7f; fVar2 = 7.0f; fVar3 = 0.0f; len = 30.0f;
    } else {
        anm = 0x5F; fVar1 = 0.7f; fVar2 = 3.0f; fVar3 = 3.0f; len = 30.0f;
    }
    m35E4 = 1.0f;
    m35A0 = len;
    m35A4 = 2.0f / (len - fVar2);
    gabi::call(LK_setSingleMoveAnime, this, anm, fVar1, fVar2, 0x24, fVar3);
    gabi::call(0x023E278C /* setOldRootQuaternion */, this, (s32)param_2, 0, (s32)param_3);
    mNormalSpeed = 0.0f;
    current.angle.y = shape_angle.y;
    return TRUE;
}
VERIFY(0x0241EF2C, &daPy_lk_c::procIceSlipFallUp_init);

/* 0241F1AC */
BOOL daPy_lk_c::procIceSlipFall() {
    WWHD_FUNC(0x0241F1AC, BOOL, this);
    s16 pv2 = mProcVar2;
    if (mProcVar3 == 1) {
        cLib_chaseAngleS(&m34F2, pv2, 0x1F40); /* HD: HIO folded */
    } else {
        cLib_chaseAngleS(&m34F4, pv2, 0x1F40);
    }
    if (mAcch.ChkGroundHit() && !gabi::call<BOOL>(LK_changeSlideProc, this)) {
        s16 a4 = m34F4;
        s32 anm = mProcVar6;
        procIceSlipFallUp_init(anm, m34F2, a4);
    }
    return TRUE;
}
VERIFY(0x0241F1AC, &daPy_lk_c::procIceSlipFall);

/* 0241F240 */
BOOL daPy_lk_c::procIceSlipFallUp() {
    WWHD_FUNC(0x0241F240, BOOL, this);
    m35E4 = (m35A0 - mFrameCtrlUnder[0].getFrame()) * m35A4;
    gabi::call(LK_resetFootEffect, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_checkNextMode(0);
    } else if (mFrameCtrlUnder[0].getFrame() > m35A0) {
        LK_checkNextMode(1);
    }
    return TRUE;
}
VERIFY(0x0241F240, &daPy_lk_c::procIceSlipFallUp);

/* 0241F2D4 */
BOOL daPy_lk_c::procIceSlipAlmostFall() {
    WWHD_FUNC(0x0241F2D4, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        current.angle.y = shape_angle.y;
        LK_checkNextMode(0);
    } else if (mFrameCtrlUnder[0].getFrame() > 6.0f /* HD: HIO folded */) {
        s16 a = shape_angle.y;
        s16 old = current.angle.y;
        current.angle.y = a;
        if (!LK_checkNextMode(1)) {
            current.angle.y = old;
        }
    }
    return TRUE;
}
VERIFY(0x0241F2D4, &daPy_lk_c::procIceSlipAlmostFall);

/* 0241F374 */
BOOL daPy_lk_c::procGrabHeavyWait_init() {
    WWHD_FUNC(0x0241F374, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x74 /* daPyProc_GRAB_HEAVY_WAIT_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x69 /* ANM_GRABWAITB */, 1.2f, 0.0f, -1, 3.0f); /* HD: HIO folded */
    gabi::call(0x023DE7E8 /* setActAnimeUpper */, this, 0x96 /* dRes_INDEX_LKANM_BCK_GRABWAITB_e */, 2 /* UPPER_MOVE2_e */, 0.0f, 0.0f, -1, -1.0f);
    /* HD: m_pbCalc[PART_UPPER_e]: every blend ratio of the upper body set to 0.0 */
    u32 pb = m_pbCalc[1];
    u32 res = gabi::call<u32>(0x027F3F94 /* matcher: __nw (an offset-pointer accessor) */, gabi::load<u32>(gabi::load<u32>(pb + 0x80) + 0xAC));
    s32 n = gabi::call<s32>(0x027E0174, res, 0);
    for (s32 i = 0; i < n; i++) {
        gabi::store<f32>(gabi::load<u32>(gabi::load<u32>(pb + 0x7C) + 0x2C) + i * 4, 0.0f);
    }
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    m35A0 = 0.0f;
    return TRUE;
}
VERIFY(0x0241F374, &daPy_lk_c::procGrabHeavyWait_init);

/* HD m_pbCalc[PART_UPPER_e]->setRatio(..): every blend ratio of the upper body set to v (count from
 * the animation resource; 027F3F94 is an offset-pointer accessor the matcher calls __nw) */
static inline void lk_setUpperRatioAll_l(u32 pb, f32 v) {
    u32 res = gabi::call<u32>(0x027F3F94, gabi::load<u32>(gabi::load<u32>(pb + 0x80) + 0xAC));
    s32 n = gabi::call<s32>(0x027E0174, res, 0);
    for (s32 i = 0; i < n; i++) {
        gabi::store<f32>(gabi::load<u32>(gabi::load<u32>(pb + 0x7C) + 0x2C) + i * 4, v);
    }
}

/* 0241F470 */
void daPy_lk_c::initGrabNextMode() {
    WWHD_FUNC(0x0241F470, void, this);
    u32 grab = gabi::ea(mActorKeepGrab.mActor.get());
    if (gabi::load<u32>(grab + 0x2E0) & 0x10000 /* fopAcStts_UNK10000_e */) {
        if (!gabi::call<BOOL>(0x023DBBE8 /* checkGrabSpecialHeavyState */, this)) {
            procGrabHeavyWait_init();
            return;
        }
        u16 anm = gabi::load<u16>(gabi::ea(this) + 0x5888);
        if (anm != 0x95 && anm != 0x96) {
            gabi::call(0x023DE7E8 /* setActAnimeUpper */, this, 0x96 /* GRABWAITB */, 2, 0.0f, 0.0f, -1, -1.0f);
        }
    } else {
        u16 anm = gabi::load<u16>(gabi::ea(this) + 0x5888);
        if (anm != 0x95 && anm != 0x96) {
            gabi::call(0x023DE7E8 /* setActAnimeUpper */, this, 0x95 /* GRABWAIT */, 2, 0.0f, 0.0f, -1, -1.0f);
        }
    }
    lk_setUpperRatioAll_l(m_pbCalc[1], 1.0f);
    LK_checkNextMode(0);
}
VERIFY(0x0241F470, &daPy_lk_c::initGrabNextMode);

/* 0241F618 */
BOOL daPy_lk_c::procBootsEquip() {
    WWHD_FUNC(0x0241F618, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    /* HD: the pass frame is a global (0x1047C04C) + 14 */
    if (frameCtrl.checkPass(gabi::load<f32>(0x1047C04C) + 14.0f)) {
        u32 flg = mNoResetFlg0;
        if (flg & 0x2000000 /* daPyFlg0_EQUIP_HEAVY_BOOTS */) {
            mNoResetFlg0 = flg & ~0x2000000u;
        } else {
            mNoResetFlg0 = flg | 0x2000000;
        }
    }
    if (frameCtrl.getRate() < 0.01f) {
        u16 anm = gabi::load<u16>(gabi::ea(this) + 0x5888);
        if (anm == 0x95 || anm == 0x96) {
            initGrabNextMode();
        } else {
            LK_checkNextMode(0);
        }
    } else if (frameCtrl.getFrame() > 19.0f) {
        u16 anm = gabi::load<u16>(gabi::ea(this) + 0x5888);
        if (anm == 0x95 || anm == 0x96) {
            if (mStickDistance > 0.05f) {
                initGrabNextMode();
            }
        } else {
            LK_checkNextMode(1);
        }
    } else if (frameCtrl.checkPass(15.0f) && (mNoResetFlg0 & 0x2000000)) {
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x31, v);
    }
    return TRUE;
}
VERIFY(0x0241F618, &daPy_lk_c::procBootsEquip);

/* HD message manager *(0x101F4B5C): status at play + 0x5BB2 (025F795C, matcher: fopMsgM_SearchByID),
 * set status 025F74D0, fopMsgM_messageSet 025F7DB0 */
static inline u32 lk_msgMgr_l() { return gabi::load<u32>(0x101F4B5C); }

/* 0241F79C */
BOOL daPy_lk_c::procNotUse() {
    WWHD_FUNC(0x0241F79C, BOOL, this);
    if (mProcVar2 == 0) {
        s32 item = mProcVar6;
        if (!checkBottleItem((u16)item)) {
            u32 id = gabi::call<u32>(0x025D7DEC /* fopAcM_createItemForPresentDemo */, &current.pos, item, 5, -1,
                                     (s32)(s8)current.roomNo, &shape_angle, &scale);
            gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); /* dComIfGp_event_setItemPartnerId() */
            mProcVar2 = 1;
        }
    }
    if (!(mFrameCtrlUnder[0].getRate() < 0.01f)) {
        return TRUE;
    }
    u32 item = gabi::call<u32>(0x025D7C98 /* fopAcM_getItemEventPartner */, this);
    if (item != 0 && (gabi::load<s16>(item + 8) == 0xFF /* fpcNm_ITEM_e */ || gabi::load<s16>(item + 8) == 0x101 /* fpcNm_Demo_Item_e */)) {
        gabi::call(0x021842C8 /* daItemBase_c::show */, item);
    }
    if (mProcVar2 == 0) {
        u16 bottle = gabi::load<u16>(gabi::ea(this) + 0x69C2); /* (u16)mProcVar6 */
        if (checkBottleItem(bottle)) {
            gabi::call(0x023DF0FC /* setBottleModel */, this, (u32)bottle);
            mProcVar2 = 1;
        }
    }
    s32 gameOverId = mGameOverId;
    u32 mgr = lk_msgMgr_l();
    if (gameOverId == -1) {
        mGameOverId = gabi::call<u32>(0x025F7DB0 /* fopMsgM_messageSet */, mgr, (u32)m3624, 0);
        return TRUE;
    }
    if (gabi::call<s32>(0x025F795C, mgr) == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        if (m3624 == 0xF0C && gabi::load<s32>(mgr + 0x948) == 1 /* mSelectNum */) {
            gabi::call(0x025F74D0, mgr, 0xF /* fopMsgStts_MSG_CONTINUES_e */);
            m3624 = 0xF10;
            gabi::call<u32>(0x025F7DB0 /* fopMsgM_messageSet */, mgr, 0xF10, 0);
            gabi::call(0x025B7568 /* dComIfGs_setReserveItemEmpty */, dComIfGs_base_l() + 0x96, (u32)mReadyItemBtn);
        } else {
            gabi::call(0x025F74D0, mgr, 0x10 /* fopMsgStts_MSG_ENDS_e */);
        }
        return TRUE;
    }
    if (gabi::call<s32>(0x025F795C, mgr) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        gabi::call(0x025F74D0, mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
        if (item != 0 && (gabi::load<s16>(item + 8) == 0xFF || gabi::load<s16>(item + 8) == 0x101)) {
            gabi::call(0x0218432C /* daItemBase_c::dead */, item);
        }
        u32 ev = dComIfGp_ea() + 0x52B8; /* dComIfGp_event_reset() */
        gabi::store<u16>(ev, gabi::load<u16>(ev) | 8);
        gabi::call(0x0253E860 /* dCamera_c::EndEventCamera */, gabi::call<u32>(0x024F8044 /* dCam_getBody */), gabi::load<u32>(gabi::ea(this) + 4));
        gabi::call(0x023F2048 /* endDemoMode */, this);
    }
    return TRUE;
}
VERIFY(0x0241F79C, &daPy_lk_c::procNotUse);

/* 0241F9DC */
void daPy_lk_c::setTalismanModel() {
    WWHD_FUNC(0x0241F9DC, void, this);
    u32 bck = gabi::call<u32>(LK_getItemAnimeResource, this, 0x10F /* dRes_INDEX_LKANM_BCK_TETOLACH_e */);
    u32 oldHeap = gabi::call<u32>(LK_setItemHeap, this);
    u32 data = gabi::call<u32>(0x023D4BB8 /* initModel */, this, gabi::ea(this) + 0x4440 /* &mpEquipItemModel */, 0x28 /* dRes_INDEX_LINK_BDL_TETOLACH_e */, 0x13000022);
    if (!gabi::call<BOOL>(0x025E8508 /* mDoExt_bckAnm::init */, gabi::ea(this) + 0x4444 /* mSwordAnim */, data, bck, 1, 2, 0.2f, 0, -1, 0)) {
        JUT_ASSERT_fail(STR(0x10035FD8), 0x52, STR(0x10035FD4));
    }
    gabi::call(0x025E3570 /* mDoExt_setCurrentHeap */, oldHeap);
}
VERIFY(0x0241F9DC, &daPy_lk_c::setTalismanModel);

/* 0241FA98 */
void daPy_lk_c::setLetterModel() {
    WWHD_FUNC(0x0241FA98, void, this);
    if (mEquipItem == 0x104 /* daPyItem_UNK104_e */) {
        return;
    }
    u32 bck = gabi::call<u32>(LK_getItemAnimeResource, this, 0x8E /* dRes_INDEX_LKANM_BCK_GETLETTERA_e */);
    u32 oldHeap = gabi::call<u32>(LK_setItemHeap, this);
    u32 data = gabi::call<u32>(0x023D4BB8 /* initModel */, this, gabi::ea(this) + 0x4440 /* &mpEquipItemModel */, 0x21 /* dRes_INDEX_LINK_BDL_LETTER_e */, 0x13000022);
    if (!gabi::call<BOOL>(0x025E8508 /* mDoExt_bckAnm::init */, gabi::ea(this) + 0x4444 /* mSwordAnim */, data, bck, 0, 2, 1.0f, 0, -1, 0)) {
        JUT_ASSERT_fail(STR(0x10035FF4), 0x78, STR(0x10035FF0));
    }
    gabi::call(0x025E3570 /* mDoExt_setCurrentHeap */, oldHeap);
    mEquipItem = 0x104;
    m35EC = 0.0f;
}
VERIFY(0x0241FA98, &daPy_lk_c::setLetterModel);

/* 0241FB78 */
void daPy_lk_c::setShapeAngleToTalkActor() {
    WWHD_FUNC(0x0241FB78, void, this);
    fopAc_ac_c* partner = gabi::call<fopAc_ac_c*>(0x025D7C6C /* fopAcM_getTalkEventPartner */, this);
    if (partner != nullptr) {
        /* HD: no special case for the AGB partner */
        s16 target = fopAcM_searchActorAngleY(this, partner);
        cLib_addCalcAngleS(&shape_angle.y, target, 4, 0x1000, 0x200); /* HD: HIO folded */
    }
}
VERIFY(0x0241FB78, &daPy_lk_c::setShapeAngleToTalkActor);

/* 0241FBCC */
BOOL daPy_lk_c::checkEndMessage(u32 i_msgNo) {
    WWHD_FUNC(0x0241FBCC, BOOL, this, i_msgNo);
    s32 gameOverId = mGameOverId;
    u32 mgr = lk_msgMgr_l();
    if (gameOverId == -1) {
        if (gabi::call<s32>(0x025F7DB0 /* fopMsgM_messageSet */, mgr, i_msgNo, 0) != -1) {
            mGameOverId = 0; /* HD: a flag, not the message id */
        }
        return FALSE;
    }
    if (gabi::call<s32>(0x025F795C, mgr) == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        gabi::call(0x025F74D0, mgr, 0x10 /* fopMsgStts_MSG_ENDS_e */);
        return FALSE;
    }
    if (gabi::call<s32>(0x025F795C, mgr) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        gabi::call(0x025F74D0, mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
        return TRUE;
    }
    s32 st = gabi::call<s32>(0x025F795C, mgr);
    if (st == 1 /* fopMsgStts_MSG_PREPARING_e */ && i_msgNo == 0x14A2) {
        gabi::call(0x025DB58C /* fopMsgM_demoMsgFlagOn */, st);
    }
    return FALSE;
}
VERIFY(0x0241FBCC, &daPy_lk_c::checkEndMessage);

/* 0241FCB4 */
void daPy_lk_c::setDemoTextureAnime(u16 btpIdx, u16 btkIdx, int r30, u16 r31) {
    WWHD_FUNC(0x0241FCB4, void, this, btpIdx, btkIdx, r30, r31);
    u32 a = gabi::ea(this);
    if (gabi::load<u16>(a + 0x65D4) != btpIdx || gabi::load<u16>(a + 0x65D6) != r31) {
        gabi::store<u16>(a + 0x65D4, btpIdx); /* m_tex_anm_heap */
        gabi::store<u16>(a + 0x65D6, r31);
        u32 res = gabi::call<u32>(0x023DC38C /* loadTextureAnimeResource */, this, (u32)btpIdx, 1);
        gabi::call(0x023DC110 /* setTextureAnimeResource */, this, res, r30);
    }
    if (gabi::load<u16>(a + 0x65E4) != btkIdx || gabi::load<u16>(a + 0x65E6) != r31) {
        gabi::store<u16>(a + 0x65E4, btkIdx); /* m_tex_scroll_heap */
        gabi::store<u16>(a + 0x65E6, r31);
        u32 res = gabi::call<u32>(0x023DC4DC /* loadTextureScrollResource */, this, (u32)btkIdx, 1);
        gabi::call(0x023DC43C /* setTextureScrollResource */, this, res, r30);
    }
}
VERIFY(0x0241FCB4, &daPy_lk_c::setDemoTextureAnime);

/* 02421148 */
BOOL daPy_lk_c::dProcDamage_init() {
    WWHD_FUNC(0x02421148, BOOL, this);
    if (mCurProc == 0xAB /* daPyProc_DEMO_DAMAGE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xAB);
    gabi::call(LK_setSingleMoveAnime, this, 0x57 /* ANM_DAMF */, 0.6f, 0.0f, 9, 0.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    m35A0 = 0.34906587f; /* M_PI / (9 - 0) */
    return TRUE;
}
VERIFY(0x02421148, &daPy_lk_c::dProcDamage_init);

/* 02421200 */
BOOL daPy_lk_c::dProcDamage() {
    WWHD_FUNC(0x02421200, BOOL, this);
    /* cM_fsin(x) = cM_ssin(cM_rad2s(x)) */
    s16 a = gabi::call<s16>(0x02019510 /* cM_rad2s */, m35A0 * mFrameCtrlUnder[0].getFrame());
    mNormalSpeed = -2.0f * cM_ssin(a);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02421200, &daPy_lk_c::dProcDamage);

/* 0242128C */
BOOL daPy_lk_c::dProcHoldup_init() {
    WWHD_FUNC(0x0242128C, BOOL, this);
    if (mCurProc == 0xAC /* daPyProc_DEMO_HOLDUP_e */) {
        return FALSE;
    }
    u32 whide = (mModeFlg >> 4) & 1; /* checkModeFlg(ModeFlg_WHIDE) */
    gabi::call(LK_commonProcInit, this, 0xAC);
    /* HD: HIO folded */
    if (whide) {
        int anm = (mNoResetFlg0 & 0x10000) ? 0x93 /* ANM_WALLHOLDUPDW */ : 0x92 /* ANM_WALLHOLDUP */;
        mModeFlg = mModeFlg | 0x10;
        gabi::call(0x023DFB18 /* setBgCheckParam */, this);
        gabi::call(LK_setSingleMoveAnime, this, anm, 1.0f, 0.0f, 0x3C, 5.0f);
    } else {
        gabi::call(LK_setSingleMoveAnime, this, 0x91 /* ANM_HOLDUP */, 1.1f, 0.0f, 0xB, 5.0f);
    }
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x0242128C, &daPy_lk_c::dProcHoldup_init);

/* 024213E4 */
BOOL daPy_lk_c::dProcHoldup() {
    WWHD_FUNC(0x024213E4, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x024213E4, &daPy_lk_c::dProcHoldup);

/* 02421434 */
BOOL daPy_lk_c::dProcOpenTreasure_init() {
    WWHD_FUNC(0x02421434, BOOL, this);
    if (mCurProc == 0xAD /* daPyProc_DEMO_OPEN_TREASURE_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xAD);
    f32 dist;
    if (demoParam0() == 1) {
        dist = 75.0f;
        gabi::call(LK_setSingleMoveAnime, this, 0x8F /* ANM_BOXOPENSHORTLINK */, 1.0f, 0.0f, -1, 3.0f);
    } else {
        dist = 55.0f;
        gabi::call(LK_setSingleMoveAnime, this, 0x8E /* ANM_BOXOPENLINK */, 1.0f, 0.0f, -1, 3.0f);
    }
    mNormalSpeed = 0.0f;
    if (mEquipItem == 0x101 /* daPyItem_BOKO_e */) {
        fopAc_ac_c* boko = mActorKeepEquip.mActor;
        if (boko != nullptr) {
            PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
            mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(shape_angle.y + 0x2000));
            /* daBoko_c::setMatrix(): the model's base matrix */
            u32 model = gabi::load<u32>(gabi::ea(boko) + 0x3B4);
            if (model != 0) {
                J3DModel_setBaseTRMtx(gabi::at<J3DModel>(model), mDoMtx_stack_c::get());
            }
        }
    }
    fopAc_ac_c* partner = gabi::call<fopAc_ac_c*>(0x025D7CC4 /* fopAcM_getEventPartner */, this);
    if (partner != nullptr) {
        s16 a = (s16)(partner->shape_angle.y - 0x8000);
        shape_angle.y = a;
        current.angle.y = a;
        current.pos.x = gabi::fnmsubs(dist, cM_ssin(a), partner->current.pos.x);
        f32 c = cM_scos(a);
        current.pos.z = gabi::fnmsubs(dist, c, partner->current.pos.z);
    }
    gabi::call(LK_deleteEquipItem, this, 0);
    mAcch.m_flags = (mAcch.m_flags | 4) & ~0x2000u; /* SetWallNone(), OffLineCheck() */
    return TRUE;
}
VERIFY(0x02421434, &daPy_lk_c::dProcOpenTreasure_init);

/* 02421678 */
BOOL daPy_lk_c::dProcOpenTreasure() {
    WWHD_FUNC(0x02421678, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02421678, &daPy_lk_c::dProcOpenTreasure);

/* 024216C8 */
BOOL daPy_lk_c::dProcGetItem_init() {
    WWHD_FUNC(0x024216C8, BOOL, this);
    s16 sVar6 = 0;
    s16 sVar5 = 0;
    s32 proc = mCurProc;
    if (proc == 0xAE /* daPyProc_DEMO_GET_ITEM_e */) {
        return TRUE;
    }
    if (proc == 0xAD /* daPyProc_DEMO_OPEN_TREASURE_e */) {
        m34C2 = 10;
        mAcch.m_flags = mAcch.m_flags & ~4u; /* ClrWallNone() */
        if (gabi::load<u16>(gabi::ea(this) + 0x5848) != 0x38 /* m_anm_heap_under[UNDER_MOVE0_e].mIdx != BOXOPENLINK */) {
            sVar5 = 1;
        }
    } else if (proc == 0xD3 /* daPyProc_DEMO_OPEN_SALVAGE_TREASURE_e */) {
        m34C2 = 6;
        sVar6 = 1;
    }
    int use = gabi::call<int>(0x023E26EC /* checkShipRideUseItem */, this, 0);
    gabi::call(LK_commonProcInit, this, 0xAE);
    mNormalSpeed = 0.0f;
    s32 item;
    if (mEquipItem == 0x100 /* daPyItem_NONE_e */) {
        gabi::call(LK_setSingleMoveAnime, this, 0x90 /* ANM_ITEMGET */, 1.0f, 0.0f, -1, 3.0f);
        u32 flg = mModeFlg;
        item = demoParam0();
        mProcVar6 = 0;
        mModeFlg = flg | 0x400;
    } else {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f); /* HD: HIO folded */
        gabi::call(0x023ECEF0 /* setAnimeUnequip */, this);
        item = demoParam0();
        m3598 = 0.0f;
        mProcVar6 = 1;
    }
    if (item == 0) {
        mProcVar2 = -1;
    } else {
        if (item == 0x100) {
            item = gabi::load<u8>(dComIfGp_ea() + 0x52A4); /* dComIfGp_event_getGtItm() */
        }
        s32 id = gabi::call<s32>(0x025D7DEC /* fopAcM_createItemForPresentDemo */, &current.pos, item, 0, -1,
                                 (s32)(s8)current.roomNo, 0, 0);
        if (id != -1) {
            gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); /* dComIfGp_event_setItemPartnerId() */
        }
        mProcVar2 = (s16)item;
    }
    s32 idx = mCameraInfoIdx;
    u32 cam = gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8); /* dComIfGp_getCamera() */
    s16 sy = shape_angle.y;
    mGameOverId = 0xFFFFFFFF;
    mProcVar0 = 0;
    mProcVar3 = (s16)(sy - gabi::load<s16>(cam + 0x236) /* fopCamM_GetAngleY() */);
    current.angle.y = sy;
    mProcVar4 = sVar6;
    gabi::call(0x023E2E18 /* initShipRideUseItem */, this, use, 0);
    mProcVar7 = 0;
    m3624 = 0;
    mProcVar5 = sVar5;
    gabi::call(0x0255F378 /* dKy_Itemgetcol_chg_on */);
    return TRUE;
}
VERIFY(0x024216C8, &daPy_lk_c::dProcGetItem_init);

/* 02421D84 */
BOOL daPy_lk_c::dProcUnequip_init() {
    WWHD_FUNC(0x02421D84, BOOL, this);
    if (mCurProc == 0xAF /* daPyProc_DEMO_UNEQUIP_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xAF);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setBlendMoveAnime, this, 2.4f); /* HD: HIO folded */
    if (mEquipItem != 0x100 /* daPyItem_NONE_e */) {
        gabi::call(0x023ECEF0 /* setAnimeUnequip */, this);
        m3598 = 0.0f;
    }
    return TRUE;
}
VERIFY(0x02421D84, &daPy_lk_c::dProcUnequip_init);

/* 02421E38 */
BOOL daPy_lk_c::dProcUnequip() {
    WWHD_FUNC(0x02421E38, BOOL, this);
    if (gabi::load<u16>(gabi::ea(this) + 0x5888) == 0xFFFF /* checkNoUpperAnime() */) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02421E38, &daPy_lk_c::dProcUnequip);

/* dComIfGp_event_compulsory(actor): dEvt_control_c (play + 0x51D0)::compulsory(actor, NULL, 0xFFFF) */
static inline BOOL dComIfGp_event_compulsory_l(void* a) {
    return gabi::call<BOOL>(0x02540310, dComIfGp_ea() + 0x51D0, a, 0, 0xFFFF);
}

/* 02421E80 */
BOOL daPy_lk_c::dProcLavaDamage() {
    WWHD_FUNC(0x02421E80, BOOL, this);
    if (mProcVar6 == 1) {
        if (dComIfGp_event_compulsory_l(this)) {
            gabi::call(0x023FE62C /* dProcLavaDamage_init_sub */, this);
        }
    } else if (speed.y < -gravity) {
        gabi::call(0x023FD4E4 /* startRestartRoom */, this, 4, 0xCA, -1.0f, 1);
    }
    return TRUE;
}
VERIFY(0x02421E80, &daPy_lk_c::dProcLavaDamage);

/* 02421F1C */
BOOL daPy_lk_c::dProcFreezeDamage() {
    WWHD_FUNC(0x02421F1C, BOOL, this);
    if (mProcVar6 == 0) {
        if (dComIfGp_event_compulsory_l(this)) {
            gabi::call(0x023F5EAC /* dProcFreezeDamage_init_sub */, this, 1);
        }
    } else {
        s16 pv0 = (s16)(mProcVar0 - 1);
        mProcVar0 = pv0;
        if (pv0 == 0) {
            gabi::call(0x023FD4E4 /* startRestartRoom */, this, 5, 0xC9, -1.0f, 1);
        }
    }
    return TRUE;
}
VERIFY(0x02421F1C, &daPy_lk_c::dProcFreezeDamage);

/* 02421FBC */
BOOL daPy_lk_c::changeSwimUpProc() {
    WWHD_FUNC(0x02421FBC, BOOL, this);
    f32 sy = speed.y;
    if (sy < 9.5f) { /* HD: HIO folded */
        sy = sy + 6.0f;
        if (sy > 9.5f) {
            speed.y = 9.5f;
            if (mEquipItem != 0x100 /* daPyItem_NONE_e */) {
                gabi::call(LK_deleteEquipItem, this, 1);
            }
        } else {
            speed.y = sy;
            if (sy > 0.0f && mEquipItem != 0x100) {
                gabi::call(LK_deleteEquipItem, this, 1);
            }
        }
    }
    if (mWaterY - current.pos.y < 55.1f && !(speed.y < 0.0f)) {
        if (mCurProc == 0xB2 /* daPyProc_DEMO_DEAD_e */) {
            mNoResetFlg0 = mNoResetFlg0 | 0x100;
            fcpy_l(gabi::ea(&current.pos.y), gabi::ea(&mWaterY));
            return TRUE;
        }
        return gabi::call<BOOL>(0x023F8D60 /* procSwimUp_init */, this, 1);
    }
    return FALSE;
}
VERIFY(0x02421FBC, &daPy_lk_c::changeSwimUpProc);

/* 024224B8 */
BOOL daPy_lk_c::dProcLookAround_init() {
    WWHD_FUNC(0x024224B8, BOOL, this);
    if (mCurProc == 0xB3 /* daPyProc_DEMO_LOOK_AROUND_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xB3);
    gabi::call(LK_setSingleMoveAnime, this, 0x94 /* ANM_COMEOUT */, 1.0f, 0.0f, 0x48, 1.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x024224B8, &daPy_lk_c::dProcLookAround_init);

/* 02422564 */
BOOL daPy_lk_c::dProcLookAround() {
    WWHD_FUNC(0x02422564, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02422564, &daPy_lk_c::dProcLookAround);

/* 024225B4 */
BOOL daPy_lk_c::dProcSalute_init() {
    WWHD_FUNC(0x024225B4, BOOL, this);
    if (mCurProc == 0xB4 /* daPyProc_DEMO_SALUTE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xB4);
    gabi::call(LK_setSingleMoveAnime, this, 0x96 /* ANM_SALTATION */, 1.0f, 0.0f, -1, 3.0f);
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x024225B4, &daPy_lk_c::dProcSalute_init);

/* 02422664 */
BOOL daPy_lk_c::dProcSalute() {
    WWHD_FUNC(0x02422664, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02422664, &daPy_lk_c::dProcSalute);

/* 024226B4 */
BOOL daPy_lk_c::dProcLookAround2_init() {
    WWHD_FUNC(0x024226B4, BOOL, this);
    if (mCurProc == 0xB5 /* daPyProc_DEMO_LOOK_AROUND2_e */) {
        return FALSE;
    }
    int use = gabi::call<int>(0x023E26EC /* checkShipRideUseItem */, this, 1);
    gabi::call(LK_commonProcInit, this, 0xB5);
    gabi::call(LK_setSingleMoveAnime, this, 0x97 /* ANM_WHO */, 1.0f, 0.0f, -1, 5.0f);
    mNormalSpeed = 0.0f;
    gabi::call(0x023E2E18 /* initShipRideUseItem */, this, use, 0);
    return TRUE;
}
VERIFY(0x024226B4, &daPy_lk_c::dProcLookAround2_init);

/* 02422790 */
BOOL daPy_lk_c::dProcLookAround2() {
    WWHD_FUNC(0x02422790, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(0x023E287C /* setShipRidePos */, this, 0);
    }
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02422790, &daPy_lk_c::dProcLookAround2);

/* 024227EC */
BOOL daPy_lk_c::dProcTalismanPickup_init() {
    WWHD_FUNC(0x024227EC, BOOL, this);
    if (mCurProc == 0xB6 /* daPyProc_DEMO_TALISMAN_PICKUP_e */) {
        return FALSE;
    }
    int use = gabi::call<int>(0x023E26EC /* checkShipRideUseItem */, this, 1);
    gabi::call(LK_commonProcInit, this, 0xB6);
    gabi::call(LK_setSingleMoveAnime, this, 0x98 /* ANM_PICKUP */, 1.5f, 2.0f, 0x59, 10.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    gabi::call(0x023E2E18 /* initShipRideUseItem */, this, use, 1);
    return TRUE;
}
VERIFY(0x024227EC, &daPy_lk_c::dProcTalismanPickup_init);

/* 024228A8 */
BOOL daPy_lk_c::dProcTalismanPickup() {
    WWHD_FUNC(0x024228A8, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(0x023E287C /* setShipRidePos */, this, 1);
    }
    if (frameCtrl.getRate() < 0.01f) {
        LK_cutEnd();
    } else if (frameCtrl.checkPass(65.0f)) {
        seStartOnlyReverb(0x2810 /* JA_SE_LK_ITEM_TAKEOUT */);
    }
    return TRUE;
}
VERIFY(0x024228A8, &daPy_lk_c::dProcTalismanPickup);

/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline Mtx34* lk_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(mtx + jnt * 0x30);
}
/* the talisman's position: the right hand matrix (CL_JNT_CL_RHANDA_e = 12) moved by (7, 0, 15 + 3 sin(g_Counter.mTimer * 1512));
 * g_Counter.mTimer is at 0x101FF560 */
static inline void lk_talismanPos_l(daPy_lk_c* self) {
    PSMTXCopy(lk_getAnmMtx(self->mpCLModel, 12), mDoMtx_stack_c::get());
    u16 a = (u16)(gabi::load<u32>(0x101FF560) * 0x5E8);
    mDoMtx_stack_c::transM(7.0f, 0.0f, gabi::fmadds(3.0f, cM_ssin(a), 15.0f));
    /* mDoMtx_stack_c::multVecZero(&m338C.getPos()) */
    u32 pos = gabi::ea(self) + 0x67A8;
    fcpy_l(pos + 0, 0x1048D0CC + 0x0C);
    fcpy_l(pos + 4, 0x1048D0CC + 0x1C);
    fcpy_l(pos + 8, 0x1048D0CC + 0x2C);
}

/* 02422950 */
BOOL daPy_lk_c::dProcTalismanWait_init() {
    WWHD_FUNC(0x02422950, BOOL, this);
    if (mCurProc == 0xB7 /* daPyProc_DEMO_TALISMAN_WAIT_e */) {
        return FALSE;
    }
    int use = gabi::call<int>(0x023E26EC /* checkShipRideUseItem */, this, 1);
    gabi::call(LK_commonProcInit, this, 0xB7);
    gabi::call(LK_setSingleMoveAnime, this, 0x99 /* ANM_WAITPICKUP */, 1.0f, 0.0f, -1, 10.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    setTalismanModel();
    u32 model = mpEquipItemModel;
    gabi::call(0x023D457C /* daPy_mtxFollowEcallBack_c::makeEmitter */, &m32E4, 0x3E9 /* ID_IT_JN_OMAMORI_TSUBU00 */,
               model ? model + 0xC8 : 0, &current.pos, 0);
    Mtx34* hand = lk_getAnmMtx(mpCLModel, 12 /* CL_JNT_CL_RHANDA_e */);
    J3DModel_setBaseTRMtx(gabi::at<J3DModel>(mpEquipItemModel), hand);
    lk_talismanPos_l(this);
    dPa_control_set(dComIfGp_getParticle(), 1, 0x3EA /* ID_IT_JN_OMAMORI_FLASH00 */, gabi::at<cXyz>(gabi::ea(this) + 0x67A8), nullptr,
                    nullptr, 0xFF, gabi::at<dPa_levelEcallBack>(gabi::ea(this) + 0x67A0) /* m338C */, -1, nullptr, nullptr, nullptr);
    gabi::call(0x023E2E18 /* initShipRideUseItem */, this, use, 1);
    return TRUE;
}
VERIFY(0x02422950, &daPy_lk_c::dProcTalismanWait_init);

/* 02422BA8 */
BOOL daPy_lk_c::dProcTalismanWait() {
    WWHD_FUNC(0x02422BA8, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(0x023E287C /* setShipRidePos */, this, 1);
    }
    lk_talismanPos_l(this);
    PSMTXTrans(mDoMtx_stack_c::get(), gabi::load<f32>(0x1048D0CC + 0x0C), gabi::load<f32>(0x1048D0CC + 0x1C), gabi::load<f32>(0x1048D0CC + 0x2C));
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(gabi::load<u32>(0x101FF560) * 0x1FF));
    J3DModel_setBaseTRMtx(gabi::at<J3DModel>(mpEquipItemModel), mDoMtx_stack_c::get());
    gabi::call(0x025E742C /* mDoExt_baseAnm::play */, gabi::ea(this) + 0x4444 /* mSwordAnim */);
    u32 data = gabi::load<u32>(mpEquipItemModel + 0xAC); /* getModelData() */
    gabi::call(0x025E86B8 /* mDoExt_bckAnm::entry */, gabi::ea(this) + 0x4444, data, gabi::load<f32>(gabi::ea(this) + 0x4448));
    gabi::call(0x025E1A40 /* mDoAud_seStart */, 0x613E /* JA_SE_OBJ_OMAMORI */, gabi::ea(this) + 0x67A8, 0, (s32)(s8)mReverb);
    u32 flg = mModeFlg;
    if (demoParam0() != 0) {
        mModeFlg = flg | 0x80;
    } else {
        mModeFlg = flg & ~0x80u;
    }
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02422BA8, &daPy_lk_c::dProcTalismanWait);

/* 02422D9C */
BOOL daPy_lk_c::dProcSurprised_init() {
    WWHD_FUNC(0x02422D9C, BOOL, this);
    if (mCurProc == 0xB8 /* daPyProc_DEMO_SURPRISED_e */) {
        return FALSE;
    }
    int use = gabi::call<int>(0x023E26EC /* checkShipRideUseItem */, this, 1);
    gabi::call(LK_commonProcInit, this, 0xB8);
    gabi::call(LK_setSingleMoveAnime, this, 0x9A /* ANM_SURPRISED */, 1.0f, 0.0f, -1, 3.0f);
    s32 p0 = demoParam0();
    mNormalSpeed = 0.0f;
    mProcVar6 = 0;
    if (p0 == 1) {
        LK_voiceStart(28);
    } else if (p0 == 2) {
        LK_voiceStart(45);
    } else if (p0 == 3) {
        LK_voiceStart(49);
    }
    gabi::call(0x023E2E18 /* initShipRideUseItem */, this, use, 1);
    return TRUE;
}
VERIFY(0x02422D9C, &daPy_lk_c::dProcSurprised_init);

/* 02422F14 */
BOOL daPy_lk_c::dProcSurprised() {
    WWHD_FUNC(0x02422F14, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(0x023E287C /* setShipRidePos */, this, 1);
    }
    if (mProcVar6 != 0 || mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
        if (mProcVar6 == 0) {
            u16 u2 = m3532;
            u16 u1 = m3530;
            mProcVar6 = 1;
            gabi::call(LK_setSingleMoveAnime, this, 0xD8 /* ANM_SURPRISEDWAIT */, 1.0f, 0.0f, -1, 3.0f);
            m3530 = u1;
            m3532 = u2;
            mModeFlg = mModeFlg & ~0x400u;
        }
    }
    return TRUE;
}
VERIFY(0x02422F14, &daPy_lk_c::dProcSurprised);

/* 02422FF8 */
BOOL daPy_lk_c::dProcTurnBack_init() {
    WWHD_FUNC(0x02422FF8, BOOL, this);
    if (mCurProc == 0xB9 /* daPyProc_DEMO_TURN_BACK_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xB9);
    fopAc_ac_c* grab = mActorKeepGrab.mActor;
    mNormalSpeed = 0.0f;
    if (grab != nullptr) {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f); /* HD: HIO folded */
        lk_setUpperRatioAll_l(m_pbCalc[1], 1.0f);
        mProcVar6 = 1;
    } else {
        gabi::call(LK_setSingleMoveAnime, this, 0x9B /* ANM_TURNBACK */, 1.0f, 0.0f, 0x2C, 5.0f);
        mProcVar6 = 0;
    }
    return TRUE;
}
VERIFY(0x02422FF8, &daPy_lk_c::dProcTurnBack_init);

/* 02423124 */
BOOL daPy_lk_c::dProcTurnBack() {
    WWHD_FUNC(0x02423124, BOOL, this);
    if (mProcVar6 != 0 || mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02423124, &daPy_lk_c::dProcTurnBack);

/* 02423180 */
BOOL daPy_lk_c::dProcLookUp_init() {
    WWHD_FUNC(0x02423180, BOOL, this);
    if (mCurProc == 0xBA /* daPyProc_DEMO_LOOK_UP_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xBA);
    gabi::call(LK_setSingleMoveAnime, this, 0x9C /* ANM_LOOKUP */, 1.0f, 0.0f, -1, 5.0f);
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x02423180, &daPy_lk_c::dProcLookUp_init);

/* 02423230 */
BOOL daPy_lk_c::dProcLookUp() {
    WWHD_FUNC(0x02423230, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02423230, &daPy_lk_c::dProcLookUp);

/* 02423280 */
BOOL daPy_lk_c::dProcQuakeWait_init() {
    WWHD_FUNC(0x02423280, BOOL, this);
    if (mCurProc == 0xBB /* daPyProc_DEMO_QUAKE_WAIT_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xBB);
    gabi::call(LK_setSingleMoveAnime, this, 0x9D /* ANM_WAITQ */, 1.0f, 0.0f, -1, 5.0f);
    gabi::call(0x023DD768 /* setTextureAnime */, this, 104, 0);
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x02423280, &daPy_lk_c::dProcQuakeWait_init);

/* 02423340 */
BOOL daPy_lk_c::dProcQuakeWait() {
    WWHD_FUNC(0x02423340, BOOL, this);
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02423340, &daPy_lk_c::dProcQuakeWait);

/* 0242337C */
BOOL daPy_lk_c::dProcDance_init() {
    WWHD_FUNC(0x0242337C, BOOL, this);
    if (mCurProc == 0xBC /* daPyProc_DEMO_DANCE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xBC);
    gabi::call(LK_setSingleMoveAnime, this, 0x9E /* ANM_GLAD */, 1.0f, 0.0f, -1, 5.0f);
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x0242337C, &daPy_lk_c::dProcDance_init);

/* 0242342C */
BOOL daPy_lk_c::dProcDance() {
    WWHD_FUNC(0x0242342C, BOOL, this);
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x0242342C, &daPy_lk_c::dProcDance);

/* 02423468 */
BOOL daPy_lk_c::dProcCaught_init() {
    WWHD_FUNC(0x02423468, BOOL, this);
    if (mCurProc == 0xBD /* daPyProc_DEMO_CAUGHT_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xBD);
    if (demoParam0() == 0) {
        mProcVar6 = 1;
        gabi::call(LK_setSingleMoveAnime, this, 0xA4 /* ANM_MOGAKU1 */, 1.0f, 0.0f, -1, 3.0f);
        gravity = 0.0f;
        mNormalSpeed = 0.0f;
        speed.y = 0.0f;
    } else {
        mProcVar6 = 0;
        gabi::call(LK_setSingleMoveAnime, this, 0xA5 /* ANM_FM_BATA */, 1.0f, 0.0f, -1, 3.0f);
        gravity = 0.0f;
        speed.y = 0.0f;
        mNormalSpeed = 0.0f;
    }
    return TRUE;
}
VERIFY(0x02423468, &daPy_lk_c::dProcCaught_init);

/* 02423554 */
BOOL daPy_lk_c::dProcCaught() {
    WWHD_FUNC(0x02423554, BOOL, this);
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02423554, &daPy_lk_c::dProcCaught);

/* 02423590 */
BOOL daPy_lk_c::dProcLookWait() {
    WWHD_FUNC(0x02423590, BOOL, this);
    fopAc_ac_c* look = gabi::call<fopAc_ac_c*>(0x02400BC4 /* getDemoLookActor */, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(0x023E287C /* setShipRidePos */, this, 0);
    }
    if (look != nullptr) {
        s16 t = gabi::call<s16>(0x0200F93C /* cLib_targetAngleY */, &current.pos, gabi::ea(look) + 0x37C /* eyePos */);
        s16 sy = shape_angle.y;
        s16 d = (s16)(t - sy);
        s16 target;
        if (d > 0x6000) {
            target = (s16)(sy + 0x6000);
        } else if (d < -0x6000) {
            target = (s16)(sy - 0x6000);
        } else {
            target = sy;
        }
        cLib_addCalcAngleS(&shape_angle.y, target, 2, 0x800, 0x100);
    }
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02423590, &daPy_lk_c::dProcLookWait);

/* 02423680 */
BOOL daPy_lk_c::dProcPushPullWait_init() {
    WWHD_FUNC(0x02423680, BOOL, this);
    if (mCurProc == 0xBF /* daPyProc_DEMO_PUSH_PULL_WAIT_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xBF);
    gabi::call(LK_setSingleMoveAnime, this, 0x7E /* ANM_WAITPUSHPULL */, 1.0f, 0.0f, -1, 5.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    dComIfGp_onPlayerStatus0_l(0x4000000 /* daPyStts0_UNK4000000_e */);
    return TRUE;
}
VERIFY(0x02423680, &daPy_lk_c::dProcPushPullWait_init);

/* 02423740 */
BOOL daPy_lk_c::dProcPushPullWait() {
    WWHD_FUNC(0x02423740, BOOL, this);
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02423740, &daPy_lk_c::dProcPushPullWait);

/* 0242377C */
BOOL daPy_lk_c::dProcPushMove_init() {
    WWHD_FUNC(0x0242377C, BOOL, this);
    if (mCurProc == 0xC0 /* daPyProc_DEMO_PUSH_MOVE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xC0);
    gabi::call(LK_setSingleMoveAnime, this, 0x7F /* ANM_WALKPUSH */, 1.0f, 0.0f, -1, 5.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    dComIfGp_onPlayerStatus0_l(0x4000000 /* daPyStts0_UNK4000000_e */);
    LK_voiceStart(19);
    return TRUE;
}
VERIFY(0x0242377C, &daPy_lk_c::dProcPushMove_init);

/* 02423854 */
BOOL daPy_lk_c::dProcPushMove() {
    WWHD_FUNC(0x02423854, BOOL, this);
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02423854, &daPy_lk_c::dProcPushMove);

/* 02423890 */
BOOL daPy_lk_c::dProcDoorOpen_init() {
    WWHD_FUNC(0x02423890, BOOL, this);
    if (mCurProc == 0xC1 /* daPyProc_DEMO_DOOR_OPEN_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xC1);
    s32 p0 = demoParam0();
    int anm = (p0 & 1) ? 0xB0 /* ANM_DOOROPENBLINK */ : 0xAF /* ANM_DOOROPENALINK */;
    mProcVar7 = 0;
    if (p0 & 2) {
        mProcVar7 = 3; /* HD */
        gabi::call(LK_setSingleMoveAnime, this, anm, 1.0f, 35.0f, -1, 0.0f);
    } else {
        gabi::call(LK_setSingleMoveAnime, this, anm, 1.0f, 0.0f, -1, 0.0f);
    }
    mNormalSpeed = 0.0f;
    mAcch.m_flags = mAcch.m_flags | 0x4004; /* SetWallNone(), OnLineCheckNone() */
    m3700.copy(*gabi::at<cXyz>(0x101FFBA8) /* cXyz::Zero */);
    mProcVar6 = 0;
    m34C2 = 3;
    return TRUE;
}
VERIFY(0x02423890, &daPy_lk_c::dProcDoorOpen_init);

/* 024239D8 */
BOOL daPy_lk_c::dProcDoorOpen() {
    WWHD_FUNC(0x024239D8, BOOL, this);
    s32 pv7 = mProcVar7;
    if (pv7 != 0) {
        /* HD: a no-reset flag (0x08000000) held while the counter runs */
        pv7 = pv7 - 1;
        mProcVar7 = pv7;
        if (pv7 != 0) {
            mNoResetFlg0 = mNoResetFlg0 | 0x8000000;
        } else {
            mNoResetFlg0 = mNoResetFlg0 & ~0x8000000u;
        }
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        mAcch.m_flags = mAcch.m_flags & ~0x4004u; /* ClrWallNone(), OffLineCheckNone() */
        LK_cutEnd();
        gabi::call(LK_setBlendMoveAnime, this, 2.4f); /* HD: HIO folded */
        mProcVar6 = 1;
        m3598 = 0.0f;
    } else if (mProcVar6 == 0) {
        if (m34C2 != 3) {
            m34C2 = 1;
        }
    } else {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x024239D8, &daPy_lk_c::dProcDoorOpen);

/* 02423AEC */
BOOL daPy_lk_c::dProcNod_init() {
    WWHD_FUNC(0x02423AEC, BOOL, this);
    if (mCurProc == 0xC2 /* daPyProc_DEMO_NOD_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xC2);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xB1 /* ANM_SEYYES */, 1.0f, 0.0f, 0x1C, 5.0f); /* HD: HIO folded */
    dComIfGp_onPlayerStatus0_l(0x10 /* daPyStts0_UNK10_e */);
    return TRUE;
}
VERIFY(0x02423AEC, &daPy_lk_c::dProcNod_init);

/* 02423B6C */
BOOL daPy_lk_c::dProcNod() {
    WWHD_FUNC(0x02423B6C, BOOL, this);
    setShapeAngleToTalkActor();
    current.angle.y = shape_angle.y;
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02423B6C, &daPy_lk_c::dProcNod);

/* 02423BCC */
void daPy_lk_c::dProcPresent_init_sub() {
    WWHD_FUNC(0x02423BCC, void, this);
    gabi::call(LK_setSingleMoveAnime, this, 0xB2 /* ANM_PRESENTATIONA */, 1.1f, 0.0f, 0x3E, 2.0f); /* HD: HIO folded */
    mModeFlg = mModeFlg | 0x400;
    if (!checkBottleItem(gabi::load<u8>(dComIfGp_ea() + 0x52B1) /* dComIfGp_event_getPreItemNo() */)) {
        u32 play = dComIfGp_ea();
        s32 id = gabi::call<s32>(0x025D7DEC /* fopAcM_createItemForPresentDemo */, &current.pos, (u32)gabi::load<u8>(play + 0x52B1), 3, -1,
                                 (s32)(s8)current.roomNo, &shape_angle, &scale);
        gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); /* dComIfGp_event_setItemPartnerId() */
    }
    gabi::call(0x023EEB10 /* keepItemData */, this);
}
VERIFY(0x02423BCC, &daPy_lk_c::dProcPresent_init_sub);

/* 02423C84 */
BOOL daPy_lk_c::dProcPresent_init() {
    WWHD_FUNC(0x02423C84, BOOL, this);
    if (mCurProc == 0xC3 /* daPyProc_DEMO_PRESENT_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xC3);
    s32 r = gabi::call<s32>(0x023F49A4 /* setTalkStartBack */, this);
    mProcVar6 = r;
    if (r == 0) {
        mNormalSpeed = 0.0f;
        dProcPresent_init_sub();
    }
    dComIfGp_onPlayerStatus0_l(0x10 /* daPyStts0_UNK10_e */);
    mDoAud_seStartSystem_l(0x80A /* JA_SE_TALK_START */);
    mProcVar7 = 0;
    return TRUE;
}
VERIFY(0x02423C84, &daPy_lk_c::dProcPresent_init);

/* 02420F08 */
BOOL daPy_lk_c::dProcTalk() {
    WWHD_FUNC(0x02420F08, BOOL, this);
    LK_cutEnd();
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(0x023E287C /* setShipRidePos */, this, 0);
    }
    setShapeAngleToTalkActor();
    if (mProcVar6 != 0) {
        gabi::Local<cXyz> d;
        cXyz_mi(&m370C, d, &current.pos);
        s32 iVar2 = 0;
        if (mAcch.m_flags & 0x10 /* ChkWallHit() */) {
            for (int i = 0; i < 3; i++) {
                u32 cir = gabi::ea(&mAcchCir[0]) + i * 0x40;
                if (gabi::load<u32>(cir + 0x10) & 2) {
                    iVar2 = cLib_distanceAngleS(gabi::load<s16>(cir + 0x3C) /* GetWallAngleY() */, current.angle.y);
                    break;
                }
            }
        }
        if (cLib_distanceAngleS(cM_atan2s(d->x, d->z), current.angle.y) > 0x4000 || iVar2 > 0x4000) {
            u32 mode = mDemoMode;
            fcpy_l(gabi::ea(&current.pos.x), gabi::ea(&m370C.x));
            mProcVar6 = 0;
            fcpy_l(gabi::ea(&current.pos.z), gabi::ea(&m370C.z));
            if (mode == 8) {
                gabi::call(LK_setSingleMoveAnime, this, 0 /* ANM_WAITS */, 1.1f, 0.0f, -1, 5.0f); /* HD: HIO folded */
            } else {
                gabi::call(LK_setSingleMoveAnime, this, 0x1B /* ANM_TALKA */, 0.7f, 0.0f, -1, 5.0f);
            }
            if (mProcVar7 == 1) {
                gabi::call(0x023DD768 /* setTextureAnime */, this, 145, 0);
                mModeFlg = mModeFlg & ~0x80u;
            }
            mNormalSpeed = 0.0f;
        }
    } else {
        u32 pv7 = mProcVar7;
        u32 p1 = gabi::load<u32>(gabi::ea(this) + 0x42C); /* mDemo.getParam1() */
        current.angle.y = shape_angle.y;
        if (p1 != pv7) {
            mProcVar7 = p1;
            if (p1 == 1) {
                gabi::call(0x023DD768 /* setTextureAnime */, this, 145, (u32)m3530);
                mModeFlg = mModeFlg & ~0x80u;
            } else {
                gabi::call(0x023DD768 /* setTextureAnime */, this, 0, (u32)m3530);
                mModeFlg = mModeFlg | 0x80;
            }
        }
    }
    return TRUE;
}
VERIFY(0x02420F08, &daPy_lk_c::dProcTalk);

/* 02421948 */
BOOL daPy_lk_c::dProcGetItem() {
    WWHD_FUNC(0x02421948, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(0x023E287C /* setShipRidePos */, this, 0);
    }
    if (mProcVar7 != 0) {
        LK_cutEnd();
        return TRUE;
    }
    J3DFrameCtrl& fc = mFrameCtrlUnder[0];
    if (fc.getFrame() < 10.0f) {
        s32 idx = mCameraInfoIdx;
        u32 cam = gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8);
        mProcVar3 = (s16)(shape_angle.y - gabi::load<s16>(cam + 0x236) /* fopCamM_GetAngleY() */);
    }
    u32 item = gabi::call<u32>(0x025D7C98 /* fopAcM_getItemEventPartner */, this);
    s32 itemNo = mProcVar2;
    if (itemNo == -1 && item != 0) {
        itemNo = gabi::call<s32>(0x021841C8 /* daItemBase_c::getItemNo */, item);
        mProcVar2 = (s16)itemNo;
    }
    bool toRate;
    if (mProcVar6 == 0) {
        f32 frame = fc.getFrame();
        if (frame < 11.0f) {
            toRate = true;
        } else {
            if (mProcVar0 == 0 && itemNo != -1) {
                u16 no = gabi::load<u16>(gabi::ea(this) + 0x691A);
                s16 pv5 = mProcVar5;
                mProcVar0 = 1;
                gabi::call(0x023DBCA0 /* setGetItemSound */, this, (u32)no, (s32)pv5);
                frame = fc.getFrame();
            }
            s16 cur = current.angle.y;
            if (frame > 17.0f) {
                shape_angle.y = (s16)(cur - mProcVar3);
            } else {
                f32 t = frame - 10.0f;
                s16 pv3 = mProcVar3;
                if (pv3 > 0x4000) {
                    shape_angle.y = (s16)gabi::ftoi((f32)cur + ((f32)(0x10000 - pv3) * t) / 7.0f);
                } else {
                    shape_angle.y = (s16)gabi::ftoi((f32)cur - ((f32)pv3 * t) / 7.0f);
                }
            }
            toRate = mProcVar6 == 0;
        }
    } else {
        toRate = false;
    }
    if (!toRate) {
        if (gabi::load<u16>(gabi::ea(this) + 0x5888) == 0xFFFF /* checkNoUpperAnime() */) {
            gabi::call(LK_setSingleMoveAnime, this, 0x90 /* ANM_ITEMGET */, 1.0f, 0.0f, -1, 3.0f);
            u32 flg = mModeFlg;
            mProcVar6 = 0;
            mModeFlg = flg | 0x400;
        }
        if (item != 0 && (gabi::load<s16>(item + 8) == 0xFF /* fpcNm_ITEM_e */ || gabi::load<s16>(item + 8) == 0x101 /* fpcNm_Demo_Item_e */)) {
            gabi::call(0x021842B8 /* daItemBase_c::hide */, item);
        }
        return TRUE;
    }
    if (item == 0) {
        return TRUE;
    }
    s16 name = gabi::load<s16>(item + 8);
    if (!(fc.getRate() < 0.01f)) {
        if (name == 0xFF || gabi::load<s16>(item + 8) == 0x101) {
            gabi::call(0x021842B8 /* daItemBase_c::hide */, item);
        }
        return TRUE;
    }
    if (name != 0xFF && gabi::load<s16>(item + 8) != 0x101) {
        return TRUE;
    }
    gabi::call(0x021842C8 /* daItemBase_c::show */, item);
    s32 msg = m3624;
    if (msg == 0) {
        s32 no = mProcVar2;
        if (no == -1) {
            return TRUE;
        }
        u16 maxLife = gabi::load<u16>(dComIfGs_base_l() + 0x20); /* dComIfGs_getMaxLife() */
        if (no == 7 && (maxLife & 3) != 0) {
            msg = (maxLife & 3) + 0x7B;
        } else {
            msg = no + 0x65; /* MSG_NO_FOR_ITEM */
        }
        m3624 = msg;
        if (msg == 0) {
            return TRUE;
        }
    }
    if (checkEndMessage(msg)) {
        s16 a = (s16)(shape_angle.y - 0x8000);
        shape_angle.y = a;
        m34DE = a;
        current.angle.y = a;
        gabi::call(0x023E278C /* setOldRootQuaternion */, this, 0, -0x8000, 0);
        /* m_old_fdata->getOldFrameTransInfo(0): mTranslate.x / .z negated */
        u32 info = gabi::load<u32>(m_old_fdata + 0x1C);
        f32 x = gabi::load<f32>(info + 0x14);
        f32 z = gabi::load<f32>(info + 0x1C);
        gabi::store<f32>(info + 0x14, -x);
        gabi::store<f32>(info + 0x1C, -z);
        LK_cutEnd();
        gabi::call(0x0218432C /* daItemBase_c::dead */, item);
        mProcVar7 = 1;
        gabi::call(LK_setBlendMoveAnime, this, 2.4f); /* HD: HIO folded */
    }
    return TRUE;
}
VERIFY(0x02421948, &daPy_lk_c::dProcGetItem);

/* 02422104 */
BOOL daPy_lk_c::dProcDead() {
    WWHD_FUNC(0x02422104, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(0x023E287C /* setShipRidePos */, this, 1);
    }
    if (mProcVar6 != 0 && dComIfGp_event_compulsory_l(this)) {
        gabi::call(0x023F7434 /* dProcDead_init_sub */, this);
        if (mProcVar2 == 0) {
            gabi::call(0x023F74B0 /* dProcDead_init_sub2 */, this);
        }
    }
    if (mProcVar2 != 0 && changeSwimUpProc()) {
        s32 pv6 = mProcVar6;
        mProcVar2 = 0;
        if (pv6 != 0) {
            return TRUE;
        }
        gabi::call(0x023F74B0 /* dProcDead_init_sub2 */, this);
    }
    if (mProcVar6 != 0 || mProcVar2 != 0) {
        return TRUE;
    }
    if (mModeFlg & 0x2000000 /* ModeFlg_02000000 */) {
        m35E4 = (frameCtrl.getFrame() - 120.0f) * 0.1f;
    }
    if (frameCtrl.getRate() < 0.01f) {
        cLib_chaseF(&m35A0, 0.0f, 0.01f);
        /* HD: the game-over object is looked up by id (025DE50C); the back alpha setter takes m35A0 */
        s32 id = mGameOverId;
        f32 ratio = m35A0;
        if (id != -1) {
            u32 go = gabi::call<u32>(0x025DE50C, id);
            if (go != 0) {
                gabi::call(0x02549C00 /* d_GameOver_setBackAlpha */, go, ratio);
            }
            ratio = m35A0;
        }
        if (ratio < 0.38f && (id = mGameOverId) != -1) {
            u32 go = gabi::call<u32>(0x025DE50C, id);
            if (go != 0) {
                gabi::store<u8>(go + 0x111, 1); /* d_GameOver_animeStart() */
            }
        }
        if (mProcVar7 == 0) {
            id = mGameOverId;
            if (id == -1) {
                return TRUE;
            }
            u32 go = gabi::call<u32>(0x025DE50C, id);
            if (go != 0 && gabi::call<BOOL>(0x02549BEC /* d_GameOver_CheckDelete */, go)) {
                mProcVar7 = 1;
            } else if (mProcVar7 == 0) {
                return TRUE;
            }
        }
        if (gabi::load<u8>(dComIfGp_ea() + 0x5BE3) == 2 /* dComIfGp_getGameoverStatus() */) {
            gabi::call(0x025217F8 /* dComIfGs_setGameStartStage */);
            gabi::call(0x02522198 /* dComIfGs_gameStart */);
        }
        return TRUE;
    }
    if (!(gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x110000 /* daPyStts0_SHIP_RIDE_e | daPyStts0_SWIM_e */)) {
        if (frameCtrl.checkPass(129.0f)) {
            LK_voiceStart(0x17);
            mFootEffectPosType = 6;
        } else if (!(frameCtrl.getFrame() < 129.0f)) {
            gabi::call(LK_resetFootEffect, this);
        }
        return TRUE;
    }
    if (!dComIfGp_checkPlayerStatus0_l(0x100000 /* daPyStts0_SWIM_e */)) {
        return TRUE;
    }
    if (frameCtrl.checkPass(9.0f) || frameCtrl.checkPass(15.0f)) {
        gabi::Local<csXyz> ang;
        gabi::call(0x0201A478 /* csXyz::csXyz */, ang.get(), 0, (s32)shape_angle.y, 0);
        gabi::Local<GXColor> amb, dif;
        gabi::call(0x025602F0 /* dKy_get_seacolor */, amb.get(), dif.get());
        if (frameCtrl.checkPass(15.0f)) {
            ang->y = (s16)(ang->y - 0x8000);
        }
        dPa_control_set(dComIfGp_getParticle(), 1, 0x314 /* ID_AK_JN_DROWNINGSPLASH00 */, &current.pos, ang.get(), nullptr, 0xFF,
                        nullptr, -1, amb.get(), nullptr, nullptr);
    } else if (frameCtrl.checkPass(50.0f)) {
        mProcVar3 = 0;
    } else if (frameCtrl.checkPass(110.0f)) {
        mProcVar3 = 1;
    }
    return TRUE;
}
VERIFY(0x02422104, &daPy_lk_c::dProcDead);

/* HD dProcTool: strcmp(dComIfGp_getStartStageName() (0x1047E6B8), name) on sead::SafeStrings; the first
 * cstr() call is direct (this TU's empty 02444F48), the next two virtual (slot 0x14). An exhausted compare
 * (0x40001 equal bytes) takes the mismatch path everywhere. */
static inline bool lk_stageIs_l(u32 nameStr) {
    gabi::Local<SafeString> name;
    name->mStringTop = nameStr;
    name->__vtbl = 0x10034B24;
    gabi::Local<SafeString> stage;
    stage->mStringTop = 0x1047E6B8;
    stage->__vtbl = 0x10034B24;
    gabi::call(0x02444F48, name.get());
    gabi::call_ptr(gabi::load<u32>(name->__vtbl + 0x14), name.get());
    u32 a = name->mStringTop;
    gabi::call_ptr(gabi::load<u32>(stage->__vtbl + 0x14), stage.get());
    u32 b = stage->mStringTop;
    if (a == b) {
        return true;
    }
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(a + i);
        u8 cb = gabi::load<u8>(b + i);
        if (ca != cb) {
            return false;
        }
        if (ca == 0) {
            return true;
        }
    }
    return false;
}
/* g_dComIfG_gameInfo demo frame counter (0x101D600C, probably the demo's current frame) */
static inline u32 lk_demoFrame_l() { return gabi::load<u32>(0x101D600C); }
/* 0241FD7C */
BOOL daPy_lk_c::dProcTool() {
    WWHD_FUNC(0x0241FD7C, BOOL, this);
    u8 demoId = gabi::load<u8>(gabi::ea(this) + 0x2DC); /* demoActorID */
    if (demoId == 0 || demoId > 0x20) {
        mProcVar7 = 0;
        mProcVar3 = 0;
        LK_checkNextMode(0);
        return TRUE;
    }
    u32 obj = gabi::load<u32>(0x101D5FFC); /* dComIfGp_demo_get() */
    if (obj == 0) {
        JUT_ASSERT_fail(STR(0x10035038) /* "d_demo.h" */, 0x23A, STR(0x10034F74) /* "m_object != (0)" */);
        obj = gabi::load<u32>(0x101D5FFC);
    }
    u32 actor = gabi::call<u32>(0x02526E70 /* dDemo_object_c::getActor */, obj, (u32)demoId);
    mProcVar3 = 0;
    mProcVar7 = 0;
    if (actor == 0) {
        LK_checkNextMode(0);
        return TRUE;
    }
    u32 heap = gabi::ea(this) + 0x5848; /* m_anm_heap_under[0]: +0 mIdx, +4 field_0x4, +6 field_0x6 */
    u16 anmIdx = 0xFFFF;
    u16 r25 = (u16)((mDemoMode >> 9) - 1); /* getDemoMode() / DEMO_NEW_ANM0_e - 1 */
    f32 frame = 0.0f;
    u16 flags = gabi::load<u16>(actor + 4);
    if (flags & 2 /* ENABLE_TRANS_e */) {
        /* HD: on the stage "Demo02" the translation is only taken while the demo frame counter runs */
        if (lk_demoFrame_l() != 0 || !lk_stageIs_l(0x1003600C /* "Demo02" */)) {
            current.pos.copy(*gabi::at<cXyz>(actor + 8));
        }
        flags = gabi::load<u16>(actor + 4);
    }
    if (flags & 8 /* ENABLE_ROTATE_e */) {
        u16 rx = gabi::load<u16>(actor + 0x20);
        shape_angle.x = rx;
        u16 ry = gabi::load<u16>(actor + 0x22);
        shape_angle.y = ry;
        u16 rz = gabi::load<u16>(actor + 0x24);
        shape_angle.z = rz;
        current.angle.x = rx;
        current.angle.y = ry;
        current.angle.z = rz;
        /* HD: on the stage "Demo24" two demo frame windows move Link by fixed offsets */
        if (lk_stageIs_l(0x10036014 /* "Demo24" */)) {
            u32 f = lk_demoFrame_l();
            if (f - 0xC1C < 0x37) {
                if (gabi::load<u32>(0x1046D204) == 0) {
                    gabi::store<u32>(0x1046D204, 1);
                    gabi::store<f32>(0x1046CDA8, 15.0f);
                    gabi::store<f32>(0x1046CDAC, -10.0f);
                    gabi::store<f32>(0x1046CDB0, 30.0f);
                }
                gabi::call(0x0200FA40 /* offset position by angle */, &current.pos, &current.pos, (s32)current.angle.y, 0x1046CDA8);
            } else if (f - 0xF51 < 0x8A) {
                if (gabi::load<u32>(0x1046D208) == 0) {
                    gabi::store<u32>(0x1046D208, 1);
                    gabi::store<f32>(0x1046CDB4, 0.0f);
                    gabi::store<f32>(0x1046CDB8, 0.0f);
                    gabi::store<f32>(0x1046CDBC, 20.0f);
                }
                gabi::call(0x0200FA40, &current.pos, &current.pos, (s32)current.angle.y, 0x1046CDB4);
            }
        }
        flags = gabi::load<u16>(actor + 4);
    }
    if (flags & 0x40 /* ENABLE_ANM_FRAME_e */) {
        frame = gabi::load<f32>(actor + 0x30);
    }
    f32 morf = -1.0f;
    if (flags & 1 /* ENABLE_UNK_e: parameters */) {
        s32 id = gabi::load<s32>(actor + 0x4C);
        u32 data = gabi::load<u32>(actor + 0x50);
        if (id == 0 || id == 2 || id == 4) {
            gabi::Local<u8[0x14]> outA, outB; /* 0x14 bytes each (TData); was cXyz (12 bytes): the parser 0283CA7C writes 0x14 (game test: frame guard) */
            gabi::Local<be<u32>> inA;
            *inA = data;
            gabi::call(0x0283CA7C, inA.get(), outA.get());
            u32 d = gabi::load<u32>(outA.a + 0xC);
            if (d == 0) {
                JUT_ASSERT_fail(STR(0x10034B14) /* "stb.h" */, 0x24A, STR(0x10035048) /* "!empty()" */);
                d = gabi::load<u32>(outA.a + 0xC);
            }
            u32 next = gabi::load<u32>(outA.a + 0x10);
            anmIdx = gabi::load<u16>(d);
            gabi::Local<be<u32>> inB;
            *inB = next;
            gabi::call(0x0283CA7C, inB.get(), outB.get());
            u32 size = gabi::load<u32>(outB.a + 8);
            u32 p = gabi::load<u32>(outB.a + 0xC);
            if (p == 0) {
                JUT_ASSERT_fail(STR(0x10034B1C), 0x24A, STR(0x10035054));
                size = gabi::load<u32>(outB.a + 8);
                p = gabi::load<u32>(outB.a + 0xC);
            }
            if (size == 3) {
                mLeftHandIdx = gabi::load<u8>(p);
                mRightHandIdx = gabi::load<u8>(p + 1);
                morf = (f32)gabi::load<u8>(p + size - 1);
            } else {
                mLeftHandIdx = gabi::load<u8>(p);
                mRightHandIdx = gabi::load<u8>(p + size - 1);
            }
            s32 id2 = gabi::load<s32>(actor + 0x4C);
            if (id2 == 2) {
                mProcVar7 = 1;
            } else if (id2 == 4) {
                mProcVar3 = 1;
            }
        } else if (id == 1 || id == 3 || id == 5) {
            gabi::Local<u8[0x14]> outA, outB; /* TData, 0x14 bytes */
            gabi::Local<be<u32>> inA;
            *inA = data;
            gabi::call(0x0283CA7C, inA.get(), outA.get());
            u32 d = gabi::load<u32>(outA.a + 0xC);
            if (d == 0) {
                JUT_ASSERT_fail(STR(0x10034B14) /* "stb.h" */, 0x24A, STR(0x10035048) /* "!empty()" */);
                d = gabi::load<u32>(outA.a + 0xC);
            }
            u32 count = gabi::load<u32>(outA.a + 8);
            u16 btp = gabi::load<u16>(d + 2);
            anmIdx = gabi::load<u16>(d);
            u16 btk = gabi::load<u16>(d + count * 2 - 2);
            /* HD: on the stage "Demo32" (from demo frame 0x147) the texture pair 0x5C/0x35 is replaced */
            if (btp == 0x5C && btk == 0x35 && lk_demoFrame_l() > 0x146 && lk_stageIs_l(0x1003601C /* "Demo32" */)) {
                btk = 0x38;
                btp = 0x5F;
            }
            setDemoTextureAnime(btp, btk, 0, r25);
            u32 next = gabi::load<u32>(outA.a + 0x10);
            gabi::Local<be<u32>> inB;
            *inB = next;
            gabi::call(0x0283CA7C, inB.get(), outB.get());
            u32 size = gabi::load<u32>(outB.a + 8);
            u32 p = gabi::load<u32>(outB.a + 0xC);
            if (p == 0) {
                JUT_ASSERT_fail(STR(0x10034B1C), 0x24A, STR(0x10035054));
                size = gabi::load<u32>(outB.a + 8);
                p = gabi::load<u32>(outB.a + 0xC);
            }
            if (size == 3) {
                mLeftHandIdx = gabi::load<u8>(p);
                mRightHandIdx = gabi::load<u8>(p + 1);
                morf = (f32)gabi::load<u8>(p + size - 1);
            } else {
                mLeftHandIdx = gabi::load<u8>(p);
                mRightHandIdx = gabi::load<u8>(p + size - 1);
            }
            s32 id2 = gabi::load<s32>(actor + 0x4C);
            if (id2 == 3) {
                /* HD: not on the stage "Demo24" */
                if (!lk_stageIs_l(0x10036014 /* "Demo24" */)) {
                    mProcVar7 = 1;
                } else if (gabi::load<s32>(actor + 0x4C) == 5) {
                    mProcVar3 = 1;
                }
            } else if (id2 == 5) {
                mProcVar3 = 1;
            }
        }
        /* the hand items */
        u8 left = mLeftHandIdx;
        u32 itemNo = 0;
        if (left == 0xC8) {
            itemNo = 0x38; /* dItemNo_SWORD_e */
        } else if (left == 0xC9) {
            itemNo = 0x39; /* dItemNo_MASTER_SWORD_1_e */
        } else if (left == 0xCA) {
            itemNo = 0x3A; /* dItemNo_MASTER_SWORD_2_e */
        } else if (left == 0xCB) {
            itemNo = 0x3E; /* dItemNo_MASTER_SWORD_3_e */
        }
        if (itemNo != 0) {
            u16 equip = mEquipItem;
            mLeftHandIdx = 3;
            if (equip != 0x103 /* daPyItem_SWORD_e */ || gabi::load<u8>(dComIfGs_base_l() + 0x2E) != itemNo /* dComIfGs_getSelectEquip(0) */) {
                gabi::call(0x02522398 /* dComIfGs_setSelectEquip */, 0, itemNo);
                gabi::call(LK_deleteEquipItem, this, 0);
                gabi::call(0x023DE0B4 /* setSwordModel */, this, 1);
            }
        } else if (left == 0xCC) {
            if (mEquipItem != 0x22 /* dItemNo_WIND_WAKER_e */) {
                gabi::call(LK_deleteEquipItem, this, 0);
            }
            mLeftHandIdx = 5;
            gabi::call(0x023DEF04 /* setTactModel */, this);
        } else if (mEquipItem != 0x100 /* daPyItem_NONE_e */) {
            gabi::call(LK_deleteEquipItem, this, 0);
        }
        u8 right = mRightHandIdx;
        if (right == 0xC8 || right == 0xC9) {
            u32 shield = mRightHandIdx == 0xC8 ? 0x3B /* dItemNo_SHIELD_e */ : 0x3C /* dItemNo_MIRROR_SHIELD_e */;
            mProcVar2 = 1;
            gabi::call(0x02522398 /* dComIfGs_setSelectEquip */, 1, shield);
            mRightHandIdx = 8;
        } else {
            mProcVar2 = 0;
            if (right != 0) {
                mRightHandIdx = (u8)(right + 6);
            }
        }
    }
    /* HD: on the stage "Demo32" some demo frames force an animation */
    if (lk_stageIs_l(0x1003601C /* "Demo32" */)) {
        u32 f = lk_demoFrame_l();
        if (!(f < 0x15E)) {
            if (anmIdx == 0x2E) {
                anmIdx = 0x1C;
                morf = 0.0f;
            } else if (f == 0x87A) {
                anmIdx = 0x2E;
                morf = 8.0f;
            } else {
                for (u32 i = 0; i < 8; i++) {
                    if (f == gabi::load<u32>(0x10034F54 + i * 4)) {
                        anmIdx = 0x19B;
                        morf = 8.0f;
                        gabi::store<u16>(heap + 4, 0xFFFF);
                    }
                }
            }
        }
    }
    if (anmIdx != 0xFFFF && (gabi::load<u16>(heap + 4) != anmIdx || gabi::load<u16>(heap + 6) != r25)) {
        /* dComIfGp_getLkDemoAnmArchive(): a sead::SafeString at play + 0x5A50 */
        u32 play = dComIfGp_ea();
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(play + 0x5A54) + 0x14), play + 0x5A50);
        gabi::Local<SafeString> key;
        key->mStringTop = gabi::load<u32>(play + 0x5A50);
        key->__vtbl = 0x10034B24;
        u32 anm = gabi::call<u32>(0x026067F4 /* dRes_control_c::getIDRes */, gabi::load<u32>(0x101F4F28), key.get(), (u32)anmIdx);
        u32 vt = gabi::load<u32>(anm + 4);
        s32 attr = gabi::call_ptr<s32>(gabi::load<u32>(vt + 0xC), anm);
        s32 fmax = gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), anm);
        gabi::call(0x023DE788 /* setFrameCtrl */, this, &mFrameCtrlUnder[0], attr, 0, fmax, 1.0f, frame);
        gabi::store<f32>(anm, frame); /* setFrame */
        gabi::store<u16>(heap + 4, anmIdx);
        gabi::store<u16>(heap + 6, r25);
        gabi::store<u16>(heap + 0, 0xFFFF);
        gabi::store<u32>(gabi::ea(this) + 0x57FC, anm); /* mAnmRatioUnder[UNDER_MOVE0_e].setAnmTransform() */
        m34C3 = 0;
        gabi::store<u32>(gabi::ea(this) + 0x581C, anm); /* mAnmRatioUpper[UPPER_MOVE0_e].setAnmTransform() */
        if (anmIdx == 0x198 && lk_stageIs_l(0x10036024 /* "Demo23" */)) {
            gabi::call(0x025E3F3C /* initOldFrameMorf */, (u32)m_old_fdata, 8.0f, 0, 0x2A); /* HD */
        } else if (!(morf < 0.0f)) {
            gabi::call(0x025E3F3C /* initOldFrameMorf */, (u32)m_old_fdata, morf, 0, 0x2A);
        }
        s32 fmax2 = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0x14), anm);
        gabi::store<f32>(actor + 0x38, (f32)fmax2); /* demo_actor->setAnmFrameMax() */
        gabi::call(0x023E07E4 /* setSeAnime */, this, gabi::load<u32>(gabi::ea(this) + 0x57FC), heap, &mFrameCtrlUnder[0]);
        return TRUE;
    }
    if (!(gabi::load<u16>(actor + 4) & 0x40 /* ENABLE_ANM_FRAME_e */)) {
        return TRUE;
    }
    /* HD: the frame is not taken for three animations on their stages */
    u16 cur = gabi::load<u16>(heap + 4);
    if (cur == 0x12F) {
        if (lk_stageIs_l(0x1003602C /* "Demo44" */)) {
            return TRUE;
        }
        cur = gabi::load<u16>(heap + 4);
    }
    if (cur == 0x198) {
        if (lk_stageIs_l(0x10036024 /* "Demo23" */)) {
            return TRUE;
        }
        cur = gabi::load<u16>(heap + 4);
    }
    if (cur == 0x1C && lk_stageIs_l(0x1003601C /* "Demo32" */)) {
        return TRUE;
    }
    u32 anm = gabi::load<u32>(gabi::ea(this) + 0x57FC);
    u32 vt = gabi::load<u32>(anm + 4);
    gabi::store<f32>(anm, frame);
    s32 fmax = gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), anm);
    s32 attr = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0xC), anm);
    if (attr == 2) { /* HD: a looping animation ends one frame earlier */
        fmax = fmax - 1;
    }
    gabi::store<f32>(actor + 0x38, (f32)fmax);
    mFrameCtrlUnder[0].mFrame = frame;
    return TRUE;
}
VERIFY(0x0241FD7C, &daPy_lk_c::dProcTool);
