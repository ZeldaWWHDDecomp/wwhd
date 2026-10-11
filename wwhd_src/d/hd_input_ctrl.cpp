/** HD controller routing, GamePad orientation and calibrated touch pointer. No GameCube counterpart. */
#include "bindings.h"
#include "d/hd_input_ctrl.h"
static void* sControllerMgr;
static hdInputManager* sInputManager;
static hdInputDisposer* sInputDisposer;
static hdInputManagerTable sInputManagerTable;
static hdInputManagerTable sInputDisposerTable;
static hdControllerRttiTable sPointerSecondTable;
static hdControllerRttiTable sPointerThirdTable;
static hdInputMatrix sIdentityMatrix;
static hdInputVector sOrientationFirst;
static hdInputVector sOrientationSecond;
static hdInputVector sOrientationThird;
static f32 sPointerWidth;
static f32 sPointerHeight;
static f32 sPointerOriginX;
static f32 sPointerOriginY;
static f32 sPointerZero;
static hdInputPosition sInvalidPosition;
static u32 sDrcRttiGuard;
static u32 sDrcRtti;
static u32 sDrcTypeBase;
static u32 sControllerRttiGuard;
static u32 sControllerRtti;
static u32 sControllerTypeBase;
static u32 sPadRttiGuard;
static u32 sPadRtti;
static u32 sPadTypeBase;
static hdInputControllerTypeMap sControllerTypeMap;
static hdInputGame* sGameInfo;
static hdInputStaticBlock sManagerStaticBlock;
static u32 sManagerRegister1;
static f32 sManagerMin;
static f32 sManagerMax;
static hdInputStaticRange sManagerStaticRange;
static u32 sManagerRegister2;
static u32 sManagerRegister3;
static hdInputStaticBlock sPointerStaticBlock;
static u32 sPointerRegister1;
static f32 sPointerMin;
static f32 sPointerMax;
static hdInputStaticRange sPointerStaticRange;
static u32 sPointerRegister2;
static u32 sPointerRegister3;
static hdPointerController* pointerCtor(hdPointerController* p);

// HD: platform RTTI uses guarded type records and two controller base vtables.
static void typeInit(be<u32>* guard, be<u32>* info, void* base) {
    if (!(u32)*guard) {
        *guard = 1;
        *info = gabi::ea(base);
    }
}
static bool isA(hdController* controller, bool secondBase, be<u32>* info) {
    hdControllerRttiTable* table = secondBase ? controller->mControllerVtable : controller->mVtable;
    return gabi::call_ptr<u32>(table->mIsA.get(), controller, info) != 0;
}
static hdController* castDrc(hdController* controller) {
    typeInit(&sDrcRttiGuard, &sDrcRtti, &sDrcTypeBase);
    if (!controller || !isA(controller, false, &sDrcRtti) || !controller) return NULL;
    return controller;
}
static bool drcConnected(hdController* controller) {
    hdDrcController* drc = (hdDrcController*)controller;
    return (s32)drc->mChannels[0].mConnectedCount > 0 && (u8)drc->mChannels[0].mStatus == 0x1F;
}
static hdController* castCtrl(hdController* controller) {
    typeInit(&sControllerRttiGuard, &sControllerRtti, &sControllerTypeBase);
    return isA(controller, true, &sControllerRtti) ? controller : NULL;
}
static void setupPointer(hdInputManager* self, hdController* controller, u32 enabled) {
    hdInputSetPointerController(self->pointer(), controller, enabled);
    hdInputSetPointerParameters(self->pointer(), 0xFF0000u, 0xFu, 5u);
}
/* 02617620 */
hdInputManager* ctor(hdInputManager* p) {
    if (!p) {
        p = (hdInputManager*)operator_new(sizeof(hdInputManager));
        if (!p) return 0;
    }
    hdInputConstructBase(p, p);
    p->mVtable = &sInputManagerTable;
    pointerCtor(p->pointer());
    p->mMode = INPUT_MODE_NONE;
    p->mCursor = NULL;
    hdInputVector* src[3] = {&sOrientationFirst, &sOrientationSecond, &sOrientationThird};
    for (u32 v = 0; v < 3; v++)
        for (u32 k = 0; k < 3; k++) p->mOrientation.mBasis[v].mBits[k] = src[v]->mBits[k];
    for (u32 k = 0; k < 9; k++) p->mOrientation.mCurrent.mBits[k] = sIdentityMatrix.mBits[k];
    p->mOrientation.mSource = NULL;
    for (u32 k = 0; k < 9; k++) p->mOrientation.mCalibration.mBits[k] = sIdentityMatrix.mBits[k];
    for (u32 k = 0; k < 9; k++) p->mOrientation.mInverse.mBits[k] = sIdentityMatrix.mBits[k];
    const f32 w = sPointerWidth, h = sPointerHeight;
    p->mRecalibrationTimer = 0;
    p->mPointerEnabled = 1;
    hdInputSetPointerBounds(p->pointer(), sPointerOriginX, sPointerOriginY, w, h);
    p->mInput = p->pointer();
    return p;
}

// creates the singleton with its disposer
/* 026177B8 */
void createInstance(void* heap) {
    if (sInputManager) return;
    hdInputManager* p = hdInputAllocateSingleton(0x270u, heap, 4u);
    hdInputDisposer* d = p->disposer();
    if (d) {
        hdInputConstructDisposer(d, heap, 3u);
        d->mVtable = &sInputDisposerTable;
    }
    sInputDisposer = d;
    sInputManager = p ? ctor(p) : NULL;
}

// initial setup: GamePad + Pro controllers, the pointer and the cursor object
/* 02617858 */
void hdInputManager::init(u32 unused, void* heap) {
    this->mMode = INPUT_MODE_BOTH;
    void* mgr = sControllerMgr;
    hdController* main = NULL;
    if (mgr) {
        main = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_COMBINED, 0u);
        if (main) {
            hdController* pad = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_PAD, 0u);
            if (pad) hdInputAttachController(castCtrl(main), pad);
            hdController* c = castDrc(hdInputControllerById(sControllerMgr, INPUT_CONTROLLER_DRC));
            if (c) {
                hdDrcStatus* base = ((hdDrcController*)c)->mChannels;
                for (u32 i = 0; i < 4; i++) {
                    hdDrcStatus* e = i < 4 ? &base[i] : base;
                    if ((s32)e->mConnectedCount <= 0 || (u8)e->mStatus != 0x1F) continue;
                    hdController* drc = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_DRC, i);
                    if (!drc) continue;
                    hdInputAttachController(castCtrl(main), drc);
                    setupPointer(this, main, 1);
                    hdInputCursor* cur = hdInputAllocateCursor(0x5Cu, heap, 4u);
                    if (cur) cur = hdInputConstructCursor(cur);
                    this->mCursor = cur;
                    this->mOrientation.mSource = &this->mOrientation.mCurrent;
                    return;
                }
            }
        }
    }
    setupPointer(this, main, 1);
    hdInputCursor* cur = hdInputAllocateCursor(0x5Cu, heap, 4u);
    if (cur) cur = hdInputConstructCursor(cur);
    this->mCursor = cur;
    this->mOrientation.mSource = &this->mOrientation.mCurrent;
}

// mode 0 (TV)?
/* 02617AE4 */
u32 hdInputManager::isTvMode() {
    return this->mMode == INPUT_MODE_TV;
}

// per-frame update
/* 02617AF4 */
void hdInputManager::calc() {
    hdInputPlatformUpdate(1u);
    hdInputPlatformUpdate(2u);
    hdInputPlatformUpdate(3u);
    const s32 mode = s32(this->mMode);
    if (mode < INPUT_MODE_NONE) {
        if (mode == INPUT_MODE_TV || u32(mode) < INPUT_MODE_BOTH) {
            if (mode != INPUT_MODE_TV) hdInputControllerByOrder(sControllerMgr, INPUT_CONTROLLER_PAD, 0u);
            void* mgr = sControllerMgr;
            hdController* c = castDrc(hdInputControllerById(mgr, INPUT_CONTROLLER_DRC));
            if (c && (s32)((hdDrcController*)c)->mChannels[0].mConnectedCount > 0) {
                const u32 b = (u8)((hdDrcController*)c)->mChannels[0].mStatus;
                if (b == 0x1F) {
                    if (hdInputControllerByOrder(mgr, INPUT_CONTROLLER_DRC, 0u) && !this->mPointer.mTouchActive) {
                        u32 n = this->mRecalibrationTimer;
                        if (n > 15) {
                            this->mRecalibrationTimer = 0;
                            hdInputResetPointer(this->pointer(), 1u);
                            n = this->mRecalibrationTimer;
                        }
                        this->mRecalibrationTimer = u8(n + 1);
                    }
                } else if (b != 0xFF && b != 0xFC) {
                    hdInputPlatformUpdate(0u);
                }
            }
        } else if (mode == INPUT_MODE_BOTH) {
            void* mgr = sControllerMgr;
            hdController* main = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_COMBINED, 0u);
            typeInit(&sControllerRttiGuard, &sControllerRtti, &sControllerTypeBase);
            if (main && isA(main, true, &sControllerRtti)) hdInputClearControllers(main);
            else hdInputClearControllers(NULL);
            if (main) {
                hdController* pad = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_PAD, 0u);
                if (pad) hdInputAttachController(castCtrl(main), pad);
                hdController* c = castDrc(hdInputControllerById(mgr, INPUT_CONTROLLER_DRC));
                if (c && drcConnected(c)) {
                    hdController* drc = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_DRC, 0u);
                    if (drc) {
                        hdInputResetPointer(this->pointer(), 1u);
                        hdInputAttachController(castCtrl(main), drc);
                    }
                }
            }
        }
    }
    hdInputUpdateOrientation(&this->mOrientation);
    if (this->isTvMode()) return;
    hdInputPosition pos;
    const f32 z = sPointerZero;
    pos.mX = z;
    pos.mY = z;
    if (this->mPointer.mRepeat & INPUT_BUTTON_TOUCH) {
        const u32 valid = this->mPointer.mPointerFlags & 1;
        pos.mX = valid ? (f32)this->mPointer.mCalibratedX : (f32)sInvalidPosition.mX;
        pos.mY = valid ? (f32)this->mPointer.mCalibratedY : (f32)sInvalidPosition.mY;
    }
    hdInputUpdateCursor(this->mCursor, &pos);
}

// is the GamePad (controller 8) connected?
/* 02617F14 */
u32 hdInputManager::hasPad() {
    hdController* c = hdInputControllerById(sControllerMgr, INPUT_CONTROLLER_PAD);
    typeInit(&sPadRttiGuard, &sPadRtti, &sDrcTypeBase);
    if (!c || !isA(c, false, &sPadRtti) || !c) return 0;
    return s32(c->mConnectedCount) > 0;
}

// is the DRC connected?
/* 02617FCC */
u32 hdInputManager::hasDrc() {
    hdController* c = castDrc(hdInputControllerById(sControllerMgr, INPUT_CONTROLLER_DRC));
    return c && drcConnected(c);
}

// switches the controller mode
/* 02618094 */
u32 hdInputManager::setMode(u32 mode) {
    if (s32(mode) >= INPUT_MODE_NONE) return 0;
    if (mode != INPUT_MODE_TV && mode < INPUT_MODE_BOTH) {
        hdController* pad = hdInputControllerByOrder(sControllerMgr, INPUT_CONTROLLER_PAD, 0u);
        if (!pad) return 0;
        setupPointer(this, pad, 1);
        this->mMode = mode;
        hdInputSetDrcOutput(0u);
        return 1;
    }
    if (mode == INPUT_MODE_TV) {
        void* mgr = sControllerMgr;
        hdController* c = castDrc(hdInputControllerById(mgr, INPUT_CONTROLLER_DRC));
        if (!c || !drcConnected(c)) return 0;
        hdController* drc = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_DRC, 0u);
        if (!drc) return 0;
        setupPointer(this, drc, 1);
        this->mMode = mode;
        hdInputSetDrcOutput(1u);
        return 1;
    }
    if (mode != INPUT_MODE_BOTH) return 0;
    u32 done = 0;
    void* mgr = sControllerMgr;
    hdController* main = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_COMBINED, 0u);
    typeInit(&sControllerRttiGuard, &sControllerRtti, &sControllerTypeBase);
    if (main && isA(main, true, &sControllerRtti)) hdInputClearControllers(main);
    else hdInputClearControllers(NULL);
    if (!main) return 0;
    hdController* pad = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_PAD, 0u);
    if (pad) {
        hdInputAttachController(castCtrl(main), pad);
        done = 1;
        this->mMode = mode;
    }
    hdController* c = castDrc(hdInputControllerById(mgr, INPUT_CONTROLLER_DRC));
    if (c && drcConnected(c)) {
        hdController* drc = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_DRC, 0u);
        if (drc) {
            hdInputAttachController(castCtrl(main), drc);
            this->mMode = mode;
            setupPointer(this, main, 1);
            return 1;
        }
    }
    if (done) setupPointer(this, main, 1);
    return done;
}

/* 02618498 */
u32 hdInputManager::isGamePadMode() { return this->mMode == INPUT_MODE_GAMEPAD; }
/* 026184AC */
u32 hdInputManager::isAlternateGamePadMode() { return this->mMode == INPUT_MODE_GAMEPAD_ALT; }
/* 026184C0 */
u32 hdInputManager::isCombinedMode() { return this->mMode == INPUT_MODE_BOTH; }

// HD: the platform pad and DRC tests intentionally have opposite polarity.
/* 026184D4 */
u32 hdInputManager::isFree(u32 mask) {
    void* mgr = sControllerMgr;
    if (this->hasPad()) {
        hdController* c = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_PAD, 0u);
        if (c && (mask & c->mHold)) return 1;
    }
    if (!this->hasDrc()) return 1;
    hdController* c = hdInputControllerByOrder(mgr, INPUT_CONTROLLER_DRC, 0u);
    if (!c || !(mask & c->mHold)) return 1;
    return 0;
}

// cursor state (5 without a cursor)
/* 026185A0 */
u32 hdInputManager::cursorState() {
    hdInputCursor* c = this->mCursor;
    return c ? (u32)c->mState : 5;
}

// takes the current orientation as the calibration (and its inverse)
/* 026185BC */
void hdInputManager::calibrate() {
    hdInputMatrix* src = this->mOrientation.mSource;
    if (!src) return;
    hdInputCopyMatrix(&this->mOrientation.mCalibration, src);
    hdInputInvertMatrix(&this->mOrientation.mInverse, &this->mOrientation.mCalibration);
}

// the calibrated GamePad orientation (identity on the TV or when gyro aiming is off)
/* 02618604 */
void orientation(hdInputMatrix* out, hdInputManager* self) {
    void* opt = hdInputGetOptions(&sGameInfo->mOptions);
    if (!hdInputGyroEnabled(opt) || self->isTvMode()) {
        for (u32 k = 0; k < 9; k++) out->mBits[k] = sIdentityMatrix.mBits[k];
        return;
    }
    hdInputMatrix m;
    for (u32 k = 0; k < 9; k++) m.mBits[k] = sIdentityMatrix.mBits[k];
    hdInputMatrix* cal = self->mOrientation.mSource;
    if (cal) hdInputMultiplyMatrix(&m, &self->mOrientation.mInverse, cal);
    for (u32 k = 0; k < 9; k++) out->mBits[k] = m.mBits[k];
}

// resets the pointer
/* 02618720 */
void hdInputManager::resetPointer(u32 arg) {
    hdInputResetPointer(this->pointer(), arg);
    hdInputClearPointer(this->pointer());
    this->mRecalibrationTimer = 0;
}

// HD: pointer repeat parameters.
/* 02618760 */
void hdInputManager::pointerParams(u32 a, u32 b) {
    hdInputSetPointerParameters(this->pointer(), 0xFF0000u, a, b);
}
/* 02618774 */
void hdInputManager::pointerDefault() {
    this->pointerParams(0xF, 5);
}
/* 02618780 */
void hdInputManager::pointerParamsMasked(u32 a, u32 b, u32 c) {
    hdInputSetPointerParameters(this->pointer(), a & 0xFF00FFFFu, b, c);
}
/* 0261878C */
void hdInputManager::pointerSlow(u32 a) {
    this->pointerParamsMasked(a, 0x1E, 1);
}

// SingletonDisposer deleting destructor (deletes the instance)
/* 02618798 */
void disposerDtor(hdInputDisposer* p, u32 flags) {
    if (!p) return;
    p->mVtable = &sInputDisposerTable;
    if (p == sInputDisposer) {
        hdInputManager* inst = sInputManager;
        sInputDisposer = NULL;
        gabi::call_ptr(inst->mVtable->mDestroy.get(), inst, 2u);
        sInputManager = NULL;
    }
    hdInputDestroyDisposer(p, 0u);
    if (flags & 1) operator_delete(p);
}

static void stdInit(hdInputStaticBlock* block, void* firstRegister, be<f32>* low, be<f32>* high, hdInputStaticRange* range, void* secondRegister, void* thirdRegister) {
    block->mWords[2]=0; block->mWords[0]=0; block->mWords[3]=0; block->mWords[1]=0;
    hdInputConstructRegister(firstRegister);
    const f32 min=*low, max=*high;
    range->mMin = min;
    range->mMax = max;
    hdInputConstructFirstFlag(&range->mFirst);
    hdInputConstructRegister(secondRegister);
    hdInputConstructSecondFlag(&range->mSecond);
    hdInputConstructRegister(thirdRegister);
}
/* 02618840 */
void staticInit() {
    stdInit(&sManagerStaticBlock, &sManagerRegister1, &sManagerMin, &sManagerMax, &sManagerStaticRange, &sManagerRegister2, &sManagerRegister3);
}

// HD: compiler-generated and template code of this translation unit.
// manager deleting destructor
/* 026188D4 */
void dtor(hdInputManager* p, u32 flags) {
    if (!p) return;
    p->mPointer.mVtableSecond = &sPointerSecondTable;
    hdInputDestroyPointerBase(p->pointer(), 0u);
    hdInputDestroyManagerBase(p, 0u);
    if (flags & 1) operator_delete(p);
}

/* 02618940 */
void nop() { }

// thunk of the second base (this - 0x130)
/* 02618D10 */
void thunk(void* p, u32 a) {
    hdInputPointerSecondBaseOperation(hdInputPointerFromSecondBase(p), a);
}

// pointer controller type (4 when both controllers are active)
/* 02618D18 */
u32 hdPointerController::controllerType() {
    if (sInputManager->isTvMode()) {
        if (!sInputManager->hasDrc()) return this->mControllerType;
        const u32 r = hdInputPlatformControllerType(0u);
        this->mControllerType = r;
        return r;
    }
    if (sInputManager->isCombinedMode()) return 4;
    if (!sInputManager->hasPad()) return this->mControllerType;
    hdController* c = hdInputControllerById(sControllerMgr, INPUT_CONTROLLER_PAD);
    typeInit(&sPadRttiGuard, &sPadRtti, &sPadTypeBase);
    if (!c || !isA(c, false, &sPadRtti)) c = 0;
    // HD: the original reads the null guest controller after a failed RTTI check.
    if (hdInputControllerConnectedCount(c) <= 0) return this->mControllerType;
    const u32 r = (u32)sControllerTypeMap.mTypes[c->platformType()];
    this->mControllerType = r;
    return r;
}

// pointer controller constructor
/* 02618E74 */
hdPointerController* pointerCtor(hdPointerController* p) {
    if (!p) {
        p = (hdPointerController*)operator_new(sizeof(hdPointerController));
        if (!p) return 0;
    }
    hdInputConstructPointerBase(p);
    p->mVtableThird = &sPointerThirdTable;
    p->mVtableSecond = &sPointerSecondTable;
    p->mControllerType = 4;
    return p;
}

static void setBit(be<u32>* dst, bool on, u32 bit) {
    const u32 v = (u32)*dst;
    *dst = on ? v | bit : v & ~bit;
}

/* the pointer position: calibration offset and scale, integer copy */
static void pointerPos(hdPointerController* self, u32 flags) {
    const f32 c8 = self->mOffsetY, y = self->mCalibratedY, c4 = self->mOffsetX;
    const f32 dy = gabi::fsubs_ppc(y, c8);
    const f32 x = self->mCalibratedX, s0 = self->mScaleY;
    const f32 dx = gabi::fsubs_ppc(x, c4);
    const f32 sx = self->mScaleX;
    const f32 py = gabi::fmuls_ppc(dy, s0);
    const f32 px = gabi::fmuls_ppc(dx, sx);
    self->mPointerFlags = flags;
    self->mCalibratedX = px;
    self->mPointerPixelX = u32(gabi::ftoi(px));
    self->mPointerPixelY = u32(gabi::ftoi(py));
    self->mCalibratedY = py;
}

// pointer update: the GamePad touch as pointer when enabled, else cleared
/* 02618EDC */
void hdPointerController::pointerUpdate(u32 a, u32 b) {
    hdInputUpdatePointerBase(this, a, b);
    hdController* pad = hdInputControllerByOrder(sControllerMgr, INPUT_CONTROLLER_PAD, 0u);
    hdInputManager* m = sInputManager;
    if (m->mPointerEnabled && m->hasPad() && pad) {
        this->mCalibratedX = pad->mPointerX;
        this->mCalibratedY = pad->mPointerY;
        setBit(&this->mRepeat, pad->mRepeat & INPUT_BUTTON_TOUCH, INPUT_BUTTON_TOUCH);
        setBit(&this->mHold, pad->mHold & INPUT_BUTTON_TOUCH, INPUT_BUTTON_TOUCH);
        setBit(&this->mTrigger, pad->mTrigger & INPUT_BUTTON_TOUCH, INPUT_BUTTON_TOUCH);
        setBit(&this->mPointerFlags, pad->mPointerFlags & 1, 1);
        setBit(&this->mPointerFlags, pad->mPointerFlags & 2, 2);
        const u32 f = this->mPointerFlags;
        if (pad->mPointerFlags & 4) {
            pointerPos(this, f | 4);
            return;
        }
        pointerPos(this, f & ~4u);
        return;
    }
    const u32 h = this->mRepeat;
    const f32 x = (f32)sInvalidPosition.mX;
    const u32 w0 = this->mHold;
    this->mCalibratedX = x;
    const f32 y = (f32)sInvalidPosition.mY;
    this->mRepeat = h & ~u32(INPUT_BUTTON_TOUCH);
    this->mCalibratedY = y;
    const u32 w4 = (u32)this->mTrigger;
    const u32 fl = this->mPointerFlags;
    this->mHold = w0 & ~u32(INPUT_BUTTON_TOUCH);
    this->mTrigger = w4 & ~u32(INPUT_BUTTON_TOUCH);
    pointerPos(this, fl & ~7u);
}

/* 0261918C */
void staticInit2() {
    stdInit(&sPointerStaticBlock, &sPointerRegister1, &sPointerMin, &sPointerMax, &sPointerStaticRange, &sPointerRegister2, &sPointerRegister3);
}

/* WWHD glue
data sControllerMgr 101F8AE8
data sInputManager 101F5088
data sInputDisposer 101F508C
data sInputManagerTable 100E29BC
data sInputDisposerTable 100E29E0
data sPointerSecondTable 100E2A78
data sPointerThirdTable 100E2A38
data sIdentityMatrix 104A03C8
data sOrientationFirst 104A0934
data sOrientationSecond 104A0940
data sOrientationThird 104A094C
data sPointerWidth 100E29D0
data sPointerHeight 100E29D4
data sPointerOriginX 10143588
data sPointerOriginY 1014358C
data sPointerZero 100E2984
data sInvalidPosition 1049FFA0
data sDrcRttiGuard 101FD698
data sDrcRtti 101FD978
data sDrcTypeBase 100E29AC
data sControllerRttiGuard 101FD66C
data sControllerRtti 101FDD74
data sControllerTypeBase 100E299C
data sPadRttiGuard 101FD694
data sPadRtti 101FD968
data sPadTypeBase 100E2A00
data sControllerTypeMap 100E2A10
data sGameInfo 101F84DC
data sManagerStaticBlock 1048DE74
data sManagerRegister1 101F5064
data sManagerMin 100E29D8
data sManagerMax 100E29DC
data sManagerStaticRange 1048DE68
data sManagerRegister2 101F5070
data sManagerRegister3 101F507C
data sPointerStaticBlock 1048DE90
data sPointerRegister1 101F5090
data sPointerMin 100E2A2C
data sPointerMax 100E2A30
data sPointerStaticRange 1048DE84
data sPointerRegister2 101F509C
data sPointerRegister3 101F50A8

ext 0273B0D4 hdInputManager* hdInputAllocateSingleton(u32, void*, u32)
ext 020011CC void hdInputConstructBase(hdInputManager*, hdInputManager*)
ext 02001924 void hdInputSetPointerBounds(hdPointerController*, f32, f32, f32, f32)
ext 02752B0C void hdInputConstructDisposer(hdInputDisposer*, void*, u32)
ext 0273CB44 hdController* hdInputControllerByOrder(void*, u32, u32)
ext 0273CB8C hdController* hdInputControllerById(void*, u32)
ext 0273D394 void hdInputAttachController(hdController*, hdController*)
ext 0273D248 void hdInputSetPointerController(hdPointerController*, hdController*, u32)
ext 0273BDF4 void hdInputSetPointerParameters(hdPointerController*, u32, u32, u32)
ext 0273B050 hdInputCursor* hdInputAllocateCursor(u32, void*, u32)
ext 0261A198 hdInputCursor* hdInputConstructCursor(hdInputCursor*)
ext C00083C0 void hdInputPlatformUpdate(u32)
ext 0273D2C0 void hdInputResetPointer(hdPointerController*, u32)
ext 0273D42C void hdInputClearControllers(hdController*)
ext 026173B0 void hdInputUpdateOrientation(hdInputOrientation*)
ext 0261A9B8 void hdInputUpdateCursor(hdInputCursor*, hdInputPosition*)
ext 02030AD8 void hdInputSetDrcOutput(u32)
ext 02618944 void hdInputCopyMatrix(hdInputMatrix*, hdInputMatrix*)
ext 02618B3C void hdInputInvertMatrix(hdInputMatrix*, hdInputMatrix*)
ext 027200D0 void* hdInputGetOptions(void*)
ext 0271FC30 u32 hdInputGyroEnabled(void*)
ext 02618990 void hdInputMultiplyMatrix(hdInputMatrix*, hdInputMatrix*, hdInputMatrix*)
ext 0273D2C8 void hdInputClearPointer(hdPointerController*)
ext 02752BEC void hdInputDestroyDisposer(hdInputDisposer*, u32)
ext 028F026C void hdInputConstructRegister(void*)
ext 028ED6F8 void hdInputConstructFirstFlag(be<u8>*)
ext 028EAB2C void hdInputConstructSecondFlag(be<u8>*)
ext 02001728 void hdInputDestroyPointerBase(hdPointerController*, u32)
ext 020011B8 void hdInputDestroyManagerBase(hdInputManager*, u32)
ext 02619220 void hdInputPointerSecondBaseOperation(hdPointerController*, u32)
ext C0008420 u32 hdInputPlatformControllerType(u32)
ext 02001790 void hdInputConstructPointerBase(hdPointerController*)
ext 0200188C void hdInputUpdatePointerBase(hdPointerController*, u32, u32)
*/
