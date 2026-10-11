/** Dragon Roost lava slab, WWHD.
 * Ported to the GameCube daObjMagmarock::Act_c names and HD layout.
 * Every marked function retains its verified behavior.
 */
#include "d/actor/d_a_obj_magmarock.h"
using MagmarockWord = be<u32>;
using MagmarockFloat = be<f32>;
using MagmarockByte = be<u8>;
static MagmarockWord l_resourceControl;
static MagmarockWord l_save;
static Mtx34 l_calcMtx;
static MagmarockWords4 l_identityQuat;
static MagmarockSincosTable l_sincos;
static MagmarockFloat l_quakeHeightBias;
static MagmarockFloat l_dampingBias;
static MagmarockFloat l_buoyancyBias;
static MagmarockColor l_effectColor;
static MagmarockWords4 l_headerZeros;
static MagmarockFloat l_piLow;
static MagmarockFloat l_piHigh;
static MagmarockByte l_headerAngle;
static MagmarockByte l_headerCircle;
static MagmarockDtorRecords l_headerDtors;
static MagmarockLightInfo l_defaultLight;
static daObjMagmarock_c *construct(daObjMagmarock_c *self);
static u8 *ptr(u32 a) { return gabi::at<u8>(a); }
// HD: the GHS process member stores this adjustment, virtual slot, and target.
static void process(u32 a, u32 fn) {
    gabi::at<daObjMagmarock_c>(a)->mProcess.d = (s16)(0);
    gabi::at<daObjMagmarock_c>(a)->mProcess.i = (s16)(-1);
    gabi::at<daObjMagmarock_c>(a)->mProcess.f = (u32)(fn);
}
static void shock() {
    u32 g = magmaGetPlay();
    gabi::Local<be<f32>[3]> axis;
    (*axis)[0] = 0;
    (*axis)[1] = 1;
    (*axis)[2] = 0;
    magmaStartShock(&gabi::at<MagmarockPlay>(g)->mVibration, 4, 1, axis.get());
}
static u32 resource(s32 index) {
    gabi::Local<be<u32>[2]> name;
    (*name)[0] = gabi::guest_str("Kyjim");
    (*name)[1] = magmarock_tu::safeString;
    return magmaGetResource(ptr((u32)l_resourceControl), name.get(), index);
}
static void sound(u32 a, u32 id, u32 position) {
    s32 rv = magmaGetReverb((s8)gabi::at<daObjMagmarock_c>(a)->current.roomNo);
    magmaSoundStart(id, ptr(position), 0, rv);
}

static u32 particle(u32 a, u32 group, u32 effect, u32 alpha) {
    u32 g = magmaGetPlay();
    return magmaParticleSet(ptr((u32)gabi::at<MagmarockPlay>(g)->mpParticleControl), group, effect,
                            &gabi::at<daObjMagmarock_c>(a)->current.pos, 0, 0, alpha, 0, -1, 0, 0,
                            0);
}
static void endParticle(gptr<u8> *slot) {
    u32 p = gabi::ea((u8 *)*slot);
    if (p) {
        u32 flags = gabi::at<MagmarockEmitter>(p)->mStatus;
        gabi::at<MagmarockEmitter>(p)->mFrameCount = 0xFFFFFFFFu;
        gabi::at<MagmarockEmitter>(p)->mStatus = flags | MAGMAROCK_EMITTER_INVALID;
        *slot = nullptr;
    }
}
static void moveParticle(u32 a, u32 p) {
    u8 kind = (u8)gabi::at<MagmarockEmitter>(p)->mKind;
    f32 y = (f32)gabi::at<daObjMagmarock_c>(a)->current.pos.y,
        x = (f32)gabi::at<daObjMagmarock_c>(a)->current.pos.x,
        z = (f32)gabi::at<daObjMagmarock_c>(a)->current.pos.z;
    if (kind >= 7)
        y = -y;
    gabi::at<MagmarockEmitter>(p)->mGlobalTranslation.y = y;
    gabi::at<MagmarockEmitter>(p)->mGlobalTranslation.x = x;
    gabi::at<MagmarockEmitter>(p)->mGlobalTranslation.z = z;
}

static s32 animationFrames(u32 p) {
    u32 table = (u32)gabi::at<MagmarockAnimation>(p)->__vtbl;
    return gabi::call_ptr<s32>((u32)gabi::at<MagmarockAnimationVtable>(table)->mpGetFrameCount,
                               ptr(p));
}

static bool isProcess(u32 a, u32 fn) {
    return (s16)gabi::at<daObjMagmarock_c>(a)->mProcess.i == -1 &&
           (s16)gabi::at<daObjMagmarock_c>(a)->mProcess.d == 0 &&
           (u32)gabi::at<daObjMagmarock_c>(a)->mProcess.f == fn;
}

// Apply the slab's warm light bias to the three signed TEV channels and RGB.
static void brighten(u32 a) {
    for (int i = 0; i < 3; i++) {
        s32 v = (s16) reinterpret_cast<be<s16> *>(
            &gabi::at<daObjMagmarock_c>(a)->mLighting.mColorC0)[i];
        reinterpret_cast<be<u16> *>(&gabi::at<daObjMagmarock_c>(a)->mLighting.mColorC0)[i] =
            (u8)(v + gabi::ftoi((f32)(255 - v) * 0.12f));
    }
    for (int i = 0; i < 3; i++) {
        s32 v =
            (u8) reinterpret_cast<be<u8> *>(&gabi::at<daObjMagmarock_c>(a)->mLighting.mColorK0)[i];
        reinterpret_cast<be<u8> *>(&gabi::at<daObjMagmarock_c>(a)->mLighting.mColorK0)[i] =
            (u8)(v + gabi::ftoi((f32)(255 - v) * 0.12f));
    }
}

/* 0236D210 */
void daObjMagmarock::Act_c::set_mtx() {
    u32 a = gabi::ea(this);
    f32 x = (f32)scale.x, y = (f32)scale.y, z = (f32)scale.z;
    u32 model = gabi::ea((void *)mpModel);
    gabi::at<MagmarockModel>(model)->mBaseScale.x = (f32)(x);
    gabi::at<MagmarockModel>(model)->mBaseScale.y = (f32)(y);
    gabi::at<MagmarockModel>(model)->mBaseScale.z = (f32)(z);
    magmaMtxTranslation(&l_calcMtx, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    magmaMtxZXYrot(&l_calcMtx, (s16)shape_angle.x, (s16)shape_angle.y, (s16)shape_angle.z);
    gabi::Local<be<f32>[4]> quat;
    magmaQuatMultiply(mCurrentQuat, mGroundQuat, quat.get());
    magmaMtxQuat(quat.get());
    f32 m[12];
    for (int i = 0; i < 12; i++)
        m[i] = (f32)l_calcMtx.m[i / 4][i % 4];
    model = gabi::ea((void *)mpModel);
    for (int i = 0; i < 12; i++)
        gabi::at<MagmarockModel>(model)->mBaseTRMtx.m[i / 4][i % 4] = m[i];
    magmaMtxCopy(&l_calcMtx, mBackgroundMatrix);
}

/* 0236DEC4 */
void daObjMagmarock::Act_c::demo_move() {
    u32 a = gabi::ea(this);
    if (magmaEventCheck(ptr((u32)l_save + 0x644), 0x380) || (u8)mStartStay)
        return;
    s16 state = (s16)mDemoState;
    if (!state) {
        if ((u16)eventInfo.mCommand == dEvtCmd_INDEMO_e) {
            mDemoState = (s16)(1);
            return;
        }
        magmaOrderEvent(this, "magma_cam", 1, 0xFFFF);
        eventInfo.mCondition = (u16)((u16)eventInfo.mCondition | 2);
        return;
    }
    if (state == 1) {
        u32 g = magmaGetPlay();
        s32 staff = magmaGetStaff(&gabi::at<MagmarockPlay>(g)->mEventManager, "Magrock", 0, 0);
        g = magmaGetPlay();
        s32 ended = magmaEndCheck(&gabi::at<MagmarockPlay>(g)->mEventManager, "magma_cam");
        g = magmaGetPlay();
        if (ended) {
            gabi::at<MagmarockPlay>(g)->mEventFlags =
                (u16)((u16)gabi::at<MagmarockPlay>(g)->mEventFlags | 8);
            mDemoState = (s16)((s16)((s16)mDemoState + 1));
            magmaEventOn(ptr((u32)l_save + 0x644), 0x380);
        } else
            magmaCutEnd(&gabi::at<MagmarockPlay>(g)->mEventManager, staff);
    }
}

/* 0236E054 */
void daObjMagmarock::Act_c::ControlEffect() {
    u32 a = gabi::ea(this);
    if ((s16)mLifted) {
        s16 before = (s16)mBeforeLift;
        u32 p = gabi::ea((void *)mpBeforeLiftEffect);
        if (before) {
            if (!p)
                mpBeforeLiftEffect = ptr(particle(a, 0, 0x8104, 255));
            else
                moveParticle(a, p);
            return;
        }
        endParticle(&mpBeforeLiftEffect);
        p = gabi::ea((void *)mpLiftEffect);
        if (!p) {
            shock();
            mpLiftEffect = ptr(particle(a, 2, 0x8105, 255));
        } else
            moveParticle(a, p);
    } else
        endParticle(&mpLiftEffect);
}

/* 0236E268 */
void daObjMagmarock::Act_c::play_anim() {
    u32 a = gabi::ea(this);
    s32 elapsed = (s32)mElapsedFrames;
    bool fade = false;
    if (elapsed > 375) {
        f32 frame = (f32)mBrkFrame;
        if (frame > 0) {
            mBrkFrame = (f32)(frame - 1.0f);
            elapsed = (s32)mElapsedFrames;
            fade = true;
        }
    }
    if (!fade && ((u32)elapsed - 15) >= 46) {
        s32 frames = animationFrames(gabi::ea((void *)mpBrkData));
        if ((f32)mBrkFrame < (f32)frames)
            mBrkFrame = (f32)((f32)mBrkFrame + 1.0f);
        elapsed = (s32)mElapsedFrames;
    }
    if (elapsed < 60) {
        s32 frames = animationFrames(gabi::ea((void *)mpBckData));
        if ((f32)mBckFrame < (f32)frames) {
            mBckFrame = (f32)((f32)mBckFrame + 1.0f);
            return;
        }
        elapsed = (s32)mElapsedFrames;
    }
    if (elapsed > 375) {
        f32 frame = (f32)mBckFrame;
        if (frame > 0)
            mBckFrame = (f32)(frame - 1.0f);
    }
}

/* 0236D4F8 */
void daObjMagmarock::Act_c::appear_proc_init() {
    mTimer = (u32)(30);
    process(gabi::ea(this), MAGMAROCK_APPEAR);
}

/* 0236E738 */
void daObjMagmarock::Act_c::appear_proc() {
    u32 a = gabi::ea(this);
    s32 timer = (s32)mTimer;
    if (timer == 10) {
        u32 g = magmaGetPlay();
        magmaBgRegister(&gabi::at<MagmarockPlay>(g)->mBgS, ptr(gabi::ea((void *)mpBackground)),
                        this);
        timer = (s32)mTimer;
    }
    if (!timer)
        this->wait_proc_init();
}

/* 0236E710 */
void daObjMagmarock::Act_c::wait_proc_init() {
    mTimer = (u32)(300);
    process(gabi::ea(this), MAGMAROCK_WAIT);
}

/* 0236E800 */
void daObjMagmarock::Act_c::wait_proc() {
    if (!(u32)mTimer)
        this->quake_proc_init();
}

/* 0236D46C */
void daObjMagmarock::Act_c::stay_proc_init() {
    u32 a = gabi::ea(this);
    u32 n = (u8)(u32)mParameters;
    if (n == 255)
        n = 0;
    mBrkFrame = (f32)(30);
    mBckFrame = (f32)(30);
    mElapsedFrames = (u32)(330);
    mTimer = (u32)(n * 15 + 30);
    u32 g = magmaGetPlay();
    magmaBgRegister(&gabi::at<MagmarockPlay>(g)->mBgS, ptr(gabi::ea((void *)mpBackground)), this);
    process(a, MAGMAROCK_STAY);
}

/* 0236E810 */
void daObjMagmarock::Act_c::stay_proc() {
    u32 a = gabi::ea(this);
    if ((u8)mHasRider) {
        u32 timer = (u32)mTimer;
        mTimer = (u32)(timer - 1);
        if (!timer)
            this->quake_proc_init();
    }
}

/* 0236E794 */
void daObjMagmarock::Act_c::quake_proc_init() {
    u32 a = gabi::ea(this);
    sound(a, 0x380F, a + 0x37C);
    mTimer = (u32)(45);
    process(a, MAGMAROCK_QUAKE);
}

/* 0236E85C */
void daObjMagmarock::Act_c::quake_proc() {
    u32 a = gabi::ea(this);
    mWobblePhase = (s16)((s16)((s16)mWobbleStep + (s16)mWobblePhase));
    magmaAddCalcAngle(&mWobbleStep, 0x1000, 2, 0x100);
    magmaAddCalc(&mWobbleAmplitude, (f32)l_quakeHeightBias + 750.0f, 0.25f, 50.0f);
    if (!(u32)mTimer)
        this->vanish_proc_init();
}

/* 0236E834 */
void daObjMagmarock::Act_c::vanish_proc_init() {
    mTimer = (u32)(90);
    process(gabi::ea(this), MAGMAROCK_VANISH);
}

/* 0236E8E8 */
void daObjMagmarock::Act_c::vanish_proc() {
    u32 a = gabi::ea(this);
    mWobblePhase = (s16)((s16)((s16)mWobblePhase + (s16)mWobbleStep));
    magmaAddCalcAngle(&mWobbleStep, 0, 4, 0x40);
    s32 timer = (s32)mTimer;
    if (timer == 80) {
        u32 g = magmaGetPlay();
        magmaBgRelease(&gabi::at<MagmarockPlay>(g)->mBgS, ptr(gabi::ea((void *)mpBackground)));
        timer = (s32)mTimer;
    }
    if (timer < 0)
        magmaActorDelete(this);
}

/* 0236D314 */
static void daObjMagmarock_ride_call_back(u8 *bg, daObjMagmarock_c *self, u8 *rider) {
    u32 a = gabi::ea(self);
    gabi::Local<be<u32>[3]> delta, axis, cross;
    magmaVectorDifference(&reinterpret_cast<fopAc_ac_c *>(rider)->current.pos, delta.get(),
                          &gabi::at<daObjMagmarock_c>(a)->current.pos);
    (*axis)[0] = 0;
    (*axis)[1] = 0x3F800000;
    (*axis)[2] = 0;
    magmaVectorCross(delta.get(), cross.get(), axis.get());
    for (int i = 0; i < 3; i++)
        (*delta)[i] = (u32)(*cross)[i];
    f32 magnitude = magmaVectorSquareMag(delta.get());
    magnitude = magmaSqrt(magnitude);
    if (magmaVectorNormalize(delta.get())) {
        f32 displacement = (f32)gabi::at<daObjMagmarock_c>(a)->current.pos.y -
                           (f32)gabi::at<daObjMagmarock_c>(a)->home.pos.y;
        f32 rate = gabi::fmadds(4.0f, displacement * 0.001f, 2.0f);
        magmaAddCalcAngle(&gabi::at<daObjMagmarock_c>(a)->mTiltAngle,
                          (s16)gabi::ftoi(-(magnitude * rate)), 8, 0x200);
        auto *table = &l_sincos.mEntries[(u16)gabi::at<daObjMagmarock_c>(a)->mTiltAngle >> 3];
        gabi::at<daObjMagmarock_c>(a)->mRiderFrames = (s16)(1);
        gabi::at<daObjMagmarock_c>(a)->mHasRider = (u8)(1);
        f32 s = (f32)table->mSin;
        for (int i = 0; i < 3; i++)
            gabi::at<daObjMagmarock_c>(a)->mRiderQuat[i] =
                (f32)gabi::at<be<f32>>(gabi::ea(delta.get()))[i] * s;
        gabi::at<daObjMagmarock_c>(a)->mRiderQuat[3] = (f32)((f32)table->mCos);
    }
}

/* 0236D20C */
static BOOL daObjMagmarock_CheckCreateHeap(daObjMagmarock_c *self) { return self->CreateHeap(); }

/* 0236CFC8 */
BOOL daObjMagmarock::Act_c::CreateHeap() {
    u32 a = gabi::ea(this);
    u32 modelData = resource(9);
    if (!modelData)
        magmaAssertion("d_a_obj_magmarock.cpp", 0x14D, "modelData != (0)");
    mpModel = ptr(magmaModelCreate(ptr(modelData), 0, 0x11020203));
    mpBrkData = ptr(resource(12));
    u32 anim = resource(6);
    mpBckData = ptr(anim);
    if (!gabi::ea((void *)mpBrkData)) {
        magmaAssertion("d_a_obj_magmarock.cpp", 0x155, "M_brk != (0)");
        anim = gabi::ea((void *)mpBckData);
    }
    if (!anim)
        magmaAssertion("d_a_obj_magmarock.cpp", 0x156, "M_bck != (0)");
    s32 brk = magmaBrkInit(mBrkController, ptr(modelData), ptr(gabi::ea((void *)mpBrkData)), 0, 2,
                           0, 1.0f, -1, 0, 0);
    s32 bck = magmaBckInit(mBckController, ptr(modelData), ptr(gabi::ea((void *)mpBckData)), 0, 2,
                           0, 1.0f, -1, 0);
    magmaMtxTranslation(&l_calcMtx, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    magmaMtxYrot(&l_calcMtx, (s16)shape_angle.y);
    magmaMtxScale((f32)scale.x, (f32)scale.y, (f32)scale.z);
    magmaMtxCopy(&l_calcMtx, mBackgroundMatrix);
    u32 bg = magmaBgNew(ptr(resource(15)), 1, mBackgroundMatrix);
    mpBackground = ptr(bg);
    if (!bg)
        return 0;
    gabi::at<MagmarockBackground>(bg)->mpCorrection = (u32)(magmarock_tu::typicalMoveBg);
    return gabi::ea((void *)mpModel) && brk && bck;
}

/* 0236D520 */
BOOL daObjMagmarock::Act_c::CreateInit() {
    // HD: independent light information copies preserve float versus word copy ordering.
    u32 a = gabi::ea(this);
    auto *actorLighting = reinterpret_cast<MagmarockTevStr *>(&tevStr);
    scale.x = (f32)(1);
    u32 model = gabi::ea((void *)mpModel);
    scale.y = (f32)(1);
    scale.z = (f32)(1);
    cullMtx = (u32)(model ? model + 0xC8 : 0);
    magmaCullBox(this, -200.0f, -30.0f, -200.0f, 200.0f, 30.0f, 200.0f);
    model = gabi::ea((void *)mpModel);
    magmaMtxCopy(ptr(model ? model + 0xC8 : 0), mBackgroundMatrix);
    f32 x = (f32)current.pos.x, y = (f32)current.pos.y;
    mWobblePhase = (s16)(0);
    mWobbleStep = (s16)(0);
    home.pos.x = (f32)(x);
    mDemoState = (s16)(0);
    mUnused = (u32)(0);
    speed.x = (f32)(0);
    speed.y = (f32)(0);
    f32 homeY = y + 15.0f;
    f32 z = (f32)current.pos.z;
    speed.z = (f32)(0);
    gravity = (f32)(-2.5f);
    mTiltAngle = (s16)(0);
    mUnusedAngle = (s16)(0);
    home.pos.z = (f32)(z);
    mWobbleAmplitude = (f32)(0);
    mHasRider = (u8)(0);
    mTimer = (u32)(30);
    home.pos.y = (f32)(homeY);
    mBrkFrame = (f32)(0);
    mElapsedFrames = (u32)(0);
    for (int i = 0; i < 4; i++) {
        u32 value = (u32)l_identityQuat.mWords[i];
        *reinterpret_cast<be<u32> *>(&mGroundQuat[i]) = value;
        *reinterpret_cast<be<u32> *>(&mRiderQuat[i]) = value;
        *reinterpret_cast<be<u32> *>(&mCurrentQuat[i]) = value;
    }
    this->set_mtx();
    gabi::at<MagmarockBackground>(gabi::ea((void *)mpBackground))->mpRideCallback =
        gabi::guest_fn(reinterpret_cast<const void *>(daObjMagmarock_ride_call_back));
    u8 stayFlag = ((u32)mParameters >> 24);
    mStartStay = (u8)(stayFlag);
    if (stayFlag) {
        this->stay_proc_init();
        return 1;
    }
    this->appear_proc_init();
    if (gabi::ea((void *)mpAppearEffect))
        return 1;
    sound(a, 0x380E, a + 0x37C);
    shock();
    // Copy the existing actor lighting to the slab's independent lighting record.
    mLighting.mLightInfo.mColor.r = (u8)((u8)actorLighting->mLightInfo.mColor.r);
    mLighting.mLightInfo.mControls[0] = (s16)((s16)actorLighting->mLightInfo.mControls[0]);
    mLighting.mLightInfo.mControls[1] = (s16)((s16)actorLighting->mLightInfo.mControls[1]);
    mLighting.mLightInfo.mColor.g = (u8)((u8)actorLighting->mLightInfo.mColor.g);
    mLighting.mLightInfo.mColor.b = (u8)((u8)actorLighting->mLightInfo.mColor.b);
    mLighting.mLightInfo.mControls[2] = (s16)((s16)actorLighting->mLightInfo.mControls[2]);
    mLighting.mLightInfo.mPosition.x = (f32)((f32)actorLighting->mLightInfo.mPosition.x);
    mLighting.mLightInfo.mColor.a = (u8)((u8)actorLighting->mLightInfo.mColor.a);
    mLighting.mLightInfo.mPosition.y = (f32)((f32)actorLighting->mLightInfo.mPosition.y);
    mLighting.mLightInfo.mPosition.z = (f32)((f32)actorLighting->mLightInfo.mPosition.z);
    mLighting.mLightInfo.mDirection.x = (f32)((f32)actorLighting->mLightInfo.mDirection.x);
    mLighting.mLightInfo.mControls[3] = (s16)((s16)actorLighting->mLightInfo.mControls[3]);
    mLighting.mColorC0.r = (u16)((u16)actorLighting->mColorC0.r);
    mLighting.mLightInfo.mDirection.y = (f32)((f32)actorLighting->mLightInfo.mDirection.y);
    mLighting.mLightInfo.mDirection.z = (f32)((f32)actorLighting->mLightInfo.mDirection.z);
    mLighting.mLightInfo.mAttenuation[0] = (f32)((f32)actorLighting->mLightInfo.mAttenuation[0]);
    mLighting.mLightInfo.mAttenuation[1] = (f32)((f32)actorLighting->mLightInfo.mAttenuation[1]);
    mLighting.mLightInfo.mAttenuation[2] = (f32)((f32)actorLighting->mLightInfo.mAttenuation[2]);
    mLighting.mLightInfo.mAttenuation[3] = (f32)((f32)actorLighting->mLightInfo.mAttenuation[3]);
    mLighting.mColorC0.g = (u16)((u16)actorLighting->mColorC0.g);
    mLighting.mColorC0.b = (u16)((u16)actorLighting->mColorC0.b);
    *reinterpret_cast<be<u32> *>(&mLighting.mLightPosWorld.x) =
        (u32)((u32) * reinterpret_cast<be<u32> *>(&actorLighting->mLightPosWorld.x));
    *reinterpret_cast<be<u32> *>(&mLighting.mLightPosWorld.y) =
        (u32)((u32) * reinterpret_cast<be<u32> *>(&actorLighting->mLightPosWorld.y));
    *reinterpret_cast<be<u32> *>(&mLighting.mLightPosWorld.z) =
        (u32)((u32) * reinterpret_cast<be<u32> *>(&actorLighting->mLightPosWorld.z));
    mLighting.mColorC0.a = (u16)((u16)actorLighting->mColorC0.a);
    mLighting.mLightInfo.mAttenuation[4] = (f32)((f32)actorLighting->mLightInfo.mAttenuation[4]);
    mLighting.mLightInfo.mAttenuation[5] = (f32)((f32)actorLighting->mLightInfo.mAttenuation[5]);
    mLighting.mLightInfo.mAttenuation[6] = (f32)((f32)actorLighting->mLightInfo.mAttenuation[6]);
    mLighting.mLightInfo.mAttenuation[7] = (f32)((f32)actorLighting->mLightInfo.mAttenuation[7]);
    mLighting.mFogStartZ = (f32)((f32)actorLighting->mFogStartZ);
    mLighting.mFogEndZ = (f32)((f32)actorLighting->mFogEndZ);
    mLighting.mFogColor.r = (u16)((u16)actorLighting->mFogColor.r);
    mLighting.mColpatBlend = (f32)((f32)actorLighting->mColpatBlend);
    mLighting.mHdLightInfo.mPosition.x = (f32)((f32)actorLighting->mHdLightInfo.mPosition.x);
    mLighting.mHdLightInfo.mPosition.y = (f32)((f32)actorLighting->mHdLightInfo.mPosition.y);
    mLighting.mHdLightInfo.mPosition.z = (f32)((f32)actorLighting->mHdLightInfo.mPosition.z);
    mLighting.mInitTimer = (u8)((u8)actorLighting->mInitTimer);
    mLighting.mEnvrIdxCurr = (u8)((u8)actorLighting->mEnvrIdxCurr);
    mLighting.mEnvrIdxPrev = (u8)((u8)actorLighting->mEnvrIdxPrev);
    mLighting.mColpatCurr = (u8)((u8)actorLighting->mColpatCurr);
    mLighting.mColpatPrev = (u8)((u8)actorLighting->mColpatPrev);
    mLighting.mRoomNo = (u8)((u8)actorLighting->mRoomNo);
    mLighting.mEnvrIdxOverride = (u8)((u8)actorLighting->mEnvrIdxOverride);
    mLighting.mFogColor.g = (u16)((u16)actorLighting->mFogColor.g);
    mLighting.mFogColor.b = (u16)((u16)actorLighting->mFogColor.b);
    mLighting.mLightMode = (u8)((u8)actorLighting->mLightMode);
    mLighting.mInitType = (u8)((u8)actorLighting->mInitType);
    mLighting.mHdLightInfo.mColor.r = (u8)((u8)actorLighting->mHdLightInfo.mColor.r);
    mLighting.mHdLightInfo.mColor.g = (u8)((u8)actorLighting->mHdLightInfo.mColor.g);
    mLighting.mHdLightInfo.mColor.b = (u8)((u8)actorLighting->mHdLightInfo.mColor.b);
    mLighting.mHdLightInfo.mColor.a = (u8)((u8)actorLighting->mHdLightInfo.mColor.a);
    mLighting.mHdLightInfo.mDirection.x = (f32)((f32)actorLighting->mHdLightInfo.mDirection.x);
    mLighting.mHdLightInfo.mDirection.y = (f32)((f32)actorLighting->mHdLightInfo.mDirection.y);
    mLighting.mHdLightInfo.mDirection.z = (f32)((f32)actorLighting->mHdLightInfo.mDirection.z);
    mLighting.mHdLightInfo.mAttenuation[0] =
        (f32)((f32)actorLighting->mHdLightInfo.mAttenuation[0]);
    mLighting.mHdLightInfo2.mPosition.x = (f32)((f32)actorLighting->mHdLightInfo2.mPosition.x);
    mLighting.mHdLightInfo.mAttenuation[1] =
        (f32)((f32)actorLighting->mHdLightInfo.mAttenuation[1]);
    mLighting.mHdLightInfo.mAttenuation[2] =
        (f32)((f32)actorLighting->mHdLightInfo.mAttenuation[2]);
    mLighting.mHdLightInfo.mAttenuation[3] =
        (f32)((f32)actorLighting->mHdLightInfo.mAttenuation[3]);
    mLighting.mHdLightInfo.mAttenuation[4] =
        (f32)((f32)actorLighting->mHdLightInfo.mAttenuation[4]);
    mLighting.mHdLightInfo.mAttenuation[5] =
        (f32)((f32)actorLighting->mHdLightInfo.mAttenuation[5]);
    mLighting.mHdLightInfo.mControls[0] = (s16)((s16)actorLighting->mHdLightInfo.mControls[0]);
    mLighting.mHdLightInfo.mControls[1] = (s16)((s16)actorLighting->mHdLightInfo.mControls[1]);
    mLighting.mHdLightInfo.mAttenuation[6] =
        (f32)((f32)actorLighting->mHdLightInfo.mAttenuation[6]);
    mLighting.mHdLightInfo.mAttenuation[7] =
        (f32)((f32)actorLighting->mHdLightInfo.mAttenuation[7]);
    mLighting.mHdLightInfo.mControls[2] = (s16)((s16)actorLighting->mHdLightInfo.mControls[2]);
    mLighting.mHdLightInfo.mControls[3] = (s16)((s16)actorLighting->mHdLightInfo.mControls[3]);
    mLighting.mFogColor.a = (u16)((u16)actorLighting->mFogColor.a);
    mLighting.mHdLightInfo2.mControls[0] = (s16)((s16)actorLighting->mHdLightInfo2.mControls[0]);
    mLighting.mHdLightInfo2.mControls[1] = (s16)((s16)actorLighting->mHdLightInfo2.mControls[1]);
    mLighting.mHdLightInfo2.mControls[2] = (s16)((s16)actorLighting->mHdLightInfo2.mControls[2]);
    mLighting.mHdLightInfo2.mControls[3] = (s16)((s16)actorLighting->mHdLightInfo2.mControls[3]);
    mLighting.mHdLightInfo2.mColor.r = (u8)((u8)actorLighting->mHdLightInfo2.mColor.r);
    mLighting.mHdLightInfo2.mColor.g = (u8)((u8)actorLighting->mHdLightInfo2.mColor.g);
    mLighting.mHdLightInfo2.mColor.b = (u8)((u8)actorLighting->mHdLightInfo2.mColor.b);
    mLighting.mHdLightInfo2.mColor.a = (u8)((u8)actorLighting->mHdLightInfo2.mColor.a);
    mLighting.mHdLightInfo2.mPosition.y = (f32)((f32)actorLighting->mHdLightInfo2.mPosition.y);
    mLighting.mHdLightInfo2.mPosition.z = (f32)((f32)actorLighting->mHdLightInfo2.mPosition.z);
    mLighting.mHdLightInfo2.mDirection.x = (f32)((f32)actorLighting->mHdLightInfo2.mDirection.x);
    mLighting.mHdLightInfo2.mDirection.y = (f32)((f32)actorLighting->mHdLightInfo2.mDirection.y);
    mLighting.mHdLightInfo2.mDirection.z = (f32)((f32)actorLighting->mHdLightInfo2.mDirection.z);
    mLighting.mHdLightInfo2.mAttenuation[0] =
        (f32)((f32)actorLighting->mHdLightInfo2.mAttenuation[0]);
    mLighting.mHdLightInfo2.mAttenuation[1] =
        (f32)((f32)actorLighting->mHdLightInfo2.mAttenuation[1]);
    mLighting.mHdLightInfo2.mAttenuation[2] =
        (f32)((f32)actorLighting->mHdLightInfo2.mAttenuation[2]);
    mLighting.mHdLightInfo2.mAttenuation[3] =
        (f32)((f32)actorLighting->mHdLightInfo2.mAttenuation[3]);
    mLighting.mHdLightInfo2.mAttenuation[4] =
        (f32)((f32)actorLighting->mHdLightInfo2.mAttenuation[4]);
    mLighting.mHdLightInfo2.mAttenuation[5] =
        (f32)((f32)actorLighting->mHdLightInfo2.mAttenuation[5]);
    mLighting.mHdLightInfo2.mAttenuation[6] =
        (f32)((f32)actorLighting->mHdLightInfo2.mAttenuation[6]);
    mLighting.mHdLightInfo2.mAttenuation[7] =
        (f32)((f32)actorLighting->mHdLightInfo2.mAttenuation[7]);
    *reinterpret_cast<be<u32> *>(&mLighting.mColorK0.r) =
        (u32)((u32) * reinterpret_cast<be<u32> *>(&actorLighting->mColorK0.r));
    *reinterpret_cast<be<u32> *>(&mLighting.mColorK1.r) =
        (u32)((u32) * reinterpret_cast<be<u32> *>(&actorLighting->mColorK1.r));
    u32 env = magmaGetEnvironment();
    magmaSettingTev(ptr(env), 0, &current.pos, &mLighting);
    brighten(a);
    u8 alpha = (u8)gabi::ftoi(gabi::fmadds(102.0f, (f32)l_dampingBias, 153.0f));
    mpAppearEffect = ptr(particle(a, 2, 0x8072, alpha));
    alpha = (u8)gabi::ftoi(gabi::fmadds(102.0f, (f32)l_buoyancyBias, 153.0f));
    u32 second = particle(a, 2, 0x8073, alpha);
    u32 first = gabi::ea((void *)mpAppearEffect);
    mpAppearEffect2 = ptr(second);
    if (first) {
        for (int i = 0; i < 3; i++)
            reinterpret_cast<be<u8> *>(&gabi::at<MagmarockEmitter>(first)->mPrmColor)[i] =
                (u8)l_effectColor.mChannels[i];
        first = gabi::ea((void *)mpAppearEffect);
        for (int i = 0; i < 3; i++)
            reinterpret_cast<be<u8> *>(&gabi::at<MagmarockEmitter>(first)->mEnvColor)[i] =
                (u8)l_effectColor.mChannels[i];
        second = gabi::ea((void *)mpAppearEffect2);
    }
    if (second) {
        for (int i = 0; i < 3; i++)
            reinterpret_cast<be<u8> *>(&gabi::at<MagmarockEmitter>(second)->mPrmColor)[i] =
                (u8)l_effectColor.mChannels[i];
        second = gabi::ea((void *)mpAppearEffect2);
        for (int i = 0; i < 3; i++)
            reinterpret_cast<be<u8> *>(&gabi::at<MagmarockEmitter>(second)->mEnvColor)[i] =
                (u8)l_effectColor.mChannels[i];
    }
    return 1;
}

/* 0236E400 */
BOOL daObjMagmarock::Act_c::LiftUpRequest(u8 *position) {
    u32 a = gabi::ea(this), p = gabi::ea(position);
    *reinterpret_cast<be<u32> *>(&mLiftTarget[0]) =
        (u32)((u32) * reinterpret_cast<be<u32> *>(&gabi::at<cXyz>(p)->x));
    *reinterpret_cast<be<u32> *>(&mLiftTarget[1]) =
        (u32)((u32) * reinterpret_cast<be<u32> *>(&gabi::at<cXyz>(p)->y));
    *reinterpret_cast<be<u32> *>(&mLiftTarget[2]) =
        (u32)((u32) * reinterpret_cast<be<u32> *>(&gabi::at<cXyz>(p)->z));
    if (!isProcess(a, MAGMAROCK_WAIT)) {
        if (isProcess(a, MAGMAROCK_APPEAR)) {
            gabi::Local<be<f32>[3]> delta;
            magmaVectorDifference(&current.pos, delta.get(), mLiftTarget);
            (*delta)[1] = 0;
            if (!magmaVectorNormalize(delta.get())) {
                (*delta)[0] = 0;
                (*delta)[1] = 0;
                (*delta)[2] = 1;
            }
            magmaVectorScale(delta.get(), delta.get(), 10.0f);
            magmaVectorAdd(&current.pos, delta.get(), &current.pos);
        }
        return 0;
    }
    magmaAddCalcPos(&current.pos, position, 0.05f, 5.0f);
    magmaAddCalc(&mWobbleAmplitude, 750.0f, 0.5f, 40.0f);
    magmaAddCalcAngle(&mWobbleStep, 0x1200, 4, 0x100);
    mWobblePhase = (s16)((s16)((s16)mWobblePhase + (s16)mWobbleStep));
    magmaAddCalc(&current.pos.y, (f32)gabi::at<cXyz>(p)->y, 0.25f, 150.0f);
    mLifted = (s16)(1);
    return 1;
}

/* 0236E5BC */
BOOL daObjMagmarock::Act_c::BeforeLiftRequest(u8 *position) {
    u32 a = gabi::ea(this), p = gabi::ea(position);
    f32 minY = (f32)this->home.pos.y + 25.0f;
    mLiftTarget[0] = (f32)((f32)gabi::at<cXyz>(p)->x);
    f32 y = (f32)gabi::at<cXyz>(p)->y;
    mLiftTarget[1] = (f32)(y);
    mLiftTarget[2] = (f32)((f32)gabi::at<cXyz>(p)->z);
    if (y < minY)
        mLiftTarget[1] = (f32)(minY);
    if (!isProcess(a, MAGMAROCK_WAIT))
        return 0;
    magmaAddCalcPos(&current.pos, mLiftTarget, 0.05f, 5.0f);
    magmaAddCalc(&mWobbleAmplitude, 500.0f, 0.25f, 20.0f);
    magmaAddCalcAngle(&mWobbleStep, 0xA00, 8, 0x100);
    mWobblePhase = (s16)((s16)((s16)mWobblePhase + (s16)mWobbleStep));
    magmaAddCalc(&current.pos.y, (f32)mLiftTarget[1], 0.25f, 150.0f);
    mBeforeLift = (s16)(1);
    mLifted = (s16)(1);
    return 1;
}

/* 0236DD08 */
void daObjMagmarock::Act_c::calc_ground_quat() {
    // HD: magma height queries retain each play-object access and the missing-packet fallback.
    u32 a = gabi::ea(this);
    u32 g = magmaGetPlay();
    f32 height;
    if ((u32)gabi::at<MagmarockPlay>(g)->mpMagmaPacket) {
        g = magmaGetPlay();
        height = magmaCheckY(ptr((u32)gabi::at<MagmarockPlay>(g)->mpMagmaPacket), &current.pos);
    } else
        height = (f32)current.pos.y - 10.0f;
    if (height > -99999992.0f)
        home.pos.y = (f32)(height + 25.0f);
    home.pos.x = (f32)((f32)current.pos.x);
    mGroundTriangle[8] = (f32)(-60);
    mGroundTriangle[2] = (f32)(120);
    mGroundTriangle[6] = (f32)(-103.9f);
    mGroundTriangle[4] = (f32)(0);
    mGroundTriangle[1] = (f32)(0);
    mGroundTriangle[7] = (f32)(0);
    mGroundTriangle[0] = (f32)(0);
    mGroundTriangle[5] = (f32)(-60);
    home.pos.z = (f32)((f32)current.pos.z);
    mGroundTriangle[3] = (f32)(103.9f);
    for (int i = 0; i < 3; i++) {
        u32 v = gabi::ea(&mGroundTriangle[i * 3]);
        magmaVectorAdd(ptr(v), &home.pos, ptr(v));
        g = magmaGetPlay();
        if ((u32)gabi::at<MagmarockPlay>(g)->mpMagmaPacket) {
            g = magmaGetPlay();
            height = magmaCheckY(ptr((u32)gabi::at<MagmarockPlay>(g)->mpMagmaPacket), ptr(v));
        } else
            height = (f32)current.pos.y - 10.0f;
        if (height > -99999992.0f)
            gabi::at<cXyz>(v)->y = height + 15.0f;
    }
    magmaQuatFromTriangle(mGroundQuat, &mGroundTriangle[0], &mGroundTriangle[3],
                          &mGroundTriangle[6], 0.25f);
}

/* 0236E964 */
static s32 daObjMagmarock_Create(daObjMagmarock_c *self) {
    u32 a = gabi::ea(self);
    u32 status = (u32)gabi::at<daObjMagmarock_c>(a)->actor_condition;
    if (!(status & fopAcCnd_INIT_e)) {
        if (self) {
            construct(self);
            status = (u32)gabi::at<daObjMagmarock_c>(a)->actor_condition;
        }
        gabi::at<daObjMagmarock_c>(a)->actor_condition = (u32)(status | fopAcCnd_INIT_e);
    }
    s32 phase = magmaResLoad(gabi::at<daObjMagmarock_c>(a)->mPhase, "Kyjim");
    if (phase == cPhs_COMPLEATE_e) {
        u32 g = magmaGetPlay();
        if (!(u32)gabi::at<MagmarockPlay>(g)->mpMagmaPacket)
            return 0;
        if (!magmaEntryHeap(
                self,
                ptr(gabi::guest_fn(reinterpret_cast<const void *>(daObjMagmarock_CheckCreateHeap))),
                0x5D40))
            return cPhs_ERROR_e;
        self->CreateInit();
    }
    return phase;
}

/* 0236EA48 */
static BOOL daObjMagmarock_Delete(daObjMagmarock_c *self) {
    u32 a = gabi::ea(self);
    magmaResDelete(gabi::at<daObjMagmarock_c>(a)->mPhase, "Kyjim");
    if (gabi::ea((void *)gabi::at<daObjMagmarock_c>(a)->heap) &&
        (u32)gabi::at<MagmarockBackground>(
            gabi::ea((void *)gabi::at<daObjMagmarock_c>(a)->mpBackground))
                ->mIndex < 256) {
        u32 g = magmaGetPlay();
        magmaBgRelease(&gabi::at<MagmarockPlay>(g)->mBgS,
                       ptr(gabi::ea((void *)gabi::at<daObjMagmarock_c>(a)->mpBackground)));
    }
    return 1;
}

/* 0236EAB0 */
BOOL daObjMagmarock::Act_c::_execute() {
    u32 a = gabi::ea(this);
    this->calc_ground_quat();
    f32 y, home, limit;
    if (!(s16)mLifted) {
        if (!isProcess(a, MAGMAROCK_QUAKE) && !isProcess(a, MAGMAROCK_VANISH)) {
            magmaAddCalc(&mWobbleAmplitude, 0.0f, 0.2f, 20.0f);
            magmaAddCalcAngle(&mWobbleStep, 0, 4, 0x100);
        }
        y = (f32)current.pos.y;
        f32 velocity = (f32)speed.y;
        home = (f32)this->home.pos.y;
        limit = home + 100.0f;
        y = y + velocity;
        velocity = velocity + (f32)gravity;
        current.pos.y = (f32)(y);
        speed.y = (f32)(velocity);
    } else {
        home = (f32)this->home.pos.y;
        limit = home + 100.0f;
        y = (f32)current.pos.y;
        speed.y = (f32)(0);
    }
    // Buoyancy and damping once the slab is below the lava surface.
    if (y < limit) {
        if (!((f32)old.pos.y < limit)) {
            shock();
            y = (f32)current.pos.y;
            home = (f32)this->home.pos.y;
        }
        if (!(y < home)) {
            speed.y = (f32)((f32)speed.y * (0.65f - (f32)l_dampingBias));
        } else {
            f32 bottom = home - 30.0f;
            if (!(y >= bottom)) {
                y = bottom;
                home = (f32)this->home.pos.y;
                current.pos.y = (f32)(y);
            }
            f32 spring = (f32)l_buoyancyBias + 0.4f;
            f32 delta = y - home;
            f32 velocity = gabi::fnmsubs(spring, delta, (f32)speed.y);
            speed.y = (f32)(velocity);
            speed.y = (f32)(velocity * (0.65f - (f32)l_dampingBias));
        }
    }
    if (!(s16)mLifted && !isProcess(a, MAGMAROCK_STAY)) {
        mTimer = (u32)((u32)mTimer - 1);
        mElapsedFrames = (u32)((u32)mElapsedFrames + 1);
    }
    this->set_mtx();
    this->demo_move();
    this->ControlEffect();
    // GHS reads the process header before resetting the two per-frame request flags.
    s16 adjust = (s16)mProcess.d, slot = (s16)mProcess.i;
    u32 receiver = a + (s32)adjust;
    mBeforeLift = (s16)(0);
    mLifted = (s16)(0);
    u32 action;
    if (slot < 0)
        action = (u32)mProcess.f;
    else {
        u32 vt = (u32)*gabi::at<be<u32>>(receiver + (s32)(s16)(u32)mProcess.f);
        action = (u32)gabi::at<MagmarockMemberEntry>(vt + (s32)slot * 8)->mTarget;
    }
    gabi::call_ptr(action, ptr(receiver));
    this->play_anim();
    auto *table = &l_sincos.mEntries[(u16)mWobblePhase >> 3];
    f32 amplitude = (f32)mWobbleAmplitude;
    shape_angle.x = (s16)((s16)gabi::ftoi(amplitude * (f32)table->mCos));
    shape_angle.z = (s16)((s16)gabi::ftoi(amplitude * (f32)table->mSin));
    if (!(s16)mRiderFrames) {
        for (int i = 0; i < 4; i++)
            *reinterpret_cast<be<u32> *>(&mRiderQuat[i]) = (u32)l_identityQuat.mWords[i];
    }
    gabi::Local<be<u32>[4]> quat;
    magmaQuatSlerp(mCurrentQuat, mRiderQuat, quat.get(), 0.25f);
    u32 bg = gabi::ea((void *)mpBackground);
    for (int i = 0; i < 4; i++)
        *reinterpret_cast<be<u32> *>(&mCurrentQuat[i]) = (u32)(*quat)[i];
    mRiderFrames = (s16)(0);
    if ((u32)gabi::at<MagmarockBackground>(bg)->mIndex < 256) {
        gabi::at<MagmarockBackground>(bg)->mFlags =
            (u32)((u32)gabi::at<MagmarockBackground>(bg)->mFlags | 4);
        magmaBgMove(ptr(gabi::ea((void *)mpBackground)));
    }
    return 0;
}

/* 0236EECC */
static BOOL daObjMagmarock_Draw(daObjMagmarock_c *self) {
    u32 a = gabi::ea(self);
    u32 env = magmaGetEnvironment();
    magmaSettingTev(ptr(env), 1, &gabi::at<daObjMagmarock_c>(a)->current.pos,
                    &gabi::at<daObjMagmarock_c>(a)->tevStr);
    env = magmaGetEnvironment();
    magmaSettingTev(ptr(env), 0, &gabi::at<daObjMagmarock_c>(a)->current.pos,
                    &gabi::at<daObjMagmarock_c>(a)->mLighting);
    brighten(a);
    env = magmaGetEnvironment();
    magmaSetLightTev(ptr(env), ptr(gabi::ea((void *)gabi::at<daObjMagmarock_c>(a)->mpModel)),
                     &gabi::at<daObjMagmarock_c>(a)->tevStr);
    s16 frame = (s16)gabi::ftoi((f32)gabi::at<daObjMagmarock_c>(a)->mBrkFrame);
    u32 model = gabi::ea((void *)gabi::at<daObjMagmarock_c>(a)->mpModel);
    magmaBrkEntry(gabi::at<daObjMagmarock_c>(a)->mBrkController,
                  ptr((u32)gabi::at<MagmarockModel>(model)->mpModelData), (f32)frame);
    frame = (s16)gabi::ftoi((f32)gabi::at<daObjMagmarock_c>(a)->mBckFrame);
    model = gabi::ea((void *)gabi::at<daObjMagmarock_c>(a)->mpModel);
    magmaBckEntry(gabi::at<daObjMagmarock_c>(a)->mBckController,
                  ptr((u32)gabi::at<MagmarockModel>(model)->mpModelData), (f32)frame);
    magmaModelUpdate(ptr(gabi::ea((void *)gabi::at<daObjMagmarock_c>(a)->mpModel)), 0);
    return 1;
}

/* 0236F25C */
static BOOL daObjMagmarock_IsDelete(daObjMagmarock_c *self) { return 1; }

// HD: compiler-generated and template code of this translation unit.
// GHS constructor and subobject initialization order retain every individual store.

/* 0236CDB8 */
static daObjMagmarock_c *construct(daObjMagmarock_c *self) {
    if (!self) {
        self = magmaAllocate(0x768);
        if (!self)
            return self;
    }
    u32 a = gabi::ea(self);
    magmaActorConstruct(self);
    gabi::at<daObjMagmarock_c>(a)->__vtbl = (u32)(magmarock_tu::actor);
    magmaBrkConstruct(gabi::at<daObjMagmarock_c>(a)->mBrkController);
    magmaFrameInit(gabi::at<daObjMagmarock_c>(a)->mBckController, 0);
    gabi::at<MagmarockSoundObject>(a + 0x49C)->__vtbl = magmarock_tu::soundObjectBase;
    magmaTextureMatrixConstruct(&gabi::at<daObjMagmarock_c>(a)->mBckController[0x14]);
    gabi::at<MagmarockSoundObject>(a + 0x49C)->mState = 0;
    gabi::at<MagmarockSoundObject>(a + 0x49C)->mLink = 0;
    gabi::at<MagmarockSoundObject>(a + 0x49C)->__vtbl = magmarock_tu::soundObject;
    gabi::at<MagmarockSoundObject>(a + 0x49C)->mParameter = 0;
    gabi::at<MagmarockSoundObject>(a + 0x49C)->mFlags = 0;
    gabi::at<MagmarockSoundObject>(a + 0x49C)->mEmbeddedVtable = magmarock_tu::textureMatrix;
    gabi::at<MagmarockSoundObject>(a + 0x49C)->mVoiceHandle = 0;
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mPosition.x =
        (f32)((f32)l_defaultLight.mPosition.x);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mPosition.y =
        (f32)((f32)l_defaultLight.mPosition.y);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mPosition.z =
        (f32)((f32)l_defaultLight.mPosition.z);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mDirection.x =
        (f32)((f32)l_defaultLight.mDirection.x);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mDirection.y =
        (f32)((f32)l_defaultLight.mDirection.y);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mDirection.z =
        (f32)((f32)l_defaultLight.mDirection.z);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mColor.r =
        (u8)((u8)l_defaultLight.mColor.r);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mColor.g =
        (u8)((u8)l_defaultLight.mColor.g);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mColor.b =
        (u8)((u8)l_defaultLight.mColor.b);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mColor.a =
        (u8)((u8)l_defaultLight.mColor.a);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mControls[0] =
        (s16)((s16)l_defaultLight.mControls[0]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mControls[1] =
        (s16)((s16)l_defaultLight.mControls[1]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mControls[2] =
        (s16)((s16)l_defaultLight.mControls[2]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mControls[3] =
        (s16)((s16)l_defaultLight.mControls[3]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mAttenuation[0] =
        (f32)((f32)l_defaultLight.mAttenuation[0]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mAttenuation[1] =
        (f32)((f32)l_defaultLight.mAttenuation[1]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mAttenuation[2] =
        (f32)((f32)l_defaultLight.mAttenuation[2]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mAttenuation[3] =
        (f32)((f32)l_defaultLight.mAttenuation[3]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mAttenuation[4] =
        (f32)((f32)l_defaultLight.mAttenuation[4]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mAttenuation[5] =
        (f32)((f32)l_defaultLight.mAttenuation[5]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mAttenuation[6] =
        (f32)((f32)l_defaultLight.mAttenuation[6]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mAttenuation[1] =
        (f32)((f32)l_defaultLight.mAttenuation[1]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mLightInfo.mAttenuation[7] =
        (f32)((f32)l_defaultLight.mAttenuation[7]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mDirection.z =
        (f32)((f32)l_defaultLight.mDirection.z);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mAttenuation[7] =
        (f32)((f32)l_defaultLight.mAttenuation[7]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mControls[2] =
        (s16)((s16)l_defaultLight.mControls[2]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mDirection.z =
        (f32)((f32)l_defaultLight.mDirection.z);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mColor.b =
        (u8)((u8)l_defaultLight.mColor.b);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mAttenuation[4] =
        (f32)((f32)l_defaultLight.mAttenuation[4]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mDirection.y =
        (f32)((f32)l_defaultLight.mDirection.y);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mControls[3] =
        (s16)((s16)l_defaultLight.mControls[3]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mAttenuation[1] =
        (f32)((f32)l_defaultLight.mAttenuation[1]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mControls[0] =
        (s16)((s16)l_defaultLight.mControls[0]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mAttenuation[0] =
        (f32)((f32)l_defaultLight.mAttenuation[0]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mColor.g =
        (u8)((u8)l_defaultLight.mColor.g);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mPosition.z =
        (f32)((f32)l_defaultLight.mPosition.z);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mAttenuation[2] =
        (f32)((f32)l_defaultLight.mAttenuation[2]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mPosition.x =
        (f32)((f32)l_defaultLight.mPosition.x);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mColor.a =
        (u8)((u8)l_defaultLight.mColor.a);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mPosition.z =
        (f32)((f32)l_defaultLight.mPosition.z);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mDirection.x =
        (f32)((f32)l_defaultLight.mDirection.x);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mControls[2] =
        (s16)((s16)l_defaultLight.mControls[2]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mControls[0] =
        (s16)((s16)l_defaultLight.mControls[0]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mPosition.x =
        (f32)((f32)l_defaultLight.mPosition.x);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mAttenuation[0] =
        (f32)((f32)l_defaultLight.mAttenuation[0]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mAttenuation[5] =
        (f32)((f32)l_defaultLight.mAttenuation[5]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mColor.b =
        (u8)((u8)l_defaultLight.mColor.b);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mAttenuation[2] =
        (f32)((f32)l_defaultLight.mAttenuation[2]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mColor.a =
        (u8)((u8)l_defaultLight.mColor.a);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mControls[1] =
        (s16)((s16)l_defaultLight.mControls[1]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mColor.g =
        (u8)((u8)l_defaultLight.mColor.g);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mPosition.y =
        (f32)((f32)l_defaultLight.mPosition.y);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mPosition.y =
        (f32)((f32)l_defaultLight.mPosition.y);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mAttenuation[6] =
        (f32)((f32)l_defaultLight.mAttenuation[6]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mColor.r =
        (u8)((u8)l_defaultLight.mColor.r);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mAttenuation[3] =
        (f32)((f32)l_defaultLight.mAttenuation[3]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mDirection.y =
        (f32)((f32)l_defaultLight.mDirection.y);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mColor.r =
        (u8)((u8)l_defaultLight.mColor.r);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mControls[1] =
        (s16)((s16)l_defaultLight.mControls[1]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mAttenuation[3] =
        (f32)((f32)l_defaultLight.mAttenuation[3]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mControls[3] =
        (s16)((s16)l_defaultLight.mControls[3]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo.mDirection.x =
        (f32)((f32)l_defaultLight.mDirection.x);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mAttenuation[4] =
        (f32)((f32)l_defaultLight.mAttenuation[4]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mAttenuation[5] =
        (f32)((f32)l_defaultLight.mAttenuation[5]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mAttenuation[6] =
        (f32)((f32)l_defaultLight.mAttenuation[6]);
    gabi::at<daObjMagmarock_c>(a)->mLighting.mHdLightInfo2.mAttenuation[7] =
        (f32)((f32)l_defaultLight.mAttenuation[7]);
    return self;
}

/* 0236F150 */
static void __sinit_d_a_obj_magmarock_cpp() {
    for (int i = 0; i < 4; i++)
        l_headerZeros.mWords[i] = 0;
    magmaRegisterGlobal(&l_headerDtors.mWords[0]);
    l_piLow = -3.1415927410125732f;
    l_piHigh = 3.1415927410125732f;
    magmaAngleConstruct(&l_headerAngle);
    magmaRegisterGlobal(&l_headerDtors.mWords[3]);
    magmaCircleConstruct(&l_headerCircle);
    magmaRegisterGlobal(&l_headerDtors.mWords[6]);
}

/* 0236F1E4 */
static void destroyStatic(u8 *self, u32 flags) {
    if (self && (flags & 1))
        magmaDeallocate(self);
}

/* 0236F1F8 */
static void emptyStatic(u8 *self) {}

/* 0236F1FC */
static void destroyMagmarock(daObjMagmarock_c *self, u32 flags) {
    if (self) {
        magmaSoundObjectDestroy(ptr(gabi::ea(self) + 0x49C), 0);
        magmaActorDestroy(self, 0);
        if (flags & 1)
            magmaDeallocate(self);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's bindings and data
ext 020087EC void magmaBgRelease(void*,void*)
ext 0200ED84 void magmaAddCalc(void*,f32,f32,f32)
ext 0200F164 void magmaAddCalcPos(void*,void*,f32,f32)
ext 0200F428 void magmaAddCalcAngle(void*,s32,u32,u32)
ext 0201ADE0 void magmaVectorDifference(void*,void*,void*)
ext 0201B080 void magmaVectorCross(void*,void*,void*)
ext 0201B47C s32 magmaVectorNormalize(void*)
ext 024EEA6C void magmaBgRegister(void*,void*,void*)
ext 024F2478 u32 magmaBgNew(void*,u32,void*)
ext 024F43DC void magmaBgMove(void*)
ext 025200D4 u32 magmaGetPlay()
ext 02520460 s32 magmaResLoad(void*,const char*)
ext 025204C8 void magmaResDelete(void*,const char*)
ext 02520540 s32 magmaGetReverb(s32)
ext 02542D88 s32 magmaGetStaff(void*,const char*,u32,u32)
ext 02543280 void magmaCutEnd(void*,s32)
ext 0254457C s32 magmaEndCheck(void*,const char*)
ext 02555D0C u32 magmaGetEnvironment()
ext 025626A4 void magmaSettingTev(void*,u32,void*,void*)
ext 02562F5C void magmaSetLightTev(void*,void*,void*)
ext 02588544 void magmaQuatFromTriangle(void*,void*,void*,void*,f32)
ext 0258CAEC f32 magmaCheckY(void*,void*)
ext 025A847C u32 magmaParticleSet(void*,u32,u32,void*,u32,u32,u32,u32,s32,u32,u32,u32)
ext 025B8B68 void magmaEventOn(void*,u32)
ext 025B8B94 s32 magmaEventCheck(void*,u32)
ext 025CB374 void magmaStartShock(void*,u32,u32,void*)
ext 025D4ED0 void magmaActorConstruct(void*)
ext 025D50BC void magmaActorDestroy(void*,u32)
ext 025D57E0 void magmaActorDelete(void*)
ext 025D63E8 s32 magmaEntryHeap(void*,void*,u32)
ext 025D674C void magmaCullBox(void*,f32,f32,f32,f32,f32,f32)
ext 025D77DC void magmaOrderEvent(void*,const char*,u32,u32)
ext 025E1A40 void magmaSoundStart(u32,void*,u32,s32)
ext 025E2DE0 void magmaModelUpdate(void*,u32)
ext 025E38E0 u32 magmaModelCreate(void*,u32,u32)
ext 025E80D0 void magmaBrkConstruct(void*)
ext 025E8154 s32 magmaBrkInit(void*,void*,void*,u32,u32,u32,f32,s32,u32,u32)
ext 025E83FC void magmaBrkEntry(void*,void*,f32)
ext 025E8508 s32 magmaBckInit(void*,void*,void*,u32,u32,u32,f32,s32,u32)
ext 025E86B8 void magmaBckEntry(void*,void*,f32)
ext 025F1B48 void magmaMtxZXYrot(void*,s32,s32,s32)
ext 025F1C28 void magmaMtxYrot(void*,s32)
ext 025F2518 void magmaMtxScale(f32,f32,f32)
ext 025F25CC void magmaMtxQuat(void*)
ext 026066C4 u32 magmaGetResource(void*,void*,s32)
ext 0273AA24 void magmaAssertion(const char*,u32,const char*)
ext 0273AD10 daObjMagmarock_c* magmaAllocate(u32)
ext 0273AF40 void magmaDeallocate(void*)
ext 027DA984 void magmaTextureMatrixConstruct(void*)
ext 027F2BC0 void magmaFrameInit(void*,u32)
ext 027F3628 void magmaSoundObjectDestroy(void*,u32)
ext 028E8B48 void magmaQuatMultiply(void*,void*,void*)
ext 028E8D88 void magmaVectorAdd(void*,void*,void*)
ext 028E8DD0 f32 magmaVectorSquareMag(void*)
ext 028E8E64 void magmaVectorScale(void*,void*,f32)
ext 028E90D4 void magmaMtxCopy(void*,void*)
ext 028E93CC void magmaMtxTranslation(void*,f32,f32,f32)
ext 028E9BC0 void magmaQuatSlerp(void*,void*,void*,f32)
ext 028EAB2C void magmaCircleConstruct(void*)
ext 028ED6F8 void magmaAngleConstruct(void*)
ext 028F026C void magmaRegisterGlobal(void*)
ext 028F4384 f32 magmaSqrt(f32)
data l_defaultLight 1016E414
data l_resourceControl 101F4F28
data l_save 101F84DC
data l_calcMtx 1048D0CC
data l_identityQuat 101E9C38
data l_sincos 104A44F8
data l_quakeHeightBias 1047BBD8
data l_dampingBias 1047BC14
data l_buoyancyBias 1047BC18
data l_effectColor 101CAA6C
data l_headerZeros 1046A4F8
data l_piLow 1046A4EC
data l_piHigh 1046A4F0
data l_headerAngle 1046A4F4
data l_headerCircle 1046A4F5
data l_headerDtors 101CAA48
str 1002C230 "d_a_obj_magmarock.cpp"
str 1002C248 "modelData != (0)"
str 1002C25C "M_brk != (0)"
str 1002C26C "M_bck != (0)"
str 1002C2D8 "Magrock"
str 1002C2E0 "magma_cam"
str 1002C328 "Kyjim"
*/
