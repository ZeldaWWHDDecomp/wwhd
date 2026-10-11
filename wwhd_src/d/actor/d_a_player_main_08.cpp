/**
 * Link weapon, food, sword, tact and particle routines; translation-unit inline copies.
 * Verified against the HD executable.
 * HD differences and compiler-generated copies retain the original call and store order.
 */
#include "d/actor/d_a_player_main_08.h"

// HD: Link's split files share the original translation unit's metadata.
const gc_tu_data gc_tu = {
    .safeString = 0x10034B24,
    .gndChk = {0x10034CB4, 0x10034CC4, 0x10034CE4, 0x10034CD4},
    .gndChkDt = {0x10034C44, 0x10034C64, 0x10034C24},
};
static constexpr dBgS_GndChk_vt l_linkGroundCheckVtables = {0x10034C74, 0x10034C84, 0x10034CA4,
                                                            0x10034C94};
static constexpr u32 l_staticGroundCheckVtable = 0x10037AF8;
// HD: the resource destructor queries the root heap before virtual free.
static u32 l_rootHeap;
static constexpr u32 l_swordBlurVtable = 0x10037BE8;
static constexpr Link08CapsuleVtables l_capsuleVtables = {0x100015A8, 0x10034B9C, 0x1004AF18,
                                                          0x1004AF60, 0x1004AF70};
static Link08MovementDefaults l_movementDefaults;
static Link08HeaderDistances l_headerDistances;
static cXyz l_tact_top;
static cXyz l_swordEffectScale;

static Link08StaticGroundCheck l_staticGroundCheck;
static Link08HeaderZeroState l_headerZeroState;
static Link08HeaderAngleLimits l_headerAngleLimits;
static u8 l_headerByte;
static u8 l_headerFlag;
static Link08DestructorRecord l_headerDestructorRecord0;
static Link08DestructorRecord l_headerDestructorRecord1;
static Link08DestructorRecord l_headerDestructorRecord2;
static Link08DestructorRecord l_headerDestructorRecord3;

static cXyz l_heel_pos;
static cXyz l_toe_pos;
static cXyz l_eye_offset;
static cXyz l_head_center_offset;
static cXyz l_crawl_start_front_offset;
static cXyz l_crawl_front_offset;
static cXyz l_crawl_back_offset;
static cXyz l_crawl_top_offset;
static cXyz l_crawl_front_up_offset;
static cXyz l_crawl_back_up_offset;
static cXyz l_crawl_stand_up_offset;
static cXyz l_crawl_top_up_offset;
static cXyz l_crawl_side_offset;
static cXyz l_crawl_lside_offset;
static cXyz l_crawl_rside_offset;
static cXyz l_crawl_lside_front_offset;
static cXyz l_crawl_rside_front_offset;
static cXyz l_crawl_min_side_offset;
static cXyz l_hammer_splash_particle_scale;
static Link08CylinderSource l_cyl_src;
static Link08CylinderSource l_wind_cyl_src;
static Link08CylinderSource l_at_cyl_src;
static Link08CapsuleSource l_fan_wind_cps_src;
static Link08ResourceName l_sleeveMaterialName;
static Link08ResourceName l_mouthMaterialName;
static Link08ResourceName l_faceMaterialName;
static Link08ResourceName l_legsMaterialName;
static Link08ResourceName l_hatMaterialName;
static Link08ResourceName l_hairMaterialName;
static Link08ResourceName l_earMaterialName;
static Link08ResourceName l_leftEyeMaterialName;
static Link08ResourceName l_rightEyeMaterialName;

static cXyz l_debug_keep_pos;
static csXyz l_debug_shape_angle;
static csXyz l_debug_current_angle;
static u32 l_rollTranslationGuard;
static u32 l_rollScaleGuard;
static cXyz l_rollTranslation;
static cXyz l_rollScale;
static u32 l_waterScaleGuard;
static u32 l_waterSmallScaleGuard;
static u32 l_waterDirectionGuard;
static cXyz l_waterScale;
static cXyz l_waterSmallScale;
static cXyz l_waterDirection;

static inline void dCam_EndEventCamera_l(fopAc_ac_c* a) {
    u32 cam = Link08_getCameraBody();
    Link08_endEventCamera(cam, a->mBsPcId.get());
}

static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return Link08_addCalc(v, target, scale, maxStep, minStep);
}

// HD: obtaining a joint matrix marks its buffer dirty.
static inline Mtx34* lk_getAnmMtx(J3DModel* m, s32 jnt) {
    Link08MtxBuffer* buffer = reinterpret_cast<Link08Model*>(m)->mpMtxBuffer;
    buffer->mFlags = buffer->mFlags | 0x10;
    // HD: guest matrix addresses use 32-bit arithmetic, including the joint offset.
    return gabi::at<Mtx34>(buffer->mpAnmMtx.v.get() + jnt * 0x30);
}
// HD: the mini-game sword state also enables the normal sword.
static inline BOOL lk_checkNormalSwordEquip() {
    return dComIfGs_getSelectEquip(0) == Link08::dItemNo_SWORD_e || dComIfGp_getMiniGameType() == 2;
}

// Calls cross independently verified parts of the GameCube translation unit.
/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */

/* ---- d_a_player_weapon.inc ---- */

/* 0243F058 */
BOOL daPy_lk_c::procWeaponFrontSwingReady() {

    if (mActorKeepEquip.mActor == nullptr) {
        return Link08_checkNextMode(this, 0);
    }
    if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getRate() < 0.01f) {
        return Link08_procWeaponFrontSwing_init(this);
    }
    if (mpAttnActorLockOn != nullptr) {
        Link08_setShapeAngleToAtnActor(this);
        current.angle.y = shape_angle.y;
        mProcVar2 = shape_angle.y;
    } else {
        cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 30, 0x3CDF, 0x1F40); // HD: HIO folded
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}

/* 0243F12C */
BOOL daPy_lk_c::procWeaponFrontSwingEnd_init() {

    Link08_commonProcInit(this, Link08::daPyProc_WEAPON_FRONT_SWING_END_e);
    Link08_setSingleMoveAnime(this, Link08::ANM_HAMSWINGBEND, 0.6f, 5.0f, 8,
                              10.0f); // HD: HIO folded
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    Link08_setEnemyWeaponAtParam(this, 0);
    return TRUE;
}

/* 0243F1A8 */
BOOL daPy_lk_c::procWeaponFrontSwing() {

    if (mActorKeepEquip.mActor == nullptr) {
        return Link08_checkNextMode(this, 0);
    }
    if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getRate() < 0.01f) {
        return procWeaponFrontSwingEnd_init();
    }
    if (Link08_changeCutReverseProc(this, Link08::ANM_CUTREL)) {
        return TRUE;
    }
    if (mpAttnActorLockOn != nullptr) {
        Link08_setShapeAngleToAtnActor(this);
        current.angle.y = shape_angle.y;
        mProcVar2 = shape_angle.y;
    } else {
        cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 30, 0x3CDF, 0x1F40);
        current.angle.y = shape_angle.y;
    }
    setResetFlg0(resetFlg0() | 2); /* onResetFlg0(daPyRFlg0_UNK2) */
    return TRUE;
}

/* 0243F2B0 */
BOOL daPy_lk_c::procWeaponFrontSwingEnd() {

    if ((mItemButton & 2) /* swordButton() */ && mActorKeepEquip.mActor != nullptr) {
        mNoResetFlg0 = mNoResetFlg0 | 4;
    } else {
        mNoResetFlg0 = mNoResetFlg0 & ~4u;
    }
    if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getRate() < 0.01f ||
        mActorKeepEquip.mActor == nullptr) {
        return Link08_checkNextMode(this, 0);
    }
    if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getFrame() > 8.0f /* HD: HIO folded */) {
        return Link08_checkNextMode(this, 1);
    }
    return TRUE;
}

/* 0243F34C */
BOOL daPy_lk_c::procWeaponThrow() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    if (frameCtrl->getRate() < 0.01f || mActorKeepEquip.mActor == nullptr) {
        return Link08_checkNextMode(this, 0);
    }
    if (frameCtrl->getFrame() > 100.0f) {
        return Link08_checkNextMode(this, 1);
    }
    if (frameCtrl->checkPass(10.0f)) {
        fopAc_ac_c* lockOn = mpAttnActorLockOn;
        fopAc_ac_c* boko = mActorKeepEquip.mActor;
        s16 angle = (s16)0xF400;
        if (lockOn != nullptr) {
            gabi::Local<cXyz> d;
            cXyz_mi(&lockOn->eyePos, d, &boko->current.pos);
            f64 dist = Link08_sqrt(Link08_squareMagnitude(d.get()));
            if (!(dist < 1.0f)) {
                gabi::Local<cXyz> xz;
                xz->x = d->x;
                xz->y = 0.0f;
                xz->z = d->z;
                f64 h = Link08_sqrt(Link08_squareMagnitude(xz.get()));
                angle = Link08_atan2s(-d->y, h);
            }
        }
        /* daBoko_c::setThrow(angle) */
        reinterpret_cast<Link08Boko*>(boko)->mThrowAngle = angle;
        reinterpret_cast<Link08Boko*>(boko)->mSetThrow = 1;
        link08VoiceStart(this, 0);
        Link08_seStartSwordCut(this, 0x2800); // HD: an extra sound
        Link08_deleteEquipItem(this, 0);
    }
    return TRUE;
}

/* ---- d_a_player_food.inc ---- */

/* 0243F4DC */
BOOL daPy_lk_c::procFoodThrow() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    Link08_setShipRidePosUseItem(this);
    if (frameCtrl->getRate() < 0.01f) {
        if (mDemo.mDemoType != 5 /* !checkSpecialDemoMode() */) {
            link08CutEnd(this);
        } else {
            dComIfGp_event_reset();
            dCam_EndEventCamera_l(this);
            Link08_endDemoMode(this);
        }
    } else if (frameCtrl->checkPass(8.0f)) {
        seStartOnlyReverb(Link08::JA_SE_LK_ESA_THROW);
        fopAc_ac_c* esa = Link08_fastCreate(Link08::fpcNm_ESA_e, 0xFFFF000Cu, &mLeftHandPos,
                                            (s32)current.roomNo, &shape_angle, nullptr, -1, 0, 0);
        if (esa != nullptr) {
            m3630 = esa != nullptr ? esa->mBsPcId.get() : 0xFFFFFFFFu;
            if (mDemo.mDemoType != 5) {
                if (dComIfGp_event_getTalkXYBtn() == 1 /* dTalkBtn_X_e */) {
                    link08SetReserveBaitEmpty(0);
                } else if (dComIfGp_event_getTalkXYBtn() == 2 /* dTalkBtn_Y_e */) {
                    link08SetReserveBaitEmpty(1);
                } else {
                    link08SetReserveBaitEmpty(2);
                }
            } else {
                link08SetReserveBaitEmpty(mReadyItemBtn);
            }
        }
    }
    return TRUE;
}

/* 0243F670 */
BOOL daPy_lk_c::procFoodSet() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    Link08_setShipRidePosUseItem(this);
    if (frameCtrl->getRate() > 0.0f && frameCtrl->checkPass(9.0f)) {
        seStartOnlyReverb(Link08::JA_SE_LK_HYOI_SET);
    }
    if (std::fabs(frameCtrl->getRate()) < 0.01f) {
        if (mDemo.mDemoType != 5) {
            link08CutEnd(this);
        } else if (mProcVar0 > 0) {
            mProcVar0 = mProcVar0 - 1;
        } else if (mProcVar0 < 0) {
            dComIfGp_event_reset();
            Link08_deleteEquipItem(this, 0);
            dCam_EndEventCamera_l(this);
            Link08_endDemoMode(this);
        } else {
            mProcVar0 = -1;
            frameCtrl->setRate(-1.1f); // HD: -HIO folded
        }
    }
    return TRUE;
}

/* ---- d_a_player_sword.inc ---- */

/* daPy_dmEcallBack_c::checkCurse(): the static curse word  */
static inline BOOL lk_checkCurse() {
    return (u16)l_curseState == 1;
}
/* play + 0x5CD8 / 0x5CDC: dComIfGp_setPlayerStatus0/1(0, flag) */
/* mEquipItem == daPyItem_BOKO_e: hold the upper body animation at its last frame */

/* play + 0x5BB7: dComIfGp_getDoStatus() */

// HD: preserve the frame, mirrored frame and evaluator update order.
static inline void lk_anmSetFrame(be<f32>& storedFrame, gptr<be<f32>>& mirroredFrame,
                                  gptr<Link08AnimationEvaluator>& evaluator, f32 frame) {
    storedFrame = frame;
    *mirroredFrame.get() = frame;
    Link08AnimationEvaluator* ev = evaluator;
    f32 value = gabi::call_ptr<f32>(ev->mEvaluate.get(), ev->mpContext.get(), frame, ev->mA.get(),
                                    ev->mB.get());
    ev->mValue = value;
    Link08_updateAnimationEvaluator(gabi::ea(&evaluator));
}

/* doTrigger() && dComIfGp_getDoStatus() == dActStts_PARRY_e */
static inline bool lk_checkParry(daPy_lk_c* self) {
    return (self->mItemTrigger & 1) && dComIfGp_getDoStatus() == 0x1A;
}
/* frameCtrl.checkPass(passFrame): the speed kick fabs(speedF) * 0.2 + add (HD: HIO folded) */
static inline void lk_cutKick(daPy_lk_c* self, J3DFrameCtrl* frameCtrl, f32 passFrame, f32 add) {
    if (frameCtrl->checkPass(passFrame)) {
        self->mNormalSpeed = gabi::fmadds(std::fabs((f32)self->speedF), 0.2f, add);
    }
}
/* the attack window: the swing sound once, onResetFlg0(daPyRFlg0_UNK2) */
static inline void lk_cutAt(daPy_lk_c* self, J3DFrameCtrl* frameCtrl, f32 start, f32 end) {
    if (frameCtrl->getFrame() >= start && frameCtrl->getFrame() < end) {
        if (!(self->mNoResetFlg0 & 0x40) /* !checkNoResetFlg0(daPyFlg0_CUT_AT_FLG) */) {
            self->setResetFlg0(self->resetFlg0() | 1);
            self->seStartSwordCut(Link08::JA_SE_LK_SW_KAZEKIRI_S);
        }
        self->setResetFlg0(self->resetFlg0() | 2);
    }
}

/* the common body of procCutA/F/R/L (GameCube: four copies with their own HIO block; HD: HIO
 * folded) */
static inline BOOL lk_procCutCommon(daPy_lk_c* self, J3DFrameCtrl* frameCtrl, u8 direction,
                                    u8 nextCut, f32 endFrame, f32 passFrame, f32 kickAdd,
                                    f32 atStart, f32 atEnd, f32 calcMax, u16 reverseAnm) {
    self->m3522 = 2;
    if (frameCtrl->getRate() < 0.01f) {
        self->mNormalSpeed = 0.0f;
        self->mDirection = direction;
        Link08_checkNextMode(self, 0);
        return TRUE;
    }
    if (frameCtrl->getFrame() > endFrame) {
        f32 speed = self->mNormalSpeed;
        u8 dir = self->mDirection;
        self->mNormalSpeed = 0.0f;
        self->mDirection = direction;
        if (Link08_checkNextMode(self, 1)) {
            return TRUE;
        }
        self->mDirection = dir;
        self->mNormalSpeed = speed;
    }
    if (Link08_changeCutReverseProc(self, (u32)reverseAnm)) {
        return TRUE;
    }
    Link08_setSpecialBattle(self, 1);
    if (lk_checkParry(self)) {
        return Link08_changeSpecialBattle(self);
    }
    if ((self->mItemTrigger & 2) /* swordTrigger() */ && self->m34C5 != 5) {
        self->m34C5 = nextCut;
    }
    self->m34C2 = 1;
    if (self->mpAttnActorLockOn != nullptr) {
        Link08_setShapeAngleToAtnActor(self);
        self->mProcVar2 = self->shape_angle.y;
    } else {
        cLib_addCalcAngleS(&self->shape_angle.y, self->mProcVar2, 30, 0x3CDF, 0x1F40);
    }
    self->current.angle.y = self->shape_angle.y;
    lk_cutKick(self, frameCtrl, passFrame, kickAdd);
    lk_cutAt(self, frameCtrl, atStart, atEnd);
    cLib_addCalc(&self->mNormalSpeed, 0.0f, 0.7f, calcMax, 0.5f);
    return TRUE;
}
/* swordButton() && m34C5 == 0: on/offNoResetFlg0(daPyFlg0_UNK4) */

/* 0243F790 */
BOOL daPy_lk_c::procCutA() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    m35EC = frameCtrl->getFrame();
    link08CutHoldFlag(this);
    return lk_procCutCommon(this, frameCtrl, 3 /* DIR_RIGHT */, 1, 16.0f, 6.0f, 10.0f, 5.0f, 11.0f,
                            2.6f, Link08::ANM_CUTRER);
}

/* 0243FAFC */
BOOL daPy_lk_c::procCutF() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    m35EC = frameCtrl->getFrame();
    link08CutHoldFlag(this);
    Link08CutAnimations& animations = *reinterpret_cast<Link08CutAnimations*>(this);
    lk_anmSetFrame(animations.mBpkFrame, animations.mpBpkFrame, animations.mpBpkEvaluator,
                   frameCtrl->getFrame());
    lk_anmSetFrame(animations.mBtkFrame, animations.mpBtkFrame, animations.mpBtkEvaluator,
                   frameCtrl->getFrame());
    return lk_procCutCommon(this, frameCtrl, 3 /* DIR_RIGHT */, 2, 17.0f, 6.0f, 8.0f, 5.0f, 12.0f,
                            0.95f, 0x27);
}

/* 0243FF44 */
BOOL daPy_lk_c::procCutR() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    m35EC = frameCtrl->getFrame();
    link08CutHoldFlag(this);
    return lk_procCutCommon(this, frameCtrl, 3 /* DIR_RIGHT */, 3, 16.0f, 6.0f, 1.0f, 6.0f, 12.0f,
                            0.95f, 0x27);
}

/* 024402B0 */
BOOL daPy_lk_c::procCutL() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    m35EC = frameCtrl->getFrame();
    link08CutHoldFlag(this);
    return lk_procCutCommon(this, frameCtrl, 2 /* DIR_LEFT */, 4, 16.0f, 6.0f, 1.0f, 5.0f, 10.0f,
                            0.95f, Link08::ANM_CUTREL);
}

/* procCutEA/EB: the end of the rate countdown (mProcVar0 extra frames) */
static inline BOOL lk_procCutE(daPy_lk_c* self, J3DFrameCtrl* frameCtrl, u8 dir, u32 reverseAnm,
                               bool lockOnOnly, f32 kickAdd, f32 calcMax) {
    if (frameCtrl->getRate() < 0.01f) {
        if (self->mProcVar0 > 0) {
            self->mProcVar0 = self->mProcVar0 - 1;
        } else {
            self->mNormalSpeed = 0.0f;
            self->mDirection = dir;
            Link08_checkNextMode(self, 0);
            return TRUE;
        }
    }
    if (Link08_changeCutReverseProc(self, reverseAnm)) {
        return TRUE;
    }
    if (!lockOnOnly || self->mpAttnActorLockOn != nullptr) {
        Link08_setShapeAngleToAtnActor(self);
        self->current.angle.y = self->shape_angle.y;
    }
    Link08_setSpecialBattle(self, 1);
    if (lk_checkParry(self)) {
        return Link08_changeSpecialBattle(self);
    }
    lk_cutKick(self, frameCtrl, 6.0f, kickAdd);
    lk_cutAt(self, frameCtrl, 5.0f, 11.0f);
    self->m34C2 = 1;
    cLib_addCalc(&self->mNormalSpeed, 0.0f, 0.7f, calcMax, 0.5f);
    return TRUE;
}

/* 0244061C */
BOOL daPy_lk_c::procCutEA() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    m35EC = frameCtrl->getFrame();
    link08CutHoldFlag(this);
    return lk_procCutE(this, frameCtrl, 3 /* DIR_RIGHT */, Link08::ANM_CUTREL, true, 15.0f, 4.0f);
}

/* 024408B0 */
BOOL daPy_lk_c::procCutEB() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    m35EC = frameCtrl->getFrame();
    link08CutHoldFlag(this);
    return lk_procCutE(this, frameCtrl, 2 /* DIR_LEFT */, Link08::ANM_CUTRER, false, 7.0f, 1.5f);
}

/* procCutExA/Kesa: the rate end, the 0x80 mode flag at passFrame, the sword trigger to nextCut */
static inline BOOL lk_procCutExA(daPy_lk_c* self, J3DFrameCtrl* frameCtrl, s16 m3522, f32 endFrame,
                                 f32 passFrame, u8 nextCut, f32 atStart, f32 atEnd) {
    self->m3522 = m3522;
    if (frameCtrl->getRate() < 0.01f) {
        self->mNormalSpeed = 0.0f;
        Link08_checkNextMode(self, 0);
        return TRUE;
    }
    if (frameCtrl->getFrame() > endFrame) {
        f32 speed = self->mNormalSpeed;
        self->mNormalSpeed = 0.0f;
        if (Link08_checkNextMode(self, 1)) {
            return TRUE;
        }
        self->mNormalSpeed = speed;
    } else if (frameCtrl->checkPass(passFrame)) {
        self->mModeFlg = self->mModeFlg | 0x80; /* onModeFlg(ModeFlg_00000080) */
    }
    if (Link08_changeCutReverseProc(self, Link08::ANM_CUTREL)) {
        return TRUE;
    }
    Link08_setSpecialBattle(self, 1);
    if (lk_checkParry(self)) {
        return Link08_changeSpecialBattle(self);
    }
    if ((self->mItemTrigger & 2) && self->m34C5 != 5) {
        self->m34C5 = nextCut;
    }
    if (self->mpAttnActorLockOn != nullptr) {
        Link08_setShapeAngleToAtnActor(self);
        self->current.angle.y = self->shape_angle.y;
    }
    lk_cutAt(self, frameCtrl, atStart, atEnd);
    self->m34C2 = 1;
    cLib_addCalc(&self->mNormalSpeed, 0.0f, 0.7f, 4.0f, 0.5f);
    return TRUE;
}

/* 02440B38 */
BOOL daPy_lk_c::procCutExA() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    m35EC = frameCtrl->getFrame();
    link08CutHoldFlag(this);
    return lk_procCutExA(this, frameCtrl, 1, 18.0f, 11.0f, 26, 12.0f, 18.0f);
}

/* 02440E5C */
BOOL daPy_lk_c::procCutExB() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    m35EC = frameCtrl->getFrame();
    link08CutHoldFlag(this);
    if (frameCtrl->getRate() < 0.01f) {
        if (mProcVar0 > 0) {
            mProcVar0 = mProcVar0 - 1;
        } else {
            mNormalSpeed = 0.0f;
            Link08_checkNextMode(this, 0);
            return TRUE;
        }
    } else if (frameCtrl->checkPass(14.0f)) {
        mModeFlg = mModeFlg | 0x80;
    }
    if (Link08_changeCutReverseProc(this, Link08::ANM_CUTREL)) {
        return TRUE;
    }
    if (mpAttnActorLockOn != nullptr) {
        Link08_setShapeAngleToAtnActor(this);
        current.angle.y = shape_angle.y;
    }
    Link08_setSpecialBattle(this, 1);
    if (lk_checkParry(this)) {
        return Link08_changeSpecialBattle(this);
    }
    lk_cutAt(this, frameCtrl, 14.0f, 20.0f);
    m34C2 = 1;
    cLib_addCalc(&mNormalSpeed, 0.0f, 0.7f, 4.0f, 0.5f);
    return TRUE;
}

/* 02441104 */
BOOL daPy_lk_c::procCutExMJ() {

    m35EC = mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getFrame();
    link08CutHoldFlag(this);
    if (m34C4 != 0) {
        m3522 = 2;
    }
    if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getRate() < 0.01f) {
        if (mProcVar2 == 0) {
            mProcVar2 = 1;
        } else if (mProcVar2 == 1) {
            Link08_setSingleMoveAnime(this, Link08::ANM_JMPEDS, 0.0f, 0.0f, -1, 30.0f);
            mModeFlg = mModeFlg & ~0x400u; /* offModeFlg(ModeFlg_00000400) */
            Link08_setTextureAnime(this, 0x37, 0);
            resetSeAnime();
            mProcVar2 = 2;
        }
        if (mProcVar3 == 0x71) {
            if (!(m34EC > 0) || m34EC > 0x4000) {
                m34EC = m34EC - 0x1800;
            } else {
                cLib_addCalcAngleS(&m34EC, 1, 2, 0x1800, 0x800);
            }
        } else if (!(m34EC < 0) || m34EC < -0x4000) {
            m34EC = m34EC + 0x1800;
        } else {
            cLib_addCalcAngleS(&m34EC, -1, 2, 0x1800, 0x800);
        }
        if (mAcch.ChkGroundHit()) {
            return Link08_changeLandProc(this, 1.3f); // HD: HIO folded
        }
    }
    if (mpAttnActorLockOn != nullptr) {
        Link08_setShapeAngleToAtnActor(this);
        current.angle.y = shape_angle.y;
    }
    Link08_setSpecialBattle(this, 1);
    if (lk_checkParry(this)) {
        return Link08_changeSpecialBattle(this);
    }
    if (m34C4 != 0 && ((mItemTrigger & 2) && m34C5 != 5)) {
        m34C5 = 30;
    }
    if (speed.y < -gravity) {
        gravity = -1.7f;
    }
    Link08_setFallVoice(this);
    if (mProcVar2 != 2 &&
        !(mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getFrame() < m35A0)) { /* NaN: in the window */
        if (!(mNoResetFlg0 & 0x40)) {
            setResetFlg0(resetFlg0() | 1);
            seStartSwordCut(0x2800);
        }
        setResetFlg0(resetFlg0() | 2);
    }
    if (current.pos.y < m3688.y - 200.0f) {
        cLib_chaseF(&mNormalSpeed, 0.0f, 0.08000000566244125f); // HD: 0.1f * HIO (0.8f) folded,
    }
    return TRUE;
}

/* 02441450 */
BOOL daPy_lk_c::procCutKesa() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    m35EC = frameCtrl->getFrame();
    link08CutHoldFlag(this);
    return lk_procCutExA(this, frameCtrl, 2, 32.0f, 16.0f, 31, 22.0f, 26.0f);
}

/* 02441750 */
BOOL daPy_lk_c::procCutTurn() {

    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[Link08::UNDER_MOVE0_e];
    m35EC = frameCtrl->getFrame();
    if ((mProcVar6 != 0 && (mItemButton & 2)) && m34C5 == 0) {
        mNoResetFlg0 = mNoResetFlg0 | 4;
    } else {
        mNoResetFlg0 = mNoResetFlg0 & ~4u;
    }
    if (frameCtrl->getRate() < 0.01f) {
        if (mProcVar0 > 0) {
            mProcVar0 = mProcVar0 - 1;
        } else {
            mDirection = 3; /* DIR_RIGHT */
            Link08_checkNextMode(this, 0);
            link08JumpCutFollowUp(this) = 0; /* HD */
            return TRUE;
        }
    } else if (frameCtrl->getFrame() > 21.0f) {
        u8 dir = mDirection;
        mDirection = 3;
        if (Link08_checkNextMode(this, 1)) {
            link08JumpCutFollowUp(this) = 0; /* HD */
            return TRUE;
        }
        mDirection = dir;
    } else if (frameCtrl->getFrame() > 17.0f) {
        Link08_mtxFollow_end(&m32E4);
    }
    if (frameCtrl->getFrame() > m35A0) {
        Link08_mtxFollow_end(&m32F0);
        Link08_endSmokeCallback(mSmokeEcallBack);
    }
    Link08_setSpecialBattle(this, 1);
    if (lk_checkParry(this)) {
        return Link08_changeSpecialBattle(this);
    }
    if (frameCtrl->getFrame() >= 3.0f && frameCtrl->getFrame() < 18.5f) {
        if (!(mNoResetFlg0 & 0x40)) {
            setResetFlg0(resetFlg0() | 1);
            seStartSwordCut(Link08::JA_SE_LK_KAITENGIRI);
        }
        setResetFlg0(resetFlg0() | 2);
        cLib_chaseF(&mAtCyl.mCyl.mRadius, m35A4, 18.0f);
    }
    /* m331C/m332C/m333C (dPa_cutTurnEcallBack_c) alpha at +4 */
    if (frameCtrl->checkPass(18.5f)) {
        link08CutTurnCallback(m332C).mAlpha = 0x80;
        link08CutTurnCallback(m333C).mAlpha = 0x80;
        link08CutTurnCallback(m331C).mAlpha = 0x80;
    } else if (link08CutTurnCallback(m331C).mAlpha == 0x80) {
        link08CutTurnCallback(m331C).mAlpha = 0;
        link08CutTurnCallback(m332C).mAlpha = 0;
        link08CutTurnCallback(m333C).mAlpha = 0;
    }
    m34C2 = 1;
    return TRUE;
}

/* 02441A44 */
BOOL daPy_lk_c::procCutRoll_init() {

    Link08_commonProcInit(this, Link08::daPyProc_CUT_ROLL_e);
    Link08_setSingleMoveAnime(this, Link08::ANM_CUTTURNB, 1.2f, 4.0f, 6, 1.0f);
    m3578 = 0;
    seStartMapInfo(Link08::JA_SE_LK_V_KAITEN_S);
    /* dComIfGp_setItemMagicCount(-2) */
    dComIfGp_setItemMagicCount(-2);
    link08CutType(this) =
        9; /* mCutType = CUT_TYPE_CUT_ROLL (daPy_py_c, plain u8 in d_a_player.h) */
    link08CutCount(this) = 0;                 /* mCutCount */
    setResetFlg0(resetFlg0() & ~0x08000000u); /* offResetFlg0(daPyRFlg0_NOT_ATTACKING) */
    if (lk_checkNormalSwordEquip()) {
        m35A4 = 180.0f;
        if (noResetFlg1() & Link08::daPyFlg1_SOUP_POWER_UP) {
            mAtCyl.mAtp = 4;
        } else {
            mAtCyl.mAtp = 2;
        }
    } else {
        m35A4 = 230.0f;
        if (noResetFlg1() & 0x8000) {
            mAtCyl.mAtp = 8;
        } else {
            mAtCyl.mAtp = 4;
        }
    }
    mAtCyl.SetR(m35A4 * 0.5f);
    mProcVar0 = 90;
    current.angle.y = shape_angle.y;
    mFootEffectPosType = 4; /* setFootEffectPosType(4) */
    dComIfGp_setPlayerStatus0(0, 0x40000000);
    Link08_endFlameDamageEmitter(this);
    return TRUE;
}

/* 02441C38 */
BOOL daPy_lk_c::procCutRollEnd_init() {

    Link08_commonProcInit(this, Link08::daPyProc_CUT_ROLL_END_e);
    Link08_setSingleMoveAnime(this, Link08::ANM_WAITQ, 1.3f, 0.0f, -1, 2.4f);
    mNormalSpeed = 0.0f;
    current.angle.y = shape_angle.y;
    mProcVar0 = 60;
    if (l_rollTranslationGuard == 0) {
        l_rollTranslation.z = 0.0f;
        l_rollTranslationGuard = 1;
        l_rollTranslation.y = 55.0f;
        l_rollTranslation.x = 0.0f;
    }
    if (l_rollScaleGuard == 0) {
        l_rollScaleGuard = 1;
        l_rollScale.x = 0.76f;
        l_rollScale.z = 0.76f;
        l_rollScale.y = 0.76f;
    }
    Mtx34* mtx = lk_getAnmMtx(mpCLModel, Link08::CL_JNT_HEAD_JNT_e /* CL_JNT_HEAD_JNT_e */);
    Link08Emitter* emitter = reinterpret_cast<Link08Emitter*>(Link08_mtxFollow_makeEmitter(
        m33A8, Link08::ID_IT_JN_PIYOPIYO00, mtx, &current.pos, nullptr));
    if (emitter != 0) {
        emitter->mFlags = emitter->mFlags | 0x40;      /* becomeImmortalEmitter */
        emitter->mVolumeSize = 5;                      /* setVolumeSize(5) */
        emitter->mLifeTime = 60;                       /* setLifeTime (HD: HIO folded) */
        emitter->mTranslation.x = l_rollTranslation.x; /* setEmitterTranslation */
        emitter->mTranslation.y = l_rollTranslation.y;
        emitter->mTranslation.z = l_rollTranslation.z;
        emitter->mGlobalParticleScale.x = l_rollScale.x; /* setGlobalParticleScale */
        emitter->mGlobalParticleScale.y = l_rollScale.y;
        emitter->mGlobalParticleScale.z = l_rollScale.z;
    }
    return TRUE;
}

/* 02441DC0 */
BOOL daPy_lk_c::procCutRoll() {

    if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getRate() < 0.01f) {
        s16 oldAngle = current.angle.y;
        if (mAcch.ChkWallHit() && mDemo.mDemoMode != 0x2B /* daPy_demo_c::DEMO_CUT_ROLL_e */) {
            f32 dist = gabi::fadds_ppc(mAcchCir[0].m_wall_r, 25.0f);
            gabi::Local<cXyz> start;
            gabi::Local<cXyz> end;
            start->x = current.pos.x;
            start->y = gabi::fadds_ppc(current.pos.y, mAcchCir[0].m_wall_h);
            start->z = current.pos.z;
            end->x = gabi::fmadds(cM_ssin(current.angle.y), dist, start->x);
            end->y = start->y;
            end->z = gabi::fmadds(cM_scos(current.angle.y), dist, start->z);
            dBgS_LinChk_Set(mLinkLinChk, start, end, this);
            if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
                u32 pla = Link08_getTrianglePlane(dComIfG_Bgsp(),
                                                  (u32)link08LineCheck(this).mPolyInfo.mBgIndex,
                                                  (u32)link08LineCheck(this).mPolyInfo.mPolyIndex);
                if (pla != 0) { // HD: NULL check
                    Link08TrianglePlane* plane = gabi::at<Link08TrianglePlane>(pla);
                    shape_angle.y = cM_atan2s(-plane->mNormal.x, -plane->mNormal.z);
                    current.angle.y = shape_angle.y;
                }
                return Link08_procFrontRollCrash_init(this);
            }
        }
        if (mStickDistance > 0.05f) {
            cLib_addCalcAngleS(&current.angle.y, m34E8, 5, 0xBB8, 0x64); // HD: HIO folded
            shape_angle.y = current.angle.y;
        }
        s16 turn = (s16)(current.angle.y - oldAngle);
        if (dComIfGp_event_runCheck()) {
            if (mDemo.mDemoMode != 0x2B) {
                mProcVar0 = 0;
            } else {
                mProcVar0 = 5;
            }
        }
        if (mProcVar0 > 0) {
            mProcVar0 = mProcVar0 - 1;
            mNormalSpeed = 30.0f;
            m34EC = m34EC - (0x36B0 + turn);
            if (!(mNoResetFlg0 & 0x40)) {
                setResetFlg0(resetFlg0() | 1);
            }
            setResetFlg0(resetFlg0() | 2);
            cLib_chaseF(&mAtCyl.mCyl.mRadius, m35A4, 18.0f);
            seStartMapInfo(Link08::JA_SE_LK_V_KAITEN_S);
        } else if (Link08_addCalcDouble(&mNormalSpeed, 0.0f, 0.1f, 2.5f, 1.0f) < 0.5f) {
            shape_angle.y = shape_angle.y + m34EC;
            current.angle.y = shape_angle.y;
            if (dComIfGp_event_runCheck() && mDemo.mDemoMode != 0x2B) {
                Link08_procWait_init(this);
            } else {
                procCutRollEnd_init();
            }
        } else {
            f32 rate = mNormalSpeed / 30.0f;
            m34EC = (s16)gabi::ftoi((f32)m34EC - gabi::fmadds((f32)(0x34B0 + turn), rate, 512.0f));
        }
    } else {
        seStartMapInfo(0x1059);
    }
    return TRUE;
}

/* 02442134 */
BOOL daPy_lk_c::procCutRollEnd() {

    seStartOnlyReverb(Link08::JA_SE_CM_MD_PIYO);
    if (mProcVar0 > 0) {
        mProcVar0 = mProcVar0 - 1;
    } else {
        Link08_checkNextMode(this, 0);
    }
    return TRUE;
}

/* 02442190 */
BOOL daPy_lk_c::procCutTurnMove_init() {

    Link08_commonProcInit(this, Link08::daPyProc_CUT_TURN_MOVE_e);
    Link08_setSingleMoveAnime(this, Link08::ANM_CUTTURNPWFB, 0.0f, 0.0f, -1, 3.0f);
    link08BokoUpperEnd(this);
    m3598 = 1.0f;
    mDirection = 0; /* DIR_FORWARD */
    BOOL bit;
    if (dComIfGs_isEventBit(0xB20)) {
        bit = TRUE;
    } else if (dComIfGs_isTmpBit(0x402) /* dComIfGs_isTmpBit */) {
        bit = TRUE;
    } else {
        bit = FALSE;
    }
    // HD: checkSwordMiniGame() and dComIfGp_getMiniGameType() != 6 both read play + 0x5CEA
    if (bit && mEquipItem == Link08::daPyItem_SWORD_e && dComIfGs_getMagic() /* getMagic */ >= 2 &&
        dComIfGp_getMiniGameType() != 2 && dComIfGp_getMiniGameType() != 6) {
        cXyz* scale = lk_checkNormalSwordEquip() ? nullptr : &l_swordEffectScale;
        Mtx34* mtx = lk_getAnmMtx(mpCLModel, Link08::CL_JNT_CL_LHANDA_e /* CL_JNT_CL_LHANDA_e */);
        Link08_mtxFollow_makeEmitter(&m32E4, Link08::ID_AK_JN_CHARGEPOWER00, mtx, &current.pos,
                                     scale);
        mtx = lk_getAnmMtx(mpCLModel, Link08::CL_JNT_CL_LHANDA_e);
        Link08_mtxFollow_makeEmitter(&m32F0, Link08::ID_AK_JN_CHARGEPOWER01, mtx, &current.pos,
                                     scale);
        mProcVar0 = 47;
    } else {
        mProcVar0 = -1;
    }
    dComIfGp_setPlayerStatus0(0, Link08::daPyStts0_SPIN_ATTACK_e);
    return TRUE;
}

/* 0244240C */
BOOL daPy_lk_c::procCutTurnCharge() {

    if (!(mItemButton & 2) || lk_checkCurse()) {
        Link08_checkNextMode(this, 0);
    } else if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getRate() < 0.01f) {
        procCutTurnMove_init();
    }
    return TRUE;
}

/* 02442470 */
BOOL daPy_lk_c::procCutTurnMove() {

    Link08_setShapeAngleToAtnActor(this);
    m34E6 = shape_angle.y;
    if (0 < mProcVar0) {
        mProcVar0 = mProcVar0 - 1;
        if (mProcVar0 == 0) {
            dComIfGp_setPlayerStatus1(0, Link08::daPyStts1_UNK20000_e);
        }
    }
    if (mProcVar0 != -1) {
        Link08_startSystemSound(Link08::JA_SE_LK_SWORD_CHARGE);
    }
    if (Link08_changeSlideProc(this)) {
        return TRUE;
    }
    if (lk_checkCurse()) {
        Link08_checkNextMode(this, 0);
    } else if (mItemButton & 2) {
        f32 speed = 0.0f;
        if (mStickDistance > 0.05f) {
            s16 target = m34E8;
            int direction = Link08_getDirectionFromAngle(this, (s32)(s16)(target - m34E6));
            u32 anm = 0xFFFF;
            cLib_addCalcAngleS(&current.angle.y, target, 4, 12000, 0x2000);
            if (mDirection == 0 /* DIR_FORWARD */ || mDirection == 1 /* DIR_BACKWARD */) {
                if (direction == 2 /* DIR_LEFT */ || direction == 3 /* DIR_RIGHT */) {
                    anm = 0x1A; /* ANM_CUTTURNPWLR */
                    mNormalSpeed = gabi::fmuls_ppc(mNormalSpeed, 0.5f);
                } else if (mDirection != direction) {
                    mNormalSpeed = gabi::fmuls_ppc(mNormalSpeed, 0.5f);
                }
            } else if (direction == 0 || direction == 1) {
                anm = 0x19; /* ANM_CUTTURNPWFB */
                mNormalSpeed = gabi::fmuls_ppc(mNormalSpeed, 0.5f);
            } else if (mDirection != direction) {
                mNormalSpeed = gabi::fmuls_ppc(mNormalSpeed, 0.5f);
            }
            if (mDirection == direction) {
                speed = gabi::fmuls_ppc(3.5f, mStickDistance); // HD: HIO folded
            } else {
                mDirection = direction;
            }
            if (anm != 0xFFFF) {
                Link08_setSingleMoveAnime(this, anm, 0.0f, 0.0f, -1, 3.0f);
            }
        }
        Link08_setNormalSpeedF(this, speed, 0.5f, 2.0f, 0.25f);
        f32 anmSpeed;
        if (mDirection == 0) {
            anmSpeed = 0.8f;
        } else if (mDirection == 1) {
            anmSpeed = -0.8f;
        } else if (mDirection == 3) {
            anmSpeed = -0.8f;
        } else {
            anmSpeed = 0.8f;
        }
        f32 rate = mNormalSpeed / mMaxNormalSpeed;
        mFrameCtrlUnder[Link08::UNDER_MOVE0_e].setRate(rate * anmSpeed);
        if (anmSpeed >= 0.0f) {
            mFrameCtrlUnder[Link08::UNDER_MOVE0_e].mLoop =
                mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getStart();
        } else {
            mFrameCtrlUnder[Link08::UNDER_MOVE0_e].mLoop =
                mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getEnd();
        }
        Link08_initSeAnime(this);
        m3598 = gabi::fnmsubs(0.050000011920928955f /* 1.0f - 0.95f */, rate, 1.0f);
        if (mNormalSpeed <= 0.001f || mNormalSpeed != mNormalSpeed) {
            mModeFlg = mModeFlg | 1; /* onModeFlg(ModeFlg_00000001) */
            m3598 = 0.0f;
        } else {
            mModeFlg = mModeFlg & ~1u;
            m3598 = 1.0f;
        }
        link08BokoUpperEnd(this);
    } else if (mProcVar0 == 0) {
        procCutRoll_init();
    } else {
        mNormalSpeed = speedF;
        Link08_procCutTurn_init(this, 0);
    }
    return TRUE;
}

/* 02442878 */
BOOL daPy_lk_c::procCutReverse() {

    cLib_addCalc(&mNormalSpeed, 0.0f, 0.5f, 1.125f, 0.3f); // HD: HIO folded
    if ((mItemButton & 2) && mEquipItem == Link08::daPyItem_SWORD_e) {
        mNoResetFlg0 = mNoResetFlg0 | 4;
    } else {
        mNoResetFlg0 = mNoResetFlg0 & ~4u;
    }
    if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getRate() < 0.01f) {
        current.angle.y = shape_angle.y;
        Link08_checkNextMode(this, 0);
    } else if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getFrame() > 11.0f) {
        current.angle.y = shape_angle.y;
        if (!Link08_checkNextMode(this, 1)) {
            current.angle.y = shape_angle.y + 0x8000;
        }
    }
    return TRUE;
}

/* 02442990 */
BOOL daPy_lk_c::procJumpCutLand_init() {

    f32 fall = m35F0 - current.pos.y;
    Link08_commonProcInit(this, Link08::daPyProc_JUMP_CUT_LAND_e);
    if (!dComIfGp_event_runCheck()) {
        // HD: HIO folded (100 * 60, 100 * 20); NaN counts as a long fall
        if (!(fall < 6000.0f)) {
            Link08_setDamagePoint(this, -2.0f);
        } else if (!(fall < 2000.0f)) {
            Link08_setDamagePoint(this, -1.0f);
        }
        if (!(fall < 2000.0f)) {
            mDamageWaitTimer = 30;
            mModeFlg = mModeFlg | 8; /* onModeFlg(ModeFlg_DAMAGE) */
        }
    }
    mNormalSpeed = 0.0f;
    Link08_setSingleMoveAnime(this, Link08::ANM_JATTACKLAND, 1.1f, 1.0f, 14, 2.0f);
    if (mEquipItem == Link08::daPyItem_NONE_e) {
        mProcVar6 = 0;
    } else {
        setResetFlg0(resetFlg0() | 2);
        if (!(mNoResetFlg0 & 0x40)) {
            seStartSwordCut(0x2800);
            Mtx34* mtx = link08BaseTRMtx(mpCLModel); /* getBaseTRMtx */
            f64 rate = Link08_getBlurTopRate(this);
            s32 color = Link08_getSwordBlurColor(this);
            Link08_initSwordBlur(mpSwBlur.get(), mtx, 130 /* 10 * HIO folded */, rate, color);
            mNoResetFlg0 = mNoResetFlg0 | 0x40;
        }
        Link08_setJumpCutAtParam(this);
        mProcVar6 = 1;
    }
    mProcVar0 = 2;
    current.angle.y = shape_angle.y + 0x8000;
    mFootEffectPosType = 3;
    setResetFlg0(resetFlg0() | 0xC00); /* RIGHT_FOOT_ON_GROUND | LEFT_FOOT_ON_GROUND */
    if (mEquipItem == Link08::dItemNo_SKULL_HAMMER_e) {
        Link08_setHammerQuake(this, 0, 0, -1);
    } else if (Link08_checkHeavyStateOn(this)) {
        gabi::Local<cXyz> dir;
        dir->x = 0.0f;
        dir->y = 1.0f;
        dir->z = 0.0f;
        dComIfGp_getVibration_StartShock(5, -0x31, dir);
    }
    return TRUE;
}

/* 02442C1C */
BOOL daPy_lk_c::procJumpCut() {

    // HD: the spin-attack follow-up of procJumpCutLand is decided here (latched in
    // link08JumpCutFollowUp(this))
    s32 v = m3578;
    if ((v < 0 ? -v : v) > 0xF800 && !lk_checkCurse() &&
        (mEquipItem == Link08::daPyItem_SWORD_e || mEquipItem == Link08::daPyItem_BOKO_e)) {
        link08JumpCutFollowUp(this) = 1;
        Link08_setAtCpsSpl(this, 0);
    }
    if (mAcch.ChkGroundHit()) {
        return procJumpCutLand_init();
    }
    if (current.angle.y == shape_angle.y &&
        Link08_changeCutReverseProc(this, Link08::ANM_JATTACK)) {
        current.angle.y = current.angle.y + 0x8000;
        mNormalSpeed = 27.0f;
    } else if (current.angle.y != shape_angle.y) {
        cLib_addCalc(&mNormalSpeed, 5.0f, 0.2f, 1.0f, 0.1f);
    }
    if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getFrame() >= 13.0f &&
        mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getFrame() < 15.0f) {
        if (mEquipItem != Link08::daPyItem_NONE_e) {
            if (!(mNoResetFlg0 & 0x40)) {
                setResetFlg0(resetFlg0() | 1);
                seStartSwordCut(0x2800);
            }
            setResetFlg0(resetFlg0() | 2);
        }
    }
    return TRUE;
}

/* 02442DC0 */
BOOL daPy_lk_c::procJumpCutLand() {

    current.angle.y = shape_angle.y;
    Link08_resetFootEffect(this);
    if (mProcVar6 != 0 && Link08_changeCutReverseProc(this, Link08::ANM_CUTRER)) {
        return TRUE;
    }
    if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getRate() < 0.01f) {
        if (mProcVar0 > 0) {
            mProcVar0 = mProcVar0 - 1;
            Link08_checkNextMode(this, 1);
        } else {
            Link08_checkNextMode(this, 0);
        }
    } else if (mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getFrame() > 12.0f) {
        Link08_checkNextMode(this, 1);
    } else if (mProcVar6 != 0) {
        mProcVar6 = 0;
        if (link08JumpCutFollowUp(this) != 0) { // HD: latched by procJumpCut
            Link08_procCutTurn_init(this, 1);
        } else {
            setResetFlg0(resetFlg0() | 2);
        }
    }
    return TRUE;
}

/* ---- d_a_player_boomerang.inc ---- */

/* 02442ED8 */
BOOL daPy_lk_c::returnBoomerang() {

    dComIfGp_clearPlayerStatus0(0, 0x00400000);
    if (mEquipItem == Link08::daPyItem_NONE_e &&
        dComIfGp_getPlayer(0) == this /* !checkNoControll() */ && !dComIfGp_event_runCheck() &&
        mDemo.mDemoType == 0 /* !checkPlayerDemoMode() */ &&
        (link08UpperHeaps(this)[Link08::UPPER_MOVE2_e].mIdx == 0xFFFF /* checkNoUpperAnime() */ ||
         gabi::call_ptr<BOOL>(link08VirtualTable(this)->mCheckPlayerGuard.get(),
                              this) /* checkPlayerGuard() (virtual) */
         ||
         link08UpperHeaps(this)[Link08::UPPER_MOVE2_e].mIdx == 0x64 /* checkDashDamageAnime() */ ||
         Link08_checkBoomerangAnime(this)) &&
        (!(mModeFlg & 0x01FD2810) || mCurProc == Link08::daPyProc_SHIP_BOOMERANG_e)) {
        u32 id = mActorKeepThrow.mID;
        fopAc_ac_c* actor = mActorKeepThrow.mActor;
        mActorKeepEquip.mID = id;
        mActorKeepEquip.mActor = actor;
        Link08_clearActorKeep(&mActorKeepThrow);
        mEquipItem = Link08::dItemNo_BOOMERANG_e; /* dItemNo_BOOMERANG_e */
        mNoResetFlg0 = mNoResetFlg0 | 0x20;
        return TRUE;
    }
    Link08_clearActorKeep(&mActorKeepThrow);
    return FALSE;
}

/* ---- d_a_player_ship.inc ---- */

/* 0244300C */
BOOL daPy_lk_c::shipSpecialDemoStart() {

    if (Link08_compulsoryEvent(dComIfGp_ea() + 0x51D0, this, 0, 0xFFFF)) {
        mDemo.mDemoType = 5; /* mDemo.setSpecialDemoType() */
        return TRUE;
    }
    return FALSE;
}

/* ---- d_a_player_tact.inc ---- */

/* 0244307C */
BOOL daPy_lk_c::checkEndTactMusic() {

    if (mCurProc == Link08::daPyProc_TACT_PLAY_e && mProcVar0 != 0) {
        return TRUE;
    }
    return FALSE;
}

/* 024430C0 */
f32 daPy_lk_c::getTactMetronomeRate() {

    if (mCurProc == Link08::daPyProc_TACT_WAIT_e) {
        f64 beat = Link08_getTactBeatFrames();
        return (f32)((f64)(f32)m35A0 / beat);
    }
    return -1.0f;
}

/* 0244311C */
BOOL daPy_lk_c::checkTactLastInput() {

    if (mCurProc == Link08::daPyProc_TACT_WAIT_e && mProcVar1 >= 0) { // HD: >= 0 (GameCube != -1)
        return TRUE;
    }
    return FALSE;
}

/* 02443144 */
BOOL daPy_lk_c::getTactTopPos(cXyz* out) {

    if (mEquipItem != Link08::dItemNo_WIND_WAKER_e ||
        link08ItemBlock(this).mpEquipItemModel == nullptr) {
        return FALSE;
    }
    J3DModel* model = link08ItemBlock(this).mpEquipItemModel;
    PSMTXMultVec(link08BaseTRMtx(model), &l_tact_top, out);
    return TRUE;
}

/* 024431AC */
BOOL daPy_lk_c::getTactNormalWait() {

    if (mCurProc == Link08::daPyProc_TACT_WAIT_e && mProcVar6 == -1 &&
        mProcVar1 < 0) { // HD: < 0 (GameCube == -1)
        return TRUE;
    }
    return FALSE;
}

/* 024431E0 */
BOOL daPy_lk_c::checkTactPlayMelody() {

    if (mCurProc == Link08::daPyProc_TACT_PLAY_e && mProcVar5 == 0) {
        return TRUE;
    }
    return FALSE;
}

/* ---- d_a_player_particle.inc ---- */

/* 02443208 */
BOOL daPy_lk_c::setItemWaterEffect(fopAc_ac_c* actor, BOOL inWater, BOOL triggerOnExit) {
    /* static: no this */
    if (l_waterScaleGuard == 0) {
        l_waterScaleGuard = 1;
        l_waterScale.x = 0.5f;
        l_waterScale.z = 0.5f;
        l_waterScale.y = 0.5f;
    }
    if (l_waterSmallScaleGuard == 0) {
        l_waterSmallScaleGuard = 1;
        l_waterSmallScale.x = 0.13f;
        l_waterSmallScale.z = 0.13f;
        l_waterSmallScale.y = 0.13f;
    }
    if (l_waterDirectionGuard == 0) {
        l_waterDirection.x = 0.0f;
        l_waterDirection.z = 0.0f;
        l_waterDirectionGuard = 1;
        l_waterDirection.y = 1.0f;
    }
    gabi::Local<be<f32>> waterY;
    if (Link08_getWaterY(&actor->current.pos, waterY.get()) &&
        ((inWater && actor->current.pos.y > *waterY) ||
         (!inWater && !(*waterY < actor->current.pos.y)))) {
        /* the item entered or left the water */
        inWater ^= 1;
        if (!inWater && !triggerOnExit) {
            return inWater;
        }
        f32 posY = actor->current.pos.y;
        f32 deltaY = std::fabs(actor->old.pos.y - posY);
        f32 waterDistY = std::fabs(*waterY - posY);
        gabi::Local<cXyz> pos;
        if (deltaY < 1.0f) {
            pos->x = actor->current.pos.x;
            pos->y = posY;
            pos->z = actor->current.pos.z;
        } else {
            f32 t = waterDistY / deltaY;
            if (t > 1.0f) {
                t = 1.0f;
            }
            gabi::Local<cXyz> a;
            gabi::Local<cXyz> b;
            gabi::Local<cXyz> sum;
            cXyz_ml(&actor->old.pos, a, t);
            cXyz_ml(&actor->current.pos, b, 1.0f - t);
            cXyz_pl(a, sum, b);
            link08CopyVectorWords(pos.get(), sum.get());
        }
        /* dComIfGp_particle_setSingleRipple(ID_IT_JN_WP_HAMON01/03) */
        dPa_control_set(dComIfGp_getParticle(), 5, 0x3D, pos, nullptr, &l_waterScale, 0xFF,
                        gabi::at<dPa_levelEcallBack>(0x1047B2E4) /* single ripple callback */, -1,
                        nullptr, nullptr, nullptr);
        dPa_control_set(dComIfGp_getParticle(), 5, 0x3F, pos, nullptr, &l_waterScale, 0xFF,
                        gabi::at<dPa_levelEcallBack>(0x1047B2E4), -1, nullptr, nullptr, nullptr);
        gabi::Local<GXColor> amb;
        gabi::Local<GXColor> dif;
        Link08_getSeaColor(amb.get(), dif.get());
        Link08Emitter* emitter = reinterpret_cast<Link08Emitter*>(dPa_control_set(
            dComIfGp_getParticle(), 1, Link08::ID_AK_JN_ELEMENTSHIBUKI00, pos, nullptr,
            &l_waterSmallScale, 0xFF, nullptr, -1, amb.get(), nullptr, nullptr));
        if (emitter != 0) {
            emitter->mSpread = 0.0f;                    /* setSpread */
            emitter->mLifeTime = 20;                    /* setLifeTime */
            emitter->mDirectionalSpeed = 80.0f;         /* setDirectionalSpeed */
            emitter->mAwayFromAxisSpeed = 20.0f;        /* setAwayFromAxisSpeed */
            emitter->mMaxFrame = 1;                     /* setMaxFrame */
            emitter->mRate = 40.0f;                     /* setRate */
            emitter->mDirection.x = l_waterDirection.x; /* setDirection */
            emitter->mDirection.y = l_waterDirection.y;
            emitter->mDirection.z = l_waterDirection.z;
        }
    }
    return inWater;
}

/* ---- d_a_player_hook.inc ---- */

/* 024435FC */
BOOL daPy_lk_c::checkHookshotReturn() {

    Link08Hookshot* hookshot = reinterpret_cast<Link08Hookshot*>(mActorKeepEquip.mActor.get());
    if (mEquipItem != Link08::dItemNo_HOOKSHOT_e || hookshot == 0) {
        return FALSE;
    }
    /* checkPull() || checkReturn(): HD mode word at +0xB0 */
    u32 mode = hookshot->mMode;
    return mode == 3 || mode == 2;
}

/* ---- d_a_player_sword.inc / main ---- */

/* 0244363C */
BOOL daPy_lk_c::checkCutRollChange() {

    if (mCurProc == Link08::daPyProc_CUT_TURN_MOVE_e && mProcVar0 >= 0) {
        return TRUE;
    }
    return FALSE;
}

/* 02443664 */
BOOL daPy_lk_c::checkGameOverStart() {

    return mCurProc == Link08::daPyProc_DEMO_DEAD_e && m35A0 < 0.38f;
}

/* 02443694 */
int daPy_lk_c::getTactTimerCancel() {

    // HD: no Korok (grab actor) check; 1 above -100, else 2
    if (mCurProc == Link08::daPyProc_TACT_WAIT_e && !(m35AC > 0.0f)) {
        return m35AC > -100.0f ? 1 : 2;
    }
    return 0;
}

/* 024436D8 */
s32 daPy_lk_c::getTactMusic() {

    if (mCurProc == Link08::daPyProc_TACT_PLAY_e) {
        return mProcVar6;
    }
    if (mCurProc == Link08::daPyProc_TACT_WAIT_e) {
        return mProcVar7;
    }
    return -1;
}

/* 02443704 */
fopAc_ac_c* daPy_lk_c::getGrabMissActor() {

    if (mCurProc != Link08::daPyProc_GRAB_READY_e && mCurProc != Link08::daPyProc_GRAB_MISS_e) {
        return nullptr;
    }
    return mActorKeepRope.mActor;
}

/* 02443768 */
void daPy_lk_c::setTactZev(u32 tactZevPartnerId, int r30, u32 r31) {

    if (tactZevPartnerId != 0xFFFFFFFF) {
        Link08_startSystemSound(Link08::JA_SE_PRE_TAKT);
    }
    m34CC = (u8)r30;
    m3494 = r31;
    mTactZevPartnerId = tactZevPartnerId;
}

/* 024437D0 */
BOOL daPy_lk_c::getBokoFlamePos(cXyz* outPos) {

    if (mEquipItem == Link08::daPyItem_BOKO_e) {
        fopAc_ac_c* boko = fopAcM_SearchByID(mActorKeepEquip.mID);
        if (boko != nullptr && reinterpret_cast<Link08Boko*>(boko)->mFlameTimer != 0) {
            link08CopyVectorWords(outPos, &mSwordTopPos);
            return TRUE;
        }
    }
    return FALSE;
}

/* 02443880 */
void daPy_lk_c::voiceStart(u32 id) {

    u32 vowel = Link08_getLinkVoiceVowel(id);
    Link08_startLinkVoice(id, &eyePos, vowel, (s32)mReverb);
}

/* 024438D4 */
void daPy_lk_c::setOutPower(f32 power, s16 angle, int flag) {

    f32 cur = m3644;
    if (cur < 0.1f) {
        m3640 = angle;
        m3644 = power;
    } else {
        u16 old = (u16)m3640;
        f32 x = gabi::fmadds(cur, cM_ssin(old), gabi::fmuls_ppc(power, cM_ssin((u16)angle)));
        f32 z = gabi::fmadds(cur, cM_scos(old), gabi::fmuls_ppc(power, cM_scos((u16)angle)));
        m3640 = cM_atan2s(x, z);
        m3644 = std_sqrtf(gabi::fmadds(x, x, gabi::fmuls_ppc(z, z)));
    }
    if (flag != 0) {
        setNoResetFlg1(noResetFlg1() | 0x10000000); /* onNoResetFlg1(daPyFlg1_UNK10000000) */
    }
}

/* 024439D4 */
BOOL daPy_lk_c::setHookshotCarryOffset(u32 carryActorID, const cXyz* offset) {

    Link08Hookshot* hookshot = reinterpret_cast<Link08Hookshot*>(mActorKeepEquip.mActor.get());
    if (mEquipItem != Link08::dItemNo_HOOKSHOT_e || hookshot == 0) {
        return FALSE;
    }
    if (carryActorID != hookshot->mCarryActorID /* getCarryActorID() (HD) */) {
        return FALSE;
    }
    /* setCarryOffset(offset): HD at +0xD768 */
    link08CopyVectorWords(&hookshot->mCarryOffset, offset);
    return TRUE;
}

/* the debug copies of the player position and angles (HD release keeps them) */
static inline void lk_setDebugKeep(daPy_lk_c* self) {
    link08CopyVectorWords(&l_debug_keep_pos, &self->current.pos);
    Link08AngleWords& shape = link08AngleWords(self->shape_angle);
    Link08AngleWords& debugShape = link08AngleWords(l_debug_shape_angle);
    debugShape.x = shape.x;
    debugShape.y = shape.y;
    debugShape.z = shape.z;
    Link08AngleWords& current = link08AngleWords(self->current.angle);
    Link08AngleWords& debugCurrent = link08AngleWords(l_debug_current_angle);
    debugCurrent.x = current.x;
    debugCurrent.y = current.y;
    debugCurrent.z = current.z;
}
/* current.pos = old.pos = *pos (GHS integer copy, in the original's load/store order: pos may
 * alias) */
static inline void lk_setPos(daPy_lk_c* self, const cXyz* pos) {
    const Link08VectorWords& source = link08VectorWords(*pos);
    Link08VectorWords& current = link08VectorWords(self->current.pos);
    Link08VectorWords& old = link08VectorWords(self->old.pos);
    u32 x = source.x;
    current.x = x;
    u32 y = source.y;
    current.y = y;
    u32 z = source.z;
    old.x = x;
    current.z = z;
    old.y = y;
    old.z = z;
}

/* 02443A28 */
void daPy_lk_c::setPlayerPosAndAngle(cXyz* pos, s16 angle) {

    if (!dComIfGp_event_runCheck() && reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())
                                              ->play.mScopeType /* dComIfGp_getScopeType() */
                                          != 1) {
        return;
    }
    if (pos != nullptr) {
        lk_setPos(this, pos);
    }
    shape_angle.y = angle;
    current.angle.y = angle;
    m34DE = angle;
    lk_setDebugKeep(this);
}

/* 02443B14 */
void daPy_lk_c::setPlayerPosAndAngle(cXyz* pos, csXyz* angle) {

    if (!dComIfGp_event_runCheck()) {
        return;
    }
    if (pos != nullptr) {
        lk_setPos(this, pos);
    }
    if (angle != nullptr) {
        shape_angle.x = angle->x;
        shape_angle.y = angle->y;
        shape_angle.z = angle->z;
        current.angle.y = shape_angle.y;
        m34DE = shape_angle.y;
    }
    lk_setDebugKeep(this);
}

/* 02443C0C */
void daPy_lk_c::setPlayerPosAndAngle(Mtx34* mtx) {

    if (dComIfGp_event_runCheck()) {
        current.pos.x = mtx->m[0][3];
        current.pos.y = mtx->m[1][3];
        current.pos.z = mtx->m[2][3];
        old.pos.x = current.pos.x;
        old.pos.y = current.pos.y;
        old.pos.z = current.pos.z;
        Link08_matrixToRotation(mtx, &shape_angle);
        current.angle.y = shape_angle.y;
        m34DE = shape_angle.y;
        lk_setDebugKeep(this);
    }
}

/* 02443CE0 */
BOOL daPy_lk_c::setThrowDamage(cXyz* pos, s16 angle, f32 speed, f32 speedY, int damage) {

    /* virtual setPlayerPosAndAngle(cXyz*, s16) (vtable slot 0x114) */
    gabi::call_ptr(link08VirtualTable(this)->mSetPlayerPosAndAngle.get(), this, pos, (s32)angle);
    this->speed.y = speedY;
    mNormalSpeed = speed;
    Link08_setDamagePoint(this, (f32)(s32)(0u - (u32)damage));
    mNoResetFlg0 = mNoResetFlg0 | 0x1000000; /* onNoResetFlg0(daPyFlg0_UNK1000000) */
    return TRUE;
}

/* 02443DA0 */
void daPy_lk_c::changeTextureAnime(u16 btpIdx, u16 btkIdx, int r7) {

    if (!dComIfGp_event_runCheck()) {
        return;
    }
    if (r7 == -1) {
        /* m_tex_anm_heap / m_tex_scroll_heap: field_0x4 (index), field_0x6 */
        Link08AnmHeap& tah = *reinterpret_cast<Link08AnmHeap*>(m_tex_anm_heap);
        Link08AnmHeap& tsh = *reinterpret_cast<Link08AnmHeap*>(m_tex_scroll_heap);
        if (tah.field_0x4 != btpIdx || tah.field_0x6 != 0xFFFE) {
            tah.field_0x4 = btpIdx;
            tah.field_0x6 = 0xFFFE;
            u32 res = Link08_loadTextureAnimeResource(this, (u32)btpIdx, 0);
            Link08_setTextureAnimeResource(this, res, 0);
        }
        if (tsh.field_0x4 != btkIdx || tsh.field_0x6 != 0xFFFE) {
            tsh.field_0x4 = btkIdx;
            tsh.field_0x6 = 0xFFFE;
            u32 res = Link08_loadTextureScrollResource(this, (u32)btkIdx, 0);
            Link08_setTextureScrollResource(this, res, 0);
        }
    } else {
        Link08_setDemoTextureAnime(this, (u32)btpIdx, (u32)btkIdx, 0, (u32)(u16)r7);
    }
}

// HD: compiler-generated and template code of this translation unit.

/* 02443EC4 */
static void __sinit_d_a_player_main_cpp() {

    /* the per-TU header statics */
    l_headerZeroState.mWords[2] = 0;
    l_headerZeroState.mWords[0] = 0;
    l_headerZeroState.mWords[3] = 0;
    l_headerZeroState.mWords[1] = 0;
    __register_global_object(gabi::ea(&l_headerDestructorRecord0));
    l_headerAngleLimits.mMinimum = -3.1415927f;
    l_headerAngleLimits.mMaximum = 3.1415927f;
    Link08_initializeHeaderByte(gabi::ea(&l_headerByte));
    __register_global_object(gabi::ea(&l_headerDestructorRecord1));
    Link08_initializeHeaderFlag(gabi::ea(&l_headerFlag));
    __register_global_object(gabi::ea(&l_headerDestructorRecord2));
    // HD: material names use SafeString objects.
    l_sleeveMaterialName.mVtable = gc_tu.safeString;
    l_sleeveMaterialName.mStringTop = gabi::guest_addr("sleeve");
    l_mouthMaterialName.mStringTop = gabi::guest_addr("mouth");
    l_mouthMaterialName.mVtable = gc_tu.safeString;
    l_hatMaterialName.mVtable = gc_tu.safeString;
    l_hairMaterialName.mVtable = gc_tu.safeString;
    l_hatMaterialName.mStringTop = gabi::guest_addr("ear_3_");
    l_hairMaterialName.mStringTop = gabi::guest_addr("ear_2_");
    l_movementDefaults.mLadderLineOffset = 50.0f;
    l_earMaterialName.mVtable = gc_tu.safeString;
    l_hammer_splash_particle_scale.x = 0.67f;
    l_movementDefaults.mSwimWallHeight = -5.0f;
    l_crawl_back_up_offset.y = 50.0f;
    l_crawl_stand_up_offset.y = 50.0f;
    l_movementDefaults.mCrawlWallRadius = 9.99f;
    l_hammer_splash_particle_scale.y = 0.67f;
    l_hammer_splash_particle_scale.z = 0.67f;
    l_crawl_start_front_offset.y = 10.0f;
    l_heel_pos.y = 3.25f;
    l_head_center_offset.x = 11.25f;
    l_faceMaterialName.mVtable = gc_tu.safeString;
    l_faceMaterialName.mStringTop = gabi::guest_addr("face");
    l_earMaterialName.mStringTop = gabi::guest_addr("ear");
    l_crawl_front_offset.y = 10.0f;
    l_eye_offset.x = 11.25f;
    l_eye_offset.y = 18.75f;
    l_legsMaterialName.mVtable = gc_tu.safeString;
    l_crawl_back_offset.y = 10.0f;
    l_crawl_front_up_offset.y = 50.0f;
    l_legsMaterialName.mStringTop = gabi::guest_addr("ear_4_");
    l_toe_pos.y = 3.25f;
    l_crawl_top_offset.y = 10.0f;
    l_crawl_top_up_offset.y = 50.0f;
    l_crawl_side_offset.x = 50.0f;
    l_crawl_side_offset.y = 50.0f;
    l_crawl_lside_offset.x = 50.0f;
    l_headerDistances.mLargeDistance0 = 50000.0f;
    l_crawl_min_side_offset.x = 30.0f;
    l_fan_wind_cps_src.mRadius = 70.0f;
    l_headerDistances.mSmallDistance0 = 10000.0f;
    l_at_cyl_src.mHeight = 75.0f;
    l_leftEyeMaterialName.mVtable = gc_tu.safeString;
    l_leftEyeMaterialName.mStringTop = gabi::guest_addr("eyeL");
    l_rightEyeMaterialName.mVtable = gc_tu.safeString;
    l_rightEyeMaterialName.mStringTop = gabi::guest_addr("eyeR");
    l_headerDistances.mLargeDistance1 = 50000.0f;
    l_crawl_rside_front_offset.x = -50.0f;
    l_crawl_rside_offset.x = -50.0f;
    l_headerDistances.mSmallDistance1 = 10000.0f;
    l_cyl_src.mRadius = 30.0f;
    l_wind_cyl_src.mRadius = 30.0f;
    l_wind_cyl_src.mHeight = 125.0f;
    l_cyl_src.mHeight = 125.0f;
    l_crawl_lside_front_offset.x = 50.0f;
    l_staticGroundCheck.mVtable = l_staticGroundCheckVtable;
    dBgS_GndChk& check = l_staticGroundCheck.mGroundCheck;
    Link08_constructGroundCheck(gabi::ea(&check));
    check.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(&check));
    check.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(&check));
    check.mGrp = 1;
    check.mbCamThrough = 0;
    check.mbLinkThrough = 0;
    check.mbArrowThrough = 0;
    check.mbBombThrough = 0;
    check.mbBoomerangThrough = 0;
    check.mbRopeThrough = 0;
    check.cBgS_Chk::__vtbl = gc_tu.gndChk.v10;
    check.cBgS_PolyInfo::__vtbl = gc_tu.gndChk.v20;
    check.dBgS_GrpPassChk::__vtbl = gc_tu.gndChk.v4C;
    check.dBgS_PolyPassChk::__vtbl = gc_tu.gndChk.v40;
    check.mbObjThrough = 1;
    __register_global_object(gabi::ea(&l_headerDestructorRecord3));
    l_movementDefaults.mSwimWallRadius = 67.5f;
    l_movementDefaults.mRopeLengthOffset = 95.0f;
}

/* ---- per-TU copies of inline constructors, destructors and accessors (after __sinit) ----
 * GHS emits one copy per translation unit; names are by content where the matcher has none. */
/* 0244426C */
static void Link08_SafeString_dtor(u32 self, s32 flags) {

    if (self != 0 && (flags & 1))
        Link08_deallocate(self);
}

/* 02444280 */
static u32 Link08_headerObject_ctor(u32 self) {

    if (self == 0) {
        self = Link08_allocate(0x24);
        if (self == 0)
            return 0;
    }
    Link08HeaderObject* object = gabi::at<Link08HeaderObject>(self);
    object->mInitialized = 0;
    object->mResource = 0;
    return self;
}

/* 024442C0 */
static u32 Link08_animationState_ctor(u32 self) {

    if (self == 0) {
        self = Link08_allocate(0x10);
        if (self == 0)
            return 0;
    }
    Link08AnimationState* state = gabi::at<Link08AnimationState>(self);
    state->mContext = 0;
    state->mFrame = 0.0f;
    state->mResource = 0;
    state->mEvaluator = 0;
    return self;
}

/* 02444310 */
static u32 J3DFrameCtrl_ct_l(u32 self) {

    if (self == 0) {
        self = Link08_allocate(0x10);
        if (self == 0)
            return 0;
    }
    Link08_initializeFrameControl(self, 0);
    return self;
}

/* the HD vertex buffer (constructor  / , destructor  & co.) */
static inline u32 link08VertexBufferCtor(u32 self) {
    if (self == 0) {
        self = Link08_allocate(0x254);
        if (self == 0)
            return 0;
    }
    Link08_constructVertexBufferObject(self + 4);
    Link08_constructVertexAttributeBinding(self + 0x158);
    Link08VertexBuffer* buffer = gabi::at<Link08VertexBuffer>(self);
    buffer->mpAllocation = 0;
    buffer->mVertexCount = 0;
    return self;
}
/* 0244435C */
static u32 Link08_sightVertexBuffer_ctor(u32 self) {
    return link08VertexBufferCtor(self);
}

/* 024443B8 */
static u32 Link08_materialElement_ctor(u32 self) {

    if (self == 0) {
        self = Link08_allocate(0x10);
    }
    return self;
}

/* 024443E4 */
static u32 daPy_footEffect_c_ct(u32 self) {

    if (self == 0) {
        self = Link08_allocate(0x4C);
        if (self == 0)
            return 0;
    }
    Link08_constructSmokeCallback(self, 1);
    Link08_constructFollowCallback(self + 0x20, 0, 0);
    return self;
}

static inline u32 lk_ctVtbl(u32 self, u32 size, u32 vtbl) {
    if (self == 0) {
        self = Link08_allocate(size);
        if (self == 0)
            return 0;
    }
    gabi::at<Link08VtableObject>(self)->mVtable = vtbl;
    return self;
}
/* 02444440 */
static u32 daPy_swimTailEcallBack_c__ct(u32 self) {
    return lk_ctVtbl(self, 0x28, 0x10037C30);
}
/* 02444480 */
static u32 daPy_dmEcallBack_c__ct(u32 self) {
    return lk_ctVtbl(self, 0xC, 0x10037C70);
}
/* 024444C0 */
static u32 daPy_mtxPosFollowEcallBack_c__ct(u32 self) {
    return lk_ctVtbl(self, 0x10, 0x10037CB0);
}

/* 02444500 */
static u32 dBgS_LinkGndChk__ct(u32 self) {

    if (self == 0) {
        self = Link08_allocate(0x118);
        if (self == 0)
            return 0;
    }
    dBgS_GndChk* check = gabi::at<dBgS_GndChk>(self + 0x34);
    Link08_constructGroundCheck(self + 0x34);
    check->mbCamThrough = 0;
    check->mbLinkThrough = 1;
    check->mGrp = 1;
    check->mbObjThrough = 0;
    check->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(check));
    check->dBgS_PolyPassChk::__vtbl = l_linkGroundCheckVtables.v40;
    check->cBgS_PolyInfo::__vtbl = l_linkGroundCheckVtables.v20;
    check->mbBoomerangThrough = 0;
    check->cBgS_Chk::__vtbl = l_linkGroundCheckVtables.v10;
    check->mbBombThrough = 0;
    check->dBgS_GrpPassChk::__vtbl = l_linkGroundCheckVtables.v4C;
    check->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(check));
    check->mbRopeThrough = 0;
    check->mbArrowThrough = 0;
    return self;
}

/* 024445B0 */
static u32 dCcD_Cps_ct_l(u32 self) {

    if (self == 0) {
        self = Link08_allocate(0x138);
        if (self == 0)
            return 0;
    }
    Link08_constructCollisionInfo(self);
    Link08CapsuleCollision* capsule = gabi::at<Link08CapsuleCollision>(self);
    capsule->mShapeAttr.mVtable = l_capsuleVtables.mShapeBase;
    capsule->mShapeAttr.mAabbVtable = l_capsuleVtables.mAabb;
    Link08_constructCapsuleGeometry(self + 0x118);
    capsule->__vtbl_hitinf = l_capsuleVtables.mHitInfo;
    capsule->mCps.mVtable = l_capsuleVtables.mCapsule;
    capsule->mShapeAttr.mVtable = l_capsuleVtables.mShape;
    return self;
}

/* 0244463C */
static u32 Link08_swordVertexBuffer_ctor(u32 self) {
    return link08VertexBufferCtor(self);
}

/* 02444698 */
static void Link08_BufferedSafeString_assureTermination(u32 self) {

    Link08BufferedString* string = gabi::at<Link08BufferedString>(self);
    // HD: preserve the wrapped guest address when the capacity is zero.
    *gabi::at<be<u8>>(string->mpBuffer + string->mCapacity - 1u) = 0;
}

/* 024446B0 */
static void Link08_materialElement_dtor(u32 self, s32 flags) {

    if (self != 0 && (flags & 1))
        Link08_deallocate(self);
}

/* 024446C4 */
static s32 Link08_animationInfo_getAttributeMode(u32 self) {

    return (gabi::at<Link08AnimationInfoOwner>(self)->mpInfo->mFlags & 4) ? 2 : 0;
}

/* 024446E0 */
static s16 Link08_animationInfo_getEndFrame(u32 self) {

    return gabi::at<Link08AnimationInfoOwner>(self)->mpInfo->mEndFrame;
}

static inline void lk_dtorBase(u32 self, s32 flags, void (*destroyBase)(u32, s32), u32 baseOff,
                               s32 baseFlags) {
    if (self != 0) {
        destroyBase(self + baseOff, baseFlags);
        if (flags & 1)
            Link08_deallocate(self);
    }
}
/* 024446EC */
static void Link08_renderPacket_dtor(u32 self, s32 flags) {

    lk_dtorBase(self, flags, Link08_destroyRenderPacketBase, 0, 0);
}
/* 02444740 */
static void Link08_renderPacketCopy_dtor(u32 self, s32 flags) {

    lk_dtorBase(self, flags, Link08_destroyRenderPacketBase, 0, 0);
}

/* 02444794 */
static void Link08_planeGeometry_dtor(u32 self, s32 flags) {

    lk_dtorBase(self, flags, Link08_destroyPlaneGeometry, 0x14, 2);
}

/* the HD vertex buffer's deleting destructor (copies , , ) */
static inline void link08VertexBufferDtor(u32 self, s32 flags) {
    if (self != 0) {
        Link08_destroyVertexAttributeBinding(self + 0x158, 2);
        Link08_destroyVertexBufferObject(self + 4, 2);
        if (flags & 1)
            Link08_deallocate(self);
    }
}
/* 024447E8 */
static void Link08_swordVertexBuffer_dtor(u32 self, s32 flags) {

    link08VertexBufferDtor(self, flags);
}

/* 02444848 */
static void Link08_callback_setupDefault() {}
/* 0244484C */
static void Link08_callback_executeDefault() {}
/* 02444850 */
static void Link08_callback_drawDefault() {}
/* 02444854 */
static void Link08_callback_setupCopy() {}
/* 02444858 */
static void Link08_callback_executeCopy() {}
/* 0244485C */
static void Link08_callback_drawCopy() {}

/* 02444860 */
static void Link08_callback_setState(u32 self, u32 v) {

    gabi::at<Link08FanSwingCallback>(self)->mState = v;
}

static inline void lk_dtorGndChk(u32 self, s32 flags, u32 off) {
    if (self != 0) {
        dBgS_GndChk* check = gabi::at<dBgS_GndChk>(self + off);
        check->cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0];
        check->dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
        check->dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
        Link08_destroyBackgroundCheck(self + off, 0);
        if (flags & 1)
            Link08_deallocate(self);
    }
}
/* 02444868 */
static void dBgS_LinkGndChk__dt(u32 self, s32 flags) {
    lk_dtorGndChk(self, flags, 0x34);
}

/* 024448E0 */
static void daPy_swimTailEcallBack_c__dt(u32 self, s32 flags) {

    if (self != 0 && (flags & 1))
        Link08_deallocate(self);
}

/* 024448F4 */
static void daPy_waterDropPcallBack_c__dt(u32 self, s32 flags) {
    lk_dtorGndChk(self, flags, 8);
}

/* 0244496C */
static void daPy_fanSwingEcallBack_c_setup(u32 self, u32 emitter) {

    Link08Emitter* particle = gabi::at<Link08Emitter>(emitter);
    particle->mFlags = particle->mFlags | 0x40; /* becomeImmortalEmitter */
    Link08FanSwingCallback* callback = gabi::at<Link08FanSwingCallback>(self);
    callback->mState = 0;
    callback->mpEmitter = emitter;
}

/* 02444988 */
static void Link08_blurVertexBuffer_dtor(u32 self, s32 flags) {

    link08VertexBufferDtor(self, flags);
}

/* 02444CC4 */
static void Link08_sightVertexBuffer_dtor(u32 self, s32 flags) {

    link08VertexBufferDtor(self, flags);
}

/* the HD vertex buffer's destructor body as inlined into : release its resource
 * (the heap holding *(+0x250) frees it:  finds the heap, virtual free slot 0x3C) */
static inline void link08ReleaseVertexBuffer(u32 obj) {
    Link08_releaseVertexAttributeBinding(obj + 0x158);
    Link08VertexBuffer* animation = gabi::at<Link08VertexBuffer>(obj);
    u32 res = animation->mpAllocation;
    animation->mpBuffer = 0;
    if (res != 0) {
        u32 heap = Link08_findHeapFromRoot(l_rootHeap, res);
        gabi::call_ptr(gabi::at<Link08Heap>(heap)->mpVirtualTable->mFree.get(), heap,
                       animation->mpAllocation.get());
        animation->mVertexCount = 0;
        animation->mpAllocation = 0;
    }
}

/* 024449E8 */
static void daPy_swBlur_c__dt(u32 self, s32 flags) {

    if (self == 0) {
        return;
    }
    Link08SwordBlur* animation = gabi::at<Link08SwordBlur>(self);
    // HD: resource-array addresses retain 32-bit wrapping arithmetic.
    u32 arr = self + 0x660;
    animation->mVtable = l_swordBlurVtable;
    for (u32 i = 0; i < 0x3C; i++) {
        u32 o = arr + i * 0x4A8;
        link08ReleaseVertexBuffer(o);
        link08ReleaseVertexBuffer(o + 0x254);
    }
    animation->mCurrentMaterial = 0;
    u32 b = self + 0x11DD8;
    for (s32 i = 0; i < (s32)animation->mEntries.mCount.get(); i++) {
        u32 n = animation->mEntries.mCount.get();
        u32 e = animation->mEntries.mpEntries.get();
        if ((u32)i < n)
            e += i * 0x23C;
        for (u32 k = 0; k < 2; k++)
            Link08_releaseShapeBinding(e + 0x10 + k * 0x1C);
    }
    for (u32 k = 0; k < 2; k++)
        Link08_releaseShapeBinding(b + 0x1C + k * 0x1C);
    for (u32 k = 0; k < 2; k++)
        Link08_releaseShapeBinding(b + 0xC4 + k * 0x1C);
    Link08_destroyShapeBindingCollection(self + 0x12214, 2);
    Link08_destroyDisplayList(self + 0x121F4, 2);
    Link08_destroyShaderBinding(b + 0xB4, 0);
    Link08_destroyShaderBinding(b + 0xC, 0);
    Link08_destroyShapeEntryArray(b, 2);
    if (arr != 0) {
        for (u32 i = 0; i < 0x3C; i++) {
            u32 o = arr + i * 0x4A8;
            link08ReleaseVertexBuffer(o);
            link08ReleaseVertexBuffer(o + 0x254);
        }
        animation->mCurrentMaterial = 0;
        Link08_destroyArray(
            arr, 0x78, 0x254,
            gabi::guest_fn(reinterpret_cast<const void*>(Link08_blurVertexBuffer_dtor)), 0, 0);
    }
    Link08_destroyRenderPacketBase(self, 0);
    if (flags & 1)
        Link08_deallocate(self);
}

/* 02444D24 */
static void daPy_dmEcallBack_c__dt(u32 self, s32 flags) {

    if (self != 0 && (flags & 1))
        Link08_deallocate(self);
}
/* 02444D38 */
static void daPy_fanSwingEcallBack_c__dt(u32 self, s32 flags) {

    if (self != 0 && (flags & 1))
        Link08_deallocate(self);
}
/* 02444D4C */
static void daPy_mtxPosFollowEcallBack_c__dt(u32 self, s32 flags) {

    if (self != 0 && (flags & 1))
        Link08_deallocate(self);
}

/* inlined into daPy_lk_c::getGroundH_l */
inline f32 Link08Player::getGroundY() {
    return mAcch.m_ground_h;
}

/* ---- daPy_lk_c inline accessors (per-TU copies) ---- */

/* 02444D60 */
f32 daPy_lk_c::getGroundH_l() {
    return reinterpret_cast<Link08Player*>(this)->getGroundY();
}

/* inlined into daPy_lk_c::getAnmMtx8_l */
inline Mtx34* Link08Player::getLeftHandMatrix() {

    return lk_getAnmMtx(mpCLModel, Link08::CL_JNT_CL_LHANDA_e);
}

/* 02444D68 */
Mtx34* daPy_lk_c::getAnmMtx8_l() {
    return reinterpret_cast<Link08Player*>(this)->getLeftHandMatrix();
}
/* inlined into daPy_lk_c::getAnmMtx12_l */
inline Mtx34* Link08Player::getRightHandMatrix() {

    return lk_getAnmMtx(mpCLModel, Link08::CL_JNT_CL_RHANDA_e);
}

/* 02444D88 */
Mtx34* daPy_lk_c::getAnmMtx12_l() {
    return reinterpret_cast<Link08Player*>(this)->getRightHandMatrix();
}

/* inlined into daPy_lk_c::checkModeFlg_10452822_l */
inline u32 Link08Player::checkPlayerFly() {

    return mModeFlg & (Link08::ModeFlg_MIDAIR | Link08::ModeFlg_HANG | Link08::ModeFlg_ROPE |
                       Link08::ModeFlg_IN_SHIP | Link08::ModeFlg_CLIMB | Link08::ModeFlg_SWIM |
                       Link08::ModeFlg_LADDER | Link08::ModeFlg_CAUGHT);
}

/* 02444DA8 */
u32 daPy_lk_c::checkModeFlg_10452822_l() {
    return reinterpret_cast<Link08Player*>(this)->checkPlayerFly();
}

/* inlined into daPy_lk_c::check_02444DBC_l */
inline BOOL Link08Player::checkMidairOrSwimWaitHD() {

    u32 f = mModeFlg;
    if (f & Link08::ModeFlg_MIDAIR)
        return TRUE;
    if ((f & Link08::ModeFlg_SWIM) && mCurProc == Link08::daPyProc_SWIM_WAIT_e)
        return TRUE;
    return FALSE;
}

/* 02444DBC */
BOOL daPy_lk_c::check_02444DBC_l() {
    return reinterpret_cast<Link08Player*>(this)->checkMidairOrSwimWaitHD();
}

/* inlined into daPy_lk_c::checkProc1E_l */
inline BOOL Link08Player::checkFrontRoll() {
    return mCurProc == Link08::daPyProc_FRONT_ROLL_e;
}

/* 02444DEC */
BOOL daPy_lk_c::checkProc1E_l() {
    return reinterpret_cast<Link08Player*>(this)->checkFrontRoll();
}
/* inlined into daPy_lk_c::checkProcA5_l */
inline BOOL Link08Player::checkBottleSwing() {
    return mCurProc == Link08::daPyProc_BOTTLE_SWING_e;
}

/* 02444E00 */
BOOL daPy_lk_c::checkProcA5_l() {
    return reinterpret_cast<Link08Player*>(this)->checkBottleSwing();
}
/* inlined into daPy_lk_c::checkCutTurnMoveProc_l */
inline BOOL Link08Player::checkCutCharge() {

    return mCurProc == Link08::daPyProc_CUT_TURN_MOVE_e; /* daPyProc_CUT_TURN_MOVE_e */
}

/* 02444E14 */
BOOL daPy_lk_c::checkCutTurnMoveProc_l() {
    return reinterpret_cast<Link08Player*>(this)->checkCutCharge();
}

/* inlined into daPy_lk_c::getBaseAnimeRate_l */
inline f32 Link08Player::getBaseAnimeFrameRate() {

    return mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getRate();
}

/* 02444E28 */
f32 daPy_lk_c::getBaseAnimeRate_l() {
    return reinterpret_cast<Link08Player*>(this)->getBaseAnimeFrameRate();
}
/* inlined into daPy_lk_c::getBaseAnimeFrame_l */
inline f32 Link08Player::getBaseAnimeFrame() {

    return mFrameCtrlUnder[Link08::UNDER_MOVE0_e].getFrame();
}

/* 02444E30 */
f32 daPy_lk_c::getBaseAnimeFrame_l() {
    return reinterpret_cast<Link08Player*>(this)->getBaseAnimeFrame();
}

/* inlined into daPy_lk_c::getEquipActorID_l */
inline u32 Link08Player::getItemID() {
    return mActorKeepEquip.mID;
}

/* 02444E38 */
u32 daPy_lk_c::getEquipActorID_l() {
    return reinterpret_cast<Link08Player*>(this)->getItemID();
}
/* inlined into daPy_lk_c::getThrowActorID_l */
inline u32 Link08Player::getThrowBoomerangID() {
    return mActorKeepThrow.mID;
}

/* 02444E40 */
u32 daPy_lk_c::getThrowActorID_l() {
    return reinterpret_cast<Link08Player*>(this)->getThrowBoomerangID();
}
/* inlined into daPy_lk_c::getGrabActorID_l */
inline u32 Link08Player::getGrabActorID() {
    return mActorKeepGrab.mID;
}

/* 02444E48 */
u32 daPy_lk_c::getGrabActorID_l() {
    return reinterpret_cast<Link08Player*>(this)->getGrabActorID();
}

/* inlined into daPy_lk_c::checkGrabBarrel_l */
inline BOOL Link08Player::checkGrabBarrel() {
    return Link08_checkGrabBarrelSearch(this, 1);
}

/* 02444E50 */
BOOL daPy_lk_c::checkGrabBarrel_l() {
    return reinterpret_cast<Link08Player*>(this)->checkGrabBarrel();
}

/* 02444E58 */
BOOL daPy_lk_c::checkPlayerNoDraw() {

    s32 idx = mCameraInfoIdx;
    return dComIfGp_checkCameraAttentionStatus(idx,
                                               2) /* dComIfGp_checkCameraAttentionStatus(idx, 2) */
           || (mNoResetFlg0 & 0x08000000);
}

/* inlined into daPy_lk_c::checkNoEquipActor_l */
inline BOOL Link08Player::checkRopeTag() {
    return mActorKeepEquip.mActor == nullptr;
}

/* 02444EC4 */
BOOL daPy_lk_c::checkNoEquipActor_l() {
    return reinterpret_cast<Link08Player*>(this)->checkRopeTag();
}

/* inlined into daPy_lk_c::checkUpperAnimeE4_l */
inline BOOL Link08Player::checkRopeReadyAnime() {

    return link08UpperHeaps(this)[Link08::UPPER_MOVE2_e].mIdx == 0xE4;
}

/* 02444ED4 */
BOOL daPy_lk_c::checkUpperAnimeE4_l() {
    return reinterpret_cast<Link08Player*>(this)->checkRopeReadyAnime();
}

/* inlined into daPy_lk_c::getModelJointMtx_l */
inline Mtx34* Link08Player::getModelJointMtx(s32 jnt) {
    return lk_getAnmMtx(mpCLModel, jnt);
}

/* 02444EE8 */
Mtx34* daPy_lk_c::getModelJointMtx_l(s32 jnt) {
    return reinterpret_cast<Link08Player*>(this)->getModelJointMtx(jnt);
}

/* inlined into daPy_lk_c::setFlg8_02444F0C_l */
inline void Link08Player::onFrollCrashFlg(u32 v) {

    m3620 = v;
    mNoResetFlg0 = mNoResetFlg0 | 8;
}

/* 02444F0C */
void daPy_lk_c::setFlg8_02444F0C_l(u32 v) {
    reinterpret_cast<Link08Player*>(this)->onFrollCrashFlg(v);
}

/* 02444F20 */
BOOL daPy_lk_c::checkComboCutTurn() {

    return mCurProc == Link08::daPyProc_CUT_TURN_e && mProcVar6 != 0;
}

/* ---- leftover functions of the translation unit ---- */

/* 02444F48 */
static void lk_SafeString_assureTermination(void* p) {}

/* 02444F4C */
static void Link08_FixedSafeString_dtor(void* p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02444F60 */
static void Link08_FixedSafeStringCopy_dtor(void* p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
ext 0200ECD4 f64 Link08_addCalcDouble(be<f32>*, f32, f32, f32, f32)
ext 024F8044 u32 Link08_getCameraBody()
ext 0253E860 void Link08_endEventCamera(u32, u32)
ext 0200ECD4 f32 Link08_addCalc(be<f32>*, f32, f32, f32, f32)
ext 028F4384 f64 Link08_sqrt(f64)
ext 028E8DD0 f64 Link08_squareMagnitude(cXyz*)
ext 020195B0 s16 Link08_atan2s(f64, f64)
ext 025D5928 fopAc_ac_c* Link08_fastCreate(u32, u32, cXyz*, s32, csXyz*, void*, s32, s32, s32)
ext 027DF40C void Link08_updateAnimationEvaluator(u32)
ext 025A5F88 void Link08_endSmokeCallback(void*)
ext 020084C8 u32 Link08_getTrianglePlane(dBgS*, u32, u32)
ext 025E1988 void Link08_startSystemSound(u32)
ext 02407070 void Link08_initSwordBlur(u32, Mtx34*, s32, f64, s32)
ext 023DC63C void Link08_clearActorKeep(daPy_actorKeep_l*)
ext 02540310 BOOL Link08_compulsoryEvent(u32, fopAc_ac_c*, s32, u32)
ext 025E1F08 f64 Link08_getTactBeatFrames()
ext 025D9F70 BOOL Link08_getWaterY(cXyz*, be<f32>*)
ext 025602F0 void Link08_getSeaColor(GXColor*, GXColor*)
ext 025E1CB4 u32 Link08_getLinkVoiceVowel(u32)
ext 025E1C90 void Link08_startLinkVoice(u32, cXyz*, u32, s32)
ext 025F232C void Link08_matrixToRotation(Mtx34*, csXyz*)
ext 028ED6F8 void Link08_initializeHeaderByte(u32)
ext 028EAB2C void Link08_initializeHeaderFlag(u32)
ext 02008E0C void Link08_constructGroundCheck(u32)
ext 027F2BC0 void Link08_initializeFrameControl(u32, s32)
ext 027B5BD8 void Link08_constructVertexBufferObject(u32)
ext 027BF734 void Link08_constructVertexAttributeBinding(u32)
ext 025A5B18 void Link08_constructSmokeCallback(u32, s32)
ext 025A5894 void Link08_constructFollowCallback(u32, s32, s32)
ext 02515FB8 void Link08_constructCollisionInfo(u32)
ext 02018150 void Link08_constructCapsuleGeometry(u32)
ext 027BF880 void Link08_destroyVertexAttributeBinding(u32, s32)
ext 027B5CBC void Link08_destroyVertexBufferObject(u32, s32)
ext 02008DAC void Link08_destroyBackgroundCheck(u32, s32)
ext 027BF7E8 void Link08_releaseVertexAttributeBinding(u32)
ext 02755FEC u32 Link08_findHeapFromRoot(u32, u32)
ext 027BEBEC void Link08_releaseShapeBinding(u32)
ext 027BE2B0 void Link08_destroyShapeBindingCollection(u32, s32)
ext 027B54A0 void Link08_destroyDisplayList(u32, s32)
ext 027FB528 void Link08_destroyShaderBinding(u32, s32)
ext 027FD764 void Link08_destroyShapeEntryArray(u32, s32)
ext 028F0164 void Link08_destroyArray(u32, u32, u32, u32, s32, s32)
ext 027F13DC void Link08_destroyRenderPacketBase(u32, s32)
ext 02018034 void Link08_destroyPlaneGeometry(u32, s32)
ext 023DCF08 BOOL Link08_checkGrabBarrelSearch(daPy_lk_c*, s32)
ext 0273AD10 u32 Link08_allocate(u32)
ext 0273AF40 void Link08_deallocate(u32)
ext 025B6DA0 void Link08_setBaitItemEmpty(dSv_player_bag_item_c*, u32)
ext 023F14E0 BOOL Link08_checkNextMode(daPy_lk_c*, s32)
ext 0243EF08 BOOL Link08_procWeaponFrontSwing_init(daPy_lk_c*)
ext 02419C70 void Link08_setShapeAngleToAtnActor(daPy_lk_c*)
ext 023DFDD8 void Link08_commonProcInit(daPy_lk_c*, u32)
ext 023E0A04 void Link08_setSingleMoveAnime(daPy_lk_c*, u32, f32, f32, s32, f32)
ext 023ED2CC void Link08_setEnemyWeaponAtParam(daPy_lk_c*, s32)
ext 0243829C BOOL Link08_changeCutReverseProc(daPy_lk_c*, u32)
ext 024072F4 void Link08_seStartSwordCut(daPy_lk_c*, u32)
ext 023DC7AC void Link08_deleteEquipItem(daPy_lk_c*, s32)
ext 023E2DC4 void Link08_setShipRidePosUseItem(daPy_lk_c*)
ext 023F2048 void Link08_endDemoMode(daPy_lk_c*)
ext 023F51D0 void Link08_setDamagePoint(daPy_lk_c*, f32)
ext 023DC38C u32 Link08_loadTextureAnimeResource(daPy_lk_c*, u32, s32)
ext 023DC110 void Link08_setTextureAnimeResource(daPy_lk_c*, u32, s32)
ext 023DC4DC u32 Link08_loadTextureScrollResource(daPy_lk_c*, u32, s32)
ext 023DC43C void Link08_setTextureScrollResource(daPy_lk_c*, u32, s32)
ext 0241FCB4 void Link08_setDemoTextureAnime(daPy_lk_c*, u32, u32, s32, u32)
ext 023EC370 void Link08_setSpecialBattle(daPy_lk_c*, s32)
ext 023E5400 BOOL Link08_changeSpecialBattle(daPy_lk_c*)
ext 023DD768 void Link08_setTextureAnime(daPy_lk_c*, u32, s32)
ext 0241C898 BOOL Link08_changeLandProc(daPy_lk_c*, f32)
ext 0241D6CC void Link08_setFallVoice(daPy_lk_c*)
ext 023D4538 void Link08_mtxFollow_end(void*)
ext 023D457C JPABaseEmitter* Link08_mtxFollow_makeEmitter(void*, u32, Mtx34*, cXyz*, cXyz*)
ext 023E4E2C void Link08_endFlameDamageEmitter(daPy_lk_c*)
ext 0241B904 BOOL Link08_procFrontRollCrash_init(daPy_lk_c*)
ext 023E2FF4 void Link08_procWait_init(daPy_lk_c*)
ext 023E4CD0 BOOL Link08_changeSlideProc(daPy_lk_c*)
ext 023E3358 s32 Link08_getDirectionFromAngle(daPy_lk_c*, s32)
ext 02416230 void Link08_setNormalSpeedF(daPy_lk_c*, f32, f32, f32, f32)
ext 023E0660 void Link08_initSeAnime(daPy_lk_c*)
ext 023E6380 void Link08_procCutTurn_init(daPy_lk_c*, s32)
ext 02407178 f64 Link08_getBlurTopRate(daPy_lk_c*)
ext 02407270 s32 Link08_getSwordBlurColor(daPy_lk_c*)
ext 023ED380 void Link08_setJumpCutAtParam(daPy_lk_c*)
ext 0243BFC8 void Link08_setHammerQuake(daPy_lk_c*, u32, u32, s32)
ext 023DBC24 BOOL Link08_checkHeavyStateOn(daPy_lk_c*)
ext 023ED2B0 void Link08_setAtCpsSpl(daPy_lk_c*, s32)
ext 023DF9C0 void Link08_resetFootEffect(daPy_lk_c*)
ext 023D69F8 BOOL Link08_checkBoomerangAnime(daPy_lk_c*)
data l_rollTranslationGuard 1046D270
data l_rollScaleGuard 1046D274
data l_rollTranslation 1046D278
data l_rollScale 1046D284
data l_waterScaleGuard 1046D290
data l_waterSmallScaleGuard 1046D294
data l_waterDirectionGuard 1046D298
data l_waterScale 1046CD84
data l_waterSmallScale 1046CD90
data l_waterDirection 1046CD9C
data l_debug_keep_pos 1046CD48
data l_debug_shape_angle 1046CD10
data l_debug_current_angle 1046CD08
data l_heel_pos 101CEB90
data l_toe_pos 101CEB9C
data l_eye_offset 101CEBA8
data l_head_center_offset 101CEBB4
data l_crawl_start_front_offset 101CEBC0
data l_crawl_front_offset 101CEBCC
data l_crawl_back_offset 101CEBD8
data l_crawl_top_offset 101CEBE4
data l_crawl_front_up_offset 101CEBF0
data l_crawl_back_up_offset 101CEBFC
data l_crawl_stand_up_offset 101CEC08
data l_crawl_top_up_offset 101CEC14
data l_crawl_side_offset 101CEC20
data l_crawl_lside_offset 101CEC2C
data l_crawl_rside_offset 101CEC38
data l_crawl_lside_front_offset 101CEC44
data l_crawl_rside_front_offset 101CEC50
data l_crawl_min_side_offset 101CEC5C
data l_hammer_splash_particle_scale 1046CD3C
data l_cyl_src 101CECDC
data l_wind_cyl_src 101CED20
data l_at_cyl_src 101CED64
data l_fan_wind_cps_src 101CEDF4
data l_sleeveMaterialName 1046CCA4
data l_mouthMaterialName 1046CCAC
data l_faceMaterialName 1046CCB4
data l_legsMaterialName 1046CCBC
data l_hatMaterialName 1046CCC4
data l_hairMaterialName 1046CCCC
data l_earMaterialName 1046CCD4
data l_leftEyeMaterialName 1046CD2C
data l_rightEyeMaterialName 1046CD34
str 10036200
str 100361F8
str 10036208
str 10036210
str 10036218
str 10036220
str 100361F4
str 10036228
str 10036230
data l_staticGroundCheck 1046D29C
data l_headerZeroState 1046CD1C
data l_headerAngleLimits 1046CCF0
data l_headerByte 1046CD18
data l_headerFlag 1046CD19
data l_headerDestructorRecord0 101CEEE4
data l_headerDestructorRecord1 101CEEF0
data l_headerDestructorRecord2 101CEEFC
data l_headerDestructorRecord3 101CEF08
data l_rootHeap 101F8B4C
data l_movementDefaults 1046CCDC
data l_headerDistances 1046CCF8
data l_tact_top 10034FF0
data l_swordEffectScale 101CEED8
*/

// clang-format on
