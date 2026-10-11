/**
 * d_a_player_main_03.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #03 (023EDF70..02400B13): proc inits (hammer, guard,
 * rolls, food, bottle, ship, damage, dead, swim), item/button action checks, demo data, stick data,
 * damage dispatch, position/ground/foot handling.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Other Link ranges use the shared class declarations and typed external bindings.
 * HD folds the GameCube HIO values (m_HIO->...) into constants; they are written as literals.
 */
#include "d/actor/d_a_player_main_03.h"

static LinkDemoEntry mDemoProcInitFuncTable[] = {
    {0, 0, 0x00000000},  {0, 0, 0x00000000},  {0, 0, 0x00000000},  {0, 0, 0x00000000},
    {0, 0, 0x00000000},  {0, -1, 0x023F0F00}, {0, 0, 0x00000000},  {0, -1, 0x02421148},
    {0, 0, 0x00000000},  {0, 0, 0x00000000},  {0, -1, 0x02421434}, {0, -1, 0x024216C8},
    {0, -1, 0x02421D84}, {0, -1, 0x0242128C}, {0, 0, 0x00000000},  {0, -1, 0x024224B8},
    {0, 0, 0x00000000},  {0, 0, 0x00000000},  {0, 0, 0x00000000},  {0, -1, 0x024225B4},
    {0, -1, 0x024226B4}, {0, -1, 0x024227EC}, {0, -1, 0x02422950}, {0, 0, 0x00000000},
    {0, -1, 0x02422D9C}, {0, -1, 0x02422FF8}, {0, -1, 0x02423180}, {0, 0, 0x00000000},
    {0, -1, 0x02423280}, {0, -1, 0x0242337C}, {0, -1, 0x02423468}, {0, 0, 0x00000000},
    {0, -1, 0x02423680}, {0, -1, 0x0242377C}, {0, 0, 0x00000000},  {0, -1, 0x02423890},
    {0, -1, 0x02423AEC}, {0, -1, 0x02423C84}, {0, -1, 0x02423F08}, {0, -1, 0x023F1E60},
    {0, -1, 0x02424070}, {0, 0, 0x00000000},  {0, 0, 0x00000000},  {0, 0, 0x00000000},
    {0, 0, 0x00000000},  {0, -1, 0x0243B50C}, {0, -1, 0x02425C54}, {0, -1, 0x02424348},
    {0, -1, 0x024248C0}, {0, -1, 0x02424434}, {0, -1, 0x02424724}, {0, 0, 0x00000000},
    {0, -1, 0x02424AA0}, {0, -1, 0x02424B9C}, {0, -1, 0x02424D14}, {0, -1, 0x02424E50},
    {0, -1, 0x023EA054}, {0, -1, 0x02425048}, {0, -1, 0x0242510C}, {0, -1, 0x024251D0},
    {0, -1, 0x02425548}, {0, 0, 0x00000000},  {0, -1, 0x02425788}, {0, -1, 0x024258A0},
    {0, 0, 0x00000000},  {0, -1, 0x023EEB70}, {0, -1, 0x024259D8}, {0, -1, 0x02425B80},
    {0, -1, 0x024311FC}, {0, -1, 0x02425E98}, {0, -1, 0x02426034}, {0, -1, 0x02430EC0},
    {0, -1, 0x02426298}, {0, -1, 0x023EED74}, {0, -1, 0x02426360},
};

static const u16 eff_joint[] = {7, 11, 26, 14, 30};
static u32 l_fixedDemoPosInitialized;
static cXyz l_fixedDemoPos;

// Local HD helpers for action initialization and demos.
/* play + 0x5CD8 / 0x5CDC: dComIfGp_setPlayerStatus0/1(0, flag) */

/* daPy_matAnm_c::offMabaFlg(): the static blink flag bytes */
static inline void linkDisableBlink() {
    gabi::store<u8>(0x101CEF19, 1);
    gabi::store<u8>(0x101CEF18, 0);
}
/* dAttention_c (play + 0x5804, cached at this + 0x6894): Lockon() = LockonTruth() || flag 0x20000000 at +0x20 */
static inline BOOL linkAttentionLockonTruth(u32 att) { return gabi::call<BOOL>(0x024EDFCC, att); }

/* ---- daPy_lk_c members in the opaque blocks of the shared header ---- */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define mpEquipItemModel_ea                                                                        \
    (gabi::ea(this) + 0x4440) /* J3DModel* mpEquipItemModel (GameCube 0x2FA8 area) */
#define mSwordAnim_ea (gabi::ea(this) + 0x4444) /* mDoExt_bckAnm mSwordAnim */

/* ---- daPy_lk_c functions of other ranges (by address) ---- */
enum : u32 {
    LK_commonProcInit = 0x023DFDD8,
    LK_setSingleMoveAnime = 0x023E0A04,
    LK_setBlendMoveAnime = 0x023E14A0,
    LK_setBlurPosResource = 0x023E55A4,
    LK_setAtParam = 0x023E5600,
    LK_getItemAnimeResource = 0x023DDEEC, /* unnamed by the matcher */
    LK_deleteEquipItem = 0x023DC7AC,
    LK_checkHeavyStateOn = 0x023DBC24,
    LK_setItemHeap = 0x023DDF64,
    LK_initModel = 0x023D4BB8,
    LK_checkBowAnime = 0x023D6A18,
    LK_checkShipRideUseItem = 0x023E26EC,
    LK_initShipRideUseItem = 0x023E2E18,
    LK_setBottleModel = 0x023DF0FC,
    LK_setTextureAnime = 0x023DD768,
    LK_getReadyItem = 0x023EAA80,
    LK_setAnimeUnequipItem = 0x023ECD98,
    LK_changeDragonShield = 0x023E9D50,
    LK_procBootsEquip_init = 0x023E9DD0,
    LK_checkDrinkBottleItem = 0x023E9A6C, /* unnamed by the matcher */
    LK_checkOpenBottleItem = 0x023E9A84,  /* unnamed by the matcher */
    LK_procBottleSwing_init = 0x023EDD9C,
    LK_checkSetItemTrigger = 0x023E9AF4,
    LK_setAnimeEquipSword = 0x023E9F60,
    LK_setAnimeUnequip = 0x023ECEF0,
    LK_resetActAnimeUpper = 0x023DC6A4,
    LK_changeCutProc = 0x023E73FC,
    LK_procScope_init = 0x023E2358,
    LK_procFanSwing_init = 0x023E79DC,
    LK_checkNextBowMode = 0x023E8984,
    LK_checkNextHookshotMode = 0x023E8BD4,
    LK_checkNextBoomerangMode = 0x023E8F2C,
    LK_checkNextRopeMode = 0x023E91E4,
    LK_procTactWait_init = 0x023E92A0,
    LK_checkNextActionGrab = 0x023EA1AC,
    LK_checkNextActionHookshotReady = 0x023EAD5C,
    LK_checkNextActionBowReady = 0x023EB208,
    LK_checkNextActionBoomerangReady = 0x023EB510,
    LK_checkRopeAnime = 0x023D6A5C,
    LK_checkNextActionRopeReady = 0x023EC084,
    LK_setDoStatusBasic = 0x023EA680,
    LK_setDoStatus = 0x023EC498,
    LK_orderTalk = 0x023EC724,
    LK_procShipReady_init = 0x023ECA94,
    LK_procPushPullWait_init = 0x023ECF74,
    LK_changeWaitProc = 0x023E32AC,
    LK_procGrabReady_init = 0x023ED0F0,
    LK_procJumpCut_init = 0x023ED5A4,
    LK_procWHideReady_init = 0x023ED788,
    LK_procCutTurnCharge_init = 0x023ED970,
    LK_procCutTurn_init = 0x023E6380,
    LK_procWeaponNormalSwing_init = 0x023ED9E0,
    LK_procWeaponSideSwing_init = 0x023EDB6C,
    LK_procWeaponFrontSwingReady_init = 0x023EDC14,
    LK_procWeaponThrow_init = 0x023EDD14,
    LK_itemTrigger = 0x023EAB40,
    LK_changeSpecialBattle = 0x023E5400,
    LK_checkUpperReadyThrowAnime = 0x023D6ABC,
    LK_setBlendAtnMoveAnime = 0x023E81E4,
    LK_setFrontWallType = 0x023E3850,
    LK_procVomitJump_init = 0x023E47D4,
    LK_changeSlideProc = 0x023E4CD0,
    LK_checkBoomerangAnime = 0x023D69F8,
    LK_setActAnimeUpper = 0x023DE7E8,
    LK_setShipRidePos = 0x023E287C,
    LK_checkShipNotNormalMode = 0x023D69CC,
    LK_setOldRootQuaternion = 0x023E278C,
    LK_resetDemoTextureAnime = 0x023DDD48,
    LK_procControllWait_init = 0x023E2EFC,
    LK_procWait_init = 0x023E2FF4,
};
/* mDoExt_blkAnm-style ratio set (inline in HD): the weight of animation `idx` for every joint of the
 * model's joint tree; 027F3F94 (matcher: __nw) resolves a self-relative offset, 027E0174 counts the joints */
static inline void linkSetJointBlendRatio(u32 pb, u32 idx, f32 value) {
    LinkActionBlendCalc* calc = gabi::at<LinkActionBlendCalc>(pb);
    u32 resource = LinkActionApi::resolveJointTree(gabi::ea(calc->mpModel.get()->mModelData.get()));
    s32 jointCount = LinkActionApi::countJoints(resource, 0);
    for (u32 i = 0; i < (u32)jointCount && jointCount > 0; i++) {
        // The blend-data pointer is mutable across every store in the HD loop.
        calc->mpBlendData.get()->mRatios[idx].mpJointWeights.get()[i] = value;
    }
}
static inline void linkAddPosition(u32 a, cXyz* res, u32 b) { gabi::call(0x0201AD78, a, res, b); }
static inline void linkCopyPositionWords(u32 dst, cXyz* src) {
    LinkActionVectorWords* out = gabi::at<LinkActionVectorWords>(dst);
    LinkActionVectorWords* in = gabi::at<LinkActionVectorWords>(gabi::ea(src));
    u32 x = in->x;
    out->x = x;
    u32 y = in->y;
    out->y = y;
    u32 z = in->z;
    out->z = z;
}
/* dComIfGp_getShipActor(): play + 0x5B3C; daShip_c mode byte at +0x636 (setCannon/setCrane/... store it) */

/* mCyl's co hit actor (dCcD_GAtTgCoCommonBase::GetAc of the cylinder's co object, this + 0x7738) and ChkCoHit */
#define LK_callB(addr) gabi::call<BOOL>(addr, this)
#define LK_upperIdx()                                                                              \
    gabi::load<u16>(gabi::ea(this) + 0x5888) /* m_anm_heap_upper[UPPER_MOVE2_e].mIdx */
/* abs() of an s32 as GHS computes it (abs(INT_MIN) stays negative) */
static inline s32 lk_abs(s32 v) {
    u32 m = (u32)(v >> 31);
    return (s32)(((u32)v ^ m) - m);
}
/* dComIfGp_getRStatus/setRStatus: play + 0x5BB5; play + 0x5BB6 (HD: checked by the Boko throw) */

/* camera attention status of a camera info index: play + 0x5B00 + idx * 0x34 */

/* checkAttentionLock(): dAttention_c::Lockon() of mpAttention */
static inline BOOL lk_checkAttentionLock(u32 att) {
    return linkAttentionLockonTruth(att) || (gabi::load<u32>(att + 0x20) & 0x20000000);
}
/* dComIfGp_getSelectItem(btn): play + 0x5BBB + btn */

/* dComIfGp_getDoStatus(): play + 0x5BB7 */

/* daPy_lk_c::checkPlayerFly() (inline): the midair/hang/... mode flags */
#define LK_checkPlayerFly() (mModeFlg & 0x10452822)
/* virtual daPy_lk_c::checkPlayerGuard() (vtable slot 0x3C) */
/* daPy_dmEcallBack_c::checkCurse(): static u16 at 0x101CEF16 */
#define LK_checkCurse() (gabi::load<u16>(0x101CEF16) == 1)
#define mHDThrowActor                                                                              \
    LK_FIELD(u32, 0x8268) /* HD-only (tail): the thrown actor followed by the camera */
#define mHDThrowAngle LK_FIELD(s16, 0x826C)   /* HD-only (tail) */
#define mHDThrowPos (gabi::ea(this) + 0x8270) /* HD-only (tail): cXyz */
#define mHD8265 LK_FIELD(u8, 0x8265)          /* HD-only (tail) */
#define mHD8260                                                                                    \
    LK_FIELD(                                                                                      \
        u32,                                                                                       \
        0x8260) /* HD-only (tail): blocks the item buttons when set; 0x8264 a byte set by the Tingle event order */
/* dComIfGp_event_compulsory(this): dEvt_control_c::compulsory(play + 0x51D0, actor, 0, 0xFFFF) */
static inline BOOL linkOrderCompulsoryEvent(void* ac) {
    return gabi::call<BOOL>(0x02540310, dComIfGp_ea() + 0x51D0, ac, 0, 0xFFFF);
}

/* dCam_getBody()->StartEventCamera(0x12, fopAcM_GetID(this), "Type" (a per-use literal), &param, 0) (varargs) */
static inline void linkStartEventCamera(fopAc_ac_c* ac, u32 typeStr, void* param) {
    u32 cam = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
    gabi::call(0x0253E70C, cam, 0x12, gabi::load<u32>(gabi::ea(ac) + 4), typeStr, param, 0);
}
/* dComIfGp_getVibration().StartShock(6, -0x21, cXyz(0, 1, 0)) (play + 0x599C) */
/* inlined into daPy_lk_c::procLargeDamage_init */
static inline void linkStartShockUp(int strength = 6) {
    u32 vib = dComIfGp_ea() + 0x599C;
    cXyz dir;
    dir.x = 0.0f;
    dir.y = 1.0f;
    dir.z = 0.0f;
    gabi::call(0x025CB374 /* dVibration_c::StartShock */, vib, strength, -0x21, &dir);
}
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline u32 linkGetAnmMtx(u32 model, s32 joint) {
    return gabi::ea(gabi::at<J3DModel>(model)->getAnmMtx(joint));
}
/* dComIfGs: the save data object *(0x101F84DC) */

/* mDemo (daPy_demo_c at 0x41C): demo type u16 at +4 (0x420), demo mode at +0x14 (0x430) */
#define LK_demoType LK_FIELD(u16, 0x420)
#define LK_demoMode LK_FIELD(s32, 0x430)
/* mDoExt_bckAnm::changeBckOnly (HD 025E871C) */
static inline void linkChangeEquipmentAnimation(u32 anm, u32 bck) {
    gabi::call(0x025E871C, anm, bck);
}
/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */
/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void linkCopyFloatBits(u32 dst, u32 src) {
    *gabi::at<be<u32>>(dst) = (u32)*gabi::at<be<u32>>(src);
}
/* fsel: a >= 0 ? b : c (NaN -> c) */
static inline f32 linkFloatSelect(f32 a, f32 b, f32 c) { return a >= 0.0f ? b : c; }

/* 023EDF70 */
BOOL daPy_lk_c::procHammerSideSwing_init() {

    commonProcInit(LinkActionConstants::daPyProc_HAMMER_SIDE_SWING_e);
    setSingleMoveAnime(LinkActionConstants::ANM_HAMSWINGA, 1.2f, 4.0f, 0x3C, 2.0f);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    LinkActionVoiceStart(this, 1);
    mProcVar2 = shape_angle.y;
    setBlurPosResource(LinkActionConstants::dRes_INDEX_LKANM__HAMMERSIDE_POS_e);
    setAtParam(0x10000 /* AT_TYPE_SKULL_HAMMER */, 4, 0, 5, 0xD, 0x11, 50.0f);
    u32 bck = getItemAnimeResource(LinkActionConstants::dRes_INDEX_LKANM_BCK_HAMSWINGAA_e);
    linkChangeEquipmentAnimation(gabi::ea(&LinkAction(this).mSwordAnim), bck);
    m35EC = 4.0f;
    return TRUE;
}

/* 023EE068 */
BOOL daPy_lk_c::procHammerFrontSwingReady_init() {

    commonProcInit(LinkActionConstants::daPyProc_HAMMER_FRONT_SWING_READY_e);
    setSingleMoveAnime(LinkActionConstants::ANM_HAMSWINGBPRE, 1.0f, 2.0f, 0x1C, 3.0f);
    u32 att = mpAttention;
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    /* checkAttentionLock() || mStickDistance <= 0.05f */
    if (linkAttentionLockonTruth(att) ||
        (gabi::at<LinkActionAttention>(att)->mFlags & 0x20000000) || !(mStickDistance > 0.05f)) {
        mProcVar2 = shape_angle.y;
    } else {
        mProcVar2 = m34E8;
    }
    setAtParam(0x10000, 4, 0, 5, 0xD, 0x12 /* CUT_TYPE_HAMMER_FRONTSWING */, 50.0f);
    LinkActionVoiceStart(this, 7);
    u32 bck = getItemAnimeResource(LinkActionConstants::dRes_INDEX_LKANM_BCK_HAMSWINGBPREA_e);
    linkChangeEquipmentAnimation(gabi::ea(&LinkAction(this).mSwordAnim), bck);
    m35EC = 2.0f;
    m355C = 0;
    return TRUE;
}

/* 023EE208 */
BOOL daPy_lk_c::procCall_init() {

    commonProcInit(LinkActionConstants::daPyProc_CALL_e);
    mNormalSpeed = 0.0f;
    linkDisableBlink();
    setSingleMoveAnime(LinkActionConstants::ANM_YOBU, 1.0f, 0.0f, -1, 2.4f);
    current.angle.y = shape_angle.y;
    if (mEquipItem == LinkActionConstants::daPyItem_BOKO_e) {
        deleteEquipItem(0);
    }
    LinkActionVoiceStart(this, 0x2A);
    return TRUE;
}

/* 023EE2B8 */
BOOL daPy_lk_c::procCrouch_init() {

    commonProcInit(LinkActionConstants::daPyProc_CROUCH_e);
    setSingleMoveAnime(LinkActionConstants::ANM_CROUCH, 0.8f, 0.0f, -1, 5.0f);
    deleteEquipItem(0);
    current.angle.y = shape_angle.y;
    return TRUE;
}

#define LK_SAFESTRING_VTBL 0x10034B24u /* sead::SafeString vtable of this TU */

static inline void linkAssureTermination(LinkActionSafeString* s) {
    gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s);
}
/* inline sead::SafeString::operator==(a, b) */
static inline bool linkSafeStringEquals(LinkActionSafeString* a, LinkActionSafeString* b) {
    linkAssureTermination(a);
    linkAssureTermination(a);
    u32 pa = a->mStr;
    linkAssureTermination(b);
    u32 pb = b->mStr;
    if (pa == pb)
        return true;
    pa = a->mStr;
    pb = b->mStr;
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
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (the start stage name at play + 0x5134) */
/* inlined into daPy_lk_c::checkGuardAccept */
static inline bool linkIsStartStage(u32 lit) {
    LinkActionSafeString a;
    a.__vtbl = LK_SAFESTRING_VTBL;
    a.mStr = lit;
    u32 play = dComIfGp_ea();
    LinkActionSafeString b;
    b.__vtbl = LK_SAFESTRING_VTBL;
    b.mStr = play + 0x5134;
    return linkSafeStringEquals(&a, &b);
}
/* dComIfGp_getStageStagInfo(): play + 0x5150 is the stage object, virtual getStagInfo at slot 0x15C */

/* 023EE328 */
BOOL daPy_lk_c::checkGuardAccept() {

    if ((mModeFlg & 0x40) && !checkBowAnime()) {
        u32 info = gabi::ea(dComIfGp_getStageStagInfo());
        /* dStage_stagInfo_GetSTType */
        if (((gabi::at<LinkActionStageInfo>(info)->mStageType >> 16) & 7) !=
                2 /* dStageType_MISC_e */
            || linkIsStartStage(gabi::guest_addr("Ojhous")) ||
            linkIsStartStage(gabi::guest_addr("Orichh"))) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 023EE510 */
BOOL daPy_lk_c::procCrouchDefense_init() {

    commonProcInit(LinkActionConstants::daPyProc_CROUCH_DEFENSE_e);
    linkDisableBlink();
    setSingleMoveAnime(LinkActionConstants::ANM_DIFENCE, 0.8f, 0.0f, -1, 2.0f);
    current.angle.y = shape_angle.y;
    mProcVar2 = 0;
    dComIfGp_setPlayerStatus1(0, LinkActionConstants::daPyStts1_UNK80000_e);
    return TRUE;
}

/* 023EE5B0 */
BOOL daPy_lk_c::procSideStep_init(int dir) {

    commonProcInit(LinkActionConstants::daPyProc_SIDE_STEP_e);
    mDirection = dir;
    int anm;
    if (dir == 2 /* DIR_LEFT */) {
        anm = 0x10; /* ANM_ATNJL */
        current.angle.y = (s16)(shape_angle.y + 0x4000);
    } else {
        anm = 0x11; /* ANM_ATNJR */
        current.angle.y = (s16)(shape_angle.y - 0x4000);
    }
    setSingleMoveAnime(anm, 0.3f, 0.0f, 4, 1.0f);
    mNormalSpeed = 30.0f * cM_scos(0x1838);
    gravity = -2.4f;
    mProcVar6 = 0;
    speed.y = 30.0f * cM_ssin(0x1838);
    LinkActionVoiceStart(this, 5);
    return TRUE;
}

/* 023EE710 */
BOOL daPy_lk_c::procBackJump_init() {

    if (mCurProc == LinkActionConstants::daPyProc_BACK_JUMP_e) {
        return FALSE;
    }
    commonProcInit(0x22);
    if (checkHeavyStateOn()) {
        mNormalSpeed = 11.25f;
        setSingleMoveAnime(LinkActionConstants::ANM_ROLLB, 1.2f, 2.0f, 0xB, 0.0f);
    } else {
        mNormalSpeed = 22.5f;
        setSingleMoveAnime(0x3D, 0.8f, 2.0f, 0xB, 0.0f);
    }
    speed.y = 19.0f;
    gravity = -3.0f;
    current.angle.y = (s16)(shape_angle.y + 0x8000);
    LinkActionVoiceStart(this, 7);
    return TRUE;
}

/* 023EE86C */
BOOL daPy_lk_c::procFrontRoll_init(f32 morf) {

    commonProcInit(LinkActionConstants::daPyProc_FRONT_ROLL_e);
    setSingleMoveAnime(LinkActionConstants::ANM_ROLLF, 1.1f, morf, 0x13, 2.0f);
    mNormalSpeed = gabi::fmadds(speedF, 1.53f, 20.0f);
    if (checkHeavyStateOn()) {
        mNormalSpeed = mNormalSpeed * 0.5f;
    }
    if (mNormalSpeed < 5.0f) {
        mNormalSpeed = 5.0f;
    } else {
        f32 max = 26.01f;
        if (checkHeavyStateOn()) {
            max = 13.005f;
        }
        if (mNormalSpeed > max) {
            mNormalSpeed = max;
        }
    }
    current.angle.y = shape_angle.y;
    if ((mAcchCir[0].m_flags & 2 /* WALL_HIT */) &&
        cLib_distanceAngleS((s16)(current.angle.y + 0x8000),
                            mAcchCir[0].m_wall_angle_y /* mAcchCir[0] wall angle */) <= 0x1388) {
        mProcVar6 = 0;
    } else {
        mProcVar6 = 1;
    }
    mNoResetFlg0 = mNoResetFlg0 & ~8u;
    mProcVar2 = 0;
    LinkActionVoiceStart(this, 7);
    return TRUE;
}

/* 023EEA30 */
BOOL daPy_lk_c::procSubjectivity_init(int crouch) {

    commonProcInit(LinkActionConstants::daPyProc_SUBJECTIVITY_e);
    mNormalSpeed = 0.0f;
    setSubjectMode();
    if (!crouch) {
        setBlendMoveAnime(2.4f);
    }
    mProcVar6 = crouch;
    return TRUE;
}

/* 023EEAA0 */
void daPy_lk_c::setHyoiModel() {

    if (mEquipItem == LinkActionConstants::dItemNo_HYOI_PEAR_e) {
        return;
    }
    u32 oldHeap = gabi::ea(setItemHeap());
    initModel(gabi::ea(&LinkAction(this).mpEquipItemModel),
              LinkActionConstants::dRes_INDEX_LINK_BDL_HYOINOMI_e, 0x13000022);
    LinkActionApi::setCurrentHeap(oldHeap);
    mEquipItem = 0x83;
}

/* 023EEB10 */
void daPy_lk_c::keepItemData() {

    mKeepItem = mEquipItem;
    deleteEquipItem(0);
    if (mKeepItem == LinkActionConstants::daPyItem_BOKO_e) {
        mKeepItem = 0x100; /* daPyItem_NONE_e */
    } else if (mKeepItem == 0x100) {
        mKeepItem = 0x10B; /* daPyItem_UNK10B_e */
    }
}

/* 023EEB70 */
BOOL daPy_lk_c::procFoodSet_init() {

    if (mCurProc == LinkActionConstants::daPyProc_FOOD_SET_e) {
        if (mDemo.mParam0 == 1) {
            if (mEquipItem == LinkActionConstants::dItemNo_HYOI_PEAR_e) {
                if (dComIfGp_event_getTalkXYBtn() == 1 /* dTalkBtn_X_e */) {
                    LinkActionApi::setEquipBottleItemEmpty(dComIfGs_ea() + 0x5C, 0);
                } else {
                    int btn = dComIfGp_event_getTalkXYBtn() == 2 /* dTalkBtn_Y_e */ ? 1 : 2;
                    LinkActionApi::setEquipBottleItemEmpty(dComIfGs_ea() + 0x5C, btn);
                }
            }
            deleteEquipItem(0);
        } else {
            setHyoiModel();
        }
        return TRUE;
    }
    if (!dComIfGp_event_runCheck()) {
        if (!linkOrderCompulsoryEvent(this)) {
            return FALSE;
        }
        mDemo.mDemoType = 5; /* mDemo.setSpecialDemoType() */
    }
    int use = checkShipRideUseItem(1);
    commonProcInit(0xA8);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(LinkActionConstants::ANM_SETHYOINOMI, 1.1f, 2.0f, 0x15, 4.0f);
    keepItemData();
    setHyoiModel();
    if (mDemo.mDemoType == 5 /* checkSpecialDemoMode() */) {
        mProcVar6 = 1;
        linkStartEventCamera(this, gabi::guest_addr("Type"), &mProcVar6);
    }
    initShipRideUseItem(use, 2);
    mProcVar0 = 30;
    return TRUE;
}

/* 023EED74 */
BOOL daPy_lk_c::procFoodThrow_init() {

    if (mCurProc == LinkActionConstants::daPyProc_FOOD_THROW_e) {
        return TRUE;
    }
    if (m3630 != 0xFFFFFFFF /* fpcM_ERROR_PROCESS_ID_e */) {
        LinkActionApi::seStartSystem(LinkActionConstants::JA_SE_ITEM_TARGET_OUT);
        return FALSE;
    }
    if (!dComIfGp_event_runCheck()) {
        if (!linkOrderCompulsoryEvent(this)) {
            return FALSE;
        }
        mDemo.mDemoType = 5;
    } else {
        deleteEquipItem(0);
    }
    int use = checkShipRideUseItem(1);
    commonProcInit(0xA7);
    if (use == 0) {
        f32 offset1 = 150.0f * cM_ssin(shape_angle.y);
        f32 offset2 = 150.0f * cM_scos(shape_angle.y);
        cXyz* attn = &attention_info.position; /* attention_info.position */
        cXyz start;
        cXyz end;
        start.x = attn->x;
        start.y = attn->y - 50.0f;
        start.z = attn->z;
        end.x = start.x + offset1;
        end.y = start.y;
        end.z = start.z + offset2;
        void* chk = mLinkLinChk;
        dBgS_LinChk_Set(chk, &start, &end, this);
        if (cBgS_LineCross(dComIfG_Bgsp(), chk)) {
            end.x = start.x - offset1;
            end.z = start.z - offset2;
            dBgS_LinChk_Set(chk, &start, &end, this);
            if (!cBgS_LineCross(dComIfG_Bgsp(), chk)) {
                shape_angle.y = (s16)(shape_angle.y - 0x8000);
            } else {
                end.x = start.x + offset2;
                end.z = start.z - offset1;
                dBgS_LinChk_Set(chk, &start, &end, this);
                if (!cBgS_LineCross(dComIfG_Bgsp(), chk)) {
                    shape_angle.y = (s16)(shape_angle.y + 0x4000);
                } else {
                    end.x = start.x - offset2;
                    end.z = start.z + offset1;
                    dBgS_LinChk_Set(chk, &start, &end, this);
                    if (!cBgS_LineCross(dComIfG_Bgsp(), chk)) {
                        shape_angle.y = (s16)(shape_angle.y - 0x4000);
                    }
                }
            }
        }
    }
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    linkDisableBlink();
    setSingleMoveAnime(LinkActionConstants::ANM_ESAMAKI, 0.6f, 0.0f, 0x11, 4.0f);
    keepItemData();
    if (mDemo.mDemoType == 5) {
        mProcVar6 = 6;
        linkStartEventCamera(this, gabi::guest_addr("Type"), &mProcVar6);
    }
    initShipRideUseItem(use, 2);
    m3630 = 0xFFFFFFFF;
    return TRUE;
}

/* 023EF260 */
int daPy_lk_c::changeBottleDrinkFace(int item) {

    if (item == LinkActionConstants::dItemNo_SOUP_BOTTLE_e ||
        item == LinkActionConstants::dItemNo_HALF_SOUP_BOTTLE_e) {
        return TRUE;
    }
    return FALSE;
}

/* 023EF280 */
BOOL daPy_lk_c::procBottleDrink_init(u16 item) {

    if (!linkOrderCompulsoryEvent(this)) {
        return FALSE;
    }
    mDemo.mDemoType = 5;
    int use = checkShipRideUseItem(1);
    commonProcInit(LinkActionConstants::daPyProc_BOTTLE_DRINK_e);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(LinkActionConstants::ANM_BINDRINKPRE, 1.0f, 0.0f, 0x51, 5.0f);
    if (changeBottleDrinkFace(item)) {
        setTextureAnime(0x85, 0);
    }
    keepItemData();
    setBottleModel((u32)item);
    dComIfGp_setPlayerStatus1(0, LinkActionConstants::daPyStts1_UNK2000_e);
    mProcVar6 = 1;
    linkStartEventCamera(this, gabi::guest_addr("Type"), &mProcVar6);
    initShipRideUseItem(use, 2);
    return TRUE;
}

/* 023EF3E0 */
BOOL daPy_lk_c::procBottleOpen_init(u16 item) {

    if (mCurProc == LinkActionConstants::daPyProc_BOTTLE_OPEN_e) {
        return TRUE;
    }
    if (mDemo.mDemoMode != 0x3D /* daPy_demo_c::DEMO_BO_OPEN_e */) {
        if (!linkOrderCompulsoryEvent(this)) {
            return FALSE;
        }
        mDemo.mDemoType = 5;
    } else {
        item = dComIfGp_event_getPreItemNo();
    }
    int use = checkShipRideUseItem(1);
    commonProcInit(0xA4);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(LinkActionConstants::ANM_BINOPENPRE, 1.0f, 0.0f, 0x2D, 2.0f);
    keepItemData();
    setBottleModel((u32)item);
    dComIfGp_setPlayerStatus1(0, LinkActionConstants::daPyStts1_UNK4000_e);
    if (mEquipItem == LinkActionConstants::dItemNo_FAIRY_BOTTLE_e) {
        mProcVar6 = 3;
    } else {
        mProcVar6 = 2;
    }
    if (mDemo.mDemoMode != 0x3D) {
        linkStartEventCamera(this, gabi::guest_addr("Type"), &mProcVar6);
    }
    initShipRideUseItem(use, 2);
    mProcVar2 = 0;
    mProcVar3 = 0;
    mProcVar0 = -1;
    mProcVar4 = 0;
    return TRUE;
}

/* 023EF61C */
BOOL daPy_lk_c::procNotUse_init(int item) {

    if (!linkOrderCompulsoryEvent(this)) {
        return FALSE;
    }
    mDemo.mDemoType = 5;
    commonProcInit(LinkActionConstants::daPyProc_NOT_USE_e);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(LinkActionConstants::ANM_PRESENTATIONB, 1.0f, 0.0f, 0x3E, 5.0f);
    keepItemData();
    dComIfGp_setPlayerStatus1(0, LinkActionConstants::daPyStts1_UNK800_e);
    mGameOverId = 0xFFFFFFFF;
    mProcVar6 = item;
    mProcVar7 = 5;
    linkStartEventCamera(this, gabi::guest_addr("Type"), &mProcVar7);
    if (LinkActionApi::isDaizaItem((u32)(u8)mProcVar6)) {
        m3624 = 0xF0C;
    } else {
        m3624 = 0x835;
    }
    mProcVar2 = 0;
    return TRUE;
}

/* 023EF750 */
void daPy_lk_c::setAnimeEquipItem() {

    u32 thrown = gabi::ea(mActorKeepThrow.mActor.get());
    if (thrown != 0) {
        // HD: the boomerang's cancel flag is set before the ready item is checked
        gabi::at<LinkActionBoomerang>(thrown)->mCancel = 1; /* daBoomerang_c::onCancelFlg() */
        if (getReadyItem() == LinkActionConstants::dItemNo_BOOMERANG_e) {
            return;
        }
    }
    m3562 = (u8)getReadyItem();
    auto& status = LinkActionPlay().mPlayerStatus[0][0];
    status = (u32)status & ~0x400000u;
    setAnimeUnequipItem((u32)m3562);
}

/* 023EF7CC */
BOOL daPy_lk_c::checkNewItemChange(u8 btn) {

    u8 itemNo = dComIfGp_getSelectItem(btn);
    if (itemNo == LinkActionConstants::dItemNo_MAGIC_ARMOR_e) {
        return changeDragonShield(1);
    }
    // HD: item 0x77 is in the list too (0x77..0x78)
    if (itemNo == LinkActionConstants::dItemNo_TINGLE_TUNER_e ||
        itemNo == LinkActionConstants::dItemNo_IRON_BOOTS_e || itemNo == 0x2B ||
        (itemNo >= 0x77 && itemNo <= LinkActionConstants::dItemNo_SAIL_e) ||
        LinkActionApi::isEsa(itemNo) || LinkActionApi::isDaizaItem(itemNo) ||
        LinkActionApi::isEmono(itemNo) || checkBottleItem(itemNo) ||
        (u32)(itemNo - 0x98) < 7 /* dItemNo_FATHER_LETTER_e..dItemNo_FILL_UP_COUPON_e */) {
        if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !LK_checkPlayerFly()) {
            if (itemNo == 0x29 || itemNo == 0x2B) {
                procBootsEquip_init((u32)itemNo);
            } else if (LinkActionApi::isEsa(itemNo)) {
                if (itemNo == LinkActionConstants::dItemNo_HYOI_PEAR_e) {
                    return procFoodSet_init();
                } else {
                    return procFoodThrow_init();
                }
            } else if (itemNo == 0x21) {
                /* HD: the Tingle item orders a talk event with the HD object at this + 0x8264 (GameCube:
                 * fopAcM_orderTalkEvent with the AGB), only when no event runs */
                if (dComIfGp_event_runCheck()) {
                    return FALSE;
                }
                u32 evt = dComIfGp_ea() + 0x51D0;
                u32 hd = gabi::ea(this) + 0x8264;
                if (LinkActionApi::orderEvent(evt, 3, 1, 0, 0xFFFF, this, hd, -1, 0xFF)) {
                    LinkAction(this).mTingleDemoOrdered = 1;
                }
            } else if (checkDrinkBottleItem((u32)itemNo)) {
                return procBottleDrink_init(itemNo);
            } else if (checkOpenBottleItem((u32)itemNo)) {
                return procBottleOpen_init(itemNo);
            } else if (itemNo == LinkActionConstants::dItemNo_EMPTY_BOTTLE_e) {
                return procBottleSwing_init(1);
            } else if (LinkActionApi::checkGetItemNum(itemNo) != 0) {
                return procNotUse_init(itemNo);
            } else {
                return FALSE;
            }
            return TRUE;
        }
        if (itemNo != 0x50) {
            return FALSE;
        }
    }
    if (mEquipItem != itemNo && itemNo != LinkActionConstants::dItemNo_NONE_e) {
        if (itemNo == LinkActionConstants::dItemNo_BOMB_BAG_e &&
            (mActivePlayerBombs >= 3 || dComIfGs_getBombNum() /* dComIfGs_getBombNum() */ == 0)) {
            LinkActionApi::seStartSystem(LinkActionConstants::JA_SE_ITEM_TARGET_OUT);
            return FALSE;
        }
        mReadyItemBtn = btn;
        setAnimeEquipItem();
    }
    return FALSE;
}

/* 023EFB68 */
BOOL daPy_lk_c::checkItemChangeFromButton() {

    // HD: the sword button ends the HD proc 0xDB
    if ((mItemTrigger & 2) && mCurProc == 0xDB) {
        deleteEquipItem(0);
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    if ((mModeFlg & 4) && !checkEquipAnime() && mActorKeepGrab.mActor.get() == nullptr &&
        !LinkActionCheckPlayerGuard(this)) {
        if (!LK_checkCurse()) {
            u8 trig = mItemTrigger;
            bool itemsBlocked;
            if (trig & 2 /* swordTrigger() */) {
                /* checkSwordEquip(): dComIfGs_getSelectEquip(0) != NONE || checkSwordMiniGame() */
                if (dComIfGs_getSelectEquip(0) != 0xFF || LinkActionPlay().mMiniGameType == 2) {
                    if (mEquipItem != LinkActionConstants::daPyItem_SWORD_e) {
                        setAnimeEquipSword(1);
                        return FALSE;
                    }
                }
                itemsBlocked = LinkAction(this).mItemButtonsBlocked != 0;
                if (!itemsBlocked) {
                    trig = mItemTrigger;
                }
            } else {
                itemsBlocked = LinkAction(this).mItemButtonsBlocked != 0;
            }
            if (!itemsBlocked && (trig & 4 /* itemTriggerX() */)) {
                if (checkNewItemChange(0)) {
                    mReadyItemBtn = 0;
                    return TRUE;
                }
            } else if (!itemsBlocked && (trig & 8 /* itemTriggerY() */)) {
                if (checkNewItemChange(1)) {
                    mReadyItemBtn = 1;
                    return TRUE;
                }
            } else if (!itemsBlocked && (trig & 0x10 /* itemTriggerZ() */)) {
                if (checkNewItemChange(2)) {
                    mReadyItemBtn = 2;
                    return TRUE;
                }
            } else if (!itemsBlocked && (trig & 0x80 /* HD: a fourth item button */)) {
                if (checkNewItemChange(3)) {
                    mReadyItemBtn = 3;
                    return TRUE;
                }
            } else {
                if ((mItemTrigger & 1) && dComIfGp_getDoStatus() == 8 /* dActStts_PUT_AWAY_e */) {
                    setAnimeUnequip();
                }
            }
        } else {
            if ((mNoResetFlg1 & LinkActionConstants::daPyFlg1_EQUIP_DRAGON_SHIELD) &&
                checkSetItemTrigger(0x2A, 0)) {
                mNoResetFlg1 = mNoResetFlg1 & ~1u;
                return FALSE;
            }
            if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !LK_checkPlayerFly()) {
                if ((mNoResetFlg0 & LinkActionConstants::daPyFlg0_EQUIP_HEAVY_BOOTS) &&
                    checkSetItemTrigger(0x29, 0)) {
                    return procBootsEquip_init(0x29);
                } else if (checkSetItemTrigger(LinkActionConstants::daPyItem_DRINK_BOTTLE_e, 0)) {
                    u8 btn = mReadyItemBtn;
                    return procBottleDrink_init(dComIfGp_getSelectItem(btn));
                } else if (checkSetItemTrigger(LinkActionConstants::dItemNo_FAIRY_BOTTLE_e, 0)) {
                    return procBottleOpen_init(0x57);
                }
            }
            if ((mItemTrigger & 1) && dComIfGp_getDoStatus() == 8) {
                setAnimeUnequip();
            }
        }
    }
    return FALSE;
}

/* 023EFF20 */
BOOL daPy_lk_c::checkNextActionFromButton() {

    if (mResetFlg0 & LinkActionConstants::daPyRFlg0_UNK80) {
        u16 eq = mEquipItem;
        if (eq == LinkActionConstants::daPyItem_SWORD_e) {
            resetActAnimeUpper(2 /* UPPER_MOVE2_e */, -1.0f);
            return changeCutProc();
        }
        if (checkPhotoBoxItem(eq) || eq == LinkActionConstants::dItemNo_TELESCOPE_e) {
            return procScope_init((u32)eq);
        }
        if (eq == LinkActionConstants::dItemNo_DEKU_LEAF_e) {
            return procFanSwing_init();
        }
        if (checkBowItem(eq)) {
            return checkNextBowMode();
        }
        if (eq == LinkActionConstants::dItemNo_HOOKSHOT_e) {
            return checkNextHookshotMode();
        }
        if (eq == LinkActionConstants::dItemNo_BOOMERANG_e) {
            return checkNextBoomerangMode();
        }
        if (eq == LinkActionConstants::dItemNo_GRAPPLING_HOOK_e) {
            return checkNextRopeMode();
        }
        if (eq == LinkActionConstants::dItemNo_WIND_WAKER_e) {
            return procTactWait_init(-1);
        }
    }
    u16 upper = LinkAction(this).m_anm_heap_upper[2].mIdx;
    if (upper == 0x95 || upper == 0x96 /* checkGrabAnime() */) {
        if (checkNextActionGrab()) {
            return TRUE;
        }
        setDoStatusBasic();
    } else if (upper == 0xA7 /* checkHookshotReadyAnime() */) {
        if (checkNextActionHookshotReady()) {
            return TRUE;
        }
        setDoStatusBasic();
    } else if (checkBowAnime()) {
        if (checkNextActionBowReady()) {
            return TRUE;
        }
        setDoStatusBasic();
    } else if (upper == 0x34 /* checkBoomerangThrowAnime() */) {
        return TRUE;
    } else if (upper == 0x35 /* checkBoomerangReadyAnime() */) {
        if (checkNextActionBoomerangReady()) {
            return TRUE;
        }
        setDoStatusBasic();
    } else if (checkRopeAnime()) {
        if (checkNextActionRopeReady()) {
            return TRUE;
        }
        setDoStatusBasic();
    } else {
        setDoStatus();
        // HD: the GameCube's spActionTrigger push/pull check moved under the do button (do status 0x11)
        if (orderTalk()) {
            return TRUE;
        }
        if (mItemTrigger & 1 /* doTrigger() */) {
            u8 doStatus = dComIfGp_getDoStatus();
            u32 play = dComIfGp_ea();
            if (doStatus == 0x31 /* dActStts_ba_sake__dupe_31 */) {
                /* dComIfGp_att_getZHint(): dAttCatch_c at play + 0x5928 */
                u32 zhint = LinkActionApi::convCatchPId(gabi::ea(LinkActionCatchInfo(play)),
                                                        LinkActionCatchInfo(play)->mPId);
                LinkActionApi::orderZHintEvent(this, zhint);
                return TRUE;
            }
            if (LinkActionPlay(play).mDoStatus == 0x1C /* dActStts_GET_IN_SHIP_e */) {
                return procShipReady_init();
            }
            if (dComIfGp_getDoStatus() == 0x11) {
                return procPushPullWait_init(1);
            }
            // HD: no CLIMB (procHangWallCatch_init / procVerticalJump_init) here
            if (dComIfGp_getDoStatus() == 0xB /* dActStts_OPEN_e */) {
                u32 entry = mpAttnEntryA;
                fopAc_ac_c* actor = mpAttnActorA;
                if (gabi::at<LinkActionAttentionEntry>(entry)->mType /* mType */ == 5) {
                    LinkActionApi::orderDoorEvent(this, gabi::ea(actor));
                    changeWaitProc();
                } else {
                    LinkActionApi::orderTreasureEvent(this, gabi::ea(actor));
                }
                return TRUE;
            }
            if (dComIfGp_getDoStatus() == 4 /* dActStts_LIFT_e */ ||
                dComIfGp_getDoStatus() == 0x1B /* dActStts_PICK_UP_e */) {
                return procGrabReady_init();
            }
            if (dComIfGp_getDoStatus() == 0x51 /* dActStts_UNK43 */) {
                return procJumpCut_init(0);
            }
            if (dComIfGp_getDoStatus() == 0x10 /* dActStts_SIDLE_e */) {
                return procWHideReady_init(0, &m3724);
            }
        }
    }
    if (!checkEquipAnime() && !LinkActionCheckPlayerGuard(this)) {
        if (!LK_checkCurse()) {
            u16 eq = mEquipItem;
            if (eq == LinkActionConstants::daPyItem_SWORD_e) {
                if (mNoResetFlg0 & LinkActionConstants::daPyFlg0_UNK4) {
                    return procCutTurnCharge_init();
                }
                if (mItemTrigger & 2 /* swordTrigger() */) {
                    if (lk_abs(m3578) > 0xF800) {
                        return procCutTurn_init(1);
                    }
                    return changeCutProc();
                }
                if (m34C5 != 0) {
                    return changeCutProc();
                }
            } else if (eq == LinkActionConstants::daPyItem_BOKO_e) {
                if (mNoResetFlg0 & 4) {
                    return procCutTurnCharge_init();
                }
                // HD: the do button swings the Boko stick; the sword button throws it (play + 0x5BB6 == 0xE)
                if (mItemTrigger & 1) {
                    fopAc_ac_c* weapon = mActorKeepEquip.mActor;
                    if (weapon != nullptr) {
                        if (lk_abs(m3578) > 0xF800) {
                            return procCutTurn_init(1);
                        }
                        u32 prm = weapon->mParameters;
                        if (prm == 0 || prm == 1) {
                            return procWeaponNormalSwing_init();
                        } else if (prm == 4) {
                            return procWeaponSideSwing_init();
                        } else {
                            return procWeaponFrontSwingReady_init();
                        }
                    }
                    if (dComIfGp_getDoStatus() == 0xE /* dActStts_THROW_e */) {
                        return procWeaponThrow_init();
                    }
                }
                if ((mItemTrigger & 2) && LinkActionPlay().mAStatus == 0xE) {
                    return procWeaponThrow_init();
                }
            }
            u32 ready;
            if (itemTrigger() && (ready = getReadyItem(), mEquipItem == ready)) {
                eq = mEquipItem;
                if (eq == 0x2F) {
                    if (LinkAction(this).m_anm_heap_upper[2].mIdx != 0xA7) {
                        return checkNextHookshotMode();
                    }
                } else if (eq == 0x25) {
                    if (!checkRopeAnime()) {
                        return checkNextRopeMode();
                    }
                } else {
                    if (eq == 0x2D) {
                        return checkNextBoomerangMode();
                    }
                    if (checkBowItem(eq)) {
                        return checkNextBowMode();
                    }
                    if (eq == 0x34) {
                        return procFanSwing_init();
                    }
                    if (eq == LinkActionConstants::dItemNo_EMPTY_BOTTLE_e) {
                        return procBottleSwing_init(0);
                    }
                    if (eq == LinkActionConstants::dItemNo_SKULL_HAMMER_e) {
                        int dir = getDirectionFromShapeAngle();
                        if (mStickDistance > 0.05f &&
                            (dir == 2 /* DIR_LEFT */ || dir == 3 /* DIR_RIGHT */)) {
                            return procHammerSideSwing_init();
                        }
                        return procHammerFrontSwingReady_init();
                    }
                    if (checkPhotoBoxItem(eq) || eq == 0x20) {
                        return procScope_init((u32)eq);
                    }
                    if (eq == 0x22) {
                        return procTactWait_init(-1);
                    }
                }
            }
        }
        if (changeSpecialBattle()) {
            return TRUE;
        }
    }
    if (dComIfGp_getRStatus() == 0 /* dActStts_BLANK_e */ && !checkUpperReadyThrowAnime() &&
        !checkEquipAnime()) {
        u16 up = LinkAction(this).m_anm_heap_upper[2].mIdx;
        if (up != 0x95 && up != 0x96 /* !checkGrabAnime() */) {
            if (mNoResetFlg1 & LinkActionConstants::daPyFlg1_NPC_CALL_COMMAND) {
                dComIfGp_setRStatus(0x24 /* dActStts_CALL_e */);
                if (mItemTrigger & 0x40 /* spActionTrigger() */) {
                    return procCall_init();
                }
            } else if (mEquipItem == 0x101) {
                dComIfGp_setRStatus(9 /* dActStts_DROP_e */);
                if (mItemTrigger & 0x40) {
                    deleteEquipItem(0);
                    LinkActionApi::initOldFrameMorf((u32)m_old_fdata, 5.0f, 0, 0x2A);
                    return TRUE;
                }
            } else if (dComIfGs_getSelectEquip(1) /* dComIfGs_getSelectEquip(1) */ == 0xFF ||
                       mEquipItem == 0x100) {
                if (mModeFlg & 1) {
                    dComIfGp_setRStatus(0xF /* dActStts_CROUCH_e */);
                    if (mItemButton & 0x40 /* spActionButton() */) {
                        return procCrouch_init();
                    }
                }
            } else {
                if (checkGuardAccept()) {
                    dComIfGp_setRStatus(0x36 /* dActStts_DEFEND_e */);
                    if (mItemButton & 0x40) {
                        return procCrouchDefense_init();
                    }
                }
            }
        }
    }
    if (mItemTrigger & 1) {
        if (dComIfGp_getDoStatus() == 0x12 /* dActStts_JUMP_e */) {
            int dir = getDirectionFromShapeAngle();
            if (dir == 2 || dir == 3) {
                return procSideStep_init(dir);
            }
            if (dir == 1 /* DIR_BACKWARD */) {
                return procBackJump_init();
            }
        } else if (dComIfGp_getDoStatus() == 0xC /* dActStts_ATTACK_e */) {
            if (!lk_checkAttentionLock(mpAttention) && mStickDistance > 0.05f) {
                shape_angle.y = m34E8;
            }
            return procFrontRoll_init(0.0f);
        }
    }
    if (gabi::ea(LinkActionPlay().mPlayerInfo[0].mpPlayer.get()) ==
            gabi::ea(this) /* daPy_getPlayerActorClass() == this */
        && !dComIfGp_event_runCheck() && !(field_0x2b0 < 0.0f) /* !checkGrabWear() */) {
        mResetFlg0 = mResetFlg0 | LinkActionConstants::daPyRFlg0_SUBJECT_ACCEPT;
        s32 camIdx = mCameraInfoIdx;
        if (dComIfGp_getCameraAttentionStatus(camIdx) & 0x1000) {
            return procSubjectivity_init(0);
        }
    }
    return checkItemChangeFromButton();
}

/* 023F0CE4 */
BOOL daPy_lk_c::checkAtnWaitAnime() {

    u32 lock = gabi::ea(mpAttnActorLockOn.get());
    if ((lock != 0 && (gabi::at<fopAc_ac_c>(lock)->group == 2 /* fopAc_ENEMY_e */ ||
                       (lock != 0 && (s16)gabi::at<fopAc_ac_c>(lock)->mProcName ==
                                         LinkActionConstants::fpcNm_BOOMERANG_e))) ||
        mDemo.mDemoMode == 0x17 /* daPy_demo_c::DEMO_A_WAIT_e */) {
        return TRUE;
    }
    return FALSE;
}

/* 023F0D2C */
BOOL daPy_lk_c::procAtnActorWait_init() {

    if (mCurProc == LinkActionConstants::daPyProc_ATN_ACTOR_WAIT_e) {
        return FALSE;
    }
    commonProcInit(8);
    s32 angle;
    fopAc_ac_c* lock = mpAttnActorLockOn;
    if (lock != nullptr) {
        angle = LinkActionApi::searchActorAngleY(this, gabi::ea(lock));
    } else {
        angle = 0;
    }
    if (mDirection == 2 /* DIR_LEFT */ ||
        (mDirection != 3 /* DIR_RIGHT */ && angle - shape_angle.y >= 0)) {
        mDirection = 2;
    } else {
        mDirection = 3;
    }
    if (checkAtnWaitAnime()) {
        setBlendAtnMoveAnime(2.4f);
    } else {
        setBlendMoveAnime(2.4f);
    }
    return TRUE;
}

/* 023F0E18 */
BOOL daPy_lk_c::procAtnActorMove_init() {

    if (mCurProc == LinkActionConstants::daPyProc_ATN_ACTOR_MOVE_e) {
        return FALSE;
    }
    commonProcInit(9);
    setBlendAtnMoveAnime(2.4f);
    mProcVar0 = 0x14;
    return TRUE;
}

/* 023F0E8C */
BOOL daPy_lk_c::procAtnMove_init() {

    if (mCurProc == LinkActionConstants::daPyProc_ATN_MOVE_e) {
        return FALSE;
    }
    commonProcInit(7);
    setBlendAtnMoveAnime(2.4f);
    mProcVar0 = 0x14;
    return TRUE;
}

/* 023F0F00 */
BOOL daPy_lk_c::procWaitTurn_init() {

    if (mCurProc == LinkActionConstants::daPyProc_WAIT_TURN_e) {
        return FALSE;
    }
    commonProcInit(0x17);
    setSingleMoveAnime(LinkActionConstants::ANM_ROT, 1.0f, 0.0f, -1, 2.4f);
    if (dComIfGp_event_runCheck()) {
        mNormalSpeed = 0.0f;
    }
    mProcVar2 = m34E8;
    current.angle.y = shape_angle.y;
    return TRUE;
}

/* 023F0FD0 */
BOOL daPy_lk_c::procMoveTurn_init(int param) {

    if (mCurProc == LinkActionConstants::daPyProc_MOVE_TURN_e) {
        return FALSE;
    }
    commonProcInit(0x18);
    setBlendMoveAnime(2.4f);
    dComIfGp_setPlayerStatus0(0, 0x800 /* daPyStts0_UNK800_e */);
    if (param != 0) {
        mProcVar2 = 0x7936;
        mProcVar3 = 0x1770;
        mProcVar0 = 2;
        current.angle.y = m34E8;
        mNormalSpeed = mNormalSpeed * 0.5f;
    } else {
        mProcVar2 = 0x1770;
        mProcVar3 = 0xBB8;
        mProcVar0 = 3;
    }
    return TRUE;
}

/* 023F10B8 */
BOOL daPy_lk_c::procSlip_init() {

    commonProcInit(LinkActionConstants::daPyProc_SLIP_e);
    linkDisableBlink();
    setSingleMoveAnime(LinkActionConstants::ANM_SLIP, 0.4f, 0.0f, -1, 1.7f);
    mNormalSpeed = speedF * 1.1f; // HD: the HIO max-speed mode is 0 (no clamp)
    mFootEffectPosType = 2;
    seStartMapInfo(LinkActionConstants::JA_SE_LK_RUN_SLIP);
    return TRUE;
}

/* 023F1154 */
BOOL daPy_lk_c::procMove_init() {

    if (mCurProc == LinkActionConstants::daPyProc_MOVE_e) {
        return FALSE;
    }
    commonProcInit(6);
    setBlendMoveAnime(2.4f);
    mProcVar0 = 0x14;
    return TRUE;
}

/* 023F11C8 */
BOOL daPy_lk_c::procVomitWait_init() {

    commonProcInit(LinkActionConstants::daPyProc_VOMIT_WAIT_e);
    setSingleMoveAnime(LinkActionConstants::ANM_VOMITJMP, 1.0f, 0.0f, -1, 5.0f);
    u32 ac = LinkActionApi::getCollisionActor(gabi::ea(&mCyl.mGObjCo));
    u32 me = gabi::ea(this);
    /* current.pos = ac->current.pos; old.pos = current.pos (integer copies) */
    u32 x = gabi::at<LinkActionActorWords>(ac)->mCurrentPos[0];
    gabi::at<LinkActionActorWords>(me)->mCurrentPos[0] = x;
    u32 y = gabi::at<LinkActionActorWords>(ac)->mCurrentPos[1];
    gabi::at<LinkActionActorWords>(me)->mCurrentPos[1] = y;
    u32 z = gabi::at<LinkActionActorWords>(ac)->mCurrentPos[2];
    current.angle.y = shape_angle.y;
    gabi::at<LinkActionActorWords>(me)->mCurrentPos[2] = z;
    gabi::at<LinkActionActorWords>(me)->mOldPos[0] = x;
    m34C2 = 11;
    gabi::at<LinkActionActorWords>(me)->mOldPos[1] = y;
    mProcVar2 = 0;
    mNormalSpeed = 0.0f;
    gabi::at<LinkActionActorWords>(me)->mOldPos[2] = z;
    dComIfGp_setPlayerStatus0(0, 0x80 /* daPyStts0_UNK80_e */);
    mProcVar0 = 0; /* as the GameCube PAL version */
    return TRUE;
}

/* 023F12A4 */
BOOL daPy_lk_c::procVomitReady_init(s16 angle, f32 dist) {

    commonProcInit(LinkActionConstants::daPyProc_VOMIT_READY_e);
    setSingleMoveAnime(LinkActionConstants::ANM_MSTEPOVER, 0.3f, 1.0f, 6, 10.0f);
    f32 g = gravity;
    speed.y = 28.0f;
    mNormalSpeed = -((dist * g) / 56.0f);
    if (checkHeavyStateOn()) {
        speed.y = speed.y * 1.5f;
        mNormalSpeed = mNormalSpeed * 1.5f;
    }
    current.angle.y = angle;
    shape_angle.y = angle;
    LinkActionVoiceStart(this, 6);
    return TRUE;
}

/* 023F13A0 */
BOOL daPy_lk_c::checkJumpFlower() {

    u32 ac = LinkActionApi::getCollisionActor(gabi::ea(&mCyl.mGObjCo));
    if (mCyl.ChkCoHit() && ac != 0 &&
        (s16)gabi::at<fopAc_ac_c>(ac)->mProcName == LinkActionConstants::fpcNm_JBO_e) {
        if (mModeFlg & 2 /* ModeFlg_MIDAIR */) {
            if (!(speed.y > 0.0f) &&
                current.pos.y > gabi::at<fopAc_ac_c>(ac)->current.pos.y - 50.0f) {
                return procVomitWait_init();
            }
        } else {
            s32 angle = LinkActionApi::searchActorAngleY(this, ac);
            if (LinkActionApi::distanceAngleS(angle, (s32)shape_angle.y) < 0x2000 &&
                mStickDistance > 0.2f) {
                f64 dist = LinkActionApi::searchActorDistanceXZ(this, ac);
                return LinkActionApi::procVomitReadyWide(this, angle, dist);
            }
        }
    }
    return FALSE;
}

/* 023F14E0 */
BOOL daPy_lk_c::checkNextMode(int noStick) {

    f32 oldMax = mMaxNormalSpeed;
    setFrontWallType();
    if (mNoResetFlg1 & LinkActionConstants::daPyFlg1_FORCE_VOMIT_JUMP) {
        return procVomitJump_init(2);
    }
    if (mNoResetFlg1 & LinkActionConstants::daPyFlg1_FORCE_VOMIT_JUMP_SHORT) {
        return procVomitJump_init(3);
    }
    BOOL atn = lk_checkAttentionLock(mpAttention) ||
               (mActorKeepThrow.mActor.get() != nullptr &&
                gabi::ea(mpAttnActorLockOn.get()) == gabi::ea(mActorKeepThrow.mActor.get())) ||
               checkUpperReadyThrowAnime() || mDemo.mDemoMode == 0x17 /* DEMO_A_WAIT_e */;
    if (atn) {
        mMaxNormalSpeed = 12.0f;
    } else {
        mMaxNormalSpeed = 17.0f;
    }
    if (changeSlideProc()) {
        return TRUE;
    }
    if (checkNextActionFromButton()) {
        return TRUE;
    }
    if (noStick != 0 && !(mStickDistance > 0.05f) && !(mItemButton & 0x40 /* spActionButton() */)) {
        mMaxNormalSpeed = oldMax;
        return FALSE;
    }
    BOOL r;
    if (atn) {
        if (checkBoomerangAnime()) {
            r = checkNextBoomerangMode();
        } else if (checkBowAnime()) {
            r = checkNextBowMode();
        } else if (LinkAction(this).m_anm_heap_upper[2].mIdx ==
                   0xA7 /* checkHookshotReadyAnime() */) {
            r = checkNextHookshotMode();
        } else if (checkRopeAnime()) {
            r = checkNextRopeMode();
        } else if (mpAttnActorLockOn.get() != nullptr || mDemo.mDemoMode == 0x17) {
            if (std::fabs((f32)mNormalSpeed) > 0.001f) {
                r = procAtnActorMove_init();
            } else {
                r = procAtnActorWait_init();
            }
        } else if (std::fabs((f32)mNormalSpeed) > 0.001f) {
            r = procAtnMove_init();
        } else {
            r = changeWaitProc();
        }
    } else {
        mDirection = 4; /* DIR_NONE */
        int dir = getDirectionFromCurrentAngle();
        if (!(std::fabs((f32)mNormalSpeed) > 0.001f)) {
            if (cLib_distanceAngleS(m34E8, current.angle.y) > 0x7800 && mStickDistance > 0.05f) {
                r = procWaitTurn_init();
                if (!r && !dComIfGp_event_runCheck() &&
                    mDemo.mDemoType == 0 /* !checkPlayerDemoMode() */) {
                    r = changeWaitProc();
                }
            } else {
                r = changeWaitProc();
            }
        } else {
            s16 angleY = current.angle.y;
            if (mCurProc == LinkActionConstants::daPyProc_MOVE_TURN_e && angleY != shape_angle.y) {
                r = procMoveTurn_init(0);
            } else if (cLib_distanceAngleS(m34E8, angleY) > 0x7800 && mStickDistance > 0.05f) {
                if (speedF / mMaxNormalSpeed > 0.6f &&
                    mCurrAttributeCode != 0xF /* dBgS_Attr_ICE_e */ &&
                    LinkAction(this).m_anm_heap_upper[2].mIdx != 0x95 &&
                    LinkAction(this).m_anm_heap_upper[2].mIdx != 0x96 &&
                    getDirectionFromAngle((s16)(m34EA - m34DC)) == 1 /* DIR_BACKWARD */) {
                    r = procSlip_init();
                } else {
                    r = procMoveTurn_init(1);
                }
            } else if (dir == 1 && mStickDistance > 0.05f) {
                r = procMoveTurn_init(1);
            } else {
                r = procMove_init();
            }
        }
    }
    if (!r) {
        r = checkJumpFlower();
    }
    return r;
}

/* 023F19D0 */
void daPy_lk_c::initShipBaseAnime() {

    if ((u16)LinkAction(this).m_anm_heap_under[0].mIdx /* m_anm_heap_under[UNDER_MOVE0_e].mIdx */ !=
        LinkActionConstants::dRes_INDEX_LKANM_BCK_SHIP_JUMP2_e) {
        setSingleMoveAnime(LinkActionConstants::ANM_VOYAGE1, 1.0f, 0.0f, -1, 5.0f);
    }
    setActAnimeUpper(LinkActionConstants::dRes_INDEX_LKANM_BCK_FN_HAIR1_e, 2 /* UPPER_MOVE2_e */,
                     1.0f, 0.0f, -1, -1.0f);
    linkSetJointBlendRatio(m_pbCalc[1], 2, 0.0f); /* m_pbCalc[PART_UPPER_e]->setRatio(2, 0.0f) */
}

/* 023F1AC8 */
BOOL daPy_lk_c::procShipCannon_init() {

    if (mCurProc == LinkActionConstants::daPyProc_SHIP_CANNON_e) {
        return FALSE;
    }
    u32 ship = gabi::ea(dComIfGp_getShipActor());
    commonProcInit(0x8E);
    deleteEquipItem(1);
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    gabi::at<LinkActionShip>(ship)->mMode = 9; /* ship->setCannon() */
    initShipBaseAnime();
    setShipRidePos(1);
    dComIfGp_setPlayerStatus0(0, 0x10000 /* daPyStts0_SHIP_RIDE_e */);
    dComIfGp_setPlayerStatus1(0, LinkActionConstants::daPyStts1_UNK4_e);
    return TRUE;
}

/* 023F1B90 */
void daPy_lk_c::initShipCraneAnime() {

    int anm;
    if (gabi::at<LinkActionShip>(gabi::ea(dComIfGp_getShipActor()))
            ->mCraneBaseAngle /* getCraneBaseAngle() */
        > 0) {
        anm = 0xCD; /* ANM_SALVRWAIT */
        mProcVar7 = 3;
        mProcVar6 = 3;
    } else {
        anm = 0xCE; /* ANM_SALVLWAIT */
        mProcVar7 = 2;
        mProcVar6 = 2;
    }
    linkDisableBlink();
    setSingleMoveAnime(anm, 1.0f, 0.0f, -1, 10.0f);
}

/* 023F1C68 */
BOOL daPy_lk_c::procShipCrane_init() {

    if (mCurProc == LinkActionConstants::daPyProc_SHIP_CRANE_e) {
        return FALSE;
    }
    u32 ship = gabi::ea(dComIfGp_getShipActor());
    commonProcInit(0x8F);
    deleteEquipItem(1);
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    gabi::at<LinkActionShip>(ship)->mMode = 0xA; /* ship->setCrane() */
    if (gabi::at<LinkActionShip>(ship)->mRopeCnt /* getRopeCnt() */ != 0) {
        initShipCraneAnime();
    } else {
        initShipBaseAnime();
    }
    setShipRidePos(1);
    dComIfGp_setPlayerStatus0(0, 0x10000);
    dComIfGp_setPlayerStatus1(0, LinkActionConstants::daPyStts1_UNK2_e);
    mProcVar2 = 0;
    return TRUE;
}

/* 023F1D84 */
BOOL daPy_lk_c::procShipSteer_init() {

    mNoResetFlg0 = mNoResetFlg0 & ~1u; /* offNoResetFlg0(daPyFlg0_UNK1) */
    u32 ship = gabi::ea(dComIfGp_getShipActor());
    if (mCurProc == LinkActionConstants::daPyProc_SHIP_STEER_e) {
        gabi::at<LinkActionShip>(ship)->mMode = 1; /* ship->setSteerMove() */
        return FALSE;
    }
    commonProcInit(0x88);
    deleteEquipItem(1);
    gravity = 0.0f;
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    gabi::at<LinkActionShip>(ship)->mMode = 1;
    initShipBaseAnime();
    setShipRidePos(1);
    dComIfGp_setPlayerStatus0(0, 0x10000);
    return TRUE;
}

/* 023F1E60 */
BOOL daPy_lk_c::procShipPaddle_init() {

    mNoResetFlg0 = mNoResetFlg0 & ~0x800000u; /* offNoResetFlg0(daPyFlg0_UNK800000) */
    if (mCurProc == LinkActionConstants::daPyProc_SHIP_PADDLE_e) {
        return FALSE;
    }
    u32 ship = gabi::ea(dComIfGp_getShipActor());
    BOOL riding = dComIfGp_checkPlayerStatus0(0, 0x10000) && !checkShipNotNormalMode();
    commonProcInit(0x89);
    deleteEquipItem(1);
    mNormalSpeed = 0.0f;
    gravity = 0.0f;
    speed.y = 0.0f;
    gabi::at<LinkActionShip>(ship)->mMode = 2; /* ship->setPaddleMove() */
    initShipBaseAnime();
    if (!riding) {
        setOldRootQuaternion(
            0, (s32)(s16)(shape_angle.y - gabi::at<fopAc_ac_c>(ship)->shape_angle.y), 0);
    }
    setShipRidePos(1);
    dComIfGp_setPlayerStatus0(0, 0x10000);
    mProcVar0 = (s16)gabi::ftoi(cM_rndF(150.0f) + 300.0f);
    return TRUE;
}

/* 023F2048 */
void daPy_lk_c::endDemoMode() {

    u32 mode = (u32)mDemo.mDemoMode;
    if (mode == 6 || mode == 0x25) {
        LinkAction(this).mItemButtonsBlocked =
            0x1E; // HD: after a talk/present demo the item buttons stay blocked for 30 frames
    }
    if (mCurProc != LinkActionConstants::daPyProc_LARGE_DAMAGE_e) {
        current.angle.y = shape_angle.y;
    }
    mNoResetFlg0 = mNoResetFlg0 & ~0x100000u; /* offNoResetFlg0(daPyFlg0_UNK100000) */
    auto& status = LinkActionPlay()
                       .mPlayerStatus[0][0]; /* dComIfGp_clearPlayerStatus0(0, daPyStts0_UNK10_e) */
    status = (u32)status & ~0x10u;
    BOOL wasSpecial = mDemo.mDemoType == 5; /* HD */
    mDemo.mDemoMode = 0;
    mDemo.mParam1 = 0; /* mDemo.setParam1(0) */
    mDemo.mParam0 = 0; /* mDemo.setParam0(0) */
    mDemo.mDemoType = 0;
    mDemo.mStick = 1.0f; /* mDemo.setStick(1.0f) */
    resetDemoTextureAnime();
    if (mEquipItem == LinkActionConstants::daPyItem_UNK104_e ||
        mEquipItem == LinkActionConstants::daPyItem_UNK10A_e) {
        deleteEquipItem(0);
        mNoResetFlg1 = mNoResetFlg1 & ~0x1000u; /* offShipTact() */
    }
    LinkActionApi::taktModeMuteOff();
    if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !(mModeFlg & 0x14452822) &&
        mCurProc != 0x69 /* LARGE_DAMAGE_UP */ && mCurProc != 0xB2 /* DEMO_DEAD */ &&
        mCurProc != 0xB0 /* DEMO_LAVA_DAMAGE */) {
        if (dComIfGp_checkPlayerStatus0(0, 0x200000 /* daPyStts0_TELESCOPE_LOOK_e */)) {
            procScope_init(0x20);
            return;
        }
        changeWaitProc();
        return;
    }
    if (dComIfGp_checkPlayerStatus0(0, 0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        u32 ship = gabi::ea(dComIfGp_getShipActor());
        if (ship == 0) {
            checkNextMode(0);
        } else if (gabi::ea(LinkActionPlay().mPlayerInfo[0].mpPlayer.get()) !=
                   gabi::ea(this) /* daPy_getPlayerActorClass() != this */) {
            procControllWait_init();
        } else {
            u8 part = gabi::at<LinkActionShip>(ship)->mPart; /* ship->getPart() */
            if (part == 2 /* PART_CANNON_e */) {
                procShipCannon_init();
            } else if (part == 3 /* PART_CRANE_e */) {
                procShipCrane_init();
            } else if (part == 1 /* PART_STEER_e */) {
                procShipSteer_init();
            } else {
                procShipPaddle_init();
            }
        }
    } else {
        s32 proc = mCurProc;
        // HD: a special demo also ends in procWait_init unless the proc is one of three ranges
        if (proc == LinkActionConstants::daPyProc_DEMO_BOSS_WARP_e ||
            (wasSpecial && !((u32)(proc - 0xF) < 4) && !((u32)(proc - 0x96) < 4) &&
             !((u32)(proc - 0x76) < 0xA))) {
            procWait_init();
        }
    }
}

/* ---- HD-only procs (no GameCube source): the inits of the HD message-bottle procs whose bodies are in
 * range #06 (dProcHDBottleThrow 0xDC, dProcHDLetterWrite 0xDB). Names are descriptive. ---- */

/* 023F42EC */
BOOL daPy_lk_c::dProcHDBottleThrow_init() {

    if (mCurProc == 0xDC) {
        return TRUE;
    }
    int use = checkShipRideUseItem(1);
    commonProcInit(0xDC);
    mNormalSpeed = 0.0f;
    initShipRideUseItem(use, 2);
    deleteEquipItem(0);
    u32 item = LinkActionApi::fastCreateItem2(&current.pos, 0x13, -1, -1, 0, 0, 0, 0);
    LinkActionApi::actorKeepSetData(&mActorKeepGrab, item);
    mEquipItem = 0x100;
    setSingleMoveAnime(LinkActionConstants::ANM_WAITS, 1.0f, 0.0f, -1, 5.0f);
    setActAnimeUpper(0x95, 2, 0.0f, 0.0f, -1, 5.0f);
    for (s32 i = 0; i < LinkAction(this).mAnmRatioUpper[2].mJointCount;
         i++) { /* [?] the HD weight array (count +0x5840, data +0x5844) */
        LinkAction(this).mAnmRatioUpper[2].mpJointWeights.get()[i] = 0.0f;
    }
    u32 grab = gabi::ea(mActorKeepGrab.mActor.get());
    m35A4 = 0.0f;
    if (grab == 0) {
        return FALSE;
    }
    LinkActionApi::setCarryNow(grab, 0);
    LinkAction(this).mThrowActor = grab;
    mProcVar7 = 1;
    m35C8 = 17.0f;
    linkStartEventCamera(this, gabi::guest_addr("Type"), &mProcVar7);
    u32 body = LinkActionApi::getCameraBody();
    LinkActionApi::endEventCamera(body, mBsPcId);
    s32 camIdx = mCameraInfoIdx;
    u32 cam = LinkActionPlay().mCameraInfo[camIdx].mpCamera;
    u32 tid = 0xFFFFFFFF;
    if (LinkAction(this).mThrowActor != 0) {
        tid = gabi::at<fopAc_ac_c>(LinkAction(this).mThrowActor)->mBsPcId;
    }
    body = LinkActionApi::getCameraBody();
    LinkActionApi::startEventCameraMode(body, 6, tid, gabi::guest_addr("Type"), 1, 0);
    LinkActionApi::stopCamera(cam + 0x248);
    tid = 0xFFFFFFFF;
    if (LinkAction(this).mThrowActor != 0) {
        tid = gabi::at<fopAc_ac_c>(LinkAction(this).mThrowActor)->mBsPcId;
    }
    LinkActionApi::forceCameraLockOn(cam + 0x248, tid);
    cXyz eye;
    cXyz eye2;
    linkAddPosition(cam + 0x264, &eye, cam + 0x7C0); /* the camera &eye */
    linkCopyPositionWords(gabi::ea(&m370C), &eye);
    linkAddPosition(cam + 0x264, &eye, cam + 0x7C0);
    linkCopyPositionWords(gabi::ea(&LinkAction(this).mThrowPos), &eye);
    linkAddPosition(cam + 0x264, &eye, cam + 0x7C0);
    linkAddPosition(cam + 0x264, &eye2, cam + 0x7C0);
    LinkAction(this).mThrowAngle = cM_atan2s(current.pos.x - eye.x, current.pos.z - eye2.z);
    if (LinkAction(this).mBottleThrowPending != 0) {
        mProcVar6 = 3;
        cXyz center;
        cXyz eye3;
        linkAddPosition(cam + 0x258, &center, cam + 0x7B4);
        linkAddPosition(cam + 0x264, &eye3, cam + 0x7C0);
        LinkActionApi::setCamera(cam + 0x248, &center, &eye3, 60.0f, 0);
        m35A8 = 0.0f;
        m35A0 = 0.0f;
        LinkAction(this).mBottleThrowPending = 0;
        m35A4 = 0.0f;
    } else {
        m35A8 = 0.0f;
        m35A4 = 0.0f;
        mProcVar6 = 4;
        m35A0 = 0.0f;
    }
    return TRUE;
}

/* 023F4688 */
BOOL daPy_lk_c::dProcHDLetterWrite_init() {

    if (mCurProc != 0xDB) {
        mProcVar7 = 1;
        linkStartEventCamera(this, gabi::guest_addr("Type"), &mProcVar7);
        int use = checkShipRideUseItem(1);
        commonProcInit(0xDB);
        mNormalSpeed = 0.0f;
        setSingleMoveAnime(0xB2, 1.0f, 0.0f, -1, 5.0f);
        deleteEquipItem(0);
        initShipRideUseItem(use, 2);
        m35A4 = 0.0f;
        LinkAction(this).mThrowPos.y = 0.0f;
        mProcVar6 = 0;
        LinkAction(this).mThrowPos.z = 0.0f;
        LinkAction(this).mThrowAngle = 0;
        m35A0 = 0.0f;
        LinkAction(this).mThrowActor = 0;
        m370C.y = 0.0f;
        m35A8 = 0.0f;
        m370C.z = 0.0f;
        LinkAction(this).mThrowPos.x = 0.0f;
        m370C.x = 0.0f;
        u32 x = LinkActionApi::getLetterWriter();
        LinkActionApi::setLetterWritingMode(x, 0);
    }
    return TRUE;
}

/* 023F47C0 */
BOOL daPy_lk_c::dProcTool_init() {

    if (mCurProc == LinkActionConstants::daPyProc_DEMO_TOOL_e) {
        return TRUE;
    }
    commonProcInit(0xA9);
    speed.y = 0.0f;
    speedF = 0.0f;
    mNormalSpeed = 0.0f;
    LinkAction(this).m_anm_heap_under[0].field_0x6 =
        -1; /* m_anm_heap_under[UNDER_MOVE0_e].field_0x6 */
    LinkAction(this).m_anm_heap_under[0].field_0x4 =
        -1;                                         /* m_anm_heap_under[UNDER_MOVE0_e].field_0x4 */
    LinkAction(this).m_anm_heap_upper[0].mIdx = -1; /* m_anm_heap_upper[UPPER_MOVE0_e].mIdx */
    LinkAction(this).m_anm_heap_under[1].mIdx = -1; /* m_anm_heap_under[UNDER_MOVE1_e].mIdx */
    LinkAction(this).m_anm_heap_upper[1].mIdx = -1; /* m_anm_heap_upper[UPPER_MOVE1_e].mIdx */
    linkSetJointBlendRatio(m_pbCalc[1], 0, 1.0f);
    linkSetJointBlendRatio(m_pbCalc[1], 1, 0.0f);
    linkSetJointBlendRatio(m_pbCalc[0], 0, 1.0f);
    linkSetJointBlendRatio(m_pbCalc[0], 1, 0.0f);
    LinkAction(this).mAnmRatioUpper[1].mpAnmFrame = 0; /* mAnmRatioUpper[1].setAnmTransform(NULL) */
    mProcVar3 = 0;
    mProcVar7 = 0;
    mProcVar2 = 0;
    LinkAction(this).mAnmRatioUnder[1].mpAnmFrame = 0; /* mAnmRatioUnder[1].setAnmTransform(NULL) */
    mProcVar6 = 0;
    return TRUE;
}

/* 023F49A4 */
int daPy_lk_c::setTalkStartBack() {

    u32 partner = LinkActionApi::getTalkEventPartner(this);
    if (partner != 0) {
        cXyz d;
        cXyz_mi(&current.pos, &d, gabi::at<cXyz>(partner + 0x314));
        cXyz xz; /* absXZ() */
        xz.x = d.x;
        xz.y = 0.0f;
        xz.z = d.z;
        f64 sq = LinkActionApi::squareMagnitude(&xz);
        f64 len = LinkActionApi::sqrtWide(sq);
        if (len < 100.0 && len > 1.0) {
            f32 k = (f32)(100.0 / len);
            f32 x = gabi::fmadds(k, d.x, gabi::at<fopAc_ac_c>(partner)->current.pos.x);
            f32 y = current.pos.y + 30.1f;
            m370C.x = x;
            m370C.y = y;
            LinkAction(this).mGndChkPos.y = y; /* mGndChk.SetPos(&m370C) */
            f32 z = gabi::fmadds(k, d.z, gabi::at<fopAc_ac_c>(partner)->current.pos.z);
            LinkAction(this).mGndChkPos.x = x;
            m370C.z = z;
            LinkAction(this).mGndChkPos.z = z;
            f64 gnd = LinkActionApi::groundCross(dComIfG_Bgsp(), mGndChk);
            if (!((f32)(gnd - (f64)(f32)current.pos.y) < -30.1f)) {
                current.angle.y = cM_atan2s(d.x, d.z);
                mNormalSpeed = 5.0f;
                setSingleMoveAnime(LinkActionConstants::ANM_WALK, -1.2f, 0.0f, -1, 5.0f);
                return 1;
            }
        }
    }
    return 0;
}

/* 023F4B24 */
BOOL daPy_lk_c::dProcTalk_init() {

    if (mCurProc == LinkActionConstants::daPyProc_DEMO_TALK_e) {
        return TRUE;
    }
    mProcVar6 = 0;
    int use = checkShipRideUseItem(1);
    LinkActionPlay().mMetronome = 0; /* HD */
    commonProcInit(0xAA);
    if (use == 0) {
        if (mDemo.mDemoMode != 8 && mEquipItem != 0x100) {
            setAnimeUnequip();
        }
        mProcVar6 = setTalkStartBack();
    }
    if (mProcVar6 == 0) {
        mNormalSpeed = 0.0f;
        if (mDemo.mDemoMode == 8) {
            setSingleMoveAnime(LinkActionConstants::ANM_WAITS, 1.1f, 0.0f, -1, 5.0f);
        } else {
            setSingleMoveAnime(LinkActionConstants::ANM_TALKA, 0.7f, 0.0f, -1, 5.0f);
        }
        mProcVar7 = mDemo.mParam1; /* mDemo.getParam1() */
        if (mProcVar7 == 1) {
            setTextureAnime(0x91, 0);
            mModeFlg = mModeFlg & ~0x80u;
        }
    }
    dComIfGp_setPlayerStatus0(0, 0x10 /* daPyStts0_UNK10_e */);
    if (mDemo.mParam0 != 1) {
        LinkActionApi::seStartSystem(LinkActionConstants::JA_SE_TALK_START);
    }
    initShipRideUseItem(use, 0);
    return TRUE;
}

/* 023F4CB0 HD-only (no GameCube source; name descriptive): the damage taken while the Magic Armor is on
 * costs rupees in HD (GameCube: magic). Plays the shield sound, drops the lost rupees around Link
 * (largest denominations first, at most 200 rupees' worth) and subtracts the whole amount. */
/* 023F4CB0 */
void daPy_lk_c::setDamageRupee(f32 amount) {

    seStartMapInfo(LinkActionConstants::JA_SE_LK_MGC_SHIELD_DEF);
    s32 lost = -(gabi::ftoi(amount) * 20);
    s32 left = lost;
    if (lost > 200) {
        left = 200;
    }
    if (left != 0) {
        cXyz pos;
        cXyz ofs;
        cXyz p;
        cXyz rot;
        do {
            int item;
            if (left >= 150) {
                left -= 100;
                item = 6; /* SILVER_RUPEE (100) */
            } else if (left >= 80) {
                left -= 50;
                item = 5; /* ORANGE_RUPEE (50) */
            } else if (left >= 30) {
                left -= 20;
                item = 4; /* PURPLE_RUPEE (20) */
            } else if (left >= 20) {
                left -= 10;
                item = 3; /* RED_RUPEE (10) */
            } else if (left >= 10) {
                left -= 5;
                item = 2; /* BLUE_RUPEE (5) */
            } else {
                left -= 1;
                item = 1; /* GREEN_RUPEE (1) */
            }
            s16 a = (s16)gabi::ftoi(cM_rndFX(32000.0f));
            mDoMtx_YrotS(LinkActionMatrixStack(), a);
            ofs.x = 0.0f;
            ofs.y = 0.0f;
            ofs.z = 40.0f;
            MtxPosition(&ofs, &rot);
            linkAddPosition(gabi::ea(&current.pos), &p, gabi::ea(&rot));
            pos.z = p.z;
            f32 py = p.y;
            pos.y = py;
            pos.x = p.x;
            f32 r = cM_rndF(60.0f);
            pos.y = py + (r + 40.0f);
            LinkActionApi::createItem(&pos, item, -1, -1, -1, 0, 0xF, 0);
        } while (left != 0);
    }
    auto& rupees = LinkActionPlay().mItemRupeeCount; /* the pending rupee change */
    rupees = (s32)rupees - lost;
}

/* 023F51D0 */
BOOL daPy_lk_c::setDamagePoint(f32 amount) {

    // HD: hero mode doubles the damage
    u32 mode = LinkActionApi::getHeroModeManager(dComIfGs_ea() + 0x12C0);
    if (LinkActionApi::isHeroMode(mode)) {
        amount = gabi::fadds_ppc(amount, amount);
    }
    if (!(mNoResetFlg1 & 1) && mTinkleShieldTimer == 0 /* !checkNoDamageMode() */) {
        auto& life = LinkActionPlay().mItemLifeCount; /* dComIfGp_setItemLifeCount(amount) */
        life = gabi::fadds_ppc(life, amount);
        if (amount < 0.0f) {
            mNoResetFlg1 = mNoResetFlg1 & ~0x8000u; /* offNoResetFlg1(daPyFlg1_SOUP_POWER_UP) */
            if (dComIfGs_getSelectEquip(0) != 0x3E /* !checkFinalMasterSwordEquip() */) {
                mNoResetFlg1 = mNoResetFlg1 & ~0x200000u;
            }
        }
        return TRUE;
    }
    setDamageRupee(amount); // HD: GameCube seStartMapInfo(JA_SE_LK_MGC_SHIELD_DEF)
    return FALSE;
}

/* 023F52D0 */
cXyz* daPy_lk_c::getDamageVec(dCcD_GObjInf* hitObj) {

    u32 obj = gabi::ea(hitObj);
    cXyz* vec = &hitObj->mGObjTg.mRVec; /* GetTgRVecP() */
    cXyz xz;                            /* abs2XZ() */
    xz.x = vec->x;
    xz.y = 0.0f;
    xz.z = vec->z;
    f64 mag = LinkActionApi::squareMagnitude(&xz);
    if (mResetFlg0 & LinkActionConstants::daPyRFlg0_UNK80000) {
        f32 sn = cM_ssin(shape_angle.y);
        f32 cs = cM_scos(shape_angle.y);
        vec->y = 0.0f;
        vec->x = -10.0f * sn;
        vec->z = -10.0f * cs;
    } else if (mag < (f64)0.1f) {
        cXyz d;
        cXyz v;
        u32 ac = LinkActionApi::getCollisionActor(obj + 0x94);
        if (ac != 0 &&
            (gabi::at<LinkActionDamageActor>(ac)->mDamageActorType == 0x1A8 ||
             (ac != 0 && gabi::at<LinkActionDamageActor>(ac)->mDamageActorType == 0x87))) {
            // HD: for these two actors the direction comes from the actor's position
            cXyz_mi(&current.pos, &d, &gabi::at<fopAc_ac_c>(ac)->current.pos);
        } else {
            cXyz_mi(&current.pos, &d, &hitObj->mGObjTg.mHitPos /* GetTgHitPosP() */);
        }
        linkCopyPositionWords(gabi::ea(&v), &d);
        f64 m2 = LinkActionApi::squareMagnitude(&v);
        if (m2 < (f64)0.1f) {
            f32 sn = cM_ssin(shape_angle.y);
            f32 cs = cM_scos(shape_angle.y);
            vec->y = 0.0f;
            vec->x = -10.0f * sn;
            vec->z = -10.0f * cs;
        } else {
            f64 len = LinkActionApi::sqrtWide(m2);
            PSVECScale(&v, &v, (f32)(10.0 / len));
            vec->y = v.y;
            vec->x = v.x;
            vec->z = v.z;
        }
    }
    return vec;
}

/* 023F54E8 */
BOOL daPy_lk_c::procLargeDamage_init(int type, int large, s16 rootX, s16 rootZ) {

    if (mCurProc == LinkActionConstants::daPyProc_LARGE_DAMAGE_e) {
        return FALSE;
    }
    commonProcInit(0x68);
    mDamageWaitTimer = mDemo.mDemoMode == 9 /* DEMO_LDAM_e */ ? 0 : 0x1E;
    int dir;
    if (type == -4) {
        mProcVar0 = 5;
        type = -1;
    } else {
        mProcVar0 = 0;
    }
    if (type == -5) {
        if (linkIsStartStage(gabi::guest_addr("kinBOSS")) ||
            linkIsStartStage(gabi::guest_addr("Xboss1"))) {
            u32 mtx =
                linkGetAnmMtx(gabi::ea(mpCLModel.get()), LinkActionConstants::CL_JNT_CENTER_e);
            LinkActionApi::makeEmitter(&m32E4, LinkActionConstants::ID_AK_SN_BKMHAKIDASHIHOUSHI00,
                                       mtx, &current.pos, nullptr);
        }
        linkStartShockUp();
        dir = 0;
    } else if (type == -3) {
        dir = mDemo.mParam0;
        if (dir == 0) {
            current.angle.y = shape_angle.y;
        } else if (dir == 2) {
            current.angle.y = (s16)(shape_angle.y + 0x4000);
        } else if (dir == 3) {
            current.angle.y = (s16)(shape_angle.y - 0x4000);
        } else {
            current.angle.y = (s16)(shape_angle.y + 0x8000);
        }
    } else if (type == -2 || type == -9) {
        s16 d;
        if (type == -2) {
            current.angle.y = m3550;
            setDamagePoint(-1.0f);
            d = (s16)(current.angle.y - shape_angle.y);
        } else {
            cXyz* v = getDamageVec(&mCyl);
            s16 a = cM_atan2s(v->x, v->z);
            d = (s16)(a - shape_angle.y);
            current.angle.y = a;
        }
        if (std::fabs((f32)d) < 8192.0f) {
            d = d < 0 ? -0x2000 : 0x2000;
            current.angle.y = (s16)(d + shape_angle.y);
        }
        dir = getDirectionFromAngle(d);
        seStartOnlyReverb(LinkActionConstants::JA_SE_LK_DAMAGE_LARGE);
        linkStartShockUp();
    } else if (type == -1 || type == -6) {
        cXyz* v = getDamageVec(&mCyl);
        s16 a = cM_atan2s(v->x, v->z);
        current.angle.y = a;
        dir = getDirectionFromAngle((s16)(a - shape_angle.y));
        seStartOnlyReverb(0x282E);
        linkStartShockUp();
    } else if (type == -10) {
        dir = getDirectionFromAngle((s16)(current.angle.y - shape_angle.y));
    } else if (type == -7) {
        dir = 0;
        u16 sy = (u16)shape_angle.y;
        current.angle.y = (s16)sy;
        current.pos.x = gabi::fnmsubs(888.8888549804688f, cM_ssin(sy), current.pos.x);
        current.pos.z = gabi::fnmsubs(888.8888549804688f, cM_scos(sy), current.pos.z);
        mDamageWaitTimer = 0;
        mAcch.m_flags = mAcch.m_flags | 0x4004;
    } else {
        if (type == 0x60) {
            dir = 1;
        } else if (type == 0x5D) {
            dir = 2;
        } else if (type == 0x5E) {
            dir = 3;
        } else {
            dir = 0;
        }
        setOldRootQuaternion((s32)rootX, 0, (s32)rootZ);
    }
    if (dir == 0) {
        mProcVar6 = 0x5C; /* ANM_DAMFB */
        mProcVar2 = 0x3FFF;
        mProcVar3 = 4;
        shape_angle.y = current.angle.y;
    } else if (dir == 3) {
        mProcVar6 = 0x59; /* ANM_DAMFL */
        mProcVar2 = 0x3FFF;
        mProcVar3 = 0;
        shape_angle.y = (s16)(current.angle.y + 0x4000);
    } else if (dir == 2) {
        mProcVar6 = 0x5A; /* ANM_DAMFR */
        mProcVar2 = -0x3FFF;
        mProcVar3 = 0;
        shape_angle.y = (s16)(current.angle.y - 0x4000);
    } else {
        mProcVar6 = 0x5B; /* ANM_DAMFF */
        mProcVar2 = -0x3FFF;
        mProcVar3 = 4;
        shape_angle.y = (s16)(current.angle.y + 0x8000);
    }
    if (large != 0) {
        mMaxNormalSpeed = 25.0f;
        gravity = -13.0f;
        mProcVar3 = mProcVar3 | 8;
        mProcVar4 = 0x514;
        m35A0 = 16.0f;
    } else {
        mMaxNormalSpeed = 80.0f;
        gravity = -9.0f;
        m35A0 = 16.0f;
        mProcVar4 = 0x7D0;
    }
    setSingleMoveAnime((s32)mProcVar6, 1.0f, 0.0f, -1, 0.0f);
    if (type < 0) {
        if (type != -7) {
            LinkActionVoiceStart(this, 3);
        }
        if (type != -5) {
            if (mProcVar0 > 0) {
                mNormalSpeed = 0.0f;
                speed.y = 0.0f;
            } else if (large != 0) {
                mNormalSpeed = 25.0f;
                speed.y = 60.0f;
            } else {
                mNormalSpeed = 80.0f;
                speed.y = 50.0f;
            }
        }
    } else {
        mNormalSpeed = 16.0f;
        u16 ay = (u16)current.angle.y;
        current.pos.x = gabi::fmadds(35.0f, cM_ssin(ay), current.pos.x);
        current.pos.z = gabi::fmadds(35.0f, cM_scos(ay), current.pos.z);
        speed.y = 0.0f;
    }
    mNoResetFlg0 = mNoResetFlg0 & ~2u; /* offNoResetFlg0(daPyFlg0_UNK2) */
    mBodyAngle.x = 0;                  /* mBodyAngle */
    mBodyAngle.y = 0;
    mBodyAngle.z = 0;
    if (type == -7) {
        mProcVar3 = mProcVar3 | 2;
        mProcVar6 = -4;
    }
    return TRUE;
}

/* 023F5EAC */
void daPy_lk_c::dProcFreezeDamage_init_sub(int event) {

    if (event != 0) {
        mProcVar6 = 1;
        mDemo.mDemoType = 5;
        LinkActionApi::changePlayer(this, this);
    }
    endDamageEmitter();
    mNoResetFlg1 = (mNoResetFlg1 | LinkActionConstants::daPyFlg1_FREEZE_STATE) &
                   ~LinkActionConstants::daPyFlg1_UNK40000;
    LinkActionApi::fadeOut(0x101CEAFC /* l_freeze_fade_color */, 1.0f);
    seStartMapInfo(LinkActionConstants::JA_SE_LK_FREEZE);
    cXyz pos;
    for (int i = 0; i < 5; i++) {
        u16 jnt = eff_joint[i]; /* eff_joint */
        u32 mtx = linkGetAnmMtx(gabi::ea(mpCLModel.get()), jnt);
        pos.x = gabi::at<Mtx34>(mtx)->m[0][3]; /* mDoMtx_multVecZero */
        pos.y = gabi::at<Mtx34>(mtx)->m[1][3];
        pos.z = gabi::at<Mtx34>(mtx)->m[2][3];
        u32 pa = gabi::ea(LinkActionPlay().mParticle.get()); /* dComIfGp_particle_setP1 */
        LinkActionApi::setParticle(pa, 1, LinkActionConstants::ID_IT_SN_LK_FREEZSMOKE00, &pos, 0, 0,
                                   0xFF, 0, -1, 0, 0, 0);
        if (i >= 3) {
            pa = gabi::ea(LinkActionPlay().mParticle.get());
            LinkActionApi::setParticle(pa, 1, LinkActionConstants::ID_IT_SN_LK_FREEZKIRA00, &pos, 0,
                                       0, 0xFF, 0, -1, 0, 0, 0);
        }
    }
}

/* 023F6020 */
BOOL daPy_lk_c::procLargeDamageUp_init(int type, int large, s16 rootX, s16 rootZ) {

    if (mCurProc == LinkActionConstants::daPyProc_LARGE_DAMAGE_UP_e) {
        return FALSE;
    }
    commonProcInit(0x69);
    int anm;
    f32 rate, start, morf;
    s32 stop;
    if (type == -3) {
        mProcVar0 = 0;
        m35A0 = 36.0f;
        anm = 0x5F; /* ANM_DAMFFUP */
        start = 0.0f;
        rate = 0.5f;
        morf = 5.0f;
        stop = -1;
    } else if (type == -1 || type == -2) {
        anm = 0x60; /* ANM_DAMFBUP */
        start = 7.0f;
        morf = 2.0f;
        stop = -1;
        m35A0 = 36.0f;
        if (type == -1) {
            mProcVar0 = 0x1E;
            rate = 0.0f;
        } else {
            mProcVar0 = 0;
            rate = 0.5f;
        }
    } else if (type == -4 || LinkActionApi::startCheckOld(dComIfGp_ea() + 0x52C4,
                                                          gabi::guest_addr("ICE_FAILED"))) {
        anm = 0x60;
        rate = 0.0f;
        start = 7.0f;
        stop = 0x24;
        m35A0 = 34.0f;
        morf = 2.0f;
        if (type == -4) {
            mProcVar0 = 0x28;
        } else {
            mProcVar0 = 30000;
            dProcFreezeDamage_init_sub(0);
        }
        mFootEffectPosType = 6;
        linkStartShockUp();
        seStartMapInfo(LinkActionConstants::JA_SE_LK_FALL_DOWN);
    } else {
        mProcVar0 = 0;
        mFootEffectPosType = 6;
        linkStartShockUp();
        seStartMapInfo(0x3811);
        morf = 2.0f;
        stop = 0x24;
        if (type == LinkActionConstants::ANM_DAMFB) {
            anm = 0x60;
            start = 0.0f;
            if (large != 0) {
                m35A0 = 30.0f;
                rate = 0.8f;
            } else {
                m35A0 = 34.0f;
                rate = 0.6f;
            }
        } else if (type == LinkActionConstants::ANM_DAMFL ||
                   type == LinkActionConstants::ANM_DAMFR) {
            anm =
                type == 0x59 ? LinkActionConstants::ANM_DAMFLUP : LinkActionConstants::ANM_DAMFRUP;
            start = 1.0f;
            if (large == 0) {
                m35A0 = 34.0f;
                rate = 0.6f;
            } else {
                m35A0 = 30.0f;
                rate = 0.7f;
            }
        } else {
            anm = 0x5F;
            start = 0.0f;
            if (large == 0) {
                m35A0 = 34.0f;
                rate = 0.6f;
            } else {
                m35A0 = 30.0f;
                rate = 0.7f;
            }
        }
    }
    m35E4 = 1.0f;
    if (dComIfGs_getLife() /* dComIfGs_getLife() */ == 0) {
        stop = -1;
    }
    m35A4 = 2.0f / (m35A0 - start);
    setSingleMoveAnime(anm, rate, start, stop, morf);
    setOldRootQuaternion((s32)rootX, 0, (s32)rootZ);
    mProcVar6 = type;
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    return TRUE;
}

/* 023F6564 */
BOOL daPy_lk_c::procFall_init(int type, f32 morf) {

    if (mDemo.mDemoMode == 0x11 /* DEMO_PFALL_e */) {
        mAcch.m_flags = mAcch.m_flags | 0x4002; /* SetGrndNone(); OnLineCheckNone() */
    }
    s32 proc = mCurProc;
    BOOL notShipOff = proc != LinkActionConstants::daPyProc_SHIP_GET_OFF_e;
    if (proc == LinkActionConstants::daPyProc_FALL_e) {
        return FALSE;
    }
    BOOL slide = proc == 0x1A /* SLIDE_FRONT */ || proc == 0x1B /* SLIDE_BACK */;
    commonProcInit(0x27);
    if (type == 1) {
        mNormalSpeed = 0.0f;
        current.angle.y = shape_angle.y;
        speed.y = 0.0f;
    }
    mProcVar3 = type == 2;
    setSingleMoveAnime(LinkActionConstants::ANM_JMPEDS, 0.0f, 0.0f, -1, morf);
    setTextureAnime(0x37, 0);
    resetSeAnime();
    mProcVar6 = 0;
    if (current.angle.y == shape_angle.y && LinkAction(this).m_anm_heap_upper[2].mIdx != 0x95 &&
        LinkAction(this).m_anm_heap_upper[2].mIdx != 0x96 /* !checkGrabAnime() */) {
        if (mNormalSpeed > 1.0f) {
            mProcVar0 = 2;
        } else {
            mProcVar0 = 1;
        }
    } else {
        mProcVar0 = 0;
    }
    mProcVar1 = 8;
    if (cLib_distanceAngleS(current.angle.y, shape_angle.y) < 0x4800 && !slide) {
        mProcVar2 = 1;
    } else {
        mProcVar2 = 0;
    }
    mProcVar7 = notShipOff;
    mNoResetFlg0 = mNoResetFlg0 & ~0x40000u; /* offNoResetFlg0(daPyFlg0_NO_FALL_VOICE) */
    return TRUE;
}

/* 023F6794 */
BOOL daPy_lk_c::procSlowFall_init() {

    if (mCurProc == LinkActionConstants::daPyProc_SLOW_FALL_e) {
        return TRUE;
    }
    commonProcInit(0x28);
    maxFallSpeed = -20.0f;
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(LinkActionConstants::ANM_MSTEPOVER_JMPED, 0.0f, 0.0f, -1, 2.4f);
    f32 f = (f32)mFrameCtrlUnder[0].mEnd - 0.001f;
    mFrameCtrlUnder[0].mFrame = f;
    *LinkAction(this).mAnmRatioUnder[0].mpAnmFrame.get() = f; /* ->setFrame() */
    return TRUE;
}

/* 023F684C */
BOOL daPy_lk_c::dProcLookWait_init() {

    int use = checkShipRideUseItem(0);
    if (mCurProc == LinkActionConstants::daPyProc_DEMO_LOOK_WAIT_e) {
        return FALSE;
    }
    if (mCurProc == LinkActionConstants::daPyProc_GRAB_WAIT_e) {
        linkSetJointBlendRatio(m_pbCalc[1], 2, 1.0f);
    }
    commonProcInit(0xBE);
    mNormalSpeed = 0.0f;
    setBlendMoveAnime(2.4f);
    mDirection = 4; /* DIR_NONE */
    current.angle.y = shape_angle.y;
    initShipRideUseItem(use, 0);
    return TRUE;
}

/* GHS pointer-to-member call returning r3 (see ptmf_call in bindings.h) */
static inline BOOL linkCallDemoProc(const LinkDemoEntry& storage, daPy_lk_c* self) {
    const auto& entry = gabi::object_at<gabi::ptmf<LinkDemoProc>>(gabi::ea(&storage));
    // GHS reads the virtual index before the this adjustment.
    s16 idx = entry.i;
    s16 delta = entry.d;
    u32 adjusted = gabi::ea(self) + delta;
    if (idx < 0) {
        return gabi::call_ptr<BOOL>(entry.f, gabi::at<daPy_lk_c>(adjusted));
    }
    // The low half of f is the vtable pointer offset for a virtual entry.
    s16 vtableOffset = *gabi::at<be<s16>>(gabi::ea(&entry.f) + 2);
    gptr<be<u32>>* vtable = gabi::at<gptr<be<u32>>>(adjusted + vtableOffset);
    return gabi::call_ptr<BOOL>(vtable->get()[idx * 2 + 1], gabi::at<daPy_lk_c>(adjusted));
}

#define LK_onDemoFlg()                                                                             \
    (mNoResetFlg0 = mNoResetFlg0 | 0x100000 /* onNoResetFlg0(daPyFlg0_UNK100000) */)

/* 023F695C */
BOOL daPy_lk_c::changeDemoProc() {

    u32 mode = (u32)mDemo.mDemoMode;
    if (!dComIfGp_event_runCheck()) {
        return FALSE;
    }
    u16 eq = mEquipItem;
    if (mNoResetFlg1 & LinkActionConstants::daPyFlg1_SHIP_TACT) {
        if (eq != LinkActionConstants::daPyItem_UNK10A_e) {
            deleteEquipItem(0);
            setTactModel();
            u32 model = gabi::ea(LinkAction(this).mpEquipItemModel.get());
            mEquipItem = 0x10A;
            gabi::at<J3DModel>(model)->mBaseScale.x = 2.5f; /* setBaseScale(tact_scale) */
            gabi::at<J3DModel>(model)->mBaseScale.z = 2.5f;
            gabi::at<J3DModel>(model)->mBaseScale.y = 2.5f;
        }
    } else if (eq == 0x10A) {
        deleteEquipItem(0);
    }
    if (mode >= 0x4B /* DEMO_LAST_e */) {
        if (mode != 0x200 /* DEMO_NEW_ANM0_e */) {
            LinkActionApi::assertionFailure(
                gabi::guest_addr("d_a_player_main.cpp"), 0x2B35,
                gabi::guest_addr("(demo_mode < daPy_demo_c::DEMO_LAST_e) || (demo_mode == "
                                 "daPy_demo_c::DEMO_NEW_ANM0_e)"));
        }
    }
    if (mode == 0x34) {
        // HD: the message-bottle demo: start the throw or the letter proc, or cancel it in midair
        if (((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !(mNoResetFlg0 & 0x20000000) &&
             !LK_checkPlayerFly()) ||
            dComIfGp_checkPlayerStatus0(0, 0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
            u32 proc = (u32)mCurProc;
            if (proc >= 0xDB && proc <= 0xDD) {
                return FALSE;
            }
            if (LinkAction(this).mBottleThrowPending != 0) {
                return dProcHDBottleThrow_init();
            }
            return dProcHDLetterWrite_init();
        }
        u32 body = LinkActionApi::getCameraBody();
        LinkActionApi::endEventCamera(body, mBsPcId);
        if (LinkAction(this).mThrowActor != 0) {
            u32 id = LinkAction(this).mThrowActor != 0
                         ? gabi::at<fopAc_ac_c>(LinkAction(this).mThrowActor)->mBsPcId
                         : 0xFFFFFFFF;
            body = LinkActionApi::getCameraBody();
            LinkActionApi::endEventCamera(body, id);
            id = LinkAction(this).mThrowActor != 0
                     ? gabi::at<fopAc_ac_c>(LinkAction(this).mThrowActor)->mBsPcId
                     : 0xFFFFFFFF;
            body = LinkActionApi::getCameraBody();
            LinkActionApi::forceCameraLockOff(body, id);
            body = LinkActionApi::getCameraBody();
            LinkActionApi::resetCamera(body);
            body = LinkActionApi::getCameraBody();
            LinkActionApi::startCamera(body);
            gabi::at<LinkActionThrownItem>(LinkAction(this).mThrowActor)->mMode = 3;
            LinkAction(this).mThrowActor = 0;
        } else if (LinkAction(this).mBottleThrowPending != 0) {
            u32 item = LinkActionApi::fastCreateItem2(&current.pos, 0x13, -1, -1, 0, 0, 0, 0);
            gabi::at<LinkActionThrownItem>(item)->mMode = 3;
            LinkAction(this).mThrowActor = 0;
        }
        LinkAction(this).mBottleThrowPending = 0;
        auto& eventFlags = LinkActionPlay().mEvtCtrl.mEventFlag; /* the event flags */
        eventFlags = (u16)eventFlags | 8;
        endDemoMode();
        return changeWaitProc();
    }
    if (mode == 0x200) {
        LK_onDemoFlg();
        return dProcTool_init();
    }
    // HD: the modes that skip the ground check go straight to their handlers
    if (mode == 0x1E || mode == 0x2F || mode == 0x3E) {
        LK_onDemoFlg();
        return linkCallDemoProc(mDemoProcInitFuncTable[mode], this);
    }
    const bool directHandler = mode == 9 || mode == 0x11 || mode == 0x40 || mode == 4;
    if (!directHandler) {
        if (!dComIfGp_checkPlayerStatus0(0, 0x10000) && !dComIfGp_checkPlayerStatus0(0, 0x100000)) {
            if (!(mAcch.m_flags & dBgS_Acch::GROUND_HIT) || LK_checkPlayerFly()) {
                return FALSE;
            }
        }
        if (mDemoProcInitFuncTable[mode].i != 0) {
            LK_onDemoFlg();
            return linkCallDemoProc(mDemoProcInitFuncTable[mode], this);
        }
    }
    if (mode == 4 /* DEMO_INIT_WAIT_e */) {
        if (dComIfGp_checkPlayerStatus0(0, 0x10000)) {
            return TRUE;
        }
        BOOL r = procWait_init();
        if (r) {
            LinkActionApi::initOldFrameMorf((u32)m_old_fdata, 0.0f, 0, 0x2A);
        }
        return r;
    }
    if (mode == 6 /* DEMO_N_TALK_e */ || mode == 8 /* DEMO_E_TALK_e */) {
        if (mModeFlg & 0x04000000) {
            return TRUE;
        }
        if (dComIfGp_checkPlayerStatus0(0, 0x10000) &&
            LinkActionApi::getTalkEventPartner(this) != 0) {
            u32 ship = gabi::ea(dComIfGp_getShipActor());
            if (LinkActionApi::getTalkEventPartner(this) == ship) {
                return procShipPaddle_init();
            }
        }
        LK_onDemoFlg();
        u32 grab = gabi::ea(mActorKeepGrab.mActor.get());
        if (grab != 0) {
            if ((gabi::at<fopAc_ac_c>(grab)->actor_status & 0x10000 /* fopAcStts_UNK10000_e */) &&
                !checkGrabSpecialHeavyState()) {
                return procGrabThrow_init(0);
            }
            return LK_callB(0x023EA054 /* procGrabPut_init */);
        }
        return dProcTalk_init();
    }
    if (mode == 9 /* DEMO_LDAM_e */) {
        LK_onDemoFlg();
        if (mCurProc == 0x69 /* LARGE_DAMAGE_UP */ || mCurProc == 0x6A /* LARGE_DAMAGE_WALL */) {
            return FALSE;
        }
        if (mModeFlg & 0x40000 /* ModeFlg_SWIM */) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
            return FALSE;
        }
        return procLargeDamage_init(-3, 1, 0, 0);
    }
    if (mode == 0x1B /* DEMO_UNK_027_e */) {
        LK_onDemoFlg();
        return procLargeDamageUp_init(-3, 1, 0, 0);
    }
    if (mode == 0x10 /* DEMO_BJUMP_e */) {
        LK_onDemoFlg();
        if (mCurProc == LinkActionConstants::daPyProc_BACK_JUMP_LAND_e) {
            return FALSE;
        }
        return procBackJump_init();
    }
    if (mode == 0x11 /* DEMO_PFALL_e */) {
        LK_onDemoFlg();
        return procFall_init(1, 6.0f);
    }
    if (mode == 0x40 /* DEMO_SFALL_e */) {
        if (mCurProc == LinkActionConstants::daPyProc_LAND_e) {
            return TRUE;
        }
        return procSlowFall_init();
    }
    if (mode == 0x1F /* DEMO_LWAIT_e */) {
        BOOL look;
        if (!(mModeFlg & 0x14452822)) {
            if (checkUpperReadyThrowAnime()) {
                look = TRUE;
            } else {
                u8 st = m34C3;
                look = (st >= 1 && st <= 2) || st == 4 || (st >= 9 && st <= 10) ||
                       mCurProc == LinkActionConstants::daPyProc_GRAB_WAIT_e;
            }
        } else {
            look = FALSE;
        }
        if (look || dComIfGp_checkPlayerStatus0(0, 0x10000)) {
            LK_onDemoFlg();
            return dProcLookWait_init();
        }
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    }
    if (mode == 0x22 /* DEMO_TACT_e */) {
        LK_onDemoFlg();
        if (mCurProc == 0x9B /* TACT_PLAY */ || mCurProc == 0x9C /* TACT_PLAY_END */) {
            return TRUE;
        }
        return procTactWait_init(mDemo.mParam0);
    }
    if (mode == 0x33 /* DEMO_UNK_051_e */) {
        bool change = mActorKeepGrab.mActor.get() == nullptr;
        if (!change) {
            u32 play = dComIfGp_ea();
            change =
                gabi::ea(mActorKeepGrab.mActor.get()) !=
                gabi::ea(LinkActionPlay(play).mpPlayerPtr[1].get()) /* dComIfGp_getCb1Player() */;
        }
        if (change) {
            deleteEquipItem(0);
            freeGrabItem();
            u32 cb1 = gabi::ea(LinkActionPlay().mpPlayerPtr[1].get());
            LinkActionApi::actorKeepSetData(&mActorKeepGrab, cb1);
            LinkActionApi::setCarryNow(gabi::ea(mActorKeepGrab.mActor.get()), 1);
        }
        return LK_callB(0x023E2484 /* procGrabWait_init */);
    }
    u32 flg0 = mNoResetFlg0;
    if (mode == 0x3D /* DEMO_BO_OPEN_e */) {
        mNoResetFlg0 = flg0 | 0x100000;
        return procBottleOpen_init(dComIfGp_event_getPreItemNo());
    }
    if (!(flg0 & 0x100000) && mCurProc != LinkActionConstants::daPyProc_SUBJECTIVITY_e &&
        !checkUpperReadyThrowAnime()) {
        return FALSE;
    }
    if (!((mode >= 1 && mode <= 3) || mode == 0x12 || mode == 0x17 || mode == 0x2A)) {
        return FALSE;
    }
    mNoResetFlg0 = mNoResetFlg0 & ~0x100000u;
    if (checkUpperReadyThrowAnime()) {
        resetActAnimeUpper(2, -1.0f);
    }
    if (dComIfGp_checkPlayerStatus0(0, 0x10000)) {
        return procShipPaddle_init();
    }
    return checkNextMode(0);
}

static u32 l_splashScaleInitialized;
static u32 l_rippleScaleInitialized;
static u32 l_armFlameScaleInitialized;
static cXyz l_splashScale;
static cXyz l_rippleScale;
static cXyz l_armFlameScale;
static const u16 l_flameJoint[4] = {15, 3, 7, 11};
static const cXyz l_deadRippleScale = {0.75f, 0.75f, 0.75f};

/* 023F73B8 */
fopAc_ac_c *daPy_lk_c::makeFairy(cXyz *pos, u32 prm) {

    setResetFlg0(resetFlg0() | LinkMotionConstants::daPyRFlg0_FAIRY_USE);
    resetCurse();
    return LinkMotionApi::fastCreate(0x168, prm, pos, (s32)current.roomNo, &shape_angle, nullptr,
                                     -1, 0, 0);
}

/* 023F7434 */
void daPy_lk_c::dProcDead_init_sub() {

    mProcVar6 = 0;
    dComIfGp_setPlayerStatus0(0, LinkMotionConstants::daPyStts0_UNK20000000_e);
    mGameOverId = LinkMotionApi::createAppend(0x1E3, 0, 0, 0, 0, 0);
    LK_demoType = 5;
    LinkActionApi::changePlayer(this, this);
}

/* 023F74B0 */
void daPy_lk_c::dProcDead_init_sub2() {

    mFrameCtrlUnder[0].mRate = 1.0f;
    LinkActionVoiceStart(this, 0x16);
    LinkMotionApi::bgmStop(0);
    LinkMotionApi::bgmStart(0x8000000A);
    u32 body = LinkActionApi::getCameraBody();
    LinkMotionApi::startEventCamera(body, 9, (u32)mBsPcId, 0);
    if (dComIfGp_checkPlayerStatus0(0, LinkMotionConstants::daPyStts0_SWIM_e)) {
        u32 pa = (u32)LinkActionPlay().mParticle.v; /* dComIfGp_particle_setShipTail */
        LinkActionApi::setParticle(pa, 5, LinkMotionConstants::ID_IT_JN_WP_HAMON03, &current.pos, 0,
                                   gabi::ea(&l_deadRippleScale), 0xFF,
                                   linkMotionSingleRippleCallback(), -1, 0, 0, 0);
        mProcVar3 = 1;
    }
}

/* 023F7580 */
BOOL daPy_lk_c::dProcDead_init() {

    if (mCurProc == LinkMotionConstants::daPyProc_DEMO_DEAD_e) {
        return TRUE;
    }
    int use = checkShipRideUseItem(0);
    u32 swim = mModeFlg & LinkMotionConstants::ModeFlg_SWIM;
    u32 flg100 = mNoResetFlg0 & LinkMotionConstants::daPyFlg0_UNK100;
    if (swim) {
        swimOutAfter(0);
    }
    commonProcInit(0xB2);
    f32 rate;
    if (linkOrderCompulsoryEvent(this)) {
        dProcDead_init_sub();
        mNormalSpeed = 0.0f;
        rate = 1.0f;
        mProcVar2 = 0;
    } else {
        mProcVar2 = 0;
        mNormalSpeed = 0.0f;
        rate = 0.0f;
        mProcVar6 = 1;
    }
    int anm;
    if (use != 0) {
        anm = 0x63; /* ANM_SHIPDIE */
    } else if (swim) {
        anm = 0x64; /* ANM_SWIMDIE */
        mModeFlg = mModeFlg | 0x40000;
        dComIfGp_setPlayerStatus0(0, LinkMotionConstants::daPyStts0_SWIM_e);
        if (flg100 == 0) {
            rate = 0.0f;
            mProcVar2 = 1;
            mNoResetFlg0 = mNoResetFlg0 & ~0x100u;
        } else {
            m34C2 = 0;
        }
    } else {
        anm = 0x62; /* ANM_DIELONG */
        mModeFlg = mModeFlg | 0x02000000;
    }
    setSingleMoveAnime(anm, rate, 0.0f, -1, 2.0f);
    m35E4 = 0.0f;
    mDamageWaitTimer = 0;
    m35A0 = 1.0f;
    if (mProcVar2 == 0) {
        speed.y = 0.0f;
    }
    gravity = 0.0f;
    mProcVar7 = 0;
    initShipRideUseItem(use, 1);
    mProcVar3 = 0;
    if (rate > 0.0f) {
        dProcDead_init_sub2();
    }
    cancelNoDamageMode();
    return TRUE;
}

/* 023F7820 */
BOOL daPy_lk_c::changeDeadProc() {

    if (!dComIfGp_event_runCheck() && LK_demoType == 0 /* !checkPlayerDemoMode() */ &&
        (u16)LinkActionSave().mSavedata.mPlayer.mPlayerStatusA.mLife /* dComIfGs_getLife() */ ==
            0 &&
        !(mModeFlg & LinkMotionConstants::ModeFlg_DAMAGE)) {
        if (LinkMotionApi::checkBottle(gabi::ea(&LinkActionSave().mSavedata.mPlayer.mPlayerItem),
                                       0x57)) {
            makeFairy(&current.pos, 5 /* daNpc_Fa1_c::Type_LINK_DOWN_e */);
            LinkMotionApi::setBottleItemIn(
                gabi::ea(&LinkActionSave().mSavedata.mPlayer.mPlayerItem), 0x57, 0x50);
            return FALSE;
        }
        if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) || dComIfGp_checkPlayerStatus0(0, 0x10000) ||
            (mModeFlg & 0x40000)) {
            return dProcDead_init();
        }
        return procFall_init(1, 6.0f);
    }
    return FALSE;
}

/* 023F7958 */
BOOL daPy_lk_c::procAutoJump_init() {

    commonProcInit(LinkMotionConstants::daPyProc_AUTO_JUMP_e);
    f32 z = current.pos.z;
    f32 y = current.pos.y;
    m3688.z = z;
    m3688.y = y;
    m35F4 = y;
    m35F0 = y;
    m3688.x = current.pos.x;
    setSingleMoveAnime(LinkMotionConstants::ANM_JMPST, 0.8f, 1.0f, 5, 1.0f);
    f32 sp = speedF;
    if (sp > 17.0f) {
        sp = 17.0f;
        speedF = sp;
    } else if (sp < 9.0f) {
        sp = 9.0f;
        speedF = sp;
    }
    mNormalSpeed = (sp * cM_scos(0x2AF8)) * 1.6f;
    speed.y = (speedF * cM_ssin(0x2AF8)) * 1.6f;
    mProcVar6 = 0;
    linkCopyPositionWords(gabi::ea(&m3700), &cXyz::Zero /* cXyz::Zero */);
    m34C2 = 1;
    current.angle.y = shape_angle.y;
    LinkActionVoiceStart(this, 6);
    mProcVar1 = 3;
    mProcVar0 = -1;
    mNoResetFlg0 = (mNoResetFlg0 | LinkMotionConstants::daPyFlg0_UNK20000) &
                   ~0x40000u /* daPyFlg0_NO_FALL_VOICE */;
    return TRUE;
}

/* 023F7AB0 */
BOOL daPy_lk_c::procClimbDownStart_init(s16 angle) {

    commonProcInit(LinkMotionConstants::daPyProc_CLIMB_DOWN_START_e);
    gravity = 0.0f;
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    speedF = 0.0f;
    deleteEquipItem(1);
    setSingleMoveAnime(LinkMotionConstants::ANM_LADDERDWST, 1.3f, 30.0f, -1, 3.0f);
    dComIfGp_setPlayerStatus1(0, LinkMotionConstants::daPyStts1_UNK10000_e);
    setOldRootQuaternion(0, -0x8000, 0);
    s16 a = (s16)(angle + 0x8000);
    current.angle.y = a;
    shape_angle.y = a;
    current.pos.x = gabi::fnmsubs(35.0f, cM_ssin((u16)angle), current.pos.x);
    current.pos.y = current.pos.y - 20.0f;
    current.pos.z = gabi::fnmsubs(35.0f, cM_scos((u16)angle), current.pos.z);
    mProcVar6 = 1;
    m35E0 = 43.67353f;
    return TRUE;
}

/* cBgS::GetTriPla(bg, poly) of mLinkLinChk's poly info (+0x14); the plane normal at +0 */
static inline u32 lk_linChkTriPla(u32 me) {
    u32 play = dComIfGp_ea();
    return LinkMotionApi::getTriPla(
        gabi::at<dBgS>(play + 0x12A0),
        (u32)(u16)linkMotionQueries(me).mLinkLinChk.mPolyInfo.mBgIndex,
        (u32)(u16)linkMotionQueries(me).mLinkLinChk.mPolyInfo.mPolyIndex);
}

/* 023F7BD4 */
BOOL daPy_lk_c::procHangFallStart_init(void *pla /* cM3dGPla* */) {

    u32 me = gabi::ea(this);
    u32 np = gabi::ea(pla);
    s16 a =
        cM_atan2s((f32)gabi::at<cM3dGPla>(np)->mNormal.x, (f32)gabi::at<cM3dGPla>(np)->mNormal.z);
    f32 x = gabi::fnmsubs(1.5f, (f32)gabi::at<cM3dGPla>(np)->mNormal.x, current.pos.x);
    f32 y = current.pos.y;
    current.pos.x = x;
    f32 z = gabi::fnmsubs(1.5f, (f32)gabi::at<cM3dGPla>(np)->mNormal.z, current.pos.z);
    current.pos.z = z;
    u16 side = (u16)(a - 0x4000);
    f32 dx = 30.0f * cM_ssin(side);
    f32 dz = 30.0f * cM_scos(side);
    gabi::Local<cXyz> p0;
    gabi::Local<cXyz> p1;
    p0->x = gabi::fmadds(4.5f, cM_ssin((u16)a), x);
    p0->y = y - 62.5f;
    p0->z = gabi::fmadds(4.5f, cM_scos((u16)a), z);
    p1->x = p0->x - dx;
    p1->y = p0->y;
    p1->z = p0->z - dz;
    void *chk = mLinkLinChk;
    dBgS_LinChk_Set(chk, p0, p1, this);
    u32 tp;
    if (cBgS_LineCross(dComIfG_Bgsp(), chk) && (tp = lk_linChkTriPla(me)) != 0 &&
        LinkActionApi::distanceAngleS((s32)cM_atan2s((f32)gabi::at<cM3dGPla>(tp)->mNormal.x,
                                                     (f32)gabi::at<cM3dGPla>(tp)->mNormal.z),
                                      (s32)shape_angle.y) < 0x549F) {
        f32 cz = current.pos.z + dz;
        f32 nx = p0->x + dx;
        f32 cx = current.pos.x + dx;
        f32 nz = p0->z + dz;
        current.pos.z = cz;
        p1->y = p0->y;
        p1->x = nx + dx;
        current.pos.x = cx;
        p0->x = nx;
        p1->z = nz + dz;
        p0->z = nz;
    } else {
        p1->x = p0->x + dx;
        p1->y = p0->y;
        p1->z = p0->z + dz;
    }
    dBgS_LinChk_Set(chk, p0, p1, this);
    f32 gx, gz;
    if (cBgS_LineCross(dComIfG_Bgsp(), chk) && (tp = lk_linChkTriPla(me)) != 0 &&
        LinkActionApi::distanceAngleS((s32)cM_atan2s((f32)gabi::at<cM3dGPla>(tp)->mNormal.x,
                                                     (f32)gabi::at<cM3dGPla>(tp)->mNormal.z),
                                      (s32)shape_angle.y) < 0x549F) {
        gx = current.pos.x - dx;
        gz = current.pos.z - dz;
        f32 gy = current.pos.y + 50.0f;
        current.pos.x = gx;
        LinkAction(this).mGndChkPos.x = gx;
        LinkAction(this).mGndChkPos.z = gz;
        current.pos.z = gz;
        LinkAction(this).mGndChkPos.y = gy;
    } else {
        gx = current.pos.x;
        f32 gy = current.pos.y + 50.0f;
        gz = current.pos.z;
        LinkAction(this).mGndChkPos.x = gx;
        LinkAction(this).mGndChkPos.z = gz;
        LinkAction(this).mGndChkPos.y = gy;
    }
    f64 gc = LinkActionApi::groundCross(dComIfG_Bgsp(), mGndChk);
    f32 gy;
    if (gc == -1000000000.0) {
        gx = gx + dx;
        gy = current.pos.y + 50.0f;
        gz = gz + dz;
    } else {
        if (gc < (f64)(f32)(current.pos.y + -30.1f)) {
            return FALSE;
        }
        gx = gx + dx;
        current.pos.y = (f32)gc;
        gy = (f32)(gc + 50.0);
        gz = gz + dz;
    }
    LinkAction(this).mGndChkPos.x = gx;
    LinkAction(this).mGndChkPos.y = gy;
    LinkAction(this).mGndChkPos.z = gz;
    f64 gc2 = LinkActionApi::groundCross(dComIfG_Bgsp(), mGndChk);
    if (gc2 < (f64)(f32)(current.pos.y - 50.0f)) {
        f32 x3 = current.pos.x - dx;
        f32 z3 = current.pos.z - dz;
        LinkAction(this).mGndChkPos.y = gy;
        LinkAction(this).mGndChkPos.x = x3;
        LinkAction(this).mGndChkPos.z = z3;
        f64 gc3 = LinkActionApi::groundCross(dComIfG_Bgsp(), mGndChk);
        if (gc3 < (f64)(f32)(current.pos.y - 50.0f)) {
            return FALSE;
        }
        current.pos.y = (f32)gc3;
        current.pos.x = x3;
        current.pos.z = z3;
    }
    s16 na = (s16)(a + 0x8000);
    setOldRootQuaternion(0, (s32)(s16)(shape_angle.y - na), 0);
    s32 proc = mCurProc;
    current.angle.y = na;
    shape_angle.y = na;
    commonProcInit(LinkMotionConstants::daPyProc_HANG_FALL_START_e);
    setSingleMoveAnime(LinkMotionConstants::ANM_HANGING, 0.8f, 2.0f, 0x15,
                       proc == 0x1E /* FRONT_ROLL */ ? 0.0f : 2.0f);
    mNormalSpeed = 0.0f;
    speed.y = 0.0f;
    dComIfGp_setPlayerStatus0(0, LinkMotionConstants::daPyStts0_HANG_e);
    LinkActionVoiceStart(this, 0xB);
    return TRUE;
}

/* 023F81A4 */
BOOL daPy_lk_c::changeAutoJumpProc() {

    if ((mModeFlg & 0x10452802) || (mAcch.m_flags & dBgS_Acch::GROUND_HIT)) {
        return FALSE;
    }
    u32 me = gabi::ea(this);
    f32 y = current.pos.y;
    f32 x = current.pos.x;
    f32 z = current.pos.z;
    f32 gndDiff = mAcch.m_ground_h - y;
    linkMotionGroundCheck(mLavaGndChk).m_pos.x = x; /* mLavaGndChk.SetPos(&current.pos) */
    linkMotionGroundCheck(mLavaGndChk).m_pos.y = y;
    linkMotionGroundCheck(mLavaGndChk).m_pos.z = z;
    if (mNoResetFlg0 & LinkMotionConstants::daPyFlg0_HOVER_BOOTS) {
        mNoResetFlg0 = mNoResetFlg0 & ~0x1000u;
        return procFall_init(2, 6.0f);
    }
    if (!(gndDiff < -30.1f)) {
        return FALSE;
    }
    f32 dz = ((speed.z + m3730.z) + m36A0.z) + m36B8.z;
    f32 dx = ((speed.x + m3730.x) + m36A0.x) + m36B8.x;
    f64 len = LinkActionApi::sqrtWide(gabi::fmadds(dx, dx, dz * dz));
    f32 morf;
    if (len > (f64)0.001f) {
        s16 a = cM_atan2s(dx, dz);
        f32 ny = current.pos.y - speed.y;
        f32 cx = current.pos.x;
        f32 cz = current.pos.z;
        f32 sn = cM_ssin((u16)a);
        f32 cs = cM_scos((u16)a);
        current.pos.y = ny;
        gabi::Local<cXyz> p0;
        gabi::Local<cXyz> p1;
        p0->z = cz;
        p0->x = cx;
        p0->y = ny - 5.0f;
        p1->z = gabi::fnmsubs(50.0f, cs, cz);
        p1->x = gabi::fnmsubs(50.0f, sn, cx);
        p1->y = ny - 5.0f;
        void *chk = mLinkLinChk;
        dBgS_LinChk_Set(chk, p0, p1, this);
        u32 pla = 0;
        if (cBgS_LineCross(dComIfG_Bgsp(), chk)) {
            pla = lk_linChkTriPla(me);
        }
        f32 nspeed = mNormalSpeed;
        if ((u16)LinkActionSave().mSavedata.mPlayer.mPlayerStatusA.mLife /* dComIfGs_getLife() */ !=
                0 ||
            LinkMotionApi::checkBottle(gabi::ea(&LinkActionSave().mSavedata.mPlayer.mPlayerItem),
                                       0x57)) {
            if (m357C != 3 && !(field_0x2b0 < 0.0f) /* !checkGrabWear() */ && !(nspeed < 9.0f) &&
                mStickDistance > 0.85f && cLib_distanceAngleS(a, shape_angle.y) < 0x800) {
                if (pla != 0) {
                    current.pos.x = (f32)linkMotionQueries(me)
                                        .mLinkLinChk.mCross.x; /* mLinkLinChk.GetCrossP() */
                    current.pos.z = (f32)linkMotionQueries(me).mLinkLinChk.mCross.z;
                }
                return procAutoJump_init();
            }
            if (pla != 0 && !(std::fabs((f32)gabi::at<cM3dGPla>(pla)->mNormal.y) > 0.05f) &&
                LinkMotionApi::getWallCode(
                    dComIfG_Bgsp(), gabi::ea(&linkMotionQueries(me).mLinkLinChk.mPolyInfo)) != 2 &&
                gndDiff < -125.0f && mWaterY - current.pos.y < -125.0f &&
                current.pos.y - m35D4 > 125.0f) {
                current.pos.x = (f32)linkMotionQueries(me).mLinkLinChk.mCross.x;
                current.pos.z = (f32)linkMotionQueries(me).mLinkLinChk.mCross.z;
                if (LinkMotionApi::getWallCode(
                        dComIfG_Bgsp(), gabi::ea(&linkMotionQueries(me).mLinkLinChk.mPolyInfo)) ==
                    1) {
                    /* mPolyInfo = mLinkLinChk */
                    linkMotionQueries(me).mPolyInfo.mPolyIndex =
                        (u16)((u16)linkMotionQueries(me).mLinkLinChk.mPolyInfo.mPolyIndex);
                    linkMotionQueries(me).mPolyInfo.mBgIndex =
                        (u16)((u16)linkMotionQueries(me).mLinkLinChk.mPolyInfo.mBgIndex);
                    linkMotionQueries(me).mPolyInfo.mpBgW =
                        (u32)((u32)linkMotionQueries(me).mLinkLinChk.mPolyInfo.mpBgW);
                    linkMotionQueries(me).mPolyInfo.mActorId =
                        (u32)((u32)linkMotionQueries(me).mLinkLinChk.mPolyInfo.mActorId);
                    return procClimbDownStart_init(
                        cM_atan2s((f32)gabi::at<cM3dGPla>(pla)->mNormal.x,
                                  (f32)gabi::at<cM3dGPla>(pla)->mNormal.z));
                }
                if (procHangFallStart_init(gabi::at<void>(pla))) {
                    return TRUE;
                }
            }
        }
        morf = mCurProc == LinkMotionConstants::daPyProc_FRONT_ROLL_e ? 0.0f : 6.0f;
        if (pla != 0) {
            current.pos.x =
                gabi::fmadds(35.0f, (f32)gabi::at<cM3dGPla>(pla)->mNormal.x, current.pos.x);
            current.pos.z =
                gabi::fmadds(35.0f, (f32)gabi::at<cM3dGPla>(pla)->mNormal.z, current.pos.z);
        } else {
            current.pos.x = gabi::fmadds(35.0f, sn, current.pos.x);
            current.pos.z = gabi::fmadds(35.0f, cs, current.pos.z);
        }
    } else {
        morf = 6.0f;
    }
    s32 staff = mStaffIdx;
    if (staff != -1 && LinkMotionApi::getMySubstanceP(gabi::ea(&LinkActionPlay().mEvtManager),
                                                      staff, gabi::guest_addr("fall"), 3) != 0) {
        return procFall_init(0, morf);
    }
    return procFall_init(1, morf);
}

/* 023F8858 */
f32 daPy_lk_c::getSwimTimerRate() {

    if ((u8)LinkActionPlay().mItemSwimTimerStatus /* dComIfGp_getItemSwimTimerStatus() */ != 0 &&
        !dComIfGp_event_runCheck()) {
        s32 cnt = (s32)LinkActionPlay().mAirMeter; /* dComIfGp_getItemTimeCount() */
        return gabi::fnmsubs((f32)cnt, 0.0011111111f, 1.0f);
    }
    if ((s32)LinkActionPlay().mAirMeter <= 0) {
        return 1.0f;
    }
    return 0.0f;
}

/* cM_rad2s (02019510) and the cosine table entry of its angle */
static inline f32 lk_cosOfRad(f32 rad) {
    u16 a = (u16)LinkMotionApi::rad2s(rad);
    return cM_scos(a);
}

/* 023F8924 */
void daPy_lk_c::setSwimTimerStartStop() {

    f32 thr = mWaterY - 175.0f;
    f32 gnd = mAcch.m_ground_h;
    f32 target = 0.0f;
    u32 play = dComIfGp_ea();
    if (!(gnd > thr)) {
        LinkActionPlay(play).mItemSwimTimerStatus = (u8)(1); /* dComIfGp_startItemSwimTimer() */
        if (mNoResetFlg0 & LinkMotionConstants::daPyFlg0_UNK100) {
            f32 rate = getSwimTimerRate();
            if (rate > 0.5f) {
                target = -30.0f * lk_cosOfRad((1.0f - rate) * 3.1415927f);
                if (mFrameCtrlUnder[0].checkPass(0.0f) &&
                    mCurProc != LinkMotionConstants::daPyProc_SWIM_UP_e) {
                    if (mProcVar6 != 0 && !(mNoResetFlg0 & LinkMotionConstants::daPyFlg0_UNK4000)) {
                        LinkActionVoiceStart(this, 0x22);
                        mProcVar6 = 0;
                    } else {
                        mProcVar6 = 1;
                    }
                }
            }
        }
    } else {
        LinkActionPlay(play).mAirMeter = (s32)(900); /* dComIfGp_setItemTimeCount(900) */
        LinkActionPlay(play).mItemSwimTimerStatus = (u8)(1);
        LinkActionPlay().mItemSwimTimerStatus = (u8)(0); /* dComIfGp_stopItemSwimTimer() */
    }
    cLib_chaseF(&m3608, target, 3.0f);
}

/* 023F8B00 */
BOOL daPy_lk_c::procSwimWait_init(int keep) {

    J3DFrameCtrl &fc = mFrameCtrlUnder[0];
    commonProcInit(LinkMotionConstants::daPyProc_SWIM_WAIT_e);
    f32 r;
    if (keep) {
        r = fc.mFrame / (f32)fc.mEnd;
        f32 c = std::fabs(lk_cosOfRad(r * 3.1415927f));
        f32 sp = speedF;
        f32 a = sp * 0.6f;
        f32 b = sp * c;
        gravity = 0.0f;
        mNormalSpeed = gabi::fmadds(b, 0.4f, a);
    } else {
        r = 0.0f;
        gravity = 0.0f;
    }
    f32 rate = getSwimTimerRate();
    setSingleMoveAnime(LinkMotionConstants::ANM_SWIMWAIT, gabi::fmadds(rate, 2.5f, 0.5f), 0.0f, -1,
                       18.0f);
    f32 f = r * (f32)fc.mEnd;
    fc.mFrame = f;
    *LinkAction(this).mAnmRatioUnder[0].mpAnmFrame = f;
    dComIfGp_setPlayerStatus0(0, LinkMotionConstants::daPyStts0_SWIM_e);
    if (mNoResetFlg0 & 0x100) {
        if (mEquipItem != 0x100) {
            deleteEquipItem(1);
        }
        current.pos.y = mWaterY;
        speed.y = 0.0f;
    }
    mProcVar2 = 0;
    m35C4 = 1.0f;
    mProcVar6 = 0;
    return TRUE;
}

// HD: preserve the compiler's guard-first initialization and x/z/y store order.
static inline void linkMotionInitializeScale(be<u32> &initialized, cXyz &scale, f32 value) {
    if ((u32)initialized == 0) {
        initialized = 1;
        scale.x = value;
        scale.z = value;
        scale.y = value;
    }
}

/* 023F8D60 */
BOOL daPy_lk_c::procSwimUp_init(int splash) {

    linkMotionInitializeScale(l_splashScaleInitialized, l_splashScale, 0.4f);
    linkMotionInitializeScale(l_rippleScaleInitialized, l_rippleScale, 0.3f);
    commonProcInit(LinkMotionConstants::daPyProc_SWIM_UP_e);
    speed.y = 0.0f;
    current.pos.y = mWaterY;
    gravity = 0.0f;
    setSingleMoveAnime(LinkMotionConstants::ANM_SWIMP, 0.7f, 0.0f, 0x18, 3.0f);
    dComIfGp_setPlayerStatus0(0, LinkMotionConstants::daPyStts0_SWIM_e);
    m35C4 = 0.0f;
    mNoResetFlg0 = mNoResetFlg0 | 0x100;
    if (splash) {
        u32 pa = (u32)LinkActionPlay().mParticle.v;
        u32 e = LinkMotionApi::setParticle(pa, 1, LinkMotionConstants::ID_IT_JN_WP_SHIBUKI,
                                           &current.pos, 0, 0, 0xFF, 0, -1, 0, 0, 0);
        if (e != 0) {
            gabi::at<LinkMotionEmitter>(e)->mRate = (f32)(15.0f); /* setRate */
            f32 x = (f32)l_splashScale.x;
            gabi::at<LinkMotionEmitter>(e)->mGlobalDynamicsScale.x =
                (f32)(x); /* setGlobalDynamicsScale */
            f32 y = (f32)l_splashScale.y;
            gabi::at<LinkMotionEmitter>(e)->mGlobalDynamicsScale.y = (f32)(y);
            f32 z = (f32)l_splashScale.z;
            gabi::at<LinkMotionEmitter>(e)->mGlobalParticleScale.x =
                (f32)(x); /* setGlobalParticleScale */
            gabi::at<LinkMotionEmitter>(e)->mGlobalDynamicsScale.z = (f32)(z);
            gabi::at<LinkMotionEmitter>(e)->mGlobalParticleScale.y = (f32)(y);
            gabi::at<LinkMotionEmitter>(e)->mGlobalParticleScale.z = (f32)(z);
        }
        pa = (u32)LinkActionPlay().mParticle.v;
        e = LinkMotionApi::setParticle(pa, 5, LinkMotionConstants::ID_IT_JN_WP_HAMON01,
                                       &current.pos, 0, 0, 0xFF, linkMotionSingleRippleCallback(),
                                       -1, 0, 0, 0);
        if (e != 0) {
            f32 x = (f32)l_rippleScale.x;
            gabi::at<LinkMotionEmitter>(e)->mGlobalDynamicsScale.x = (f32)(x);
            f32 y = (f32)l_rippleScale.y;
            gabi::at<LinkMotionEmitter>(e)->mGlobalDynamicsScale.y = (f32)(y);
            f32 z = (f32)l_rippleScale.z;
            gabi::at<LinkMotionEmitter>(e)->mGlobalParticleScale.x = (f32)(x);
            gabi::at<LinkMotionEmitter>(e)->mGlobalDynamicsScale.z = (f32)(z);
            gabi::at<LinkMotionEmitter>(e)->mGlobalParticleScale.y = (f32)(y);
            gabi::at<LinkMotionEmitter>(e)->mGlobalParticleScale.z = (f32)(z);
        }
    }
    mProcVar6 = 0;
    return TRUE;
}

/* 023F8F80 */
BOOL daPy_lk_c::changeSwimProc() {

    if (!(mNoResetFlg0 & LinkMotionConstants::daPyFlg0_UNK80) ||
        (mModeFlg & 0x42000 /* IN_SHIP | SWIM */)) {
        return FALSE;
    }
    // HD: GameCube's !checkNoControll() became daPy_getPlayerActorClass() == this
    if ((u32)LinkActionPlay().mPlayerInfo[0].mpPlayer.v != gabi::ea(this) ||
        LK_demoMode == 0x11 /* DEMO_PFALL_e */ || !(mWaterY - current.pos.y > 90.0f)) {
        return FALSE;
    }
    u32 play = dComIfGp_ea();
    LinkActionPlay(play).mAirMeter = (s32)(900); /* dComIfGp_setItemTimeCount(900) */
    LinkActionPlay(play).mItemSwimTimerStatus = (u8)(1);
    LinkActionPlay().mItemTimeMax = (s32)(900); /* dComIfGp_setItemTimeMax(900) */
    mNoResetFlg0 = mNoResetFlg0 & ~0x02000000u; /* offNoResetFlg0(daPyFlg0_EQUIP_HEAVY_BOOTS) */
    setNoResetFlg1(noResetFlg1() & ~1u);        /* offNoResetFlg1(daPyFlg1_EQUIP_DRAGON_SHIELD) */
    endFlameDamageEmitter();
    f32 ns = mNormalSpeed * 0.75f;
    mMaxNormalSpeed = 18.0f;
    mNormalSpeed = ns;
    mNoResetFlg0 = mNoResetFlg0 & ~0x100u;
    if (!(ns < 18.0f)) {
        mNormalSpeed = 18.0f;
    }
    m35C4 = 0.0f;
    setSwimTimerStartStop();
    if (mModeFlg & LinkMotionConstants::ModeFlg_MIDAIR) {
        f32 sy = mOldSpeed.y;
        current.pos.y = current.pos.y + 90.0f;
        if (sy < -50.0f) {
            sy = -50.0f;
        } else if (sy > 0.0f) {
            sy = 0.0f;
        }
        speed.y = sy;
        f32 oy = mOldSpeed.y;
        f32 k = gabi::fmadds(oy * oy, 0.0004f, 0.2f);
        f32 wy = mWaterY;
        f32 cx = current.pos.x;
        if (k > 1.0f) {
            k = 1.0f;
        }
        gabi::Local<cXyz> pos;
        pos->x = cx;
        pos->y = wy;
        pos->z = current.pos.z;
        LinkMotionApi::createWpillar(pos.get(), 1.0f, k, 0);
        seStartOnlyReverb(LinkMotionConstants::JA_SE_LK_INTO_WATER);
        return procSwimWait_init(0);
    }
    return procSwimUp_init(0);
}

/* 023F9214 */
BOOL daPy_lk_c::dProcFreezeDamage_init() {

    if (mCurProc == LinkMotionConstants::daPyProc_DEMO_FREEZE_DAMAGE_e) {
        return TRUE;
    }
    commonProcInit(0xB1);
    mProcVar0 = 60;
    mNormalSpeed = 0.0f;
    if (linkOrderCompulsoryEvent(this)) {
        dProcFreezeDamage_init_sub(1);
    } else {
        mProcVar6 = 0;
    }
    return TRUE;
}

/* 023F92AC */
s32 daPy_lk_c::checkWallAtributeDamage(dBgS_AcchCir *cir) {

    if ((cir->m_flags & dBgS_AcchCir::WALL_HIT) &&
        LinkMotionApi::chkPolySafe(dComIfG_Bgsp(), gabi::ea(cir))) {
        s32 code = LinkMotionApi::getAttributeCode(dComIfG_Bgsp(), gabi::ea(cir));
        if (code == 9 /* dBgS_Attr_DAMAGE_e */ || code == 0x16 /* dBgS_Attr_ELECTRICITY_e */) {
            return code;
        }
    }
    return 0;
}

/* daPy_dmEcallBack_c statics: the damage effect type  and its timer  */
#define LK_dmType() (u16) LinkMotionGlobals::damageType()
static inline void lk_dmSet(u16 type, s16 timer) {
    LinkMotionGlobals::damageType() = (u16)(type);
    LinkMotionGlobals::damageTimer() = (s16)(timer);
}

/* 023F9340 */
void daPy_lk_c::setDamageElecEmitter() {

    if (LK_dmType() != 2 /* !daPy_dmEcallBack_c::checkElec() */) {
        endDamageEmitter();
        u32 mtx = linkGetAnmMtx(gabi::ea(mpCLModel.get()), 4 /* CL_JNT_CHEST_JNT_e */);
        LinkActionApi::makeEmitter(&mDmEcallBack[0], LinkMotionConstants::ID_AK_JN_CCTHUNDER00, mtx,
                                   &current.pos, nullptr);
        mtx = linkGetAnmMtx(gabi::ea(mpCLModel.get()), 4);
        LinkActionApi::makeEmitter(&mDmEcallBack[0xC], LinkMotionConstants::ID_AK_JN_CCTHUNDER01,
                                   mtx, &current.pos, nullptr);
    }
    lk_dmSet(2, 0x4B); /* daPy_dmEcallBack_c::setElec(75) */
}

/* 023F93FC */
void daPy_lk_c::setDamageFlameEmitter() {

    linkMotionInitializeScale(l_armFlameScaleInitialized, l_armFlameScale, 0.6f);
    if (LK_dmType() != 0 /* !daPy_dmEcallBack_c::checkFlame() */) {
        endDamageEmitter();
        gabi::Local<cXyz> pos;
        for (int i = 0; i < 4; i++) {
            const be<u16> *joint = reinterpret_cast<const be<u16> *>(l_flameJoint) + i;
            u32 mtx = linkGetAnmMtx(gabi::ea(mpCLModel.get()), (u16)*joint);
            pos->x = (f32)gabi::at<Mtx34>(mtx)->m[0][3]; /* mDoMtx_multVecZero */
            pos->y = (f32)gabi::at<Mtx34>(mtx)->m[1][3];
            pos->z = (f32)gabi::at<Mtx34>(mtx)->m[2][3];
            mtx = linkGetAnmMtx(gabi::ea(mpCLModel.get()), (u16)*joint);
            u32 cb = gabi::ea(&reinterpret_cast<LinkMotionDamageCallback *>(
                mDmEcallBack)[i]); /* mDmEcallBack[i] */
            LinkActionApi::makeEmitter(gabi::at<void>(cb),
                                       LinkMotionConstants::ID_AK_JN_BODYABLAZE00, mtx, pos.get(),
                                       nullptr);
            u32 e = (u32)gabi::at<LinkMotionDamageCallback>(cb)->mpEmitter.v;
            if (e != 0 && (i == 2 || i == 3)) {
                f32 x = (f32)l_armFlameScale.x; /* setGlobalScale */
                gabi::at<LinkMotionEmitter>(e)->mGlobalDynamicsScale.x = (f32)(x);
                f32 y = (f32)l_armFlameScale.y;
                gabi::at<LinkMotionEmitter>(e)->mGlobalDynamicsScale.y = (f32)(y);
                f32 z = (f32)l_armFlameScale.z;
                gabi::at<LinkMotionEmitter>(e)->mGlobalParticleScale.x = (f32)(x);
                gabi::at<LinkMotionEmitter>(e)->mGlobalDynamicsScale.z = (f32)(z);
                gabi::at<LinkMotionEmitter>(e)->mGlobalParticleScale.y = (f32)(y);
                gabi::at<LinkMotionEmitter>(e)->mGlobalParticleScale.z = (f32)(z);
            }
        }
    }
    lk_dmSet(0, 100); /* daPy_dmEcallBack_c::setFlame(100) */
}

/* 023F955C */
void daPy_lk_c::setDamageEmitter() {

    u32 obj = LinkMotionApi::getTgHitGObj(&mCyl);
    if (obj != 0) {
        u8 spl = (u8)gabi::at<dCcD_GObjInf>(obj)->mGObjAt.mSpl; /* GetAtSpl() */
        if (spl == 3) {
            setDamageCurseEmitter();
        } else if (spl == 0xB) {
            setDamageElecEmitter();
        } else if ((u32)gabi::at<dCcD_GObjInf>(obj)->mAtType &
                   0x200 /* ChkAtType(AT_TYPE_FIRE) */) {
            setDamageFlameEmitter();
        }
    }
}

/* 023F95F4 */
BOOL daPy_lk_c::procElecDamage_init(const cXyz *from) {

    if (!linkOrderCompulsoryEvent(this)) {
        return FALSE;
    }
    LK_demoType = 5;
    int use = checkShipRideUseItem(0);
    if (mModeFlg & LinkMotionConstants::ModeFlg_SWIM) {
        current.pos.y = current.pos.y + m35C4;
        mProcVar2 = 1;
        swimOutAfter(1);
    } else {
        mProcVar2 = 0;
    }
    commonProcInit(LinkMotionConstants::daPyProc_ELEC_DAMAGE_e);
    if (!(mAcch.m_flags & dBgS_Acch::GROUND_HIT)) {
        gravity = 0.0f;
        if (mProcVar2 == 0) {
            mModeFlg = mModeFlg | 2;
        }
    }
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(LinkMotionConstants::ANM_DAMBIRI, 1.0f, 0.0f, -1, 0.0f);
    mDamageWaitTimer = 0x1E;
    mProcVar0 = 0x14;
    if (from != nullptr) {
        gabi::Local<cXyz> d;
        cXyz_mi(&current.pos, d, from);
        linkCopyPositionWords(gabi::ea(&m370C), d);
        mProcVar3 = 1;
    } else if (LinkMotionApi::chkTgHit(&mCyl)) {
        cXyz *v = getDamageVec(&mCyl);
        linkCopyPositionWords(gabi::ea(&m370C), v);
        mProcVar3 = 1;
    } else {
        mProcVar3 = 0;
    }
    initShipRideUseItem(use, 0);
    seStartOnlyReverb(LinkMotionConstants::JA_SE_LK_ELEC_PARALYSED);
    LinkActionVoiceStart(this, 0x2C);
    linkStartShockUp(4);
    return TRUE;
}

/* 023F9930 */
BOOL daPy_lk_c::checkElecReturnDamage(dCcD_GObjInf *obj, cXyz *out) {

    if (LinkMotionApi::chkAtHit(obj) && LinkMotionApi::getAtHitGObj(obj) != 0) {
        u32 g = LinkMotionApi::getAtHitGObj(obj);
        if ((u8)gabi::at<dCcD_GObjInf>(g)->mGObjTg.mSpl /* GetTgSpl() */ == 1) {
            linkCopyPositionWords(gabi::ea(out), &obj->mGObjAt.mHitPos /* GetAtHitPosP() */);
            return TRUE;
        }
    }
    return FALSE;
}

/* 023F99D0 */
BOOL daPy_lk_c::procDamage_init() {

    cXyz *v = getDamageVec(&mCyl);
    u16 sy = (u16)shape_angle.y;
    f32 sn = cM_ssin(sy);
    f32 cs = cM_scos(sy);
    commonProcInit(LinkMotionConstants::daPyProc_DAMAGE_e);
    mDamageWaitTimer = 0x1E;
    f32 vx = v->x;
    f32 vy = v->y;
    f32 vz = v->z;
    f32 lz = gabi::fmadds(vz, cs, vx * sn);
    f32 lx = gabi::fmsubs(vx, cs, vz * sn);
    s16 a = cM_atan2s(lz, vy);
    f32 sq = gabi::fmadds(vy, vy, lz * lz);
    mProcVar2 = a;
    f64 len = LinkActionApi::sqrtWide(sq);
    s16 b = (s16)LinkMotionApi::atan2sYWide(-lx, len);
    mProcVar3 = b;
    s16 v3 = b;
    if (mProcVar2 > 0x1F40) {
        mProcVar2 = 0x1F40;
        v3 = mProcVar3;
    } else if (mProcVar2 < -0x1F40) {
        v3 = mProcVar3;
        mProcVar2 = -0x1F40;
    }
    if (v3 > 0x1F40) {
        mProcVar3 = 0x1F40;
    } else if (v3 < -0x1F40) {
        mProcVar3 = -0x1F40;
    }
    int dir = getDirectionFromAngle(cM_atan2s(lx, lz));
    int anm;
    if (dir == 1 /* DIR_BACKWARD */) {
        anm = 0x57; /* ANM_DAMF */
    } else if (dir == 2 /* DIR_LEFT */) {
        anm = 0x56; /* ANM_DAMR */
    } else if (dir == 3 /* DIR_RIGHT */) {
        anm = 0x55; /* ANM_DAML */
    } else {
        anm = 0x58; /* ANM_DAMB */
    }
    setSingleMoveAnime(anm, 0.6f, 0.0f, 9, 0.0f);
    current.angle.y = cM_atan2s(v->x, v->z);
    if (anm != 0x56) {
        mFootEffectPosType = 2;
    }
    if (anm != 0x55) {
        mFootEffectPosType = 1;
    }
    gabi::Local<cXyz> xz; /* damage_vec->absXZ() */
    f32 z = v->z;
    xz->x = v->x;
    xz->z = z;
    xz->y = 0.0f;
    f64 sq2 = LinkActionApi::squareMagnitude(xz.get());
    f64 len2 = LinkActionApi::sqrtWide(sq2);
    mNormalSpeed = (f32)(len2 * (f64)0.05f + (f64)13.0f);
    LinkActionVoiceStart(this, 2);
    seStartOnlyReverb(LinkMotionConstants::JA_SE_LK_DAMAGE_NORMAL);
    mProcVar6 = anm;
    mNoResetFlg0 = mNoResetFlg0 & ~2u;
    linkStartShockUp(4);
    return TRUE;
}

/* 023F9D1C */
void daPy_lk_c::setDamagePointWait(f32 amount) {

    if (mDamageWaitTimer == 0) {
        setDamagePoint(amount);
        mDamageWaitTimer = 0x1E;
        LinkActionVoiceStart(this, 2);
        seStartOnlyReverb(LinkMotionConstants::JA_SE_LK_DAMAGE_NORMAL);
    }
}

/* 023F9D84 */
BOOL daPy_lk_c::checkNormalDamage(int life) {

    u16 up = LK_upperIdx();
    if (up == 0x95 || up == 0x96 /* checkGrabAnime() */ ||
        mCurProc != LinkMotionConstants::daPyProc_MOVE_e) {
        return TRUE;
    }
    f32 ns = mNormalSpeed;
    f32 k;
    if (m3580 == 8) {
        k = 1.0f * ns;
    } else {
        k = cM_scos((u16)m34E2) * ns;
    }
    if (std::fabs(k / mMaxNormalSpeed) < 0.9f) {
        return TRUE;
    }
    if (!(noResetFlg1() & 1) && mTinkleShieldTimer == 0 /* !checkNoDamageMode() */ &&
        !LinkMotionApi::checkBottle(gabi::ea(&LinkActionSave().mSavedata.mPlayer.mPlayerItem),
                                    0x57) &&
        (s32)(u16)LinkActionSave().mSavedata.mPlayer.mPlayerStatusA.mLife /* dComIfGs_getLife() */
            <= life) {
        return TRUE;
    }
    return FALSE;
}

/* 023F9EAC */
void daPy_lk_c::setDashDamage() {

    setActAnimeUpper(LinkMotionConstants::dRes_INDEX_LKANM_BCK_DAMDASH_e, 2, 2.3f, 0.0f, -1, 2.4f);
    setPriTextureAnime(0x40, 0);
    mDamageWaitTimer = 0x14;
    linkCopyFloatBits(
        gabi::ea(&mFrameCtrlUpper[2].mFrame),
        gabi::ea(&mFrameCtrlUnder[1]
                      .mFrame)); /* mFrameCtrlUpper[2].setFrame(mFrameCtrlUnder[1].getFrame()) */
    LinkActionVoiceStart(this, 2);
    seStartOnlyReverb(0x282D);
}

/* |damage_vec| in XZ through PSVECSquareMag + std::sqrtf (f1 as returned) */
static inline f64 lk_absXZ(cXyz *v) {
    gabi::Local<cXyz> xz;
    f32 z = v->z;
    f32 x = v->x;
    xz->z = z;
    xz->x = x;
    xz->y = 0.0f;
    f64 sq = LinkActionApi::squareMagnitude(xz.get());
    return LinkActionApi::sqrtWide(sq);
}
/* the knockback (true) or guard slide (false) parameters of a guarded hit */
static inline bool lk_isKnockback(u32 obj) {
    return obj != 0 &&
           (u32)((u8)gabi::at<dCcD_GObjInf>(obj)->mGObjAt.mSpl /* GetAtSpl() */ - 5) <= 2;
}

/* 023F9F40 */
BOOL daPy_lk_c::procCrouchDefenseSlip_init() {

    commonProcInit(LinkMotionConstants::daPyProc_CROUCH_DEFENSE_SLIP_e);
    cXyz *v = getDamageVec(&mCyl);
    current.angle.y = cM_atan2s(v->x, v->z);
    u32 obj = LinkMotionApi::getTgHitGObj(&mCyl);
    if (lk_isKnockback(obj)) {
        setSingleMoveAnime(LinkMotionConstants::ANM_DIFENCEA, 0.9f, 0.0f, 0xB, 2.0f);
        f64 len = lk_absXZ(v);
        m35A4 = 4.9f;
        mNormalSpeed = (f32)(len * (f64)0.02f + (f64)52.0f);
        mProcVar0 = 0;
        m35A8 = 3.0f;
        m35A0 = 0.5f;
        m35AC = 12.5f;
        f64 r = LinkMotionApi::randomWide();
        mProcVar6 = r < (f64)0.2f;
    } else {
        setSingleMoveAnime(0x17, 0.85f, 0.0f, 0xB, 2.0f);
        f64 len = lk_absXZ(v);
        mProcVar0 = 0;
        mNormalSpeed = (f32)(len * (f64)0.02f + (f64)7.5f);
        m35A0 = 0.5f;
        m35A8 = 0.3f;
        m35AC = 12.5f;
        mProcVar6 = 0;
        m35A4 = 1.125f;
    }
    if (mEquipItem == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        u32 bck = getItemAnimeResource(LinkMotionConstants::dRes_INDEX_LKANM_BCK_ATNGAHAMA_e);
        linkChangeEquipmentAnimation(mSwordAnim_ea, bck);
        mFootEffectPosType = 3;
        linkCopyFloatBits(gabi::ea(&m35EC), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    } else {
        mFootEffectPosType = 3;
        linkCopyFloatBits(gabi::ea(&m35E8), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    }
    dComIfGp_setPlayerStatus1(0, LinkMotionConstants::daPyStts1_UNK80000_e);
    return TRUE;
}

/* 023FA1B8 */
BOOL daPy_lk_c::procGuardSlip_init() {

    cXyz *v = getDamageVec(&mCyl);
    commonProcInit(LinkMotionConstants::daPyProc_GUARD_SLIP_e);
    int anm;
    if (mEquipItem == 0x33) {
        anm = 0x3B; /* ANM_ATNGAHAM */
        u32 bck = getItemAnimeResource(0x18);
        linkChangeEquipmentAnimation(mSwordAnim_ea, bck);
    } else if (mDirection == 2 /* DIR_LEFT */) {
        anm = 0x39; /* ANM_ATNGAL */
    } else {
        anm = 0x3A; /* ANM_ATNGAR */
    }
    u32 obj = LinkMotionApi::getTgHitGObj(&mCyl);
    if (lk_isKnockback(obj)) {
        setSingleMoveAnime(anm, 0.9f, 0.0f, 0xB, 2.0f);
        f64 len = lk_absXZ(v);
        mProcVar0 = 0;
        m35AC = 12.5f;
        mNormalSpeed = (f32)(len * (f64)0.02f + (f64)52.0f);
        m35A0 = 0.5f;
        m35A4 = 4.9f;
        m35A8 = 3.0f;
        f64 r = LinkMotionApi::randomWide();
        mProcVar6 = r < (f64)0.2f;
    } else {
        setSingleMoveAnime(anm, 0.85f, 0.0f, 0xB, 2.0f);
        f64 len = lk_absXZ(v);
        mProcVar0 = 0;
        m35A0 = 0.5f;
        mProcVar6 = 0;
        m35AC = 12.5f;
        mNormalSpeed = (f32)(len * (f64)0.02f + (f64)7.5f);
        m35A4 = 1.125f;
        m35A8 = 0.3f;
    }
    if (mEquipItem == 0x33) {
        linkCopyFloatBits(gabi::ea(&m35EC), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    } else {
        linkCopyFloatBits(gabi::ea(&m35E8), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    }
    current.angle.y = cM_atan2s(v->x, v->z);
    mFootEffectPosType = 3;
    linkSetJointBlendRatio(m_pbCalc[1], 2, 0.0f); /* m_pbCalc[PART_UPPER_e]->setRatio(2, 0.0f) */
    return TRUE;
}

/* 023FA4C4 */
BOOL daPy_lk_c::procPolyDamage_init() {

    commonProcInit(LinkMotionConstants::daPyProc_POLY_DAMAGE_e);
    mDamageWaitTimer = 0x1E;
    setSingleMoveAnime(LinkMotionConstants::ANM_DAM, 0.6f, 0.0f, 9, 1.0f);
    mNormalSpeed = 0.0f;
    LinkActionVoiceStart(this, 2);
    seStartOnlyReverb(LinkMotionConstants::JA_SE_LK_DAMAGE_NORMAL);
    mNoResetFlg0 = mNoResetFlg0 & ~2u;
    return TRUE;
}

/* mCyl's tg checks (dCcD_GObjInf at this + 0x765C) */
#define LK_cylChkTgHit() LinkMotionApi::chkTgHit(&mCyl)
#define LK_cylGetTgHitGObj() LinkMotionApi::getTgHitGObj(&mCyl)
#define LK_changePlayer() LinkActionApi::changePlayer(this, this)

/* 023FA578 */
BOOL daPy_lk_c::changeDamageProc() {

    if (!(mModeFlg & LinkMotionConstants::ModeFlg_DAMAGE) && mDamageWaitTimer > 0) {
        s16 t = (s16)(mDamageWaitTimer - 1);
        mDamageWaitTimer = t;
        if (t == 0) {
            if (LK_upperIdx() == 0x64 /* checkDashDamageAnime() */) {
                resetActAnimeUpper(2, 2.4f);
            } else if (mModeFlg & LinkMotionConstants::ModeFlg_SWIM) {
                resetPriTextureAnime();
            }
        }
    }
    if (mNoResetFlg0 & LinkMotionConstants::daPyFlg0_UNK1000000) {
        mNoResetFlg0 = mNoResetFlg0 & ~0x01000000u;
        LK_changePlayer();
        return procLargeDamage_init(-5, 1, 0, 0);
    }
    u32 ship = gabi::ea(dComIfGp_getShipActor());
    bool stop = dComIfGp_event_runCheck() || LK_demoType != 0 /* checkPlayerDemoMode() */;
    if (!stop) {
        // HD: checkNoControll() is daPy_getPlayerActorClass() != this
        stop = (u32)LinkActionPlay().mPlayerInfo[0].mpPlayer.v != gabi::ea(this) ||
               mCurProc == LinkMotionConstants::daPyProc_SHIP_READY_e;
    }
    if (!stop && dComIfGp_checkPlayerStatus0(0, 0x10000) && ship != 0) {
        stop = (u32)gabi::at<LinkMotionShip>(ship)->mTornadoActor.v != 0 ||
               (u32)gabi::at<LinkMotionShip>(ship)->mWhirlActor.v != 0; /* ship->checkForceMove() */
    }
    if (stop) {
        mNoResetFlg0 = mNoResetFlg0 & ~0x200u; /* offNoResetFlg0(daPyFlg0_SHIP_DROP) */
        return FALSE;
    }
    s32 attr = 0;
    if ((noResetFlg1() & LinkMotionConstants::daPyFlg1_UNK4) ||
        ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) &&
         !(mModeFlg & LinkMotionConstants::ModeFlg_HANG) &&
         mCurrAttributeCode == 0x15 /* dBgS_Attr_FREEZE_e */)) {
        LK_changePlayer();
        return dProcFreezeDamage_init();
    }
    if (dComIfGp_checkPlayerStatus0(0, 0x10000) && (mNoResetFlg0 & 0x200)) {
        mNoResetFlg0 = mNoResetFlg0 & ~0x200u;
        LK_changePlayer();
        return procLargeDamage_init(-2, 1, 0, 0);
    }
    BOOL isAttr = FALSE;
    u32 damage;
    if (!(mModeFlg & 8) && mDamageWaitTimer == 0) {
        if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) &&
            ((attr = mCurrAttributeCode) == 9 || attr == 0x16)) {
            isAttr = TRUE;
        } else {
            for (int i = 0; i < 3; i++) {
                attr = checkWallAtributeDamage(&mAcchCir[i]);
                if (attr != 0) {
                    isAttr = TRUE;
                    break;
                }
            }
        }
    }
    if (isAttr) {
        damage = 1;
    } else {
        damage = (u8)mStts.m_dmg; /* mStts.GetDmg() */
        if (LK_cylChkTgHit() && LK_cylGetTgHitGObj() != 0 &&
            (u32)gabi::at<dCcD_GObjInf>(LK_cylGetTgHitGObj())->mAtType ==
                LinkMotionConstants::AT_TYPE_BOMB) {
            damage = 1;
        }
    }
    u32 spl;
    if (LK_cylChkTgHit()) {
        u32 g = LK_cylGetTgHitGObj();
        if (g != 0) {
            spl = (u8)gabi::at<dCcD_GObjInf>(g)->mGObjAt.mSpl; /* GetAtSpl() */
        } else {
            spl = (u8)mStts.mAtSpl; /* mStts.GetAtSpl() */
        }
    } else {
        spl = 0;
    }
    if (mModeFlg & 0x04000000) {
        if (isAttr || (LK_cylChkTgHit() && !checkSuccessGuard(spl))) {
            setDamagePoint((f32) - (s32)damage);
            mDamageWaitTimer = 0x14;
            LinkActionVoiceStart(this, 2);
            seStartOnlyReverb(LinkMotionConstants::JA_SE_LK_DAMAGE_NORMAL);
            LK_changePlayer();
            if (LK_cylChkTgHit() && !checkSuccessGuard(spl)) {
                setDamageEmitter();
            } else if (attr == 0x16 /* dBgS_Attr_ELECTRICITY_e */) {
                setDamageElecEmitter();
            }
            u16 type = LK_dmType();
            u32 mf = mModeFlg;
            if (type == 2 /* daPy_dmEcallBack_c::checkElec() */) {
                if ((mf & LinkMotionConstants::ModeFlg_ROPE) ||
                    (mf & LinkMotionConstants::ModeFlg_SWIM)) {
                    return procElecDamage_init(nullptr);
                }
            } else if ((mf & 0x40000) && !(noResetFlg1() & 1) && mTinkleShieldTimer == 0) {
                setPriTextureAnime(0x40, 0);
            }
        }
    } else {
        u32 atType = (u32)mAtCps[0].mAtType; /* mAtCps[0].GetAtType() */
        if (atType != LinkMotionConstants::AT_TYPE_BOKO_STICK &&
            atType != LinkMotionConstants::AT_TYPE_WATER) {
            gabi::Local<cXyz> hitPos;
            u32 me = gabi::ea(this);
            if (checkElecReturnDamage(&mAtCps[0], hitPos) ||
                checkElecReturnDamage(&mAtCps[1], hitPos) ||
                checkElecReturnDamage(&mAtCps[2], hitPos)) {
                setDamagePoint(-1.0f);
                setResetFlg0(resetFlg0() | LinkMotionConstants::daPyRFlg0_UNK80000);
                setDamageElecEmitter();
                LK_changePlayer();
                if (procElecDamage_init(hitPos)) {
                    return TRUE;
                }
                if (LK_checkPlayerFly()) {
                    return procLargeDamage_init(-1, 1, 0, 0);
                }
                return procDamage_init();
            }
        }
        if (LK_cylChkTgHit() && mCurProc != 0x6D && mCurProc != 0xD /* !checkGuardSlip() */ &&
            (!dComIfGp_checkPlayerStatus0(0, 0x10000) ||
             (!(noResetFlg1() & 1) && mTinkleShieldTimer == 0))) {
            freeRopeItem();
            freeHookshotItem();
            if (!checkSuccessGuard(spl)) {
                if (mEquipItem == LinkMotionConstants::daPyItem_BOKO_e) {
                    deleteEquipItem(0);
                }
                setDamageEmitter();
                LK_changePlayer();
                setDamagePoint((f32) - (s32)damage);
                u16 type = LK_dmType();
                u32 grab = gabi::ea(mActorKeepGrab.mActor.get());
                if (type == 2 && procElecDamage_init(nullptr)) {
                    return TRUE;
                }
                if (mModeFlg & LinkMotionConstants::ModeFlg_IN_SHIP) {
                    // HD: on the ship the large damage happens only in ship mode 1; otherwise the
                    // damage wait
                    if ((u8)LinkActionPlay().mMiniGameType == 1) {
                        return procLargeDamage_init(-9, 1, 0, 0);
                    }
                    setDamagePointWait(0.0f);
                    return TRUE;
                }
                if (field_0x2b0 < 0.0f /* checkGrabWear() */ &&
                    LinkActionApi::getCollisionActor(gabi::ea(&mCyl.mGObjTg)) != 0 &&
                    (s16)gabi::at<fopAc_ac_c>(
                        LinkActionApi::getCollisionActor(gabi::ea(&mCyl.mGObjTg)))
                            ->mProcName == LinkMotionConstants::fpcNm_NZ_e &&
                    grab != 0) {
                    cXyz *v = getDamageVec(&mCyl);
                    gabi::at<fopAc_ac_c>(grab)->shape_angle.y = (s16)(cM_atan2s(v->x, v->z));
                    gabi::at<fopAc_ac_c>(grab)->shape_angle.x = (s16)(0x2000);
                    return procLargeDamage_init(-4, 1, 0, 0);
                }
                if (spl == 7 || spl == 2 || spl == 10) {
                    return procLargeDamage_init(-6, 0, 0, 0);
                }
                if (LK_checkPlayerFly() || spl == 6 || spl == 1 || spl == 9) {
                    return procLargeDamage_init(-1, 1, 0, 0);
                }
                if (checkNormalDamage(damage)) {
                    return procDamage_init();
                }
                setDashDamage();
            } else {
                u32 se = LinkMotionApi::getTgHitObjSe(&mCyl);
                if (se == 6) {
                    seStartOnlyReverb(LinkMotionConstants::JA_SE_OBJ_COL_N_BDY_MPLT);
                } else if (se == 2 || se == 5) {
                    seStartOnlyReverb(LinkMotionConstants::JA_SE_OBJ_COL_SWS_NMTLP);
                } else {
                    seStartOnlyReverb(se == 4 ? LinkMotionConstants::JA_SE_OBJ_COL_NWHP_NMTL
                                              : LinkMotionConstants::JA_SE_OBJ_COL_SWM_NSWL);
                }
                if (mCurProc == LinkMotionConstants::daPyProc_CROUCH_DEFENSE_e) {
                    return procCrouchDefenseSlip_init();
                }
                return procGuardSlip_init();
            }
        } else if (isAttr) {
            LK_changePlayer();
            if (mEquipItem == 0x101) {
                deleteEquipItem(0);
            }
            setDamagePoint((f32) - (s32)damage);
            if (attr == 0x16) {
                setDamageElecEmitter();
                if (procElecDamage_init(nullptr)) {
                    return TRUE;
                }
            }
            if (checkNormalDamage(damage)) {
                return procPolyDamage_init();
            }
            setDashDamage();
        }
    }
    LinkMotionApi::clearTg(&mStts);
    LinkMotionApi::clearAt(&mStts);
    return FALSE;
}

/* 023FB020 */
BOOL daPy_lk_c::changeBoomerangCatchProc() {

    if (mNoResetFlg0 & LinkMotionConstants::daPyFlg0_UNK20) {
        if (!dComIfGp_event_runCheck() && LK_demoType == 0 && (mModeFlg & 0x20000) &&
            (LK_upperIdx() == 0xFFFF /* checkNoUpperAnime() */ ||
             LK_upperIdx() == 0x34 /* checkBoomerangThrowAnime() */)) {
            setActAnimeUpper(LinkMotionConstants::dRes_INDEX_LKANM_BCK_BOOMCATCH_e, 2, 1.0f, 1.0f,
                             0xB, 0.0f);
            setPriTextureAnime(0x49, 0);
            seStartOnlyReverb(LinkMotionConstants::JA_SE_LK_BOOM_CATCH);
            if (mModeFlg & 1) {
                return LK_callB(0x023E2648 /* procBoomerangCatch_init */);
            }
        }
        mNoResetFlg0 = mNoResetFlg0 & ~0x20u;
    }
    return FALSE;
}

/* 023FB10C */
void daPy_lk_c::throwBoomerang() {

    u32 boom = gabi::ea(mActorKeepEquip.mActor.get());
    gabi::at<fopAc_ac_c>(boom)->mParameters =
        (u32)(1); /* fopAcM_SetParam(boomerang, daBoomerang_c::Mode_Throw) */
    fopAc_ac_c *lock = mpAttnActorLockOn;
    if (lock != nullptr) {
        LinkMotionApi::setAimActor(boom, gabi::ea(lock));
    }
    gabi::at<fopAc_ac_c>(boom)->shape_angle.y = (s16)(shape_angle.y);
    u32 id = mActorKeepEquip.mID;
    u32 ac = gabi::ea(mActorKeepEquip.mActor.get());
    mActorKeepThrow.mID = id;
    mActorKeepThrow.mActor.v = ac;
    LinkMotionApi::clearActorKeep(&mActorKeepEquip);
    mEquipItem = 0x100;
    dComIfGp_setPlayerStatus0(0, LinkMotionConstants::daPyStts0_BOOMERANG_WAIT_e);
    s16 by = (s16)mBodyAngle.y; /* mBodyAngle.y */
    mBodyAngle.y = (s16)(0);
    s16 a = (s16)(shape_angle.y + by);
    shape_angle.y = a;
    current.angle.y = a;
    LinkActionVoiceStart(this, 0);
    if (dComIfGp_checkPlayerStatus0(0, LinkMotionConstants::daPyStts0_SHIP_RIDE_e)) {
        u32 st = dComIfGp_ea(); /* dComIfGp_clearPlayerStatus0(0, daPyStts0_BOOMERANG_AIM_e) */
        LinkActionPlay(st).mPlayerStatus[0][0] = LinkActionPlay(st).mPlayerStatus[0][0] & ~0x80000u;
        mModeFlg = (mModeFlg & ~0x20000000u) |
                   0x80; /* offModeFlg(ModeFlg_SUBJECT); onModeFlg(ModeFlg_00000080) */
    } else if (mAcch.m_flags & dBgS_Acch::GROUND_HIT) {
        procWait_init();
    }
}

/* the item-equip pass frames of the upper animation (rest/take/take-both/single item) */
#define LK_checkPass(fc, f) LinkMotionApi::checkPass(fc, (f32)(f))

/* 023FB230 */
void daPy_lk_c::checkItemAction() {

    /* the ready item button: HD has a fourth item button (3) */
    u8 sel;
    if ((sel = dComIfGp_getSelectItem(0), mEquipItem == sel) &&
        (sel = dComIfGp_getSelectItem(1), mEquipItem != sel) &&
        (sel = dComIfGp_getSelectItem(2), mEquipItem != sel)) {
        mReadyItemBtn = 0;
    } else if ((sel = dComIfGp_getSelectItem(1), mEquipItem == sel) &&
               (sel = dComIfGp_getSelectItem(0), mEquipItem != sel) &&
               (sel = dComIfGp_getSelectItem(2), mEquipItem != sel)) {
        mReadyItemBtn = 1;
    } else if ((sel = dComIfGp_getSelectItem(2), mEquipItem == sel) &&
               (sel = dComIfGp_getSelectItem(0), mEquipItem != sel) &&
               (sel = dComIfGp_getSelectItem(1), mEquipItem != sel)) {
        mReadyItemBtn = 2;
    } else if ((sel = dComIfGp_getSelectItem(3), mEquipItem == sel)) {
        mReadyItemBtn = 3;
    }
    BOOL equipAnime = checkEquipAnime();
    u16 up = LK_upperIdx();
    if (!(equipAnime || up == 0x34 /* BOOMTHROW */ || up == 0x33 /* BOOMCATCH */ ||
          up == 0xE2 /* ROPETHROW */)) {
        return;
    }
    J3DFrameCtrl *fc = &mFrameCtrlUpper[2];
    if (up == 0x33) {
        if (fc->mRate < 0.01f || (mStickDistance > 0.05f && fc->mFrame > 11.0f)) {
            resetActAnimeUpper(2, 2.4f);
            if (dComIfGp_checkPlayerStatus0(0, 0x10000)) {
                procShipPaddle_init();
            }
        }
        return;
    }
    if (up == 0x34) {
        if (fc->mRate < 0.01f || (mStickDistance > 0.05f && fc->mFrame > 10.0f)) {
            resetActAnimeUpper(2, 2.4f);
        } else if (LK_checkPass(fc, 7.0f)) {
            throwBoomerang();
        }
        return;
    }
    if (up == 0xE2) {
        if (fc->mRate < 0.01f) {
            mFrameCtrlUpper[1].mRate = 0.0f;
            mFrameCtrlUpper[0].mRate = 1.25f;
            mFrameCtrlUnder[0].mRate = 0.0f;
            mFrameCtrlUnder[1].mRate = 0.0f;
        }
        return;
    }
    if (!equipAnime) {
        return;
    }
    if (mEquipItem == LinkMotionConstants::daPyItem_SWORD_e) {
        u16 u = up;
        bool se = false;
        if (u == LinkMotionConstants::dRes_INDEX_LKANM_BCK_REST_e) {
            se = LK_checkPass(fc, 7.0f - fc->mRate);
            if (!se) {
                u = LK_upperIdx();
            }
        }
        if (!se && u == LinkMotionConstants::dRes_INDEX_LKANM_BCK_TAKE_e) {
            se = LK_checkPass(fc, 7.0f - fc->mRate);
            if (!se) {
                u = LK_upperIdx();
            }
        }
        if (!se && u == LinkMotionConstants::dRes_INDEX_LKANM_BCK_TAKEBOTH_e) {
            se = LK_checkPass(fc, 6.0f - fc->mRate);
        }
        if (!se && checkSingleItemEquipAnime()) {
            se = LK_checkPass(fc, 4.0f - fc->mRate);
        }
        if (se) {
            seStartOnlyReverb(LinkMotionConstants::JA_SE_LK_SW_PUTIN_S);
        }
    }
    if (std::fabs((f32)fc->mRate) < 0.01f) {
        resetActAnimeUpper(2, 2.4f);
        return;
    }
    {
        u16 u = LK_upperIdx();
        bool pass = false;
        if (u == 0xD7) {
            pass = LK_checkPass(fc, 7.0f);
            if (!pass) {
                u = LK_upperIdx();
            }
        }
        if (!pass && u == 0x103) {
            pass = LK_checkPass(fc, 7.0f);
            if (!pass) {
                u = LK_upperIdx();
            }
        }
        if (!pass && u == 0x104) {
            pass = LK_checkPass(fc, 6.0f);
        }
        if (!pass && checkSingleItemEquipAnime()) {
            pass = LK_checkPass(fc, 4.0f);
        }
        if (!pass) {
            return;
        }
    }
    u16 eq = mEquipItem;
    if ((eq != 0x100 && eq != 0x103) || (m3562 != 0x100 && m3562 != 0x103)) {
        seStartOnlyReverb(LinkMotionConstants::JA_SE_LK_ITEM_TAKEOUT);
        eq = mEquipItem;
    }
    BOOL had = eq != 0x100;
    deleteEquipItem(0);
    mEquipItem = m3562;
    if (!dComIfGp_event_runCheck() && LK_demoType == 0) {
        if (mEquipItem != 0x103 || !(noResetFlg1() & LinkMotionConstants::daPyFlg1_UNK4000000)) {
            if (lk_checkAttentionLock(mpAttention) || mEquipItem != 0x103 || had) {
                setResetFlg0(resetFlg0() | LinkMotionConstants::daPyRFlg0_UNK80);
            }
        }
    }
    if (mCurProc == LinkMotionConstants::daPyProc_DEMO_STAND_ITEM_PUT_e) {
        resetActAnimeUpper(2, -1.0f);
    } else if (mEquipItem == 0x103) {
        seStartOnlyReverb(LinkMotionConstants::JA_SE_LK_SW_PULLOUT_S);
        setSwordModel(0);
    } else {
        makeItemType();
    }
}

/* 023FB8A0 */
void daPy_lk_c::setShieldGuard() {

    gabi::Local<be<f32>> ratio; /* m_pbCalc[PART_UPPER_e]->getRatio(2) (joint 0) */
    *ratio.get() = (f32)linkMotionBlend(m_pbCalc[1]).mpBlendData->mEntries[2].mpJointWeights[0];
    s32 proc = mCurProc;
    bool defend;
    if (proc == 0x6D || proc == 0xD /* checkGuardSlip() */) {
        defend = true;
    } else if (checkEquipAnime() || LK_upperIdx() == 0x95 ||
               LK_upperIdx() == 0x96 /* checkGrabAnime() */) {
        defend = false;
    } else {
        defend = false;
        if (lk_checkAttentionLock(mpAttention)) {
            u16 eq = mEquipItem;
            if (eq != LinkMotionConstants::daPyItem_BOKO_e &&
                (u8)LinkActionSave().mSavedata.mPlayer.mPlayerStatusA.mSelectEquip[1] !=
                    0xFF /* checkShieldEquip() */) {
                if (checkGuardAccept() && (mActorKeepEquip.mID == 0xFFFFFFFF ||
                                           mActorKeepEquip.mActor.get() != nullptr)) {
                    defend = true;
                }
            }
        }
        proc = mCurProc;
    }
    if (defend) {
        dComIfGp_setRStatus(0x36 /* dActStts_DEFEND_e */);
        proc = mCurProc;
    }
    u16 anm =
        mEquipItem == 0x33 /* dItemNo_SKULL_HAMMER_e */ ? 0x1B /* ATNGHAM */ : 0x16 /* ATNG */;
    if (proc != 0x6D && proc != 0xD) {
        if ((mItemButton & 0x40 /* spActionButton() */) && dComIfGp_getRStatus() == 0x36) {
            freeGrabItem();
            if (LK_upperIdx() != anm) {
                resetActAnimeUpper(2, -1.0f);
                setActAnimeUpper((u32)anm, 2, 1.25f, 0.0f, -1, 2.4f);
            } else if (*ratio.get() < 1.0f) {
                cLib_chaseF(ratio.get(), 1.0f, 0.41666666f);
                linkSetJointBlendRatio(m_pbCalc[1], 2, *ratio.get());
            }
            u16 under = (u16)LinkAction(this)
                            .m_anm_heap_under[0]
                            .mIdx; /* m_anm_heap_under[UNDER_MOVE0_e].mIdx */
            if (under == LinkMotionConstants::dRes_INDEX_LKANM_BCK_ATNLS_e ||
                under == LinkMotionConstants::dRes_INDEX_LKANM_BCK_ATNRS_e) {
                /* mAnmRatioUpper[UPPER_MOVE2_e].getAnmTransform()->setFrame(mFrameCtrlUnder[UNDER_MOVE0_e].getFrame())
                 */
                *LinkAction(this).mAnmRatioUpper[2].mpAnmFrame = mFrameCtrlUnder[0].mFrame;
            }
        } else if (LK_upperIdx() == 0x16 || LK_upperIdx() == 0x1B /* checkUpperGuardAnime() */) {
            cLib_chaseF(ratio.get(), 0.0f, 0.41666666f);
            f32 r = *ratio.get();
            if (!(r > 0.0f)) {
                resetActAnimeUpper(2, 2.4f);
            } else {
                linkSetJointBlendRatio(m_pbCalc[1], 2, r);
            }
        }
    }
    if (LinkActionCheckPlayerGuard(this)) {
        mCyl.mGObjTg.mSPrm = mCyl.mGObjTg.mSPrm | 1; /* OnTgShield() */
    } else {
        mCyl.mGObjTg.mSPrm = mCyl.mGObjTg.mSPrm & ~1u; /* OffTgShield() */
    }
}

/* 023FBC7C */
BOOL daPy_lk_c::checkAtHitEnemy(dCcD_GObjInf *obj) {

    if (LinkMotionApi::chkAtHit(obj)) {
        u32 ac = LinkActionApi::getCollisionActor(gabi::ea(&obj->mGObjAt));
        if (ac != 0 && (u8)gabi::at<fopAc_ac_c>(ac)->group == LinkMotionConstants::fopAc_ENEMY_e) {
            return TRUE;
        }
    }
    return FALSE;
}

/* J3DAnmBase::getFrameMax (HD: virtual, vtable at +4, slot 0x14) */
static inline s32 anm_getFrameMax(u32 anm) {
    return gabi::call_ptr<s32>((u32)gabi::at<LinkMotionAnm>(anm)->__vtbl->getFrameMax, anm);
}

/* 023FBCEC */
void daPy_lk_c::playTextureAnime() {
    u16 textureAnime = (u16)linkMotionAnimeHeap(m_tex_anm_heap).field_0x4;
    if (textureAnime != 0xFFFF) {
        if (mCurProc == LinkMotionConstants::daPyProc_DEMO_TOOL_e) {
            u16 frame = (u16)gabi::ftoi(mFrameCtrlUnder[0].mFrame);
            m3530 = frame;
            m3532 = frame;
        } else if ((u16)linkMotionAnimeHeap(m_tex_anm_heap).field_0x6 == 0xFFFE &&
                   checkMabaAnimeBtp(textureAnime)) {
            u16 frame = m3530;
            if (frame != 0) {
                frame = (u16)(frame + 1);
                m3530 = frame;
                if ((s32)frame >= anm_getFrameMax(mpAnmTexPatternData)) {
                    m3530 = 0;
                    m3532 = 0;
                } else {
                    m3532 = m3530;
                }
            } else if (LinkMotionApi::randomWide() < (f64)0.012f) {
                u16 nextFrame = (u16)(m3530 + 1);
                m3530 = nextFrame;
                m3532 = nextFrame;
            } else {
                m3532 = m3530;
            }
        } else {
            m3530 = (u16)(m3530 + 1);
            m3532 = (u16)(m3532 + 1);
        }
    } else {
        u32 mode = mModeFlg;
        if ((u16)linkMotionAnimeHeap(m_tex_anm_heap).field_0x2 != 0xFFFF) {
            if (mode & LinkMotionConstants::ModeFlg_SWIM) {
                m3530 = 0;
                m3532 = 0;
            } else {
                u16 frame = (u16)gabi::ftoi(mFrameCtrlUpper[2].mFrame);
                m3530 = frame;
                m3532 = frame;
            }
        } else if ((mode & 0x400) || m34C3 == 9 || m34C3 == 10) {
            u16 frame = (u16)gabi::ftoi(mFrameCtrlUnder[0].mFrame);
            m3530 = frame;
            m3532 = frame;
        } else if (mode & 0x100) {
            u16 frame = m3530;
            bool finished = false;
            u16 scrollAnime = 0;
            if (frame != 0) {
                frame = (u16)(frame + 1);
                m3530 = frame;
                if ((s32)frame >= anm_getFrameMax(mpAnmTexPatternData)) {
                    scrollAnime = (u16)linkMotionAnimeHeap(m_tex_scroll_heap).mIdx;
                    m3530 = 0;
                    finished = true;
                }
            } else if (LinkMotionApi::randomWide() < (f64)0.012f) {
                m3530 = (u16)(m3530 + 1);
            }
            // Preserve the index captured before resetting the blink frame.
            if (!finished) {
                scrollAnime = (u16)linkMotionAnimeHeap(m_tex_scroll_heap).mIdx;
            }
            if (scrollAnime == LinkMotionConstants::dRes_INDEX_LKANM_BTK_TABEKOBE_e) {
                u16 nextFrame = (u16)(m3532 + 1);
                m3532 = nextFrame;
                if (!((s32)nextFrame < anm_getFrameMax(mpTexScrollResData))) {
                    m3532 = 0;
                }
            } else {
                m3532 = m3530;
            }
        }
    }
    s32 max;
    if ((max = anm_getFrameMax(mpAnmTexPatternData), (s32)(u16)m3530 >= max)) {
        if (anm_getFrameMax(mpAnmTexPatternData) == 0) {
            m3530 = 0;
        } else {
            m3530 = (u16)anm_getFrameMax(mpAnmTexPatternData);
        }
    }
    if ((max = anm_getFrameMax(mpTexScrollResData), (s32)(u16)m3532 >= max)) {
        if (anm_getFrameMax(mpTexScrollResData) == 0) {
            m3532 = 0;
        } else {
            m3532 = (u16)anm_getFrameMax(mpTexScrollResData);
        }
    }
}

/* the joint count of the model data's joint tree (027F3F94 resolves the self-relative tree pointer)
 */
static inline s32 lk_jointNum(u32 me) {
    u32 res = LinkActionApi::resolveJointTree(
        (u32)gabi::at<daPy_lk_c>(me)->mpCLModelData.v /* mpCLModelData */);
    return (u16)gabi::at<LinkMotionJointTree>(res)->mJointCount;
}
/* mDoExt_AnmRatioPack (HD 0x10: count at +8, weights at +0xC) */
static inline void lk_ratioFill(u32 pack, f32 value) {
    LinkActionAnmRatio *ratio = gabi::at<LinkActionAnmRatio>(pack);
    for (s32 joint = 0; joint < ratio->mJointCount; joint++) {
        ratio->mpJointWeights[joint] = value;
    }
}
static inline void lk_ratioSet(u32 pack, s32 joint, f32 value) {
    gabi::at<LinkActionAnmRatio>(pack)->mpJointWeights[joint] = value;
}
/* the per-animation joint flag updates of a blend calc (pb + 0x84: 0x68 per animation) */
static inline void lk_pbUpdateFlags(u32 me, u32 pb, s32 k, u32 a0, u32 b0, u32 a1, u32 b1) {
    LinkMotionApi::updateJointFlags(pb, k);
    u32 res = LinkActionApi::resolveJointTree((u32)gabi::at<daPy_lk_c>(me)->mpCLModelData.v);
    if ((u32)linkMotionBlend(pb).mpBlendData->mEntries[k].mpAnmTransform != 0) {
        LinkMotionApi::updateMaterialFlags(gabi::ea(&linkMotionBlend(pb).mpJointFlags[k]), res, a0,
                                           b0);
    }
    res = LinkActionApi::resolveJointTree((u32)gabi::at<daPy_lk_c>(me)->mpCLModelData.v);
    if ((u32)linkMotionBlend(pb).mpBlendData->mEntries[k].mpAnmTransform != 0) {
        LinkMotionApi::updateMaterialFlags(gabi::ea(&linkMotionBlend(pb).mpJointFlags[k]), res, a1,
                                           b1);
    }
}

/* 023FC06C HD-only (no GameCube function; name descriptive): sets the under/upper blend calcs on
 * the model data's first joints and recomputes the per-joint animation weights
 * (mAnmRatioUnder/Upper) from the blend ratios, then refreshes the joint flags of each animation */
/* 023FC06C */
void daPy_lk_c::setAnimeRatioHD() {

    u32 me = gabi::ea(this);
    u32 md = (u32)gabi::at<daPy_lk_c>(me)->mpCLModelData.v;
    gabi::at<LinkMotionJoint>(gabi::at<J3DModelData>(md)->mJointNodes)->mpMtxCalc =
        m_pbCalc[0]; /* joint 0's mtx calc */
    md = (u32)gabi::at<daPy_lk_c>(me)->mpCLModelData.v;
    u32 n = (u32)gabi::at<J3DModelData>(md)->mJointNum;
    u32 pb1 = m_pbCalc[1];
    u32 jt = (u32)gabi::at<J3DModelData>(md)->mJointNodes;
    if (n > 2) {
        jt += 0x38;
    }
    gabi::at<LinkMotionJoint>(jt)->mpMtxCalc = pb1;
    u32 pb0 = m_pbCalc[0];
    for (s32 i = 0; i < lk_jointNum(me);) {
        u32 t = (u32)linkMotionBlend(pb0).mpBlendData.v;
        u32 anm1 = (u32)gabi::at<LinkMotionBlendData>(t)->mEntries[1].mpAnmTransform;
        f32 w = (f32)gabi::at<LinkMotionBlendData>(t)->mEntries[1].mpJointWeights[i];
        if (anm1 != 0) {
            lk_ratioSet(me + 0x57F8 /* mAnmRatioUnder[0] */, i, 1.0f - w);
            lk_ratioSet(me + 0x5808 /* mAnmRatioUnder[1] */, i, w);
        } else {
            lk_ratioFill(me + 0x57F8, 1.0f);
            lk_ratioFill(me + 0x5808, 0.0f);
        }
        i++;
    }
    for (s32 k = 0; k < 2; k++) {
        lk_pbUpdateFlags(me, pb0, k, 0, 0, 2, 3);
    }
    pb1 = m_pbCalc[1];
    for (s32 i = 0; i < lk_jointNum(me); i++) {
        u32 mdl = gabi::at<daPy_lk_c>(me)->mpCLModel->mModelData.v; /* mpCLModel->getModelData() */
        u32 jn = (u32)gabi::at<J3DModelData>(mdl)->mJointNum;
        u32 j = (u32)gabi::at<J3DModelData>(mdl)->mJointNodes;
        if ((u32)(u16)i < jn) {
            j += (u16)i * 0x1C;
        }
        if ((u32)gabi::at<LinkMotionJoint>(j)->mCallBack != 0) {
            LinkMotionApi::jointCB0(this, i);
        }
        u32 t = (u32)linkMotionBlend(pb1).mpBlendData.v;
        u32 anm2 = (u32)gabi::at<LinkMotionBlendData>(t)->mEntries[2].mpAnmTransform;
        f32 w2 = (f32)gabi::at<LinkMotionBlendData>(t)->mEntries[2].mpJointWeights[i];
        f32 w1 = (f32)gabi::at<LinkMotionBlendData>(t)->mEntries[1].mpJointWeights[i];
        u32 anm1 = (u32)gabi::at<LinkMotionBlendData>(t)->mEntries[1].mpAnmTransform;
        if (anm2 == 0) {
            w2 = 0.0f;
        }
        f32 a = (1.0f - w2) * w1;
        if (anm1 == 0) {
            a = 0.0f;
        }
        f32 base = 1.0f - (w2 + a);
        lk_ratioSet(me + 0x5818 /* mAnmRatioUpper[0] */, i, base);
        lk_ratioSet(me + 0x5828 /* mAnmRatioUpper[1] */, i, a);
        lk_ratioSet(me + 0x5838 /* mAnmRatioUpper[2] */, i, w2);
    }
    for (s32 k = 0; k < 3; k++) {
        lk_pbUpdateFlags(me, pb1, k, 0, 3, 2, 0);
    }
    reinterpret_cast<LinkMotionMtxBuffer *>(mpCLModel->mpMtxBuffer.get())->mpMtxCalc = m_pbCalc[0];
    reinterpret_cast<LinkMotionMtxBuffer *>(mpCLModel->mpMtxBuffer.get())->mCurrentMtxCalc = 0;
}

/* 023FC568 */
void daPy_lk_c::setBeltConveyerPower() {

    const u32 zero = 0x101FFBA8; /* cXyz::Zero */
    if (!checkHeavyStateOn() && !dComIfGp_event_runCheck() && LK_demoType == 0 &&
        !(mNoResetFlg0 & 0xA0000000 /* daPyFlg0_UNK20000000 | daPyFlg0_UNK80000000 */) &&
        ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) ||
         (mModeFlg & LinkMotionConstants::ModeFlg_SWIM))) {
        gabi::Local<cXyz> vec;
        gabi::Local<be<s32>> spd;
        u32 gnd = gabi::ea(static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd)); /* mAcch.m_gnd */
        if (LinkMotionApi::chkPolySafe(dComIfG_Bgsp(), gnd) &&
            LinkMotionApi::getPolyRoomPathVec(gnd, vec.get(), spd.get())) {
            gabi::Local<cXyz> unused;
            LinkMotionApi::normalizeZP(vec.get(), unused.get());
            PSVECScale(vec, vec, (f32)(s32)*spd.get());
        } else {
            u32 z = (u32) reinterpret_cast<LinkActionVectorWords *>(&cXyz::Zero)->z;
            u32 y = (u32) reinterpret_cast<LinkActionVectorWords *>(&cXyz::Zero)->y;
            reinterpret_cast<LinkActionVectorWords *>(vec.get())->x =
                (u32)((u32) reinterpret_cast<LinkActionVectorWords *>(&cXyz::Zero)->x);
            reinterpret_cast<LinkActionVectorWords *>(vec.get())->y = (u32)(y);
            *spd.get() = 0;
            reinterpret_cast<LinkActionVectorWords *>(vec.get())->z = (u32)(z);
            PSVECScale(vec, vec, 0.0f);
        }
        gabi::Local<cXyz> xz1;
        xz1->x = m36B8.x;
        xz1->y = 0.0f;
        xz1->z = m36B8.z;
        f64 a = LinkActionApi::squareMagnitude(xz1.get());
        gabi::Local<cXyz> xz2;
        xz2->x = vec->x;
        xz2->y = 0.0f;
        xz2->z = vec->z;
        f64 b = LinkActionApi::squareMagnitude(xz2.get());
        f32 d = (f32)(b - a);
        f32 step = d >= 0.0f ? 1.0f : 3.0f; /* fsel */
        LinkMotionApi::addCalcPosXZ(&m36B8, vec.get(), 0.5f, step, 0.5f);
    } else {
        linkCopyPositionWords(gabi::ea(&m36B8), gabi::at<cXyz>(zero));
    }
}

/* 023FC7F0 */
void daPy_lk_c::setWindAtPower() {

    u32 me = gabi::ea(this);
    void *wind = gabi::at<void>(me + 0x778C); /* mWindCyl */
    if (LinkMotionApi::chkTgHit(reinterpret_cast<dCcD_GObjInf *>(wind))) {
        u32 g = LinkMotionApi::getTgHitGObj(reinterpret_cast<dCcD_GObjInf *>(wind));
        m34BA = (g != 0 && (u8)gabi::at<dCcD_GObjInf>(g)->mGObjAt.mSpl == 1) ? 1 : 0;
    }
    gabi::Local<cXyz> vec;
    f32 step;
    bool zeroPath = true;
    if (LinkMotionApi::chkTgHit(reinterpret_cast<dCcD_GObjInf *>(wind)) &&
        !(mDamageWaitTimer != 0 && m34BA != 0) && !dComIfGp_event_runCheck() && LK_demoType == 0 &&
        mCurProc != LinkMotionConstants::daPyProc_FAN_GLIDE_e) {
        zeroPath = false;
        vec->y = (f32)mWindCyl.mGObjTg.mRVec.y; /* *mWindCyl.GetTgRVecP() */
        f32 z = (f32)mWindCyl.mGObjTg.mRVec.z;
        f32 x = (f32)mWindCyl.mGObjTg.mRVec.x;
        vec->x = x;
        vec->z = z;
        gabi::Local<cXyz> xz;
        xz->y = 0.0f;
        xz->x = x;
        xz->z = z;
        f64 len = LinkActionApi::sqrtWide(LinkActionApi::squareMagnitude(xz.get()));
        f32 lim;
        if (m34BA != 0) {
            lim = 27.0f;
            step = 18.0f;
        } else {
            lim = 30.0f;
            step = 1.0f;
        }
        if (len < 1.0) {
            gabi::Local<cXyz> d;
            gabi::Local<cXyz> d2;
            cXyz_mi(&current.pos, d, gabi::at<cXyz>(me + 0x7858) /* GetTgHitPosP() */);
            cXyz_ml(d, d2, lim);
            f32 y2 = d2->y;
            f32 z2 = d2->z;
            vec->y = y2;
            f32 x2 = d2->x;
            vec->x = x2;
            vec->z = z2;
            xz->y = 0.0f;
            xz->x = x2;
            xz->z = z2;
            len = LinkActionApi::sqrtWide(LinkActionApi::squareMagnitude(xz.get()));
        }
        if (len > (f64)lim) {
            PSVECScale(vec, vec, (f32)((f64)lim / len));
        }
    }
    if (zeroPath) {
        linkCopyPositionWords(gabi::ea(vec.get()), &cXyz::Zero /* cXyz::Zero */);
        step = m34BA != 0 ? 1.0f : 3.0f;
    }
    if (mCurProc == 0x93) {
        return;
    }
    if (dComIfGp_event_runCheck() || LK_demoType != 0) {
        m373C.x = 0.0f;
        m373C.z = 0.0f;
    } else {
        LinkMotionApi::addCalcPosXZ(&m373C, vec.get(), 0.5f, step, 0.5f);
    }
    if (checkHeavyStateOn() || dComIfGp_event_runCheck() || LK_demoType != 0) {
        // HD: the wind push is cleared only when the heavy state still holds
        if (checkHeavyStateOn()) {
            m3730.z = 0.0f;
            m3730.x = 0.0f;
        }
    } else {
        LinkMotionApi::addCalcPosXZ(&m3730, vec.get(), 0.5f, step, 0.5f);
    }
}

/* f32 fused multiply-add with a double operand as the recompiled original computes it (a*c+b,
 * rounded once to single) */
static inline f32 lk_fmaddsD(f64 a, f64 c, f64 b) {
    return (f32)(a * c + b);
}

/* 023FCB9C */
void daPy_lk_c::posMoveFromFootPos() {

    u32 me = gabi::ea(this);
    if ((u8)gabi::at<LinkMotionOldFrame>(m_old_fdata)
            ->mOldFrameFlg /* m_old_fdata->getOldFrameFlg() */
        == 0) {
        speedF = 0.0f;
        /* mFootData[i].field_0x018 / field_0x00C = rtoe_pos_offset / rheel_pos_offset (x mirrored
         * for the left foot) */
        reinterpret_cast<LinkMotionFootData *>(mFootData)[1].field_0x018.x = (f32)(14.05f);
        reinterpret_cast<LinkMotionFootData *>(mFootData)[0].field_0x018.x = (f32)(-14.05f);
        reinterpret_cast<LinkMotionFootData *>(mFootData)[1].field_0x018.y = (f32)(0.0f);
        reinterpret_cast<LinkMotionFootData *>(mFootData)[0].field_0x018.y = (f32)(0.0f);
        reinterpret_cast<LinkMotionFootData *>(mFootData)[0].field_0x018.z = (f32)(5.02f);
        reinterpret_cast<LinkMotionFootData *>(mFootData)[0].field_0x00C.x = (f32)(-10.85f);
        reinterpret_cast<LinkMotionFootData *>(mFootData)[1].field_0x018.z = (f32)(5.02f);
        reinterpret_cast<LinkMotionFootData *>(mFootData)[1].field_0x00C.x = (f32)(10.85f);
        reinterpret_cast<LinkMotionFootData *>(mFootData)[1].field_0x00C.y = (f32)(0.0f);
        reinterpret_cast<LinkMotionFootData *>(mFootData)[1].field_0x00C.z = (f32)(-6.52f);
        reinterpret_cast<LinkMotionFootData *>(mFootData)[0].field_0x00C.y = (f32)(0.0f);
        reinterpret_cast<LinkMotionFootData *>(mFootData)[0].field_0x00C.z = (f32)(-6.52f);
        m34BC = 2;
        return;
    }
    const u32 stk = gabi::ea(mDoMtx_stack_c::get());     /* mDoMtx_stack_c::now */
    const u32 toePos = 0x101CEB9C, heelPos = 0x101CEB90; /* l_toe_pos, l_heel_pos */
    gabi::Local<cXyz> toe[2];
    gabi::Local<cXyz> heel[2];
    gabi::Local<cXyz> mid[2];
    gabi::Local<cXyz> tmp;
    LinkMotionApi::concatMatrix(&m37B4,
                                gabi::at<Mtx34>(linkGetAnmMtx(gabi::ea(mpCLModel.get()), 0x22)),
                                gabi::at<Mtx34>(stk));
    LinkMotionApi::multVec(gabi::at<Mtx34>(stk), gabi::at<cXyz>(toePos), toe[1].get());
    LinkMotionApi::multVec(gabi::at<Mtx34>(stk), gabi::at<cXyz>(heelPos), heel[1].get());
    LinkMotionApi::concatMatrix(&m37B4,
                                gabi::at<Mtx34>(linkGetAnmMtx(gabi::ea(mpCLModel.get()), 0x27)),
                                gabi::at<Mtx34>(stk));
    LinkMotionApi::multVec(gabi::at<Mtx34>(stk), gabi::at<cXyz>(toePos), toe[0].get());
    LinkMotionApi::multVec(gabi::at<Mtx34>(stk), gabi::at<cXyz>(heelPos), heel[0].get());
    LinkMotionApi::concatMatrix(&m37B4,
                                gabi::at<Mtx34>(linkGetAnmMtx(gabi::ea(mpCLModel.get()), 0x1E)),
                                gabi::at<Mtx34>(stk));
    u16 a = (u16)m34E0;
    f32 sn = cM_ssin(a);
    f32 cs = cM_scos(a);
    f32 h[2];
    for (int i = 0; i < 2; i++) {
        linkAddPosition(gabi::ea(heel[i].get()), tmp, gabi::ea(toe[i].get()));
        linkCopyPositionWords(gabi::ea(mid[i].get()), tmp);
        PSVECScale(mid[i], mid[i], 0.5f);
        f32 z = mid[i]->z;
        f32 y = mid[i]->y;
        f32 m13 = (f32)gabi::at<Mtx34>(stk)->m[1][3];
        f32 m23 = (f32)gabi::at<Mtx34>(stk)->m[2][3];
        f32 t = gabi::fmadds(y - m13, cs, m13);
        h[i] = gabi::fmadds(z - m23, sn, t);
    }
    u32 idx = !(h[0] < h[1]);
    m34BC = idx;
    gabi::Local<cXyz> d;
    cXyz_mi(toe[idx], d,
            &reinterpret_cast<LinkMotionFootData *>(mFootData)[idx]
                 .field_0x018 /* mFootData[idx].field_0x018 */);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->z = d->z;
    xz->y = 0.0f;
    f64 step = LinkActionApi::sqrtWide(LinkActionApi::squareMagnitude(xz.get()));
    f32 r = m3598;
    if (r < 1.0f && std::fabs((f32)(m35B4 - mStickDistance)) < 0.2f) {
        step = lk_fmaddsD(step, 0.3f, m359C * 0.7f);
    }
    f32 ns = mNormalSpeed;
    f32 v = ns * (1.0f - r);
    if (ns < 0.0f) {
        v = (f32)(-(step * (f64)r - (f64)v)); /* fnmsubs */
    } else {
        v = lk_fmaddsD(step, r, v);
    }
    if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) &&
        LinkMotionApi::chkPolySafe(dComIfG_Bgsp(),
                                   gabi::ea(static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd))) &&
        m3580 != 8) {
        s32 ga = LinkMotionApi::getGroundAngle(
            this, gabi::ea(static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd)), (s32)current.angle.y);
        v = v * cM_scos((u16)ga);
        if (ga < 0) {
            v = v * 0.85f;
        }
    } else {
        v = v * cM_scos(0);
    }
    u32 z0, x0, y0;
    if (!(std::fabs(v) < 0.05f)) {
        s32 proc = mCurProc;
        speedF = v;
        if (proc == LinkMotionConstants::daPyProc_SWIM_MOVE_e) {
            f32 fr = mFrameCtrlUnder[0].mFrame / (f32)mFrameCtrlUnder[0].mEnd;
            f32 c = lk_cosOfRad(fr * 3.1415927f);
            f32 sp = speedF;
            f64 rate = LinkMotionApi::swimTimerRateWide(this);
            f32 den = lk_fmaddsD(rate, 0.35f, 1.0f);
            f32 num = gabi::fmadds(sp * std::fabs(c), 0.4f, sp * 0.6f);
            f32 k = num / den;
            u16 ay = (u16)current.angle.y;
            speed.x = k * cM_ssin(ay);
            speed.z = k * cM_scos(ay);
        } else if (proc == LinkMotionConstants::daPyProc_CUT_ROLL_e && dComIfGp_event_runCheck() &&
                   LK_demoType == 0 && LK_demoMode != 0x2B /* DEMO_CUT_ROLL_e */) {
            speed.x = 0.0f;
            speed.z = 0.0f;
        } else {
            if (proc == 0x56) {
                v = speedF;
            }
            u16 ay = (u16)current.angle.y;
            speed.x = v * cM_ssin(ay);
            speed.z = speedF * cM_scos(ay);
        }
    } else {
        speedF = 0.0f;
        speed.x = 0.0f;
        speed.z = 0.0f;
    }
    z0 = (u32) reinterpret_cast<LinkActionVectorWords *>(&m36A0)->z;
    x0 = (u32) reinterpret_cast<LinkActionVectorWords *>(&m36A0)->x;
    y0 = (u32) reinterpret_cast<LinkActionVectorWords *>(&m36A0)->y;
    reinterpret_cast<LinkActionVectorWords *>(&m36AC)->x = (u32)(x0); /* m36AC = m36A0 */
    reinterpret_cast<LinkActionVectorWords *>(&m36AC)->y = (u32)(y0);
    reinterpret_cast<LinkActionVectorWords *>(&m36AC)->z = (u32)(z0);
    if (!(mModeFlg & 0x11612832) && !dComIfGp_event_runCheck() && LK_demoType == 0 &&
        !checkHeavyStateOn() && mCurrAttributeCode == 0xF /* dBgS_Attr_ICE_e */ &&
        !(mNoResetFlg0 & 0xA0000000) && !(mAcch.m_flags & dBgS_Acch::WALL_HIT) &&
        (mAcch.m_flags & dBgS_Acch::GROUND_HIT)) {
        LinkMotionApi::addCalc(&m36A0.x, 0.0f, 0.03f, 100.0f, 0.5f);
        LinkMotionApi::addCalc(&m36A0.z, 0.0f, 0.03f, 100.0f, 0.5f);
        gabi::Local<cXyz> t1;
        gabi::Local<cXyz> t2;
        cXyz_mi(&mOldSpeed, t1, &speed);
        cXyz_ml(t1, t2, 0.75f);
        LinkMotionApi::addVector(&m36A0, t2.get(), &m36A0);
        cXyz_mi(&mOldSpeed, t2, &speed);
        cXyz_ml(t2, t1, 0.25f);
        LinkMotionApi::addVector(&speed, t1.get(), &speed);
        m36A0.y = 0.0f;
    } else {
        m36A0.x = 0.0f;
        m36A0.z = 0.0f;
        m36A0.y = 0.0f;
    }
    setBeltConveyerPower();
    setWindAtPower();
    f32 oldY = speed.y;
    f32 sy;
    if (checkHeavyStateOn()) {
        f32 lim = maxFallSpeed * 1.5f;
        sy = gabi::fmadds(gravity, 2.25f, speed.y);
        speed.y = sy;
        if (sy < lim) {
            sy = lim;
            speed.y = sy;
        }
    } else {
        sy = speed.y + gravity;
        f32 lim = maxFallSpeed;
        speed.y = sy;
        if (sy < lim) {
            sy = lim;
            speed.y = sy;
        }
    }
    if (!(sy > 0.0f) && oldY > 0.0f) {
        m35F0 = current.pos.y;
    }
    LinkMotionApi::addVector(&current.pos, &speed, &current.pos);
    for (int i = 0; i < 2; i++) {
        LinkMotionFootData &footData =
            reinterpret_cast<LinkMotionFootData *>(mFootData)[i]; /* mFootData[i] */
        linkCopyPositionWords(gabi::ea(&footData.field_0x018), toe[i]);
        linkCopyPositionWords(gabi::ea(&footData.field_0x00C), heel[i]);
    }
    m359C = (f32)step;
}

/* 023FD438 */
BOOL daPy_lk_c::checkNoCollisionCorret() {

    if ((mModeFlg & 0x10402820) || LK_demoType == 1 /* TYPE_TOOL_e */ ||
        LK_demoMode == 0xA /* DEMO_OPEN_TREASURE_e */ || LK_demoMode == 0x1E /* DEMO_UNK_030_e */ ||
        (resetFlg0() & LinkMotionConstants::daPyRFlg0_CRAWL_AUTO_MOVE) ||
        (u16)eventInfo.mCommand == 3 /* eventInfo.checkCommandDoor() */) {
        return TRUE;
    }
    u32 proc = (u32)mCurProc;
    if (proc == 0x12 /* VERTICAL_JUMP */ || proc == 0x2A /* CRAWL_END */ ||
        proc == 0x85 /* HOOKSHOT_FLY */ || proc == 0x97 /* VOMIT_WAIT */ ||
        proc == 0xC1 /* DEMO_DOOR_OPEN */) {
        return TRUE;
    }
    return FALSE;
}

/* dComIfGp_setNextStage(name, point, roomNo, layer, lastSpeed, mode, wipe?, ?) (0252012C) */
static inline void lk_setNextStage(u32 name, s32 point, s32 room, u32 mode, s32 a8, s32 a9) {
    LinkMotionApi::setNextStage(name, point, room, -1, 0.0f, mode, a8, a9);
}

/* 023FD4E4 */
BOOL daPy_lk_c::startRestartRoom(u32 mode, int eventInfoIdx, f32 damage, int point) {

    if ((s8)LinkActionPlay().mNextStage.enable != 0 /* HD */ ||
        (mNoResetFlg0 & LinkMotionConstants::daPyFlg0_UNK4000)) {
        return FALSE;
    }
    if (point == 0 && !linkOrderCompulsoryEvent(this)) {
        return FALSE;
    }
    LK_demoType = 3; /* mDemo.setOriginalDemoType() */
    if (point == 0) {
        LK_demoMode = 1; /* DEMO_N_WAIT_e */
    }
    mNoResetFlg0 = mNoResetFlg0 | 0x4000;
    LK_changePlayer();
    if ((u8)LinkActionPlay().mMiniGameType /* dComIfGp_getMiniGameType() */ == 1) {
        lk_setNextStage(gabi::guest_addr("sea"), 1, LinkMotionConstants::dIsleRoom_BoatingCourse_e,
                        0, 1, 0);
        LinkActionApi::seStartSystem(LinkMotionConstants::JA_SE_FORCE_BACK);
        linkMotionShipRaceResult() = 3; /* daNpc_Sarace_c::ship_race_result */
        mTinkleShieldTimer = 0;
        return TRUE;
    }
    u32 stType =
        ((u32) reinterpret_cast<LinkMotionStageInfo *>(dComIfGp_getStageStagInfo())->mStageFlags >>
         16) &
        7; /* dStage_stagInfo_GetSTType() */
    setDamagePoint(damage);
    mTinkleShieldTimer = 0;
    if (stType == LinkMotionConstants::dStageType_SEA_e &&
        !LinkMotionApi::isEventBit(gabi::ea(&LinkActionSave().mSavedata.mEvent), 0x2A08)) {
        s32 room = current.roomNo;
        if ((room == 0xB || room == 0x2C) && LinkMotionApi::chkPlayerId(0x80, room)) {
            u32 name = gabi::ea(LinkActionPlay().mCurStage.mName);
            lk_setNextStage(name, 0x80, (s32)current.roomNo, mode, 1, 0);
            u32 prm = setParamData(-1, 0, eventInfoIdx, 0);
            LinkActionSave().mRestart.mRestartParam =
                (u32)(prm); /* dComIfGs_setRestartRoomParam() */
            LinkActionApi::seStartSystem(0x8B3);
            return TRUE;
        }
    }
    bool nextStageSet = false;
    s32 pt;
    if ((mNoResetFlg0 & LinkMotionConstants::daPyFlg0_DEKU_SP_RETURN_FLG) &&
        mCurProc != 0xB2 /* DEMO_DEAD */) {
        pt = mDekuSpRestartPoint;
        if (pt != 0xFF) {
            u32 name = gabi::ea(LinkActionPlay().mCurStage.mName);
            lk_setNextStage(name, (s16)pt, 0x29, mode, 1, 0);
            nextStageSet = true;
        }
    } else {
        pt = mRestartPoint;
        if (pt != 0xFF && LinkMotionApi::checkSeaLandingEvent((s32)current.roomNo)) {
            u32 deku = mNoResetFlg0 & 0x10;
            u32 name = gabi::ea(LinkActionPlay().mCurStage.mName);
            if (deku) {
                lk_setNextStage(name, (s16)pt, 0x29, mode, 1, 0);
            } else {
                lk_setNextStage(name, (s16)pt, (s32)current.roomNo, mode, 1, 0);
            }
            nextStageSet = true;
        }
    }
    if (!nextStageSet) {
        if (stType == 7) {
            u32 pos;
            if (gabi::ea(dComIfGp_getShipActor()) != 0) {
                pos = gabi::ea(&dComIfGp_getShipActor()->current.pos);
            } else {
                pos = gabi::ea(&current.pos);
            }
            s32 qx = gabi::ftoi(((f32)gabi::at<cXyz>(pos)->x + 350000.0f) * 1.9999999e-05f);
            s32 qz = gabi::ftoi(((f32)gabi::at<cXyz>(pos)->z + 350000.0f) * 1.9999999e-05f);
            if (qx < 0) {
                qx = 0;
            } else if (qx > 13) {
                qx = 13;
            }
            if (qz < 0) {
                qz = 0;
            } else if (qz > 13) {
                qz = 13;
            }
            s32 sector = (qz >> 1) * 7 + (qx >> 1);
            s32 idx = (sector << 2) | (qx & 1) | ((qz & 1) << 1);
            LinkMotionApi::changeScene(idx, 0.0f, mode, -1);
        } else {
            if (mCurProc == 0xB2) {
                LinkMotionApi::changeScene(0, 0.0f, mode, -1);
            } else {
                u32 prm = setParamData((s8)(u8)LinkActionSave()
                                           .mRestart.mRestartRoom /* dComIfGs_getRestartRoomNo() */,
                                       0, eventInfoIdx, 0);
                LinkMotionApi::restartRoom(prm, mode);
                LinkActionApi::seStartSystem(0x8B3);
            }
            return TRUE;
        }
    }
    if (mCurProc != 0xB2) {
        u32 prm = setParamData(-1, 0, eventInfoIdx, 0);
        LinkActionSave().mRestart.mRestartParam = (u32)(prm);
        LinkActionApi::seStartSystem(0x8B3);
    }
    return TRUE;
}

/* 023FDA70 */
void daPy_lk_c::posMove() {

    u32 me = gabi::ea(this);
    gabi::Local<cXyz> start; /* m3700 before the move */
    f32 sz = m3700.z;
    u32 oldFrame = m_old_fdata;
    u32 pb0 = m_pbCalc[0];
    f32 sx = m3700.x;
    f32 sy = m3700.y;
    start->z = sz;
    start->x = sx;
    start->y = sy;
    u32 ot = (u32)gabi::at<LinkMotionOldFrame>(oldFrame)
                 ->mOldFrameTransInfo.v; /* m_old_fdata->getOldFrameTransInfo(0) */
    setAnimeRatioHD();
    LinkMotionApi::calcUnderBlend(pb0, mpCLModel.get());
    u32 bt = (u32)linkMotionBlend(pb0).mpRootTransform.v; /* the blended root transform */
    u16 ang = (u16)shape_angle.y;
    f32 tz = (f32)gabi::at<LinkMotionRootTransform>(bt)->mTranslate.z;
    f32 ty = (f32)gabi::at<LinkMotionRootTransform>(bt)->mTranslate.y;
    f32 cs = cM_scos(ang);
    f32 tx = (f32)gabi::at<LinkMotionRootTransform>(bt)->mTranslate.x;
    f32 sn = cM_ssin(ang);
    u8 m = m34C2;
    if (m == 11) {
        gabi::Local<cXyz> d;
        cXyz_mi(&current.pos, d, &old.pos);
        f32 dz = d->z;
        f32 dx = d->x;
        f32 rx = gabi::fmsubs(cs, dx, sn * dz);
        f32 ox = (f32)gabi::at<LinkMotionTransform>(ot)->mTranslate.x - rx;
        f32 rz = gabi::fmadds(cs, dz, sn * dx);
        f32 oy = (f32)gabi::at<LinkMotionTransform>(ot)->mTranslate.y - d->y;
        gabi::at<LinkMotionTransform>(ot)->mTranslate.x = (f32)(ox);
        gabi::at<LinkMotionTransform>(ot)->mTranslate.y = (f32)(oy);
        gabi::at<LinkMotionTransform>(ot)->mTranslate.z =
            (f32)((f32)gabi::at<LinkMotionTransform>(ot)->mTranslate.z - rz);
    } else if (m == 10) {
        f32 dx = tx - (f32)gabi::at<LinkMotionTransform>(ot)->mTranslate.x;
        f32 dy = ty - (f32)gabi::at<LinkMotionTransform>(ot)->mTranslate.y;
        f32 dz = tz - (f32)gabi::at<LinkMotionTransform>(ot)->mTranslate.z;
        gabi::Local<cXyz> sv;
        sv->x = gabi::fmadds(sn, dz, cs * dx);
        sv->y = dy;
        sv->z = gabi::fmsubs(cs, dz, sn * dx);
        LinkMotionApi::subtractVector(&current.pos, sv.get(), &current.pos);
        LinkMotionApi::subtractVector(&old.pos, sv.get(), &old.pos);
        gabi::at<LinkMotionTransform>(ot)->mTranslate.z = (f32)(tz);
        gabi::at<LinkMotionTransform>(ot)->mTranslate.x = (f32)(tx);
        gabi::at<LinkMotionTransform>(ot)->mTranslate.y = (f32)(ty);
    } else if (m == 2 || m == 6) {
        gabi::at<LinkMotionTransform>(ot)->mTranslate.x = (f32)(tx);
        gabi::at<LinkMotionTransform>(ot)->mTranslate.z = (f32)(tz);
        if (m34C2 == 6) {
            gabi::at<LinkMotionTransform>(ot)->mTranslate.y = (f32)(ty);
        }
    } else if (m == 4) {
        start->y = ty;
        start->x = 0.0f;
        start->z = 0.0f;
        gabi::at<LinkMotionTransform>(ot)->mTranslate.x = (f32)(0.0f);
        gabi::at<LinkMotionTransform>(ot)->mTranslate.z = (f32)(0.0f);
        gabi::at<LinkMotionTransform>(ot)->mTranslate.y = (f32)(ty);
        m34C2 = 5;
        m35E0 = ty;
    } else if (m == 7 || m == 3) {
        start->x = tx;
        start->y = ty;
        start->z = tz;
        m34C2 = m == 7 ? 5 : 1;
    }
    m3700.y = ty;
    m3700.z = tz;
    m3700.x = tx;
    posMoveFromFootPos();
    if (!checkNoCollisionCorret()) {
        LinkMotionApi::addVector(&current.pos, &mStts.m_cc_move, &current.pos);
        if (!dComIfGp_event_runCheck() && LK_demoType == 0) {
            if (!(noResetFlg1() & LinkMotionConstants::daPyFlg1_UNK10000000) ||
                !checkHeavyStateOn()) {
                u16 a = (u16)m3640;
                f32 k = m3644;
                current.pos.x = gabi::fmadds(k, cM_ssin(a), current.pos.x);
                current.pos.z = gabi::fmadds(k, cM_scos(a), current.pos.z);
            }
            u32 whirl = 0;
            u32 wid = mWhirlId;
            if (wid != 0xFFFFFFFF) {
                gabi::Local<be<u32>> id;
                *id.get() = wid;
                whirl = LinkMotionApi::judgeActor(0x025E1234, id.get());
            }
            if (whirl != 0) {
                gabi::Local<cXyz> d;
                cXyz_mi(&current.pos, d, &gabi::at<fopAc_ac_c>(whirl)->current.pos);
                u16 a = (u16)(cM_atan2s(d->x, d->z) + 0x5000);
                cLib_chaseF(&m3610, 40.0f, 5.0f);
                f32 k = m3610;
                current.pos.x = gabi::fmadds(k, cM_ssin(a), current.pos.x);
                gabi::Local<cXyz> xz;
                xz->y = 0.0f;
                xz->x = d->x;
                xz->z = d->z;
                current.pos.z = gabi::fmadds(k, cM_scos(a), current.pos.z);
                f64 len = LinkActionApi::sqrtWide(LinkActionApi::squareMagnitude(xz.get()));
                if (len < 500.0) {
                    startRestartRoom(5, 0xC9, -1.0f, 0);
                }
            } else {
                m3610 = 0.0f;
            }
            if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) &&
                LinkMotionApi::chkPolySafe(dComIfG_Bgsp(),
                                           gabi::ea(static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd)))) {
                u16 g = (u16)LinkMotionApi::getGroundAngle(
                    this, gabi::ea(static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd)), 0);
                current.pos.z = gabi::fmadds(m36A0.z, cM_scos(g), current.pos.z);
                g = (u16)LinkMotionApi::getGroundAngle(
                    this, gabi::ea(static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd)), 0x4000);
                current.pos.x = gabi::fmadds(m36A0.x, cM_scos(g), current.pos.x);
            }
            LinkMotionApi::addVector(&current.pos, &m36B8, &current.pos);
            if (mCurProc != LinkMotionConstants::daPyProc_FAN_GLIDE_e) {
                current.pos.x = current.pos.x + m3730.x;
                current.pos.z = current.pos.z + m3730.z;
            }
        }
    } else if (!dComIfGp_event_runCheck() && LK_demoType == 0 &&
               (u32)LinkActionPlay().mPlayerInfo[0].mpPlayer.v == me &&
               mCurProc == LinkMotionConstants::daPyProc_HANG_MOVE_e) {
        gabi::Local<cXyz> xz;
        xz->x = (f32)mStts.m_cc_move.x;
        xz->y = 0.0f;
        xz->z = (f32)mStts.m_cc_move.z;
        f64 len = LinkActionApi::sqrtWide(LinkActionApi::squareMagnitude(xz.get()));
        if (len > 1.0) {
            s16 a = cM_atan2s((f32)mStts.m_cc_move.x, (f32)mStts.m_cc_move.z);
            u16 s0 = (u16)shape_angle.y;
            f32 c = cM_scos(s0);
            if ((s16)(a - s0) >= 0) {
                current.pos.x = (f32)(len * (f64)c + (f64)(f32)current.pos.x);
                f32 sv = cM_ssin((u16)shape_angle.y);
                current.pos.z = (f32)(-(len * (f64)sv - (f64)(f32)current.pos.z));
            } else {
                current.pos.x = (f32)(-(len * (f64)c - (f64)(f32)current.pos.x));
                f32 sv = cM_ssin((u16)shape_angle.y);
                current.pos.z = (f32)(len * (f64)sv + (f64)(f32)current.pos.z);
            }
        }
    }
    m = m34C2;
    mStts.m_cc_move.x = 0.0f; /* mStts.ClrCcMove() */
    mStts.m_cc_move.y = 0.0f;
    m3644 = 0.0f;
    mStts.m_cc_move.z = 0.0f;
    gabi::Local<cXyz> d;
    if (m == 1 || m == 5) {
        cXyz_mi(&m3700, d, start);
    } else if (m >= 8 && m <= 9) {
        cXyz_mi(start, d, &m3700);
    } else {
        return;
    }
    f32 dx = d->x;
    f32 dz = d->z;
    current.pos.x = current.pos.x + gabi::fmadds(dz, sn, dx * cs);
    current.pos.z = current.pos.z + gabi::fmsubs(dz, cs, dx * sn);
    if (m34C2 == 5) {
        current.pos.y = current.pos.y + d->y;
    }
}

/* dPa_control_c::mStatus : onStatus(1) / offStatus(1) */
static inline void lk_paStatus(bool on) {
    u8 v = (u8)LinkMotionGlobals::particleStatus();
    LinkMotionGlobals::particleStatus() = (u8)(on ? (u8)(v | 1) : (u8)(v & ~1));
}

/* 023FE224 */
void daPy_lk_c::setWaterY() {

    if (!LinkMotionApi::seaChkArea((f32)current.pos.x, (f32)current.pos.z)) {
        u32 flg = mNoResetFlg0;
        f32 h = (f32) reinterpret_cast<LinkMotionWater *>(&mAcch.m_wtr)
                    ->m_height; /* mAcch.m_wtr.GetHeight() */
        if (mAcch.m_flags & dBgS_Acch::WATER_HIT) {
            mNoResetFlg0 = flg | 0x80;
        } else {
            mNoResetFlg0 = flg & ~0x80u;
        }
        mWaterY = h;
        lk_paStatus(false);
        return;
    }
    mNoResetFlg0 = mNoResetFlg0 | 0x80;
    f64 w = LinkMotionApi::seaCalcWave((f32)current.pos.x, (f32)current.pos.z);
    f32 h = (f32) reinterpret_cast<LinkMotionWater *>(&mAcch.m_wtr)->m_height;
    mWaterY = (f32)w;
    if ((f64)h > w) {
        mWaterY = h;
        lk_paStatus(false);
    } else {
        lk_paStatus(true);
    }
}

/* 023FE2F0 */
void daPy_lk_c::autoGroundHit() {

    u32 flg = mNoResetFlg0;
    u32 was = flg & LinkMotionConstants::daPyFlg0_UNK80000000;
    flg = flg & ~0x80000000u;
    mNoResetFlg0 = flg;
    if (mTinkleHoverTimer == 0) {
        mNoResetFlg0 = flg & ~0x1000u; /* offNoResetFlg0(daPyFlg0_HOVER_BOOTS) */
    }
    if (!dComIfGp_event_runCheck() && LK_demoType == 0 && mTinkleHoverTimer > 0) {
        mTinkleHoverTimer = (s16)(mTinkleHoverTimer - 1);
    }
    if (mModeFlg & 0x10452822 /* checkPlayerFly() */) {
        return;
    }
    u32 acchFlg = mAcch.m_flags;
    f32 gh = mAcch.m_ground_h;
    f32 d = gh - current.pos.y;
    if (acchFlg & dBgS_Acch::GROUND_HIT) {
        return;
    }
    if (d < 0.0f && !(d < -30.1f) && was == 0) {
        speed.y = 0.0f;
        current.pos.y = gh;
        mAcch.m_flags = mAcch.m_flags | dBgS_Acch::GROUND_HIT;
        return;
    }
    if ((mNoResetFlg0 & 0x1000) && mTinkleHoverTimer > 0 &&
        (!dComIfGp_event_runCheck() || LK_demoType == 5)) {
        mNoResetFlg0 = mNoResetFlg0 | 0x80000000;
        speed.y = 0.0f;
        f32 oy = old.pos.y;
        mAcch.m_flags = mAcch.m_flags | dBgS_Acch::GROUND_HIT;
        current.pos.y = oy;
    }
}

/* 023FE470 */
BOOL daPy_lk_c::procLavaDamage_init() {

    if (mCurProc == LinkMotionConstants::daPyProc_LAVA_DAMAGE_e) {
        if (!(speed.y > 0.0f)) {
            speed.y = 30.0f;
        }
        gravity = -2.5f;
        return TRUE;
    }
    commonProcInit(0x6B);
    gabi::Local<cXyz> xz; /* current.pos.absXZ() */
    xz->x = current.pos.x;
    xz->y = 0.0f;
    xz->z = current.pos.z;
    f64 len = LinkActionApi::sqrtWide(LinkActionApi::squareMagnitude(xz.get()));
    f32 k = gabi::fmsubs((f32)(1700.0 - len), 0.00058823527f, 0.25f);
    gravity = -2.5f;
    k = linkFloatSelect(k, k, 0.0f);
    mNormalSpeed = gabi::fmadds(40.0f, k, 15.0f);
    speed.y = gabi::fmadds(36.0f, k, 32.0f);
    setSingleMoveAnime(LinkMotionConstants::ANM_LAVADAM, 1.0f, 0.0f, -1, 5.0f);
    u32 mtx = linkGetAnmMtx(gabi::ea(mpCLModel.get()), 0 /* CL_JNT_LINK_ROOT_e */);
    LinkActionApi::makeEmitter(&m32E4, LinkMotionConstants::ID_AK_SN_HIDARUMAFIRE, mtx,
                               &current.pos, nullptr);
    LinkActionVoiceStart(this, 4);
    seStartMapInfo(LinkMotionConstants::JA_SE_LK_FALL_MAGMA);
    current.angle.y = cM_atan2s(current.pos.x, current.pos.z);
    mDamageWaitTimer = 0x1E;
    setDamagePoint(-1.0f);
    return TRUE;
}

/* 023FE62C */
void daPy_lk_c::dProcLavaDamage_init_sub() {

    mProcVar6 = 0;
    speed.y = 80.0f;
    LinkActionVoiceStart(this, 4);
    seStartMapInfo(LinkMotionConstants::JA_SE_LK_FALL_MAGMA);
    LK_demoType = 5;
    gravity = -2.5f;
    dComIfGp_setPlayerStatus0(0, LinkMotionConstants::daPyStts0_UNK10000000_e);
    LK_changePlayer();
}

/* 023FE6B8 */
BOOL daPy_lk_c::dProcLavaDamage_init() {

    if (mCurProc == LinkMotionConstants::daPyProc_DEMO_LAVA_DAMAGE_e) {
        return TRUE;
    }
    commonProcInit(0xB0);
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(LinkMotionConstants::ANM_LAVADAM, 1.0f, 0.0f, -1, 5.0f);
    u32 mtx = linkGetAnmMtx(gabi::ea(mpCLModel.get()), 0);
    LinkActionApi::makeEmitter(&m32E4, 0x8078, mtx, &current.pos, nullptr);
    if (linkOrderCompulsoryEvent(this)) {
        dProcLavaDamage_init_sub();
    } else {
        speed.y = 0.0f;
        gravity = 0.0f;
        mProcVar6 = 1;
    }
    return TRUE;
}

/* 023FE7C0 */
BOOL daPy_lk_c::checkLavaFace(cXyz *oldPos, int attr) {

    u32 me = gabi::ea(this);
    if (LK_demoType == 5 /* checkSpecialDemoMode() */) {
        return FALSE;
    }
    if (oldPos != nullptr) {
        f32 z = current.pos.z;
        f32 y = oldPos->y + 20.0f;
        linkMotionGroundCheck(mLavaGndChk).m_pos.z = z; /* mLavaGndChk.SetPos() */
        linkMotionGroundCheck(mLavaGndChk).m_pos.x = current.pos.x;
        linkMotionGroundCheck(mLavaGndChk).m_pos.y = y;
        f64 gc = LinkActionApi::groundCross(dComIfG_Bgsp(), mLavaGndChk);
        f32 gh = mAcch.m_ground_h;
        f32 cy = current.pos.y;
        m35D4 = (f32)gc;
        if ((f64)gh > gc) {
            gc = -1000000000.0f;
            m35D4 = -1000000000.0f;
        }
        if (!(gc > (f64)cy)) {
            return FALSE;
        }
        attr = LinkMotionApi::getAttributeCode(
            dComIfG_Bgsp(),
            gabi::ea(static_cast<cBgS_PolyInfo *>(&linkMotionQueries(me).mLavaGndChk)));
    }
    if (attr == 6 /* dBgS_Attr_LAVA_e */) {
        if (oldPos != nullptr) {
            current.pos.y = m35D4;
        }
        if (LK_callB(0x023EB6C8 /* checkBossGomaStage */)) {
            return procLavaDamage_init();
        }
        return dProcLavaDamage_init();
    }
    if (attr == 8 /* dBgS_Attr_VOID_e */) {
        startRestartRoom(5, 0xC9, -1.0f, 0);
    }
    return FALSE;
}

/* 023FE918 */
BOOL daPy_lk_c::checkSwimFallCheck() {

    if (!(mNoResetFlg0 & 0x80) ||
        (current.pos.y > mWaterY + 30.1f &&
         !((u8)LinkMotionGlobals::particleStatus() & 1) /* !dPa_control_c::isStatus(1) */)) {
        return TRUE;
    }
    return FALSE;
}

/* 023FE960 */
int daPy_lk_c::setRoomInfo() {

    u32 me = gabi::ea(this);
    u32 gnd = gabi::ea(static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd)); /* mAcch.m_gnd */
    s32 room = LinkMotionApi::getRoomId(dComIfG_Bgsp(), gnd);
    tevStr.mRoomNo = (u8)((u8)room); /* tevStr.mRoomNo */
    u8 col = (u8)LinkMotionApi::getPolyColor(dComIfG_Bgsp(), gnd);
    tevStr.mEnvrIdxOverride = (u8)(col); /* tevStr.mEnvrIdxOverride */
    s32 rev = dComIfGp_getReverb(room);
    mStts.mRoomId = (u8)room;  /* mStts.SetRoomId() */
    current.roomNo = (u8)room; /* current.roomNo */
    mReverb = (s8)rev;
    if ((((u32) reinterpret_cast<LinkMotionStageInfo *>(dComIfGp_getStageStagInfo())->mStageFlags >>
          16) &
         7) == LinkMotionConstants::dStageType_SEA_e) {
        if (LinkMotionApi::chkMoveBG(dComIfG_Bgsp(), gnd)) {
            mRestartPoint = 0;
        } else {
            s32 link = LinkMotionApi::getLinkNo(dComIfG_Bgsp(), gnd);
            mRestartPoint = link;
            if (link == 0xFF) {
                // HD: standing on the actor 0x1C7 also restarts at point 0
                u32 ac =
                    LinkMotionApi::getActorPointer(dComIfG_Bgsp(), (u32)(u16)mAcch.m_gnd.mBgIndex);
                if (ac != 0 &&
                    (s16)gabi::at<LinkActionDamageActor>(ac)->mDamageActorType == 0x1C7) {
                    mRestartPoint = 0;
                }
            }
        }
    } else {
        mRestartPoint = 0xFF;
    }
    return room;
}

/* 023FEA6C */
void daPy_lk_c::checkFallCode() {

    u32 me = gabi::ea(this);
    f32 gh = mAcch.m_ground_h;
    if (m3580 == 4 || gh == -1000000000.0f) {
        u32 mf = mModeFlg;
        bool restart;
        if (!(mWaterY > gh)) {
            restart = (mf & LinkMotionConstants::ModeFlg_MIDAIR) &&
                      !(mf & LinkMotionConstants::ModeFlg_HOOKSHOT) &&
                      m35F4 - current.pos.y > 2000.0f;
        } else {
            restart = (mf & LinkMotionConstants::ModeFlg_SWIM) != 0;
        }
        if (restart || ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !(mNoResetFlg0 & 0xA0000000))) {
            startRestartRoom(5, 0xC9, -1.0f, 0);
            mAcch.m_flags = mAcch.m_flags & ~(u32)dBgS_Acch::GROUND_HIT; /* ClrGroundHit() */
        }
        return;
    }
    if ((mNoResetFlg0 & LinkMotionConstants::daPyFlg0_DEKU_SP_RETURN_FLG) && (mModeFlg & 0x40000)) {
        startRestartRoom(5, 0xC9, -1.0f, 0);
        return;
    }
    if ((u16)LinkActionPlay().mItemNowLife == 0) { // HD: the exits are disabled
        return;
    }
    u32 gnd = gabi::ea(static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd));
    s32 exitId = LinkMotionApi::getExitId(dComIfG_Bgsp(), gnd);
    s32 room = LinkMotionApi::getRoomId(dComIfG_Bgsp(), gnd);
    u32 ship = gabi::ea(dComIfGp_getShipActor());
    if (exitId == 0x3F) {
        return;
    }
    bool go = (mModeFlg & 0x40000) || (mAcch.m_flags & dBgS_Acch::GROUND_HIT) ||
              mCurProc == LinkMotionConstants::daPyProc_FAN_GLIDE_e;
    if (!go && dComIfGp_checkPlayerStatus0(0, 0x10000) && ship != 0 &&
        !((u32)gabi::at<LinkMotionShip>(ship)->mTornadoActor.v != 0 ||
          (u32)gabi::at<LinkMotionShip>(ship)->mWhirlActor.v != 0) /* !ship->checkForceMove() */) {
        go = true;
    }
    if (!go && !(m3580 == 5 && m35F4 - current.pos.y > 500.0f)) {
        return;
    }
    LinkActionPlay().mFwaterTimer = (u8)(1); /* dComIfGs_startFwaterTimer() */
    f32 sp = speedF;
    f32 spd;
    if (sp < 1.5f) {
        spd = 1.5f;
    } else {
        spd = linkFloatSelect(17.0f - sp, sp, 17.0f);
    }
    s32 type;
    if (dComIfGp_checkPlayerStatus0(0, 0x10000)) {
        type = 1;
        if (gabi::ea(dComIfGp_getShipActor()) != 0) {
            u32 sh = gabi::ea(dComIfGp_getShipActor());
            gabi::at<LinkMotionShip>(sh)->mStateFlg =
                gabi::at<LinkMotionShip>(sh)->mStateFlg | 0x20000000; /* ship->onSceneChange() */
        }
    } else if (mModeFlg & LinkMotionConstants::ModeFlg_CRAWL) {
        type = mFrameCtrlUnder[0].mRate < 0.0f ? 3 : 2;
    } else {
        type = 0;
    }
    u16 cmd = (u16)eventInfo.mCommand; /* eventInfo command */
    if (m3580 == 5) {
        mAcch.m_flags = mAcch.m_flags & ~(u32)dBgS_Acch::GROUND_HIT;
    }
    if (cmd != 3 /* !eventInfo.checkCommandDoor() */ && !linkOrderCompulsoryEvent(this)) {
        return;
    }
    if (!LinkMotionApi::changeSceneExitId(gnd, spd, type, (s32)(s8)room)) {
        return;
    }
    setNoResetFlg1(noResetFlg1() | LinkMotionConstants::daPyFlg1_UNK100000);
    if (LinkMotionApi::isEventBit(gabi::ea(&LinkActionSave().mSavedata.mEvent), 0x3E10)) {
        LinkMotionApi::onEventBit(gabi::ea(&LinkActionSave().mSavedata.mEvent), 0x3E01);
    }
    LK_changePlayer();
    if (linkIsStartStage(gabi::guest_addr("GanonM")) && current.roomNo == 1) {
        LinkActionApi::seStartSystem(LinkMotionConstants::JA_SE_LK_MAZE_WARP_IN);
    }
    if ((u16)eventInfo.mCommand == 3) {
        return;
    }
    LK_demoType = 3; /* mDemo.setOriginalDemoType() */
    if (m3580 == 5) {
        LK_demoMode = 0x11; /* DEMO_PFALL_e */
    } else if ((mModeFlg & 0x40000) || m34C3 == 1 || m34C3 == 4) {
        mNormalSpeed = mNormalSpeed * 0.75f;
        LK_demoMode = 0xE; /* DEMO_KEEP_e */
    } else {
        LK_demoMode = 2; /* DEMO_N_WALK_e */
    }
    gabi::Local<cXyz> d;
    cXyz_mi(&current.pos, d, &old.pos);
    f64 len = LinkActionApi::sqrtWide(LinkActionApi::squareMagnitude(d.get()));
    if (len > (f64)0.1f) {
        mDemo.mDemoMoveAngle = cM_atan2s(d->x, d->z); /* mDemo.setMoveAngle() */
    } else {
        mDemo.mDemoMoveAngle = current.angle.y;
    }
}

/* 023FF020 */
void daPy_lk_c::setShapeAngleOnGround() {

    u32 me = gabi::ea(this);
    if (mNoResetFlg0 & 0xA0000000) {
        return;
    }
    f32 k = m35E4;
    if (k < 0.0f) {
        k = 0.0f;
        m35E4 = k;
    } else if (k > 1.0f) {
        k = 1.0f;
        m35E4 = k;
    }
    s16 ax;
    if (mModeFlg & LinkMotionConstants::ModeFlg_CRAWL) {
        const u32 stk = gabi::ea(mDoMtx_stack_c::get()); /* mDoMtx_stack_c::now */
        LinkMotionApi::translateMatrix(gabi::at<Mtx34>(stk), (f32)current.pos.x, (f32)current.pos.y,
                                       (f32)current.pos.z);
        LinkMotionApi::rotateMatrixY(gabi::at<Mtx34>(stk), (s32)shape_angle.y);
        gabi::Local<cXyz> front;
        gabi::Local<cXyz> back;
        LinkMotionApi::multVec(gabi::at<Mtx34>(stk), LinkMotionGlobals::crawlFront(), front.get());
        LinkMotionApi::multVec(gabi::at<Mtx34>(stk), LinkMotionGlobals::crawlBack(), back.get());
        linkCopyPositionWords(gabi::ea(&linkMotionGroundCheck(mGndChk).m_pos),
                              front); /* mGndChk.SetPos(&front) */
        f64 gc = LinkActionApi::groundCross(dComIfG_Bgsp(), mGndChk);
        f32 ny0 = -1.0f;
        front->y = (f32)gc;
        u32 tp;
        if (gc != -1000000000.0 &&
            (tp = LinkMotionApi::getTriPla(
                 dComIfG_Bgsp(), (u32)(u16)linkMotionQueries(me).mGndChk.mBgIndex,
                 (u32)(u16)linkMotionQueries(me).mGndChk.mPolyIndex)) != 0) {
            ny0 = (f32)gabi::at<cM3dGPla>(tp)->mNormal.y;
        }
        linkCopyPositionWords(gabi::ea(&linkMotionGroundCheck(mGndChk).m_pos), back);
        gc = LinkActionApi::groundCross(dComIfG_Bgsp(), mGndChk);
        f32 ny1 = -1.0f;
        back->y = (f32)gc;
        if (gc != -1000000000.0 &&
            (tp = LinkMotionApi::getTriPla(
                 dComIfG_Bgsp(), (u32)(u16)linkMotionQueries(me).mGndChk.mBgIndex,
                 (u32)(u16)linkMotionQueries(me).mGndChk.mPolyIndex)) != 0) {
            ny1 = (f32)gabi::at<cM3dGPla>(tp)->mNormal.y;
        }
        gabi::Local<cXyz> d;
        cXyz_mi(front, d, back);
        gabi::Local<cXyz> xz;
        f32 dy = d->y;
        xz->x = d->x;
        xz->y = 0.0f;
        xz->z = d->z;
        f64 len = LinkActionApi::sqrtWide(LinkActionApi::squareMagnitude(xz.get()));
        s32 a = LinkMotionApi::atan2sYWide(-dy, len);
        if (cLib_distanceAngleS((s16)a, shape_angle.x) < 0x1000 && ny0 > 0.0f && ny1 > 0.0f &&
            std::fabs(ny0 - ny1) > 0.05f) {
            ax = (s16)gabi::ftoi((f32)a * m35E4);
        } else {
            ax = (s16)gabi::ftoi((f32)m34E2 * m35E4);
        }
    } else {
        ax = (s16)gabi::ftoi((f32)m34E2 * k);
    }
    shape_angle.x = ax;
    s32 g =
        LinkMotionApi::getGroundAngle(this, gabi::ea(static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd)),
                                      (s32)(s16)(shape_angle.y - 0x4000));
    shape_angle.z = (s16)gabi::ftoi((f32)g * m35E4);
}

/* 023FF47C */
void daPy_lk_c::setStepsOffset() {

    LinkMotionApi::addCalc(&m35C4, 0.0f, 0.5f, 25.0f, 5.0f);
    u16 a = (u16)m34E2;
    f32 t = cM_ssin(a) / cM_scos(a);
    f32 sp = speedF;
    u16 ay = (u16)current.angle.y;
    f32 px = gabi::fmadds(sp, cM_ssin(ay), current.pos.x);
    f32 py = current.pos.y + 30.1f;
    f32 pz = gabi::fmadds(sp, cM_scos(ay), current.pos.z);
    linkMotionGroundCheck(mGndChk).m_pos.x = px; /* mGndChk.SetPos() */
    linkMotionGroundCheck(mGndChk).m_pos.y = py;
    linkMotionGroundCheck(mGndChk).m_pos.z = pz;
    f64 gc = LinkActionApi::groundCross(dComIfG_Bgsp(), mGndChk);
    u32 flg = mNoResetFlg0 & 0x80000000;
    f32 y = current.pos.y;
    f32 sp2 = speedF;
    if (flg && (f64)y > gc) {
        gc = y;
    }
    f32 diff = gabi::fmadds(sp2, t, (f32)(gc - (f64)y));
    if (diff > 0.0f) {
        current.pos.y = (f32)gc;
        m35C4 = gabi::fnmsubs(diff, 0.7f, m35C4);
    } else if (!flg) {
        gabi::Local<cXyz> d;
        cXyz_mi(&old.pos, d, &current.pos);
        gabi::Local<cXyz> xz;
        f32 dy = d->y;
        xz->x = d->x;
        xz->y = 0.0f;
        xz->z = d->z;
        f64 len = LinkActionApi::sqrtWide(LinkActionApi::squareMagnitude(xz.get()));
        f32 f = (f32)(-(len * (f64)t - (f64)dy)); /* fnmsubs */
        if (!(f < 0.0f)) {
            m35C4 = gabi::fmadds(f, 0.7f, m35C4);
        }
    }
}

/* mpCLModel->setBaseScale(scale); mpCLModel->setBaseTRMtx(mDoMtx_stack_c::get()) (float copies) */
static inline void lk_setModelBase(u32 me, u32 stk) {
    u32 model = (u32)gabi::at<daPy_lk_c>(me)->mpCLModel.v;
    f32 sz = (f32)gabi::at<fopAc_ac_c>(me)->scale.z;
    f32 sy = (f32)gabi::at<fopAc_ac_c>(me)->scale.y;
    f32 sx = (f32)gabi::at<fopAc_ac_c>(me)->scale.x;
    gabi::at<J3DModel>(model)->mBaseScale.z = (f32)(sz);
    gabi::at<J3DModel>(model)->mBaseScale.y = (f32)(sy);
    gabi::at<J3DModel>(model)->mBaseScale.x = (f32)(sx);
    f32 m[12];
    for (int i = 0; i < 12; i++) {
        m[i] = (f32) reinterpret_cast<be<f32> *>(gabi::at<Mtx34>(stk))[i];
    }
    model = (u32)gabi::at<daPy_lk_c>(me)->mpCLModel.v;
    for (int i = 0; i < 12; i++) {
        reinterpret_cast<be<f32> *>(&gabi::at<J3DModel>(model)->mBaseTransformMtx)[i] = m[i];
    }
}

/* 023FF6A0 */
void daPy_lk_c::setWorldMatrix() {

    u32 me = gabi::ea(this);
    const u32 stk = gabi::ea(mDoMtx_stack_c::get()); /* mDoMtx_stack_c::now */
    f32 ty = (current.pos.y + m35C4) + m3608;
    LinkMotionApi::translateMatrix(gabi::at<Mtx34>(stk), (f32)current.pos.x, ty,
                                   (f32)current.pos.z);
    u32 ship = gabi::ea(dComIfGp_getShipActor());
    if (dComIfGp_checkPlayerStatus0(0, 0x10000) && ship != 0) {
        LinkMotionApi::rotateMatrixZXY(gabi::at<Mtx34>(stk), (s32)m353C,
                                       (s32)(s16)gabi::at<fopAc_ac_c>(ship)->shape_angle.y,
                                       (s32)m353E);
        LinkMotionApi::rotateMatrixY(
            gabi::at<Mtx34>(stk),
            (s32)(s16)(shape_angle.y - (s16)gabi::at<fopAc_ac_c>(ship)->shape_angle.y));
        gabi::Local<Mtx34> rel;
        LinkMotionApi::copyMatrix(gabi::at<Mtx34>(stk), rel.get());
        lk_setModelBase(me, stk);
        LinkMotionApi::inverseMatrix(gabi::at<Mtx34>(stk), gabi::at<Mtx34>(stk));
        LinkMotionApi::copyMatrix(gabi::at<Mtx34>(stk), &m37B4);
        mDoMtx_YrotS(gabi::at<Mtx34>(stk), (s16)-shape_angle.y);
        LinkMotionApi::concatMatrix(gabi::at<Mtx34>(stk), rel.get(), gabi::at<Mtx34>(stk));
        gabi::Local<cXyz> v;
        LinkMotionApi::multVecSR(gabi::at<Mtx34>(stk), linkMotionBaseZ(), v.get());
        shape_angle.x = cM_atan2s(-v->y, v->z);
        LinkMotionApi::multVecSR(gabi::at<Mtx34>(stk), linkMotionBaseX(), v.get());
        f32 vz = v->z;
        f32 vy = v->y;
        f64 len = LinkActionApi::sqrtWide(gabi::fmadds(vy, vy, vz * vz));
        f32 y2 = v->y;
        f32 x2 = v->x;
        if (!(y2 < 0.0f)) {
            shape_angle.z = (s16)LinkMotionApi::atan2sXWide(len, x2);
        } else {
            shape_angle.z = (s16)LinkMotionApi::atan2sXWide(-len, x2);
        }
        return;
    }
    if (mModeFlg & LinkMotionConstants::ModeFlg_CLIMB) {
        LinkMotionApi::rotateMatrixZXY(gabi::at<Mtx34>(stk), (s32)shape_angle.x, (s32)shape_angle.y,
                                       (s32)shape_angle.z);
        LinkMotionApi::translateStack(0.0f, 0.0f, 20.5f);
        LinkMotionApi::rotateMatrixY(gabi::at<Mtx34>(stk), (s32)m34EC);
        LinkMotionApi::translateStack(0.0f, 0.0f, -20.5f);
    } else {
        LinkMotionApi::rotateMatrixZXY(gabi::at<Mtx34>(stk), (s32)shape_angle.x,
                                       (s32)(s16)(shape_angle.y + m34EC), (s32)shape_angle.z);
    }
    lk_setModelBase(me, stk);
    LinkMotionApi::inverseMatrix(gabi::at<Mtx34>(stk), gabi::at<Mtx34>(stk));
    LinkMotionApi::copyMatrix(gabi::at<Mtx34>(stk), &m37B4);
}

/* 023FFB08 */
void daPy_lk_c::setWaistAngle() {

    s16 target;
    if (mModeFlg & 0x1045A823) {
        target = 0;
    } else {
        f32 k = std::fabs(mNormalSpeed / mMaxNormalSpeed);
        if (k > 1.0f) {
            k = 1.0f;
        }
        if (m3580 == 8) {
            target = 0;
        } else {
            target = (s16)gabi::ftoi(((f32)m34E2 * 0.7f) * k);
        }
    }
    cLib_addCalcAngleS(&m34E0, target, 2, 0x800, 0x200);
}

/* 023FFBE0 */
int daPy_lk_c::setLegAngle(f32 height, int foot, s16 *outUpper, s16 *outLower) {

    const u32 stk = gabi::ea(mDoMtx_stack_c::get()); /* mDoMtx_stack_c::now */
    const f64 zero = 3.814697265625e-06;             /* cM3d_IsZero */
    if (std::fabs(height) < 0.1f) {
        return FALSE;
    }
    f32 fwd = height * 0.5f;
    if (fwd > 10.0f) {
        fwd = 10.0f;
    }
    LinkMotionFootData &footData =
        reinterpret_cast<LinkMotionFootData *>(mFootData)[foot]; /* mFootData[foot] */
    gabi::Local<cXyz> hip;                                       /* spE8 */
    gabi::Local<cXyz> knee;                                      /* spDC */
    gabi::Local<cXyz> ankle;                                     /* spD0 */
    gabi::Local<cXyz> tmp;
    LinkMotionApi::concatMatrix(&m37B4, &footData.field_0x088[0], gabi::at<Mtx34>(stk));
    hip->y = (f32)gabi::at<Mtx34>(stk)->m[1][3];
    hip->x = 0.0f;
    hip->z = (f32)gabi::at<Mtx34>(stk)->m[2][3];
    LinkMotionApi::concatMatrix(&m37B4, &footData.field_0x088[1], gabi::at<Mtx34>(stk));
    knee->z = (f32)gabi::at<Mtx34>(stk)->m[2][3];
    knee->x = 0.0f;
    knee->y = (f32)gabi::at<Mtx34>(stk)->m[1][3];
    LinkMotionApi::concatMatrix(&m37B4, &footData.field_0x088[2], gabi::at<Mtx34>(stk));
    f32 ay = (f32)gabi::at<Mtx34>(stk)->m[1][3] + 3.25f;
    ankle->x = 0.0f;
    ankle->z = (f32)gabi::at<Mtx34>(stk)->m[2][3];
    ankle->y = ay;
    gabi::Local<cXyz> upper; /* spAC */
    gabi::Local<cXyz> lower; /* spA0 */
    cXyz_mi(knee, tmp, hip);
    linkCopyPositionWords(gabi::ea(upper.get()), tmp);
    cXyz_mi(ankle, tmp, knee);
    linkCopyPositionWords(gabi::ea(lower.get()), tmp);
    gabi::Local<cXyz> target; /* spB8 */
    f32 tz = ankle->z + fwd;
    f32 ty = ankle->y + height;
    target->z = tz;
    target->y = ty;
    target->x = ankle->x;
    if (!(ty < hip->y)) {
        return FALSE;
    }
    gabi::Local<cXyz> d; /* sp7C */
    cXyz_mi(target, tmp, hip);
    linkCopyPositionWords(gabi::ea(d.get()), tmp);
    f64 dd = LinkActionApi::squareMagnitude(d.get());
    if (std::fabs(dd) < zero) {
        return FALSE;
    }
    f64 uu = LinkActionApi::squareMagnitude(upper.get());
    f64 ll = LinkActionApi::squareMagnitude(lower.get());
    f64 su = LinkActionApi::sqrtWide(uu);
    f64 sl = LinkActionApi::sqrtWide(ll);
    f32 sum = (f32)(su + sl);
    f64 sd = LinkActionApi::sqrtWide(dd);
    if (!((f64)sum > sd)) {
        return FALSE;
    }
    f32 a1 = (f32)(dd + uu);
    f32 a2 = (f32)(dd + dd);
    f32 a3 = (f32)((f64)a1 - ll);
    f32 k = a3 / a2;
    f32 kd = (f32)(dd * (f64)k);
    f32 h2 = (f32)(-((f64)kd * (f64)k - uu)); /* fnmsubs */
    f32 my = gabi::fmadds(k, d->y, hip->y);
    h2 = linkFloatSelect(h2, h2, 0.0f);
    f32 mz = gabi::fmadds(k, d->z, hip->z);
    f64 h = LinkActionApi::sqrtWide(h2);
    gabi::Local<cXyz> n; /* sp70 */
    f32 dz = d->z;
    f32 dy = d->y;
    n->x = 0.0f;
    n->y = dz;
    n->z = -dy;
    f64 nl = LinkActionApi::sqrtWide(LinkActionApi::squareMagnitude(n.get()));
    if (std::fabs(nl) < zero) {
        return FALSE;
    }
    f32 r = (f32)(h / nl);
    gabi::Local<cXyz> kneeNew; /* spC4 */
    kneeNew->x = 0.0f;
    kneeNew->y = gabi::fmadds(r, n->y, my);
    kneeNew->z = gabi::fmadds(r, n->z, mz);
    cXyz_mi(kneeNew, tmp, hip);
    f32 uy = tmp->y;
    f32 uz = tmp->z;
    cXyz_mi(target, tmp, kneeNew);
    f32 ly = tmp->y;
    f32 lz = tmp->z;
    LinkMotionApi::concatMatrix(
        &m37B4, gabi::at<Mtx34>(linkGetAnmMtx(gabi::ea(mpCLModel.get()), 0)), gabi::at<Mtx34>(stk));
    s16 root = cM_atan2s(-(f32)gabi::at<Mtx34>(stk)->m[1][3], -(f32)gabi::at<Mtx34>(stk)->m[2][3]);
    s16 aU = cM_atan2s(uy, uz);
    s16 aL = cM_atan2s(ly, lz);
    s16 diff = (s16)(aU - root);
    if (diff > 0x6000) {
        aU = (s16)(root + 0x6000);
    } else if (diff < -0x4000) {
        aU = (s16)(root - 0x4000);
    }
    s16 diff2 = (s16)(aL - aU);
    if (diff2 > 0) {
        *reinterpret_cast<be<s16> *>(outUpper) = ((s16)(cM_atan2s(upper->y, upper->z) - aU));
        *reinterpret_cast<be<s16> *>(outLower) = ((s16)(cM_atan2s(lower->y, lower->z) - aU));
    } else {
        if (diff2 < -0x7000) {
            aL = (s16)(aU - 0x7000);
        }
        *reinterpret_cast<be<s16> *>(outUpper) = ((s16)(cM_atan2s(upper->y, upper->z) - aU));
        *reinterpret_cast<be<s16> *>(outLower) = ((s16)(cM_atan2s(lower->y, lower->z) - aL));
    }
    return TRUE;
}

/* 0240005C */
void daPy_lk_c::footBgCheck() {

    const u32 stk = gabi::ea(mDoMtx_stack_c::get()); /* mDoMtx_stack_c::now */
    u32 model = gabi::ea(mpCLModel.get());
    u32 base = model != 0 ? model + 0xC8 : 0; /* mpCLModel->getBaseTRMtx() */
    u32 mode1 = mModeFlg & 1;
    LinkMotionApi::concatMatrix(&m37B4, gabi::at<Mtx34>(linkGetAnmMtx(model, 0x1E)),
                                gabi::at<Mtx34>(stk));
    u16 wa = (u16)m34E0;
    f32 sn = cM_ssin(wa);
    f32 cs = cM_scos(wa);
    f32 h[2];
    u32 flag[2];
    gabi::Local<cXyz> p;
    gabi::Local<cXyz> t1;
    gabi::Local<cXyz> t2;
    gabi::Local<cXyz> xz;
    gabi::Local<cXyz> w;
    for (int i = 0; i < 2; i++) {
        LinkMotionFootData &footData =
            reinterpret_cast<LinkMotionFootData *>(mFootData)[i]; /* mFootData[i] */
        linkAddPosition(gabi::ea(&footData.field_0x018), t1, gabi::ea(&footData.field_0x00C));
        cXyz_ml(t1, t2, 0.5f);
        linkCopyPositionWords(gabi::ea(p.get()), t2);
        cXyz_mi(p, t1, &footData.field_0x024);
        xz->z = t1->z;
        xz->x = t1->x;
        xz->y = 0.0f;
        f64 m = LinkActionApi::squareMagnitude(xz.get());
        if (m < 100.0 && mode1) {
            u8 tm = (u8)footData.field_0x001;
            if (tm != 0) {
                footData.field_0x001 = (u8)((u8)(tm - 1));
            } else {
                p->x = (f32)footData.field_0x024.x;
                p->y = (f32)footData.field_0x024.y;
                p->z = (f32)footData.field_0x024.z;
            }
        } else {
            footData.field_0x001 = (u8)(5);
        }
        f32 px = p->x;
        f32 py = p->y;
        f32 pz = p->z;
        footData.field_0x024.y = (f32)(py);
        footData.field_0x024.x = (f32)(px);
        footData.field_0x024.z = (f32)(pz);
        f32 m13 = (f32)gabi::at<Mtx34>(stk)->m[1][3];
        f32 m23 = (f32)gabi::at<Mtx34>(stk)->m[2][3];
        f32 lift = gabi::fmadds(p->z - m23, sn, gabi::fmadds(p->y - m13, cs, m13));
        LinkMotionApi::multVec(gabi::at<Mtx34>(base), p.get(), w.get());
        f32 gy = current.pos.y + 30.1f;
        footData.field_0x034.m_pos.x = (f32)(w->x); /* field_0x034.SetPos() */
        footData.field_0x034.m_pos.y = (f32)(gy);
        footData.field_0x034.m_pos.z = (f32)(w->z);
        f64 gc = LinkActionApi::groundCross(dComIfG_Bgsp(), &footData.field_0x034);
        flag[i] = 0;
        if (mNoResetFlg0 & 0x80000000) {
            f32 y = current.pos.y;
            if ((f64)y > gc) {
                gc = y;
                flag[i] = 1;
            }
        }
        if (gc != -1000000000.0 && (f32)((f64)gy - gc) < 60.2f) {
            h[i] = (f32)gc;
            footData.field_0x000 = (u8)(1);
        } else {
            h[i] = current.pos.y;
            footData.field_0x000 = (u8)(0);
        }
        f32 v = h[i];
        if (mNoResetFlg0 & 0xA0000000) {
            f32 y = current.pos.y;
            if (!(v > y)) {
                v = y;
                h[i] = v;
            }
        }
        h[i] = v - (p->y - lift);
    }
    gabi::Local<be<s16>> upper[2];
    gabi::Local<be<s16>> lower[2];
    bool none = !(mAcch.m_flags & dBgS_Acch::GROUND_HIT) || (mModeFlg & 0x1045A822);
    int j = 0;
    f32 target;
    if (none) {
        target = 0.0f;
    } else {
        j = h[0] > h[1] ? 1 : 0;
        if (mCurProc == LinkMotionConstants::daPyProc_DEMO_TOOL_e || std::fabs((f32)m35C4) > 1.0f) {
            target = 0.0f;
        } else {
            target = linkFloatSelect(h[1] - h[0], h[0], h[1]) - current.pos.y;
        }
    }
    LinkMotionApi::addCalc(&m35B8, target, 0.5f, 7.5f, 2.5f);
    gabi::at<Mtx34>(base)->m[1][3] = (f32)((f32)gabi::at<Mtx34>(base)->m[1][3] + m35B8);
    m37B4.m[1][3] = m37B4.m[1][3] - m35B8;
    if (none) {
        for (int i = 0; i < 2; i++) {
            *upper[i].get() = 0;
            *lower[i].get() = 0;
            reinterpret_cast<LinkMotionFootData *>(mFootData)[i].field_0x030 = 0.0f;
        }
    } else {
        int k = (j + 1) & 1;
        LinkMotionFootData &firstFoot = reinterpret_cast<LinkMotionFootData *>(mFootData)[j];
        LinkMotionFootData &secondFoot = reinterpret_cast<LinkMotionFootData *>(mFootData)[k];
        firstFoot.field_0x030 = (f32)(0.0f);
        if (!setLegAngle(h[j] - (f32)gabi::at<Mtx34>(base)->m[1][3], j, (s16 *)upper[j].get(),
                         (s16 *)lower[j].get())) {
            *lower[j].get() = 0;
            *upper[j].get() = 0;
        }
        f32 d = h[k] - (f32)gabi::at<Mtx34>(base)->m[1][3];
        if (d > 0.0f || mode1) {
            secondFoot.field_0x030 = (f32)(d * 0.3f);
            if (!setLegAngle(d * 0.7f, k, (s16 *)upper[k].get(), (s16 *)lower[k].get())) {
                *lower[k].get() = 0;
                *upper[k].get() = 0;
            }
        } else {
            secondFoot.field_0x030 = (f32)(0.0f);
            *lower[k].get() = 0;
            *upper[k].get() = 0;
        }
    }
    for (int i = 0; i < 2; i++) {
        LinkMotionFootData &footData = reinterpret_cast<LinkMotionFootData *>(mFootData)[i];
        s32 u = *upper[i].get();
        s32 cur = (s16)footData.field_0x008;
        if (u * cur < 0 && lk_abs(u - cur) >= 0x8000) {
            u = (s16)(u >= 0 ? u - 0x4000 : u + 0x4000);
            *upper[i].get() = (s16)u;
        }
        cLib_addCalcAngleS(&footData.field_0x008, (s16)u, 2, 0x1800, 0x10);
        cLib_addCalcAngleS(&footData.field_0x006, *lower[i].get(), 2, 0x1800, 0x10);
        s16 f6 = (s16)footData.field_0x006;
        s16 f8 = (s16)footData.field_0x008;
        footData.field_0x00A = (s16)((s16)(f6 - f8));
        footData.field_0x002 = (s16)((s16) - (m34E0 + f6));
    }
    for (int i = 0; i < 2; i++) {
        LinkMotionFootData &footData = reinterpret_cast<LinkMotionFootData *>(mFootData)[i];
        s32 g;
        if (flag[i] == 0 && (u8)footData.field_0x000 != 0 && mode1) {
            g = LinkMotionApi::getGroundAngle(
                this, gabi::ea(static_cast<cBgS_PolyInfo *>(&footData.field_0x034)),
                (s32)shape_angle.y);
            footData.field_0x002 = (s16)((s16)((s16)footData.field_0x002 + g));
            g = LinkMotionApi::getGroundAngle(
                this, gabi::ea(static_cast<cBgS_PolyInfo *>(&footData.field_0x034)),
                (s32)(s16)(shape_angle.y - 0x4000));
        } else {
            g = 0;
        }
        cLib_addCalcAngleS(&footData.field_0x004, (s16)g, 2, 0x1800, 0x10);
    }
}

/* 02400990 */
void daPy_lk_c::setMoveSlantAngle() {

    f32 k = std::fabs(speedF / mMaxNormalSpeed);
    if (mModeFlg & 0x02000000) {
        m351C = 0;
        mBodyAngle.z = 0; /* mBodyAngle.z */
        return;
    }
    s16 v;
    if (mCurProc == LinkMotionConstants::daPyProc_MOVE_e && k > 0.95f) {
        f32 t = (k - 0.95f) / 0.050000012f;
        s16 d = (s16)(m34DE - shape_angle.y);
        cLib_addCalcAngleS(&m351C, (s16)gabi::ftoi(((f32)d * 1.6f) * t), 4, 0xC8, 0x64);
        v = (s16)(m351C >> 1);
        mBodyAngle.z = v;
        shape_angle.z = v;
        return;
    }
    s16 cur = m351C;
    s16 step = (s16)gabi::ftoi((f32)cur * 0.35f);
    if (step == 0) {
        m351C = 0;
        shape_angle.z = 0;
        mBodyAngle.z = 0;
    } else {
        s16 n = (s16)(cur - step);
        m351C = n;
        v = (s16)(n >> 1);
        mBodyAngle.z = v;
        shape_angle.z = v;
    }
}
/* ---- setStickData (023F32E4) ---- */
// The original copies zero-page words when a texture-list entry is absent.
static inline u32 linkTextureWord(u32 texture, u32 word) {
    return gabi::load<u32>(texture + word * sizeof(u32));
}

/* ---- setDemoData ---- */
/* inline sead::SafeString::operator==(a, b) where GHS calls the first assureTermination directly (02444F48) */
static inline bool linkSafeStringEqualsDirect(LinkActionSafeString* a, LinkActionSafeString* b) {
    gabi::call(0x02444F48 /* sead::SafeString::assureTerminationImpl_ */, a);
    linkAssureTermination(a);
    u32 pa = a->mStr;
    linkAssureTermination(b);
    u32 pb = b->mStr;
    if (pa == pb)
        return true;
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::load<u8>(a->mStr + n);
        u8 cb = gabi::load<u8>(pb + n);
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
/* inlined into daPy_lk_c::setDemoData */
static inline bool linkStringEqualsDirect(u32 lit, u32 str) {
    LinkActionSafeString a;
    a.__vtbl = LK_SAFESTRING_VTBL;
    a.mStr = lit;
    LinkActionSafeString b;
    b.__vtbl = LK_SAFESTRING_VTBL;
    b.mStr = str;
    return linkSafeStringEqualsDirect(&a, &b);
}
/* strcmp(dComIfGp_getRunEventName(), lit) == 0 */
/* inlined into daPy_lk_c::setDemoData */
static inline bool linkIsRunEvent(u32 lit) {
    LinkActionSafeString a;
    a.__vtbl = LK_SAFESTRING_VTBL;
    a.mStr = lit;
    u32 name = gabi::call<u32>(0x02542E94 /* dEvent_manager_c::getRunEventName (matcher: dummy1) */,
                               dComIfGp_ea() + 0x52C4);
    LinkActionSafeString b;
    b.__vtbl = LK_SAFESTRING_VTBL;
    b.mStr = name;
    return linkSafeStringEquals(&a, &b);
}
static inline u32 linkGetEventSubstance(s32 staff, u32 name, s32 type) {
    return gabi::call<u32>(0x0254487C /* dEvent_manager_c::getMySubstanceP */,
                           dComIfGp_ea() + 0x52C4, staff, name, type);
}
static inline void linkEndEventCut(s32 staff) {
    gabi::call(0x02543280 /* dEvent_manager_c::cutEnd */, dComIfGp_ea() + 0x52C4, staff);
}

/* 023F22FC */
void daPy_lk_c::setDemoData() {

    u8 id = demoActorID; /* demoActorID */
    u32 demo_actor_p = 0;
    if (id != 0 && id <= 0x20) {
        u32 demo = LinkActionDemoObject();
        if (demo == 0) {
            LinkActionApi::assertionFailure(gabi::guest_addr("d_demo.h"), 0x23A,
                                            gabi::guest_addr("m_object != (0)"));
            demo = LinkActionDemoObject();
        }
        demo_actor_p = LinkActionApi::getDemoActor(demo, (u32)id);
    }
    u32 pos_p = 0;
    s16 angle = 0;
    s32 demo_mode = 1; /* DEMO_N_WAIT */
    u32 prm0_p = 0;
    u32 prm1_p = 0;
    if (!dComIfGp_event_runCheck()) {
        if (mDemo.mDemoType != 0)
            endDemoMode();
        if (m3554 > 0) {
            m3554 = (s16)(m3554 - 1);
        } else {
            mNoResetFlg1 = mNoResetFlg1 & ~0x20000u; /* FOREST_WATER_USE */
        }
        return;
    }
    {
        auto& status =
            LinkActionPlay().mPlayerStatus[0][0]; /* clearPlayerStatus0(BOOMERANG_WAIT) */
        status = (u32)status & ~0x00400000u;
    }
    s32 proc;
    if (!(LinkActionPlay().mEvtCtrl.mEventFlag & 0x20)) { /* !dComIfGp_event_chkEventFlag(0x20) */
        proc = mCurProc;
        if (proc == 0 /* SCOPE */) {
            procWait_init();
            proc = mCurProc;
        } else if (proc == 0x8A /* SHIP_SCOPE */) {
            procShipPaddle_init();
            proc = mCurProc;
        }
    } else {
        proc = mCurProc;
    }
    if (proc == 1 /* SUBJECTIVITY */)
        procWait_init();
    if (mDemo.mDemoType == 4 /* TYPE_START */ &&
        !LinkActionApi::checkStartDemo(dComIfGp_ea() + 0x52C4)) {
        mDemo.mDemoType = 2; /* setSystemDemoType() */
        LinkAction(this).mTingleDemoOrdered = 0;
    } else {
        // HD: the byte set by the Tingle event order starts an original-type demo (mode 0x34)
        if (LinkAction(this).mTingleDemoOrdered != 0) {
            mDemo.mParam0 = 0;
            mDemo.mDemoMode = 0x34;
            mDemo.mDemoType = 3;
        }
        LinkAction(this).mTingleDemoOrdered = 0;
    }
    if (demo_actor_p != 0) {
        gabi::at<LinkActionDemoActor>(demo_actor_p)->mpModel = mpCLModel; /* setModel(mpCLModel) */
        u16 type = mDemo.mDemoType;
        mStaffIdx = -1;
        if (type != 1 /* TYPE_TOOL */) {
            mDemo.mDemoType = 1;
            freeGrabItem();
            freeRopeItem();
            freeHookshotItem();
            deleteEquipItem(0);
            procWait_init();
        }
        u16 en = gabi::at<LinkActionDemoActor>(demo_actor_p)->mEnable;
        if (en & 0x20 /* ENABLE_ANM */)
            demo_mode = gabi::at<LinkActionDemoActor>(demo_actor_p)->mAnimation;
        pos_p = (en & 2 /* ENABLE_TRANS */)
                    ? gabi::ea(&gabi::at<LinkActionDemoActor>(demo_actor_p)->mTranslation)
                    : gabi::ea(&current.pos);
        angle = (en & 8 /* ENABLE_ROTATE */)
                    ? gabi::at<LinkActionDemoActor>(demo_actor_p)->mRotation.y
                    : (s16)shape_angle.y;
        if ((en & 0x10 /* ENABLE_SHAPE */) && dComIfGs_getClearCount() == 0 /* getClearCount() */) {
            u32 casual = mNoResetFlg1 & 8; /* CASUAL_CLOTHES */
            s32 shape = gabi::at<LinkActionDemoActor>(demo_actor_p)->mShape;
            if (casual ? shape == 0 : shape == 1) {
                // HD: the Link textures are a list of 10 texture objects (0x24 bytes each)
                u32 list = mpCurrLinktex;
                LinkActionTextureArray tmp;
                LinkActionApi::constructArray(&tmp, 10, 0x24, 0x02444280u);
                for (u32 i = 0; i < 10; i++) {
                    u32 src = 0;
                    if (i < gabi::at<LinkActionTextureList>(list)->mCount)
                        src = gabi::ea(
                            gabi::at<LinkActionTextureList>(list)->mpEntries.get()[i].get());
                    for (u32 word = 0; word < 9; word++)
                        tmp.mTextures[i].mWords[word] = linkTextureWord(src, word);
                }
                LinkActionApi::copyTextureList((u32)mpCurrLinktex,
                                               gabi::ea(&LinkAction(this).mTextures));
                for (u32 i = 0; i < 10; i++)
                    for (u32 word = 0; word < 9; word++)
                        LinkAction(this).mTextures[i].mWords[word] =
                            (u32)tmp.mTextures[i].mWords[word];
                u32 f = mNoResetFlg1;
                mNoResetFlg1 = (f & 8) ? (f & ~8u) : (f | 8u);
            }
        }
    } else {
        if (mDemo.mDemoType == 0 && dComIfGp_event_runCheck())
            mDemo.mDemoType = 2;
        if (mStaffIdx != -1) {
            u32 cut_name = LinkActionApi::getMyNowCutName(dComIfGp_ea() + 0x52C4, (s32)mStaffIdx);
            // HD: two events (names at gabi::guest_addr("SO_TRIFORCE_CHECK") / gabi::guest_addr("PHOTO_GALLERY")) get special cut handling
            bool evA = linkIsRunEvent(gabi::guest_addr("SO_TRIFORCE_CHECK"));
            bool evB = linkIsRunEvent(gabi::guest_addr("PHOTO_GALLERY"));
            bool skip = cut_name != 0 && (evA || evB) &&
                        linkStringEqualsDirect(gabi::guest_addr("004init_wait"), cut_name);
            if (!skip && cut_name != 0) {
                demo_mode = (u8)gabi::at<be<u8>>(cut_name)[0] * 100 +
                            (u8)gabi::at<be<u8>>(cut_name)[1] * 10 +
                            (u8)gabi::at<be<u8>>(cut_name)[2] - 0x14D0;
                if (mDemo.mDemoType != 4 &&
                    (demo_mode == 1 || demo_mode == 0x2A || demo_mode == 0x17)) {
                    if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !LK_checkPlayerFly()) {
                        speedF = 0.0f;
                        mNormalSpeed = 0.0f;
                    }
                }
                pos_p = linkGetEventSubstance(mStaffIdx, gabi::guest_addr("pos"), 1);
                if (pos_p == 0)
                    pos_p = LinkActionApi::getEventGoal(dComIfGp_ea() + 0x52C4);
                u32 angle_p = linkGetEventSubstance(mStaffIdx, gabi::guest_addr("angle"), 3);
                if (angle_p != 0) {
                    angle = (s16)gabi::at<csXyz>(angle_p)->y;
                } else if (demo_mode == 5 /* DEMO_WAIT_TURN */) {
                    angle = mDemo.mDemoMoveAngle;
                } else {
                    angle = shape_angle.y;
                }
                // HD: a fixed position (static, set on first use) for one cut of event B
                if ((u32)l_fixedDemoPosInitialized == 0) {
                    l_fixedDemoPosInitialized = 1;
                    l_fixedDemoPos.y = 450.0f;
                    l_fixedDemoPos.x = -500.0f;
                    l_fixedDemoPos.z = 560.0f;
                }
                if (evB && current.pos.z > 200.0f &&
                    linkStringEqualsDirect(gabi::guest_addr("020l_around2"), cut_name)) {
                    LinkActionSetPlayerPosAndAngle(this, gabi::ea(&l_fixedDemoPos), 0x5A00);
                }
                prm0_p = linkGetEventSubstance(mStaffIdx, gabi::guest_addr("prm0"), 3);
                prm1_p = linkGetEventSubstance(mStaffIdx, gabi::guest_addr("prm1"), 3);
                if (mDemo.mDemoType == 2 /* TYPE_SYSTEM */) {
                    u32 stick_p = linkGetEventSubstance(mStaffIdx, gabi::guest_addr("stick"), 0);
                    // HD: event A's cut gabi::guest_addr("003n_dash") ignores the stick value
                    if (stick_p != 0 &&
                        !(evA && linkStringEqualsDirect(gabi::guest_addr("003n_dash"), cut_name))) {
                        linkCopyFloatBits(gabi::ea(this) + 0x434, stick_p);
                    } else {
                        mDemo.mStick = 1.0f;
                    }
                }
                u32 face_p = linkGetEventSubstance(mStaffIdx, gabi::guest_addr("face"), 3);
                if (face_p != 0) {
                    s32 face = (s32)*gabi::at<be<s32>>(face_p);
                    if (face == 0) {
                        resetDemoTextureAnime();
                    } else if (face == 1) {
                        LinkActionChangeTextureAnime(this, 0x237, 0x1A7, -1);
                    }
                }
            }
        }
    }
    if (demo_mode == 0x27 /* DEMO_SHIP */ && gabi::ea(dComIfGp_getShipActor()) == 0)
        demo_mode = 1;
    u16 type = mDemo.mDemoType;
    if (type == 4 /* TYPE_START */) {
        if (mDemo.mDemoMode == 0xE /* DEMO_KEEP */) {
            s16 timer = mDemo.mTimer;
            if (timer != 0) {
                mDemo.mTimer = (s16)(timer - 1);
                cXyz d;
                cXyz_mi(&current.pos, &d, &home.pos);
                cXyz xz;
                xz.x = d.x;
                xz.y = 0.0f;
                xz.z = d.z;
                f64 sq = LinkActionApi::squareMagnitude(&xz);
                if (sq > (f64)90000.0f) {
                    mDemo.mTimer = 0;
                    linkEndEventCut(mStaffIdx);
                }
            } else {
                linkEndEventCut(mStaffIdx);
            }
        }
        return;
    }
    if (type == 1 || (type == 2 && mStaffIdx != -1)) {
        mDemo.mParam0 = prm0_p != 0 ? (s32)*gabi::at<be<s32>>(prm0_p) : 0;
        mDemo.mParam1 = prm1_p != 0 ? (s32)*gabi::at<be<s32>>(prm1_p) : 0;
        if (demo_mode == 4 /* DEMO_INIT_WAIT */ || demo_mode == 0x2C /* DEMO_POS_INIT */) {
            mNormalSpeed = 0.0f;
            mStts.m_cc_move.z = 0.0f; /* mStts.ClrCcMove() */
            mStts.m_cc_move.y = 0.0f;
            mStts.m_cc_move.x = 0.0f;
            speedF = 0.0f;
            LinkActionSetPlayerPosAndAngle(this, pos_p, angle);
            mDemo.mDemoMoveAngle = angle; /* mDemo.setMoveAngle */
            if (demo_mode == 4 && mDemo.mParam0 != 0) {
                deleteEquipItem(0);
                s32 p0 = mDemo.mParam0;
                if (p0 == 1 && (dComIfGs_getSelectEquip(0) != 0xFF /* checkSwordEquip() */ ||
                                LinkActionPlay().mMiniGameType == 2)) {
                    setSwordModel(1);
                    mDemo.mParam0 = 0;
                } else {
                    if (p0 == 1)
                        p0 = mDemo.mParam0;
                    if (p0 == 2) {
                        mDemo.mParam0 = 0;
                        mEquipItem = 0x100; /* daPyItem_NONE */
                    } else {
                        if (p0 == 3) {
                            mEquipItem = 0x100;
                            if (dComIfGp_checkPlayerStatus0(0, 0x10000 /* SHIP_RIDE */)) {
                                auto& status = LinkActionPlay().mPlayerStatus[0][0];
                                status = (u32)status & ~0x10000u;
                            }
                        }
                        mDemo.mParam0 = 0;
                    }
                }
            }
            if (dComIfGp_checkPlayerStatus0(0, 0x10000)) {
                u32 ship = gabi::ea(dComIfGp_getShipActor());
                if (ship != 0)
                    LinkActionApi::shipInitStartPos(ship, &current.pos, (s32)(s16)shape_angle.y);
            }
            mDemo.mDemoMode = demo_mode;
        } else if (demo_mode == 0x2B /* DEMO_CUT_ROLL */) {
            cXyz d;
            cXyz_mi(gabi::at<cXyz>(pos_p), &d, &current.pos);
            s16 old = current.angle.y;
            s16 a = (s16)LinkActionApi::atan2sWide((f32)d.x, (f32)d.z);
            mDemo.mDemoMode = demo_mode;
            current.angle.y = a;
            shape_angle.y = a;
            m34EC = (s16)(m34EC - (s16)(a - old));
        } else if (demo_mode == 2 /* DEMO_N_WALK */ || demo_mode == 3 /* DEMO_N_DASH */) {
            cXyz d;
            cXyz_mi(gabi::at<cXyz>(pos_p), &d, &current.pos);
            f32 ratio = fabsf((f32)mNormalSpeed) / (f32)mMaxNormalSpeed;
            f32 x = d.x;
            f32 z = d.z;
            if (ratio < 0.5f)
                demo_mode = 2;
            cXyz xz;
            xz.z = z;
            xz.x = x;
            xz.y = 0.0f;
            f64 sq = LinkActionApi::squareMagnitude(&xz);
            bool stop = false;
            if (sq < (f64)100.0f) {
                stop = true;
            } else if (sq < (f64)2500.0f && fabsf((f32)mNormalSpeed) < 0.001f) {
                stop = true;
            }
            if (stop) {
                demo_mode = 1;
                mNormalSpeed = 0.0f;
            } else if ((demo_mode == 2 && sq < (f64)400.0f) || sq < (f64)2500.0f) {
                mDemo.mStick = 0.0f; /* mDemo.setStick(0.0f) */
            }
            mDemo.mDemoMoveAngle = (s16)LinkActionApi::atan2sWide(x, z);
            mDemo.mDemoMode = demo_mode;
        } else if (demo_mode == 5 /* DEMO_WAIT_TURN */ || demo_mode == 0x18 /* DEMO_SURPRISED */) {
            mDemo.mDemoMoveAngle = angle;
            mDemo.mDemoMode = demo_mode;
        } else {
            mDemo.mDemoMode = demo_mode;
        }
    } else if (type != 3 /* TYPE_ORIGINAL */) {
        mDemo.mDemoMode = demo_mode;
    }
    if ((mModeFlg & 0x80000) || mDemo.mDemoMode == 6 /* DEMO_N_TALK */ ||
        mDemo.mDemoMode == 8 /* DEMO_E_TALK */) {
        dComIfGp_setPlayerStatus0(0, 0x10);
    } else {
        auto& status = LinkActionPlay().mPlayerStatus[0][0];
        status = (u32)status & ~0x10u;
    }
    s32 staff = mStaffIdx;
    if (staff != -1) {
        u32 m = (u32)demo_mode;
        if (m == 1 || m == 4 || m == 0xE || m == 0x11 || m == 0x12 || m == 0x17 || m == 0x27 ||
            m == 0x2A || m == 0x2B || m == 0x2C || m == 0x33) {
            linkEndEventCut(staff);
        }
    }
}

/* HD: the sead controller (GamePad) object at *0x101F5088: +0x18 trigger bits, +0x124 hold bits */
static inline u32 lk_hdPadTrig() { return gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x18); }
static inline u32 lk_hdPadHold() { return gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x124); }
/* inline dSv_player_item_c::getItem(slot) over the save's item/bag tables (save + 0x20 base) */
static inline u8 lk_saveGetItem(u32 base, u32 idx) {
    if ((s32)idx < 0x15)
        return gabi::load<u8>(base + idx + 0x3C);
    if ((s32)idx < 0x18)
        return 0xFF;
    if ((s32)idx < 0x20)
        return gabi::load<u8>(base + idx + 0x5E);
    if ((s32)idx < 0x24)
        return 0xFF;
    if ((s32)idx < 0x2C)
        return gabi::load<u8>(base + idx + 0x5A);
    if (idx - 0x30 < 8)
        return gabi::load<u8>(base + idx + 0x56);
    return 0xFF;
}
/* inline dComIfGp_setSelectItem(btn) for the HD extra buttons: the save's select slot (save + selOff) ->
 * the play item byte (play + 0x12A0 + playOff); a slot holding no item is cleared */
static inline void lk_setSelectItemHD(u32 selOff, u32 playOff) {
    u8 sel = gabi::load<u8>(dComIfGs_ea() + selOff);
    u32 p = dComIfGp_ea() + 0x12A0;
    if (sel == 0xFF) {
        gabi::store<u8>(p + playOff, 0xFF);
        return;
    }
    u32 s = dComIfGs_ea();
    gabi::store<u8>(p + playOff, lk_saveGetItem(s + 0x20, gabi::load<u8>(s + selOff)));
    s = dComIfGs_ea();
    if (lk_saveGetItem(s + 0x20, gabi::load<u8>(s + selOff)) == 0xFF)
        gabi::store<u8>(s + selOff, 0xFF);
}
static inline s16 lk_camControledAngleY(s32 idx) {
    return gabi::call<s16>(0x024F8018 /* dCam_getControledAngleY */,
                           gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8));
}

/* 023F32E4 */
void daPy_lk_c::setStickData() {

    mItemTrigger = 0;
    u8 old = mItemButton;
    mItemButton = 0;
    m34CA = old;
    if (gabi::ea(LinkActionPlay().mPlayerInfo[0].mpPlayer.get()) != gabi::ea(this)) {
        mStickDistance = 0.0f;
        m34E8 = 0;
    } else if (mDemo.mDemoMode == 0x44 /* checkBowMiniGame() */) {
        mStickDistance = CPad_GET_STICK_VALUE(0);
        m34DC = (s16)(CPad_GET_STICK_ANGLE(0) + 0x8000);
        s16 cam = lk_camControledAngleY(mCameraInfoIdx);
        m34E8 = (s16)(cam + m34DC);
        // HD: the bow mini-game shoots with the Z-hold read (GameCube: hold A -> BTN_A)
        if (CPad_CHECK_HOLD_Z(0))
            mItemButton = mItemButton | 0x10;
    } else if ((dComIfGp_event_runCheck() || mDemo.mDemoType != 0) &&
               !dComIfGp_checkPlayerStatus1(0, 1) && mCurProc != 0xA2 /* NOT_USE */ &&
               mCurProc != 0xA6 /* BOTTLE_GET */) {
        s32 mode = mDemo.mDemoMode;
        if (mode == 2 /* DEMO_N_WALK */ || mode == 3 /* DEMO_N_DASH */) {
            u8 id = demoActorID; /* demoActorID */
            bool have = false;
            if (id != 0 && id <= 0x20) {
                u32 demo = LinkActionDemoObject();
                if (demo == 0) {
                    LinkActionApi::assertionFailure(gabi::guest_addr("d_demo.h"), 0x23A,
                                                    gabi::guest_addr("m_object != (0)"));
                    demo = LinkActionDemoObject();
                }
                have = LinkActionApi::getDemoActor(demo, (u32)id) != 0;
            }
            if (have) {
                m34E8 = (s16)mDemo.mDemoMoveAngle;
                mStickDistance = 1.0f;
            } else {
                linkCopyFloatBits(gabi::ea(this) + 0x6A08,
                                  gabi::ea(this) + 0x434); /* mDemo.getStick() */
                m34E8 = (s16)mDemo.mDemoMoveAngle;
            }
        } else {
            mStickDistance = 0.0f;
            m34E8 = (s16)mDemo.mDemoMoveAngle; /* mDemo.getMoveAngle() */
        }
        if (mDemo.mDemoMode == 0xE /* DEMO_KEEP */)
            mItemButton = (u8)m34CA;
    } else if (LinkActionApi::isOverlapPeek() == 1) {
        mStickDistance = 0.0f;
        m34E8 = 0;
    } else {
        mStickDistance = CPad_GET_STICK_VALUE(0);
        if ((mNoResetFlg1 & 0x100 /* CONFUSE */) && !dComIfGp_event_runCheck()) {
            m34DC = CPad_GET_STICK_ANGLE(0);
        } else {
            m34DC = (s16)(CPad_GET_STICK_ANGLE(0) + 0x8000);
        }
        s16 cam = lk_camControledAngleY(mCameraInfoIdx);
        m34E8 = (s16)(cam + m34DC);
        u8 bVar2 = LinkActionPlay().mButtonMode; /* dComIfGp_getButtonActionMode() */
        bool bVar1 = false;
        if (CPad_CHECK_TRIG_B(0)) {
            // HD: B also works while the ship's item use is active
            if ((bVar2 & 2) || checkShipRideUseItem(0) == 1) {
                mItemTrigger = mItemTrigger | 2;
            } else {
                bVar1 = true;
            }
        }
        if (CPad_CHECK_TRIG_A(0)) {
            if (bVar2 & 1) {
                // HD: on the ship with the (swift) sail item, A also assigns save select slot 0x2D
                if (checkShipRideUseItem(0) == 1 && !dComIfGp_checkPlayerStatus1(0, 2) &&
                    !dComIfGp_checkPlayerStatus1(0, 4) &&
                    (LinkActionApi::checkItemGet(0x78, 1) ||
                     LinkActionApi::checkItemGet(0x77, 1))) {
                    LinkActionSave().mSavedata.mPlayer.mPlayerStatusA.mSelectItem[4] = 1;
                    lk_setSelectItemHD(0x2D, 0x491F);
                }
                mItemTrigger = mItemTrigger | 1;
            } else {
                bVar1 = true;
            }
        }
        if (CPad_CHECK_TRIG_X(0)) {
            if ((bVar2 & 4) && mTinkleHoverTimer == 0) {
                mItemTrigger = mItemTrigger | 4;
            } else {
                bVar1 = true;
            }
        }
        if (CPad_CHECK_TRIG_Y(0)) {
            if ((bVar2 & 8) && mTinkleHoverTimer == 0) {
                mItemTrigger = mItemTrigger | 8;
            } else {
                bVar1 = true;
            }
        }
        if (CPad_CHECK_TRIG_Z(0)) {
            if ((bVar2 & 0x20) && mTinkleHoverTimer == 0) {
                mItemTrigger = mItemTrigger | 0x10;
            } else {
                bVar1 = true;
            }
        }
        /* HD: the GamePad's extra item button (0x80): item 0x22 normally, items 0x31 / 0x25 on the ship;
         * it writes the save's select slot 0x2C and the play item byte */
        {
            u32 sel = 0;
            u32 need = 0;
            if (lk_hdPadTrig() & 0x10000) {
                sel = 2;
                need = 0x22;
            } else {
                int use = checkShipRideUseItem(0);
                if (use == 1 || checkShipRideUseItem(0) == 2) {
                    if (lk_hdPadTrig() & 0x40000) {
                        sel = 0xD;
                        need = 0x31;
                    } else if (lk_hdPadTrig() & 0x80000) {
                        sel = 3;
                        need = 0x25;
                    }
                }
            }
            if (need != 0 && LinkActionApi::checkItemGet(need, 1)) {
                if ((bVar2 & 0x40) && mTinkleHoverTimer == 0) {
                    LinkActionSave().mSavedata.mPlayer.mPlayerStatusA.mSelectItem[3] = (u8)sel;
                    lk_setSelectItemHD(0x2C, 0x491E);
                    mItemTrigger = mItemTrigger | 0x80;
                } else {
                    bVar1 = true;
                }
            }
        }
        if (CPad_CHECK_TRIG_L(0))
            mItemTrigger = mItemTrigger | 0x20;
        if (CPad_CHECK_R_LOCK_TRIG(0))
            mItemTrigger = mItemTrigger | 0x40;
        if (CPad_CHECK_HOLD_A(0) && (bVar2 & 1))
            mItemButton = mItemButton | 1;
        if (CPad_CHECK_HOLD_B(0) && ((bVar2 & 2) || checkShipRideUseItem(0) == 1)) {
            mItemButton = mItemButton | 2;
        }
        if (mTinkleHoverTimer == 0) {
            if (CPad_CHECK_HOLD_X(0) && (bVar2 & 4))
                mItemButton = mItemButton | 4;
            if (CPad_CHECK_HOLD_Y(0) && (bVar2 & 8))
                mItemButton = mItemButton | 8;
            if (CPad_CHECK_HOLD_Z(0) && (bVar2 & 0x20))
                mItemButton = mItemButton | 0x10;
            bool hold = false;
            if ((lk_hdPadHold() & 0x10000) && (bVar2 & 0x40)) {
                hold = true;
            } else {
                int use = checkShipRideUseItem(0);
                if (use == 1 || checkShipRideUseItem(0) == 2) {
                    hold = (lk_hdPadHold() & 0x40000) || (lk_hdPadHold() & 0x80000);
                }
            }
            if (hold)
                mItemButton = mItemButton | 0x80;
        }
        if (CPad_CHECK_HOLD_L(0))
            mItemButton = mItemButton | 0x20;
        if (CPad_CHECK_R_LOCK_BUTTON(0))
            mItemButton = mItemButton | 0x40;
        if (checkHeavyStateOn()) {
            mStickDistance = gabi::fmuls_ppc((f32)mStickDistance, 0.5f);
        }
        if (field_0x2b0 < 0.0f /* checkGrabWear() */) {
            mStickDistance = gabi::fmuls_ppc((f32)mStickDistance, 0.8f);
        }
        if (bVar1 && !dComIfGp_event_runCheck() && !dComIfGp_checkPlayerStatus1(0, 8)) {
            LinkActionApi::seStartSystem(LinkActionConstants::JA_SE_ITEM_TARGET_OUT);
        }
    }
    s16 angle_diff = (s16)(m34DC - m34EA);
    s32 cnt = m3578;
    s32 abs_v = lk_abs(angle_diff);
    if ((s32)((u32)cnt * (u32)(s32)angle_diff) < 0) {
        m3578 = angle_diff;
        m3524 = 4;
    } else if ((u32)(abs_v - 0x6D5) < 0x641E) {
        m3578 = cnt + angle_diff;
        m3524 = 4;
    } else if (m3524 > 0) {
        m3524 = (s16)(m3524 - 1);
    } else {
        m3578 = 0;
    }
}

/* WWHD glue
data eff_joint 10035840
ext 023D457C void LinkActionApi::makeEmitter(void*, u32, u32, cXyz*, void*)
str 10034F74 in 023F22FC
str 10034F74 in 023F32E4
str 10035038 in 023F22FC
str 10035038 in 023F32E4
str 10035708 in 023EE328
str 10035710 in 023EE328
str 10035734 in 023EEB70
str 1003573C in 023EED74
str 10035744 in 023EF280
str 1003574C in 023EF3E0
str 10035754 in 023EF61C
str 1003577C in 023F22FC
str 10035780 in 023F22FC
str 10035788 in 023F22FC
str 10035790 in 023F22FC
str 10035798 in 023F22FC
str 100357A0 in 023F22FC
str 100357A8 in 023F22FC
str 100357BC in 023F22FC
str 100357CC in 023F22FC
str 100357D8 in 023F22FC
str 100357E8 in 023F22FC
str 100357F8 in 023F42EC
str 10035800 in 023F4688
str 10035830 in 023F54E8
str 10035838 in 023F54E8
str 10035850 in 023F6020
str 10035860 in 023F695C
str 10035874 in 023F695C
data l_fixedDemoPosInitialized 1046D0C8
data l_fixedDemoPos 1046CE14
ext 027F3F94 u32 LinkActionApi::resolveJointTree(u32)
ext 027E0174 s32 LinkActionApi::countJoints(u32, s32)
ext 0273AA24 void LinkActionApi::assertionFailure(u32, u32, u32)
ext 025F0658 void LinkActionApi::fadeOut(u32, f32)
ext 025D9D0C void LinkActionApi::setCarryNow(u32, s32)
ext 0253E70C void LinkActionApi::startEventCameraMode(u32, u32, u32, u32, u32, u32)
ext 025D8870 void LinkActionApi::createItem(cXyz*, s32, s32, s32, s32, u32, u32, u32)
ext 02035ED4 void LinkActionApi::setLetterWritingMode(u32, s32)
ext 02035D78 u32 LinkActionApi::getLetterWriter()
ext 025A847C void LinkActionApi::setParticle(u32, u32, u32, cXyz*, u32, u32, u32, u32, s32, u32, u32, u32)
ext 023DBBE8 BOOL daPy_lk_c::checkGrabSpecialHeavyState()
ext 020079B4 f32 CPad_GET_STICK_VALUE(u32)
ext 02007A1C s16 CPad_GET_STICK_ANGLE(u32)
ext 020078BC BOOL CPad_CHECK_TRIG_B(u32)
ext 02007898 BOOL CPad_CHECK_TRIG_A(u32)
ext 020078E8 BOOL CPad_CHECK_TRIG_X(u32)
ext 02007914 BOOL CPad_CHECK_TRIG_Y(u32)
ext 02007814 BOOL CPad_CHECK_TRIG_Z(u32)
ext 0200786C BOOL CPad_CHECK_TRIG_L(u32)
ext 02007D4C BOOL CPad_CHECK_R_LOCK_TRIG(u32)
ext 020076BC BOOL CPad_CHECK_HOLD_A(u32)
ext 020076E0 BOOL CPad_CHECK_HOLD_B(u32)
ext 0200770C BOOL CPad_CHECK_HOLD_X(u32)
ext 02007738 BOOL CPad_CHECK_HOLD_Y(u32)
ext 02007638 BOOL CPad_CHECK_HOLD_Z(u32)
ext 02007690 BOOL CPad_CHECK_HOLD_L(u32)
ext 02007CDC BOOL CPad_CHECK_R_LOCK_BUTTON(u32)
data mDemoProcInitFuncTable 10037858
ext 02008974 f64 LinkActionApi::groundCross(void*, void*)
ext 0200FAAC s32 LinkActionApi::distanceAngleS(s32, s32)
ext 020195B0 s32 LinkActionApi::atan2sWide(f32, f32)
ext 023D4688 void LinkActionApi::changePlayer(void*, void*)
ext 023DE638 void LinkActionApi::actorKeepSetData(void*, u32)
ext 023F12A4 BOOL LinkActionApi::procVomitReadyWide(void*, s32, f64)
ext 024832E0 void LinkActionApi::shipInitStartPos(u32, cXyz*, s32)
ext 024EBAE8 u32 LinkActionApi::convCatchPId(u32, u32)
ext 024F8044 u32 LinkActionApi::getCameraBody()
ext 025052BC void LinkActionApi::forceCameraLockOff(u32, u32)
ext 02514F2C void LinkActionApi::stopCamera(u32)
ext 02514F38 void LinkActionApi::startCamera(u32)
ext 02514F88 void LinkActionApi::setCamera(u32, cXyz*, cXyz*, f32, s32)
ext 02515048 void LinkActionApi::resetCamera(u32)
ext 02515470 void LinkActionApi::forceCameraLockOn(u32, u32)
ext 02515BBC u32 LinkActionApi::getCollisionActor(u32)
ext 02520FE4 BOOL LinkActionApi::checkGetItemNum(u32)
ext 02526E70 u32 LinkActionApi::getDemoActor(u32, u32)
ext 0253E860 void LinkActionApi::endEventCamera(u32, u32)
ext 0253EC0C BOOL LinkActionApi::orderEvent(u32, u32, u32, u32, u32, void*, u32, s32, u32)
ext 025445B8 BOOL LinkActionApi::startCheckOld(u32, u32)
ext 02544830 u32 LinkActionApi::getMyNowCutName(u32, s32)
ext 02544900 u32 LinkActionApi::getEventGoal(u32)
ext 025449B0 BOOL LinkActionApi::checkStartDemo(u32)
ext 0254DA50 BOOL LinkActionApi::checkItemGet(u32, s32)
ext 02550FAC BOOL LinkActionApi::isDaizaItem(u32)
ext 02550FF4 BOOL LinkActionApi::isEmono(u32)
ext 0255101C BOOL LinkActionApi::isEsa(u32)
ext 025B58B8 void LinkActionApi::setEquipBottleItemEmpty(u32, int)
ext 025D6894 s32 LinkActionApi::searchActorAngleY(void*, u32)
ext 025D6958 f64 LinkActionApi::searchActorDistanceXZ(void*, u32)
ext 025D7640 void LinkActionApi::orderZHintEvent(void*, u32)
ext 025D7710 void LinkActionApi::orderDoorEvent(void*, u32)
ext 025D7C08 void LinkActionApi::orderTreasureEvent(void*, u32)
ext 025D7C6C u32 LinkActionApi::getTalkEventPartner(void*)
ext 025D7F20 u32 LinkActionApi::fastCreateItem2(cXyz*, s32, s32, s32, u32, u32, u32, u32)
ext 025DBE00 s32 LinkActionApi::isOverlapPeek()
ext 025E1988 void LinkActionApi::seStartSystem(u32)
ext 025E1E88 void LinkActionApi::taktModeMuteOff()
ext 025E3570 void LinkActionApi::setCurrentHeap(u32)
ext 025E3F3C void LinkActionApi::initOldFrameMorf(u32, f32, s32, s32)
ext 0271FC5C BOOL LinkActionApi::isHeroMode(u32)
ext 027200D0 u32 LinkActionApi::getHeroModeManager(u32)
ext 0272B8B8 void LinkActionApi::copyTextureList(u32, u32)
ext 028E8DD0 f64 LinkActionApi::squareMagnitude(cXyz*)
ext 028EFFD0 void LinkActionApi::constructArray(void*, u32, u32, u32)
ext 028F4384 f64 LinkActionApi::sqrtWide(f64)
ext 023DCA08 void daPy_lk_c::endDamageEmitter()
ext 023DCA80 void daPy_lk_c::freeRopeItem()
ext 023DCF8C void daPy_lk_c::freeGrabItem()
ext 023DE0B4 void daPy_lk_c::setSwordModel(BOOL)
ext 023DEF04 void daPy_lk_c::setTactModel()
ext 023DFA7C void daPy_lk_c::freeHookshotItem()
ext 023E9E48 BOOL daPy_lk_c::procGrabThrow_init(int)
ext 023E73FC int daPy_lk_c::changeCutProc()
ext 023E9D50 BOOL daPy_lk_c::changeDragonShield(int)
ext 023E4CD0 int daPy_lk_c::changeSlideProc()
ext 023E5400 BOOL daPy_lk_c::changeSpecialBattle()
ext 023E32AC BOOL daPy_lk_c::changeWaitProc()
ext 023D69F8 BOOL daPy_lk_c::checkBoomerangAnime()
ext 023D6A18 BOOL daPy_lk_c::checkBowAnime()
ext 023E9A6C BOOL daPy_lk_c::checkDrinkBottleItem(int)
ext 023DBC24 BOOL daPy_lk_c::checkHeavyStateOn()
ext 023EB510 BOOL daPy_lk_c::checkNextActionBoomerangReady()
ext 023EB208 BOOL daPy_lk_c::checkNextActionBowReady()
ext 023EA1AC BOOL daPy_lk_c::checkNextActionGrab()
ext 023EAD5C BOOL daPy_lk_c::checkNextActionHookshotReady()
ext 023EC084 BOOL daPy_lk_c::checkNextActionRopeReady()
ext 023E8F2C BOOL daPy_lk_c::checkNextBoomerangMode()
ext 023E8984 BOOL daPy_lk_c::checkNextBowMode()
ext 023E8BD4 BOOL daPy_lk_c::checkNextHookshotMode()
ext 023E91E4 BOOL daPy_lk_c::checkNextRopeMode()
ext 023E9A84 BOOL daPy_lk_c::checkOpenBottleItem(int)
ext 023D6A5C BOOL daPy_lk_c::checkRopeAnime()
ext 023E9AF4 BOOL daPy_lk_c::checkSetItemTrigger(int, int)
ext 023D69CC BOOL daPy_lk_c::checkShipNotNormalMode()
ext 023E26EC int daPy_lk_c::checkShipRideUseItem(int)
ext 023D6ABC BOOL daPy_lk_c::checkUpperReadyThrowAnime()
ext 023DFDD8 BOOL daPy_lk_c::commonProcInit(int)
ext 023DC7AC void daPy_lk_c::deleteEquipItem(BOOL)
ext 023DDEEC u32 daPy_lk_c::getItemAnimeResource(u32)
ext 023EAA80 int daPy_lk_c::getReadyItem()
ext 023D4BB8 u32 daPy_lk_c::initModel(u32, int, u32)
ext 023E2E18 void daPy_lk_c::initShipRideUseItem(int, int)
ext 023EAB40 BOOL daPy_lk_c::itemTrigger()
ext 023EC724 int daPy_lk_c::orderTalk()
ext 023E9DD0 BOOL daPy_lk_c::procBootsEquip_init(u16)
ext 023EDD9C BOOL daPy_lk_c::procBottleSwing_init(int)
ext 023E2EFC BOOL daPy_lk_c::procControllWait_init()
ext 023ED970 BOOL daPy_lk_c::procCutTurnCharge_init()
ext 023E6380 BOOL daPy_lk_c::procCutTurn_init(BOOL)
ext 023E79DC BOOL daPy_lk_c::procFanSwing_init()
ext 023ED0F0 BOOL daPy_lk_c::procGrabReady_init()
ext 023ED5A4 BOOL daPy_lk_c::procJumpCut_init(int)
ext 023ECF74 BOOL daPy_lk_c::procPushPullWait_init(int)
ext 023E2358 BOOL daPy_lk_c::procScope_init(int)
ext 023ECA94 BOOL daPy_lk_c::procShipReady_init()
ext 023E92A0 BOOL daPy_lk_c::procTactWait_init(int)
ext 023E47D4 BOOL daPy_lk_c::procVomitJump_init(int)
ext 023ED788 BOOL daPy_lk_c::procWHideReady_init(u32, cXyz*)
ext 023E2FF4 BOOL daPy_lk_c::procWait_init()
ext 023EDC14 BOOL daPy_lk_c::procWeaponFrontSwingReady_init()
ext 023ED9E0 BOOL daPy_lk_c::procWeaponNormalSwing_init()
ext 023EDB6C BOOL daPy_lk_c::procWeaponSideSwing_init()
ext 023EDD14 BOOL daPy_lk_c::procWeaponThrow_init()
ext 023DC6A4 BOOL daPy_lk_c::resetActAnimeUpper(int, f32)
ext 023DDD48 void daPy_lk_c::resetDemoTextureAnime()
ext 023DE7E8 BOOL daPy_lk_c::setActAnimeUpper(u32, int, f32, f32, int, f32)
ext 023E9F60 void daPy_lk_c::setAnimeEquipSword(BOOL)
ext 023ECEF0 void daPy_lk_c::setAnimeUnequip()
ext 023ECD98 void daPy_lk_c::setAnimeUnequipItem(u16)
ext 023E5600 void daPy_lk_c::setAtParam(u32, int, int, u8, u8, u8, f32)
ext 023E81E4 void daPy_lk_c::setBlendAtnMoveAnime(f32)
ext 023E14A0 void daPy_lk_c::setBlendMoveAnime(f32)
ext 023E55A4 void daPy_lk_c::setBlurPosResource(u16)
ext 023DF0FC void daPy_lk_c::setBottleModel(u32)
ext 023EC498 void daPy_lk_c::setDoStatus()
ext 023EA680 void daPy_lk_c::setDoStatusBasic()
ext 023E3850 void daPy_lk_c::setFrontWallType()
ext 023E278C void daPy_lk_c::setOldRootQuaternion(s16, s16, s16)
ext 023E287C void daPy_lk_c::setShipRidePos(int)
ext 023E0A04 BOOL daPy_lk_c::setSingleMoveAnime(int, f32, f32, int, f32)
ext 023DD768 void daPy_lk_c::setTextureAnime(u32, int)
ext 0253E70C void LinkMotionApi::startEventCamera(u32, u32, u32, u32)
ext 025A847C u32 LinkMotionApi::setParticle(u32, u32, u32, cXyz*, u32, u32, u32, u32, s32, u32, u32, u32)
ext 023DFAAC void daPy_lk_c::swimOutAfter(BOOL)
ext 023E4E2C void daPy_lk_c::endFlameDamageEmitter()
ext 023DCB54 void daPy_lk_c::setDamageCurseEmitter()
ext 023DC58C void daPy_lk_c::resetPriTextureAnime()
ext 023DF600 void daPy_lk_c::makeItemType()
ext 023E7B24 void daPy_lk_c::setPriTextureAnime(u16, int)
ext 020195B0 s32 LinkMotionApi::atan2sYWide(f32, f64)
ext 020195B0 s32 LinkMotionApi::atan2sXWide(f64, f32)
ext 025D5928 fopAc_ac_c* LinkMotionApi::fastCreate(u32, u32, cXyz*, s32, csXyz*, void*, s32, u32, u32)
ext 025DB374 u32 LinkMotionApi::createAppend(u32, u32, u32, u32, u32, u32)
ext 025E1904 void LinkMotionApi::bgmStop(u32)
ext 025E18EC void LinkMotionApi::bgmStart(u32)
ext 025B5C80 BOOL LinkMotionApi::checkBottle(u32, u32)
ext 025B51DC void LinkMotionApi::setBottleItemIn(u32, u32, u32)
ext 020084C8 u32 LinkMotionApi::getTriPla(dBgS*, u32, u32)
ext 024EF080 s32 LinkMotionApi::getWallCode(dBgS*, u32)
ext 0254487C u32 LinkMotionApi::getMySubstanceP(u32, s32, u32, s32)
ext 02019510 s32 LinkMotionApi::rad2s(f32)
ext 025DAE64 void LinkMotionApi::createWpillar(cXyz*, f32, f32, u32)
ext 02008254 BOOL LinkMotionApi::chkPolySafe(dBgS*, u32)
ext 024EF0F4 s32 LinkMotionApi::getAttributeCode(dBgS*, u32)
ext 02516360 u32 LinkMotionApi::getTgHitGObj(dCcD_GObjInf*)
ext 025162A4 BOOL LinkMotionApi::chkTgHit(dCcD_GObjInf*)
ext 025160DC BOOL LinkMotionApi::chkAtHit(dCcD_GObjInf*)
ext 025161D8 u32 LinkMotionApi::getAtHitGObj(dCcD_GObjInf*)
ext 02019788 f64 LinkMotionApi::randomWide()
ext 0251638C u32 LinkMotionApi::getTgHitObjSe(dCcD_GObjInf*)
ext 02515EE0 void LinkMotionApi::clearTg(dCcD_Stts*)
ext 02515EAC void LinkMotionApi::clearAt(dCcD_Stts*)
ext 020CF87C void LinkMotionApi::setAimActor(u32, u32)
ext 023DC63C void LinkMotionApi::clearActorKeep(void*)
ext 027F2BF8 BOOL LinkMotionApi::checkPass(J3DFrameCtrl*, f32)
ext 025E3A6C void LinkMotionApi::updateJointFlags(u32, s32)
ext 027E01E0 void LinkMotionApi::updateMaterialFlags(u32, u32, u32, u32)
ext 023D7A0C void LinkMotionApi::jointCB0(daPy_lk_c*, s32)
ext 025AB184 BOOL LinkMotionApi::getPolyRoomPathVec(u32, cXyz*, be<s32>*)
ext 0201B3C0 void LinkMotionApi::normalizeZP(cXyz*, cXyz*)
ext 0200EF78 void LinkMotionApi::addCalcPosXZ(cXyz*, cXyz*, f32, f32, f32)
ext 028E9108 void LinkMotionApi::concatMatrix(Mtx34*, Mtx34*, Mtx34*)
ext 028E8F64 void LinkMotionApi::multVec(Mtx34*, cXyz*, cXyz*)
ext 023E7D6C s32 LinkMotionApi::getGroundAngle(daPy_lk_c*, u32, s32)
ext 023F8858 f64 LinkMotionApi::swimTimerRateWide(daPy_lk_c*)
ext 0200ECD4 void LinkMotionApi::addCalc(be<f32>*, f32, f32, f32, f32)
ext 028E8D88 void LinkMotionApi::addVector(cXyz*, cXyz*, cXyz*)
ext 0252012C void LinkMotionApi::setNextStage(u32, s32, s32, s32, f32, s32, u32, u32)
ext 025B8B94 BOOL LinkMotionApi::isEventBit(u32, u32)
ext 025C123C BOOL LinkMotionApi::chkPlayerId(s32, s32)
ext 0252174C BOOL LinkMotionApi::checkSeaLandingEvent(s32)
ext 025C3748 void LinkMotionApi::changeScene(s32, f32, s32, s32)
ext 025C3D68 void LinkMotionApi::restartRoom(u32, s32)
ext 025E410C void LinkMotionApi::calcUnderBlend(u32, J3DModel*)
ext 028E8DAC void LinkMotionApi::subtractVector(cXyz*, cXyz*, cXyz*)
ext 025D5218 u32 LinkMotionApi::judgeActor(u32, be<u32>*)
ext 0246B6A4 BOOL LinkMotionApi::seaChkArea(f32, f32)
ext 0246BA0C f64 LinkMotionApi::seaCalcWave(f32, f32)
ext 024EF130 s32 LinkMotionApi::getRoomId(dBgS*, u32)
ext 024EEEB8 u32 LinkMotionApi::getPolyColor(dBgS*, u32)
ext 024EEABC BOOL LinkMotionApi::chkMoveBG(dBgS*, u32)
ext 024EF064 s32 LinkMotionApi::getLinkNo(dBgS*, u32)
ext 02008438 u32 LinkMotionApi::getActorPointer(dBgS*, u32)
ext 024EECC8 s32 LinkMotionApi::getExitId(dBgS*, u32)
ext 025C38B8 BOOL LinkMotionApi::changeSceneExitId(u32, f32, s32, s32)
ext 025B8B68 void LinkMotionApi::onEventBit(u32, u32)
ext 028E93CC void LinkMotionApi::translateMatrix(Mtx34*, f32, f32, f32)
ext 025F1C28 void LinkMotionApi::rotateMatrixY(Mtx34*, s32)
ext 028E90D4 void LinkMotionApi::copyMatrix(Mtx34*, Mtx34*)
ext 028E91EC void LinkMotionApi::inverseMatrix(Mtx34*, Mtx34*)
ext 028E9044 void LinkMotionApi::multVecSR(Mtx34*, cXyz*, cXyz*)
ext 025F1B48 void LinkMotionApi::rotateMatrixZXY(Mtx34*, s32, s32, s32)
ext 025F24E0 void LinkMotionApi::translateStack(f32, f32, f32)
data l_splashScaleInitialized 1046D0CC
data l_rippleScaleInitialized 1046D0D0
data l_armFlameScaleInitialized 1046D0EC
data l_splashScale 1046D0D4
data l_rippleScale 1046D0E0
data l_armFlameScale 1046D0F0
data l_flameJoint 10035904
data l_deadRippleScale 100358CC
str 100358E4 in 023F81A4
str 10035950 in 023FD4E4
str 10035964 in 023FEA6C
*/
