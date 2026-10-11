/** Regular knob doors, WWHD. */
#include "d/actor/d_a_knob00.h"
#include "bindings.h"
#include "d/d_item.h"
#include "gc_api/d/d_com_inf_game.h"
BOOL knobCheckArea(daKnob00_c*, f32, f32, f32);
s32 knobDayNight();
void knobCutEnd(dEvent_manager_c*, s32);
BOOL knobEndCheck(dEvent_manager_c*, s16);
BOOL knobRegisterBg(dBgS*, dBgW*, fopAc_ac_c*);
void knobAssert(const char*, u32, const char*);
void knobReleaseBg(dBgS*, dBgW*);
void knobFadeOut(GXColor*, f32);
s32 knobReverb(s32);
void knobStartSound(u32, cXyz*, u32, s32);
BOOL knobCheckItem(u32);
void knobResDelete(request_of_phase_process_class*, const char*);
void knobCreateActor(u32, u32, cXyz*, s8, csXyz*, void*, u32, void*);
s32 knobResLoad(request_of_phase_process_class*, const char*);
void knobConstructActor(daKnob00_c*);
void knobConstructMessage(KnobMessageWork*);
void knobInitFrame(J3DFrameCtrl*, u32);
void knobConstructBinding(KnobAnimationBinding*);
void knobConstructStop(KnobStopBars*);
BOOL knobEnterHeap(daKnob00_c*, u32, u32);
s32 knobCheckExecute(daKnob00_c*);
void knobStartDemo(daKnob00_c*);
BOOL knobIsAdvance(dEvent_manager_c*, s32);
f32 knobRandom(f32);
void knobDestroyMatrixCalc(KnobMatrixCalc*, u32);
void knobDestroyMessage(KnobMessageWork*, u32);
void knobDestroyActor(daKnob00_c*, u32);
KnobGameInfo* knobGameInfo();
void knobTranslate(Mtx34*, f32, f32, f32);
void knobRotateY(Mtx34*, s16);
BOOL knobDrawCheck(daKnob00_c*, s32);
void knobModelUpdateDL(J3DModel*, u32);
KnobMt* knobSearchMt(const char*, u32, u32);
u32 knobSetTalkPartner(dEvt_control_c*, KnobMt*);
KnobShip* knobSearchByID(u32, be<u32>*);
BOOL knobIsActor(fopAc_ac_c*);
BOOL knobSetBackground(dBgW*, void*, u32, Mtx34*);
void knobCopyMemory(void*, void*, u32, u32);
void knobPasswordGet(void*, be<u16>*, s32);
static void emptyString(void*);
static BOOL CheckCreateHeap(daKnob00_c*);
static KnobSave* sSave;
static void* sMessageManager;
static Mtx34 sMatrix;
static KnobTrigTable sTrig;
static KnobAnimationTable sAnimationResources;
static KnobShapeResources sShapeResources;
static f32 sFadeRate;
static GXColor sWhiteColor;
static GXColor sBlackColor;
static u8 sMapSendStop;
static s8 sStayRoom;

static void resetEvent() {
    auto* game = knobGameInfo();
    game->mEvent.mEventFlag = (u16)game->mEvent.mEventFlag | 8;
}

static BOOL eventBit(u16 bit) { return dComIfGs_isEventBit(bit); }
static BOOL symbol(s32 bit) { return dComIfGs_isSymbol(bit); }

static void releaseBg(daKnob00_c* a) {
    auto* p = knobGameInfo();
    dBgW* bg = a->mpBgW;
    knobReleaseBg(&p->mBgS, bg);
}

static void cutEnd(daKnob00_c* a) {
    s32 staff = a->mStaffId;
    KnobGameInfo* p = knobGameInfo();
    knobCutEnd(&p->mEventManager, staff);
}

static void onEventBit(u16 bit) { dComIfGs_onEventBit(bit); }

/* 021A37A4 */
static BOOL CheckCreateHeap(daKnob00_c* a) { return a->CreateHeap(); }

/* 021A34F0 */
BOOL daKnob00_c::CreateHeap() {
    auto* data = (J3DModelData*)dComIfG_getObjectRes("Knob", 9, 0x100132F8);
    if (!data)
        knobAssert("d_a_knob00.cpp", 0xA6, "modelData != (0)");
    J3DModel* model = mDoExt_J3DModel__create(data, 0, 0x11020203);
    mpModel = model;
    if (!model)
        return FALSE;
    f32 x = scale.x, z = scale.z, y = scale.y;
    model->mBaseScale.x = x;
    model->mBaseScale.z = z;
    model->mBaseScale.y = y;
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    knobTranslate(&sMatrix, x, y, z);
    knobRotateY(&sMatrix, (s16)current.angle.y);
    model = mpModel;
    J3DModel::mtx_assign(&model->mBaseTransformMtx, &sMatrix);
    auto* animation = (J3DAnmTransform*)dComIfG_getObjectRes("Knob", 5, 0x100132F8);
    if (!mBckAnm.init(data, animation, true, 0, 1.0f, 0, -1, false))
        return FALSE;
    s32 joint = data->getJointName()->getIndex("DoorDummy");
    m_jnt = (s8)joint;
    if ((s8)joint < 0)
        knobAssert("d_a_knob00.cpp", 0xC2, "m_jnt >= 0");
    u8 shape = getShapeType();
    s32 file = (shape >= 1 && shape <= 7) ? (u8)sShapeResources.mResources[shape] : 10;
    data = (J3DModelData*)dComIfG_getObjectRes("Knob", file, 0x100132F8);
    model = mDoExt_J3DModel__create(data, 0x80000, 0x11020002);
    mpModel2 = model;
    if (!model)
        return FALSE;
    dBgW* background = new_dBgW();
    mpBgW = background;
    if (!background)
        return FALSE;
    void* bgData = dComIfG_getObjectRes("Knob", 20, 0x100132F8);
    if (!bgData)
        return FALSE;
    calcMtx();
    model = mpModel;
    background = mpBgW;
    return knobSetBackground(background, bgData, 1, model ? &model->mBaseTransformMtx : nullptr) ==
           0;
}

/* 021A3430 */
u8 daKnob00_c::getShapeType() { return (u8)getSwbit2(); }

/* 021A4318 */
void daKnob00_c::setEventPrm() {
    u8 action = mAction;
    m2C6 = mFrontCheck == 0 ? 7 : 8;
    if (action == 4 || action == 5 || action == 8 || action == 10)
        m2C6 = 9;
    if (!knobCheckArea(this, 6400.0f, 12100.0f, 62500.0f)) {
        mFlags = (u8)mFlags & 0xFB;
    } else {
        u8 event = m2C6;
        u16 flags = (u16)eventInfo.mCondition;
        eventInfo.mEventId = (s16)mEventIdx[event];
        u8 tool = (u8)mToolId[event];
        eventInfo.mCondition = flags | 4;
        eventInfo.mMapToolId = tool;
    }
}

/* 021A4FFC */
u8 daKnob00_c::getType2() { return (u32)mParameters >> 28; }

// HD: passwords use the UTF-16 viewer input and generated message-table words.
/* 021A45C8 */
s32 daKnob00_c::chkPassward() {
    // HD: passwords use fixed UTF-16 SafeStrings, with a bounded copy and comparison.
    KnobGameInfo* game = knobGameInfo();
    auto* vtable = gabi::at<KnobWideVtable>(game->mInputPassword.__vtbl);
    u32 target = vtable->mAssureTermination.get();
    gabi::call_ptr(target, &game->mInputPassword);
    be<u16>* input = game->mInputPassword.mpData;
    gabi::Local<KnobWideString> source;
    source->mpData = input;
    source->__vtbl = 0x10013310;
    gabi::Local<KnobWideBuffer> entered;
    entered->mpData = entered->mChars;
    entered->__vtbl = 0x10013370;
    entered->mCapacity = 17;
    entered->mChars[16] = 0;
    emptyString(source.get());
    be<u16>* text = source->mpData;
    s32 length = 0;
    if (*text) {
        do {
            ++length;
            text++;
            if (length > 0x40000) {
                length = 0;
                break;
            }
        } while (*text);
    }
    s32 capacity = entered->mCapacity;
    if (length >= capacity)
        length = capacity - 1;
    source->assureTermination();
    text = source->mpData;
    knobCopyMemory(entered->mChars, text, length * 2, 0);
    gabi::Local<KnobWideBuffer> expected;
    expected->mChars[16] = 0;
    expected->mChars[0] = 0;
    expected->mCapacity = 17;
    entered->mChars[length] = 0;
    entered->__vtbl = 0x10013388;
    expected->mpData = expected->mChars;
    expected->__vtbl = 0x10013388;
    if (!dComIfGs_isEventBit(0x2110))
        return -1;
    s32 password = sSave->mEvent.getEventReg(0xBA0F);
    expected->assureTermination();
    be<u16>* output = expected->mpData;
    knobPasswordGet(sMessageManager.get(), output, password + 0x1B37);
    expected->assureTermination();
    expected->assureTermination();
    be<u16>* lhs = expected->mpData;
    entered->assureTermination();
    be<u16>* rhs = entered->mpData;
    if (lhs == rhs)
        return password;
    lhs = expected->mpData;
    for (u32 i = 0; i < 0x40001; i++) {
        u16 x = *lhs, y = *rhs;
        if (x != y)
            return -1;
        if (!x)
            return password;
        lhs++;
        rhs++;
    }
    return -1;
}

/* 021A4A94 */
BOOL daKnob00_c::msgDoor() {
    if (mDoorType == 1)
        return TRUE;
    s32 night = knobDayNight();
    return mDoorType == (night == 1 ? 2 : 3);
}

/* 021A3AA0 */
void daKnob00_c::openInit(s32 index) {
    J3DModel* model = mpModel;
    J3DModelData* data = model->mModelData;
    auto* resource = (J3DAnmTransform*)dComIfG_getObjectRes(
        "Knob", sAnimationResources.mResources[index], 0x100132F8);
    if (!mBckAnm.init(data, resource, true, 0, 1.0f, 0, -1, true))
        knobAssert("d_a_knob00.cpp", 0x1C0, "0");
    openInitCom(0);
    mFlags = (u8)mFlags | FLAG_OPEN;
    if (index >= 2)
        mBckAnm.setFrame(34.0f);
    else
        releaseBg(this);
}

/* 021A3BF8 */
BOOL daKnob00_c::openProc(s32 action) {
    auto anm = &mBckAnm;
    if (anm->play())
        return TRUE;
    if (action == 12 || action == 13) {
        auto night = knobDayNight();
        bool white = (night == 1) == (action == 13);
        if (anm->checkFrame(15.0f)) {
            sFadeRate = 0.0f;
            knobFadeOut(white ? &sWhiteColor : &sBlackColor, 0.05f);
        }
    }
    u32 sound = 0;
    if (action == 12 || action == 14) {
        if (anm->checkFrame(28.0f))
            sound = 0x696E;
        else if (anm->checkFrame(60.0f))
            sound = 0x696F;
        else if (anm->checkFrame(67.0f) && action == 14)
            sound = 0x6970;
    } else {
        if (anm->checkFrame(25.0f))
            sound = 0x696E;
        else if (anm->checkFrame(49.0f))
            sound = 0x696F;
        else if (anm->checkFrame(64.0f) && action == 15)
            sound = 0x6970;
    }
    if (sound) {
        auto room = (s8)current.roomNo;
        auto reverb = knobReverb(room);
        knobStartSound(sound, &eyePos, 0, reverb);
    }
    return FALSE;
}

/* 021A3E14 */
void daKnob00_c::openEnd() {
    mFlags = (u8)mFlags & 0xFE;
    KnobGameInfo* p = knobGameInfo();
    dBgW* bg = mpBgW;
    if (knobRegisterBg(&p->mBgS, bg, this))
        knobAssert("d_a_knob00.cpp", 0x21B, "!rt");
    sMapSendStop = 0;
    closeEndCom();
}

/* 021A4B04 */
BOOL daKnob00_c::chkException() {
    switch ((s16)home.angle.z) {
    case 0x6A6:
        if (symbol(0))
            return TRUE;
        return !eventBit(0x2110);
    case 0x6A7:
        return !eventBit(0x0520);
    case 0x6A8:
        if (!eventBit(0x0A02))
            return TRUE;
        return eventBit(0x2110) != 0;
    case 0x6A9:
        return eventBit(0x1701) != 0;
    case 0x6AA:
        if (!symbol(1))
            return TRUE;
        return eventBit(0x1A80) != 0;
    default:
        return FALSE;
    }
}

/* 021A3434 */
void daKnob00_c::calcMtx() {
    f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
    knobTranslate(&sMatrix, x, y, z);
    knobRotateY(&sMatrix, (s16)current.angle.y);
    J3DModel::mtx_assign(&mpModel->mBaseTransformMtx, &sMatrix);
}

/* 021A5348 */
BOOL daKnob00_c::CreateInit() {
    s8 room = current.roomNo;
    f32 att = (f32)attention_info.position.y + 150.0f;
    f32 eye = eyePos.y + 150.0f;
    tevStr.mRoomNo = room;
    attention_info.position.y = att;
    eyePos.y = eye;
    mAction = ACTION_INIT;
    attention_info.flags = 0x20;
    calcMtx();
    mpBgW->Move();
    initProc(0);
    mDoorType = (u8)getType();
    if (getShapeType() == 4 && mDoorType == 1) {
        s8 rn = current.roomNo;
        mDoorType = 5;
        knobCreateActor(0x1A2, 0xFFFFFF03, &current.pos, rn, &current.angle, (void*)nullptr, 0,
                        (void*)nullptr);
        att = (f32)attention_info.position.y + 60.0f;
        eye = eyePos.y + 60.0f;
        attention_info.position.y = att;
        eyePos.y = eye;
    }
    if (getShapeType() == 6) {
        mDoorType = 6;
        home.angle.z = 0x366A;
    }
    return TRUE;
}

/* 021A5464 */
s32 daKnob00_c::create() {
    s32 phase = knobResLoad(&mPhase, "Knob");
    u32 condition = actor_condition;
    if (!(condition & 8)) {
        if (this) {
            knobConstructActor(this);
            __vtbl = 0x100133A0;
            knobConstructMessage(messageWork());
            messageWork()->__vtbl = 0x10013360;
            knobInitFrame(&mBckAnm.mFrameCtrl, 0);
            animationStorage()->mCalc.__vtbl = 0x1016E54C;
            knobConstructBinding(&animationStorage()->mCalc.mBinding);
            animationStorage()->mCalc.mBinding.mpResource = 0;
            animationStorage()->mCalc.mSlots[0] = 0;
            animationStorage()->mCalc.__vtbl = 0x10013338;
            animationStorage()->mCalc.mSlots[1] = 0;
            animationStorage()->mCalc.mBinding.__vtbl = 0x1016D820;
            animationStorage()->mCalc.mSlots[2] = 0;
            animationStorage()->mpAnimation = nullptr;
            knobConstructStop(&mStopBars);
            condition = actor_condition;
        }
        actor_condition = condition | 8;
    }
    if (phase != 4)
        return phase;
    if (current.roomNo == -1)
        current.roomNo = (s8)getFRoomNo();
    if (!knobEnterHeap(this, gabi::guest_fn((const void*)CheckCreateHeap), 0x2700))
        return 5;
    CreateInit();
    return 4;
}

/* 021A39CC */
void daKnob00_c::setStart(f32 lateral, f32 forward) {
    KnobGameInfo* game = knobGameInfo();
    u16 angle = (s16)shape_angle.y + 0x7FFF;
    f32 cosine = sTrig.mEntries[angle >> 3].mCos, sine = sTrig.mEntries[angle >> 3].mSin;
    f32 dx = gabi::fmsubs(sine, forward, cosine * lateral),
        dz = gabi::fmadds(cosine, forward, sine * lateral);
    f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
    KnobPlayer* player = game->mpPlayer;
    gabi::Local<cXyz> position;
    position->set(x + dx, y, z + dz);
    auto* vt = gabi::at<KnobPlayerVtable>(player->__vtbl);
    u32 target = vt->mSetPlayerPosAndAngle.get();
    s16 rotation = player->shape_angle.y;
    gabi::call_ptr(target, player, position.get(), rotation);
}

/* 021A3BBC */
void daKnob00_c::setAngle() {
    KnobGameInfo* game = knobGameInfo();
    s16 angle = shape_angle.y;
    game->mpPlayer->mDemoMoveAngle = (s16)(angle + 0x7FFF);
}

/* 021A3E90 */
BOOL daKnob00_c::adjustmentProc() {
    KnobGameInfo* game = knobGameInfo();
    KnobPlayer* player = game->mpPlayer;
    u16 angle = (s16)shape_angle.y + 0x7FFF;
    f32 px = player->current.pos.x, py = player->current.pos.y, pz = player->current.pos.z;
    f32 targetZ = gabi::fmadds(sTrig.mEntries[angle >> 3].mCos, -70.0f, (f32)current.pos.z);
    f32 targetX = gabi::fmadds(sTrig.mEntries[angle >> 3].mSin, -70.0f, (f32)current.pos.x);
    s16 timer = mAdjustTimer;
    f32 targetY = current.pos.y;
    gabi::Local<cXyz> position;
    if (timer > 0)
        position->set(gabi::fmadds(px, 0.8f, targetX * 0.2f), py,
                      gabi::fmadds(pz, 0.8f, targetZ * 0.2f));
    else
        position->set(targetX, targetY, targetZ);
    auto* vt = gabi::at<KnobPlayerVtable>(player->__vtbl);
    u32 target = vt->mSetPlayerPosAndAngle.get();
    s16 rotation = player->current.angle.y;
    gabi::call_ptr(target, player, position.get(), rotation);
    if (timer > 0) {
        mAdjustTimer = (s16)mAdjustTimer - 1;
        return FALSE;
    }
    return TRUE;
}

/* 021A3FB8 */
BOOL daKnob00_c::demoProc() {
    s32 action = getDemoAction();
    s32 staff = mStaffId;
    KnobGameInfo* p = knobGameInfo();
    if (knobIsAdvance(&p->mEventManager, staff)) {
        switch (action) {
        case DEMO_START:
            setStart(0, -70);
            break;
        case DEMO_SHIP_FRONT:
        case DEMO_SHIP_BACK:
            setStart(action == 17 ? -43.0f : 46.0f, action == 17 ? -14.8f : -86.0f);
            if ((u32)mpBgW->m_id < 0x100)
                releaseBg(this);
            break;
        case DEMO_OPEN_FRONT:
        case DEMO_OPEN_BACK:
        case DEMO_OPEN_FRONT_CONTINUE:
        case DEMO_OPEN_BACK_CONTINUE:
            openInit(action - 12);
            break;
        case DEMO_ANGLE:
            setAngle();
            break;
        case DEMO_ADJUST:
            mAdjustTimer = 10;
            break;
        case DEMO_MESSAGE:
            m2D0.init((s16)home.angle.z);
            break;
        }
    }
    switch (action) {
    case DEMO_OPEN_FRONT:
    case DEMO_OPEN_BACK:
    case DEMO_OPEN_FRONT_CONTINUE:
    case DEMO_OPEN_BACK_CONTINUE:
        if ((u8)mFlags & 1) {
            if (openProc(action)) {
                openEnd();
                cutEnd(this);
            }
        } else
            cutEnd(this);
        break;
    case DEMO_ADJUST:
        if (adjustmentProc())
            cutEnd(this);
        break;
    case DEMO_MESSAGE: {
        cXyz* position = &attention_info.position;
        if (mAction == ACTION_FIGURE_TALK) {
            KnobMt* mt = knobSearchMt("Mt", 0, 0);
            if (mt)
                position = &mt->attention_info.position;
        }
        return m2D0.proc(position) != 0;
    }
    default:
        cutEnd(this);
    }
    return FALSE;
}

/* 021A4568 */
BOOL daKnob00_c::demoProc2() {
    if (getDemoAction() == 16)
        return TRUE;
    s32 staff = mStaffId;
    KnobGameInfo* p = knobGameInfo();
    knobCutEnd(&p->mEventManager, staff);
    return FALSE;
}

/* 021A4CB4 */
BOOL daKnob00_c::actionWait() {
    if ((u16)eventInfo.mCommand == dEvtCmd_INDOOR_e) {
        initOpenDemo(0);
        mAction = ACTION_DEMO;
        demoProc();
    } else
        setEventPrm();
    return TRUE;
}

/* 021A4ED0 */
BOOL daKnob00_c::actionDemo() {
    s16 event = (s16)mEventIdx[(u8)m2C6];
    KnobGameInfo* p = knobGameInfo();
    if (knobEndCheck(&p->mEventManager, event)) {
        u8 no = m2C6;
        if (no != 7 && no != 8) {
            mAction = ACTION_WAIT;
            resetEvent();
            shape_angle.y = current.angle.y;
        }
    } else
        demoProc();
    return TRUE;
}

/* 021A4F70 */
BOOL daKnob00_c::actionTalk() {
    if (demoProc()) {
        u8 action = mAction;
        if (action == 9) {
            KnobMt* mt = knobSearchMt("Mt", 0, 0);
            if (mt)
                mt->mAttnEnabled = 0;
            action = mAction;
        }
        mAction = action + 1;
        resetEvent();
        shape_angle.y = current.angle.y;
    }
    return TRUE;
}

/* 021A4C58 */
BOOL daKnob00_c::actionTalkWait() {
    if ((u16)eventInfo.mCommand == dEvtCmd_INDOOR_e) {
        initOpenDemo(0);
        mAction = ACTION_TALK;
        demoProc();
    } else
        setEventPrm();
    return TRUE;
}

/* 021A4808 */
BOOL daKnob00_c::actionPassward2() {
    auto msg = &m2D0;
    switch ((u8)mPasswordState) {
    case 0:
        if (demoProc2()) {
            mPasswordState = (u8)mPasswordState + 1;
            if (!eventBit(0x3B20)) {
                onEventBit(0x3B20);
                f32 random = knobRandom(6.0f);
                sSave->mEvent.setEventReg(0xBA0F, (u8)gabi::ftoi(random));
            }
            s32 reg = sSave->mEvent.getEventReg(0xBA0F);
            msg->init((s16)(reg + 0x1B1A));
        }
        break;
    case 1:
        if (msg->proc(&attention_info.position)) {
            mPasswordState = (u8)mPasswordState + 1;
            KnobGameInfo* p = knobGameInfo();
            p->mPasswordOpen = 2;
        }
        break;
    case 2: {
        KnobGameInfo* p = knobGameInfo();
        u8 state = (u8)p->mPasswordOpen;
        if (state > 1)
            break;
        if (state == 1 && chkPassward() != -1) {
            onEventBit(0x1910);
            mPasswordState = (u8)mPasswordState + 1;
            msg->init(0x1B18);
        } else {
            mPasswordState = 10;
            msg->init(0x1B19);
        }
        break;
    }
    case 3:
    case 10: {
        u8 state = mPasswordState;
        if (msg->proc(&attention_info.position)) {
            mAction = state == 3 ? 1 : 5;
            resetEvent();
            shape_angle.y = current.angle.y;
        }
        break;
    }
    }
    return TRUE;
}

/* 021A44DC */
BOOL daKnob00_c::actionVilla() {
    if ((u16)eventInfo.mCommand == dEvtCmd_INDOOR_e) {
        initOpenDemo(0);
        mAction = ACTION_VILLA_TALK;
        demoProc();
    } else if (eventBit(0x2D80))
        mAction = ACTION_WAIT;
    else
        setEventPrm();
    return TRUE;
}

/* 021A4A30 */
BOOL daKnob00_c::actionPassward() {
    if ((u16)eventInfo.mCommand == dEvtCmd_INDOOR_e) {
        initOpenDemo(0);
        mAction = ACTION_PASSWORD_TALK;
        actionPassward2();
        mPasswordState = 0;
    } else
        setEventPrm();
    return TRUE;
}

/* 021A4D10 */
BOOL daKnob00_c::actionInit() {
    if ((u32)mpBgW->m_id >= 0x100) {
        if (drawCheckShip()) {
            KnobGameInfo* p = knobGameInfo();
            dBgW* bg = mpBgW;
            knobRegisterBg(&p->mBgS, bg, this);
        } else
            mHidden = 1;
    }
    u8 type = mDoorType;
    if (type == 6) {
        if (!eventBit(0x3401)) {
            actionFigure();
            mAction = ACTION_FIGURE;
            return TRUE;
        }
        type = mDoorType;
    }
    if (type == 5) {
        if (!eventBit(0x2D80)) {
            actionVilla();
            mAction = ACTION_VILLA;
            return TRUE;
        }
        type = mDoorType;
    }
    if (type == 4) {
        if (!eventBit(0x1910)) {
            mAction = ACTION_PASSWORD;
            actionPassward();
            return TRUE;
        }
        if (knobCheckItem(dItemNo_BOMB_BAG_e)) {
            mAction = ACTION_DEAD;
            return TRUE;
        }
    }
    if (msgDoor() && !chkException()) {
        actionTalkWait();
        mAction = ACTION_TALK_WAIT;
    } else {
        actionWait();
        mAction = ACTION_WAIT;
    }
    return TRUE;
}

/* 021A43F8 */
BOOL daKnob00_c::actionFigure() {
    if (eventInfo.mCommand == dEvtCmd_INDOOR_e) {
        KnobMt* mt = knobSearchMt("Mt", 0, 0);
        if (mt) {
            dEvt_control_c* control = &knobGameInfo()->mEvent;
            u32 id = knobSetTalkPartner(control, mt);
            control->mPtTalk = id;
            u32 status = mt->actor_status;
            mt->mAttnEnabled = 1;
            mt->actor_status = status | fopAcStts_UNK800_e;
        }
        initOpenDemo(0);
        mAction = ACTION_FIGURE_TALK;
        demoProc();
    } else if (eventBit(0x3401))
        mAction = ACTION_WAIT;
    else
        setEventPrm();
    return TRUE;
}

// HD: attached doors remain hidden until the parent ship exposes them.
/* 021A3940 */
BOOL daKnob00_c::drawCheckShip() {
    u32 id = parentActorID;
    if (id != 0xFFFFFFFF) {
        gabi::Local<be<u32>> key;
        *key = id;
        KnobShip* ship = knobSearchByID(0x025E1234, key.get());
        BOOL actor = knobIsActor(ship);
        if (actor && ship && ship->mProcName == KNOB_PROCESS_SHIP)
            return ship->mDoorVisible != 0;
    }
    return TRUE;
}

/* 021A37A8 */
BOOL daKnob00_c::draw() {
    if (!knobDrawCheck(this, 0)) {
        if (getShapeType() == 1)
            actor_status = (u32)actor_status & ~fopAcStts_SHOWMAP_e;
        return TRUE;
    }
    u8 shape = getShapeType(), hidden = mHidden;
    if (shape == 1)
        actor_status = (u32)actor_status & ~fopAcStts_SHOWMAP_e;
    if (hidden)
        return TRUE;
    s32 type = getShapeType() == 5 ? 0 : 1;
    settingTevStruct(dKy_getEnvlight(), type, &current.pos, &tevStr);
    auto* environment = dKy_getEnvlight();
    J3DModel* second = mpModel2;
    setLightTevColorType(environment, second, &tevStr);
    J3DModel* model = mpModel;
    f32 frame = mBckAnm.getFrame();
    J3DModelData* data = model->mModelData;
    mBckAnm.entry(data, frame);
    model = mpModel;
    model->calc();
    model = mpModel;
    s8 joint = m_jnt;
    J3DMtxBuffer_hd* block = model->mpMtxBuffer;
    u16 flags = block->mFlags;
    u32 matrices = block->mpAnmMtx;
    second = mpModel2;
    block->mFlags = flags | 0x10;
    J3DModel::mtx_assign(&second->mBaseTransformMtx,
                         gabi::at<Mtx34>(matrices + sizeof(Mtx34) * (u32)(s32)joint));
    second = mpModel2;
    knobModelUpdateDL(second, 0);
    return TRUE;
}

/* 021A393C */
static BOOL daKnob00_Draw(daKnob00_c* a) { return a->draw(); }

/* 021A5008 */
BOOL daKnob00_c::execute() {
    if ((u8)mHidden && drawCheckShip()) {
        mHidden = 0;
        KnobGameInfo* p = knobGameInfo();
        dBgW* bg = mpBgW;
        knobRegisterBg(&p->mBgS, bg, this);
    }
    switch (knobCheckExecute(this)) {
    case 0:
        mAction = ACTION_INIT;
        break;
    case 1:
        knobStartDemo(this);
        demoProc();
        break;
    case 2:
        switch ((u8)mAction) {
        case ACTION_INIT:
            actionInit();
            break;
        case ACTION_WAIT:
            actionWait();
            break;
        case ACTION_DEMO:
            actionDemo();
            break;
        case ACTION_TALK:
        case ACTION_VILLA_TALK:
        case ACTION_FIGURE_TALK:
            actionTalk();
            break;
        case ACTION_TALK_WAIT:
            actionTalkWait();
            break;
        case ACTION_PASSWORD:
            actionPassward();
            break;
        case ACTION_PASSWORD_TALK:
            actionPassward2();
            break;
        case ACTION_VILLA:
            actionVilla();
            break;
        case ACTION_FIGURE:
            actionFigure();
            break;
        }
        break;
    default:
        knobAssert("d_a_knob00.cpp", 0x4E7, "0");
    }
    mRoomNo2 = sStayRoom;
    if (getType2() == 1) {
        calcMtx();
        dBgW* bg = mpBgW;
        if (bg && (u32)bg->m_id < 0x100)
            bg->Move();
    }
    return TRUE;
}

/* 021A52D0 */
static BOOL daKnob00_IsDelete(daKnob00_c* a) { return TRUE; }

/* 021A52D8 */
static BOOL daKnob00_Delete(daKnob00_c* a) {
    if (a->heap != nullptr) {
        dBgW* bg = a->mpBgW;
        if (bg && (u32)bg->m_id < 0x100) {
            KnobGameInfo* p = knobGameInfo();
            bg = a->mpBgW;
            knobReleaseBg(&p->mBgS, bg);
        }
    }
    knobResDelete(&a->mPhase, "Knob");
    return TRUE;
}

/* 021A55CC */
static s32 daKnob00_Create(daKnob00_c* a) { return a->create(); }

// HD: compiler-generated and template code of this translation unit.
/* 021A342C */
static void emptyVirtual(void* a) {}

/* 021A55D0 */
static void sinit_knob00() { sinit_header_statics(0x10464C20, 0x101B8638); }

/* 021A5664 */
static void trivialDestructor(void* a, s32 flags) {
    if (a && (flags & 1))
        operator_delete(a);
}

/* 021A5678 */
static void knobDestructor(daKnob00_c* a, s32 flags) {
    if (a) {
        knobDestroyMatrixCalc(&a->animationStorage()->mCalc, 0);
        knobDestroyMessage(a->messageWork(), 0);
        knobDestroyActor(a, 0);
        if (flags & 1)
            operator_delete(a);
    }
}

/* 021A56E4 */
static void emptyVirtual2(void* a) {}

/* 021A56E8 */
static void emptyString(void* a) {}

/* 021A56EC */
static void terminateWideString(KnobWideBuffer* text) {
    s32 capacity = text->mCapacity;
    be<u16>* data = text->mpData;
    *gabi::at<be<u16>>(gabi::ea(data) + 2 * (u32)capacity - 2) = 0;
}

// clang-format off
/* WWHD glue
data sSave 101F84DC
data sMessageManager 101F4B5C
data sMatrix 1048D0CC
data sTrig 104A44F8
data sAnimationResources 101B8628
data sShapeResources 100133C3
data sFadeRate 101F4810
data sWhiteColor 101D5E9C
data sBlackColor 101D5E94
data sMapSendStop 1047A964
data sStayRoom 1047E6C8
str 100132F4 in 021A5008
str 100133B0 in 021A5008
str 100133CC
str 100133D8 in 021A34F0
str 100133E8
str 100133FC
str 1001340C in 021A3AA0
str 10013410 in 021A3AA0
str 10013444
str 10013448 in 021A3E14
str 10013474 in 021A3FB8
str 10013484 in 021A43F8
str 1001348C in 021A4F70
str 100134A0
ext 0252AE04 BOOL knobCheckArea(daKnob00_c*, f32, f32, f32)
ext 02556D14 s32 knobDayNight()
ext 02543280 void knobCutEnd(dEvent_manager_c*, s32)
ext 025440C8 BOOL knobEndCheck(dEvent_manager_c*, s16)
ext 024EEA6C BOOL knobRegisterBg(dBgS*, dBgW*, fopAc_ac_c*)
ext 0273AA24 void knobAssert(const char*, u32, const char*)
ext 020087EC void knobReleaseBg(dBgS*, dBgW*)
ext 025F0658 void knobFadeOut(GXColor*, f32)
ext 02520540 s32 knobReverb(s32)
ext 025E1A40 void knobStartSound(u32, cXyz*, u32, s32)
ext 02520C0C BOOL knobCheckItem(u32)
ext 025204C8 void knobResDelete(request_of_phase_process_class*, const char*)
ext 025D5834 void knobCreateActor(u32, u32, cXyz*, s8, csXyz*, void*, u32, void*)
ext 02520460 s32 knobResLoad(request_of_phase_process_class*, const char*)
ext 0252A244 void knobConstructActor(daKnob00_c*)
ext 025DD5F0 void knobConstructMessage(KnobMessageWork*)
ext 027F2BC0 void knobInitFrame(J3DFrameCtrl*, u32)
ext 027DA984 void knobConstructBinding(KnobAnimationBinding*)
ext 0252BA44 void knobConstructStop(KnobStopBars*)
ext 025D63E8 BOOL knobEnterHeap(daKnob00_c*, u32, u32)
ext 0252AC98 s32 knobCheckExecute(daKnob00_c*)
ext 0252AD50 void knobStartDemo(daKnob00_c*)
ext 025447C8 BOOL knobIsAdvance(dEvent_manager_c*, s32)
ext 020198D8 f32 knobRandom(f32)
ext 027F3628 void knobDestroyMatrixCalc(KnobMatrixCalc*, u32)
ext 025DD630 void knobDestroyMessage(KnobMessageWork*, u32)
ext 025D50BC void knobDestroyActor(daKnob00_c*, u32)
ext 025200D4 KnobGameInfo* knobGameInfo()
ext 028E93CC void knobTranslate(Mtx34*, f32, f32, f32)
ext 025F1C28 void knobRotateY(Mtx34*, s16)
ext 0252B0FC BOOL knobDrawCheck(daKnob00_c*, s32)
ext 025E2DE0 void knobModelUpdateDL(J3DModel*, u32)
ext 025D9F38 KnobMt* knobSearchMt(const char*, u32, u32)
ext 0253F124 u32 knobSetTalkPartner(dEvt_control_c*, KnobMt*)
ext 025D5218 KnobShip* knobSearchByID(u32, be<u32>*)
ext 025D4604 BOOL knobIsActor(fopAc_ac_c*)
ext 0200A030 BOOL knobSetBackground(dBgW*, void*, u32, Mtx34*)
ext C0009988 void knobCopyMemory(void*, void*, u32, u32)
ext 025F8A08 void knobPasswordGet(void*, be<u16>*, s32)
ext 0252A2DC void dDoor_info_c::initOpenDemo(s32)
ext 0252A37C u32 dDoor_info_c::getFRoomNo()
ext 0252A3A0 void dDoor_info_c::openInitCom(s32)
ext 0252A48C void dDoor_info_c::openProcCom()
ext 0252A550 void dDoor_info_c::closeEndCom()
ext 0252A684 s32 dDoor_info_c::getDemoAction()
ext 0252A9E0 void dDoor_info_c::initProc(u32)
ext 0252AA20 u32 dDoor_info_c::getSwbit2()
ext 0252AA2C u32 dDoor_info_c::getType()
ext 0252BEC4 void dDoor_msg_c::init(s32)
ext 0252BEDC BOOL dDoor_msg_c::proc(cXyz*)
*/
// clang-format on
