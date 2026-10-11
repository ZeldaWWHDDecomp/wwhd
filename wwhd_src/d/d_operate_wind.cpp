#include "d/d_operate_wind.h"

// Wind direction control. HD: GamePad layout/state implementation replaces J2D.
// HD: constructor/state template entries retain their verified register widths.

static dOwVtableIdentity_c l_safeStringVtable;
static dOwVtableIdentity_c l_stateDescriptorVtable;
static dOwVtableIdentity_c l_windVtable;
static dOwVtableIdentity_c l_stateIdVtable;
static dOwVtableIdentity_c l_stateFactoryVtable;
static u32 l_heapRoot;
static u32 l_gamePad;
static dOwRegistration_c l_angleRegistration;
static dOwRegistration_c l_angleFlagRegistration;
static dOwRegistration_c l_moveFlagRegistration;
static u32 l_uiRoot;
static u32 l_stateIdInitialized;
static u32 l_stateIdCount;
static dOwVtableIdentity_c l_defaultFactoryVtable;
static dOwUiFlags_c l_uiFlags;
static dOwSafeString_c l_languagePane;
static dOwSafeString_c l_controlArea;
static dOwSafeString_c l_windNowPane;
static dOwSafeString_c l_arrowChangePane;
static dOwSafeString_c l_windChangePane;
static dOwSafeString_c l_innerControlArea;
static f32 l_minAngle;
static f32 l_maxAngle;
static f32 l_moveRangeX;
static f32 l_moveRangeY;
static f32 l_moveStepX;
static f32 l_moveStepY;
static u8 l_angleRangeFlag;
static u8 l_moveRangeFlag;
static dOwZeroBlock_c l_zeroBlock;
static dOwSafeString_c l_allGroup;
static dOwSafeString_c l_windNowGroup;
static dOwSafeString_c l_windChangeGroup;
static dOwSafeString_c l_inAllAnimation;
static dOwSafeString_c l_windNowAnimation;
static dOwSafeString_c l_windChangeAnimation;
static dOwSafeString_c l_outAllAnimation;
static dOwSafeString_c l_screenName;
static dOwSafeString_c l_buttonName;
static dOwStateDescriptor_c StateID_Wait;
static dOwStateDescriptor_c StateID_InAll;
static dOwStateDescriptor_c StateID_Disp;
static dOwStateDescriptor_c StateID_OutWait;
static dOwStateDescriptor_c StateID_OutAll;
static dOwSafeString_c l_layoutName;
static dOwStateDescriptor_c l_managerInAll;
static dOwPoint_c l_noTouchPoint;
static const u32 l_animationGroups[4] = {0, 1, 2, 0};

static u32 wind_pane(u32 owner, u32 name) {

    dOwSafeStringVtable_c *vt =
        gabi::at<dOwSafeStringVtable_c>(gabi::at<dOwSafeString_c>(name)->mVtable);
    u32 target = vt->mAssureTermination;
    dOwPaneOwnerVtable_c *table =
        gabi::at<dOwPaneOwnerVtable_c>(gabi::at<dOwPaneOwner_c>(owner)->mVtable);

    gabi::call_ptr<void>(target, name);

    return gabi::call_ptr<u32>(table->mFindPane, owner,
                               u32(gabi::at<dOwSafeString_c>(name)->mpString), 1);
}

// Screen operations in the GameCube source order.

/* 0264E240 */
void dOperate_wind_c::screenSet() {

    dOwLayout_c *layout = this->mpLayout.get();
    dOwSafeStringVtable_c *nameVt = gabi::at<dOwSafeStringVtable_c>(l_controlArea.mVtable);
    dOwPaneManager_c *paneManager = layout->mpPaneManager.get();
    dOwPaneOwner_c *owner = paneManager->mpRootPane.get();
    dOwPaneOwnerVtable_c *table = gabi::at<dOwPaneOwnerVtable_c>(owner->mVtable);
    gabi::call_ptr<void>(nameVt->mAssureTermination, gabi::ea(&l_controlArea));
    dOwPane_c *pane = gabi::at<dOwPane_c>(
        gabi::call_ptr<u32>(table->mFindPane, gabi::ea(owner), u32(l_controlArea.mpString), 1));
    this->mpCenterPane = pane;
    this->mPosXBits = pane->mPositionXBits;
    this->mPosYBits = pane->mPositionYBits;
    this->mPosZBits = pane->mPositionZBits;
}

/* 0264DD5C */
void dOperate_wind_c::arrowDirection() {
    u32 s = gabi::ea(this);

    this->mTouchAngleSet = 0;
    dOw_callTouchControl(s);
    u8 near = this->mDragging;
    this->mChangeBlocked = 0;
    if (near) {
        u32 whole = u32(this->mDeg) << 16;
        this->mArrowAngle = whole;
        this->mTargetAngle = whole;
        dOw_callCursorRotate(s, (f32(s32(whole)) * 180.0f) / 2147483648.0f);
    } else {
        dOw_callButtonControl(s);
        u32 target = u32(this->mDeg) << 16;
        u32 previous = this->mTargetAngle;
        gabi::Local<gabi::be<u32>> to, step;
        *to = target;
        *step = 0x20000000;
        if (previous != target) {
            dOw_playSound(gabi::guest_str("SE_TAKT_WIND_CURSOR"));
            this->mTargetAngle = u32(*to);
        }
        dOw_addCalcAngle(gabi::ea(&this->mArrowAngle), to.a, step.a);
        dOw_callCursorRotate(s, (f32(s32(this->mArrowAngle)) * 180.0f) / 2147483648.0f);
    }
    if (this->mCheckChange && this->mPreviousDeg != this->mDeg) {
        this->mCheckChange = 0;
        this->mChanged = 1;
        dOw_playSound(gabi::guest_str("SE_TAKT_WIND_CURSOR"));
    }
}

/* 0264DF8C */
void dOperate_wind_c::windSet() {

    s32 angle = s32(this->mArrowAngle) / 65536;
    dOw_tactWindSet(0, s16(-(angle + 0x4000)));
}

/* 0264D5C0 */
void dOperate_wind_c::arrowRotate(f32 angle) {

    dOwPane_c *pane = this->mpWindArrow.get();
    u32 x = pane->mRotationXBits, y = pane->mRotationYBits;
    (void)u32(pane->mRotationZBits);
    pane->mRotationX = gabi::f32_from_bits(x);
    f32 yy = gabi::f32_from_bits(y);
    u8 flags = pane->mFlags;
    pane->mRotationY = yy;
    pane->mRotationZ = angle;
    pane->mFlags = flags | dOwPane_RotationChanged_e;
}

/* 0264D650 */
void dOperate_wind_c::initialize() {
    u32 s = gabi::ea(this);

    u32 angle = dOw_getWindAngle();
    u32 quantized = (angle + dOwAngle_HalfStep_e) & dOwAngle_DirectionMask_e;
    u32 whole = quantized << 16;
    this->mDeg = u16(quantized);
    this->mArrowAngle = whole;
    f32 degrees = (f32(s32(whole)) * 180.0f) / 2147483648.0f;
    dOw_callArrowRotate(s, degrees);
    this->mDeg = 0;
    this->mArrowAngle = 0;
    dOw_callCursorRotate(s, 0.0f);
    dOwStick_c *obj = this->stick.get();
    this->mPreviousDeg = this->mDeg;
    this->mTargetAngle = this->mArrowAngle;
    dOwStickVtable_c *vt = gabi::at<dOwStickVtable_c>(obj->mVtable);
    f32 result = gabi::call_ptr<f32>(vt->mGetValue, gabi::ea(obj));
    this->mDragging = 0;
    this->mTouchBlocked = 0;
    this->mChangeRequested = 0;
    this->mChanged = 0;
    this->mCheckChange = 0;
    this->mTouchMode = dOwTouch_Idle_e;
    this->mStickValue = result;
}

/* 0264E2C8 */
u32 dOperate_wind_c::_create(u32 archive, u32 a, u32 b) {
    u32 s = gabi::ea(this);

    dOw_setLayoutName(s, gabi::ea(&l_screenName), 2);

    dOwScreenVtable_c *vt = gabi::at<dOwScreenVtable_c>(this->mVtable);
    u32 target = vt->mLoadLayout;

    if (!gabi::call_ptr<u32>(target, s, gabi::ea(&l_layoutName), gabi::ea(this->mpArchive.get()),
                             archive, a, b))
        return 0;

    dOw_callCenterPane(s);

    dOwLayout_c *layout = this->mpLayout.get();
    dOwSafeStringVtable_c *namevt = gabi::at<dOwSafeStringVtable_c>(l_windNowPane.mVtable);
    dOwPaneManager_c *paneManager = layout->mpPaneManager.get();
    dOwPaneOwner_c *owner = paneManager->mpRootPane.get();
    dOwPaneOwnerVtable_c *table = gabi::at<dOwPaneOwnerVtable_c>(owner->mVtable);
    u32 nameTarget = namevt->mAssureTermination;

    gabi::call_ptr<void>(nameTarget, gabi::ea(&l_windNowPane));

    dOwPane_c *pane = gabi::at<dOwPane_c>(
        gabi::call_ptr<u32>(table->mFindPane, gabi::ea(owner), u32(l_windNowPane.mpString), 1));

    this->mpWindArrow = pane;

    pane = gabi::at<dOwPane_c>(wind_pane(gabi::ea(owner), gabi::ea(&l_arrowChangePane)));
    this->mpCursorArrow = pane;

    pane = gabi::at<dOwPane_c>(wind_pane(gabi::ea(owner), gabi::ea(&l_windChangePane)));
    this->mpCursorPane = pane;

    f32 x = pane->mWidth * 0.5f, y = pane->mHeight * 0.5f;

    this->mCursorRadiusSquared = gabi::fmadds(x, x, y * y);

    pane = gabi::at<dOwPane_c>(wind_pane(gabi::ea(owner), gabi::ea(&l_innerControlArea)));
    x = pane->mWidth * 0.5f;
    y = pane->mHeight * 0.5f;

    this->mCenterRadiusSquared = gabi::fmadds(x, x, y * y) * 0.5f;

    pane = gabi::at<dOwPane_c>(wind_pane(gabi::ea(owner), gabi::ea(&l_languagePane)));
    pane->mFlags = pane->mFlags & ~dOwPane_Visible_e;

    u32 state = this->mStateManager;
    dOwStateVtable_c *stateVt =
        gabi::at<dOwStateVtable_c>(gabi::at<dOwStateInterface_c>(state)->mVtable);

    state = gabi::call_ptr<u32>(stateVt->mGetState, state, gabi::ea(&StateID_Wait));
    this->mState = state;

    stateVt = gabi::at<dOwStateVtable_c>(gabi::at<dOwStateInterface_c>(state)->mVtable);
    gabi::call_ptr<void>(stateVt->mInitialize, state);
    return 1;
}

/* 0264D4BC */
void dOperate_wind_c::_delete(u32 flag) {
    u32 s = gabi::ea(this);

    if (!s)
        return;
    dOwStick_c *p = this->stick.get();
    this->mVtable = gabi::ea(&l_windVtable);
    this->mStateIdVtable = gabi::ea(&l_stateIdVtable);
    this->mStateFactoryVtable = gabi::ea(&l_stateFactoryVtable);
    if (p) {
        dOwStickVtable_c *vt = gabi::at<dOwStickVtable_c>(p->mVtable);
        gabi::call_ptr<void>(vt->mDestroy, gabi::ea(p), 3);
    }
    dOw_layoutScreenDtor(s, 0);
    if (flag & 1)
        dOw_operatorDelete(s);
}

/* 0264E5B4 */
void dOperate_wind_c::_move() {
    u32 s = gabi::ea(this);

    dOw_executeStateManager(gabi::ea(&this->mStateManager));
    dOwScreenVtable_c *vt = gabi::at<dOwScreenVtable_c>(this->mVtable);
    gabi::call_ptr<void>(vt->mBeforeMove, s);
    vt = gabi::at<dOwScreenVtable_c>(this->mVtable);
    gabi::call_ptr<void>(vt->mAnimateLayout, s);
}

/* 0264E60C */
void dOperate_wind_c::_draw() {
    u32 s = gabi::ea(this);

    if (this->mVisible) {
        dOwScreenVtable_c *vt = gabi::at<dOwScreenVtable_c>(this->mVtable);
        gabi::call_ptr<void>(vt->mDrawLayout, s);
    }
}

// HD: touch input, layout animations and wind screen states.

/* 0264D554 */
void dOperate_wind_c::wait() {

    u16 timer = this->mTimer;
    if (timer) {
        timer = u16(timer - 1);
        this->mTimer = timer;
        if (!timer)
            this->mWindAngle = u16(dOw_getWindAngle());
    }
    dOwPlayView_c *play = gabi::at<dOwPlayView_c>(dComIfGp_ea());
    if (play->getPlay()->mOperateWind == 2)
        dOw_changeState(gabi::ea(&this->mStateManager), gabi::ea(&StateID_InAll));
}

/* 0264D608 */
void dOperate_wind_c::cursorRotate(f32 angle) {

    dOwPane_c *pane = this->mpCursorArrow.get();
    u32 x = pane->mRotationXBits, y = pane->mRotationYBits;
    (void)u32(pane->mRotationZBits);
    pane->mRotationX = gabi::f32_from_bits(x);
    f32 yy = gabi::f32_from_bits(y);
    u8 flags = pane->mFlags;
    pane->mRotationY = yy;
    pane->mRotationZ = angle;
    pane->mFlags = flags | dOwPane_RotationChanged_e;
}

/* 0264D73C */
void dOperate_wind_c::initializeState_InAll() {
    u32 s = gabi::ea(this);

    dOwPlayView_c *play = gabi::at<dOwPlayView_c>(dComIfGp_ea());
    u8 b = play->mActionButtonB, a = play->mActionButtonA, c = play->mActionButtonR;
    play->mSavedActionButtonA = a;
    play->mSavedActionButtonR = c;
    play->mSavedActionButtonB = b;
    play = gabi::at<dOwPlayView_c>(dComIfGp_ea());
    play->mActionButtonA = 23;
    play = gabi::at<dOwPlayView_c>(dComIfGp_ea());
    play->mActionButtonB = 7;
    l_uiFlags.mWindControl = 1;
    l_uiFlags.mControlMode = 3;
    dOwLayout_c *layout = this->mpLayout.get();
    dOw_startAnimation(layout->mpAnimator, dOwAnimation_InAll_e, 0, 1.0f);
    dOw_callInitialize(s);
    dOwRoot_c *global = gabi::at<dOwRoot_c>(l_uiRoot);
    u32 manager = global->mpWindManager;
    dOw_changeState(manager + 0x18, gabi::ea(&l_managerInAll));
    this->mVisible = 1;
    this->mActive = 1;
}

/* 0264D800 */
void dOperate_wind_c::executeState_InAll() {

    dOwLayout_c *layout = this->mpLayout.get();
    if (dOw_isAnimationDone(layout->mpAnimator, dOwAnimation_InAll_e))
        dOw_changeState(gabi::ea(&this->mStateManager), gabi::ea(&StateID_Disp));
}

/* 0264D850 */
void dOperate_wind_c::initializeState_Disp() {

    dOwLayout_c *layout = this->mpLayout.get();
    dOw_startAnimation(layout->mpAnimator, dOwAnimation_WindNow_e, 1, 1.0f);
    layout = this->mpLayout.get();
    dOw_startAnimation(layout->mpAnimator, dOwAnimation_WindChange_e, 2, 1.0f);
}

/* 0264DF80 */
void dOperate_wind_c::finalizeState_Disp() { this->mActive = 0; }

/* 0264D8C8 */
u32 dOperate_wind_c::checkTouchBlocked() {

    if (dOw_isInputBlocked(l_gamePad))
        return 1;
    dOwPadView_c *pad = gabi::at<dOwPadView_c>(l_gamePad);
    dOwPoint_c *point =
        gabi::at<dOwPoint_c>((pad->mTouchFlags & dOwPad_TouchValid_e) ? gabi::ea(pad) + 0x1B4
                                                                      : gabi::ea(&l_noTouchPoint));
    f32 x = point->x, y = point->y;
    this->mTouchX = x;
    gabi::Local<dOwPoint_c> xy;
    xy->x = x;
    xy->y = y;
    u32 pane = gabi::ea(this->mpCenterPane.get());
    this->mTouchY = y;
    if (!dOw_containsPoint(pane, xy.a))
        return 1;
    if (!this->mDragging) {
        f32 dx = this->mTouchX - this->mPosX, dy = this->mTouchY - this->mPosY;
        f32 d = gabi::fmadds(dx, dx, dy * dy);
        if (d < this->mCenterRadiusSquared)
            return 1;
    }
    return 0;
}

/* 0264DA14 */
void dOperate_wind_c::touchAngle() {

    f32 dx = this->mTouchX - this->mPosX, dy = this->mTouchY - this->mPosY;
    this->mTouchAngleSet = 1;
    // HD: keep the full returned register until the direction mask is applied.
    u32 angle = dOw_atan2sRegister(-dx, dy);
    this->mDeg = u16((angle + dOwAngle_HalfStep_e) & dOwAngle_DirectionMask_e);
}

/* 0264DA70 */
u32 dOperate_wind_c::isTouchNearCursor() {

    gabi::Local<cXyz> point; // HD: pane query writes all twelve bytes of a position.
    dOw_getPanePosition(point.a, gabi::ea(this->mpCursorPane.get()));
    f32 dy = this->mTouchY - point->y, dx = this->mTouchX - point->x;
    return gabi::fmadds(dx, dx, dy * dy) < this->mCursorRadiusSquared;
}

/* 0264DAD4 */
void dOperate_wind_c::requestChange() {

    if (!this->mChanged || this->mChangeRequested || this->mChangeBlocked)
        return;
    this->mChangeRequested = 1;
    dOwRoot_c *global = gabi::at<dOwRoot_c>(l_uiRoot);
    dOw_notifyWindChange(global->mpWindManager);
}

/* 0264DB10 */
void dOperate_wind_c::touchControl() {
    u32 s = gabi::ea(this);

    s32 blocked = dOw_callCheckTouchBlocked(s);
    u32 mode = this->mTouchMode;
    if (mode == dOwTouch_Idle_e) {
        if (dOw_callIsTouchHeld(s)) {
            if (blocked > 0) {
                this->mTouchBlocked = 1;
                return;
            }
            u8 was = this->mTouchBlocked;
            this->mDragging = 0;
            this->mTouchMode = dOwTouch_Drag_e;
            if (was)
                return;
            dOw_playSound(gabi::guest_str("SE_DRC_TOUCH_ON"));
            if (blocked == 0) {
                dOw_callTouchAngle(s);
                if (dOw_callIsTouchNearCursor(s))
                    this->mDragging = 1;
            }
        } else {
            u16 angle = this->mDeg;
            this->mTouchBlocked = 0;
            this->mCheckChange = 1;
            this->mPreviousDeg = angle;
            dOw_callRequestChange(s);
        }
    } else if (mode == dOwTouch_Drag_e) {
        u32 held = dOw_callIsTouchHeld(s);
        u8 near = this->mDragging;
        if (held) {
            if (near) {
                this->mCheckChange = 1;
                this->mPreviousDeg = this->mDeg;
                dOw_callTouchAngle(s);
            } else if (dOw_callIsTouchNearCursor(s)) {
                this->mDragging = 1;
                dOw_callTouchAngle(s);
            }
        } else {
            if (near)
                this->mDragging = 0;
            this->mTouchMode = dOwTouch_Idle_e;
        }
    }
}

/* 0264DCF4 */
void dOperate_wind_c::buttonControl() {

    if (this->mTouchAngleSet)
        return;
    dOwPadView_c *pad = gabi::at<dOwPadView_c>(l_gamePad);
    u32 mode = pad->mRepeatButtons, a = pad->mTriggerButtons, b = pad->mHoldButtons,
        buttons = a | b;
    if (mode & dOwPad_RepeatMask_e)
        buttons &= dOwPad_RepeatMask_e;
    if (buttons & dOwPad_Clockwise_e)
        this->mDeg = u16((this->mDeg + dOwAngle_Step_e) & dOwAngle_DirectionMask_e);
    else if (buttons & dOwPad_CounterClockwise_e)
        this->mDeg = u16((this->mDeg - dOwAngle_Step_e) & dOwAngle_DirectionMask_e);
}

/* 0264DED8 */
void dOperate_wind_c::executeState_Disp() {
    u32 s = gabi::ea(this);

    dOwRoot_c *root = gabi::at<dOwRoot_c>(l_uiRoot);
    dOwManager_c *manager = gabi::at<dOwManager_c>(root->mpWindManager);
    if (manager->mConfirmRequested) {
        dOw_changeState(gabi::ea(&this->mStateManager), gabi::ea(&StateID_OutWait));
        return;
    }
    if (manager->mCancelRequested) {
        dOwPlayView_c *play = gabi::at<dOwPlayView_c>(dComIfGp_ea());
        play->getPlay()->mOperateWind = 0;
        dOw_changeState(gabi::ea(&this->mStateManager), gabi::ea(&StateID_OutAll));
        return;
    }
    dOw_callArrowDirection(s);
}

/* 0264DFAC */
void dOperate_wind_c::executeState_OutWait() {
    u32 s = gabi::ea(this);

    dOwRoot_c *root = gabi::at<dOwRoot_c>(l_uiRoot);
    dOwManager_c *manager = gabi::at<dOwManager_c>(root->mpWindManager);
    if (manager->mConfirmDone) {
        this->mWindAngle = this->mDeg;
        dOw_callWindSet(s);
        dOwPlayView_c *play = gabi::at<dOwPlayView_c>(dComIfGp_ea());
        play->getPlay()->mOperateWind = 1;
        dOw_changeState(gabi::ea(&this->mStateManager), gabi::ea(&StateID_OutAll));
    }
}

/* 0264E018 */
void dOperate_wind_c::initializeState_OutAll() {

    dOwLayout_c *layout = this->mpLayout.get();
    dOw_startAnimation(layout->mpAnimator, dOwAnimation_OutAll_e, 0, 1.0f);
}

/* 0264E034 */
void dOperate_wind_c::executeState_OutAll() {

    dOwLayout_c *layout = this->mpLayout.get();
    if (dOw_isAnimationDone(layout->mpAnimator, dOwAnimation_OutAll_e))
        dOw_changeState(gabi::ea(&this->mStateManager), gabi::ea(&StateID_Wait));
}

/* 0264E084 */
void dOperate_wind_c::finalizeState_OutAll() {

    dOwPlayView_c *play = gabi::at<dOwPlayView_c>(dComIfGp_ea());
    play->getPlay()->mOperateWind = 0;
    this->mVisible = 0;
    play = gabi::at<dOwPlayView_c>(dComIfGp_ea());
    u8 c = play->mSavedActionButtonR, b = play->mSavedActionButtonB;
    play->mActionButtonR = c;
    u8 a = play->mSavedActionButtonA;
    play->mActionButtonB = b;
    play->mActionButtonA = a;
    l_uiFlags.mWindControl = 0;
}

/* 0264E560 */
void dOperate_wind_c::_execute() {
    u32 s = gabi::ea(this);

    dOwScreenVtable_c *vt = gabi::at<dOwScreenVtable_c>(this->mVtable);
    gabi::call_ptr<void>(vt->mBeforeExecute, s);
    vt = gabi::at<dOwScreenVtable_c>(this->mVtable);
    gabi::call_ptr<void>(vt->mExecuteLayout, s);
    dOw_layoutScreenExecute(s);
}

/* 0264E628 */
u32 dOperate_wind_c::loadLayout(u32 archive, u32 a, u32 b, u32 c, u32 d) {
    u32 s = gabi::ea(this);

    dOwHeapRoot_c *global = gabi::at<dOwHeapRoot_c>(l_heapRoot);
    dOwLayout_c *layout = gabi::at<dOwLayout_c>(dOw_heapNew(0x110, global->mpHeap, 4));
    if (layout)
        layout = gabi::at<dOwLayout_c>(dOw_layoutCtor(gabi::ea(layout)));
    this->mpLayout = layout;
    if (!layout)
        return 0;
    dOwLayoutVtable_c *vt = gabi::at<dOwLayoutVtable_c>(layout->mVtable);
    if (!gabi::call_ptr<u32>(vt->mLoad, gabi::ea(layout), archive, a, b, c, 4, 3, d))
        return 0;
    for (u32 i = 0; i < dOwAnimation_Count_e; ++i) {
        dOwLayout_c *target = this->mpLayout.get();
        u32 index = l_animationGroups[i];
        dOw_bindAnimation(target->mpAnimator, i, gabi::ea(&l_inAllAnimation) + 8 * i,
                          gabi::ea(&l_allGroup) + 8 * index);
    }
    dOwScreenVtable_c *screenVt = gabi::at<dOwScreenVtable_c>(this->mVtable);
    gabi::call_ptr<void>(screenVt->mAnimateLayout, s);
    return 1;
}

// HD: this entry consumes no receiver, while its callers pass the original extra r3.
/* 0264D9B0 */
u32 dOw_isTouchHeld() {

    if (dOw_isInputBlocked(l_gamePad))
        return 0;
    dOwPadView_c *pad = gabi::at<dOwPadView_c>(l_gamePad);
    return (pad->mRepeatButtons & dOwPad_TouchHeld_e) != 0;
}

// HD: compiler-generated and template code of this translation unit.
// Distinct empty template hooks retain their individual verified entries.

/* 0264D388 */
void dOw_deleteStateFactory(u32 s, u32 flag) {
    if (s && (flag & 1))
        dOw_operatorDelete(s);
}

/* 0264D39C */
void dOw_emptyStateHook1() {}

/* 0264D3A0 */
void dOw_emptyStateHook2() {}

/* 0264D3A4 */
void dOw_emptyStateHook3() {}

/* 0264D3A8 */
void dOw_emptyStateHook4() {}

/* 0264D3AC */
void dOw_emptyStateHook5() {}

/* 0264D3B0 */
void dOw_emptyStateHook6() {}

/* 0264D3B4 */
void dOw_emptyStateHook7() {}

/* 0264D3B8 */
void dOw_emptyStateHook8() {}

/* 0264D3C4 */
void dOw_emptyStateHook9() {}

/* 0264D3C8 */
void dOw_emptyStateHook10() {}

/* 0264D3CC */
void dOw_emptyStateHook11() {}

/* 0264D3BC */
u32 dOw_getStateId(u32 s) {
    dOwStateId_c *state = gabi::at<dOwStateId_c>(s);
    return u32(state->mId);
}

/* 0264D3D0 */
u32 dOw_initializeState(u32 desc, u32 obj) {
    dOwStateDescriptor_c *stateDesc = gabi::at<dOwStateDescriptor_c>(desc);

    s16 index = stateDesc->mInitialize.mVirtualIndex,
        adjust = stateDesc->mInitialize.mThisAdjustment;
    u32 self = obj + s32(adjust);
    u32 target;
    if (index < 0)
        target = stateDesc->mInitialize.mFunction;
    else {
        s16 vtOff = stateDesc->mInitialize.mVtableOffset;
        // HD: the member descriptor supplies both the subobject vtable offset and slot.
        u32 vt = gabi::load<u32>(self + s32(vtOff));
        target = gabi::load<u32>(vt + u32(s32(index)) * 8 + 4);
    }
    return gabi::call_ptr<u32>(target, self, obj);
}

/* 0264D410 */
u32 dOw_executeState(u32 desc, u32 obj) {
    dOwStateDescriptor_c *stateDesc = gabi::at<dOwStateDescriptor_c>(desc);

    s16 index = stateDesc->mExecute.mVirtualIndex, adjust = stateDesc->mExecute.mThisAdjustment;
    u32 self = obj + s32(adjust);
    u32 target;
    if (index < 0)
        target = stateDesc->mExecute.mFunction;
    else {
        s16 vtOff = stateDesc->mExecute.mVtableOffset;
        // HD: the member descriptor supplies both the subobject vtable offset and slot.
        u32 vt = gabi::load<u32>(self + s32(vtOff));
        target = gabi::load<u32>(vt + u32(s32(index)) * 8 + 4);
    }
    return gabi::call_ptr<u32>(target, self, obj);
}

/* 0264D450 */
u32 dOw_finalizeState(u32 desc, u32 obj) {
    dOwStateDescriptor_c *stateDesc = gabi::at<dOwStateDescriptor_c>(desc);

    s16 index = stateDesc->mFinalize.mVirtualIndex, adjust = stateDesc->mFinalize.mThisAdjustment;
    u32 self = obj + s32(adjust);
    u32 target;
    if (index < 0)
        target = stateDesc->mFinalize.mFunction;
    else {
        s16 vtOff = stateDesc->mFinalize.mVtableOffset;
        // HD: the member descriptor supplies both the subobject vtable offset and slot.
        u32 vt = gabi::load<u32>(self + s32(vtOff));
        target = gabi::load<u32>(vt + u32(s32(index)) * 8 + 4);
    }
    return gabi::call_ptr<u32>(target, self, obj);
}

/* 0264D490 */
u32 dOw_getCurrentState(u32 s) {
    dOperate_wind_c *i_this = gabi::at<dOperate_wind_c>(s);

    dOwStateId_c *state = gabi::at<dOwStateId_c>(i_this->mStateIdVtable);
    if (state->mId == -1)
        return i_this->mBaseVtable;
    dOwStateVtable_c *vt = gabi::at<dOwStateVtable_c>(state->mVtable);
    return gabi::call_ptr<u32>(vt->mGetState, gabi::ea(state));
}

// HD: full-register allocating constructor; retain wraparound allocation tests.
/* 0264E0E8 */
u32 dOw_createScreen(u32 s) {

    if (!s)
        s = dOw_operatorNew(sizeof(dOperate_wind_c));
    if (s) {
        dOperate_wind_c *i_this = gabi::at<dOperate_wind_c>(s);
        dOw_layoutScreenCtor(s);
        i_this->mStateFactoryVtable = gabi::ea(&l_stateFactoryVtable);
        i_this->mVtable = gabi::ea(&l_windVtable);
        i_this->mStateIdVtable = gabi::ea(&l_stateIdVtable);
        if (s + 0x50 == 0)
            dOw_operatorNew(4);
        i_this->mpCursorArrow = 0;
        i_this->mPosZ = 0;
        i_this->mpWindArrow = 0;
        i_this->mPosY = 0;
        i_this->stick = 0;
        dOwPoint_c *vec = gabi::at<dOwPoint_c>(s + 0x70);
        i_this->mPosX = 0;
        if (!vec)
            vec = gabi::at<dOwPoint_c>(dOw_operatorNew(8));
        if (vec) {
            vec->y = 0;
            vec->x = 0;
        }
        i_this->mWindAngle = 0;
        i_this->mActive = 0;
        i_this->mChangeBlocked = 0;
        i_this->mTimer = 2;
        i_this->mChangeRequested = 0;
        i_this->mTargetAngle = 0;
        i_this->mTouchBlocked = 0;
        i_this->mCheckChange = 0;
        i_this->mCursorRadiusSquared = 0;
        i_this->mArrowAngle = 0;
        i_this->mDeg = 0;
        i_this->mVisible = 0;
        i_this->mStickValue = 0;
        i_this->mDragging = 0;
        i_this->mChanged = 0;
        i_this->mTouchAngleSet = 0;
        i_this->mCenterRadiusSquared = 0;
        i_this->mTouchMode = dOwTouch_Idle_e;
        u32 manager = dOw_stControlCtor(0, 5, 2, 3, 2, 0x1000, 0x2000, 1.0f, 1.0f);
        i_this->stick = gabi::at<dOwStick_c>(manager);
    }
    return s;
}

/* 0264E754 */
void dOw_staticInit() {

    for (u32 off : {12u, 8u, 4u, 0u})
        l_zeroBlock.mWords[off / 4] = 0;

    dOw_registerRange(gabi::ea(&l_angleRegistration));

    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;

    dOw_initializeRangeFlag(gabi::ea(&l_angleRangeFlag));
    dOw_registerRange(gabi::ea(&l_angleFlagRegistration));
    dOw_initializeScaleFlag(gabi::ea(&l_moveRangeFlag));
    dOw_registerRange(gabi::ea(&l_moveFlagRegistration));

    l_moveRangeX = 50000.0f;
    l_moveStepX = 10000.0f;
    l_moveRangeY = 50000.0f;
    l_moveStepY = 10000.0f;

    const u32 names[] = {gabi::ea(&l_allGroup),         gabi::ea(&l_inAllAnimation),
                         gabi::ea(&l_windNowAnimation), gabi::ea(&l_windChangeAnimation),
                         gabi::ea(&l_outAllAnimation),  gabi::ea(&l_languagePane),
                         gabi::ea(&l_screenName),       gabi::ea(&l_buttonName),
                         gabi::ea(&l_arrowChangePane),  gabi::ea(&l_windNowPane),
                         gabi::ea(&l_controlArea),      gabi::ea(&l_layoutName),
                         gabi::ea(&l_windChangePane),   gabi::ea(&l_innerControlArea),
                         gabi::ea(&l_windNowGroup),     gabi::ea(&l_windChangeGroup)};

    const u32 strings[] = {
        gabi::guest_str("G_All_00"),         gabi::guest_str("InAll"),
        gabi::guest_str("LoopWindNow"),      gabi::guest_str("LoopWindChange"),
        gabi::guest_str("OutAll"),           gabi::guest_str("N_JpJa_00"),
        gabi::guest_str("ControlWind_00"),   gabi::guest_str("BtnControlWind_00"),
        gabi::guest_str("N_ArrowChange_00"), gabi::guest_str("N_WindNow_00"),
        gabi::guest_str("B_ControlArea_00"), gabi::guest_str("ControlWind_00"),
        gabi::guest_str("B_WindChange_00"),  gabi::guest_str("B_ControlArea_01"),
        gabi::guest_str("G_LoopWindNow_00"), gabi::guest_str("G_LoopWindChange_00")};

    for (u32 i = 0; i < 16; ++i) {
        dOwSafeString_c *name = gabi::at<dOwSafeString_c>(names[i]);
        name->mpString = strings[i];
        name->mVtable = gabi::ea(&l_safeStringVtable);
    }

    bool initialized = l_stateIdInitialized != 0;
    u32 id;

    if (initialized)
        id = l_stateIdCount;
    else {
        l_stateIdInitialized = 1;
        id = 0;
    }

    const u32 objects[] = {gabi::ea(&StateID_Wait), gabi::ea(&StateID_InAll),
                           gabi::ea(&StateID_Disp), gabi::ea(&StateID_OutWait),
                           gabi::ea(&StateID_OutAll)};

    const u32 stateNames[] = {gabi::guest_str("::StateID_Wait"), gabi::guest_str("::StateID_InAll"),
                              gabi::guest_str("::StateID_Disp"),
                              gabi::guest_str("::StateID_OutWait"),
                              gabi::guest_str("::StateID_OutAll")};

    for (u32 i = 0; i < 5; ++i) {
        dOwStateDescriptor_c *descriptor = gabi::at<dOwStateDescriptor_c>(objects[i]);
        descriptor->mId = ++id;
        descriptor->mpName = stateNames[i];
        descriptor->mVtable = gabi::ea(&l_stateDescriptorVtable);
        for (u32 k = 0; k < dOwState_MethodCount_e; ++k) {
            dOwMemberFunction_c *callback = gabi::at<dOwMemberFunction_c>(
                gabi::ea(&descriptor->mInitialize) + sizeof(dOwMemberFunction_c) * k);
            callback->mThisAdjustment = 0;
            callback->mVirtualIndex = u16(dOwState_FirstMethod_e + i * dOwState_MethodCount_e + k);
            callback->mFunction = 4;
        }
        descriptor->mDefault.mThisAdjustment = 0;
        descriptor->mDefault.mVirtualIndex = 0;
        descriptor->mDefault.mFunction = 0;
        descriptor->mFactoryVtable = gabi::ea(&l_defaultFactoryVtable);
    }

    l_stateIdCount = id;
}

/* WWHD glue
data l_safeStringVtable 100EEB24
data l_stateDescriptorVtable 100EEB94
data l_windVtable 100EED70
data l_stateIdVtable 100EEEB0
data l_stateFactoryVtable 100EEEC0
data l_heapRoot 1018C404
data l_gamePad 101F5088
data l_angleRegistration 101F58F4
data l_angleFlagRegistration 101F5900
data l_moveFlagRegistration 101F590C
data l_uiRoot 101F8344
data l_stateIdInitialized 101FD8E4
data l_stateIdCount 101FDD50
data l_defaultFactoryVtable 101FF32C
data l_uiFlags 1047B03C
data l_languagePane 104919A0
data l_controlArea 104919A8
data l_windNowPane 104919B0
data l_arrowChangePane 104919B8
data l_windChangePane 104919C0
data l_innerControlArea 104919C8
data l_minAngle 104919D0
data l_maxAngle 104919D4
data l_moveRangeX 104919D8
data l_moveRangeY 104919DC
data l_moveStepX 104919E0
data l_moveStepY 104919E4
data l_angleRangeFlag 104919E8
data l_moveRangeFlag 104919E9
data l_zeroBlock 104919EC
data l_allGroup 104919FC
data l_windNowGroup 10491A04
data l_windChangeGroup 10491A0C
data l_inAllAnimation 10491A14
data l_windNowAnimation 10491A1C
data l_windChangeAnimation 10491A24
data l_outAllAnimation 10491A2C
data l_screenName 10491A34
data l_buttonName 10491A3C
data StateID_Wait 10491A44
data StateID_InAll 10491A74
data StateID_Disp 10491AA4
data StateID_OutWait 10491AD4
data StateID_OutAll 10491B04
data l_layoutName 10491B34
data l_managerInAll 10491B70
data l_noTouchPoint 1049FFA0
data l_animationGroups 100EEB3C
str 100EEBF4 "SE_DRC_TOUCH_ON"
str 100EEC04 "SE_TAKT_WIND_CURSOR"
str 100EEC2C "InAll"
str 100EEC34 "OutAll"
str 100EEC3C "B_WindChange_00"
str 100EEC4C "::StateID_InAll"
str 100EEC5C "G_LoopWindChange_00"
str 100EEC70 "LoopWindNow"
str 100EEC7C "N_JpJa_00"
str 100EEC88 "BtnControlWind_00"
str 100EEC9C "::StateID_OutWait"
str 100EECB0 "G_All_00"
str 100EECBC "G_LoopWindNow_00"
str 100EECD0 "LoopWindChange"
str 100EECE0 "ControlWind_00"
str 100EECF0 "B_ControlArea_00"
str 100EED04 "N_WindNow_00"
str 100EED14 "N_ArrowChange_00"
str 100EED28 "B_ControlArea_01"
str 100EED3C "::StateID_Wait"
str 100EED4C "::StateID_Disp"
str 100EED5C "::StateID_OutAll"
ext 0273AF40 void dOw_operatorDelete(u32 a0)
ext 026F88F0 void dOw_layoutScreenDtor(u32 a0, u32 a1)
ext 0257DACC u32 dOw_getWindAngle()
ext 020063C0 void dOw_changeState(u32 a0, u32 a1)
ext 0264D5C0 void dOw_callArrowRotate(u32 a0, f32 a1)
ext 0264D608 void dOw_callCursorRotate(u32 a0, f32 a1)
ext 020053E4 void dOw_startAnimation(u32 a0, u32 a1, u32 a2, f32 a3)
ext 0264D650 void dOw_callInitialize(u32 a0)
ext 02005840 u32 dOw_isAnimationDone(u32 a0, u32 a1)
ext 0257E490 void dOw_tactWindSet(u32 a0, s16 a1)
ext 02617AE4 u32 dOw_isInputBlocked(u32 a0)
ext 0287F05C u32 dOw_containsPoint(u32 a0, u32 a1)
ext 020195B0 u32 dOw_atan2sRegister(f32 a0, f32 a1)
ext 02704D2C void dOw_getPanePosition(u32 a0, u32 a1)
ext 0264F3F4 void dOw_notifyWindChange(u32 a0)
ext 0264D8C8 s32 dOw_callCheckTouchBlocked(u32 a0)
ext 0264D9B0 u32 dOw_callIsTouchHeld(u32 a0)
ext 02030B38 void dOw_playSound(u32 a0)
ext 0264DA14 void dOw_callTouchAngle(u32 a0)
ext 0264DA70 u32 dOw_callIsTouchNearCursor(u32 a0)
ext 0264DAD4 void dOw_callRequestChange(u32 a0)
ext 0264DB10 void dOw_callTouchControl(u32 a0)
ext 0264DCF4 void dOw_callButtonControl(u32 a0)
ext 0271F738 void dOw_addCalcAngle(u32 a0, u32 a1, u32 a2)
ext 0264DD5C void dOw_callArrowDirection(u32 a0)
ext 0264DF8C void dOw_callWindSet(u32 a0)
ext 0273AD10 u32 dOw_operatorNew(u32 a0)
ext 026F89E8 void dOw_layoutScreenCtor(u32 a0)
ext 0258861C u32 dOw_stControlCtor(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f32 a7, f32 a8)
ext 026F91F8 void dOw_layoutScreenExecute(u32 a0)
ext 02006364 void dOw_executeStateManager(u32 a0)
ext 0273B050 u32 dOw_heapNew(u32 a0, u32 a1, u32 a2)
ext 026FADB0 u32 dOw_layoutCtor(u32 a0)
ext 02004E04 void dOw_bindAnimation(u32 a0, u32 a1, u32 a2, u32 a3)
ext 026F90D8 void dOw_setLayoutName(u32 a0, u32 a1, u32 a2)
ext 0264E240 void dOw_callCenterPane(u32 a0)
ext 028F026C void dOw_registerRange(u32 a0)
ext 028ED6F8 void dOw_initializeRangeFlag(u32 a0)
ext 028EAB2C void dOw_initializeScaleFlag(u32 a0)
*/
