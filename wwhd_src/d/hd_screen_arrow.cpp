/* HD arrow-type icon: two element particles and Wait/Disp/Change layout states.
 * GameCube-style source verified as hd_screen_arrow; no GameCube counterpart. */
#include "d/hd_screen_arrow.h"

static const u32 screenVtable = 0x100E2D28;
static const u32 firstBaseVtable = 0x100E2E38;
static const u32 secondBaseVtable = 0x100E2E48;
static const u32 safeStringVtable = 0x100E2BBC;
static const u32 stateVtable = 0x100E2C28;
static hdArrow_state_c sWait;
static hdArrow_state_c sDisp;
static hdArrow_state_c sChange;
static hdArrow_string_c sAnimationNames[3] = {};
static hdArrow_string_c sAnimationPanes[3] = {};
static hdArrow_string_c sPaneNames[2] = {};
static hdArrow_string_c sScreenName;
static u32 sStateCounterReady;
static u32 sStateCounter;
static u8 sStateContext;
static hdArrow_heapManagerPointer_c sHeapManager;
static u16 sParticleIds[3] = {737, 738, 751};
static u32 sAnimationPaneIndices[3] = {1, 2, 0};
static f32 sAnimationStart = 1.0f;
static f32 sZero = 0.0f;

enum hdArrow_emitterFlags_e { EMITTER_STOP = 1, EMITTER_KILL = 4, EMITTER_ATTACHED = 0x40 };

/* inlined into particle transitions */
static void hdArrow_stopEmitter(gptr<hdArrow_emitter_c>* slot) {
    hdArrow_emitter_c* emitter = *slot;
    emitter->mFlags = u32(emitter->mFlags) | EMITTER_KILL;
    emitter = *slot;
    emitter->mMaxFrame = -1;
    emitter->mFlags = u32(emitter->mFlags) | EMITTER_STOP;
    emitter = *slot;
    emitter->mFlags = u32(emitter->mFlags) & ~u32(EMITTER_ATTACHED);
}

/* inlined into setType and changeInit */
static void hdArrow_spawn(u32 type, cXyz* position, gptr<hdArrow_emitter_c>* slot) {
    u32 index = u8(type - 1);
    if (index >= 3) index = 0;
    u32 id = sParticleIds[index];
    dPa_control_c* particles = dComIfGp_getParticle();
    *slot = hdArrow_spawnEmitter(particles, 7, id, position, 0, 0, 255, 0, 0xFFFFFFFF, 0, 0, 0);
}

/* 0261AC84 */
static void hdArrow_destroy(hdArrow_screen_c* i_this, u32 flags) {
    if (!i_this) return;
    i_this->mFirstBaseVtable = firstBaseVtable;
    i_this->mSecondBaseVtable = secondBaseVtable;
    hdArrow_destroyBase(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}

/* 0261ACF0 */
void hdArrow_screen_c::killParticles() {
    for (u32 i = 0; i < 2; ++i) {
        if (!mpEmitter[i]) continue;
        hdArrow_stopEmitter(&mpEmitter[i]);
        mpEmitter[i] = nullptr;
    }
    mShown = 0;
}

/* 0261AE00 */
void hdArrow_screen_c::hide() {
    if (!mShown) return;
    mDisplayRequested = 0;
    hdArrow_changeState(&mpStateMachine, &sDisp);
}

/* 0261AE28 */
void hdArrow_screen_c::setType() {
    u32 type = hdArrow_arrowType();
    mArrowType = u8(type);
    hdArrow_setAnimationFrame(animation(), 0, 1, f32(f64(type)));
    hdArrow_setAnimationFrame(animation(), 1, 2, f32(f64(u8(mArrowType))));
    if (mpEmitter[0]) hdArrow_stopEmitter(&mpEmitter[0]);
    mpEmitter[0] = mpEmitter[1];
    mpEmitter[1] = nullptr;
    type = mArrowType;
    if (type && !mpEmitter[0]) hdArrow_spawn(type, &mParticlePos, &mpEmitter[0]);
}

/* 0261AFA4 */
void hdArrow_screen_c::waitMove() {
    if (!mShown) hdArrow_changeState(&mpStateMachine, &sWait);
    else if (u32 type = hdArrow_arrowType(); mArrowType != type) hdArrow_changeState(&mpStateMachine, &sChange);
    gabi::call_ptr<void>(mVtable->mUpdate, this);
}

/* 0261B020 */
void hdArrow_screen_c::changeInit() {
    hdArrow_startAnimation(animation(), 2, 0, sAnimationStart);
    hdArrow_setAnimationFrame(animation(), 0, 1, f32(f64(u8(mArrowType))));
    u32 type = hdArrow_arrowType();
    mArrowType = u8(type);
    hdArrow_setAnimationFrame(animation(), 1, 2, f32(f64(type)));
    if (mpEmitter[1]) {
        mpEmitter[0] = mpEmitter[1];
        mpEmitter[1] = nullptr;
        hdArrow_emitter_c* emitter = mpEmitter[0];
        f32 x = mParticlePos.x;
        f32 y = mParticlePos.y;
        f32 z = mParticlePos.z;
        if (emitter->mCoordinateMode >= 7) y = -y;
        emitter->mGlobalTranslation.x = x;
        emitter->mGlobalTranslation.y = y;
        emitter->mGlobalTranslation.z = z;
    }
    if (!mArrowType) return;
    gabi::Local<hdArrow_changeFrame_c> frame;
    hdArrow_panePosition(&frame->mPanePosition.mPosition, mpPane[1]);
    type = mArrowType;
    // HD: the second pane position is copied to the spawn slot in z/y/x store order.
    frame->mSpawnPosition.z = f32(frame->mPanePosition.mPosition.z);
    frame->mSpawnPosition.y = f32(frame->mPanePosition.mPosition.y);
    frame->mSpawnPosition.x = f32(frame->mPanePosition.mPosition.x);
    hdArrow_spawn(type, &frame->mSpawnPosition, &mpEmitter[1]);
}

/* 0261B1C8 */
void hdArrow_screen_c::changeMove() {
    for (u32 i = 0; i < 2; ++i) {
        if (!mpEmitter[i]) continue;
        gabi::Local<hdArrow_positionFrame_c> position;
        hdArrow_panePosition(&position->mPosition, mpPane[i]);
        hdArrow_emitter_c* emitter = mpEmitter[i];
        f32 x = position->mPosition.x;
        f32 y = position->mPosition.y;
        f32 z = position->mPosition.z;
        if (emitter->mCoordinateMode >= 7) y = -y;
        emitter->mGlobalTranslation.y = y;
        emitter->mGlobalTranslation.z = z;
        emitter->mGlobalTranslation.x = x;
        emitter = mpEmitter[i];
        emitter->mGlobalAlpha = u8(mpPane[i]->mAlpha);
    }
    if (!mShown) hdArrow_changeState(&mpStateMachine, &sWait);
    else if (u32 type = hdArrow_arrowType(); mArrowType != type) hdArrow_changeState(&mpStateMachine, &sChange);
    else if (hdArrow_animationDone(animation(), 2)) hdArrow_changeState(&mpStateMachine, &sDisp);
    gabi::call_ptr<void>(mVtable->mUpdate, this);
}

/* 0261B370 */
void hdArrow_screen_c::killCurrent() {
    if (!mpEmitter[0]) return;
    hdArrow_stopEmitter(&mpEmitter[0]);
    mpEmitter[0] = nullptr;
}

/* 0261B3C4 */
static hdArrow_screen_c* hdArrow_construct(hdArrow_screen_c* i_this) {
    if (!i_this) {
        i_this = (hdArrow_screen_c*)operator_new(sizeof(hdArrow_screen_c));
        if (!i_this) return nullptr;
    }
    hdArrow_constructBase(i_this);
    i_this->mVtable.v = screenVtable;
    i_this->mSecondBaseVtable = secondBaseVtable;
    i_this->mFirstBaseVtable = firstBaseVtable;
    // HD: GHS checks embedded-array addresses after unsigned guest wraparound.
    if (!gabi::ea(i_this->mpPane)) operator_new(sizeof(i_this->mpPane));
    if (!gabi::ea(i_this->mpEmitter)) operator_new(sizeof(i_this->mpEmitter));
    f32 zero = sZero;
    i_this->mShown = 0;
    i_this->mParticlePos.z = zero;
    i_this->mDisplayRequested = 0;
    i_this->mParticlePos.x = zero;
    i_this->mArrowType = 0;
    i_this->mParticlePos.y = zero;
    for (u32 i = 0; i < 2; ++i) {
        i_this->mpPane[i] = nullptr;
        i_this->mpEmitter[i] = nullptr;
    }
    return i_this;
}

/* 0261B4E4 */
u32 hdArrow_screen_c::setup(u32 a, u32 b, u32 c) {
    if (!hdArrow_setupBase(this, &sScreenName, a, b, c)) return 0;
    hdArrow_pane_c* root = mpLayout->mpInfo->mpRoot;
    for (u32 i = 0; i < 2; ++i) {
        hdArrow_string_c* name = &sPaneNames[i];
        u32 assureTermination = name->mVtable->mAssureTermination;
        hdArrow_paneVtable_c* vtable = root->mVtable;
        gabi::call_ptr<void>(assureTermination, name);
        mpPane[i] = gabi::call_ptr<hdArrow_pane_c*>(vtable->mFindPane, root, (be<u8>*)name->mStringTop, 1u);
    }
    gabi::Local<hdArrow_positionFrame_c> position;
    hdArrow_panePosition(&position->mPosition, mpPane[0]);
    hdArrow_machine_c* machine = mpStateMachine;
    mParticlePos.x = f32(position->mPosition.x);
    mParticlePos.z = f32(position->mPosition.z);
    mParticlePos.y = f32(position->mPosition.y);
    hdArrow_action_c* action = gabi::call_ptr<hdArrow_action_c*>(machine->mVtable->mSetState, machine, &sWait);
    mpCurrentState = action;
    gabi::call_ptr<void>(action->mVtable->mEnter, action);
    return 1;
}

/* 0261B628 */
void hdArrow_screen_c::hideParticles() {
    hdArrow_hideBase(this);
    for (u32 i = 0; i < 2; ++i) {
        if (!mpEmitter[i]) continue;
        hdArrow_emitter_c* emitter = mpEmitter[i];
        emitter->mMaxFrame = -1;
        emitter->mFlags = u32(emitter->mFlags) | EMITTER_STOP;
        emitter = mpEmitter[i];
        emitter->mFlags = u32(emitter->mFlags) & ~u32(EMITTER_ATTACHED);
    }
}

/* 0261B70C */
void hdArrow_screen_c::stateExecute() {
    hdArrow_executeState(&mpStateMachine);
}

/* 0261B714 */
void hdArrow_screen_c::toWait() {
    hdArrow_changeState(&mpStateMachine, &sWait);
}

/* 0261B724 */
u32 hdArrow_screen_c::loadScreen(u32 a, u32 b, u32 c, u32 d, u32 e) {
    hdArrow_layout_c* layout = hdArrow_allocLayout(sizeof(hdArrow_layout_c), sHeapManager.mpManager->mpHeap, 4);
    if (layout) layout = hdArrow_constructLayout(layout);
    mpLayout = layout;
    if (!layout) return 0;
    if (!gabi::call_ptr<u32>(layout->mVtable->mLoad, layout, a, b, c, d, 3u, 3u, e)) return 0;
    for (u32 i = 0; i < 3; ++i)
        hdArrow_bindAnimation(animation(), i, &sAnimationNames[i], &sAnimationPanes[sAnimationPaneIndices[i]]);
    return 1;
}

/* ---------------- HD: compiler-generated and template code ---------------- */

/* 0261B83C */
static void __sinit_hd_screen_arrow_cpp() {
    sinit_header_statics_rodata(0x1048DF04, 0x101F5128, 0x1048DF10, 0x100E2C84);
    sAnimationNames[0].setVtable(safeStringVtable);
    sAnimationPanes[0].setVtable(safeStringVtable);
    sAnimationPanes[0].setText("G_ArrowAll_00");
    sAnimationPanes[1].setVtable(safeStringVtable);
    sAnimationPanes[1].setText("G_Arrow_00");
    sAnimationPanes[2].setVtable(safeStringVtable);
    sScreenName.setVtable(safeStringVtable);
    sScreenName.setText("Arrow_00");
    sAnimationNames[0].setText("ArrowType");
    sAnimationNames[1].setVtable(safeStringVtable);
    sAnimationNames[1].setText("ArrowType");
    sAnimationNames[2].setVtable(safeStringVtable);
    u32 guard = sStateCounterReady;
    sPaneNames[0].setVtable(safeStringVtable);
    sAnimationNames[2].setText("ArrowChange");
    sAnimationPanes[2].setText("G_Arrow_01");
    sPaneNames[0].setText("P_Arrow_00");
    sPaneNames[1].setVtable(safeStringVtable);
    sPaneNames[1].setText("P_Arrow_01");
    u32 id;
    if (guard) id = sStateCounter;
    else {
        id = 0;
        sStateCounterReady = 1;
    }
    sWait.mEnter.mThisDelta = 0;
    sWait.mEnter.mVirtualIndex = 0x19;
    sWait.mMove.mThisDelta = 0;
    sWait.mId = id + 1;
    sWait.mMove.mVirtualIndex = 0x1A;
    sDisp.mId = id + 2;
    sDisp.mEnter.mThisDelta = 0;
    sWait.mLeave.mThisDelta = 0;
    sWait.mLeave.mVirtualIndex = 0x1B;
    sWait.setName("::StateID_Wait");
    sWait.mEnter.mFunction = 4;
    sDisp.setName("::StateID_Disp");
    sChange.mEnter.mThisDelta = 0;
    sChange.mEnter.mVirtualIndex = 0x1F;
    sDisp.mEnter.mFunction = 4;
    sDisp.mMove.mFunction = 4;
    sWait.mMove.mFunction = 4;
    sDisp.mLeave.mFunction = 4;
    sDisp.mEnter.mVirtualIndex = 0x1C;
    sDisp.mUnused.mFunction = 0;
    sDisp.mMove.mThisDelta = 0;
    sDisp.mMove.mVirtualIndex = 0x1D;
    sDisp.mLeave.mThisDelta = 0;
    sWait.mLeave.mFunction = 4;
    sWait.mUnused.mFunction = 0;
    sDisp.mLeave.mVirtualIndex = 0x1E;
    sWait.mUnused.mThisDelta = 0;
    sChange.mMove.mThisDelta = 0;
    sChange.mId = id + 3;
    sChange.setName("::StateID_Change");
    sDisp.mUnused.mThisDelta = 0;
    sWait.mUnused.mVirtualIndex = 0;
    sWait.mVtable = stateVtable;
    sDisp.mVtable = stateVtable;
    sDisp.mUnused.mVirtualIndex = 0;
    sDisp.mContext = &sStateContext;
    sWait.mContext = &sStateContext;
    sChange.mEnter.mFunction = 4;
    sChange.mMove.mFunction = 4;
    sChange.mLeave.mFunction = 4;
    sChange.mUnused.mFunction = 0;
    sChange.mVtable = stateVtable;
    sChange.mContext = &sStateContext;
    sChange.mMove.mVirtualIndex = 0x20;
    sChange.mLeave.mThisDelta = 0;
    sChange.mLeave.mVirtualIndex = 0x21;
    sChange.mUnused.mThisDelta = 0;
    sStateCounter = id + 3;
    sChange.mUnused.mVirtualIndex = 0;
}

/* 0261BB00 */
static void hdArrow_waitEnter() {}
/* 0261BB04 */
static void hdArrow_waitLeave() {}
/* 0261BB10 */
static void hdArrow_dispEnter() {}
/* 0261BB14 */
static void hdArrow_dispLeave() {}
/* 0261BB18 */
static void hdArrow_changeLeave() {}

/* WWHD glue
data sWait 1048DF60 nocheck
data sDisp 1048DF90 nocheck
data sChange 1048DFC0 nocheck
data sAnimationNames 1048DF38 nocheck
data sAnimationPanes 1048DF20 nocheck
data sPaneNames 1048DF50 nocheck
data sScreenName 1048DFF0 nocheck
data sStateCounterReady 101FD8E4 nocheck
data sStateCounter 101FDD50 nocheck
data sStateContext 101FF32C nocheck
data sHeapManager 1018C404 nocheck
data sParticleIds 100E2BB4
data sAnimationPaneIndices 100E2BD4
data sAnimationStart 100E2C70
data sZero 100E2C80
str 100E2C98 "G_ArrowAll_00" in 0261B83C
str 100E2CB4 "G_Arrow_00" in 0261B83C
str 100E2CC0 "G_Arrow_01" in 0261B83C
str 100E2CA8 "ArrowType" in 0261B83C
str 100E2C8C "ArrowChange" in 0261B83C
str 100E2CCC "P_Arrow_00" in 0261B83C
str 100E2CD8 "P_Arrow_01" in 0261B83C
str 100E2CE4 "Arrow_00" in 0261B83C
str 100E2CF0 "::StateID_Wait" in 0261B83C
str 100E2D00 "::StateID_Disp" in 0261B83C
str 100E2D10 "::StateID_Change" in 0261B83C
ext 02700A70 void hdArrow_destroyBase(hdArrow_screen_c* screen, u32 flags)
ext 02700ADC void hdArrow_constructBase(hdArrow_screen_c* screen)
ext 02526C9C u32 hdArrow_arrowType()
ext 0200552C void hdArrow_setAnimationFrame(void* animation, u32 track, u32 mode, f32 frame)
ext 020053E4 void hdArrow_startAnimation(void* animation, u32 track, u32 flags, f32 frame)
ext 02005840 u32 hdArrow_animationDone(void* animation, u32 track)
ext 025A847C hdArrow_emitter_c* hdArrow_spawnEmitter(dPa_control_c* manager, u32 group, u32 id, const cXyz* pos, u32 angle, u32 scale, u32 alpha, u32 callback, u32 setup, u32 primary, u32 environment, u32 scale2D)
ext 02704D2C void hdArrow_panePosition(cXyz* out, hdArrow_pane_c* pane)
ext 02700B48 u32 hdArrow_setupBase(hdArrow_screen_c* screen, hdArrow_string_c* name, u32 a, u32 b, u32 c)
ext 026F8B00 void hdArrow_hideBase(hdArrow_screen_c* screen)
ext 02006364 void hdArrow_executeState(gptr<hdArrow_machine_c>* machine)
ext 020063C0 void hdArrow_changeState(gptr<hdArrow_machine_c>* machine, hdArrow_state_c* state)
ext 0273B050 hdArrow_layout_c* hdArrow_allocLayout(u32 size, void* heap, u32 alignment)
ext 026FADB0 hdArrow_layout_c* hdArrow_constructLayout(hdArrow_layout_c* layout)
ext 02004E04 void hdArrow_bindAnimation(void* animation, u32 track, hdArrow_string_c* name, hdArrow_string_c* archive)
*/
