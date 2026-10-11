// Forbidden Woods door and animated barriers. All verified HD behavior is retained.
#include "d/actor/d_a_kddoor.h"
u8 kddoorType(daKddoor_c*);
u8 kddoorFrontSwitch(daKddoor_c*);
u8 kddoorBackSwitch(daKddoor_c*);
u8 kddoorFrontRoom(daKddoor_c*);
u8 kddoorBackRoom(daKddoor_c*);
BOOL kddoorIsSwitch(void*, u32, s32);
void kddoorOnSwitch(void*, u32, s32);
void kddoorStartSound(u32, cXyz*, u32, s32);
void kddoorCutEnd(void*, s32);
KddoorJointInfo* kddoorJointInfo(void*);
void kddoorCopyMatrix(void*, void*);
void kddoorRotateY(void*, s32);
void kddoorRotateZ(void*, s32);
BOOL kddoorRoomDisplayed(void*, u32);
void* kddoorRoomEnemy(s8);
void kddoorDelete(void*);
void kddoorDestroyCylinder(dCcD_Cyl*, u32);
void kddoorDestroyStatus(dCcD_Stts*, u32);
void kddoorDestroyArray(void*, u32, u32, void*, u32, u32);
void kddoorDestroyAnimation(void*, u32);
void kddoorDestroyActor(daKddoor_c*, u32);
void kddoorTranslate(void*, f32, f32, f32);
void kddoorTranslateMatrix(f32, f32, f32);
f32 kddoorRandom();
void kddoorOpenInitCom(daKddoor_c*, s32);
void kddoorReleaseBg(void*, dBgW*);
s32 kddoorRegisterBg(void*, dBgW*, daKddoor_c*);
void kddoorAssert(const char*, u32, const char*);
BOOL kddoorChase(be<f32>*, f32, f32);
void kddoorCloseEndCom(daKddoor_c*);
void kddoorShock(void*, u32, s32, cXyz*);
void kddoorInitStatus(dCcD_Stts*, u32, u32, void*);
void kddoorSetCylinder(dCcD_Cyl*, void*);
void kddoorInitTev(dKy_tevstr_c*, s32, u32);
void kddoorResDelete(request_of_phase_process_class*, const char*);
void kddoorKeyResDelete(KddoorKey*);
void kddoorSmokeEnd(void*);
void* kddoorEnvlight();
void kddoorSettingTev(void*, s32, cXyz*, dKy_tevstr_c*);
void kddoorLightColor(void*, void*, dKy_tevstr_c*);
void kddoorMorphEntry(mDoExt_McaMorf*);
void kddoorMorphUpdate(mDoExt_McaMorf*);
BOOL kddoorDrawCheck(daKddoor_c*, s32);
void kddoorModelUpdate(J3DModel*, u32);
void kddoorKeyDraw(KddoorKey*, daKddoor_c*);
void* kddoorAllocate(u32);
void kddoorConstructSmoke(KddoorSmokeCallback*, u32);
void kddoorConstructStatus(dCcD_Stts*);
void kddoorConstructGlobalStatus(void*);
void kddoorConstructCollider(dCcD_Cyl*);
void kddoorConstructCylinder(cM3dGCyl*);
void kddoorRegisterDestructor(void*);
void kddoorInitRandom(void*);
void kddoorInitRandomByte(void*);
void kddoorKeyOn(KddoorKey*);
void kddoorKeyOff(KddoorKey*);
void kddoorConstructArray(void*, u32, u32, void*);
void kddoorMultVec(void*, cXyz*, cXyz*);
void kddoorCylinderCenter(cM3dGCyl*, cXyz*);
void kddoorCylinderRadius(cM3dGCyl*, f32);
void* kddoorStageResource(const char*, const char*);
J3DModel* kddoorCreateModel(void*, u32, u32);
BOOL kddoorKeyCreate(KddoorKey*, s32);
dBgW* kddoorCreateBackground(void*);
void kddoorKeyMatrix(KddoorKey*, daKddoor_c*);
s32 kddoorSetBackground(dBgW*, void*, u32, void*);
BOOL kddoorEndCheck(void*, s16);
void kddoorMoveBackground(dBgW*);
void kddoorInitProc(daKddoor_c*, u32);
s32 kddoorResLoad(request_of_phase_process_class*, const char*);
void kddoorConstructActor(daKddoor_c*);
void kddoorConstructDoorSmoke(void*);
void kddoorConstructKey(KddoorKey*);
s32 kddoorKeyResLoad(KddoorKey*);
BOOL kddoorEnterHeap(daKddoor_c*, void*, u32);
void kddoorCopyMemory(void*, void*, u32);
u32 kddoorCheckExecute(daKddoor_c*);
void kddoorStartDemo(daKddoor_c*);
void kddoorAddCalcZero(be<f32>*, f32, f32);
void kddoorAddCalc(be<f32>*, f32, f32, f32);
void kddoorParticle(void*, u32, u32, cXyz*, void*, void*, u32, void*, s32, void*, void*, void*);
void* kddoorObjectResource(void*, KddoorResourceName*, s32);
mDoExt_McaMorf* kddoorConstructMorph(void*, void*, void*, void*, void*, s32, void*, s32, void*,
                                     void*, u32, u32, f32);
void* kddoorModelHeader(void*);
// HD: callee adapters preserve the verified argument widths and embedded component views.
BOOL kddoorCheckArea(daKddoor_c*, f32, f32, f32);
void kddoorInitOpenDemo(daKddoor_c*, s32);
s32 kddoorStaffId(void*, const char*, u32, u32);
void kddoorOrderEvent(daKddoor_c*, s16, u32, u32, u32, u32);
void kddoorMorphCalc(mDoExt_McaMorf*);
void kddoorSetCollision(void*, dCcD_Cyl*);
u32 kddoorDemoAction(daKddoor_c*);
BOOL kddoorIsAdvance(void*, s32);
void kddoorSmokeInit(void*, daKddoor_c*);
void kddoorSetGoal(daKddoor_c*);
void kddoorKeyInit(KddoorKey*, daKddoor_c*);
void kddoorSmokeProc(void*, daKddoor_c*);
BOOL kddoorKeyProc(KddoorKey*);
BOOL kddoorStartDemoCheck(void*);
void kddoorCancelStartDemo(void*);
KddoorGameInfo* kddoorGameInfo();
static BOOL nodeCB(void*, s32);
static BOOL daKddoor_actionWait(daKddoor_c*);
static BOOL daKddoor_actionStopClose(daKddoor_c*);
static BOOL daKddoor_actionDemo(daKddoor_c*);
static BOOL daKddoor_actionInit(daKddoor_c*);
static BOOL CheckCreateHeap(daKddoor_c*);
static dDoor_ssk_c* constructStop(dDoor_ssk_c*);
static J3DModel* sCurrentModel;
static Mtx34* sCalcMatrix;
static Mtx34 sMatrix;
static Mtx34 sCurrentMatrix;
static KddoorTrigTable sTrig;
static u32 sDrawList;
static u32 sXluDrawList;
static KddoorColliderSource sCylinderSource;
static void* sResourceControl;
static KddoorLightBlock sLightPrototype;
static KddoorHeaderStatics sHeaderStatics;
static KddoorArrayDestructor sHeaderDtor0;
static KddoorArrayDestructor sHeaderDtor1;
static KddoorArrayDestructor sHeaderDtor2;
static u32 sActionGuard;
static KddoorActionTable sActions;
static KddoorActionTable sActionPrototype;
static KddoorSave* sSave;
static KddoorRoomTable sRooms;
static u8 sMapSendStop;
static s8 sStayRoom;

static u8 type(daKddoor_c* a) { return kddoorType(a); }
static u8 frontSwitch(daKddoor_c* a) { return kddoorFrontSwitch(a); }
static u8 backSwitch(daKddoor_c* a) { return kddoorBackSwitch(a); }
static u8 frontRoom(daKddoor_c* a) { return kddoorFrontRoom(a); }
static u8 backRoom(daKddoor_c* a) { return kddoorBackRoom(a); }
static BOOL switchOn(u8 sw, u8 room) {
    KddoorSave* save = sSave;
    return kddoorIsSwitch(save->mSwitchInfo, sw, room);
}
static void setSwitch(u8 sw, u8 room) {
    KddoorSave* save = sSave;
    kddoorOnSwitch(save->mSwitchInfo, sw, room);
}
static bool roomActive(u8 room) {
    kddoorGameInfo();
    return (sRooms.mRooms[room].mFlags & 1) != 0;
}
static void sound(dDoor_info_c* a, u32 id) {
    if (a && gabi::ea(&a->eyePos)) {
        s8 room = a->current.roomNo;
        s32 reverb = dComIfGp_getReverb(room);
        kddoorStartSound(id, &a->eyePos, 0, reverb);
    }
}
static void copyMatrix(Mtx34* source, Mtx34* destination) {
    J3DModel::mtx_assign(destination, source);
}
static Mtx34* jointMatrix(J3DModel* model, u32 joint) { return model->getAnmMtx(joint); }
static void cutEnd(daKddoor_c* a) {
    s32 mStaffId = a->mStaffId;
    auto play = kddoorGameInfo();
    kddoorCutEnd(&play->mEventManager, mStaffId);
}

/* 0219641C */
BOOL daKddoor_c::chkMakeKey() { return type(this) == 2; }

/* 02198418 */
void daKddoor_c::setKey() {
    if (this->chkMakeKey()) {
        u8 sw = frontSwitch(this);
        KddoorSave* save = sSave;
        if (!kddoorIsSwitch(save->mSwitchInfo, sw, -1)) {
            kddoorKeyOn(&this->mKey);
            return;
        }
    }
    kddoorKeyOff(&this->mKey);
}

/* 02196448 */
BOOL daKddoor_c::chkMakeStop() {
    if (backSwitch(this) != 0xFF)
        return TRUE;
    if (this->chkMakeKey())
        return FALSE;
    return frontSwitch(this) != 0xFF;
}

/* 02196D2C */
s32 daKddoor_c::chkStopF() {
    u8 kind = type(this), sw = frontSwitch(this), room = frontRoom(this);
    if (sw == 0xFF)
        return 0;
    if (kind > 1)
        return 0;
    if (!roomActive(room))
        return -1;
    return !switchOn(sw, room);
}

/* 02196E10 */
s32 daKddoor_c::chkStopB() {
    u8 sw = backSwitch(this), room = backRoom(this);
    if (sw == 0xFF)
        return 0;
    if (!roomActive(room))
        return -1;
    return !switchOn(sw, room);
}

/* 02196EE0 */
void daKddoor_c::setStop() {
    if (this->chkMakeStop()) {
        u8 mFrontCheck = this->mFrontCheck;
        this->mStop.mSide = mFrontCheck;
        if (!mFrontCheck) {
            this->mStop.mbEnabled = (u8)this->chkStopF();
            this->mStop.mOtherEnabled = (u8)this->chkStopB();
        } else {
            this->mStop.mbEnabled = (u8)this->chkStopB();
            this->mStop.mOtherEnabled = (u8)this->chkStopF();
        }
    }
}

/* 02197D2C */
BOOL daKddoor_c::chkGenocideCase() {
    u8 kind = type(this);
    return !this->mFrontCheck ? kind == 1 : kind == 2 || kind == 4;
}

/* 02197F28 */
BOOL daKddoor_c::chkFeelerCase() {
    u8 kind = type(this);
    return !this->mFrontCheck && kind >= 3 && kind <= 4;
}

/* 02197D98 */
BOOL daKddoor_c::chkStopOpen() {

    u8 sw, room;
    if (!this->mFrontCheck) {
        sw = frontSwitch(this);
        room = frontRoom(this);
    } else {
        sw = backSwitch(this);
        room = backRoom(this);
    }
    if (this->chkGenocideCase()) {
        auto play = kddoorGameInfo();
        if (play->mEvent.mMode)
            return FALSE;
        play = kddoorGameInfo();
        BOOL visible = kddoorRoomDisplayed(&play->mRoomControl, room);
        if (visible && !kddoorRoomEnemy((s8)room)) {
            u8 timer = this->m2A1;
            if (timer) {
                this->m2A1 = timer - 1;
                return FALSE;
            }
            if (sw != 0xFF)
                setSwitch(sw, room);
            return TRUE;
        }
        this->m2A1 = 0x41;
        return FALSE;
    }
    return sw != 0xFF && switchOn(sw, room);
}

/* 02197F14 */
void daKddoor_c::setStopDemo() { this->m2C6 = (u8)this->mFrontCheck != 0; }

/* 02197F8C */
BOOL daKddoor_c::chkStopClose() {
    u8 kind = type(this);
    if (this->chkGenocideCase())
        return FALSE;
    if (this->chkFeelerCase())
        return FALSE;
    u8 sw, room;
    if (!this->mFrontCheck) {
        if (kind >= 3 && kind <= 4)
            return FALSE;
        sw = frontSwitch(this);
        room = frontRoom(this);
    } else {
        sw = backSwitch(this);
        room = backRoom(this);
    }
    if (sw == 0xFF)
        return FALSE;
    return !switchOn(sw, room);
}

/* 02198DAC */
void dDoor_ssk_c::init(dDoor_info_c* i_door) {
    s8 room = i_door->tevStr.mRoomNo;
    kddoorInitTev(&mTevStr, room, 255);
    for (u32 i = 0; i < 3; ++i)
        this->mArms[i].init();
}

/* 02198990 */
void dDoor_ssk_c::end() {
    for (u32 i = 0; i < 3; ++i)
        this->mArms[i].end();
}

/* 021969A4 */
void dDoor_ssk_c::calcMtx(dDoor_info_c* i_door) {
    if (this->mbEnabled) {
        this->mArms[0].calcMtx(i_door, 0.0f, 100.0f, this->mSide);
        this->mArms[1].calcMtx(i_door, 100.0f, 75.0f, this->mSide);
        this->mArms[2].calcMtx(i_door, -100.0f, 75.0f, this->mSide);
    }
}

/* 0219623C */
static BOOL nodeCB(void* i_node, s32 i_param) {
    if (i_param == 0) {
        KddoorJointInfo* info = kddoorJointInfo(i_node);
        J3DModel* model = sCurrentModel;
        auto* arm = static_cast<dDoor_ssk_sub_c*>(model->getUserArea());
        u16 index = info->mIndex;
        // HD: four joints sway with fixed phase multipliers and Y/Z rotations.
        if (arm && (u32)index - 1 < 4) {
            Mtx34* source = jointMatrix(model, index);
            Mtx34* matrix = sCalcMatrix;
            kddoorCopyMatrix(source, matrix);
            f32 amplitude = gabi::fmadds((f32)index, 1000.0f, 500.0f);
            s16 phase = arm->mPhase;
            u16 angle = (u16)((s32)phase * 2000 - (s32)index * 14000);
            f32 sine = sTrig.mEntries[angle >> 3].mSin;
            s16 rotation = (s16)gabi::ftoi(gabi::fmuls_ppc(sine, amplitude));
            matrix = sCalcMatrix;
            kddoorRotateY(matrix, rotation);
            phase = arm->mPhase;
            angle = (u16)((s32)phase * 1500 - (s32)index * 14000);
            sine = sTrig.mEntries[angle >> 3].mSin;
            rotation = (s16)gabi::ftoi(gabi::fmuls_ppc(sine, amplitude));
            matrix = sCalcMatrix;
            kddoorRotateZ(matrix, rotation);
            Mtx34* destination = jointMatrix(model, index);
            matrix = sCalcMatrix;
            copyMatrix(matrix, destination);
            matrix = sCalcMatrix;
            kddoorCopyMatrix(matrix, &sCurrentMatrix);
        }
    }
    return TRUE;
}

/* 02198678 */
void dDoor_ssk_c::execute(dDoor_info_c* i_door) {
    for (u32 i = 0; i < 3; ++i) {
        dDoor_ssk_sub_c* arm = &mArms[i];
        auto* body = arm->mpMorf.get();
        if (body) {
            J3DModel* model = body->mpModel;
            J3DModelData* data = model->mModelData;
            u16 count = data->getJointNum();
            for (u16 index = 0; index < count; ++index) {
                model = arm->mpMorf->mpModel;
                data = model->mModelData;
                data->getJointNodePointer(index)->mCallBack = gabi::guest_fn((const void*)nodeCB);
                data = arm->mpMorf->mpModel->mModelData;
                count = data->getJointNum();
            }
            kddoorMorphCalc(arm->mpMorf.get());
            // HD: the oscillation phase advances by one, independent of the randomized angle.
            arm->mPhase = (s16)((s32)arm->mPhase + 1);
        }
        auto* play = kddoorGameInfo();
        kddoorSetCollision(&play->mCollision, &arm->mCyl);
        if ((f32)arm->mScale.x > 0.9f) {
            u8 timer = arm->mSoundTimer;
            if (timer)
                arm->mSoundTimer = timer - 1;
            else {
                sound(i_door, 0x3823);
                f32 random = kddoorRandom();
                arm->mSoundTimer = (u8)gabi::ftoi(gabi::fmadds(random, 30.0f, 50.0f));
            }
        } else
            arm->mSoundTimer = 0;
    }
}

/* 021984F4 */
void dDoor_ssk_c::draw(dDoor_info_c* i_door) {

    auto env = kddoorEnvlight();
    kddoorSettingTev(env, 0, &i_door->current.pos, &this->mTevStr);
    for (u32 i = 0; i < 3; ++i) {
        dDoor_ssk_sub_c* arm = &this->mArms[i];
        auto morph = arm->mpMorf.get();
        if (morph) {
            auto model = morph->mpModel.get();
            env = kddoorEnvlight();
            kddoorLightColor(env, model, &this->mTevStr);
            kddoorMorphEntry(arm->mpMorf.get());
        }
        auto cap = arm->mpCap.get();
        if (cap) {
            auto model = cap->mpModel.get();
            env = kddoorEnvlight();
            kddoorLightColor(env, model, &this->mTevStr);
            kddoorMorphUpdate(arm->mpCap.get());
        }
    }
}

/* 02196FC8 */
void dDoor_ssk_c::closeInit() {
    this->mMoving = 1;
    for (u32 i = 0; i < 3; ++i)
        this->mArms[i].closeInit();
}

/* 02197600 */
BOOL dDoor_ssk_c::closeProc(dDoor_info_c* i_door) {
    if (!this->mMoving)
        return TRUE;
    BOOL done = TRUE;
    for (u32 i = 0; i < 3; ++i)
        if (!this->mArms[i].closeProc(i_door))
            done = FALSE;
    if (done)
        this->mMoving = 0;
    return done;
}

/* 0219708C */
void dDoor_ssk_c::openInit() {
    this->mMoving = 1;
    for (u32 i = 0; i < 3; ++i)
        this->mArms[i].openInit();
}

/* 0219783C */
BOOL dDoor_ssk_c::openProc(dDoor_info_c* i_door) {
    if (!this->mMoving)
        return TRUE;
    BOOL done = TRUE;
    for (u32 i = 0; i < 3; ++i)
        if (!this->mArms[i].openProc(i_door))
            done = FALSE;
    if (done) {
        this->mbEnabled = 0;
        this->mMoving = 0;
    }
    return done;
}

/* 02198D48 */
void dDoor_ssk_sub_c::init() {
    kddoorInitStatus(&this->mStts, 255, 255, nullptr);
    kddoorSetCylinder(&this->mCyl, &sCylinderSource);
    mCyl.mStts = &mStts;
    this->mState = 1;
    mSmoke.mRateOff = 0;
}

/* 02198980 */
void dDoor_ssk_sub_c::end() { mSmoke.end(); }

/* 02197018 */
void dDoor_ssk_sub_c::openInit() {
    this->mScale.set(1, 1, 1);
    this->mCapScale.set(1, 1, 1);
    f32 random = kddoorRandom();
    this->mStarted = 0;
    this->mDelay = (s16)gabi::ftoi(gabi::fmuls_ppc(random, 5.0f));
}

/* 02197694 */
BOOL dDoor_ssk_sub_c::openProc(dDoor_info_c* i_door) {
    s16 timer = this->mDelay;
    if (timer > 0) {
        this->mDelay = timer - 1;
        return FALSE;
    }
    if (((f32)this->mScale.y > 0.1f)) {
        if (!this->mStarted) {
            this->mStarted = 1;
            sound(i_door, 0x694D);
        }
        kddoorAddCalcZero(&this->mScale.y, 0.25f, 0.3f);
        f32 value = this->mScale.y;
        this->mScale.z = value;
        this->mScale.x = value;
        return FALSE;
    }
    f32 capY = this->mCapScale.y;
    this->mScale.set(0, 0, 0);
    if ((capY > 0.1f)) {
        kddoorAddCalcZero(&this->mCapScale.y, 0.5f, 0.3f);
        f32 value = this->mCapScale.y;
        this->mCapScale.z = value;
        this->mCapScale.x = value;
        return FALSE;
    }
    this->mCapScale.set(0, 0, 0);
    return TRUE;
}

/* 02196F54 */
void dDoor_ssk_sub_c::closeInit() {
    this->mScale.set(0, 0, 0);
    this->mCapScale.set(0, 0, 0);
    f32 random = kddoorRandom();
    this->mStarted = 0;
    this->mDelay = (s16)gabi::ftoi(gabi::fmuls_ppc(random, 5.0f));
}

/* 021972E0 */
BOOL dDoor_ssk_sub_c::closeProc(dDoor_info_c* i_door) {
    gabi::Local<csXyz> rotation;
    gabi::Local<u8[16]> callerLinkage;
    s16 timer = this->mDelay;
    if (timer > 0) {
        this->mDelay = timer - 1;
        return FALSE;
    }
    if (((f32)this->mCapScale.y < 0.9f)) {
        kddoorAddCalc(&this->mCapScale.y, 1.0f, 0.5f, 0.3f);
        f32 value = this->mCapScale.y;
        this->mCapScale.x = value;
        this->mCapScale.z = value;
        return FALSE;
    }
    u8 started = this->mStarted;
    this->mCapScale.set(1, 1, 1);
    if (!started) {
        this->mStarted = 1;
        sound(i_door, 0x694B);
        auto play = kddoorGameInfo();
        auto particle = play->mpParticle.get();
        auto color = &i_door->tevStr.mColorK0;
        kddoorParticle(particle, 0, 0x816C, &this->mPos, nullptr, nullptr, 255, nullptr, -1, color,
                       color, nullptr);
        rotation->x = i_door->shape_angle.x;
        u8 mFrontCheck = i_door->mFrontCheck;
        s16 yaw = i_door->shape_angle.y;
        rotation->z = i_door->shape_angle.z;
        s8 room = i_door->current.roomNo;
        rotation->y = mFrontCheck == 1 ? (s16)((s32)yaw + 0x7FFF) : yaw;
        play = kddoorGameInfo();
        particle = play->mpParticle.get();
        kddoorParticle(particle, 2, 0xA16D, &this->mPos, rotation.get(), nullptr, 185, &mSmoke,
                       room, nullptr, nullptr, nullptr);
    }
    if (((f32)this->mScale.y < 0.9f)) {
        kddoorAddCalc(&this->mScale.y, 1.0f, 0.3f, 0.3f);
        f32 value = this->mScale.y;
        this->mScale.x = value;
        this->mScale.z = value;
        return FALSE;
    }
    this->mScale.set(1, 1, 1);
    return TRUE;
}

/* 021964B8 */
BOOL dDoor_ssk_sub_c::drawSet() {
    gabi::Local<KddoorResourceName> bodyName, capName;
    gabi::Local<u8[16]> callerLinkage;
    bodyName->vtable = 0x10012A10;
    bodyName->text = gabi::at<char>(gabi::guest_addr("Ssk"));
    void* resource =
        kddoorObjectResource(sResourceControl.get(), bodyName.get(), dRes_INDEX_SSK_BDL_TURU_02_e);
    auto* body = kddoorConstructMorph(nullptr, resource, nullptr, nullptr, nullptr, 2, nullptr, -1,
                                      nullptr, nullptr, 0, 0x11020203, 1.0f);
    mpMorf = body;
    if (!body || !body->mpModel)
        return FALSE;
    J3DModel* model = body->mpModel;
    model->mUserArea = gabi::ea(this);
    J3DModelData* data = mpMorf->mpModel->mModelData;
    u16 count = data->getJointNum();
    for (u16 index = 0; index < count; ++index) {
        model = mpMorf->mpModel;
        data = model->mModelData;
        data->getJointNodePointer(index)->mCallBack = gabi::guest_fn((const void*)nodeCB);
        data = mpMorf->mpModel->mModelData;
        count = data->getJointNum();
    }
    capName->vtable = 0x10012A10;
    capName->text = gabi::at<char>(gabi::guest_addr("Ssk"));
    resource =
        kddoorObjectResource(sResourceControl.get(), capName.get(), dRes_INDEX_SSK_BDL_KTANA_00_e);
    auto* top = kddoorConstructMorph(nullptr, resource, nullptr, nullptr, nullptr, 2, nullptr, -1,
                                     nullptr, nullptr, 0, 0x11020203, 1.0f);
    body = mpMorf;
    mpCap = top;
    // Preserve the original body-model test, also present in GameCube.
    return body && body->mpModel;
}

/* 02196770 */
void dDoor_ssk_sub_c::calcMtx(dDoor_info_c* i_door, f32 x, f32 z, u32 side) {
    gabi::Local<cXyz> origin;
    gabi::Local<u8[16]> callerLinkage;
    auto* body = mpMorf.get();
    if (body) {
        J3DModel* model = body->mpModel;
        f32 xScale = mScale.x, yScale = mScale.y, zScale = mScale.z;
        model->mBaseScale.x = xScale;
        model->mBaseScale.y = yScale;
        model->mBaseScale.z = zScale;
        auto* top = mpCap.get();
        model = top->mpModel;
        xScale = mCapScale.x;
        yScale = mCapScale.y;
        zScale = mCapScale.z;
        model->mBaseScale.x = xScale;
        model->mBaseScale.z = zScale;
        model->mBaseScale.y = yScale;
        kddoorTranslate(&sMatrix, (f32)i_door->current.pos.x, (f32)i_door->current.pos.y,
                        (f32)i_door->current.pos.z);
        kddoorRotateY(&sMatrix, (s16)i_door->current.angle.y);
        if (side == 1)
            kddoorRotateY(&sMatrix, 0x7FFF);
        kddoorTranslateMatrix(x, 0.0f, z);
        kddoorRotateY(&sMatrix, (s16)mAngle);
        model = mpMorf->mpModel;
        copyMatrix(&sMatrix, &model->mBaseTransformMtx);
        model = mpCap->mpModel;
        copyMatrix(&sMatrix, &model->mBaseTransformMtx);
        origin->set(0, 0, 0);
        kddoorMultVec(&sMatrix, origin.get(), &mPos);
        kddoorCylinderCenter(&mCyl.mCyl, &mPos);
        f32 radius = gabi::fmuls_ppc(50.0f, (f32)mCapScale.x);
        kddoorCylinderRadius(&mCyl.mCyl, radius);
    }
}

/* 02196BF8 */
static BOOL CheckCreateHeap(daKddoor_c* a) { return a->CreateHeap(); }

/* 02196A70 */
BOOL daKddoor_c::CreateHeap() {
    // HD: this resource path uses the bmd directly, without the GameCube bdl fallback.

    auto data = kddoorStageResource("Stage", "door10.bmd");
    if (!data)
        kddoorAssert("d_a_kddoor.cpp", 0x383, "modelData != (0)");
    auto mpModel = kddoorCreateModel(data, 0, 0x11020203);
    this->mpModel = mpModel;
    if (!mpModel)
        return FALSE;
    if (this->chkMakeKey() && !kddoorKeyCreate(&this->mKey, 0))
        return FALSE;
    if (this->chkMakeStop())
        for (u32 i = 0; i < 3; ++i)
            if (!this->mStop.mArms[i].drawSet())
                return FALSE;
    auto background = kddoorCreateBackground(nullptr);
    this->mpBgW = background;
    if (!background)
        return FALSE;
    auto collision = kddoorStageResource("Stage", "door10.dzb");
    if (!collision)
        return FALSE;
    this->calcMtx();
    kddoorKeyMatrix(&this->mKey, this);
    this->mStop.calcMtx(this);
    mpModel = this->mpModel.get();
    auto matrix = mpModel ? &mpModel->mBaseTransformMtx : nullptr;
    return kddoorSetBackground(this->mpBgW.get(), collision, 1, matrix) == 0;
}

/* 0219807C */
void daKddoor_c::setEventPrm() {
    u8 other = this->mStop.mOtherEnabled;
    if (!this->mFrontCheck) {
        this->m2C6 = 2;
        if (other == 255) {
            other = (u8)this->chkStopB();
            this->mStop.mOtherEnabled = other;
        }
    } else {
        this->m2C6 = 3;
        if (other == 255) {
            other = (u8)this->chkStopF();
            this->mStop.mOtherEnabled = other;
        }
    }
    if (this->mStop.mbEnabled)
        return;
    u8 keyEnabled = this->mKey.mbEnabled;
    if (other == 1)
        this->m2C6 = (u8)this->m2C6 + 2;
    if (keyEnabled) {
        KddoorSave* save = sSave;
        if (!save->mKeyNum)
            return;
    }
    u8 sw = frontSwitch(this);
    if (this->chkFeelerCase() && sw != 255) {
        u8 room = frontRoom(this);
        if (!switchOn(sw, room))
            return;
    }
    if (kddoorCheckArea(this, 12100.0f, 12100.0f, 62500.0f)) {
        u8 m2C6 = this->m2C6;
        u16 flags = eventInfo.mCondition;
        s16 event = this->mEventIdx[m2C6];
        eventInfo.mEventId = event;
        u8 tool = this->mToolId[m2C6];
        eventInfo.mCondition = flags | 4;
        eventInfo.mMapToolId = tool;
    }
}

/* 02196BFC */
void daKddoor_c::openInit() {
    kddoorOpenInitCom(this, 1);
    this->mFlags |= FLAG_OPEN;
    auto play = kddoorGameInfo();
    kddoorReleaseBg(&play->mBgS, this->mpBgW.get());
    this->mTravel = 0;
    this->speedF = 0;
    sound(this, 0x6907);
}

/* 021970DC */
BOOL daKddoor_c::openProc() {
    kddoorChase(&this->speedF, 30.0f, 4.0f);
    BOOL done = kddoorChase(&this->mTravel, 300.0f, (f32)this->speedF) != 0;
    this->calcMtx();
    return done;
}

/* 02197154 */
void daKddoor_c::openEnd() {
    sound(this, 0x6909);
    u16 flags = this->mFlags;
    this->speedF = 0;
    this->mTravel = 300;
    this->mFlags = flags & ~FLAG_OPEN;
}

/* 02196C88 */
void daKddoor_c::closeInit() {
    this->mFlags |= FLAG_CLOSE;
    auto play = kddoorGameInfo();
    if (kddoorRegisterBg(&play->mBgS, this->mpBgW.get(), this))
        kddoorAssert("d_a_kddoor.cpp", 0x41E, "!rt");
    sMapSendStop = 0;
    sound(this, 0x6908);
}

/* 021971CC */
BOOL daKddoor_c::closeProc() {
    kddoorChase(&this->speedF, 60.0f, 6.0f);
    BOOL done = kddoorChase(&this->mTravel, 0.0f, (f32)this->speedF) != 0;
    this->calcMtx();
    return done;
}

/* 02197244 */
void daKddoor_c::closeEnd() {
    gabi::Local<cXyz> shock;
    gabi::Local<u8[16]> callerLinkage;
    this->mFlags &= ~FLAG_CLOSE;
    kddoorCloseEndCom(this);
    auto play = kddoorGameInfo();
    shock->set(0, 1, 0);
    kddoorShock(&play->mVibration, 4, -33, shock.get());
    sound(this, 0x690A);
}

/* 021966A0 */
void daKddoor_c::calcMtx() {
    kddoorTranslate(&sMatrix, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    kddoorRotateY(&sMatrix, (s16)home.angle.y);
    kddoorTranslateMatrix(0.0f, (f32)mTravel, 0.0f);
    copyMatrix(&sMatrix, &mpModel->mBaseTransformMtx);
}

/* 02198E08 */
BOOL daKddoor_c::CreateInit() {
    auto play = kddoorGameInfo();
    if (kddoorRegisterBg(&play->mBgS, this->mpBgW.get(), this))
        kddoorAssert("d_a_kddoor.cpp", 0x45D, "0");
    f32 attentionY = attention_info.position.y, eyeY = this->eyePos.y;
    u8 room = this->current.roomNo;
    tevStr.mRoomNo = room;
    this->mTravel = 0;
    attention_info.flags = fopAc_Attn_ACTION_DOOR_e;
    this->eyePos.y = gabi::fadds_ppc(eyeY, 150.0f);
    this->mAction = daKddoor_c::ACTION_INIT;
    attention_info.position.y = gabi::fadds_ppc(attentionY, 150.0f);
    this->calcMtx();
    kddoorMoveBackground(this->mpBgW.get());
    auto background = this->mpBgW.get();
    room = frontRoom(this);
    reinterpret_cast<KddoorBackground*>(background)->mRoomId = room;
    kddoorInitProc(this, 1);
    this->mStop.init(this);
    return TRUE;
}

/* 02198EEC */
s32 daKddoor_c::create() {
    s32 phase = kddoorResLoad(&this->mPhase, "Ssk");
    u32 condition = actor_condition;
    if (!(condition & 8)) {
        if (this) {
            kddoorConstructActor(this);
            __vtbl = 0x10012A38;
            kddoorConstructDoorSmoke(&this->mSmoke);
            kddoorConstructKey(&this->mKey);
            constructStop(&this->mStop);
            condition = actor_condition;
        }
        actor_condition = condition | 8;
    }
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (this->chkMakeKey()) {
        phase = kddoorKeyResLoad(&this->mKey);
        if (phase != cPhs_COMPLEATE_e)
            return phase;
    }
    this->current.roomNo = frontRoom(this);
    if (!kddoorEnterHeap(this, gabi::at<void>(gabi::guest_fn((const void*)CheckCreateHeap)),
                         0xD960)) {
        this->mpBgW = nullptr;
        return cPhs_ERROR_e;
    }
    this->CreateInit();
    return cPhs_COMPLEATE_e;
}

/* 021978D4 */
void daKddoor_c::demoProc() {
    u32 demoAction = kddoorDemoAction(this);
    s32 mStaffId = this->mStaffId;
    auto play = kddoorGameInfo();
    if (kddoorIsAdvance(&play->mEventManager, mStaffId)) {
        switch (demoAction) {
        case DEMO_OPEN_STOP:
            this->mStop.openInit();
            break;
        case DEMO_CLOSE_STOP:
            this->setStop();
            if (this->mStop.mbEnabled)
                this->mStop.closeInit();
            break;
        case DEMO_OPEN:
            this->openInit();
            break;
        case DEMO_CLOSE:
            this->closeInit();
            break;
        case DEMO_SMOKE_INIT:
            kddoorSmokeInit(&this->mSmoke, this);
            break;
        case DEMO_SMOKE_END:
            kddoorSmokeEnd(&this->mSmoke);
            break;
        case DEMO_GOAL:
            kddoorSetGoal(this);
            break;
        case DEMO_KEY:
            kddoorKeyInit(&this->mKey, this);
            break;
        }
    }
    switch (demoAction) {
    case DEMO_OPEN_STOP:
        if (this->mStop.openProc(this))
            cutEnd(this);
        this->mStop.calcMtx(this);
        break;
    case DEMO_CLOSE_STOP:
        if (this->mStop.closeProc(this))
            cutEnd(this);
        this->mStop.calcMtx(this);
        break;
    case DEMO_OPEN:
        if (!(this->mFlags & FLAG_OPEN))
            cutEnd(this);
        else if (this->openProc()) {
            this->openEnd();
            cutEnd(this);
        }
        break;
    case DEMO_CLOSE:
        if (!(this->mFlags & FLAG_CLOSE))
            cutEnd(this);
        else if (this->closeProc()) {
            this->closeEnd();
            cutEnd(this);
        }
        break;
    case DEMO_SMOKE_INIT:
        kddoorSmokeProc(&this->mSmoke, this);
        cutEnd(this);
        break;
    case DEMO_KEY:
        if (kddoorKeyProc(&this->mKey))
            cutEnd(this);
        kddoorKeyMatrix(&this->mKey, this);
        break;
    case DEMO_END:
        play = kddoorGameInfo();
        if (play->mEvent.mEventFlag & 1) {
            this->mAction = daKddoor_c::ACTION_WAIT;
            play = kddoorGameInfo();
            u16 flags = play->mEvent.mEventFlag;
            play->mEvent.mEventFlag = flags | 8;
            this->shape_angle.y = this->current.angle.y;
            play = kddoorGameInfo();
            flags = play->mEvent.mEventFlag;
            play->mEvent.mEventFlag = flags & ~1;
            play = kddoorGameInfo();
            if (kddoorStartDemoCheck(&play->mEventManager)) {
                play = kddoorGameInfo();
                kddoorCancelStartDemo(gabi::at<void>(gabi::ea(&play->mEventManager) + 0x24));
            }
        }
        cutEnd(this);
        break;
    default:
        cutEnd(this);
        break;
    }
}

/* 021981E4 */
static BOOL daKddoor_actionWait(daKddoor_c* i_this) {
    u16 command = i_this->eventInfo.mCommand;
    if (command == dEvtCmd_INDOOR_e) {
        kddoorInitOpenDemo(i_this, 1);
        i_this->mAction = daKddoor_c::ACTION_DEMO;
        i_this->demoProc();
        return TRUE;
    }
    if (i_this->mStop.mbEnabled) {
        if (command == dEvtCmd_INDEMO_e) {
            auto play = kddoorGameInfo();
            i_this->mStaffId = kddoorStaffId(&play->mEventManager, "SHUTTER_DOOR", 0, 0);
            u8 mFrontCheck = i_this->mFrontCheck;
            s16 yaw = i_this->current.angle.y;
            i_this->shape_angle.y = yaw;
            if (mFrontCheck == 1)
                i_this->shape_angle.y = (s16)((s32)yaw + 0x7FFF);
            i_this->mAction = daKddoor_c::ACTION_DEMO;
            i_this->demoProc();
            return TRUE;
        }
        if (i_this->chkStopOpen()) {
            i_this->setStopDemo();
            u8 m2C6 = i_this->m2C6;
            u8 tool = i_this->mToolId[m2C6];
            s16 event = i_this->mEventIdx[m2C6];
            kddoorOrderEvent(i_this, event, tool, 0xFFFF, 0, 1);
        }
        return TRUE;
    }
    if (i_this->chkStopClose()) {
        i_this->mStop.mbEnabled = 1;
        i_this->mStop.closeInit();
        i_this->mStop.calcMtx(i_this);
        i_this->mAction = daKddoor_c::ACTION_STOP_CLOSE;
    } else
        i_this->setEventPrm();
    return TRUE;
}

/* 02198338 */
static BOOL daKddoor_actionStopClose(daKddoor_c* i_this) {
    if (i_this->mStop.closeProc(i_this))
        i_this->mAction = daKddoor_c::ACTION_WAIT;
    i_this->mStop.calcMtx(i_this);
    return TRUE;
}

/* 0219838C */
static BOOL daKddoor_actionDemo(daKddoor_c* i_this) {
    s16 event = i_this->mEventIdx[i_this->m2C6];
    auto play = kddoorGameInfo();
    if (kddoorEndCheck(&play->mEventManager, event)) {
        i_this->mAction = daKddoor_c::ACTION_WAIT;
        play = kddoorGameInfo();
        u16 flags = play->mEvent.mEventFlag;
        play->mEvent.mEventFlag = flags | 8;
        i_this->shape_angle.y = i_this->current.angle.y;
    } else
        i_this->demoProc();
    return TRUE;
}

/* 02198498 */
static BOOL daKddoor_actionInit(daKddoor_c* i_this) {
    i_this->setKey();
    kddoorKeyMatrix(&i_this->mKey, i_this);
    i_this->setStop();
    i_this->mStop.calcMtx(i_this);
    daKddoor_actionWait(i_this);
    i_this->mAction = daKddoor_c::ACTION_WAIT;
    return TRUE;
}

/* 0219859C */
BOOL daKddoor_c::draw() {

    if (kddoorDrawCheck(this, 0)) {
        auto env = kddoorEnvlight();
        kddoorSettingTev(env, 0, &this->current.pos, &this->tevStr);
        env = kddoorEnvlight();
        kddoorLightColor(env, this->mpModel.get(), &this->tevStr);
        auto play = kddoorGameInfo();
        sDrawList = play->mBgDrawList;
        play = kddoorGameInfo();
        sXluDrawList = play->mBgXluDrawList;
        kddoorModelUpdate(this->mpModel.get(), 0);
        play = kddoorGameInfo();
        sDrawList = play->mDrawList;
        play = kddoorGameInfo();
        sXluDrawList = play->mXluDrawList;
        if (this->mKey.mbEnabled)
            kddoorKeyDraw(&this->mKey, this);
        if (this->mStop.mbEnabled)
            this->mStop.draw(this);
    }
    return TRUE;
}

/* 02198674 */
static BOOL daKddoor_Draw(daKddoor_c* i_this) { return i_this->draw(); }

/* 0219884C */
static BOOL daKddoor_Execute(daKddoor_c* i_this) {

    if (!sActionGuard) {
        sActionGuard = 1;
        kddoorCopyMemory(&sActions, &sActionPrototype, 0x10);
    }
    u32 state = kddoorCheckExecute(i_this);
    switch (state) {
    case 0:
        i_this->mAction = daKddoor_c::ACTION_INIT;
        break;
    case 1:
        kddoorStartDemo(i_this);
        i_this->demoProc();
        break;
    case 2:
        gabi::call_ptr(sActions.mActions[(u8)i_this->mAction].get(), i_this);
        break;
    default:
        kddoorAssert("d_a_kddoor.cpp", 0x59C, "0");
        break;
    }
    u8 enabled = i_this->mStop.mbEnabled;
    i_this->mRoomNo2 = sStayRoom;
    if (enabled)
        i_this->mStop.execute(i_this);
    return TRUE;
}

/* 02198978 */
static BOOL daKddoor_IsDelete(daKddoor_c* i_this) { return TRUE; }

/* 021989D8 */
static BOOL daKddoor_Delete(daKddoor_c* i_this) {
    // HD: this entry releases resources; the generated destructor has its own marked entry.
    if (i_this->heap) {
        auto play = kddoorGameInfo();
        kddoorReleaseBg(&play->mBgS, i_this->mpBgW.get());
    }
    i_this->mStop.end();
    kddoorResDelete(&i_this->mPhase, "Ssk");
    if (i_this->chkMakeKey())
        kddoorKeyResDelete(&i_this->mKey);
    kddoorSmokeEnd(&i_this->mSmoke);
    return TRUE;
}

/* 0219901C */
static s32 daKddoor_Create(daKddoor_c* i_this) { return i_this->create(); }

// HD: compiler-generated constructors, destructors and template code.

/* 02198A58 */
static dDoor_ssk_sub_c* constructArm(dDoor_ssk_sub_c* a) {
    // HD: initialize the full-range phase and yaw; the GameCube speed/frequency fields are absent.
    if (!a)
        a = (dDoor_ssk_sub_c*)kddoorAllocate(0x1C4);
    if (!a)
        return a;
    kddoorConstructSmoke(&a->mSmoke, 1);
    kddoorConstructStatus(&a->mStts);
    kddoorConstructGlobalStatus(&a->mStts.__vtbl_gstts);
    a->mStts.__vtbl = 0x1004AE88;
    a->mStts.__vtbl_gstts = 0x1004AEC0;
    kddoorConstructCollider(&a->mCyl);
    reinterpret_cast<KddoorCylinderStorage*>(&a->mCyl)->mShapeVtable = 0x100015A8;
    reinterpret_cast<KddoorCylinderStorage*>(&a->mCyl)->mAabVtable = 0x10012A28;
    kddoorConstructCylinder(&a->mCyl.mCyl);
    a->mCyl.__vtbl_hitinf = 0x1004B108;
    a->mCyl.mCyl.__vtbl = 0x1004B150;
    reinterpret_cast<KddoorCylinderStorage*>(&a->mCyl)->mShapeVtable = 0x1004B160;
    f32 random = kddoorRandom();
    a->mPhase = (s16)gabi::ftoi(gabi::fmuls_ppc(random, 65536.0f));
    random = kddoorRandom();
    f32 delta = gabi::fmuls_ppc(gabi::fsubs_ppc(random, 0.5f), 12000.0f);
    a->mScale.set(1, 1, 1);
    a->mCapScale.set(1, 1, 1);
    a->mAngle = (s16)gabi::ftoi(delta);
    return a;
}

/* 02198B8C */
static dDoor_ssk_c* constructStop(dDoor_ssk_c* s) {
    if (!s)
        s = static_cast<dDoor_ssk_c*>(kddoorAllocate(sizeof(dDoor_ssk_c)));
    if (!s)
        return s;
    kddoorConstructArray(s, 3, sizeof(dDoor_ssk_sub_c),
                         gabi::at<void>(gabi::guest_fn((const void*)constructArm)));
    f32 leading[6], trailing[8];
    u8 bytes[4];
    s16 shorts[4];
    auto* lights = reinterpret_cast<KddoorTevStorage*>(&s->mTevStr);
    for (u32 i = 0; i < 6; ++i) {
        leading[i] = sLightPrototype.mLeading[i];
        lights->mPrimary.mLeading[i] = leading[i];
    }
    for (u32 i = 0; i < 4; ++i) {
        bytes[i] = sLightPrototype.mBytes[i];
        lights->mPrimary.mBytes[i] = bytes[i];
    }
    for (u32 i = 0; i < 4; ++i) {
        shorts[i] = sLightPrototype.mShorts[i];
        lights->mPrimary.mShorts[i] = shorts[i];
    }
    for (u32 i = 0; i < 8; ++i)
        trailing[i] = sLightPrototype.mTrailing[i];
    // HD: construction copies all three light blocks in the expanded tev structure.
    KddoorLightBlock* copies[] = {&lights->mPrimary, &lights->mSecondary, &lights->mTertiary};
    for (auto* block : copies) {
        for (u32 i = 0; i < 6; ++i)
            block->mLeading[i] = leading[i];
        for (u32 i = 0; i < 4; ++i)
            block->mBytes[i] = bytes[i];
        for (u32 i = 0; i < 4; ++i)
            block->mShorts[i] = shorts[i];
        for (u32 i = 0; i < 8; ++i)
            block->mTrailing[i] = trailing[i];
    }
    return s;
}

/* 02199020 */
static void sinit_kddoor() {
    for (u32 i = 0; i < 4; ++i)
        sHeaderStatics.mSeeds[i] = 0;
    kddoorRegisterDestructor(&sHeaderDtor0);
    sHeaderStatics.mMin = -3.1415927410125732f;
    sHeaderStatics.mMax = 3.1415927410125732f;
    kddoorInitRandom(&sHeaderStatics.mRandomByte0);
    kddoorRegisterDestructor(&sHeaderDtor1);
    kddoorInitRandomByte(&sHeaderStatics.mRandomByte1);
    kddoorRegisterDestructor(&sHeaderDtor2);
}

/* 021990B4 */
static void stringDestructor(void* self, u32 flags) {
    if (self && (flags & 1))
        kddoorDelete(self);
}

/* 021990C8 */
static void armDestructor(dDoor_ssk_sub_c* a, u32 flags) {
    if (a) {
        kddoorDestroyCylinder(&a->mCyl, 2);
        kddoorDestroyStatus(&a->mStts, 2);
        if (flags & 1)
            kddoorDelete(a);
    }
}

/* 02199128 */
static void actorDestructor(daKddoor_c* a, u32 flags) {
    if (a) {
        kddoorDestroyArray(&a->mStop, 3, 0x1C4,
                           gabi::at<void>(gabi::guest_fn((const void*)armDestructor)), 0, 0);
        kddoorDestroyAnimation(gabi::at<void>(gabi::ea(&a->mKey) + 0x18), 0);
        kddoorDestroyActor(a, 0);
        if (flags & 1)
            kddoorDelete(a);
    }
}

/* 021991A8 */
static void terminateString(void* self) {}

// clang-format off
/* WWHD glue
data sResourceControl 101F4F28
data sLightPrototype 1016E414
data sHeaderStatics 10464B0C
data sHeaderDtor0 101B8104
data sHeaderDtor1 101B8110
data sHeaderDtor2 101B811C
data sActionGuard 101FDAC8
data sActions 101FDACC
data sActionPrototype 101B8090
data sCurrentModel 104B462C
data sCalcMatrix 1018C7B0
data sMatrix 1048D0CC
data sCurrentMatrix 104B4868
data sTrig 104A44F8
data sDrawList 104B4634
data sXluDrawList 104B4638
data sCylinderSource 101B80C0
str 10012A0C in 0219884C
str 10012A48
str 10012A54
str 10012A60 in 0219884C
str 10012A9C
str 10012AA4 in 02196A70
str 10012AB4
str 10012AC8
str 10012ACC in 02196C88
str 10012B10
str 10012B2C in 02198E08
str 10012B30 in 02198E08
str 10012B48 in 02198EEC
str 10012B48 in 021989D8
str 10012A84 in 021964B8
data sSave 101F84DC
data sRooms 1047E8E8
data sMapSendStop 1047A964
data sStayRoom 1047E6C8
ext 0252AA2C u8 kddoorType(daKddoor_c*)
ext 0252AA14 u8 kddoorFrontSwitch(daKddoor_c*)
ext 0252AA20 u8 kddoorBackSwitch(daKddoor_c*)
ext 0252A37C u8 kddoorFrontRoom(daKddoor_c*)
ext 0252A388 u8 kddoorBackRoom(daKddoor_c*)
ext 025BA0C0 BOOL kddoorIsSwitch(void*, u32, s32)
ext 025B9E38 void kddoorOnSwitch(void*, u32, s32)
ext 025E1A40 void kddoorStartSound(u32, cXyz*, u32, s32)
ext 02543280 void kddoorCutEnd(void*, s32)
ext 027F7878 KddoorJointInfo* kddoorJointInfo(void*)
ext 028E90D4 void kddoorCopyMatrix(void*, void*)
ext 025F1C28 void kddoorRotateY(void*, s32)
ext 025F1C5C void kddoorRotateZ(void*, s32)
ext 025C4334 BOOL kddoorRoomDisplayed(void*, u32)
ext 025D98E8 void* kddoorRoomEnemy(s8)
ext 0273AF40 void kddoorDelete(void*)
ext 02515A70 void kddoorDestroyCylinder(dCcD_Cyl*, u32)
ext 02515860 void kddoorDestroyStatus(dCcD_Stts*, u32)
ext 028F0164 void kddoorDestroyArray(void*, u32, u32, void*, u32, u32)
ext 027F3628 void kddoorDestroyAnimation(void*, u32)
ext 025D50BC void kddoorDestroyActor(daKddoor_c*, u32)
ext 028E93CC void kddoorTranslate(void*, f32, f32, f32)
ext 025F24E0 void kddoorTranslateMatrix(f32, f32, f32)
ext 02019788 f32 kddoorRandom()
ext 0252A3A0 void kddoorOpenInitCom(daKddoor_c*, s32)
ext 020087EC void kddoorReleaseBg(void*, dBgW*)
ext 024EEA6C s32 kddoorRegisterBg(void*, dBgW*, daKddoor_c*)
ext 0273AA24 void kddoorAssert(const char*, u32, const char*)
ext 0200F5C8 BOOL kddoorChase(be<f32>*, f32, f32)
ext 0252A550 void kddoorCloseEndCom(daKddoor_c*)
ext 025CB374 void kddoorShock(void*, u32, s32, cXyz*)
ext 02515F14 void kddoorInitStatus(dCcD_Stts*, u32, u32, void*)
ext 02516518 void kddoorSetCylinder(dCcD_Cyl*, void*)
ext 0255FFF4 void kddoorInitTev(dKy_tevstr_c*, s32, u32)
ext 025204C8 void kddoorResDelete(request_of_phase_process_class*, const char*)
ext 0252B494 void kddoorKeyResDelete(KddoorKey*)
ext 0252B3CC void kddoorSmokeEnd(void*)
ext 02555D0C void* kddoorEnvlight()
ext 025626A4 void kddoorSettingTev(void*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void kddoorLightColor(void*, void*, dKy_tevstr_c*)
ext 025E5590 void kddoorMorphEntry(mDoExt_McaMorf*)
ext 025E54D8 void kddoorMorphUpdate(mDoExt_McaMorf*)
ext 0252B0FC BOOL kddoorDrawCheck(daKddoor_c*, s32)
ext 025E2DE0 void kddoorModelUpdate(J3DModel*, u32)
ext 0252B9D4 void kddoorKeyDraw(KddoorKey*, daKddoor_c*)
ext 0273AD10 void* kddoorAllocate(u32)
ext 025A5B18 void kddoorConstructSmoke(KddoorSmokeCallback*, u32)
ext 0200BD2C void kddoorConstructStatus(dCcD_Stts*)
ext 02515DA0 void kddoorConstructGlobalStatus(void*)
ext 02515FB8 void kddoorConstructCollider(dCcD_Cyl*)
ext 02018590 void kddoorConstructCylinder(cM3dGCyl*)
ext 028F026C void kddoorRegisterDestructor(void*)
ext 028ED6F8 void kddoorInitRandom(void*)
ext 028EAB2C void kddoorInitRandomByte(void*)
ext 0252B848 void kddoorKeyOn(KddoorKey*)
ext 0252B5A8 void kddoorKeyOff(KddoorKey*)
ext 028EFFD0 void kddoorConstructArray(void*, u32, u32, void*)
ext 028E8F64 void kddoorMultVec(void*, cXyz*, cXyz*)
ext 020182E0 void kddoorCylinderCenter(cM3dGCyl*, cXyz*)
ext 020184DC void kddoorCylinderRadius(cM3dGCyl*, f32)
ext 0252447C void* kddoorStageResource(const char*, const char*)
ext 025E38E0 J3DModel* kddoorCreateModel(void*, u32, u32)
ext 0252B834 BOOL kddoorKeyCreate(KddoorKey*, s32)
ext 024F23F4 dBgW* kddoorCreateBackground(void*)
ext 0252B854 void kddoorKeyMatrix(KddoorKey*, daKddoor_c*)
ext 0200A030 s32 kddoorSetBackground(dBgW*, void*, u32, void*)
ext 025440C8 BOOL kddoorEndCheck(void*, s16)
ext 024F43DC void kddoorMoveBackground(dBgW*)
ext 0252A9E0 void kddoorInitProc(daKddoor_c*, u32)
ext 02520460 s32 kddoorResLoad(request_of_phase_process_class*, const char*)
ext 0252A244 void kddoorConstructActor(daKddoor_c*)
ext 0252B190 void kddoorConstructDoorSmoke(void*)
ext 0252B3D0 void kddoorConstructKey(KddoorKey*)
ext 0252B484 s32 kddoorKeyResLoad(KddoorKey*)
ext 025D63E8 BOOL kddoorEnterHeap(daKddoor_c*, void*, u32)
ext C000A848 void kddoorCopyMemory(void*, void*, u32)
ext 0252AC98 u32 kddoorCheckExecute(daKddoor_c*)
ext 0252AD50 void kddoorStartDemo(daKddoor_c*)
ext 0200EDC8 void kddoorAddCalcZero(be<f32>*, f32, f32)
ext 0200ED84 void kddoorAddCalc(be<f32>*, f32, f32, f32)
ext 025A847C void kddoorParticle(void*, u32, u32, cXyz*, void*, void*, u32, void*, s32, void*, void*, void*)
ext 026066C4 void* kddoorObjectResource(void*, KddoorResourceName*, s32)
ext 025E4F64 mDoExt_McaMorf* kddoorConstructMorph(void*, void*, void*, void*, void*, s32, void*, s32, void*, void*, u32, u32, f32)
ext 027F3F94 void* kddoorModelHeader(void*)
ext 0252AE04 BOOL kddoorCheckArea(daKddoor_c*, f32, f32, f32)
ext 0252A2DC void kddoorInitOpenDemo(daKddoor_c*, s32)
ext 02542D88 s32 kddoorStaffId(void*, const char*, u32, u32)
ext 025D7A58 void kddoorOrderEvent(daKddoor_c*, s16, u32, u32, u32, u32)
ext 025E55A0 void kddoorMorphCalc(mDoExt_McaMorf*)
ext 0200E240 void kddoorSetCollision(void*, dCcD_Cyl*)
ext 0252A684 u32 kddoorDemoAction(daKddoor_c*)
ext 025447C8 BOOL kddoorIsAdvance(void*, s32)
ext 0252B1E8 void kddoorSmokeInit(void*, daKddoor_c*)
ext 0252A6D0 void kddoorSetGoal(daKddoor_c*)
ext 0252B4A4 void kddoorKeyInit(KddoorKey*, daKddoor_c*)
ext 0252B2D8 void kddoorSmokeProc(void*, daKddoor_c*)
ext 0252B5B4 BOOL kddoorKeyProc(KddoorKey*)
ext 025449B0 BOOL kddoorStartDemoCheck(void*)
ext 0254351C void kddoorCancelStartDemo(void*)
ext 025200D4 KddoorGameInfo* kddoorGameInfo()
*/
// clang-format on
