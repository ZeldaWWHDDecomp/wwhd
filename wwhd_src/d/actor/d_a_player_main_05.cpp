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
 * Functions use named native bindings and private HD views.
 */
#include "d/actor/d_a_player_main_05.h"
#include "d/actor/d_a_player_anm_t60.h"
#include "true60.h"
#include "d/actor/d_a_player_t60.h"
#include "d/actor/d_a_player_neckdmg_t60.h" /* true60 */
#include "d/actor/d_a_player_swimrise_t60.h"
#include "d/actor/d_a_player_crawl_t60.h"
#include "d/actor/d_a_player_lavarestart_t60.h" /* true60: the lava restart test */
#include "d/actor/d_a_player_waitturn_t60.h" /* true60 */
#include <cstdlib>

namespace link05 {
static u16 l_curseType;
static const cXyz l_crawl_start_front_offset = {0.0f, 0.0f, 112.0f};
static const cXyz l_crawl_top_offset = {0.0f, 0.0f, 0.0f};
static const cXyz l_ship_offset = {0.0f, 15.0f, -35.0f};
static f32 l_rope_hanging_offset;
static cXyz l_zero;
static Mtx34 l_matrixStack;
static Link05TimerHD g_Counter;

}


namespace link05 {
using Link05DemoMessagePointer = gptr<Link05DemoMessageManager>;
static Link05DemoMessagePointer l_demoMessageManager;
static f32 l_bootsEquipFrame;
}

/* true60: a step that advances once-per-30-Hz-frame work (every step at dt 1, the tick30 step in converted code) */
static inline bool lk05FrameStep() { return t60::dt() == 1.0f || t60::tick30(); }
/* true60 host state: procAutoJump's wall / body contact seen on a step of the current frame;
 * dProcTalismanWait's step counter at the frame's first step */
static bool s_lk05AutoJumpTouch;
static uint32_t s_lk05AjSpeedFirst = 0xFFFFFFFFu; /* true60: procAutoJump's frame-first step (speed chase) */
static u32 s_lk05FrameFirstStep;
/* true60 (the 60 Hz counter rule): how far step k of the current 30 Hz frame lies before the frame's end, in frames
 * (1 - (k + 1) x dt; 0 on the tick30 step): for visual phases read from the 30 Hz g_Counter */
static inline f32 lk05CounterLag() {
    if (t60::first_step()) {
        s_lk05FrameFirstStep = t60_step_counter();
    }
    f32 k = (f32)(t60_step_counter() - s_lk05FrameFirstStep);
    return 1.0f - (k + 1.0f) * (f32)t60::dt();
}

/* true60 (2026-11-22, route 191): procAutoJump's apex test (speed.y <
 * -gravity: the fall animation ANM_JMPEDS, mProcVar6 = 2) reads speed.y, which the posMove gravity lowers per step by
 * dt x gravity. A tests it once a frame with the frame-start speed.y; per step, B's later step read the half-step value
 * and crossed the threshold half a frame early (B 191 step 2, A 192: anim 2.499 tslack). Form: the test decides on the
 * frame's first step (frame-start speed.y = A's); the later steps keep the frame's animation. Negative control
 * WWHD_T60_AJAPEX_NEGCTRL=1 (game build: the per-step test). dt 1: the original. */
static inline bool lk05AjApexDecides() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_AJAPEX_NEGCTRL"); return e && e[0] == '1'; }();
#else
    const bool nc = false;
#endif
    return t60::dt() == 1.0f || nc || t60::first_step();
}

/* true60 (the step-resolution threshold rule frame-grid form; slope step-resolution threshold 86/85): procSlowFall's height test
 * (pos.y - groundH < 200: chase maxFallSpeed -20 -> -5) is a threshold on a per-step position. A tests it once a frame
 * with the frame-start height (the previous frame's end); B's tick30 step read the half-step height and crossed the
 * threshold a frame early (B 85 step 2, A 86: speed.y slope 1 one frame early). Form: the decision is taken on the
 * frame's first step with the frame-start height (= A's) and held over the frame's later steps; the chase itself still
 * runs per step (60 Hz motion). A frame whose first step did not run procSlowFall (init on a later step) decides on
 * the step itself. NC WWHD_T60_SLOWFALL_NEGCTRL=1 (game build: the per-step test). dt 1: the original. */
static bool s_lk05SfLow;
static uint32_t s_lk05SfFirst = 0xFFFFFFFFu;
static inline bool lk05SlowFallLow(bool low) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_SLOWFALL_NEGCTRL"); return e && e[0] == '1'; }();
#else
    const bool nc = false;
#endif
    if (t60::dt() == 1.0f || nc) {
        return low;
    }
    if (t60::first_step()) {
        s_lk05SfLow = low;
        s_lk05SfFirst = t60_step_counter();
        return low;
    }
    if (s_lk05SfFirst != 0xFFFFFFFFu && t60_step_counter() - s_lk05SfFirst < (uint32_t)t60::steps()) {
        return s_lk05SfLow;
    }
    return low;
}

/* true60 (the step-resolution threshold rule frame-grid form; / uiWP10 / uiWX10 / uiWXfl1 jump 5 205/204):
 * dProcTalk's talk-start step back (setTalkStartBack: ANM_WALK at -1.2, speed 5 away from the partner) ends when Link
 * has passed the back point m370C (its direction more than 0x4000 off his heading) or walks into a wall: a threshold
 * on the per-step position. A tests it once a frame with the frame-start position; B's tick30 step read the half-step
 * position, passed the point a frame early and snapped to it (speedF 5 -> 0 at B 204 step 2, A 205). Form: the decision
 * is taken on the frame's first step with the frame-start position and wall flags (= A's) and held over the frame's
 * later steps; the walk itself still moves per step. NC WWHD_T60_TALKBACK_NEGCTRL=1 (game build: the per-step test).
 * dt 1: the original. */
static bool s_lk05TbPassed;
static uint32_t s_lk05TbFirst = 0xFFFFFFFFu;
static inline bool lk05TalkBackPassed(bool passed) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_TALKBACK_NEGCTRL"); return e && e[0] == '1'; }();
#else
    const bool nc = false;
#endif
    if (t60::dt() == 1.0f || nc) {
        return passed;
    }
    if (t60::first_step()) {
        s_lk05TbPassed = passed;
        s_lk05TbFirst = t60_step_counter();
        return passed;
    }
    if (s_lk05TbFirst != 0xFFFFFFFFu && t60_step_counter() - s_lk05TbFirst < (uint32_t)t60::steps()) {
        return s_lk05TbPassed;
    }
    return passed;
}
/* the frame whose first step snapped to m370C: A's single update ran only the step-back branch that frame, so B's later
 * steps of it skip the mProcVar6 == 0 branch (heading = shape angle, texture switch) too; the next frame runs it as A */
static uint32_t s_lk05TbSnap = 0xFFFFFFFFu;
static inline void lk05TalkBackSnapped() {
    s_lk05TbSnap = (t60::dt() != 1.0f && t60::first_step()) ? t60_step_counter() : 0xFFFFFFFFu;
}
static inline bool lk05TalkBackSnapHeld() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_TALKBACK_NEGCTRL"); return e && e[0] == '1'; }();
#else
    const bool nc = false;
#endif
    return !nc && t60::dt() != 1.0f && s_lk05TbSnap != 0xFFFFFFFFu && t60_step_counter() != s_lk05TbSnap &&
           t60_step_counter() - s_lk05TbSnap < (uint32_t)t60::steps();
}

/* true60 (M2g, pot potP4 @263): the apex frame's root mode. A's apex frame sets
 * ANM_JMPEDS and mRootMoveMode 2 after the per-frame mRootMoveMode = 1, so its one calc keeps the root translation in x /
 * z (jointBeforeCB holds it only in mode 1): the hands, and the carried pot placed at them (setGrabItemPos), sit 1.968
 * forward for that frame (potP4 263; Link's own position exact: posMove's mode 2 moves nothing). B took the apex on the
 * first step (lk05AjApexDecides) and the frame's later steps set mode 1 again, so the frame-end calc held the root: the
 * pot 1.968 back against A at 263 (only the first step's calc had it). Form: the later steps of the apex frame keep
 * mode 2 (A's mode for the frame's calc) up to and including the tick30 step; the next frame's first step is mode 1 as
 * in A. NC WWHD_T60_AJROOT_NEGCTRL=1 (game build: mode 1 on the later steps). dt 1: the original. */
static bool s_lk05AjApexHold;
static inline bool lk05AjRootNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_AJROOT_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}

/* 02419FEC */
void daPy_lk_c::checkNextActionItemFly() {
    if (link05::l_curseType != 1) {
        u16 item = mEquipItem;
        if (checkBowItem(item)) {
            lk05CheckNextActionBowFly(this);
        } else if (item == link05::dItemNo_BOOMERANG_e) {
            lk05CheckNextActionBoomerangFly(this);
        }
    }
}

/* 0241A05C */
BOOL daPy_lk_c::procSideStepLand_init() {
    lk05CommonProcInit(this, link05::daPyProc_SIDE_STEP_LAND_e);
    mNormalSpeed = 0.0f;
    int anm = mDirection == link05::DIR_LEFT ? link05::ANM_ATNJLLAND : link05::ANM_ATNJRLAND;
    // HD: HIO folded
    lk05SetSingleMoveAnime(this, anm, 0.85f, 1.0f, 5, 0.0f);
    mFootEffectPosType = 3;
    setResetFlg0(resetFlg0() | (daPyRFlg0_RIGHT_FOOT_ON_GROUND | daPyRFlg0_LEFT_FOOT_ON_GROUND));
    current.angle.y = (s16)(current.angle.y + 0x8000);
    if (lk05CheckHeavyStateOn(this)) {
        cXyz v;
        v.x = 0.0f;
        v.y = 1.0f;
        v.z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x31, &v);
    }
    return TRUE;
}

/* 0241A154 */
BOOL daPy_lk_c::checkJumpCutFromButton() {
    u16 item = mEquipItem;
    if ((item == link05::daPyItem_SWORD_e && ((resetFlg0() & daPyRFlg0_UNK80) || (mItemTrigger & 2))) ||
        // HD: the A trigger
        (item == link05::daPyItem_BOKO_e && (mItemTrigger & 1) ) ||
        (item == link05::dItemNo_SKULL_HAMMER_e && lk05ItemTrigger(this) &&
         lk05GetReadyItem(this) == link05::dItemNo_SKULL_HAMMER_e)) {
        return lk05ProcJumpCut_init(this, 1);
    }
    return FALSE;
}

/* 0241A204 */
void daPy_lk_c::setParachuteFanModel(f32 frame) {
    J3DAnmTransform* bck = lk05GetItemAnimeResource(this, link05::dRes_INDEX_LKANM_BCK_FANBA_e);
    JKRHeap* oldHeap = lk05SetItemHeap(this);
    J3DModelData* modelData = static_cast<J3DModelData*>(dComIfG_getObjectRes("Link", link05::dRes_INDEX_LINK_BDL_FANB_e, link05::SAFE_STRING_VTBL));
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr,
                                               bck, 0, 0.5f, gabi::ftoi(frame), 0xD,
                                               0, nullptr, 0, 0x11020203);
    Link05EquipmentHD* equipment = reinterpret_cast<Link05EquipmentHD*>(this);
    equipment->mpParachuteFanMorf = morf;
    if (morf == nullptr || morf->mpModel == nullptr) {
        JUT_ASSERT_fail("d_a_player_fan.inc", 0x156, "0");
        morf = equipment->mpParachuteFanMorf;
    }
    equipment->mpEquipItemModel = morf->mpModel;
    lk05SetCurrentHeap(oldHeap);
    equipment->mpEquipItemModel->mUserArea = gabi::ea(this);
    modelData->getJointNodePointer(link05::FANB_JNT_LROOT_e)->setCallBack(lk05ParachuteJointCallback);
    modelData->getJointNodePointer(link05::FANB_JNT_RROOT_e)->setCallBack(lk05ParachuteJointCallback);
    modelData->getJointNodePointer(link05::FANB_JNT_LARMB_e)->setCallBack(lk05ParachuteJointCallback);
    modelData->getJointNodePointer(link05::FANB_JNT_RARMB_e)->setCallBack(lk05ParachuteJointCallback);
}

/* 0241A3C4 */
BOOL daPy_lk_c::procFanGlide_init(int param_0) {
    // HD: reject repeated fan-glide initialization.
    if (mCurProc == link05::daPyProc_FAN_GLIDE_e) {
        return FALSE;
    }
    lk05CommonProcInit(this, link05::daPyProc_FAN_GLIDE_e);
    lk05DeleteEquipItem(this, 0);
    // HD: HIO folded
    f32 start = param_0 != 0 ? 3.0f : 1.0f;
    lk05SetSingleMoveAnime(this, link05::ANM_USEFANB, 0.5f, start, 0xD, 0.0f);
    mEquipItem = link05::daPyItem_UNK102_e;
    setParachuteFanModel(start);
    mProcVar3 = 0;
    mProcVar4 = 0;
    mProcVar2 = 0;
    mProcVar6 = 0;
    m3600 = 0.0f;
    m3604 = 0.0f;
    mProcVar0 = 0;
    mProcVar5 = 0;
    mWindSpeed.copy(link05::l_zero /* cXyz::Zero */);
    lk05VoiceStart(this, 5);
    seStartOnlyReverb(link05::JA_SE_LK_FAN_CHUTE_OPEN);
    mProcVar7 = 0x28;
    if (!lk05CheckHeavyStateOn(this)) {
        dComIfGp_setItemMagicCount(-1);
    }
    dComIfGp_setPlayerStatus1(0, link05::daPyStts1_DEKU_LEAF_FLY_e);
    mProcVar1 = 0x14;
    return TRUE;
}

/* 0241A578 */
BOOL daPy_lk_c::checkFanGlideProc(int param_0) {
    if (lk05CheckSetItemTrigger(this, link05::dItemNo_DEKU_LEAF_e, 1)) {
        if (dComIfGs_getMagic() != 0) { /* dComIfGs_getMagic() >= 1 */
            return procFanGlide_init(param_0);
        }
        lk05SeStartSystem(link05::JA_SE_ITEM_TARGET_OUT);
    }
    return FALSE;
}

/* 0241A604 */
BOOL daPy_lk_c::procSideStep() {
    // HD: m_HIO->mSideStep.m.field_0x0 folded (on)
    fopAc_ac_c* lockOn = mpAttnActorLockOn;
    if (lockOn != nullptr) {
        s16 a = fopAcM_searchActorAngleY(this, lockOn);
        cLib_addCalcAngleS(&shape_angle.y, a, 5, 0x5E8, 0x13C);
    }
    if (mDirection == link05::DIR_LEFT) {
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
        if (current.pos.y < mProcStartPos.y - 50.0f) {
            lk05ProcFall_init(this, 2, 14.0f);
        }
    }
    lk05CheckItemChangeFromButton(this);
    return TRUE;
}

/* 0241A718 */
BOOL daPy_lk_c::procSideStepLand() {
    lk05ResetFootEffect(this);
    if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        lk05CheckNextMode(this, 0);
    } else if (mFrameCtrlUnder[0].getFrame() > 1.5f) {
        lk05CheckNextMode(this, 1);
    }
    return TRUE;
}

/* 0241A798 */
BOOL daPy_lk_c::procCrouchDefense() {
    dComIfGp_setRStatus(link05::dActStts_DEFEND_e);
    if (lk05CameraChangeModeOK(lk05GetCameraBody(), 4) &&
        !(current.pos.y < mWaterY)) {
        setResetFlg0(resetFlg0() | daPyRFlg0_SUBJECT_ACCEPT);
        if ((dComIfGp_getCameraAttentionStatus(mCameraInfoIdx) & 0x1000) && !dComIfGp_event_runCheck()) {
            return lk05ProcSubjectivity_init(this, 1);
        }
    }
    // HD: HIO folded
    LKT60_CHASE_S(0, &mNormalSpeed, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, 0.6f, 2.5f, 1.8f)); /* true60: frame-exact */
    u32 att = mpAttention;
    if (lk05CheckAttentionLock(att) || !(mItemButton & 0x40)) {
        mDirection = link05::DIR_RIGHT;
        current.angle.y = shape_angle.y;
        lk05CheckNextMode(this, 0);
        return TRUE;
    }
    u16 a = (u16)(mMoveTargetAngleY - shape_angle.y);
    f32 f = mStickDistance * cM_scos(a);
    // HD: camera option
    if (link05::cameraReverse() == 0) {
        f = -f;
    }
    if (!(f < 0.0f)) {
        cLib_addCalcAngleS(&mBodyAngle.x, (s16)gabi::ftoi(2500.0f * f), 4, 0xC00, 0x180);
    } else {
        cLib_addCalcAngleS(&mBodyAngle.x, (s16)gabi::ftoi(8500.0f * f), 4, 0xC00, 0x180);
    }
    cLib_addCalcAngleS(&mBodyAngle.y, (s16)gabi::ftoi(10000.0f * mStickDistance * cM_ssin(a)), 4, 0xC00, 0x180);
    // HD: turn the body with the stick held sideways
    f32 stick = mStickDistance;
    if (stick > 0.9f) {
        f32 s = cM_ssin(a);
        if (std::fabs(s) > 0.91f || std::fabs(cM_ssin(mStickAngle)) > 0.95f) {
            s16 cur = shape_angle.y;
            cLib_addCalcAngleS(&shape_angle.y, (s16)(cur + (s16)gabi::ftoi(512.0f * (stick - 0.5f) * s)), 4, 0x40, 0x10);
        }
    }
    return TRUE;
}

/* 0241AB04 */
BOOL daPy_lk_c::procNockBackEnd_init() {
    lk05CommonProcInit(this, link05::daPyProc_NOCK_BACK_END_e);
    // HD: HIO folded
    lk05SetSingleMoveAnime(this, link05::ANM_ROLLFMIS, 0.8f, 8.0f, 0x18, 3.0f);
    mNormalSpeed = 0.0f;
    return TRUE;
}

/* 0241AB6C */
BOOL daPy_lk_c::procCrouchDefenseSlip() {
    dComIfGp_setRStatus(link05::dActStts_DEFEND_e);
    /* true60 (the 30 Hz timer rule; the first-step AI-timer rule for Link): the slide's minimum length counts 30 Hz frames: on the
     * frame's FIRST step (count before the end test, as in the original: no hold) */
    if (mProcVar0 > 0 && lkT60_countStep()) {
        mProcVar0 = mProcVar0 - 1;
    }
    if (mEquipItem == link05::dItemNo_SKULL_HAMMER_e) {
        lk05CopyFloatBits(&m35EC, &mFrameCtrlUnder[0].mFrame);
    } else {
        lk05CopyFloatBits(&m35E8, &mFrameCtrlUnder[0].mFrame);
    }
    f64 r = LKT60_CHASE(3, &mNormalSpeed, 0.0f, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, m35A0, m35A4, m35A8)); /* true60: frame-exact */
    if (!(r > 0.001f)) {
        if (mProcVar6 != 0) {
            return procNockBackEnd_init();
        }
        if (mProcVar0 == 0) {
            return lk05ProcCrouchDefense_init(this);
        }
    } else if (mNormalSpeed < m35AC) {
        lk05ResetFootEffect(this);
    }
    return TRUE;
}

/* 0241AC94 */
BOOL daPy_lk_c::procCrouch() {
    dComIfGp_setRStatus(link05::dActStts_CROUCH_e);
    if (lk05CameraChangeModeOK(lk05GetCameraBody(), 4) &&
        !(current.pos.y < mWaterY)) {
        setResetFlg0(resetFlg0() | daPyRFlg0_SUBJECT_ACCEPT);
        if ((dComIfGp_getCameraAttentionStatus(mCameraInfoIdx) & 0x1000) && !dComIfGp_event_runCheck()) {
            return lk05ProcSubjectivity_init(this, 1);
        }
    }
    // HD: HIO folded
    LKT60_CHASE_S(1, &mNormalSpeed, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, 0.6f, 2.5f, 1.8f)); /* true60: frame-exact */
    if (!(mItemButton & 0x40)) {
        lk05CheckNextMode(this, 0);
        return TRUE;
    }
    {
        u32 att = mpAttention;
        if (lk05CheckAttentionLock(att) && dComIfGs_getSelectEquip(1) != 0xFF /* checkShieldEquip() */ &&
            lk05CheckGuardAccept(this)) {
            lk05CheckNextMode(this, 0);
            return TRUE;
        }
    }
    if (link05::animation(this)->m_old_fdata->mOldFrameRate.get() < 0.01f && mStickDistance > 0.05f && !(mWaterY > current.pos.y + 15.0f) /* !checkCrawlWaterIn() */) {
        cXyz front;
        cXyz top;
        cXyz vec;
        PSMTXTrans(&link05::l_matrixStack /* mDoMtx_stack_c */, current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_ZXYrotM(&link05::l_matrixStack, mGroundAngle, shape_angle.y, 0);
        PSMTXMultVec(&link05::l_matrixStack, &link05::l_crawl_start_front_offset /* link05::l_crawl_start_front_offset */, &front);
        PSMTXMultVec(&link05::l_matrixStack, &link05::l_crawl_top_offset /* link05::l_crawl_top_offset */, &top);
        if (lk05GetCrawlMoveVec(this, &top, &front, &vec)) {
            cXyz p;
            cXyz d;
            cXyz xz;
            cXyz_mi(&current.pos, &p, &vec);
            f32 y = p.y + 5.0f;
            reinterpret_cast<dBgS_GndChk*>(mGndChk)->m_pos.z = p.z; /* mGndChk.SetPos(&p) */
            reinterpret_cast<dBgS_GndChk*>(mGndChk)->m_pos.x = p.x;
            p.y = y;
            reinterpret_cast<dBgS_GndChk*>(mGndChk)->m_pos.y = y;
            p.y = (f32)lk05GroundCross(dComIfG_Bgsp(), reinterpret_cast<dBgS_GndChk*>(mGndChk));
            cXyz_mi(&current.pos, &d, &p);
            xz.x = d.x;
            xz.y = 0.0f;
            xz.z = d.z;
            f64 len = lk05Sqrtf(lk05VecSquareMag(&xz));
            s16 ang = lk05Atan2s_d(-d.y, len);
            if (cLib_distanceAngleS(ang, mGroundAngle) > 0x100) {
                return TRUE;
            }
        }
        lk05ProcCrawlStart_init(this);
    }
    return TRUE;
}

/* 0241AF64 */
BOOL daPy_lk_c::procWaitTurn() {
    // HD: HIO folded
    LKT60_CHASE_S(2, &mNormalSpeed, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, 0.6f, 2.5f, 1.8f)); /* true60: frame-exact */
    if (lk05ChangeSlideProc(this)) {
        return TRUE;
    }
    // HD: HIO folded
    s16 r = cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 0x1E, 0x3CDF, 0x1F40);
    current.angle.y = shape_angle.y;
    if (lk05CheckNextActionFromButton(this)) {
        return TRUE;
    }
    if (r == 0) {
        if (dComIfGp_event_runCheck() && mDemo.getDemoMode() == 5 /* daPy_demo_c::DEMO_WAIT_TURN_e */) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
            return TRUE;
        }
        lk05CheckNextMode(this, 0);
    }
    return TRUE;
}

/* 0241B050 */
BOOL daPy_lk_c::procMoveTurn() {
    // HD: HIO folded
    /* true60 (d_a_player_waitturn_t60.h): the frame's start / turn for the MOVE catch-up below */
    const bool lkT60Mt = lkT60_waitTurnStep() && !lkT60_moveTurnTurnNegctrl();
    const s16 lkT60Pv0 = lkT60Mt ? (s16)mProcVar0 : (s16)0, lkT60Pv2 = lkT60Mt ? (s16)mProcVar2 : (s16)0,
              lkT60Pv3 = lkT60Mt ? (s16)mProcVar3 : (s16)0;
    if (lkT60Mt)
        lkT60_waitTurnBegin(current.angle.y, shape_angle.y, (f32)mNormalSpeed);
    const bool lkT60MidOn = lkT60_midStep(); /* true60: the chase's proc vars on a middle step */
    const s16 lkT60Pv0m = lkT60MidOn ? (s16)mProcVar0 : (s16)0, lkT60Pv2m = lkT60MidOn ? (s16)mProcVar2 : (s16)0,
              lkT60Pv3m = lkT60MidOn ? (s16)mProcVar3 : (s16)0;
    lk05SetSpeedAndAngleNormal(this, 0xBB8);
    cLib_addCalcAngleS(&shape_angle.y, current.angle.y, mProcVar0, mProcVar2, mProcVar3);
    if (lkT60Mt)
        lkT60_waitTurnEnd(current.angle.y, shape_angle.y, (f32)mNormalSpeed);
    /* true60: a middle step's turn result, to see whether the entered MOVE's init kept it */
    const LkT60WaitTurn* lkT60Mid = lkT60_midEntryRecord();
    const s16 lkT60MidA = lkT60Mid ? (s16)current.angle.y : (s16)0, lkT60MidS = lkT60Mid ? (s16)shape_angle.y : (s16)0;
    const f32 lkT60MidV = lkT60Mid ? (f32)mNormalSpeed : 0.0f;
    if (!lk05CheckNextMode(this, 0)) {
        lk05SetBlendMoveAnime(this, -1.0f);
    } else if (lkT60Mt) {
        if (const LkT60WaitTurn* t = lkT60_moveTurnCatchUp(gabi::ea(&mCurProc), (s32)mCurProc, current.angle.y,
                                                           shape_angle.y, (f32)mNormalSpeed)) {
            /* true60: MOVE entered on the frame's first step: the frame's whole turn and shape
             * chase from its start at dt 1, as A turned before it entered MOVE; MOVE's blend from it */
            f32 previousDt = t60_set_dt(1.0f);
            const s32 proc = mCurProc;
            mCurProc = kLkT60WaitTurnMoveTurn;
            current.angle.y = t->angle;
            shape_angle.y = t->shape;
            mNormalSpeed = t->speed;
            lk05SetSpeedAndAngleNormal(this, 0xBB8);
            cLib_addCalcAngleS(&shape_angle.y, current.angle.y, lkT60Pv0, lkT60Pv2, lkT60Pv3);
            mCurProc = proc;
            if (current.angle.y != t->angle1 || shape_angle.y != t->shape1 || (f32)mNormalSpeed != t->speed1)
                lk05SetBlendMoveAnime(this, -1.0f); /* the init's blend from the frame's turn; no new morf */
            t60_set_dt(previousDt);
        }
    } else if (lkT60Mid && (s32)mCurProc == kLkT60WaitTurnMove && current.angle.y == lkT60MidA &&
               shape_angle.y == lkT60MidS && (f32)mNormalSpeed == lkT60MidV) {
        /* true60: MOVE entered on a middle step: the frame's whole turn and shape chase from its start
         * at dt 1 (A's one turn before its frame-end entry), MOVE's blend from it, and MOVE's body held for the rest of
         * the frame */
        f32 previousDt = t60_set_dt(1.0f);
        const s32 proc = mCurProc;
        mCurProc = kLkT60WaitTurnMoveTurn;
        current.angle.y = lkT60Mid->angle;
        shape_angle.y = lkT60Mid->shape;
        mNormalSpeed = lkT60Mid->speed;
        lk05SetSpeedAndAngleNormal(this, 0xBB8);
        cLib_addCalcAngleS(&shape_angle.y, current.angle.y, lkT60Pv0m, lkT60Pv2m, lkT60Pv3m);
        mCurProc = proc;
        lk05SetBlendMoveAnime(this, -1.0f);
        t60_set_dt(previousDt);
        g_lkT60MidHold.tag = lkT60_frameTag();
        g_lkT60MidHold.proc = proc;
        g_lkT60MidHold.valid = true;
    }
    return TRUE;
}

/* 0241B0C0 */
BOOL daPy_lk_c::procSlip() {
    // HD: HIO folded
    f64 r = LKT60_CHASE(5, &mNormalSpeed, 0.0f, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, 0.6f, 1.25f, 0.1875f)); /* true60: frame-exact */
    if (!(r > 0.001f)) {
        if (mStickDistance > 0.05f) {
            s16 a = shape_angle.y;
            current.angle.y = (s16)(a + 0x8000);
            shape_angle.y = (s16)(a + 0x100);
            mNormalSpeed = mMaxNormalSpeed  * 0.5f;
            lk05ProcMoveTurn_init(this, 0);
        } else {
            lk05CheckNextMode(this, 0);
        }
        return TRUE;
    }
    if (!(mAcch.m_flags & 0x10) /* !ChkWallHit() */) {
        if (mNormalSpeed < 2.5f) {
            lk05ResetFootEffect(this);
        }
        u16 ang = current.angle.y;
        f32 y = current.pos.y + 2.5f;
        cXyz start;
        cXyz end;
        start.z = current.pos.z;
        start.y = y;
        f32 sz = current.pos.z;
        f32 sx = current.pos.x;
        end.y = y;
        start.x = sx;
        end.z = gabi::fmadds(50.0f, cM_scos(ang), sz);
        end.x = gabi::fmadds(50.0f, cM_ssin(ang), sx);
        lk05LineSet(reinterpret_cast<dBgS_LinChk*>(mLinkLinChk), &start, &end, this);
        if (!lk05LineCross(dComIfG_Bgsp(), reinterpret_cast<dBgS_LinChk*>(mLinkLinChk))) {
            return TRUE;
        }
        cM3dGPla* pla = lk05GetTriPla(dComIfG_Bgsp(), (u32)reinterpret_cast<dBgS_LinChk*>(mLinkLinChk)->mBgIndex,
                                  (u32)reinterpret_cast<dBgS_LinChk*>(mLinkLinChk)->mPolyIndex);
        if (pla == 0) {
            return TRUE;
        }
        f32 ny = pla->mNormal.y.get();
        if (!(ny < 0.5f) || ny < -0.8f) { /* !cBgW_CheckBWall(ny) */
            return TRUE;
        }
    }
    mNormalSpeed = 0.0f;
    lk05CheckNextMode(this, 0);
    return TRUE;
}

/* 0241B2BC */
BOOL daPy_lk_c::procSlideFrontLand_init() {
    lk05CommonProcInit(this, link05::daPyProc_SLIDE_FRONT_LAND_e);
    // HD: HIO folded
    lk05SetSingleMoveAnime(this, link05::ANM_SLIDEFLAND, 0.8f, 2.0f, 9, 5.0f);
    mNormalSpeed = mNormalSpeed * 0.5f;
    return TRUE;
}

// HD: ITest62 rotates the slide direction by the debug timer; its name is a SafeString.
/* inlined into daPy_lk_c::procSlideFront and procSlideBack */
static inline s16 lk05SlideAngle(s16 ang, const char* testName) {
    SafeString name;
    name.mTextPtr = gabi::guest_addr(testName); /* "ITest62" */
    name.__vtbl = link05::SAFE_STRING_VTBL;
    SafeString stage;
    stage.mTextPtr = gabi::ea(dComIfGp_getStartStageName()); /* dComIfGp_getStartStageName() */
    stage.__vtbl = link05::SAFE_STRING_VTBL;
    lk05AssureTermination(&name);
    lk05AssureTermination(&name);
    u32 a = name.mTextPtr;
    lk05AssureTermination(&stage);
    u32 b = stage.mTextPtr;
    bool equal = true;
    if (a != b) {
        equal = false;
        for (u32 i = 0; i < 0x40001; i++) {
            u8 ca = lk05GuestByte(a + i);
            u8 cb = lk05GuestByte(b + i);
            if (ca != cb) break;
            if (ca == 0) {
                equal = true;
                break;
            }
        }
    }
    if (equal) {
        ang = (s16)(ang + ((u32)link05::g_Counter.mTimer << 10));
    }
    return ang;
}

/* 0241B32C */
BOOL daPy_lk_c::procSlideFront() {
    cM3dGPla* pla = lk05GetSlidePolygon(this);
    if (pla == 0) {
        procSlideFrontLand_init();
        return TRUE;
    }
    s16 ang = cM_atan2s(pla->mNormal.x.get(), pla->mNormal.z.get());
    ang = lk05SlideAngle(ang, "ITest62");
    cLib_addCalcAngleS(&current.angle.y, ang, 4, 0x1000, 0x400);
    cLib_addCalcAngleS(&shape_angle.y, current.angle.y, 4, 0x1000, 0x400);
    f32 ny = pla->mNormal.y.get();
    s16 cur = current.angle.y;
    // HD: HIO folded
    f32 k = 3.0f  * gabi::fmadds(0.5f, 1.0f - ny, 1.0f);
    f32 c = cM_scos((s16)(cur - ang));
    /* true60 (the per-step movement rule): the slope's push per update: x dt */
    if (t60::dt() != 1.0f) {
        k = k * (f32)t60::dt();
    }
    f32 spd = gabi::fmadds(k, c, mNormalSpeed);
    f32 max = mMaxNormalSpeed;
    mNormalSpeed = spd;
    if (spd > max) {
        mNormalSpeed = max;
    }
    /* true60 (check item, the one-emitter-per-event rule): sustained start on every update of the slide: on the frame's step */
    if (lk05FrameStep())
        seStartMapInfo(link05::JA_SE_LK_SLIP_SUS);
    return TRUE;
}

/* 0241B51C */
BOOL daPy_lk_c::procSlideBackLand_init() {
    lk05CommonProcInit(this, link05::daPyProc_SLIDE_BACK_LAND_e);
    // HD: HIO folded
    lk05SetSingleMoveAnime(this, link05::ANM_SLIDEBLAND, 1.0f, 2.0f, 8, 5.0f);
    return TRUE;
}

/* 0241B578 */
BOOL daPy_lk_c::procSlideBack() {
    cM3dGPla* pla = lk05GetSlidePolygon(this);
    if (pla == 0) {
        procSlideBackLand_init();
        return TRUE;
    }
    s16 ang = cM_atan2s(pla->mNormal.x.get(), pla->mNormal.z.get());
    ang = lk05SlideAngle(ang, "ITest62");
    cLib_addCalcAngleS(&current.angle.y, ang, 4, 0x1000, 0x400);
    cLib_addCalcAngleS(&shape_angle.y, (s16)(current.angle.y + 0x8000), 4, 0x1000, 0x400);
    s16 cur = current.angle.y;
    f32 ny = pla->mNormal.y.get();
    // HD: HIO folded
    f32 k = 3.0f  * gabi::fmadds(0.5f, 1.0f - ny, 1.0f);
    f32 c = cM_scos((s16)(cur - ang));
    /* true60 (the per-step movement rule): the slope's push per update: x dt */
    if (t60::dt() != 1.0f) {
        k = k * (f32)t60::dt();
    }
    f32 spd = gabi::fmadds(k, c, mNormalSpeed);
    f32 max = mMaxNormalSpeed;
    mNormalSpeed = spd;
    if (spd > max) {
        mNormalSpeed = max;
    }
    /* true60 (check item, the one-emitter-per-event rule): sustained start on every update of the slide: on the frame's step */
    if (lk05FrameStep())
        seStartMapInfo(link05::JA_SE_LK_SLIP_SUS);
    return TRUE;
}

/* 0241B774 */
BOOL daPy_lk_c::procSlideFrontLand() {
    LKT60_CHASE_S(6, &mNormalSpeed, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, 0.5f, 5.0f, 1.0f)); /* true60: frame-exact */
    if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        current.angle.y = shape_angle.y;
        lk05CheckNextMode(this, 0);
    // HD: HIO folded
    } else if (mFrameCtrlUnder[0].getFrame() > 6.0f ) {
        s16 a = shape_angle.y;
        s16 old = current.angle.y;
        current.angle.y = a;
        if (!lk05CheckNextMode(this, 1)) {
            current.angle.y = old;
        }
    }
    return TRUE;
}

/* 0241B83C */
BOOL daPy_lk_c::procSlideBackLand() {
    LKT60_CHASE_S(7, &mNormalSpeed, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, 0.5f, 5.0f, 1.0f)); /* true60: frame-exact */
    if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        current.angle.y = shape_angle.y;
        lk05CheckNextMode(this, 0);
    // HD: HIO folded
    } else if (mFrameCtrlUnder[0].getFrame() > 6.0f ) {
        s16 a = shape_angle.y;
        s16 old = current.angle.y;
        current.angle.y = a;
        if (!lk05CheckNextMode(this, 1)) {
            current.angle.y = old;
        }
    }
    return TRUE;
}

/* 0241B904 */
BOOL daPy_lk_c::procFrontRollCrash_init() {
    lk05CommonProcInit(this, link05::daPyProc_FRONT_ROLL_CRASH_e);
    // HD: HIO folded
    lk05SetSingleMoveAnime(this, link05::ANM_ROLLFMIS, 0.0f, 6.0f, 0x18, 1.0f);
    mNoResetFlg0 = mNoResetFlg0 & ~8u; /* offNoResetFlg0(daPyFlg0_UNK8) */
    current.angle.y = (s16)(current.angle.y - 0x8000);
    mNormalSpeed = speedF * 0.4f;
    speed.y = 7.0f;
    {
        cXyz v;
        v.x = 0.0f;
        v.y = 1.0f;
        v.z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x31, &v);
    }
    lk05VoiceStart(this, 8);
    lk05SeStart(link05::JA_SE_LK_BODYATTACK, &current.pos, (u32)mFrollCrashMtrlSndId, (s32)(s8)mReverb);
    setResetFlg0(resetFlg0() | 0x2000 /* daPyRFlg0_FRONT_ROLL_CRASH */);
    {
        cXyz pos;
        lk05CopyFloatBits(&pos.z, &current.pos.z);
        lk05CopyFloatBits(&pos.y, &current.pos.y);
        lk05CopyFloatBits(&pos.x, &current.pos.x);
        lk05SoundSet(&pos, 100, fopAcM_GetID(this) /* fopAcM_GetID */, 5);
    }
    if ((mAcch.m_flags & 0x10) && (mAcchCir[0].m_flags & 2)) { /* ChkWallHit() */
        fopAc_ac_c* actor = lk05GetActorPointer(dComIfG_Bgsp(), (u32)mAcchCir[0].mBgIndex);
        if (actor != 0 && actor->mProcName.get() == link05::fpcNm_Obj_Movebox_e) {
            /* daObjMovebox::Act_c::set_rollCrash() */
            s32 mode = static_cast<Link05MoveboxHD*>(actor)->mType.get();
            if (mode == 0 || mode == 5) {
                static_cast<Link05MoveboxHD*>(actor)->mbRollCrash = 1;
            }
        }
    }
    return TRUE;
}

/* 0241BAB8 */
BOOL daPy_lk_c::procFrontRoll() {
    if (mFrameCtrlUnder[0].getFrame() > 6.0f) {
        mFootEffectPosType = 4;
        lk05EndFlameDamageEmitter(this);
    }
    if (lk05GetSlidePolygon(this) != 0) {
        LKT60_CHASE_S(8, &mNormalSpeed, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, 0.5f, 2.5f, 0.1f)); /* true60: frame-exact */
    }
    if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        if (!(mStickDistance > 0.05f)) {
            // HD: HIO folded
            mNormalSpeed = mNormalSpeed - 5.0f;
        }
        lk05CheckNextMode(this, 0);
    } else if (lkT60_rollEndFrame(&mFrameCtrlUnder[0]) > 17.0f) { /* true60: frame end */
        lk05CheckNextMode(this, 1);
    } else if (!(speedF < 10.0f) && !(noResetFlg1() & daPyFlg1_UNK100000)) {
        if (!(mNoResetFlg0 & 8)) {
            if (mProcVar6 == 0 || !(mAcch.m_flags & 0x10) || !(mAcchCir[0].m_flags & 2)) {
                return TRUE;
            }
            if (cLib_distanceAngleS((s16)(current.angle.y + 0x8000), mAcchCir[0].GetWallAngleY() ) > 0x1388) {
                return TRUE;
            }
            f32 frame = mFrameCtrlUnder[0].getFrame();
            if (frame < 6.0f || frame > 15.0f) {
                return TRUE;
            }
            if (!(mNoResetFlg0 & 8)) {
                mFrollCrashMtrlSndId = lk05GetMtrlSndId(dComIfG_Bgsp(), &mAcchCir[0]);
            }
        }
        procFrontRollCrash_init();
    }
    return TRUE;
}

/* 0241BC90 */
BOOL daPy_lk_c::procFrontRollCrash() {
    if (!(mModeFlg & link05::ModeFlg_MIDAIR)) {
        if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
            lk05CheckNextMode(this, 0);
        } else if (lkT60_rollEndFrame(&mFrameCtrlUnder[0]) > 20.0f) { /* true60: frame end */
            lk05CheckNextMode(this, 1);
        }
    }
    if (mAcch.ChkGroundHit() && (mModeFlg & 2)) {
        mNormalSpeed = 0.0f;
        mFrameCtrlUnder[0].setRate(0.7f);
        lk05VoiceStart(this, 9);
        mModeFlg = (mModeFlg & ~2u) | 0x8000;
    }
    return TRUE;
}

/* 0241BD70 */
BOOL daPy_lk_c::procNockBackEnd() {
    if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        lk05CheckNextMode(this, 0);
    } else if (lkT60_rollEndFrame(&mFrameCtrlUnder[0]) > 16.0f) { /* true60: frame end */
        lk05CheckNextMode(this, 1);
    }
    return TRUE;
}

/* 0241BDD0 */
BOOL daPy_lk_c::procSideRoll_init() {
    lk05CommonProcInit(this, link05::daPyProc_SIDE_ROLL_e);
    int anm = (s16)(current.angle.y - shape_angle.y) > 0 ? link05::ANM_MROLLL : link05::ANM_MROLLR;
    // HD: HIO folded
    lk05SetSingleMoveAnime(this, anm, 1.3f, 6.0f, 0x16, 6.0f);
    mNormalSpeed = 26.0f;
    if (lk05CheckHeavyStateOn(this)) {
        mNormalSpeed = mNormalSpeed * 0.5f;
    }
    mFootEffectPosType = 4;
    return TRUE;
}

/* 0241BE78 */
BOOL daPy_lk_c::procSideRoll() {
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (lkT60_endRate(&frameCtrl) < 0.01f) {
        current.angle.y = shape_angle.y;
        if (!(mStickDistance > 0.05f)) {
            mNormalSpeed = 0.0f;
        }
        lk05CheckNextMode(this, 0);
    } else if (lkT60_pass(&frameCtrl, 10.0f)) {
        lk05EndFlameDamageEmitter(this);
    // HD: HIO folded
    } else if (frameCtrl.getFrame() > 19.0f ) {
        s16 a = shape_angle.y;
        u32 spd = gmem_ld32(gabi::ea(&mNormalSpeed));
        s16 old = current.angle.y;
        current.angle.y = a;
        if (!lk05CheckNextMode(this, 1)) {
            gmem_stf32(gabi::ea(&mNormalSpeed), spd);
            current.angle.y = old;
        }
    }
    return TRUE;
}

/* 0241BF80 */
BOOL daPy_lk_c::procBackJumpLand_init() {
    lk05CommonProcInit(this, link05::daPyProc_BACK_JUMP_LAND_e);
    mNormalSpeed = 0.0f;
    // HD: HIO folded
    lk05SetSingleMoveAnime(this, link05::ANM_ROLLBLAND, 0.8f, 0.0f, 5, 0.0f);
    mFootEffectPosType = 3;
    setResetFlg0(resetFlg0() | 0xC00);
    current.angle.y = shape_angle.y;
    if (lk05CheckHeavyStateOn(this)) {
        cXyz v;
        v.x = 0.0f;
        v.y = 1.0f;
        v.z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x31, &v);
    }
    u16 item = mEquipItem;
    if ((item == link05::daPyItem_SWORD_e || item == link05::daPyItem_BOKO_e) && link05::l_curseType != 1 &&
        reinterpret_cast<Link05AnmHeapHD*>(m_anm_heap_upper)[2].mIdx == 0xFFFF /* checkNoUpperAnime() */) {
        mProcVar6 = 1;
    } else {
        mProcVar6 = 0;
    }
    return TRUE;
}

/* 0241C094 */
BOOL daPy_lk_c::procBackJump() {
    if (mAcch.ChkGroundHit() && lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        procBackJumpLand_init();
    } else {
        if (checkFanGlideProc(0)) {
            return TRUE;
        }
        if (current.pos.y < mProcStartPos.y - 30.0f) {
            // HD: HIO folded
            lk05ProcFall_init(this, 2, 10.0f);
        }
    }
    lk05CheckItemChangeFromButton(this);
    return TRUE;
}

/* 0241C140 */
BOOL daPy_lk_c::procBackJumpLand() {
    lk05ResetFootEffect(this);
    if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        if (dComIfGp_event_runCheck()) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        } else {
            lk05CheckNextMode(this, 0);
        }
    } else if (mFrameCtrlUnder[0].getFrame() > 2.0f) {
        lk05CheckNextMode(this, 1);
    } else if (mProcVar6 != 0) {
        s32 v = mStickSpinAngle;
        mProcVar6 = 0;
        if ((v < 0 ? -v : v) > 0xF800 && mEquipItem == link05::daPyItem_SWORD_e) {
            lk05ProcCutTurn_init(this, 1);
        }
    }
    return TRUE;
}

/* 0241C244 */
BOOL daPy_lk_c::procShipJumpRide_init() {
    Link05ShipHD* ship = static_cast<Link05ShipHD*>(dComIfGp_getShipActor());
    if (ship == nullptr || !(ship->attention_info.flags & fopAc_Attn_ACTION_SHIP_e)) {
        return FALSE;
    }
    lk05CommonProcInit(this, link05::daPyProc_SHIP_JUMP_RIDE_e);
    lk05DeleteEquipItem(this, 1);
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    speed.y = 0.0f;
    ship->mNextMode = 2;                                 /* ship->setPaddleMove() */
    ship->mStateFlag |= 0x2000; /* ship->onJumpRideFlg() */
    lk05SetSingleMoveAnime(this, link05::ANM_SLIPICE, 1.0f, 0.0f, -1, 5.0f);
    lk05SetShipRidePos(this, 0);
    mNoResetFlg0 = mNoResetFlg0 & ~(u32)daPyFlg0_SHIP_DROP;
    dComIfGp_setPlayerStatus1(0, 0x80);
    seStartOnlyReverb(link05::JA_SE_LK_FT_JUMP_SHIP);
    return TRUE;
}

/* 0241C344 */
BOOL daPy_lk_c::checkJumpRideShip() {
    if (mCyl.ChkCoHit()) {
        fopAc_ac_c* ac = mCyl.GetCoHitAc();
        if (ac != 0 && ac->mProcName.get() == link05::fpcNm_SHIP_e) {
            /* ship->getBodyMtx(): the body model's base matrix (NULL-preserving) */
            J3DModel* model = static_cast<Link05ShipHD*>(ac)->mpBodyAnm->mpModel;
            cXyz p;
            cXyz d;
            cXyz xz;
            PSMTXMultVec((model ? &model->mBaseTransformMtx : nullptr), &link05::l_ship_offset /* link05::l_ship_offset */, &p);
            cXyz_mi(&old.pos, &d, &p);
            if (d.y > 5.0f) {
                xz.x = d.x;
                xz.z = d.z;
                xz.y = 0.0f;
                if (lk05VecSquareMag(&xz) < 10000.0f) {
                    return procShipJumpRide_init();
                }
            }
        }
    }
    return FALSE;
}

/* 0241C448 */
BOOL daPy_lk_c::procLandDamage_init(int param_1) {
    cXyz v;
    if (param_1 == 2) {
        if (!dComIfGp_event_runCheck()) {
            lk05SetDamagePoint(this, -2.0f);
        }
        v.x = 0.0f;
        v.y = 1.0f;
        v.z = 0.0f;
        dComIfGp_getVibration_StartShock(7, -0x21, &v);
    } else {
        dComIfG_inf_c* play = link05::gameInfo();
        if (param_1 == 1) {
            if (!play->play.mEvtCtrl.mMode) {
                lk05SetDamagePoint(this, -1.0f);
            }
            v.x = 0.0f;
            v.y = 1.0f;
            v.z = 0.0f;
            dComIfGp_getVibration_StartShock(5, -0x21, &v);
        } else {
            v.x = 0.0f;
            v.y = 1.0f;
            v.z = 0.0f;
            lk05StartShock(&play->play.mVibration, 2, -0x21, &v);
        }
    }
    lk05CommonProcInit(this, link05::daPyProc_LAND_DAMAGE_e);
    mNormalSpeed = 0.0f;
    if (param_1 == 0) {
        mProcVar0 = 3;
        mModeFlg = mModeFlg & ~8u; /* offModeFlg(ModeFlg_DAMAGE) */
    } else {
        mProcVar0 = 0x1E;
        mDamageWaitTimer = 0x1E;
    }
    // HD: HIO folded
    lk05SetSingleMoveAnime(this, link05::ANM_LANDDAMA, 0.8f, 4.0f, 9, 0.0f);
    mProcVar6 = 0;
    seStartOnlyReverb(link05::JA_SE_LK_FALL_DAMAGE);
    lk05VoiceStart(this, 13);
    mFootEffectPosType = 5;
    return TRUE;
}

/* 0241C69C */
BOOL daPy_lk_c::procVomitLand_init() {
    lk05CommonProcInit(this, link05::daPyProc_VOMIT_LAND_e);
    mNormalSpeed = 0.0f;
    // HD: HIO folded
    lk05SetSingleMoveAnime(this, link05::ANM_LANDDAMA, 1.0f, 4.0f, 9, 0.0f);
    mProcVar0 = 7;
    mFootEffectPosType = 5;
    mProcVar6 = 0;
    return TRUE;
}

/* 0241C714 */
BOOL daPy_lk_c::procLand_init(f32 param_1, int param_2) {
    lk05CommonProcInit(this, link05::daPyProc_LAND_e);
    mNormalSpeed = 0.0f;
    if (param_2 != 0) {
        // HD: HIO folded
        lk05SetSingleMoveAnime(this, link05::ANM_JMPEDS, param_1, 2.0f, 0xC, 5.0f);
    } else {
        lk05SetSingleMoveAnime(this, link05::ANM_JMPEDS, param_1, 3.0f, 0xB, 0.0f);
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
    if (lk05CheckHeavyStateOn(this)) {
        cXyz v;
        v.x = 0.0f;
        v.y = 1.0f;
        v.z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x31, &v);
    }
    return TRUE;
}

/* 0241C898 */
BOOL daPy_lk_c::changeLandProc(f32 param_1) {
    f32 posY = current.pos.y;
    f32 dist = mFallStartY - posY;
    // HD: HIO folded
    if (mCurrAttributeCode != link05::dBgS_Attr_GIANT_FLOWER_e && !(dist < 2000.0f)) {
        if (!(dist < 6000.0f)) {
            return procLandDamage_init(2);
        }
        return procLandDamage_init(1);
    }
    int direction = getDirectionFromAngle((s16)(current.angle.y - shape_angle.y));
    s32 proc = mCurProc;
    BOOL bVar2;
    if (proc == link05::daPyProc_CUT_EX_MJ_e && (m34C5 != 0 || (mNoResetFlg0 & 4))) {
        bVar2 = TRUE;
    } else {
        bVar2 = FALSE;
        u16 anm = reinterpret_cast<Link05AnmHeapHD*>(m_anm_heap_upper)[2].mIdx;
        if (anm != link05::ANM_WALKBARREL && anm != link05::ANM_SALTATION /* !checkGrabAnime() */ && mStickDistance > 0.5f && direction != link05::DIR_BACKWARD &&
            (proc == link05::daPyProc_CUT_EX_MJ_e || !(mProcStartPos.y - posY < 300.0f)) && !(noResetFlg1() & daPyFlg1_UNK8000000) &&
            getDirectionFromCurrentAngle() == link05::DIR_FORWARD) {
            lk05VoiceStart(this, 7);
            if (direction == 0) {
                // HD: HIO folded
                speedF = 17.0f;
                lk05ProcFrontRoll_init(this, 6.0f);
            } else {
                procSideRoll_init();
            }
            return TRUE;
        }
    }
    if (!(dist < 1000.0f)) {
        if (!lk05CheckBossGomaStage(this) && mCurrAttributeCode != 5) {
            procLandDamage_init(0);
            return TRUE;
        }
        proc = mCurProc;
    }
    if (proc == link05::daPyProc_VOMIT_JUMP_e) {
        procVomitLand_init();
    } else if (bVar2) {
        lk05CheckNextMode(this, 0);
    } else {
        procLand_init(param_1, 0);
    }
    return TRUE;
}

/* 0241CB30 */
BOOL daPy_lk_c::checkSpecialRope() {
    /* strcmp(dComIfGp_getStartStageName(), "GanonK") (inlined) */
    u32 a = gabi::ea(dComIfGp_getStartStageName());
    u32 b = gabi::guest_addr("GanonK");
    u8 ca, cb;
    do {
        ca = lk05GuestByte(a++);
        cb = lk05GuestByte(b++);
    } while (ca == cb && ca != 0);
    if (ca == cb && mActorKeepEquip.mActor == nullptr) {
        return TRUE;
    }
    return FALSE;
}

/* 0241CBB0 */
void daPy_lk_c::setBlendRopeMoveAnime(int param_0) {
    f32 f30 = m35A4 * m35A8;
    f32 f2 = std::fabs(f30);
    Link05FloatHD blend;
    int anm;
    f32 dVar6;
    if (mModeFlg & link05::ModeFlg_00000400) {
        blend.mValue = 0.0f;
        anm = link05::ANM_ROPESWINGB; /* ANM_ROPESWINGB */
    } else {
        /* m_pbCalc[PART_UNDER_e]->getRatio(1) */
        Link05BlendCalcHD* pb = link05::animation(this)->m_pbCalc[0];
        lk05CopyFloatBits(&blend.mValue, pb->mAnmRatio[1].mpValues);
        s16 pv0 = mProcVar0;
        if (pv0 != 0) {
            anm = pv0 == -1 ? link05::ANM_ROPESWINGF : link05::ANM_ROPESWINGB;
            dVar6 = 1.0f;
            cLib_chaseF(&blend.mValue, 1.0f, 0.05f);
            pv0 = mProcVar0;
            if ((pv0 == -1 && 1.5707964f < f30) || (pv0 == 1 && -1.5707964f < f30)) {
                mProcVar0 = 0;
            }
        } else {
            if (param_0 == 0 && mStickDistance > 0.05f && !(mItemButton & 0x40)) {
                if (1.5707964f > f30 && !(0.31415927f > f30)) {
                    lk05SetMoveAnime(this, 0.0f, 1.0f, 1.0f, link05::ANM_ROPECATCH, 0x76, 7, 7.0f);
                    mProcVar0 = -1;
                    return;
                }
                if (-1.5707964f > f30 && !(-2.8274333f > f30)) {
                    lk05SetMoveAnime(this, 0.0f, 1.0f, 1.0f, link05::ANM_ROPECATCH, link05::ANM_ROPESWINGB, 7, 7.0f);
                    mProcVar0 = 1;
                    return;
                }
            }
            anm = f2 > 1.5707964f ? 0x76 : link05::ANM_ROPESWINGB;
            // HD: HIO folded
            dVar6 = (0.6f * (f32)(s16)mProcVar2) / 12000.0f;
            if (checkSpecialRope()) {
                dVar6 = dVar6 * 8.0f;
            }
            if (dVar6 > 0.6f) {
                dVar6 = 0.6f;
            }
        }
        Link05AnmDataHD* data = lk05GetAnmData(this, anm);
        if (data->mBckIdx.get() != reinterpret_cast<Link05AnmHeapHD*>(m_anm_heap_under)[1].mIdx /* m_anm_heap_under[UNDER_MOVE1_e].mIdx */) {
            lk05AddCalc(&blend.mValue, -dVar6, 0.5f, 0.05f, 0.005f);
            if (!(blend.mValue > 0.0f)) {
                blend.mValue = 0.0f;
            } else {
                anm = anm == link05::ANM_ROPESWINGB ? 0x76 : link05::ANM_ROPESWINGB;
            }
        } else if (mProcVar0 == 0) {
            lk05AddCalc(&blend.mValue, dVar6, 0.5f, 0.05f, 0.005f);
        }
    }
    // HD: HIO folded
    f32 f4 = param_0 != 0 ? 3.0f : -1.0f;
    lk05SetMoveAnime(this, (f32)blend.mValue, 1.0f, 1.0f, link05::ANM_ROPECATCH, anm, 7, f4);
}

// HD: the rope hanging offset is runtime data instead of the GameCube 95.0f constant.
static inline f64 lk05RopeLength(const cXyz* v) {
    return lk05Sqrtf(lk05VecSquareMag(v));
}

/* 0241D040 */
BOOL daPy_lk_c::procRopeSwing_init(fopAc_ac_c* rope_actor, s16 param_1) {
    fopAc_ac_c* i_rope_actor = rope_actor;
    lk05CommonProcInit(this, link05::daPyProc_ROPE_SWING_e);
    cXyz v10;
    cXyz v1c;
    cXyz v28;
    cXyz v34;
    if (rope_actor != nullptr) {
        lk05ActorKeepSetData(&mActorKeepRope, rope_actor);
        if (fpcM_GetName(rope_actor) == link05::fpcNm_HIMO3_e) {
            rope_actor->mParameters = 1; /* fopAcM_SetParam(rope_actor, 1) */
            lk05DeleteEquipItem(this, 1);
            mEquipItem = link05::dItemNo_GRAPPLING_HOOK_e;
            cXyz_mi(&rope_actor->current.pos, &v1c, &current.pos);
            v10.copy(v1c);
            f64 len = lk05RopeLength(&v10);
            f32 d = (f32)(len - (f64)(f32)link05::l_rope_hanging_offset);
            f32 maxLen = static_cast<Link05RopeHD*>(rope_actor)->mPlayerMoveLength.get(); /* rope->getPlayerMoveLength() */
            if (d > maxLen) {
                d = maxLen;
            }
            lk05VecNormalize(&v10, &v1c);
            cXyz_ml(&v10, &v34, d);
            cXyz_mi(&rope_actor->current.pos, &v1c, &v34);
            current.pos.copy(v1c);
        }
    } else {
        rope_actor = mActorKeepRope.mActor;
    }
    cXyz_mi(&rope_actor->current.pos, &v28, &current.pos);
    v10.copy(v28);
    f64 r = lk05RopeLength(&v10);
    m35A0 = (f32)r;
    f64 s = lk05Sqrtf((f32)(2.0 / r));
    m35A4 = (f32)s;
    f32 dVar11 = (f32)((f64)1.5707964f / s);
    f32 f31;
    if (i_rope_actor != nullptr) {
        // HD: HIO folded
        mProcVar2 = 0x2EE0;
        if (checkSpecialRope()) {
            mProcVar2 = (s16)gabi::ftoi((f32)(s16)mProcVar2 * 0.125f);
        }
        u16 sa = shape_angle.y;
        f32 z = v10.z;
        f32 sn = cM_ssin(sa);
        f32 cs = cM_scos(sa);
        f32 x = v10.x;
        f32 y = v10.y;
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
        f64 sq = lk05Sqrtf(gabi::fnmsubs(ratio, ratio, 1.0f));
        f64 at = lk05Atan2f_d(ratio, sq);
        m35A8 = (f32)(at / (f64)(f32)m35A4);
        f64 h = lk05Sqrtf(gabi::fmadds(y, y, lz * lz));
        s16 a3 = lk05Atan2s_d(-lx, h);
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
    mRopePos.copy(rope_actor->current.pos); /* mRopePos (daPy_py_c, GameCube 0x2EC) */
    dComIfGp_setPlayerStatus0(0, link05::daPyStts0_UNK800000_e);
    if (i_rope_actor == nullptr || mActorKeepEquip.mActor != nullptr) {
        mModeFlg = (mModeFlg & ~(u32)link05::ModeFlg_00000400) | link05::ModeFlg_00000100;
        lk05SetTextureAnime(this, 8, 0);
        if (i_rope_actor == nullptr) {
            lk05InitOldFrameMorf((u32)m_old_fdata, 15.0f, 0, 0x2A);
        }
    } else {
        setBlendRopeMoveAnime(1);
    }
    // HD: setStart also sets the frame
    mFrameCtrlUnder[0].mFrame = 0.0f;
    mFrameCtrlUnder[0].mStart = 0;
    // HD: HIO folded
    mFrameCtrlUnder[0].mEnd = 7;
    if (i_rope_actor != nullptr && mActorKeepEquip.mActor == nullptr) {
        /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform()->setFrame(frame) */
        lk05CopyFloatBits(&reinterpret_cast<Link05TransformHD*>(link05::animation(this)->mAnmRatioUnder[0].mpAnmTransform.get())->mFrame, &mFrameCtrlUnder[0].mFrame);
    }
    shape_angle.x = param_1;
    mProcVar0 = 0;
    // HD: reset the additional rope counter
    reinterpret_cast<Link05RopeCounterHD*>(this)->mRopeCounter = 0;
    return TRUE;
}

/* 0241D5F0 */
int daPy_lk_c::changeRopeSwingProc() {
    if (mCyl.ChkCoHit()) {
        fopAc_ac_c* hit = mCyl.GetCoHitAc();
        if (hit != nullptr && fpcM_GetName(hit) == link05::fpcNm_HIMO3_e) {
            cXyz d;
            cXyz_mi(&hit->current.pos, &d, &current.pos);
            f64 len = lk05RopeLength(&d);
            if (!((f32)(len - (f64)(f32)link05::l_rope_hanging_offset) < 100.0f) && hit->current.pos.y > current.pos.y) {
                return procRopeSwing_init(hit, 0x1800);
            }
        }
    }
    return FALSE;
}

/* dComIfGs_isEventBit: dSv_event_c at save + 0x644 */

/* 0241D6CC */
void daPy_lk_c::setFallVoice() {
    u32 flg = mNoResetFlg0;
    if (flg & 0x40000 /* daPyFlg0_NO_FALL_VOICE */) {
        return;
    }
    if (flg & 0x80 /* daPyFlg0_UNK80 */) {
        f32 gh = mAcch.GetGroundH();
        if (!(mWaterY < gh) && mGroundCode != 4 && gh != -1000000000.0f) {
            return;
        }
    }
    f32 y = current.pos.y;
    // HD: HIO folded
    if (mFallStartY - y > 500.0f && !(y - mAcch.GetGroundH() < 2000.0f) ) {
        lk05VoiceStart(this, 12);
        mNoResetFlg0 = mNoResetFlg0 | 0x40000;
        lk05SetTextureAnime(this, 0x56, 0);
    }
}

/* 0241D79C */
BOOL daPy_lk_c::procAutoJump() {
    /* true60 (pot route: in the auto jump the speed ramps 14.625 -> 17 by 0.4 a frame and B's position
     * fell behind while it ramped: 0.89 at N 2, 1.35 at N 4 = (1 - 1/N) / 2 x dv a frame): A changes mNormalSpeed
     * once at the frame's start and moves the whole frame at the new speed; per step each step moved at a part-ramped
     * speed. Form: the speed chase once a frame on the first step at dt 1 (A's value), held for the frame's later steps
     * (each moves its dt share at the frame's speed). A body first run on a later step: the per-step chase.
     * LK05_AJSPEED_NEGCTRL = the per-step chase. dt 1: the original. */
#if !defined(LK05_AJSPEED_NEGCTRL)
    bool ajChase = true;
    float ajDt0 = 1.0f;
    if (t60::dt() != 1.0f) {
        if (t60::first_step()) {
            s_lk05AjSpeedFirst = t60_step_counter();
            ajDt0 = t60_set_dt(1.0f);
        } else if (s_lk05AjSpeedFirst != 0xFFFFFFFFu && t60_step_counter() - s_lk05AjSpeedFirst < (uint32_t)t60::steps()) {
            ajChase = false;
        }
    }
    if (ajChase) {
#endif
    if (mProcVar0 == 0) {
        f32 stick = mStickDistance;
        if (stick > 0.05f) {
            if (getDirectionFromCurrentAngle() == link05::DIR_BACKWARD) {
                // HD: HIO folded
                cLib_chaseF(&mNormalSpeed, 0.0f, 0.8f * stick);
            } else {
                cLib_chaseF(&mNormalSpeed, 17.0f, 0.4f);
            }
        } else {
            cLib_chaseF(&mNormalSpeed, 0.0f, 0x1.47ae16p-4f); /* 0.1f * 0.8f (HD: HIO folded, rounded) */
        }
    } else {
        cLib_chaseF(&mNormalSpeed, 17.0f, 0.4f);
    }
#if !defined(LK05_AJSPEED_NEGCTRL)
    }
    if (t60::dt() == 1.0f && ajDt0 != 1.0f)
        t60_set_dt(ajDt0); /* the first step's whole-frame chase ends: its dt back */
#endif
    checkNextActionItemFly();
    if (lk05CheckJumpFlower(this) || checkJumpRideShip()) {
        return TRUE;
    }
    if (mAcch.ChkGroundHit()) {
        return changeLandProc(1.3f);
    }
    if (checkFanGlideProc(0)) {
        return TRUE;
    }
    u16 anm = reinterpret_cast<Link05AnmHeapHD*>(m_anm_heap_upper)[2].mIdx;
    if (anm != link05::ANM_WALKBARREL && anm != link05::ANM_SALTATION /* !checkGrabAnime() */ &&
        (lk05ChangeFrontWallTypeProc(this) || changeRopeSwingProc() || checkJumpCutFromButton() ||
         lk05CheckItemChangeFromButton(this))) {
        return TRUE;
    }
    mRootMoveMode = 1;
    if (t60::dt() != 1.0f) { /* true60 (M2g): the apex frame's later steps keep A's mode 2 */
        if (t60::first_step())
            s_lk05AjApexHold = false;
        else if (s_lk05AjApexHold && !lk05AjRootNegctrl())
            mRootMoveMode = 2;
        if (t60::tick30())
            s_lk05AjApexHold = false;
    }
    s16 pv0 = mProcVar0;
    if (lkT60_countBranch(mCurProc, 0, pv0)) {
        /* true60 (the 30 Hz timer rule; the first-step AI-timer rule for Link): the carried glide (Medli, 100 frames) counts 30 Hz
         * frames on the frame's FIRST step, the glide branch held over the frame after the count's 0 (the release
         * on the next frame's first step, as in the original's next update); its wall-contact budget counts on the
         * frame's tick30 step: a wall hit (per step) or a body push (an unconverted party's: read on the frame's
         * first step only) on any step of the frame counts that frame once */
        if (pv0 > 0 && lkT60_countStep()) {
            mProcVar0 = (s16)(pv0 - 1);
            lkT60_counted(mCurProc, 0, mProcVar0);
        }
        dComIfGp_setDoStatus(link05::dActStts_LET_GO_e);
        if (t60::dt() == 1.0f) {
            if ((mAcch.m_flags & 0x10) || mCyl.ChkCoHit()) {
                mProcVar1 = mProcVar1 - 1;
            }
        } else {
            if (t60::first_step()) {
                s_lk05AutoJumpTouch = false;
            }
            if ((mAcch.m_flags & 0x10) || mCyl.ChkCoHit()) {
                s_lk05AutoJumpTouch = true;
            }
            if (t60::tick30() && s_lk05AutoJumpTouch) {
                mProcVar1 = mProcVar1 - 1;
            }
        }
        if ((mItemTrigger & 1)) {
            fopAc_ac_c* grab = mActorKeepGrab.mActor;
            if (grab != 0) {
                lk05CopyFloatBits(&grab->speedF, &speedF);
            }
            lk05FreeGrabItem(this);
            mProcVar0 = 0;
            gravity = -2.5f;
        } else if (mProcVar1 == 0) {
            lk05FreeGrabItem(this);
            mProcVar0 = 0;
            gravity = -2.5f;
        } else if (mProcVar0 == 0) {
            // HD: HIO folded
            gravity = -2.5f;
        }
        f32 sy = speed.y;
        if (sy > 0.0f) {
            /* true60 (the per-step movement rule): the rise's deceleration per update: x dt */
            f32 n = sy - (t60::dt() == 1.0f ? 0.5f : 0.5f * (f32)t60::dt());
            if (n > 0.0f) {
                speed.y = n;
            } else {
                speed.y = 0.0f;
            }
        }
        setFallVoice();
        return TRUE;
    }
    if (speed.y < -gravity && mProcVar6 != 2 && lk05AjApexDecides() /* true60 */) {
        fopAc_ac_c* grab = mActorKeepGrab.mActor;
        if (grab != 0 && grab->mProcName.get() == link05::fpcNm_NPC_MD_e && dComIfGs_isEventBit(link05::UNK_1620)) {
            gravity = 0.0f;
            mProcVar0 = 0x64;
            speed.y = 5.0f;
        }
        lk05SetSingleMoveAnime(this, link05::ANM_JMPEDS, 0.0f, 0.0f, -1, 20.0f);
        mRootMoveMode = 2;
        s_lk05AjApexHold = t60::dt() != 1.0f && !t60::tick30(); /* true60 (M2g): held over the frame's later steps */
        mProcVar6 = 2;
        mModeFlg = mModeFlg & ~(u32)link05::ModeFlg_00000400;
        lk05SetTextureAnime(this, 0x37, 0);
        resetSeAnime();
    } else if (mProcStartPos.y > current.pos.y && reinterpret_cast<Link05AnmHeapHD*>(m_tex_anm_heap)->mIdx != 0x22B /* m_tex_anm_heap.mIdx */) {
        lk05SetTextureAnime(this, 0x37, 0);
    }
    setFallVoice();
    return TRUE;
}

/* 0241DB84 */
BOOL daPy_lk_c::procLand() {
    lk05ResetFootEffect(this);
    if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        if (mDemo.getDemoMode() == 0x40 /* daPy_demo_c::DEMO_SFALL_e */) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        } else {
            lk05CheckNextMode(this, 0);
        }
    // HD: HIO folded
    } else if (mFrameCtrlUnder[0].getFrame() > 7.0f ) {
        lk05CheckNextMode(this, 1);
    }
    return TRUE;
}

/* 0241DC30 */
BOOL daPy_lk_c::procLandDamage() {
    lk05ResetFootEffect(this);
    s32 pv6 = mProcVar6;
    f32 rate = mFrameCtrlUnder[0].getRate();
    if (pv6 != 0) {
        if (rate < 0.01f) {
            lk05CheckNextMode(this, 0);
        // HD: HIO folded
        } else if (mFrameCtrlUnder[0].getFrame() > 9.0f ) {
            lk05CheckNextMode(this, 1);
        }
    } else if (rate < 0.01f) {
        s16 pv0 = mProcVar0;
        if (lkT60_countBranch(mCurProc, 1, pv0)) {
            /* true60 (the 30 Hz timer rule; the first-step AI-timer rule for Link): the lie-down wait counts 30 Hz frames on the
             * frame's FIRST step, the wait branch held over the frame after the count's 0 */
            if (pv0 > 0 && lkT60_countStep()) {
                mProcVar0 = (s16)(pv0 - 1);
                lkT60_counted(mCurProc, 1, mProcVar0);
            }
        } else {
            f32 r = (mModeFlg & link05::ModeFlg_DAMAGE) ? 0.3f : 1.2f;
            lk05SetSingleMoveAnime(this, link05::ANM_LANDDAMAST, r, 1.0f, 9, 5.0f);
            mProcVar6 = 1;
        }
    }
    return TRUE;
}

/* 0241DD5C */
/* true60 (the first-step AI-timer rule for Link: first-step decision): procFall's "falling below the start"
 * test (mProcStartPos.y > pos.y: the fall face, mProcVar6) is a body decision on the position. A's body reads the
 * previous frame's end position; at N steps a frame whose body first runs on the tick30 step (the procedure entered in
 * the previous frame's execute after its body) read the first step's half fall and set it a frame early (stZ1-route
 * stG1, ledge landing -> FALL 3633: B's tick30 at 636.67 < 637.60, A from 3634). The test runs on the frame's first step
 * (the previous frame-end position, as A); a later step keeps the first step's result. NC WWHD_T60_FALLFACE_NEGCTRL=1;
 * dt 1: every call (the original). */

BOOL daPy_lk_c::procFall() {
    if (mProcVar6 == 0 && lkT60_fallFaceStep() && mProcStartPos.y > current.pos.y && reinterpret_cast<Link05AnmHeapHD*>(m_tex_anm_heap)->mIdx != 0x22B /* m_tex_anm_heap.mIdx */) {
        lk05SetTextureAnime(this, 0x37, 0);
        mProcVar6 = 1;
    }
    if (mProcVar3 != 0) {
        LKT60_CHASE_S(9, &mNormalSpeed, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, 0.2f, 1.0f, 0.1f)); /* true60: frame-exact */
    }
    checkNextActionItemFly();
    if (lk05CheckJumpFlower(this) || (mProcVar7 != 0 && checkJumpRideShip())) {
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
            /* true60 (the 30 Hz timer rule; the first-step AI-timer rule for Link): the wall-grab delay counts 30 Hz frames: on the
             * frame's FIRST step (count before the grab test: no hold) */
            if (pv1 > 0 && lkT60_countStep()) {
                pv1 = (s16)(pv1 - 1);
                mProcVar1 = pv1;
            }
            if (mStickDistance > 0.05f && getDirectionFromShapeAngle() == 0 && pv1 == 0 &&
                lk05ChangeFrontWallTypeProc(this)) {
                return TRUE;
            }
        } else if (pv0 == 2) {
            if (lk05ChangeFrontWallTypeProc(this) || changeRopeSwingProc()) {
                return TRUE;
            }
        }
        if (mProcVar2 != 0 && checkJumpCutFromButton()) {
            return TRUE;
        }
    }
    lk05CheckItemChangeFromButton(this);
    return TRUE;
}

/* 0241DF20 */
BOOL daPy_lk_c::procSlowFall() {
    if (lk05SlowFallLow(current.pos.y - mAcch.GetGroundH() < 200.0f) /* true60 */) {
        cLib_chaseF(&maxFallSpeed, -5.0f, 1.0f);
    }
    if (mAcch.ChkGroundHit()) {
        // HD: HIO folded
        procLand_init(0.6f, 1);
    }
    return TRUE;
}

/* 0241DFA0 */
BOOL daPy_lk_c::procSmallJump() {
    if (mAcch.ChkGroundHit()) {
        changeLandProc(1.3f);
    } else if (checkFanGlideProc(0)) {
        return TRUE;
    } else if (speed.y < -gravity) {
        // HD: HIO folded
        lk05ProcFall_init(this, (s32)mProcVar6, 7.0f);
        lk05SetTextureAnime(this, 0x37, 0);
    }
    return TRUE;
}

/* 0241E034 */
BOOL daPy_lk_c::procVerticalJump() {
    cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 2, 0x1000, 0x400);
    current.angle.y = shape_angle.y;
    if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        s16 a = mProcVar2;
        current.angle.y = a;
        shape_angle.y = a;
        BOOL r;
        if (mProcVar6 != 0) {
            r = lk05ProcHangStart_init(this);
        } else {
            r = lk05ProcHangWallCatch_init(this);
        }
        if (r == 0) {
            lk05ProcWait_init(this);
        }
    }
    return TRUE;
}

/* 0241E0D4 */
BOOL daPy_lk_c::procGuardCrash() {
    LKT60_CHASE_S(10, &mNormalSpeed, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, 0.5f, 1.25f, 0.25f)); /* true60: frame-exact */
    s16 pv0 = mProcVar0;
    if (lkT60_countBranch(mCurProc, 2, pv0)) {
        /* true60 (the 30 Hz timer rule; the first-step AI-timer rule for Link): the guard-break stun counts 30 Hz frames on the
         * frame's FIRST step, the stun branch held over the frame after the count's 0 */
        if (pv0 > 0 && lkT60_countStep()) {
            mProcVar0 = (s16)(pv0 - 1);
            lkT60_counted(mCurProc, 2, mProcVar0);
        }
    } else {
        lk05CheckNextMode(this, 0);
    }
    return TRUE;
}

/* 0241E150 */
BOOL daPy_lk_c::procDamage() {
    // HD: HIO folded
    f32 dVar6 = mFrameCtrlUnder[0].getFrame() - 1.5f;
    cLib_addCalcAngleS(&mNeckAngle.y, 0, 4, 0x800, 0x100);
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
    mNeckAngle.x = ax;
    mNeckAngle.y = 0;
    mBodyAngle.x = bx;
    mNeckAngle.z = az;
    lkT60_neckDmgMark(); /* true60: setNeckAngle eases these at the full step */
    mBodyAngle.z = bz;
    mBodyAngle.y = 0;
    // HD: HIO folded
    f64 r = LKT60_CHASE(11, &mNormalSpeed, 0.0f, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, 0.5f, 1.2f, 0.25f)); /* true60: frame-exact */
    if (!(r > 0.001f) && lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        lk05CheckNextMode(this, 0);
    } else if (mNormalSpeed < 12.0f) {
        lk05ResetFootEffect(this);
    }
    return TRUE;
}

/* 0241E464 */
BOOL daPy_lk_c::procLargeDamageWall_init(int param_1, int param_2, s16 param_3, s16 param_4) {
    if (mCurProc == link05::daPyProc_LARGE_DAMAGE_WALL_e) {
        return FALSE;
    }
    cM3dGPla* pla = lk05GetTriPla(dComIfG_Bgsp(), (u32)reinterpret_cast<dBgS_LinChk*>(mLinkLinChk)->mBgIndex,
                              (u32)reinterpret_cast<dBgS_LinChk*>(mLinkLinChk)->mPolyIndex);
    // HD: reject a missing wall plane.
    if (pla == 0) {
        return FALSE;
    }
    s16 sVar6 = cM_atan2s(pla->mNormal.x.get(), pla->mNormal.z.get());
    if (param_1 < 0) {
        return FALSE;
    }
    f32 ny = pla->mNormal.y.get();
    if (!(ny < 0.5f) || ny < -0.8f) { /* !cBgW_CheckBWall(ny) */
        return FALSE;
    }
    if (cLib_distanceAngleS(sVar6, (s16)(current.angle.y - 0x8000)) > 0x1555) {
        return FALSE;
    }
    lk05CommonProcInit(this, link05::daPyProc_LARGE_DAMAGE_WALL_e);
    cXyz xz;
    lk05CopyFloatBits(&xz.z, &pla->mNormal.z);
    lk05CopyFloatBits(&xz.x, &pla->mNormal.x);
    xz.y = 0.0f;
    f64 len = lk05Sqrtf(lk05VecSquareMag(&xz));
    s16 sVar7 = lk05Atan2s_d(pla->mNormal.y.get(), len);
    lk05CopyFloatBits(&current.pos.x, &reinterpret_cast<dBgS_LinChk*>(mLinkLinChk)->mLin.mEnd.x); /* mLinkLinChk.GetCrossP() */
    lk05CopyFloatBits(&current.pos.z, &reinterpret_cast<dBgS_LinChk*>(mLinkLinChk)->mLin.mEnd.z);
    current.angle.y = sVar6;
    lk05SetOldRootQuaternion(this, (s32)param_3, 0, (s32)param_4);
    // HD: HIO folded (the same values for param_2 on and off)
    if (param_1 == 0x5C) {
        mProcVar6 = link05::ANM_DAMFBUP; /* ANM_DAMFBUP */
        m34F2 = (s16)(sVar7 - 0x4000);
        mProcVar0 = 0;
        lk05SetSingleMoveAnime(this, link05::ANM_DAMFBUP, 1.2f, 0.0f, 2, 1.0f);
        mNormalSpeed = 0.0f;
        speed.y = 0.0f;
        gravity = 0.0f;
    } else if (param_1 == 0x59) {
        mProcVar6 = link05::ANM_DAMFLUP; /* ANM_DAMFLUP */
        m34F4 = (s16)(sVar7 - 0x4000);
        mProcVar0 = 1;
        lk05SetSingleMoveAnime(this, link05::ANM_DAMFLUP, 1.3f, 0.0f, 2, 1.0f);
        mNormalSpeed = 0.0f;
        speed.y = 0.0f;
        gravity = 0.0f;
    } else if (param_1 == 0x5A) {
        mProcVar6 = link05::ANM_DAMFRUP; /* ANM_DAMFRUP */
        s32 anm = mProcVar6;
        m34F4 = (s16)(0x4000 - sVar7);
        mProcVar0 = 1;
        lk05SetSingleMoveAnime(this, anm, 1.3f, 0.0f, 2, 1.0f);
        mNormalSpeed = 0.0f;
        gravity = 0.0f;
        speed.y = 0.0f;
    } else {
        mProcVar6 = link05::ANM_DAMFFUP; /* ANM_DAMFFUP */
        mProcVar0 = 0;
        m34F2 = (s16)(0x4000 - sVar7);
        lk05SetSingleMoveAnime(this, link05::ANM_DAMFFUP, 1.5f, 18.0f, 0x13, 1.0f);
        mNormalSpeed = 0.0f;
        speed.y = 0.0f;
        gravity = 0.0f;
    }
    {
        cXyz v;
        v.x = 0.0f;
        v.y = 1.0f;
        v.z = 0.0f;
        dComIfGp_getVibration_StartShock(6, -0x31, &v);
    }
    mProcVar0 = (s16)param_2;
    return TRUE;
}

/* 0241E7D4 */
BOOL daPy_lk_c::procLargeDamage() {
    if (lkT60Ld_skip(this)) /* true60 (d_a_player_crawl_t60.h): in the air, the body once a frame on tick30 */
        return TRUE;
    lkT60Ld_open();
    s16 pv0 = mProcVar0;
    if (pv0 > 0) {
        /* true60 (the 30 Hz timer rule; the first-step AI-timer rule for Link): the hit stop before the launch counts 30 Hz frames:
         * on the frame's FIRST step (the launch on the step that ends it, as in the frame that ends it; the launched
         * motion then runs on every step of that frame) */
        if (!lkT60_countStep()) {
            return lkT60Crawl_close(TRUE);
        }
        pv0 = (s16)(pv0 - 1);
        mProcVar0 = pv0;
        if (pv0 != 0) {
            return lkT60Crawl_close(TRUE);
        }
        // HD: HIO folded
        mNormalSpeed = 25.0f;
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
        return lkT60Crawl_close(TRUE);
    }
    u32 acchFlags = mAcch.m_flags;
    if (acchFlags & 0x20 /* ChkGroundHit() */) {
        if (lk05ChangeSlideProc(this)) {
            mNormalSpeed = 0.0f;
            return lkT60Crawl_close(TRUE);
        }
        pv3 = mProcVar3;
        if (pv3 & 2) {
            lk05VoiceStart(this, 2);
            lk05SeStartSystem(link05::JA_SE_MAJUTOU_JAIL_DOOR);
            mAcch.m_flags = mAcch.m_flags & ~0x4004u; /* ClrWallNone(), OffLineCheckNone() */
            pv3 = mProcVar3;
        }
        s16 a2 = m34F2;
        s32 anm = mProcVar6;
        s16 a4 = m34F4;
        lk05ProcLargeDamageUp_init(this, anm, (s32)((pv3 >> 3) & 1), (s32)a2, (s32)a4);
        return lkT60Crawl_close(TRUE);
    }
    if ((acchFlags & 0x10 /* ChkWallHit() */) && mNormalSpeed > m35A0) {
        for (int i = 0; i < 3; i++) {
            dBgS_AcchCir* cir = &mAcchCir[i];
            if (!(cir->m_flags & dBgS_AcchCir::WALL_HIT /* ChkWallHit() */)) {
                continue;
            }
            cXyz start;
            cXyz end;
            f32 wallH = cir->GetWallH();
            f32 py = current.pos.y;
            f32 pz = current.pos.z;
            f32 y = py + wallH;
            u16 ang = current.angle.y;
            f32 px = current.pos.x;
            start.y = y;
            lk05CopyFloatBits(&start.x, &current.pos.x);
            lk05CopyFloatBits(&start.z, &current.pos.z);
            f32 r = cir->GetWallR() + 25.0f;
            end.y = y;
            end.x = gabi::fmadds(cM_ssin(ang), r, px);
            end.z = gabi::fmadds(cM_scos(ang), r, pz);
            lk05LineSet(reinterpret_cast<dBgS_LinChk*>(mLinkLinChk), &start, &end, this);
            if (lk05LineCross(dComIfG_Bgsp(), reinterpret_cast<dBgS_LinChk*>(mLinkLinChk))) {
                s16 p3 = mProcVar3;
                s16 a2 = m34F2;
                s32 anm = mProcVar6;
                s16 a4 = m34F4;
                return lkT60Crawl_close(procLargeDamageWall_init(anm, (p3 >> 3) & 1, a2, a4));
            }
        }
    }
    return lkT60Crawl_close(TRUE);
}

/* 0241EA50 */
BOOL daPy_lk_c::procLargeDamageUp() {
    m35E4 = (m35A0 - mFrameCtrlUnder[0].getFrame()) * m35A4;
    lk05ResetFootEffect(this);
    if (mProcVar0 > 0) {
        s16 pv0;
        if (!lk05EventStartCheckOld(dComIfGp_getPEvtManager(), "ICE_FAILED" /* "ICE_FAILED" */)) {
            /* true60 (the 30 Hz timer rule; the first-step AI-timer rule for Link): the lie-down wait counts 30 Hz frames: on the
             * frame's FIRST step (count before the get-up test: no hold) */
            pv0 = mProcVar0;
            if (lkT60_countStep()) {
                pv0 = (s16)(pv0 - 1);
                mProcVar0 = pv0;
            }
        } else {
            dComIfGp_evmng_cutEnd(mStaffIdx);
            pv0 = mProcVar0;
        }
        if (pv0 == 0 || (mProcVar6 == -4 && lk05PadStickValue(0) > 0.05f)) {
            mFrameCtrlUnder[0].setRate(0.5f);
        }
    } else if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        if (dComIfGp_event_runCheck()) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        } else {
            lk05CheckNextMode(this, 0);
        }
    } else if (lkT60_endFrame(&mFrameCtrlUnder[0]) > m35A0 /* true60: frame end */) {
        lk05CheckNextMode(this, 1);
    }
    return TRUE;
}

/* 0241EBB4 */
BOOL daPy_lk_c::procLargeDamageWall() {
    if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
        lk05ProcLargeDamage_init(this, (s32)mProcVar6, (s32)mProcVar0, (s32)m34F2, (s32)m34F4);
    }
    return TRUE;
}

/* 0241EBFC */
BOOL daPy_lk_c::procLavaDamage() {
    if (mAcch.ChkGroundHit()) {
        int direction = getDirectionFromAngle((s16)(current.angle.y - shape_angle.y));
        int anm;
        if (direction == link05::DIR_FORWARD) {
            anm = 0x5C; /* ANM_DAMFB */
        } else if (direction == link05::DIR_BACKWARD) {
            anm = 0x5B; /* ANM_DAMFF */
        } else if (direction == link05::DIR_LEFT) {
            anm = 0x5A; /* ANM_DAMFR */
        } else {
            anm = 0x59; /* ANM_DAMFL */
        }
        lk05ProcLargeDamageUp_init(this, anm, 1, 0, 0);
    }
    return TRUE;
}

/* 0241ECB0 */
BOOL daPy_lk_c::procElecDamage() {
    lk05SetShipRidePosUseItem(this);
    s16 pv0 = mProcVar0;
    if (lkT60_countBranch(mCurProc, 3, pv0)) {
        /* true60 (the 30 Hz timer rule; the first-step AI-timer rule for Link): the shock's length counts 30 Hz frames on the
         * frame's FIRST step, the shock branch held over the frame after the count's 0 */
        if (pv0 > 0 && lkT60_countStep()) {
            mProcVar0 = (s16)(pv0 - 1);
            lkT60_counted(mCurProc, 3, mProcVar0);
        }
    } else {
        dComIfGp_event_reset();
        lk05EndDemoMode(this);
        if (mProcVar3 == 0 && mAcch.ChkGroundHit()) {
            lk05CheckNextMode(this, 0);
        } else {
            if (mProcVar3 != 0) {
                current.angle.y = cM_atan2s(m370C.x, m370C.z);
            } else {
                current.angle.y = (s16)(shape_angle.y - 0x8000);
            }
            lk05ProcLargeDamage_init(this, -10, 1, 0, 0);
        }
    }
    // HD: mark electric-damage recovery in the additional reset flag.
    setResetFlg0(resetFlg0() | 0x80000000);
    return TRUE;
}

/* 0241EDB8 */
BOOL daPy_lk_c::procGuardSlip() {
    s16 pv0 = mProcVar0;
    /* true60 (the 30 Hz timer rule; the first-step AI-timer rule for Link): the slide's minimum length counts 30 Hz frames: on the
     * frame's FIRST step (count before the end test, as in the original: no hold) */
    if (pv0 > 0 && lkT60_countStep()) {
        mProcVar0 = (s16)(pv0 - 1);
    }
    lk05SetShapeAngleToAtnActor(this);
    if (mEquipItem == link05::dItemNo_SKULL_HAMMER_e) {
        lk05CopyFloatBits(&m35EC, &mFrameCtrlUnder[0].mFrame);
    } else {
        lk05CopyFloatBits(&m35E8, &mFrameCtrlUnder[0].mFrame);
    }
    f64 r = LKT60_CHASE(4, &mNormalSpeed, 0.0f, &mCurProc, lk05AddCalc(&mNormalSpeed, 0.0f, m35A0, m35A4, m35A8)); /* true60: frame-exact */
    if (!(r > 0.001f)) {
        if (mProcVar6 != 0) {
            return procNockBackEnd_init();
        }
        if (mProcVar0 == 0) {
            // HD: m_pbCalc[PART_UPPER_e]: every blend ratio of the upper body set to 1.0
            Link05BlendCalcHD* pb = link05::animation(this)->m_pbCalc[1];
            u32 res = lk05GetJointTree(gabi::ea(pb->mpModel->mModelData.get()));
            s32 n = lk05GetBlendCount(res, 0);
            for (s32 i = 0; i < n; i++) {
                pb->mAnmRatio[2].mpValues[i] = 1.0f;
            }
            lk05CheckNextMode(this, 0);
        }
    } else if (mNormalSpeed < m35AC) {
        lk05ResetFootEffect(this);
    }
    return TRUE;
}

/* 0241EF2C */
BOOL daPy_lk_c::procIceSlipFallUp_init(int param_1, s16 param_2, s16 param_3) {
    lk05CommonProcInit(this, link05::daPyProc_ICE_SLIP_FALL_UP_e);
    mFootEffectPosType = 6;
    {
        cXyz v;
        v.x = 0.0f;
        v.z = 0.0f;
        v.y = 1.0f;
        dComIfGp_getVibration_StartShock(3, -0x3F, &v);
    }
    seStartMapInfo(link05::JA_SE_LK_FALL_DOWN);
    // HD: HIO folded
    int anm;
    f32 fVar1, fVar2, fVar3, len;
    if (param_1 == link05::ANM_DAMFB) {
        anm = link05::ANM_DAMFBUP; fVar1 = 0.8f; fVar2 = 3.0f; fVar3 = 2.0f; len = 33.0f;
    } else if (param_1 == link05::ANM_DAMFL) {
        anm = link05::ANM_DAMFLUP; fVar1 = 0.7f; fVar2 = 7.0f; fVar3 = 0.0f; len = 30.0f;
    } else if (param_1 == link05::ANM_DAMFR) {
        anm = link05::ANM_DAMFRUP; fVar1 = 0.7f; fVar2 = 7.0f; fVar3 = 0.0f; len = 30.0f;
    } else {
        anm = link05::ANM_DAMFFUP; fVar1 = 0.7f; fVar2 = 3.0f; fVar3 = 3.0f; len = 30.0f;
    }
    m35E4 = 1.0f;
    m35A0 = len;
    m35A4 = 2.0f / (len - fVar2);
    lk05SetSingleMoveAnime(this, anm, fVar1, fVar2, 0x24, fVar3);
    lk05SetOldRootQuaternion(this, (s32)param_2, 0, (s32)param_3);
    mNormalSpeed = 0.0f;
    current.angle.y = shape_angle.y;
    return TRUE;
}

/* 0241F1AC */
BOOL daPy_lk_c::procIceSlipFall() {
    s16 pv2 = mProcVar2;
    if (mProcVar3 == 1) {
        // HD: HIO folded
        cLib_chaseAngleS(&m34F2, pv2, 0x1F40);
    } else {
        cLib_chaseAngleS(&m34F4, pv2, 0x1F40);
    }
    if (mAcch.ChkGroundHit() && !lk05ChangeSlideProc(this)) {
        s16 a4 = m34F4;
        s32 anm = mProcVar6;
        procIceSlipFallUp_init(anm, m34F2, a4);
    }
    return TRUE;
}





inline void lk05DemoTalismanPosition(daPy_lk_c *self) {
  PSMTXCopy(lk05DemoAnmMatrix(self->mpCLModel, 12), mDoMtx_stack_c::get());
  u16 angle = (u16)(link05::g_Counter.mTimer * 0x5E8);
  if (t60::dt() != 1.0f) { /* true60 (the 60 Hz counter rule): the charm's bob at the step's phase (see the spin) */
    angle = (u16)((s32)(link05::g_Counter.mTimer * 0x5E8) - (s32)(lk05CounterLag() * (f32)0x5E8));
  }
  mDoMtx_stack_c::transM(7.0f, 0.0f, gabi::fmadds(3.0f, cM_ssin(angle), 15.0f));
  lk05CopyFloatBits(&lk05DemoFollow(self)->mPos.x,
                    &link05::l_matrixStack.m[0][3]);
  lk05CopyFloatBits(&lk05DemoFollow(self)->mPos.y,
                    &link05::l_matrixStack.m[1][3]);
  lk05CopyFloatBits(&lk05DemoFollow(self)->mPos.z,
                    &link05::l_matrixStack.m[2][3]);
}

/* 0241F240 */
BOOL daPy_lk_c::procIceSlipFallUp() {
  m35E4 = (m35A0 - mFrameCtrlUnder[0].getFrame()) * m35A4;
  lk05ResetFootEffect(this);
  if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    lk05CheckNextMode(this, 0);
  } else if (lkT60_endFrame(&mFrameCtrlUnder[0]) > m35A0 /* true60: frame end */) {
    lk05CheckNextMode(this, 1);
  }
  return TRUE;
}

/* 0241F2D4 */
BOOL daPy_lk_c::procIceSlipAlmostFall() {
  if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    current.angle.y = shape_angle.y;
    lk05CheckNextMode(this, 0);
  } else if (mFrameCtrlUnder[0].getFrame() > 6.0f /* HD: HIO folded */) {
    s16 a = shape_angle.y;
    s16 old = current.angle.y;
    current.angle.y = a;
    if (!lk05CheckNextMode(this, 1)) {
      current.angle.y = old;
    }
  }
  return TRUE;
}

/* 0241F374 */
BOOL daPy_lk_c::procGrabHeavyWait_init() {
  lk05CommonProcInit(this, link05demo::daPyProc_GRAB_HEAVY_WAIT_e);
  lk05SetSingleMoveAnime(this, link05demo::ANM_GRABWAITB, 1.2f, 0.0f, -1,
                         3.0f); /* HD: HIO folded */
  lk05DemoSetActAnimeUpper(this, link05demo::dRes_INDEX_LKANM_BCK_GRABWAITB_e,
                           2 /* UPPER_MOVE2_e */, 0.0f, 0.0f, -1, -1.0f);
  /* HD: m_pbCalc[PART_UPPER_e]: every blend ratio of the upper body set to 0.0
   */
  lk05DemoUpperRatioAll(m_pbCalc[1], 0.0f);
  current.angle.y = shape_angle.y;
  mNormalSpeed = 0.0f;
  m35A0 = 0.0f;
  return TRUE;
}

/* 0241F470 */
void daPy_lk_c::initGrabNextMode() {
  fopAc_ac_c *grab = mActorKeepGrab.mActor;
  if (grab->actor_status & link05demo::fopAcStts_UNK10000_e) {
    if (!lk05DemoCheckGrabHeavy(this)) {
      procGrabHeavyWait_init();
      return;
    }
    u16 anm = lk05DemoUpperHeap(this, 2)->mIdx;
    if (anm != 0x95 && anm != 0x96) {
      lk05DemoSetActAnimeUpper(this, 0x96 /* GRABWAITB */, 2, 0.0f, 0.0f, -1,
                               -1.0f);
    }
  } else {
    u16 anm = lk05DemoUpperHeap(this, 2)->mIdx;
    if (anm != 0x95 && anm != 0x96) {
      lk05DemoSetActAnimeUpper(this, 0x95 /* GRABWAIT */, 2, 0.0f, 0.0f, -1,
                               -1.0f);
    }
  }
  lk05DemoUpperRatioAll(m_pbCalc[1], 1.0f);
  lk05CheckNextMode(this, 0);
}

/* 0241F618 */
BOOL daPy_lk_c::procBootsEquip() {
  J3DFrameCtrl &frameCtrl = mFrameCtrlUnder[0];
  /* HD: the pass frame is a global (0x1047C04C) + 14 */
  if (lkT60_pass(&frameCtrl, link05::l_bootsEquipFrame + 14.0f)) {
    u32 flg = mNoResetFlg0;
    if (flg & 0x2000000 /* daPyFlg0_EQUIP_HEAVY_BOOTS */) {
      mNoResetFlg0 = flg & ~0x2000000u;
    } else {
      mNoResetFlg0 = flg | 0x2000000;
    }
  }
  if (lkT60_endRate(&frameCtrl) < 0.01f) {
    u16 anm = lk05DemoUpperHeap(this, 2)->mIdx;
    if (anm == 0x95 || anm == 0x96) {
      initGrabNextMode();
    } else {
      lk05CheckNextMode(this, 0);
    }
  } else if (frameCtrl.getFrame() > 19.0f) {
    u16 anm = lk05DemoUpperHeap(this, 2)->mIdx;
    if (anm == 0x95 || anm == 0x96) {
      if (mStickDistance > 0.05f) {
        initGrabNextMode();
      }
    } else {
      lk05CheckNextMode(this, 1);
    }
  } else if (lkT60_pass(&frameCtrl, 15.0f) && (mNoResetFlg0 & 0x2000000)) {
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 1.0f;
    v->z = 0.0f;
    dComIfGp_getVibration_StartShock(5, -0x31, v);
  }
  return TRUE;
}

/* 0241F79C */
BOOL daPy_lk_c::procNotUse() {
  if (mProcVar2 == 0) {
    s32 item = mProcVar6;
    if (!checkBottleItem((u16)item)) {
      u32 id =
          lk05DemoCreateItem(&current.pos, item, 5, -1, (s32)(s8)current.roomNo,
                             &shape_angle, &scale);
      dComIfGp_event_setItemPartnerId(
          id); /* dComIfGp_event_setItemPartnerId() */
      mProcVar2 = 1;
    }
  }
  if (!(lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f)) {
    return TRUE;
  }
  fopAc_ac_c *item = lk05DemoGetItemPartner(this);
  if (item != 0 && (fpcM_GetName(item) == link05demo::fpcNm_ITEM_e ||
                    fpcM_GetName(item) == link05demo::fpcNm_Demo_Item_e)) {
    lk05DemoShowItem(item);
  }
  if (mProcVar2 == 0) {
    u16 bottle = lk05DemoProcVar6Low(this); /* (u16)mProcVar6 */
    if (checkBottleItem(bottle)) {
      lk05DemoSetBottleModel(this, (u32)bottle);
      mProcVar2 = 1;
    }
  }
  s32 gameOverId = mGameOverId;
  Link05DemoMessageManager *mgr = link05::l_demoMessageManager.get();
  if (gameOverId == -1) {
    mGameOverId = lk05DemoMessageSet(mgr, (u32)m3624, 0);
    return TRUE;
  }
  if (lk05DemoMessageStatus(mgr) == link05demo::fopMsgStts_MSG_DISPLAYED_e) {
    if (m3624 == 0xF0C && mgr->mSelectNum == 1 /* mSelectNum */) {
      lk05DemoSetMessageStatus(mgr, link05demo::fopMsgStts_MSG_CONTINUES_e);
      m3624 = 0xF10;
      lk05DemoMessageSet(mgr, 0xF10, 0);
      lk05DemoEmptyReserve(lk05DemoReserveItems(), (u32)mReadyItemBtn);
    } else {
      lk05DemoSetMessageStatus(mgr, link05demo::fopMsgStts_MSG_ENDS_e);
    }
    return TRUE;
  }
  if (lk05DemoMessageStatus(mgr) == link05demo::fopMsgStts_BOX_CLOSED_e) {
    lk05DemoSetMessageStatus(mgr, link05demo::fopMsgStts_MSG_DESTROYED_e);
    if (item != 0 &&
        (fpcM_GetName(item) == 0xFF || fpcM_GetName(item) == 0x101)) {
      lk05DemoDeleteItem(item);
    }
    dComIfGp_event_onEventFlag(8);
    lk05DemoEndEventCamera(lk05GetCameraBody(), (u32)mBsPcId);
    lk05EndDemoMode(this);
  }
  return TRUE;
}

/* 0241F9DC */
void daPy_lk_c::setTalismanModel() {
  J3DAnmTransform *bck = lk05GetItemAnimeResource(
      this, link05demo::dRes_INDEX_LKANM_BCK_TETOLACH_e);
  JKRHeap *oldHeap = lk05SetItemHeap(this);
  J3DModelData *data = lk05DemoInitModel(
      this, &reinterpret_cast<Link05EquipmentHD *>(this)->mpEquipItemModel,
      link05demo::dRes_INDEX_LINK_BDL_TETOLACH_e, 0x13000022);
  if (!lk05DemoInitBck(lk05DemoSword(this), data, bck, 1, 2, 0.2f, 0, -1, 0)) {
    JUT_ASSERT_fail("d_a_player_dproc.inc", 0x52, "0");
  }
  lk05SetCurrentHeap(oldHeap);
}

/* 0241FA98 */
void daPy_lk_c::setLetterModel() {
  if (mEquipItem == link05demo::daPyItem_UNK104_e) {
    return;
  }
  J3DAnmTransform *bck = lk05GetItemAnimeResource(
      this, link05demo::dRes_INDEX_LKANM_BCK_GETLETTERA_e);
  JKRHeap *oldHeap = lk05SetItemHeap(this);
  J3DModelData *data = lk05DemoInitModel(
      this, &reinterpret_cast<Link05EquipmentHD *>(this)->mpEquipItemModel,
      link05demo::dRes_INDEX_LINK_BDL_LETTER_e, 0x13000022);
  if (!lk05DemoInitBck(lk05DemoSword(this), data, bck, 0, 2, 1.0f, 0, -1, 0)) {
    JUT_ASSERT_fail("d_a_player_dproc.inc", 0x78, "0");
  }
  lk05SetCurrentHeap(oldHeap);
  mEquipItem = 0x104;
  m35EC = 0.0f;
}

/* 0241FB78 */
void daPy_lk_c::setShapeAngleToTalkActor() {
  fopAc_ac_c *partner = lk05DemoTalkPartner(this);
  if (partner != nullptr) {
    /* HD: no special case for the AGB partner */
    s16 target = fopAcM_searchActorAngleY(this, partner);
    cLib_addCalcAngleS(&shape_angle.y, target, 4, 0x1000,
                       0x200); /* HD: HIO folded */
  }
}

/* 0241FBCC */
BOOL daPy_lk_c::checkEndMessage(u32 i_msgNo) {
  s32 gameOverId = mGameOverId;
  Link05DemoMessageManager *mgr = link05::l_demoMessageManager.get();
  if (gameOverId == -1) {
    if (lk05DemoMessageSet(mgr, i_msgNo, 0) != -1) {
      mGameOverId = 0; /* HD: a flag, not the message id */
    }
    return FALSE;
  }
  if (lk05DemoMessageStatus(mgr) == link05demo::fopMsgStts_MSG_DISPLAYED_e) {
    lk05DemoSetMessageStatus(mgr, link05demo::fopMsgStts_MSG_ENDS_e);
    return FALSE;
  }
  if (lk05DemoMessageStatus(mgr) == link05demo::fopMsgStts_BOX_CLOSED_e) {
    lk05DemoSetMessageStatus(mgr, link05demo::fopMsgStts_MSG_DESTROYED_e);
    return TRUE;
  }
  s32 st = lk05DemoMessageStatus(mgr);
  if (st == link05demo::fopMsgStts_MSG_PREPARING_e && i_msgNo == 0x14A2) {
    lk05DemoMessageFlagOn(st);
  }
  return FALSE;
}

/* 0241FCB4 */
void daPy_lk_c::setDemoTextureAnime(u16 btpIdx, u16 btkIdx, int r30, u16 r31) {
  if (lk05DemoTextureHeap(this)->mDemoIdx != btpIdx ||
      lk05DemoTextureHeap(this)->mDemoAnmNo != r31) {
    lk05DemoTextureHeap(this)->mDemoIdx = btpIdx; /* m_tex_anm_heap */
    lk05DemoTextureHeap(this)->mDemoAnmNo = r31;
    u32 res = lk05DemoLoadTextureAnime(this, (u32)btpIdx, 1);
    lk05DemoSetTextureAnimeResource(this, res, r30);
  }
  if (lk05DemoScrollHeap(this)->mDemoIdx != btkIdx ||
      lk05DemoScrollHeap(this)->mDemoAnmNo != r31) {
    lk05DemoScrollHeap(this)->mDemoIdx = btkIdx; /* m_tex_scroll_heap */
    lk05DemoScrollHeap(this)->mDemoAnmNo = r31;
    u32 res = lk05DemoLoadTextureScroll(this, (u32)btkIdx, 1);
    lk05DemoSetTextureScroll(this, res, r30);
  }
}

/* 02421148 */
BOOL daPy_lk_c::dProcDamage_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_DAMAGE_e) {
    return FALSE;
  }
  lk05CommonProcInit(this, 0xAB);
  lk05SetSingleMoveAnime(this, link05demo::ANM_DAMF, 0.6f, 0.0f, 9,
                         0.0f); /* HD: HIO folded */
  mNormalSpeed = 0.0f;
  m35A0 = 0.34906587f; /* M_PI / (9 - 0) */
  return TRUE;
}

/* 02421200 */
BOOL daPy_lk_c::dProcDamage() {
  /* cM_fsin(x) = cM_ssin(cM_rad2s(x)) */
  s16 a = lk05DemoRad2s(m35A0 * mFrameCtrlUnder[0].getFrame());
  mNormalSpeed = -2.0f * cM_ssin(a);
  if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
  }
  return TRUE;
}

/* 0242128C */
BOOL daPy_lk_c::dProcHoldup_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_HOLDUP_e) {
    return FALSE;
  }
  u32 whide = (mModeFlg >> 4) & 1; /* checkModeFlg(ModeFlg_WHIDE) */
  lk05CommonProcInit(this, 0xAC);
  /* HD: HIO folded */
  if (whide) {
    int anm = (mNoResetFlg0 & 0x10000) ? link05demo::ANM_WALLHOLDUPDW
                                       : link05demo::ANM_WALLHOLDUP;
    mModeFlg = mModeFlg | 0x10;
    lk05DemoSetBgCheckParam(this);
    lk05SetSingleMoveAnime(this, anm, 1.0f, 0.0f, 0x3C, 5.0f);
  } else {
    lk05SetSingleMoveAnime(this, link05demo::ANM_HOLDUP, 1.1f, 0.0f, 0xB, 5.0f);
  }
  mNormalSpeed = 0.0f;
  return TRUE;
}

/* 024213E4 */
BOOL daPy_lk_c::dProcHoldup() {
  if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
  }
  return TRUE;
}

/* 02421434 */
BOOL daPy_lk_c::dProcOpenTreasure_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_OPEN_TREASURE_e) {
    return TRUE;
  }
  lk05CommonProcInit(this, 0xAD);
  f32 dist;
  if (demoParam0() == 1) {
    dist = 75.0f;
    lk05SetSingleMoveAnime(this, link05demo::ANM_BOXOPENSHORTLINK, 1.0f, 0.0f,
                           -1, 3.0f);
  } else {
    dist = 55.0f;
    lk05SetSingleMoveAnime(this, link05demo::ANM_BOXOPENLINK, 1.0f, 0.0f, -1,
                           3.0f);
  }
  mNormalSpeed = 0.0f;
  if (mEquipItem == link05demo::daPyItem_BOKO_e) {
    fopAc_ac_c *boko = mActorKeepEquip.mActor;
    if (boko != nullptr) {
      PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y,
                 current.pos.z);
      mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(shape_angle.y + 0x2000));
      /* daBoko_c::setMatrix(): the model's base matrix */
      J3DModel *model = reinterpret_cast<Link05DemoBoko *>(boko)->mModel;
      if (model != 0) {
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
      }
    }
  }
  fopAc_ac_c *partner = lk05DemoEventPartner(this);
  if (partner != nullptr) {
    s16 a = (s16)(partner->shape_angle.y - 0x8000);
    shape_angle.y = a;
    current.angle.y = a;
    current.pos.x = gabi::fnmsubs(dist, cM_ssin(a), partner->current.pos.x);
    f32 c = cM_scos(a);
    current.pos.z = gabi::fnmsubs(dist, c, partner->current.pos.z);
  }
  lk05DeleteEquipItem(this, 0);
  mAcch.m_flags =
      (mAcch.m_flags | 4) & ~0x2000u; /* SetWallNone(), OffLineCheck() */
  return TRUE;
}

/* 02421678 */
BOOL daPy_lk_c::dProcOpenTreasure() {
  if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
  }
  return TRUE;
}

/* 024216C8 */
BOOL daPy_lk_c::dProcGetItem_init() {
  s16 sVar6 = 0;
  s16 sVar5 = 0;
  s32 proc = mCurProc;
  if (proc == link05demo::daPyProc_DEMO_GET_ITEM_e) {
    return TRUE;
  }
  if (proc == link05demo::daPyProc_DEMO_OPEN_TREASURE_e) {
    mRootMoveMode = 10;
    mAcch.m_flags = mAcch.m_flags & ~4u; /* ClrWallNone() */
    if (lk05DemoUnderHeap(this, 0)->mIdx !=
        0x38 /* m_anm_heap_under[UNDER_MOVE0_e].mIdx != BOXOPENLINK */) {
      sVar5 = 1;
    }
  } else if (proc == link05demo::daPyProc_DEMO_OPEN_SALVAGE_TREASURE_e) {
    mRootMoveMode = 6;
    sVar6 = 1;
  }
  int use = lk05DemoCheckShipRideUseItem(this, 0);
  lk05CommonProcInit(this, 0xAE);
  mNormalSpeed = 0.0f;
  s32 item;
  if (mEquipItem == link05demo::daPyItem_NONE_e) {
    lk05SetSingleMoveAnime(this, link05demo::ANM_ITEMGET, 1.0f, 0.0f, -1, 3.0f);
    u32 flg = mModeFlg;
    item = demoParam0();
    mProcVar6 = 0;
    mModeFlg = flg | 0x400;
  } else {
    lk05SetBlendMoveAnime(this, 2.4f); /* HD: HIO folded */
    lk05DemoSetAnimeUnequip(this);
    item = demoParam0();
    m3598 = 0.0f;
    mProcVar6 = 1;
  }
  if (item == 0) {
    mProcVar2 = -1;
  } else {
    if (item == 0x100) {
      item = lk05DemoPlay()->mGetItem; /* dComIfGp_event_getGtItm() */
    }
    s32 id = lk05DemoCreateItem(&current.pos, item, 0, -1,
                                (s32)(s8)current.roomNo, 0, 0);
    if (id != -1) {
      dComIfGp_event_setItemPartnerId(
          id); /* dComIfGp_event_setItemPartnerId() */
    }
    mProcVar2 = (s16)item;
  }
  s32 idx = mCameraInfoIdx;
  Link05DemoCamera *cam =
      reinterpret_cast<Link05DemoCamera *>(dComIfGp_getCamera(idx));
  s16 sy = shape_angle.y;
  mGameOverId = 0xFFFFFFFF;
  mProcVar0 = 0;
  mProcVar3 = (s16)(sy - cam->mAngleY /* fopCamM_GetAngleY() */);
  current.angle.y = sy;
  mProcVar4 = sVar6;
  lk05DemoInitShipRideUseItem(this, use, 0);
  mProcVar7 = 0;
  m3624 = 0;
  mProcVar5 = sVar5;
  lk05DemoItemGetColorOn();
  return TRUE;
}

/* 02421D84 */
BOOL daPy_lk_c::dProcUnequip_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_UNEQUIP_e) {
    return FALSE;
  }
  lk05CommonProcInit(this, 0xAF);
  mNormalSpeed = 0.0f;
  lk05SetBlendMoveAnime(this, 2.4f); /* HD: HIO folded */
  if (mEquipItem != link05demo::daPyItem_NONE_e) {
    lk05DemoSetAnimeUnequip(this);
    m3598 = 0.0f;
  }
  return TRUE;
}

/* 02421E38 */
BOOL daPy_lk_c::dProcUnequip() {
  if (lk05DemoUpperHeap(this, 2)->mIdx == 0xFFFF /* checkNoUpperAnime() */) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
  }
  return TRUE;
}

/* 02421E80 */
BOOL daPy_lk_c::dProcLavaDamage() {
  if (mProcVar6 == 1) {
    if (lk05DemoCompulsory(&link05::gameInfo()->play.mEvtCtrl, this, 0,
                           0xFFFF)) {
      lk05DemoLavaInitSub(this);
    }
  } else if (speed.y < -gravity && lkT60_lavaRestartTest()) { /* true60: once a frame (d_a_player_lavarestart_t60.h) */
    lk05DemoRestartRoom(this, 4, 0xCA, -1.0f, 1);
  }
  return TRUE;
}

/* 02421F1C */
BOOL daPy_lk_c::dProcFreezeDamage() {
  if (mProcVar6 == 0) {
    if (lk05DemoCompulsory(&link05::gameInfo()->play.mEvtCtrl, this, 0,
                           0xFFFF)) {
      lk05DemoFreezeInitSub(this, 1);
    }
  } else if (lkT60_countStep()) { /* true60 (the 30 Hz timer rule; the first-step AI-timer rule for Link): the freeze countdown
                                    * counts 30 Hz frames, with its end test, on the frame's FIRST step */
    s16 pv0 = (s16)(mProcVar0 - 1);
    mProcVar0 = pv0;
    if (pv0 == 0) {
      lk05DemoRestartRoom(this, 5, 0xC9, -1.0f, 1);
    }
  }
  return TRUE;
}

/* 02421FBC */
BOOL daPy_lk_c::changeSwimUpProc() {
  f32 sy = speed.y;
  const f32 lkT60_vStart = sy; /* true60 (surface test): speed.y before this step's push */
  if (sy < 9.5f) { /* HD: HIO folded */
    /* true60 (the per-step movement rule): the rise's push per update: x dt */
    const f32 sy0 = sy;
    sy = sy + (t60::dt() == 1.0f ? 6.0f : 6.0f * (f32)t60::dt());
    if (sy > 9.5f) {
      speed.y = 9.5f;
      if (mEquipItem != link05demo::daPyItem_NONE_e) {
        lk05DeleteEquipItem(this, 1);
      }
    } else {
      speed.y = sy;
      if (sy > 0.0f && mEquipItem != 0x100) {
        lk05DeleteEquipItem(this, 1);
      }
    }
    /* true60 (d_a_player_swimrise_t60.h): the push enters posMoveFromFootPos's exact-arc term */
    lkT60_swimRiseRecord(sy0, (f32)speed.y, sy > 9.5f);
  }
  /* true60 (the process-order read rule; s85 bko84s3: SWIM_UP B f147 / A f148 with y exact through f146): the
   * surface test is A's one test of the frame, before the frame's move: on the frame's first step from the frame-start
   * pos.y and A's pushed speed.y (lkT60_swimSurfaceSpeed); the later steps do not test (step 2 tested the half-frame
   * pos.y, 4.75 higher, and crossed the 55.1 depth a frame early). dt 1: the original test. */
  if (t60::dt() == 1.0f ? (mWaterY - current.pos.y < 55.1f && !(speed.y < 0.0f))
                        : (t60::first_step() && mWaterY - current.pos.y < 55.1f &&
                           !(lkT60_swimSurfaceSpeed(lkT60_vStart) < 0.0f))) {
    lkT60_swimRiseDrop(); /* true60: the surface sets pos.y / speed.y below: no push in this step's add */
    if (mCurProc == link05demo::daPyProc_DEMO_DEAD_e) {
      mNoResetFlg0 = mNoResetFlg0 | 0x100;
      lk05CopyFloatBits(&current.pos.y, &mWaterY);
      return TRUE;
    }
    return lk05DemoSwimUpInit(this, 1);
  }
  return FALSE;
}

/* 024224B8 */
BOOL daPy_lk_c::dProcLookAround_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_LOOK_AROUND_e) {
    return FALSE;
  }
  lk05CommonProcInit(this, 0xB3);
  lk05SetSingleMoveAnime(this, link05demo::ANM_COMEOUT, 1.0f, 0.0f, 0x48,
                         1.0f); /* HD: HIO folded */
  mNormalSpeed = 0.0f;
  return TRUE;
}

/* 02422564 */
BOOL daPy_lk_c::dProcLookAround() {
  if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
  }
  return TRUE;
}

/* 024225B4 */
BOOL daPy_lk_c::dProcSalute_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_SALUTE_e) {
    return FALSE;
  }
  lk05CommonProcInit(this, 0xB4);
  lk05SetSingleMoveAnime(this, link05demo::ANM_SALTATION, 1.0f, 0.0f, -1, 3.0f);
  mNormalSpeed = 0.0f;
  return TRUE;
}

/* 02422664 */
BOOL daPy_lk_c::dProcSalute() {
  if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
  }
  return TRUE;
}

/* 024226B4 */
BOOL daPy_lk_c::dProcLookAround2_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_LOOK_AROUND2_e) {
    return FALSE;
  }
  int use = lk05DemoCheckShipRideUseItem(this, 1);
  lk05CommonProcInit(this, 0xB5);
  lk05SetSingleMoveAnime(this, link05demo::ANM_WHO, 1.0f, 0.0f, -1, 5.0f);
  mNormalSpeed = 0.0f;
  lk05DemoInitShipRideUseItem(this, use, 0);
  return TRUE;
}

/* 02422790 */
BOOL daPy_lk_c::dProcLookAround2() {
  if (dComIfGp_checkPlayerStatus0(0, link05demo::daPyStts0_SHIP_RIDE_e)) {
    lk05SetShipRidePos(this, 0);
  }
  dComIfGp_evmng_cutEnd(mStaffIdx);
  return TRUE;
}

/* 024227EC */
BOOL daPy_lk_c::dProcTalismanPickup_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_TALISMAN_PICKUP_e) {
    return FALSE;
  }
  int use = lk05DemoCheckShipRideUseItem(this, 1);
  lk05CommonProcInit(this, 0xB6);
  lk05SetSingleMoveAnime(this, link05demo::ANM_PICKUP, 1.5f, 2.0f, 0x59,
                         10.0f); /* HD: HIO folded */
  mNormalSpeed = 0.0f;
  lk05DemoInitShipRideUseItem(this, use, 1);
  return TRUE;
}

/* 024228A8 */
BOOL daPy_lk_c::dProcTalismanPickup() {
  J3DFrameCtrl &frameCtrl = mFrameCtrlUnder[0];
  if (dComIfGp_checkPlayerStatus0(0, link05demo::daPyStts0_SHIP_RIDE_e)) {
    lk05SetShipRidePos(this, 1);
  }
  if (lkT60_endRate(&frameCtrl) < 0.01f) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
  } else if (lkT60_pass(&frameCtrl, 65.0f)) {
    seStartOnlyReverb(link05demo::JA_SE_LK_ITEM_TAKEOUT);
  }
  return TRUE;
}

/* 02422950 */
BOOL daPy_lk_c::dProcTalismanWait_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_TALISMAN_WAIT_e) {
    return FALSE;
  }
  int use = lk05DemoCheckShipRideUseItem(this, 1);
  lk05CommonProcInit(this, 0xB7);
  lk05SetSingleMoveAnime(this, link05demo::ANM_WAITPICKUP, 1.0f, 0.0f, -1,
                         10.0f); /* HD: HIO folded */
  mNormalSpeed = 0.0f;
  setTalismanModel();
  J3DModel *model =
      reinterpret_cast<Link05EquipmentHD *>(this)->mpEquipItemModel;
  lk05DemoMakeEmitter(&mProcEffectEcallBack0, 0x3E9 /* ID_IT_JN_OMAMORI_TSUBU00 */,
                      model->getBaseTRMtx(), &current.pos, 0);
  Mtx34 *hand = lk05DemoAnmMatrix(mpCLModel, 12 /* CL_JNT_CL_RHANDA_e */);
  J3DModel_setBaseTRMtx(
      reinterpret_cast<Link05EquipmentHD *>(this)->mpEquipItemModel.get(),
      hand);
  lk05DemoTalismanPosition(this);
  dPa_control_set(dComIfGp_getParticle(), 1,
                  0x3EA /* ID_IT_JN_OMAMORI_FLASH00 */,
                  &lk05DemoFollow(this)->mPos, nullptr, nullptr, 0xFF,
                  reinterpret_cast<dPa_levelEcallBack *>(mTalismanFlashEcallBack) /* mTalismanFlashEcallBack */, -1,
                  nullptr, nullptr, nullptr);
  lk05DemoInitShipRideUseItem(this, use, 1);
  return TRUE;
}

/* 02422BA8 */
BOOL daPy_lk_c::dProcTalismanWait() {
  if (dComIfGp_checkPlayerStatus0(0, link05demo::daPyStts0_SHIP_RIDE_e)) {
    lk05SetShipRidePos(this, 1);
  }
  lk05DemoTalismanPosition(this);
  PSMTXTrans(mDoMtx_stack_c::get(), link05::l_matrixStack.m[0][3],
             link05::l_matrixStack.m[1][3], link05::l_matrixStack.m[2][3]);
  if (t60::dt() == 1.0f) {
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(link05::g_Counter.mTimer * 0x1FF));
  } else {
    /* true60 (the 60 Hz counter rule): the charm's spin follows the 30 Hz counter g_Counter (constant over a
     * frame's steps); step k of the frame shows the spin at (k + 1) x dt of the frame (the tick30 step = the
     * original angle) */
    f32 lag = lk05CounterLag();
    mDoMtx_YrotM(mDoMtx_stack_c::get(),
                 (s16)((s32)(link05::g_Counter.mTimer * 0x1FF) - (s32)(lag * (f32)0x1FF)));
  }
  J3DModel_setBaseTRMtx(
      reinterpret_cast<Link05EquipmentHD *>(this)->mpEquipItemModel.get(),
      mDoMtx_stack_c::get());
  lk05DemoPlayBaseAnm(lk05DemoSword(this));
  J3DModelData *data = reinterpret_cast<Link05EquipmentHD *>(this)
                           ->mpEquipItemModel->mModelData; /* getModelData() */
  lk05DemoEntryBck(
      lk05DemoSword(this), data,
      reinterpret_cast<Link05DemoSwordFrame *>(lk05DemoSword(this))->mFrame);
  /* true60 (the one-emitter-per-event rule): sustained start on every update: on the frame's step */
  if (lk05FrameStep())
    lk05SeStart(link05demo::JA_SE_OBJ_OMAMORI, &lk05DemoFollow(this)->mPos, 0,
                (s32)(s8)mReverb);
  u32 flg = mModeFlg;
  if (demoParam0() != 0) {
    mModeFlg = flg | 0x80;
  } else {
    mModeFlg = flg & ~0x80u;
  }
  dComIfGp_evmng_cutEnd(mStaffIdx);
  return TRUE;
}

/* 02422D9C */
BOOL daPy_lk_c::dProcSurprised_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_SURPRISED_e) {
    return FALSE;
  }
  int use = lk05DemoCheckShipRideUseItem(this, 1);
  lk05CommonProcInit(this, 0xB8);
  lk05SetSingleMoveAnime(this, link05demo::ANM_SURPRISED, 1.0f, 0.0f, -1, 3.0f);
  s32 p0 = demoParam0();
  mNormalSpeed = 0.0f;
  mProcVar6 = 0;
  if (p0 == 1) {
    lk05VoiceStart(this, 28);
  } else if (p0 == 2) {
    lk05VoiceStart(this, 45);
  } else if (p0 == 3) {
    lk05VoiceStart(this, 49);
  }
  lk05DemoInitShipRideUseItem(this, use, 1);
  return TRUE;
}

/* 02422F14 */
BOOL daPy_lk_c::dProcSurprised() {
  if (dComIfGp_checkPlayerStatus0(0, link05demo::daPyStts0_SHIP_RIDE_e)) {
    lk05SetShipRidePos(this, 1);
  }
  if (mProcVar6 != 0 || lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
    if (mProcVar6 == 0) {
      u16 u2 = mTexScrollFrame;
      u16 u1 = mTexAnmFrame;
      mProcVar6 = 1;
      lk05SetSingleMoveAnime(this, link05demo::ANM_SURPRISEDWAIT, 1.0f, 0.0f,
                             -1, 3.0f);
      mTexAnmFrame = u1;
      mTexScrollFrame = u2;
      mModeFlg = mModeFlg & ~0x400u;
    }
  }
  return TRUE;
}

/* 02422FF8 */
BOOL daPy_lk_c::dProcTurnBack_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_TURN_BACK_e) {
    return FALSE;
  }
  lk05CommonProcInit(this, 0xB9);
  fopAc_ac_c *grab = mActorKeepGrab.mActor;
  mNormalSpeed = 0.0f;
  if (grab != nullptr) {
    lk05SetBlendMoveAnime(this, 2.4f); /* HD: HIO folded */
    lk05DemoUpperRatioAll(m_pbCalc[1], 1.0f);
    mProcVar6 = 1;
  } else {
    lk05SetSingleMoveAnime(this, link05demo::ANM_TURNBACK, 1.0f, 0.0f, 0x2C,
                           5.0f);
    mProcVar6 = 0;
  }
  return TRUE;
}

/* 02423124 */
BOOL daPy_lk_c::dProcTurnBack() {
  if (mProcVar6 != 0 || lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
  }
  return TRUE;
}

/* 02423180 */
BOOL daPy_lk_c::dProcLookUp_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_LOOK_UP_e) {
    return FALSE;
  }
  lk05CommonProcInit(this, 0xBA);
  lk05SetSingleMoveAnime(this, link05demo::ANM_LOOKUP, 1.0f, 0.0f, -1, 5.0f);
  mNormalSpeed = 0.0f;
  return TRUE;
}

/* 02423230 */
BOOL daPy_lk_c::dProcLookUp() {
  if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
  }
  return TRUE;
}

/* 02423280 */
BOOL daPy_lk_c::dProcQuakeWait_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_QUAKE_WAIT_e) {
    return FALSE;
  }
  lk05CommonProcInit(this, 0xBB);
  lk05SetSingleMoveAnime(this, link05demo::ANM_WAITQ, 1.0f, 0.0f, -1, 5.0f);
  lk05SetTextureAnime(this, 104, 0);
  mNormalSpeed = 0.0f;
  return TRUE;
}

/* 02423340 */
BOOL daPy_lk_c::dProcQuakeWait() {
  dComIfGp_evmng_cutEnd(mStaffIdx);
  return TRUE;
}

/* 0242337C */
BOOL daPy_lk_c::dProcDance_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_DANCE_e) {
    return FALSE;
  }
  lk05CommonProcInit(this, 0xBC);
  lk05SetSingleMoveAnime(this, link05demo::ANM_GLAD, 1.0f, 0.0f, -1, 5.0f);
  mNormalSpeed = 0.0f;
  return TRUE;
}

/* 0242342C */
BOOL daPy_lk_c::dProcDance() {
  dComIfGp_evmng_cutEnd(mStaffIdx);
  return TRUE;
}

/* 02423468 */
BOOL daPy_lk_c::dProcCaught_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_CAUGHT_e) {
    return TRUE;
  }
  lk05CommonProcInit(this, 0xBD);
  if (demoParam0() == 0) {
    mProcVar6 = 1;
    lk05SetSingleMoveAnime(this, link05demo::ANM_MOGAKU1, 1.0f, 0.0f, -1, 3.0f);
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    speed.y = 0.0f;
  } else {
    mProcVar6 = 0;
    lk05SetSingleMoveAnime(this, link05demo::ANM_FM_BATA, 1.0f, 0.0f, -1, 3.0f);
    gravity = 0.0f;
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
  }
  return TRUE;
}

/* 02423554 */
BOOL daPy_lk_c::dProcCaught() {
  dComIfGp_evmng_cutEnd(mStaffIdx);
  return TRUE;
}

/* 02423590 */
BOOL daPy_lk_c::dProcLookWait() {
  fopAc_ac_c *look = lk05DemoLookActor(this);
  if (dComIfGp_checkPlayerStatus0(0, link05demo::daPyStts0_SHIP_RIDE_e)) {
    lk05SetShipRidePos(this, 0);
  }
  if (look != nullptr) {
    s16 t = lk05DemoTargetAngleY(&current.pos, &look->eyePos);
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
  dComIfGp_evmng_cutEnd(mStaffIdx);
  return TRUE;
}

/* 02423680 */
BOOL daPy_lk_c::dProcPushPullWait_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_PUSH_PULL_WAIT_e) {
    return FALSE;
  }
  lk05CommonProcInit(this, 0xBF);
  lk05SetSingleMoveAnime(this, link05demo::ANM_WAITPUSHPULL, 1.0f, 0.0f, -1,
                         5.0f); /* HD: HIO folded */
  mNormalSpeed = 0.0f;
  dComIfGp_setPlayerStatus0(0, link05demo::daPyStts0_UNK4000000_e);
  return TRUE;
}

/* 02423740 */
BOOL daPy_lk_c::dProcPushPullWait() {
  dComIfGp_evmng_cutEnd(mStaffIdx);
  return TRUE;
}

/* 0242377C */
BOOL daPy_lk_c::dProcPushMove_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_PUSH_MOVE_e) {
    return FALSE;
  }
  lk05CommonProcInit(this, 0xC0);
  lk05SetSingleMoveAnime(this, link05demo::ANM_WALKPUSH, 1.0f, 0.0f, -1,
                         5.0f); /* HD: HIO folded */
  mNormalSpeed = 0.0f;
  dComIfGp_setPlayerStatus0(0, link05demo::daPyStts0_UNK4000000_e);
  lk05VoiceStart(this, 19);
  return TRUE;
}

/* 02423854 */
BOOL daPy_lk_c::dProcPushMove() {
  dComIfGp_evmng_cutEnd(mStaffIdx);
  return TRUE;
}

/* 02423890 */
BOOL daPy_lk_c::dProcDoorOpen_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_DOOR_OPEN_e) {
    return TRUE;
  }
  lk05CommonProcInit(this, 0xC1);
  s32 p0 = demoParam0();
  int anm =
      (p0 & 1) ? link05demo::ANM_DOOROPENBLINK : link05demo::ANM_DOOROPENALINK;
  mProcVar7 = 0;
  if (p0 & 2) {
    mProcVar7 = 3; /* HD */
    lk05SetSingleMoveAnime(this, anm, 1.0f, 35.0f, -1, 0.0f);
  } else {
    lk05SetSingleMoveAnime(this, anm, 1.0f, 0.0f, -1, 0.0f);
  }
  mNormalSpeed = 0.0f;
  mAcch.m_flags = mAcch.m_flags | 0x4004; /* SetWallNone(), OnLineCheckNone() */
  mPrevRootTrans.copy(link05::l_zero);
  mProcVar6 = 0;
  mRootMoveMode = 3;
  return TRUE;
}

/* 024239D8 */
BOOL daPy_lk_c::dProcDoorOpen() {
  s32 pv7 = mProcVar7;
  /* true60 (the 30 Hz timer rule; the first-step AI-timer rule for Link): the HD door counter (3 frames from a mid-way init) counts 30 Hz
   * frames: the count and the flag it drives on the frame's FIRST step (count before the decision: no hold; the later
   * steps leave the flag as the first step set it) */
  if (pv7 != 0 && lkT60_countStep()) {
    /* HD: a no-reset flag (0x08000000) held while the counter runs */
    pv7 = pv7 - 1;
    mProcVar7 = pv7;
    if (pv7 != 0) {
      mNoResetFlg0 = mNoResetFlg0 | 0x8000000;
    } else {
      mNoResetFlg0 = mNoResetFlg0 & ~0x8000000u;
    }
  }
  if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    mAcch.m_flags =
        mAcch.m_flags & ~0x4004u; /* ClrWallNone(), OffLineCheckNone() */
    dComIfGp_evmng_cutEnd(mStaffIdx);
    lk05SetBlendMoveAnime(this, 2.4f); /* HD: HIO folded */
    mProcVar6 = 1;
    m3598 = 0.0f;
  } else if (mProcVar6 == 0) {
    if (mRootMoveMode != 3) {
      mRootMoveMode = 1;
    }
  } else {
    dComIfGp_evmng_cutEnd(mStaffIdx);
  }
  return TRUE;
}

/* 02423AEC */
BOOL daPy_lk_c::dProcNod_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_NOD_e) {
    return TRUE;
  }
  lk05CommonProcInit(this, 0xC2);
  mNormalSpeed = 0.0f;
  lk05SetSingleMoveAnime(this, link05demo::ANM_SEYYES, 1.0f, 0.0f, 0x1C,
                         5.0f); /* HD: HIO folded */
  dComIfGp_setPlayerStatus0(0, link05demo::daPyStts0_UNK10_e);
  return TRUE;
}

/* 02423B6C */
BOOL daPy_lk_c::dProcNod() {
  setShapeAngleToTalkActor();
  current.angle.y = shape_angle.y;
  if (lkT60_endRate(&mFrameCtrlUnder[0]) < 0.01f) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
  }
  return TRUE;
}

/* 02423BCC */
void daPy_lk_c::dProcPresent_init_sub() {
  lk05SetSingleMoveAnime(this, link05demo::ANM_PRESENTATIONA, 1.1f, 0.0f, 0x3E,
                         2.0f); /* HD: HIO folded */
  mModeFlg = mModeFlg | 0x400;
  if (!checkBottleItem(
          dComIfGp_event_getPreItemNo() /* dComIfGp_event_getPreItemNo() */)) {
    s32 id =
        lk05DemoCreateItem(&current.pos, (u32)dComIfGp_event_getPreItemNo(), 3,
                           -1, (s32)(s8)current.roomNo, &shape_angle, &scale);
    dComIfGp_event_setItemPartnerId(id); /* dComIfGp_event_setItemPartnerId() */
  }
  lk05DemoKeepItemData(this);
}

/* 02423C84 */
BOOL daPy_lk_c::dProcPresent_init() {
  if (mCurProc == link05demo::daPyProc_DEMO_PRESENT_e) {
    return TRUE;
  }
  lk05CommonProcInit(this, 0xC3);
  s32 r = lk05DemoTalkStartBack(this);
  mProcVar6 = r;
  if (r == 0) {
    mNormalSpeed = 0.0f;
    dProcPresent_init_sub();
  }
  dComIfGp_setPlayerStatus0(0, link05demo::daPyStts0_UNK10_e);
  lk05SeStartSystem(link05demo::JA_SE_TALK_START);
  mProcVar7 = 0;
  return TRUE;
}

/* 02420F08 */
BOOL daPy_lk_c::dProcTalk() {
  dComIfGp_evmng_cutEnd(mStaffIdx);
  if (dComIfGp_checkPlayerStatus0(0, link05demo::daPyStts0_SHIP_RIDE_e)) {
    lk05SetShipRidePos(this, 0);
  }
  setShapeAngleToTalkActor();
  if (mProcVar6 != 0) {
    gabi::Local<cXyz> d;
    cXyz_mi(&m370C, d, &current.pos);
    s32 iVar2 = 0;
    if (mAcch.m_flags & 0x10 /* ChkWallHit() */) {
      for (int i = 0; i < 3; i++) {
        dBgS_AcchCir *cir = &mAcchCir[i];
        if (cir->m_flags & 2) {
          iVar2 = cLib_distanceAngleS(cir->m_wall_angle_y /* GetWallAngleY() */,
                                      current.angle.y);
          break;
        }
      }
    }
    if (lk05TalkBackPassed(cLib_distanceAngleS(cM_atan2s(d->x, d->z), current.angle.y) > 0x4000 ||
        iVar2 > 0x4000) /* true60 */) {
      u32 mode = mDemo.getDemoMode();
      lk05CopyFloatBits(&current.pos.x, &m370C.x);
      mProcVar6 = 0;
      lk05CopyFloatBits(&current.pos.z, &m370C.z);
      if (mode == 8) {
        lk05SetSingleMoveAnime(this, link05demo::ANM_WAITS, 1.1f, 0.0f, -1,
                               5.0f); /* HD: HIO folded */
      } else {
        lk05SetSingleMoveAnime(this, link05demo::ANM_TALKA, 0.7f, 0.0f, -1,
                               5.0f);
      }
      if (mProcVar7 == 1) {
        lk05SetTextureAnime(this, 145, 0);
        mModeFlg = mModeFlg & ~0x80u;
      }
      mNormalSpeed = 0.0f;
      lk05TalkBackSnapped(); /* true60: the snap frame's later steps hold */
    }
  } else if (lk05TalkBackSnapHeld()) {
    /* true60: A's snap frame runs the step-back branch only; its heading follows the next frame */
  } else {
    u32 pv7 = mProcVar7;
    u32 p1 = mDemo.getParam1(); /* mDemo.getParam1() */
    current.angle.y = shape_angle.y;
    if (p1 != pv7) {
      mProcVar7 = p1;
      if (p1 == 1) {
        lk05SetTextureAnime(this, 145, (u32)mTexAnmFrame);
        mModeFlg = mModeFlg & ~0x80u;
      } else {
        lk05SetTextureAnime(this, 0, (u32)mTexAnmFrame);
        mModeFlg = mModeFlg | 0x80;
      }
    }
  }
  return TRUE;
}

/* 02421948 */
BOOL daPy_lk_c::dProcGetItem() {
  if (dComIfGp_checkPlayerStatus0(0, link05demo::daPyStts0_SHIP_RIDE_e)) {
    lk05SetShipRidePos(this, 0);
  }
  if (mProcVar7 != 0) {
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
  }
  J3DFrameCtrl &fc = mFrameCtrlUnder[0];
  if (fc.getFrame() < 10.0f) {
    s32 idx = mCameraInfoIdx;
    Link05DemoCamera *cam =
        reinterpret_cast<Link05DemoCamera *>(dComIfGp_getCamera(idx));
    mProcVar3 = (s16)(shape_angle.y - cam->mAngleY /* fopCamM_GetAngleY() */);
  }
  fopAc_ac_c *item = lk05DemoGetItemPartner(this);
  s32 itemNo = mProcVar2;
  if (itemNo == -1 && item != 0) {
    itemNo = lk05DemoGetItemNo(item);
    mProcVar2 = (s16)itemNo;
  }
  bool toRate;
  if (mProcVar6 == 0) {
    f32 frame = fc.getFrame();
    if (frame < 11.0f) {
      toRate = true;
    } else {
      if (mProcVar0 == 0 && itemNo != -1) {
        u16 no = (u16)mProcVar2;
        s16 pv5 = mProcVar5;
        mProcVar0 = 1;
        lk05DemoGetItemSound(this, (u32)no, (s32)pv5);
        frame = fc.getFrame();
      }
      s16 cur = current.angle.y;
      if (frame > 17.0f) {
        shape_angle.y = (s16)(cur - mProcVar3);
      } else {
        f32 t = frame - 10.0f;
        s16 pv3 = mProcVar3;
        if (pv3 > 0x4000) {
          shape_angle.y =
              (s16)gabi::ftoi((f32)cur + ((f32)(0x10000 - pv3) * t) / 7.0f);
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
    if (lk05DemoUpperHeap(this, 2)->mIdx == 0xFFFF /* checkNoUpperAnime() */) {
      lk05SetSingleMoveAnime(this, link05demo::ANM_ITEMGET, 1.0f, 0.0f, -1,
                             3.0f);
      u32 flg = mModeFlg;
      mProcVar6 = 0;
      mModeFlg = flg | 0x400;
    }
    if (item != 0 && (fpcM_GetName(item) == link05demo::fpcNm_ITEM_e ||
                      fpcM_GetName(item) == link05demo::fpcNm_Demo_Item_e)) {
      lk05DemoHideItem(item);
    }
    return TRUE;
  }
  if (item == 0) {
    return TRUE;
  }
  s16 name = fpcM_GetName(item);
  if (!(lkT60_endRate(&fc) < 0.01f)) {
    if (name == 0xFF || fpcM_GetName(item) == 0x101) {
      lk05DemoHideItem(item);
    }
    return TRUE;
  }
  if (name != 0xFF && fpcM_GetName(item) != 0x101) {
    return TRUE;
  }
  lk05DemoShowItem(item);
  s32 msg = m3624;
  if (msg == 0) {
    s32 no = mProcVar2;
    if (no == -1) {
      return TRUE;
    }
    /* HD: the max life is read only for item 7 (the heart container) */
    u16 maxLife = no == 7 ? dComIfGs_getMaxLife() : 0;
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
    lk05SetOldRootQuaternion(this, 0, -0x8000, 0);
    /* m_old_fdata->getOldFrameTransInfo(0): mTranslate.x / .z negated */
    Link05DemoOldTransform *info =
        reinterpret_cast<Link05DemoOldFrame *>(gabi::at<void>(m_old_fdata))
            ->mpTransform;
    f32 x = info->mTranslate.x;
    f32 z = info->mTranslate.z;
    info->mTranslate.x = -x;
    info->mTranslate.z = -z;
    dComIfGp_evmng_cutEnd(mStaffIdx);
    lk05DemoDeleteItem(item);
    mProcVar7 = 1;
    lk05SetBlendMoveAnime(this, 2.4f); /* HD: HIO folded */
  }
  return TRUE;
}

/* 02422104 */
BOOL daPy_lk_c::dProcDead() {
  J3DFrameCtrl &frameCtrl = mFrameCtrlUnder[0];
  if (dComIfGp_checkPlayerStatus0(0, link05demo::daPyStts0_SHIP_RIDE_e)) {
    lk05SetShipRidePos(this, 1);
  }
  if (mProcVar6 != 0 &&
      lk05DemoCompulsory(&link05::gameInfo()->play.mEvtCtrl, this, 0, 0xFFFF)) {
    lk05DemoDeadInitSub(this);
    if (mProcVar2 == 0) {
      lk05DemoDeadInitSub2(this);
    }
  }
  if (mProcVar2 != 0 && changeSwimUpProc()) {
    s32 pv6 = mProcVar6;
    mProcVar2 = 0;
    if (pv6 != 0) {
      return TRUE;
    }
    lk05DemoDeadInitSub2(this);
  }
  if (mProcVar6 != 0 || mProcVar2 != 0) {
    return TRUE;
  }
  if (mModeFlg & 0x2000000 /* ModeFlg_02000000 */) {
    m35E4 = (frameCtrl.getFrame() - 120.0f) * 0.1f;
  }
  if (lkT60_endRate(&frameCtrl) < 0.01f) {
    cLib_chaseF(&m35A0, 0.0f, 0.01f);
    /* HD: the game-over object is looked up by id (025DE50C); the back alpha
     * setter takes m35A0 */
    s32 id = mGameOverId;
    f32 ratio = m35A0;
    if (id != -1) {
      Link05DemoGameOver *go = lk05DemoFindGameOver(id);
      if (go != 0) {
        lk05DemoGameOverAlpha(go, ratio);
      }
      ratio = m35A0;
    }
    if (ratio < 0.38f && (id = mGameOverId) != -1) {
      Link05DemoGameOver *go = lk05DemoFindGameOver(id);
      if (go != 0) {
        go->mAnimeStart = 1; /* d_GameOver_animeStart() */
      }
    }
    if (mProcVar7 == 0) {
      id = mGameOverId;
      if (id == -1) {
        return TRUE;
      }
      Link05DemoGameOver *go = lk05DemoFindGameOver(id);
      if (go != 0 && lk05DemoGameOverDelete(go)) {
        mProcVar7 = 1;
      } else if (mProcVar7 == 0) {
        return TRUE;
      }
    }
    if (lk05DemoPlay()->mGameoverStatus ==
        2 /* dComIfGp_getGameoverStatus() */) {
      lk05DemoGameStartStage();
      lk05DemoGameStart();
    }
    return TRUE;
  }
  if (!(dComIfGp_checkPlayerStatus0(0, 0xFFFFFFFF) &
        0x110000 /* daPyStts0_SHIP_RIDE_e | daPyStts0_SWIM_e */)) {
    if (lkT60_pass(&frameCtrl, 129.0f)) {
      lk05VoiceStart(this, 0x17);
      mFootEffectPosType = 6;
    } else if (!(frameCtrl.getFrame() < 129.0f)) {
      lk05ResetFootEffect(this);
    }
    return TRUE;
  }
  if (!dComIfGp_checkPlayerStatus0(0, link05demo::daPyStts0_SWIM_e)) {
    return TRUE;
  }
  if (lkT60_pass(&frameCtrl, 9.0f) || lkT60_pass(&frameCtrl, 15.0f)) {
    gabi::Local<csXyz> ang;
    lk05DemoConstructAngle(ang.get(), 0, (s32)shape_angle.y, 0);
    gabi::Local<GXColor> amb, dif;
    lk05DemoSeaColor(amb.get(), dif.get());
    if (lkT60_pass(&frameCtrl, 15.0f)) {
      ang->y = (s16)(ang->y - 0x8000);
    }
    dPa_control_set(dComIfGp_getParticle(), 1,
                    0x314 /* ID_AK_JN_DROWNINGSPLASH00 */, &current.pos,
                    ang.get(), nullptr, 0xFF, nullptr, -1, amb.get(), nullptr,
                    nullptr);
  } else if (lkT60_pass(&frameCtrl, 50.0f)) {
    mProcVar3 = 0;
  } else if (lkT60_pass(&frameCtrl, 110.0f)) {
    mProcVar3 = 1;
  }
  return TRUE;
}

namespace link05 {
// HD: demo manager pointers and stage-specific correction state are separate
// globals.
using Link05DemoObjectPointer = gptr<dDemo_object_c>;
static Link05DemoObjectPointer l_demoObject;
static u32 l_demoFrame;
static u32 l_demoOffsetInitA;
static u32 l_demoOffsetInitB;
static cXyz l_demoOffsetA;
static cXyz l_demoOffsetB;
static Link05DemoFrameTable l_demoAnimationFrames;
using Link05DemoResourcePointer = gptr<void>;
static Link05DemoResourcePointer l_demoResourceControl;
} // namespace link05
/* HD dProcTool: strcmp(dComIfGp_getStartStageName() (0x1047E6B8), name) on
 * sead::SafeStrings; the first cstr() call is direct (this TU's empty
 * 02444F48), the next two virtual (slot 0x14). An exhausted compare (0x40001
 * equal bytes) takes the mismatch path everywhere. */
static inline bool lk05DemoStageIs(const char *nameStr) {
  gabi::Local<SafeString> name;
  name->mTextPtr = gabi::guest_str(nameStr);
  name->__vtbl = 0x10034B24;
  gabi::Local<SafeString> stage;
  stage->mTextPtr = 0x1047E6B8;
  stage->__vtbl = 0x10034B24;
  lk05DemoStringConstruct(name.get());
  lk05AssureTermination(name.get());
  u32 a = name->mTextPtr;
  lk05AssureTermination(stage.get());
  u32 b = stage->mTextPtr;
  if (a == b) {
    return true;
  }
  for (u32 i = 0; i < 0x40001; i++) {
    u8 ca = lk05GuestByte(a + i);
    u8 cb = lk05GuestByte(b + i);
    if (ca != cb) {
      return false;
    }
    if (ca == 0) {
      return true;
    }
  }
  return false;
}
/* g_dComIfG_gameInfo demo frame counter (0x101D600C, probably the demo's
 * current frame) */
static inline u32 lk05DemoFrame() { return link05::l_demoFrame; }
/* 0241FD7C */
BOOL daPy_lk_c::dProcTool() {
  u8 demoId = demoActorID; /* demoActorID */
  if (demoId == 0 || demoId > 0x20) {
    mProcVar7 = 0;
    mProcVar3 = 0;
    lk05CheckNextMode(this, 0);
    return TRUE;
  }
  dDemo_object_c *obj = link05::l_demoObject; /* dComIfGp_demo_get() */
  if (obj == 0) {
    JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
    obj = link05::l_demoObject;
  }
  dDemo_actor_c *demo_actor = lk05DemoGetActor(obj, (u32)demoId);
  mProcVar3 = 0;
  mProcVar7 = 0;
  if (demo_actor == 0) {
    lk05CheckNextMode(this, 0);
    return TRUE;
  }
  Link05AnmHeapHD *anm_heap_under = lk05DemoUnderHeap(
      this, 0); /* m_anm_heap_under[0]: +0 mIdx, +4 mDemoIdx, +6 mDemoAnmNo */
  u16 anmIdx = 0xFFFF;
  u16 r25 = (u16)((mDemo.getDemoMode() >> 9) -
                  1); /* getDemoMode() / DEMO_NEW_ANM0_e - 1 */
  f32 frame = 0.0f;
  u16 flags = demo_actor->mFlags;
  if (flags & 2 /* ENABLE_TRANS_e */) {
    /* HD: on the stage "Demo02" the translation is only taken while the demo
     * frame counter runs */
    if (lk05DemoFrame() != 0 || !lk05DemoStageIs("Demo02")) {
      current.pos.copy(demo_actor->mTrans);
    }
    flags = demo_actor->mFlags;
  }
  if (flags & 8 /* ENABLE_ROTATE_e */) {
    u16 rx = demo_actor->mRotate.x;
    shape_angle.x = rx;
    u16 ry = demo_actor->mRotate.y;
    shape_angle.y = ry;
    u16 rz = demo_actor->mRotate.z;
    shape_angle.z = rz;
    current.angle.x = rx;
    current.angle.y = ry;
    current.angle.z = rz;
    /* HD: on the stage "Demo24" two demo frame windows move Link by fixed
     * offsets */
    if (lk05DemoStageIs("Demo24")) {
      u32 f = lk05DemoFrame();
      if (f - 0xC1C < 0x37) {
        if (link05::l_demoOffsetInitA == 0) {
          link05::l_demoOffsetInitA = 1;
          link05::l_demoOffsetA.x = 15.0f;
          link05::l_demoOffsetA.y = -10.0f;
          link05::l_demoOffsetA.z = 30.0f;
        }
        lk05DemoOffsetPosition(&current.pos, &current.pos, (s32)current.angle.y,
                               &link05::l_demoOffsetA);
      } else if (f - 0xF51 < 0x8A) {
        if (link05::l_demoOffsetInitB == 0) {
          link05::l_demoOffsetInitB = 1;
          link05::l_demoOffsetB.x = 0.0f;
          link05::l_demoOffsetB.y = 0.0f;
          link05::l_demoOffsetB.z = 20.0f;
        }
        lk05DemoOffsetPosition(&current.pos, &current.pos, (s32)current.angle.y,
                               &link05::l_demoOffsetB);
      }
    }
    flags = demo_actor->mFlags;
  }
  if (flags & 0x40 /* ENABLE_ANM_FRAME_e */) {
    frame = demo_actor->mAnmFrame;
  }
  f32 morf = -1.0f;
  if (flags & 1 /* ENABLE_UNK_e: parameters */) {
    s32 id = demo_actor->mPrm.mId;
    u32 data = gabi::ea(demo_actor->mPrm.mData.get());
    if (id == 0 || id == 2 || id == 4) {
      gabi::Local<Link05DemoParseData> outA,
          outB; /* 0x14 bytes each (TData); was cXyz (12 bytes): the parser
                   0283CA7C writes 0x14 (game test: frame guard) */
      gabi::Local<be<u32>> inA;
      *inA = data;
      lk05DemoParseData(inA.get(), outA.get());
      u32 d = gabi::ea(outA->mpData.get());
      if (d == 0) {
        JUT_ASSERT_fail("stb.h", 0x24A, "!empty()");
        d = gabi::ea(outA->mpData.get());
      }
      u32 next = outA->mpNext;
      anmIdx = lk05DemoReadHalfword(d);
      gabi::Local<be<u32>> inB;
      *inB = next;
      lk05DemoParseData(inB.get(), outB.get());
      u32 size = outB->mSize;
      u32 p = gabi::ea(outB->mpData.get());
      if (p == 0) {
        JUT_ASSERT_fail(lk05DemoHandParseFile(), 0x24A,
                        lk05DemoHandParseCondition());
        size = outB->mSize;
        p = gabi::ea(outB->mpData.get());
      }
      if (size == 3) {
        mLeftHandIdx = lk05GuestByte(p);
        mRightHandIdx = lk05GuestByte(p + 1);
        morf = (f32)lk05GuestByte(p + size - 1);
      } else {
        mLeftHandIdx = lk05GuestByte(p);
        mRightHandIdx = lk05GuestByte(p + size - 1);
      }
      s32 id2 = demo_actor->mPrm.mId;
      if (id2 == 2) {
        mProcVar7 = 1;
      } else if (id2 == 4) {
        mProcVar3 = 1;
      }
    } else if (id == 1 || id == 3 || id == 5) {
      gabi::Local<Link05DemoParseData> outA, outB; /* TData, 0x14 bytes */
      gabi::Local<be<u32>> inA;
      *inA = data;
      lk05DemoParseData(inA.get(), outA.get());
      u32 d = gabi::ea(outA->mpData.get());
      if (d == 0) {
        JUT_ASSERT_fail("stb.h", 0x24A, "!empty()");
        d = gabi::ea(outA->mpData.get());
      }
      u32 count = outA->mSize;
      u16 btp = lk05DemoReadHalfword(d + 2);
      anmIdx = lk05DemoReadHalfword(d);
      u16 btk = lk05DemoReadHalfword(d + count * 2 - 2);
      /* HD: on the stage "Demo32" (from demo frame 0x147) the texture pair
       * 0x5C/0x35 is replaced */
      if (btp == 0x5C && btk == 0x35 && lk05DemoFrame() > 0x146 &&
          lk05DemoStageIs("Demo32")) {
        btk = 0x38;
        btp = 0x5F;
      }
      setDemoTextureAnime(btp, btk, 0, r25);
      u32 next = outA->mpNext;
      gabi::Local<be<u32>> inB;
      *inB = next;
      lk05DemoParseData(inB.get(), outB.get());
      u32 size = outB->mSize;
      u32 p = gabi::ea(outB->mpData.get());
      if (p == 0) {
        JUT_ASSERT_fail(lk05DemoHandParseFile(), 0x24A,
                        lk05DemoHandParseCondition());
        size = outB->mSize;
        p = gabi::ea(outB->mpData.get());
      }
      if (size == 3) {
        mLeftHandIdx = lk05GuestByte(p);
        mRightHandIdx = lk05GuestByte(p + 1);
        morf = (f32)lk05GuestByte(p + size - 1);
      } else {
        mLeftHandIdx = lk05GuestByte(p);
        mRightHandIdx = lk05GuestByte(p + size - 1);
      }
      s32 id2 = demo_actor->mPrm.mId;
      if (id2 == 3) {
        /* HD: not on the stage "Demo24" */
        if (!lk05DemoStageIs("Demo24")) {
          mProcVar7 = 1;
        } else if (demo_actor->mPrm.mId == 5) {
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
      if (equip != link05demo::daPyItem_SWORD_e ||
          dComIfGs_getSelectEquip(0) !=
              itemNo /* dComIfGs_getSelectEquip(0) */) {
        lk05DemoSelectEquip(0, itemNo);
        lk05DeleteEquipItem(this, 0);
        lk05DemoSetSwordModel(this, 1);
      }
    } else if (left == 0xCC) {
      if (mEquipItem != link05demo::dItemNo_WIND_WAKER_e) {
        lk05DeleteEquipItem(this, 0);
      }
      mLeftHandIdx = 5;
      lk05DemoSetTactModel(this);
    } else if (mEquipItem != link05demo::daPyItem_NONE_e) {
      lk05DeleteEquipItem(this, 0);
    }
    u8 right = mRightHandIdx;
    if (right == 0xC8 || right == 0xC9) {
      u32 shield = mRightHandIdx == 0xC8 ? link05demo::dItemNo_SHIELD_e
                                         : link05demo::dItemNo_MIRROR_SHIELD_e;
      mProcVar2 = 1;
      lk05DemoSelectEquip(1, shield);
      mRightHandIdx = 8;
    } else {
      mProcVar2 = 0;
      if (right != 0) {
        mRightHandIdx = (u8)(right + 6);
      }
    }
  }
  /* HD: on the stage "Demo32" some demo frames force an animation */
  if (lk05DemoStageIs("Demo32")) {
    u32 f = lk05DemoFrame();
    if (!(f < 0x15E)) {
      if (anmIdx == 0x2E) {
        anmIdx = 0x1C;
        morf = 0.0f;
      } else if (f == 0x87A) {
        anmIdx = 0x2E;
        morf = 8.0f;
      } else {
        for (u32 i = 0; i < 8; i++) {
          if (f == link05::l_demoAnimationFrames.mFrames[i]) {
            anmIdx = 0x19B;
            morf = 8.0f;
            anm_heap_under->mDemoIdx = 0xFFFF;
          }
        }
      }
    }
  }
  if (anmIdx != 0xFFFF && (anm_heap_under->mDemoIdx != anmIdx ||
                           anm_heap_under->mDemoAnmNo != r25)) {
    /* dComIfGp_getLkDemoAnmArchive(): a sead::SafeString at play + 0x5A50 */
    Link05DemoPlay *play = lk05DemoPlay();
    lk05AssureTermination(&play->mLkDemoAnmArchive);
    gabi::Local<SafeString> key;
    key->mTextPtr = play->mLkDemoAnmArchive.mTextPtr;
    key->__vtbl = 0x10034B24;
    Link05DemoAnimation *anm = lk05DemoGetIDRes(
        link05::l_demoResourceControl.get(), key.get(), (u32)anmIdx);
    Link05DemoAnimationVtable *vt =
        gabi::at<Link05DemoAnimationVtable>(anm->mVtable);
    s32 attr = gabi::call_ptr<s32>(vt->mGetAttribute, anm);
    s32 fmax = gabi::call_ptr<s32>(vt->mGetFrameMax, anm);
    lk05DemoSetFrameCtrl(this, &mFrameCtrlUnder[0], attr, 0, fmax, 1.0f, frame);
    anm->mFrame = frame; /* setFrame */
    anm_heap_under->mDemoIdx = anmIdx;
    anm_heap_under->mDemoAnmNo = r25;
    anm_heap_under->mIdx = 0xFFFF;
    link05::animation(this)->mAnmRatioUnder[0].mpAnmTransform =
        reinterpret_cast<J3DAnmTransform *>(
            anm); /* mAnmRatioUnder[UNDER_MOVE0_e].setAnmTransform() */
    m34C3 = 0;
    reinterpret_cast<Link05DemoUpperRatio *>(this)
        ->mAnmRatioUpper[0]
        .mpAnmTransform = reinterpret_cast<J3DAnmTransform *>(
        anm); /* mAnmRatioUpper[UPPER_MOVE0_e].setAnmTransform() */
    if (anmIdx == 0x198 && lk05DemoStageIs("Demo23")) {
      lk05InitOldFrameMorf((u32)m_old_fdata, 8.0f, 0, 0x2A); /* HD */
    } else if (!(morf < 0.0f)) {
      lk05InitOldFrameMorf((u32)m_old_fdata, morf, 0, 0x2A);
    }
    s32 fmax2 = anm->getFrameMax();
    demo_actor->mAnmFrameMax = (f32)fmax2; /* demo_actor->setAnmFrameMax() */
    lk05DemoSetSeAnime(
        this,
        reinterpret_cast<Link05DemoAnimation *>(
            link05::animation(this)->mAnmRatioUnder[0].mpAnmTransform.get()),
        anm_heap_under, &mFrameCtrlUnder[0]);
    return TRUE;
  }
  if (!(demo_actor->mFlags & 0x40 /* ENABLE_ANM_FRAME_e */)) {
    return TRUE;
  }
  /* HD: the frame is not taken for three animations on their stages */
  u16 cur = anm_heap_under->mDemoIdx;
  if (cur == 0x12F) {
    if (lk05DemoStageIs("Demo44")) {
      return TRUE;
    }
    cur = anm_heap_under->mDemoIdx;
  }
  if (cur == 0x198) {
    if (lk05DemoStageIs("Demo23")) {
      return TRUE;
    }
    cur = anm_heap_under->mDemoIdx;
  }
  if (cur == 0x1C && lk05DemoStageIs("Demo32")) {
    return TRUE;
  }
  Link05DemoAnimation *anm = reinterpret_cast<Link05DemoAnimation *>(
      link05::animation(this)->mAnmRatioUnder[0].mpAnmTransform.get());
  Link05DemoAnimationVtable *vt =
      gabi::at<Link05DemoAnimationVtable>(anm->mVtable);
  anm->mFrame = frame;
  s32 fmax = gabi::call_ptr<s32>(vt->mGetFrameMax, anm);
  s32 attr = anm->getAttribute();
  if (attr == 2) { /* HD: a looping animation ends one frame earlier */
    fmax = fmax - 1;
  }
  demo_actor->mAnmFrameMax = (f32)fmax;
  mFrameCtrlUnder[0].mFrame = frame;
  return TRUE;
}
// clang-format off
/* WWHD glue
data l_curseType 101CEF16
str 101CEB48 in 0241A204
str 10035F5C in 0241A204
str 10035F58 in 0241A204
str 10035FC4 in 0241EA50
str 10035F88 in 0241B32C
str 10035F90 in 0241B578
str 10035FA4 in 0241CB30
data l_crawl_start_front_offset 101CEBC0
data l_crawl_top_offset 101CEBE4
data l_ship_offset 10034F84
data l_rope_hanging_offset 1046CCEC
data l_zero 101FFBA8
data l_matrixStack 1048D0CC
data g_Counter 101FF560
ext 023D6850 s32 lk05ParachuteJointCallback(J3DNode* joint, s32 timing) callback
ext 023F14E0 BOOL lk05CheckNextMode(daPy_lk_c* self, s32 mode)
ext 027E0174 s32 lk05GetBlendCount(u32 resource, s32 index)
ext 027F3F94 u32 lk05GetJointTree(u32 resource)
ext 02419C70 void lk05SetShapeAngleToAtnActor(daPy_lk_c* self)
ext 023F2048 void lk05EndDemoMode(daPy_lk_c* self)
ext 023E2DC4 void lk05SetShipRidePosUseItem(daPy_lk_c* self)
ext 023F54E8 BOOL lk05ProcLargeDamage_init(daPy_lk_c* self, s32 anm, s32 mode, s32 x, s32 z)
ext 020079B4 f64 lk05PadStickValue(s32 pad)
ext 025445B8 BOOL lk05EventStartCheckOld(void* manager, const char* name)
ext 023F6020 BOOL lk05ProcLargeDamageUp_init(daPy_lk_c* self, s32 anm, s32 mode, s32 x, s32 z)
ext 023E278C void lk05SetOldRootQuaternion(daPy_lk_c* self, s32 x, s32 y, s32 z)
ext 023E2FF4 BOOL lk05ProcWait_init(daPy_lk_c* self)
ext 02418B48 BOOL lk05ProcHangWallCatch_init(daPy_lk_c* self)
ext 02417D30 BOOL lk05ProcHangStart_init(daPy_lk_c* self)
ext 023DCF8C void lk05FreeGrabItem(daPy_lk_c* self)
ext 02418E00 BOOL lk05ChangeFrontWallTypeProc(daPy_lk_c* self)
ext 023F13A0 BOOL lk05CheckJumpFlower(daPy_lk_c* self)
ext 025E3F3C void lk05InitOldFrameMorf(u32 oldFrame, f32 morph, s32 first, s32 count)
ext 023DD768 void lk05SetTextureAnime(daPy_lk_c* self, s32 anm, s32 param)
ext 0201971C f64 lk05Atan2f_d(f32 y, f64 x)
ext 0201B31C void lk05VecNormalize(cXyz* out, cXyz* in)
ext 023DE638 void lk05ActorKeepSetData(daPy_actorKeep_l* keep, fopAc_ac_c* actor)
ext 023E048C Link05AnmDataHD* lk05GetAnmData(daPy_lk_c* self, s32 anm)
ext 023EB6C8 BOOL lk05CheckBossGomaStage(daPy_lk_c* self)
ext 023EE86C BOOL lk05ProcFrontRoll_init(daPy_lk_c* self, f32 morph)
ext 025CB374 BOOL lk05StartShock(dVibration_c* vibration, s32 power, s32 timer, cXyz* direction)
ext 023F51D0 BOOL lk05SetDamagePoint(daPy_lk_c* self, f32 points)
ext 023E287C void lk05SetShipRidePos(daPy_lk_c* self, s32 mode)
ext 023E6380 BOOL lk05ProcCutTurn_init(daPy_lk_c* self, s32 mode)
ext 024EECAC u32 lk05GetMtrlSndId(dBgS* bgs, cBgS_PolyInfo* poly)
ext 023E4E2C void lk05EndFlameDamageEmitter(daPy_lk_c* self)
ext 02008438 fopAc_ac_c* lk05GetActorPointer(dBgS* bgs, u32 bg)
ext 0255F458 void lk05SoundSet(cXyz* pos, s32 radius, fpc_ProcID id, s32 param)
ext 025E1A40 void lk05SeStart(u32 sound, cXyz* pos, u32 param, s32 reverb)
ext 020084C8 cM3dGPla* lk05GetTriPla(dBgS* bgs, u32 bg, u32 poly)
ext 02008860 BOOL lk05LineCross(dBgS* bgs, dBgS_LinChk* check)
ext 024F1AFC void lk05LineSet(dBgS_LinChk* check, cXyz* start, cXyz* end, fopAc_ac_c* actor)
ext 023F0FD0 BOOL lk05ProcMoveTurn_init(daPy_lk_c* self, s32 mode)
ext 020195B0 s16 lk05Atan2s_d(f32 y, f64 x)
ext 027200D0 u32 lk05GetCameraOption(u32 options)
ext 024F8044 u32 lk05GetCameraBody(void)
ext 02514E24 BOOL lk05CameraChangeModeOK(u32 camera, s32 mode)
ext 025E3570 void lk05SetCurrentHeap(JKRHeap* heap)
ext 024EDFCC BOOL lk05AttentionLockonTruth(u32 attention)
ext 025E1988 void lk05SeStartSystem(u32 sound)
ext 028F4384 f64 lk05Sqrtf(f64 value)
ext 028E8DD0 f64 lk05VecSquareMag(const cXyz* value)
ext 02008974 f64 lk05GroundCross(dBgS* bgs, dBgS_GndChk* check)
ext 0200ECD4 f64 lk05AddCalc(be<f32>* value, f32 target, f32 scale, f32 maxStep, f32 minStep)
ext 02419ED4 void lk05CheckNextActionBowFly(daPy_lk_c* self)
ext 02419F54 void lk05CheckNextActionBoomerangFly(daPy_lk_c* self)
ext 023DFDD8 BOOL lk05CommonProcInit(daPy_lk_c* self, s32 proc)
ext 023E0A04 BOOL lk05SetSingleMoveAnime(daPy_lk_c* self, s32 anm, f32 rate, f32 start, s32 end, f32 morph)
ext 023DBC24 BOOL lk05CheckHeavyStateOn(daPy_lk_c* self)
ext 023EAB40 BOOL lk05ItemTrigger(daPy_lk_c* self)
ext 023EAA80 s32 lk05GetReadyItem(daPy_lk_c* self)
ext 023ED5A4 BOOL lk05ProcJumpCut_init(daPy_lk_c* self, s32 mode)
ext 023DDEEC J3DAnmTransform* lk05GetItemAnimeResource(daPy_lk_c* self, u32 resource)
ext 023DDF64 JKRHeap* lk05SetItemHeap(daPy_lk_c* self)
ext 023DC7AC void lk05DeleteEquipItem(daPy_lk_c* self, s32 mode)
ext 023E9AF4 BOOL lk05CheckSetItemTrigger(daPy_lk_c* self, s32 item, s32 mode)
ext 023EFB68 BOOL lk05CheckItemChangeFromButton(daPy_lk_c* self)
ext 023F6564 BOOL lk05ProcFall_init(daPy_lk_c* self, s32 mode, f32 morph)
ext 023DF9C0 void lk05ResetFootEffect(daPy_lk_c* self)
ext 023EEA30 BOOL lk05ProcSubjectivity_init(daPy_lk_c* self, s32 mode)
ext 023EE510 BOOL lk05ProcCrouchDefense_init(daPy_lk_c* self)
ext 023EE328 BOOL lk05CheckGuardAccept(daPy_lk_c* self)
ext 02415FB0 BOOL lk05GetCrawlMoveVec(daPy_lk_c* self, cXyz* top, cXyz* front, cXyz* vec)
ext 02416160 BOOL lk05ProcCrawlStart_init(daPy_lk_c* self)
ext 023E4CD0 BOOL lk05ChangeSlideProc(daPy_lk_c* self)
ext 023EFF20 BOOL lk05CheckNextActionFromButton(daPy_lk_c* self)
ext 0241650C void lk05SetSpeedAndAngleNormal(daPy_lk_c* self, s32 angle)
ext 023E14A0 void lk05SetBlendMoveAnime(daPy_lk_c* self, f32 morph)
ext 023E4A58 cM3dGPla* lk05GetSlidePolygon(daPy_lk_c* self)
ext 023E0E50 s32 lk05SetMoveAnime(daPy_lk_c* self, f32 ratio, f32 rate0, f32 rate1, s32 anm0, s32 anm1, s32 mode, f32 morph)
ext 023DE7E8 BOOL lk05DemoSetActAnimeUpper(daPy_lk_c*, u32, s32, f32, f32, s32, f32)
ext 023DBBE8 BOOL lk05DemoCheckGrabHeavy(daPy_lk_c*)
ext 025D7DEC u32 lk05DemoCreateItem(cXyz*, s32, s32, s32, s32, csXyz*, cXyz*)
ext 025D7C98 fopAc_ac_c* lk05DemoGetItemPartner(daPy_lk_c*)
ext 021842C8 void lk05DemoShowItem(fopAc_ac_c*)
ext 0218432C void lk05DemoDeleteItem(fopAc_ac_c*)
ext 021842B8 void lk05DemoHideItem(fopAc_ac_c*)
ext 021841C8 s32 lk05DemoGetItemNo(fopAc_ac_c*)
ext 023DF0FC void lk05DemoSetBottleModel(daPy_lk_c*, u32)
ext 025F7DB0 u32 lk05DemoMessageSet(Link05DemoMessageManager*, u32, s32)
ext 025F795C s32 lk05DemoMessageStatus(Link05DemoMessageManager*)
ext 025F74D0 void lk05DemoSetMessageStatus(Link05DemoMessageManager*, s32)
ext 025B7568 void lk05DemoEmptyReserve(void*, u32)
ext 0253E860 void lk05DemoEndEventCamera(u32, u32)
ext 023D4BB8 J3DModelData* lk05DemoInitModel(daPy_lk_c*, gptr<J3DModel>*, s32, u32)
ext 025E8508 BOOL lk05DemoInitBck(mDoExt_bckAnm*, J3DModelData*, J3DAnmTransform*, s32, s32, f32, s32, s32, s32)
ext 025D7C6C fopAc_ac_c* lk05DemoTalkPartner(daPy_lk_c*)
ext 025DB58C void lk05DemoMessageFlagOn(s32)
ext 023DC38C u32 lk05DemoLoadTextureAnime(daPy_lk_c*, u32, s32)
ext 023DC110 void lk05DemoSetTextureAnimeResource(daPy_lk_c*, u32, s32)
ext 023DC4DC u32 lk05DemoLoadTextureScroll(daPy_lk_c*, u32, s32)
ext 023DC43C void lk05DemoSetTextureScroll(daPy_lk_c*, u32, s32)
ext 02019510 s16 lk05DemoRad2s(f32)
ext 023DFB18 void lk05DemoSetBgCheckParam(daPy_lk_c*)
ext 025D7CC4 fopAc_ac_c* lk05DemoEventPartner(daPy_lk_c*)
ext 023E26EC s32 lk05DemoCheckShipRideUseItem(daPy_lk_c*, s32)
ext 023ECEF0 void lk05DemoSetAnimeUnequip(daPy_lk_c*)
ext 023E2E18 void lk05DemoInitShipRideUseItem(daPy_lk_c*, s32, s32)
ext 0255F378 void lk05DemoItemGetColorOn()
ext 02540310 BOOL lk05DemoCompulsory(void*, void*, s32, u32)
ext 023FE62C void lk05DemoLavaInitSub(daPy_lk_c*)
ext 023FD4E4 void lk05DemoRestartRoom(daPy_lk_c*, s32, s32, f32, s32)
ext 023F5EAC void lk05DemoFreezeInitSub(daPy_lk_c*, s32)
ext 023F8D60 BOOL lk05DemoSwimUpInit(daPy_lk_c*, s32)
ext 023D457C void lk05DemoMakeEmitter(daPy_mtxFollowEcallBack_c*, u32, Mtx34*, cXyz*, void*)
ext 025E742C void lk05DemoPlayBaseAnm(mDoExt_bckAnm*)
ext 025E86B8 void lk05DemoEntryBck(mDoExt_bckAnm*, J3DModelData*, f32)
ext 02400BC4 fopAc_ac_c* lk05DemoLookActor(daPy_lk_c*)
ext 0200F93C s16 lk05DemoTargetAngleY(cXyz*, cXyz*)
ext 023EEB10 void lk05DemoKeepItemData(daPy_lk_c*)
ext 023F49A4 s32 lk05DemoTalkStartBack(daPy_lk_c*)
ext 023DBCA0 void lk05DemoGetItemSound(daPy_lk_c*, u32, s32)
ext 023F7434 void lk05DemoDeadInitSub(daPy_lk_c*)
ext 023F74B0 void lk05DemoDeadInitSub2(daPy_lk_c*)
ext 025DE50C Link05DemoGameOver* lk05DemoFindGameOver(s32)
ext 02549C00 void lk05DemoGameOverAlpha(Link05DemoGameOver*, f32)
ext 02549BEC BOOL lk05DemoGameOverDelete(Link05DemoGameOver*)
ext 025217F8 void lk05DemoGameStartStage()
ext 02522198 void lk05DemoGameStart()
ext 0201A478 void lk05DemoConstructAngle(csXyz*, s32, s32, s32)
ext 025602F0 void lk05DemoSeaColor(GXColor*, GXColor*)
ext 02444F48 void lk05DemoStringConstruct(SafeString*)
ext 02526E70 dDemo_actor_c* lk05DemoGetActor(dDemo_object_c*, u32)
ext 0200FA40 void lk05DemoOffsetPosition(cXyz*, cXyz*, s32, cXyz*)
ext 0283CA7C void lk05DemoParseData(be<u32>*, Link05DemoParseData*)
ext 02522398 void lk05DemoSelectEquip(s32, u32)
ext 023DE0B4 void lk05DemoSetSwordModel(daPy_lk_c*, s32)
ext 023DEF04 void lk05DemoSetTactModel(daPy_lk_c*)
ext 026067F4 Link05DemoAnimation* lk05DemoGetIDRes(void*, SafeString*, u32)
ext 023DE788 void lk05DemoSetFrameCtrl(daPy_lk_c*, J3DFrameCtrl*, s32, s32, s32, f32, f32)
ext 023E07E4 void lk05DemoSetSeAnime(daPy_lk_c*, Link05DemoAnimation*, Link05AnmHeapHD*, J3DFrameCtrl*)
data l_demoMessageManager 101F4B5C
data l_bootsEquipFrame 1047C04C
data l_demoObject 101D5FFC
data l_demoFrame 101D600C
data l_demoOffsetInitA 1046D204
data l_demoOffsetInitB 1046D208
data l_demoOffsetA 1046CDA8
data l_demoOffsetB 1046CDB4
data l_demoAnimationFrames 10034F54
data l_demoResourceControl 101F4F28
str 10035FD8 in 0241F9DC
str 10035FD4 in 0241F9DC
str 10035FF4 in 0241FA98
str 10035FF0 in 0241FA98
str 10035038 in 0241FD7C
str 10034F74 in 0241FD7C
str 10034B14 in 0241FD7C
str 10035048 in 0241FD7C
str 1003600C in 0241FD7C
str 10036014 in 0241FD7C
str 1003601C in 0241FD7C
str 10036024 in 0241FD7C
str 1003602C in 0241FD7C
*/
// clang-format on
