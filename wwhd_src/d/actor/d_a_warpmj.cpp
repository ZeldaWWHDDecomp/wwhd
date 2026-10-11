/** Forsaken Fortress warp portal: sea alignment, materials, and event actions.
 * HD: preserve verified animation construction, draw-tag callbacks, and guest call ordering.
 */
#include "d/actor/d_a_warpmj.h"
static void* sResourceManager;
static WarpmjLightBlock sLightDefault;
static WarpmjSave* sSave;
static void* sDemo;
static WarpmjHeader sHeader;
static WarpmjWords sHeaderZero;
static WarpmjDestructorRecords sDestructorRecords;
static f32 sRate;
static f32 sZero;
static f32 sCullMin;
static f32 sCullMax;
static f32 sCullTop;
static f32 sSeaProbeHeight;
static f32 sNoSeaHeight;
static f32 sColorScale;
static f32 sWarpRadius;
static f32 sMinAngle;
static f32 sMaxAngle;
static WarpmjActionTable sInitActions;
static WarpmjActionTable sExecuteActions;
static WarpmjActionNames sActionNames;

static void* resource(s32 index) {
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::guest_addr("Gmjwp");
    name->__vtbl = 0x100428F4;
    return warpmjGetResource(sResourceManager.get(), name.get(), index);
}
static void resourceAssert(s32 line, const char* message) {
    warpmjAssert("d_a_warpmj.cpp", line, message);
}
// HD: snapshot all twelve matrix floats before writing the model.
static void copyModelMatrix(J3DModel* model) {
    f32 values[12];
    for (unsigned i = 0; i < 12; ++i)
        values[i] = mDoMtx_stack_c::get()->m[i / 4][i % 4];
    for (unsigned i = 0; i < 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
}
static s32 invokeAction(const WarpmjAction* descriptor, daWarpmj_c* i_this) {
    s32 staff = i_this->mStaff;
    return (i_this->*(*descriptor))(staff);
}

// HD: allocating actor constructor.
/* 024DD128 */
static daWarpmj_c* warpmj_actorCtor(daWarpmj_c* i_this) {

    if (!i_this) {
        i_this = warpmjAllocateActor(0x5AC);
        if (!i_this)
            return i_this;
    }
    warpmjConstructActor(i_this);

    i_this->__vtbl = 0x10042934;
    // HD: the extra tev structure contains three light blocks; keep mixed-width copy order.
    WarpmjLightBlock* blocks[] = {
        reinterpret_cast<WarpmjLightBlock*>(i_this->mWarpTev.mLightObj),
        reinterpret_cast<WarpmjLightBlock*>(i_this->mWarpTev.mHdLightParam),
        reinterpret_cast<WarpmjLightBlock*>(i_this->mWarpTev.mHdLightParam + 0x84)};
    for (auto* block : blocks) {
        for (u32 i = 0; i < 6; ++i)
            block->mVectors[i] = (f32)sLightDefault.mVectors[i];
        for (u32 i = 0; i < 4; ++i)
            block->mColor[i] = sLightDefault.mColor[i];
        for (u32 i = 0; i < 4; ++i)
            block->mCoefficients[i] = (s16)sLightDefault.mCoefficients[i];
        for (u32 i = 0; i < 8; ++i)
            block->mAttenuation[i] = (f32)sLightDefault.mAttenuation[i];
    }
    return i_this;
}
/* 024DD738 */
BOOL daWarpmj_c::_delete() {

    warpmjDeleteResource(&mPhase, "Gmjwp");
    return 1;
}
/* 024DD124 */
static BOOL CheckCreateHeap(daWarpmj_c* i_this) { return i_this->CreateHeap(); }
/* 024DCE1C */
BOOL daWarpmj_c::CreateHeap() {

    void* modelData = resource(GMJWP_BDL_PORTAL);
    if (!modelData)
        resourceAssert(0xAC, "modelData != (0)");
    mpModel = warpmjCreateModel(modelData, 0x80000, 0x11000222);
    if (!mpModel)
        return 0;
    void* animation = resource(GMJWP_BCK_PORTAL);
    if (!animation)
        resourceAssert(0xBB, "pbck != (0)");
    WarpmjBckConstruction* bck = warpmjAllocateBck(0x8C);
    if (bck) {
        warpmjInitFrame(bck, 0);
        bck->mAnimationVtable = 0x1016E54C;
        warpmjConstructTransform(&bck->mTransform);
        bck->mTransformVtable = 0x1016D820;
        bck->mAnimationVtable = 0x1004290C;
        bck->mpTransform = nullptr;
        bck->mAnimationWords[0] = 0;
        bck->mAnimationWords[1] = 0;
        bck->mAnimationWords[2] = 0;
        bck->mAnimationWords[3] = 0;
    }
    mpBck = reinterpret_cast<mDoExt_bckAnm*>(bck);
    if (!bck)
        return 0;
    f32 rate = sRate;
    if (!warpmjInitBck(bck, modelData, animation, 1, 0, rate, 0, -1, 0))
        return 0;
    mDoExt_bckAnm* bckAnm = mpBck;
    bckAnm->mFrameCtrl.mRate = (f32)(s16)bckAnm->mFrameCtrl.mEnd;
    animation = resource(GMJWP_BTK_PORTAL);
    if (!animation)
        resourceAssert(0xCE, "pbtk != (0)");
    mDoExt_btkAnm* btk = warpmjCreateBtk(nullptr);
    mpBtk = btk;
    if (!btk)
        return 0;
    if (!warpmjInitBtk(btk, modelData, animation, 1, 2, rate, 0, -1, 0, 0))
        return 0;
    mpBtk->mFrameCtrl.mRate = sZero;
    animation = resource(GMJWP_BRK_PORTAL);
    if (!animation)
        resourceAssert(0xDE, "pbrk != (0)");
    mDoExt_brkAnm* brk = warpmjCreateBrk(nullptr);
    mpBrk = brk;
    if (!brk)
        return 0;
    if (!warpmjInitBrk(brk, modelData, animation, 1, 0, rate, 0, -1, 0, 0))
        return 0;
    brk = mpBrk;
    brk->mFrameCtrl.mRate = (f32)(s16)brk->mFrameCtrl.mEnd;
    return 1;
}
/* 024DD3B0 */
void daWarpmj_c::CreateInit() {

    J3DModel* model = mpModel;
    cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    f32 zero = sZero;
    f32 low = sCullMin, high = sCullMax, top = sCullTop;
    warpmjSetCullBox(this, low, zero, low, high, top, high);
    f32 x = current.pos.x, rate = sRate, y = current.pos.y;
    cullSizeFar = rate;
    gabi::Local<cXyz> position;
    position->x = x;
    position->y = y + sSeaProbeHeight;
    position->z = current.pos.z;
    f32 sea = getSeaY(position);
    model = mpModel;
    f32 sz = scale.z, sy = scale.y, sx = scale.x;
    if (sea == sNoSeaHeight)
        sea = zero;
    current.pos.y = sea;
    model->mBaseScale.y = sy;
    model->mBaseScale.x = sx;
    model->mBaseScale.z = sz;
    y = current.pos.y;
    x = current.pos.x;
    f32 z = current.pos.z;
    warpmjTranslate(mDoMtx_stack_c::get(), x, y + rate, z);
    copyModelMatrix(mpModel.get());
    for (unsigned i = 0; i < 3; ++i) {
        void* particleSystem = warpmjGetGameInfo()->mpParticles.get();
        u32 kind = i == 2 ? 4 : 0, id = i == 2 ? 0xC3FC : 0x83FD + i;
        mpParticles[i] = warpmjSpawnParticle(particleSystem, kind, id, &current.pos, 0,
                                                   0, 0xFF, 0, -1, 0, 0, 0);
    }
    mOrder = ORDER_IDLE;
    setEndAnm();
    void* events = &warpmjGetGameInfo()->mEventManager;
    s32 event = warpmjGetEventIndex(events, "TO_GANON_WARP", 0xFF);
    s32 room = current.roomNo;
    u32 destination = (u8)(u32)mParameters;
    mEventIndex = (s16)event;
    mDestination = destination;
    warpmjInitTev(&mWarpTev, room, 0xFF);
}
/* 024DD660 */
s32 daWarpmj_c::_create() {

    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (gabi::ea(this)) {
            warpmj_actorCtor(this);
            flags = actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    WarpmjSave* save = sSave;
    if (!warpmjCheckEventBit(&save->mEventBits, 0x3D02))
        return cPhs_ERROR_e;
    s32 phase = warpmjLoadResource(&mPhase, "Gmjwp");
    if (phase == cPhs_COMPLEATE_e) {
        if (!warpmjEntryHeap(this, gabi::guest_fn((const void*)CheckCreateHeap), 0x3000))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}
/* 024DDF70 */
BOOL daWarpmj_c::_execute() {

    if (!demoActorID) {
        checkOrder();
        demo_proc();
        eventOrder();
    } else
        demo_execute();
    s32 reverb = warpmjGetReverb((s32)current.roomNo);
    warpmjStartLevelSound(0x1089, &eyePos, 0, reverb);
    gabi::Local<cXyz> position;
    position->x = current.pos.x;
    position->y = current.pos.y + sSeaProbeHeight;
    position->z = current.pos.z;
    f32 sea = getSeaY(position);
    J3DModel* model = mpModel;
    f32 sz = scale.z, sx = scale.x, sy = scale.y;
    if (sea == sNoSeaHeight)
        sea = sZero;
    current.pos.y = sea;
    model->mBaseScale.x = sx;
    model->mBaseScale.z = sz;
    model->mBaseScale.y = sy;
    f32 y = current.pos.y, x = current.pos.x, z = current.pos.z;
    warpmjTranslate(mDoMtx_stack_c::get(), x, y + sRate, z);
    copyModelMatrix(mpModel.get());
    return 1;
}
/* 024DDB54 */
void daWarpmj_c::normal_execute() {

    animPlay();
    if (check_warp())
        mOrder = ORDER_REQUESTED;
}
/* 024DDE90 */
void daWarpmj_c::demo_execute() {

    u32 id = demoActorID;
    if (id == 0 || id > 32)
        return;
    void* demo = sDemo;
    if (!demo) {
        warpmjAssert("d_demo.h", 0x23A, "m_object != (0)");
        demo = sDemo;
    }
    WarpmjDemoActor* actor = warpmjGetDemoActor(demo, id);
    if (!actor)
        return;
    s32 state = actor->mState;
    mDemoState = state;
    if (state == 0)
        warpmjRemoveDrawTag(&draw_tag);
    else if (state == 1) {
        void* queue = warpmjGetDrawQueue(this);
        warpmjQueueDrawTag(&draw_tag, queue);
        animPlay();
    }
}
/* 024DDC6C */
void daWarpmj_c::demo_proc() {

    void* events = &warpmjGetGameInfo()->mEventManager;
    mStaff = warpmjGetStaff(events, "Warpmj", 0, 0);
    if (!warpmjGetGameInfo()->mEventRunning || eventInfo.mCommand == dEvtCmd_INTALK_e)
        return;
    s32 staff = mStaff;
    if (staff == -1)
        return;
    events = &warpmjGetGameInfo()->mEventManager;
    s32 action = warpmjGetAction(events, staff, sActionNames.mNames, 3, 0, 0);
    staff = mStaff;
    if (action == -1) {
        events = &warpmjGetGameInfo()->mEventManager;
        warpmjEndCut(events, staff);
        return;
    }
    events = &warpmjGetGameInfo()->mEventManager;
    if (warpmjIsAdvance(events, staff))
        invokeAction(&sInitActions.mActions[(u32)action], this);
    if (invokeAction(&sExecuteActions.mActions[(u32)action], this)) {
        staff = mStaff;
        events = &warpmjGetGameInfo()->mEventManager;
        warpmjEndCut(events, staff);
    }
}
/* 024DE2D8 */
void daWarpmj_c::initWait(s32 staff) {}
/* 024DE154 */
BOOL daWarpmj_c::actWait(s32 staff) {
    animPlay();
    return 1;
}
/* 024DE178 */
void daWarpmj_c::initWarp(s32 staff) {

    void* events = &warpmjGetGameInfo()->mEventManager;
    warpmjSetGoal(events, &current.pos);
    warpmjStartSound(0x2891);
}
/* 024DE1B8 */
BOOL daWarpmj_c::actWarp(s32 staff) {
    animPlay();
    return 1;
}
/* 024DE1DC */
void daWarpmj_c::initWarpArrive(s32 staff) {
    setEndAnm();
    warpmjStartSound(0x2894);
}
/* 024DE204 */
BOOL daWarpmj_c::actWarpArrive(s32 staff) {
    animPlay();
    return 1;
}
/* 024DDE30 */
void daWarpmj_c::eventOrder() {

    if (mOrder == ORDER_REQUESTED) {
        warpmjOrderEvent(this, (s32)mEventIndex, 0xFF, 0xFFFF, 0, 1);
        eventInfo.mCondition = eventInfo.mCondition | dEvtCnd_UNK2_e;
    }
}
/* 024DDB98 */
void daWarpmj_c::checkOrder() {

    if (eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        s32 event = mEventIndex;
        void* events = &warpmjGetGameInfo()->mEventManager;
        s32 started = warpmjCheckStart(events, event);
        event = mEventIndex;
        if (started && mOrder != ORDER_IDLE)
            mOrder = ORDER_IDLE;
        events = &warpmjGetGameInfo()->mEventManager;
        if (warpmjCheckEnd(events, event)) {
            s32 room = current.roomNo;
            u32 destination = (u8)(u32)mDestination;
            warpmjChangeScene(destination, room);
        }
    } else if (mOrder == ORDER_IDLE && !warpmjGetGameInfo()->mEventRunning)
        normal_execute();
}
/* 024DDA78 */
u32 daWarpmj_c::animPlay() {

    mDoExt_baseAnm* animation = mpBtk.get();
    if (animation) {
        animation->mFrameCtrl.mRate = sRate;
        return warpmjPlayAnimation(mpBtk.get());
    }
    return (u32)gabi::ea(this); // HD: the no-controller path retains the incoming actor in r3.
}
/* 024DD334 */
void daWarpmj_c::setEndAnm() {

    mDoExt_bckAnm* bck = mpBck;
    if (bck)
        bck->mFrameCtrl.mFrame = (f32)(s16)bck->mFrameCtrl.mEnd;
    mDoExt_brkAnm* brk = mpBrk;
    if (brk)
        brk->mFrameCtrl.mFrame = (f32)(s16)brk->mFrameCtrl.mEnd;
}
/* 024DD2E0 */
f32 daWarpmj_c::getSeaY(cXyz* position) {

    f32 z = position->z, x = position->x;
    if (warpmjCheckSea(x, z)) {
        x = position->x;
        z = position->z;
        return warpmjGetSeaHeight(x, z);
    }
    return warpmjGetWaterHeight(position);
}
/* 024DDA98 */
BOOL daWarpmj_c::check_warp() {

    warpmjGetGameInfo();
    fopAc_ac_c* player = warpmjGetGameInfo()->mpPlayer.get();
    if (!(warpmjGetGameInfo()->mPlayerStatus & WARP_PLAYER_READY) || !player)
        return 0;
    gabi::Local<cXyz> delta, horizontal;
    warpmjSubtract(&player->current.pos, delta.get(), &current.pos);
    horizontal->x = delta->x;
    horizontal->y = sZero;
    horizontal->z = delta->z;
    f32 magnitude = warpmjSquaredMagnitude(horizontal.get());
    f32 distance = warpmjSqrt(magnitude);
    return distance < sWarpRadius;
}
/* 024DD76C */
static void colorToFloat(be<f32>* output, be<u8>* color) {

    f32 rgba[4];
    for (unsigned i = 0; i < 4; ++i)
        rgba[i] = (f32)(u8)color[i] / 255.0f;
    for (unsigned i = 0; i < 4; ++i)
        output[i] = rgba[i];
}
/* 024DD820 */
BOOL daWarpmj_c::_draw() {

    void* env = warpmjGetEnvironment();
    warpmjSetLight(env, 0, &current.pos, &tevStr);
    env = warpmjGetEnvironment();
    J3DModel* model = mpModel;
    warpmjApplyLight(env, model, &tevStr);
    env = warpmjGetEnvironment();
    warpmjSetLight(env, 2, &current.pos, &mWarpTev);
    model = mpModel;
    J3DModelData* data = model->mModelData;
    WarpmjMaterialBlock* materialBlock = warpmjGetMaterialBlock(data);
    u16 index = 0;
    if (index < (u16)materialBlock->mCount) {
        f32 rate = sRate;
        do {
            u32 count = data->mMaterialNum;
            // HD: the shared model-data layout stores this array as a guest address.
            WarpmjMaterial* material = gabi::at<WarpmjMaterial>(data->mMaterials);
            if (index < count)
                material += index;
            for (unsigned channel = 0; channel < 3; ++channel) {
                be<u8>* color = material->mpColorBlock->getColor(1);
                color[channel] = reinterpret_cast<be<u8>*>(&mWarpTev.mColorK0)[channel];
            }
            be<u8>* color = material->mpColorBlock->getColor(1);
            WarpmjColorBlock* object = material->mpColorBlock;
            object->setColor(1, color);
            gabi::Local<be<f32>[4]> floats;
            colorToFloat(*floats, color);
            gabi::Local<be<f32>[4]> corrected;
            warpmjCorrectColor(corrected.get(), floats.get(), rate);
            u32 flags = material->mUniformFlags;
            material->mUniformFlags = flags | WARP_UNIFORM_DIRTY;
            be<f32>* uniform = warpmjGetUniform(&material->mUniformFlags, 8);
            f32 alpha = (f32)(u8)color[3] / sColorScale;
            f32 red = (*corrected)[0], green = (*corrected)[1], blue = (*corrected)[2];
            uniform[1] = green;
            uniform[2] = blue;
            uniform[0] = red;
            uniform[3] = alpha;
            ++index;
            materialBlock = warpmjGetMaterialBlock(data);
        } while (index < (u16)materialBlock->mCount);
    }
    mDoExt_baseAnm* animation = mpBtk.get();
    if (animation) {
        model = mpModel;
        f32 frame = (f32)animation->mFrameCtrl.mFrame;
        warpmjEntryBtk(animation, model->mModelData.get(), frame);
    }
    animation = mpBrk.get();
    if (animation) {
        model = mpModel;
        f32 frame = (f32)animation->mFrameCtrl.mFrame;
        warpmjEntryBrk(animation, model->mModelData.get(), frame);
    }
    animation = mpBck.get();
    if (animation) {
        model = mpModel;
        f32 frame = (f32)animation->mFrameCtrl.mFrame;
        warpmjEntryBck(animation, model->mModelData.get(), frame);
    }
    warpmjUpdateModel(mpModel.get(), 0);
    return 1;
}
/* 024DD734 */
static s32 daWarpmj_Create(daWarpmj_c* i_this) { return i_this->_create(); }
/* 024DD768 */
static BOOL daWarpmj_Delete(daWarpmj_c* i_this) { return i_this->_delete(); }
/* 024DDA74 */
static BOOL daWarpmj_Draw(daWarpmj_c* i_this) { return i_this->_draw(); }
/* 024DE150 */
static BOOL daWarpmj_Execute(daWarpmj_c* i_this) { return i_this->_execute(); }
/* 024DE2D0 */
static BOOL daWarpmj_IsDelete(daWarpmj_c* i_this) { return 1; }
// HD: compiler-generated initialization and deleting destructors.
/* 024DE228 */
static void staticInit() {

    sHeaderZero.mWords[2] = 0;
    sHeaderZero.mWords[0] = 0;
    sHeaderZero.mWords[3] = 0;
    sHeaderZero.mWords[1] = 0;
    warpmjRegisterGlobal(&sDestructorRecords.mRecords[0]);
    f32 low = sMinAngle, high = sMaxAngle;
    sHeader.mMinAngle = low;
    sHeader.mMaxAngle = high;
    warpmjInitHeaderByte(&sHeader.mByte);
    warpmjRegisterGlobal(&sDestructorRecords.mRecords[1]);
    warpmjInitHeaderFlag(&sHeader.mFlag);
    warpmjRegisterGlobal(&sDestructorRecords.mRecords[2]);
}
/* 024DE2BC */
static void staticDestructor(void* object, s32 flags) {

    if (object && (flags & 1))
        warpmjDeleteObject(object);
}
/* 024DE2DC */
static void warpmj_actorDtor(daWarpmj_c* i_this, s32 flags) {

    if (i_this) {
        warpmjDestroyActor(i_this, 0);
        if (flags & 1)
            warpmjDeleteObject(i_this);
    }
}

// clang-format off
/* WWHD glue
data sResourceManager 101F4F28
data sLightDefault 1016E414
data sSave 101F84DC
data sDemo 101D5FFC
data sHeader 1046EBAC
data sHeaderZero 1046EBB8
data sDestructorRecords 101D32B8
data sRate 10042960
data sZero 10042970
data sCullMin 100429BC
data sCullMax 100429C0
data sCullTop 100429C4
data sSeaProbeHeight 100429C8
data sNoSeaHeight 100429CC
data sColorScale 100429E0
data sWarpRadius 100429F0
data sMinAngle 10042A18
data sMaxAngle 10042A1C
data sInitActions 101D325C
data sExecuteActions 101D3274
data sActionNames 101D32AC
ext 026066C4 void* warpmjGetResource(void*, SafeString*, s32)
ext 0273AA24 void warpmjAssert(const char*, s32, const char*)
ext 025E38E0 J3DModel* warpmjCreateModel(void*, u32, u32)
ext 0273AD10 WarpmjBckConstruction* warpmjAllocateBck(u32)
ext 027F2BC0 void warpmjInitFrame(void*, s32)
ext 027DA984 void warpmjConstructTransform(void*)
ext 025E8508 s32 warpmjInitBck(void*, void*, void*, s32, s32, f32, s32, s32, s32)
ext 025E7C6C mDoExt_btkAnm* warpmjCreateBtk(void*)
ext 025E7CE0 s32 warpmjInitBtk(void*, void*, void*, s32, s32, f32, s32, s32, s32, s32)
ext 025E80D0 mDoExt_brkAnm* warpmjCreateBrk(void*)
ext 025E8154 s32 warpmjInitBrk(void*, void*, void*, s32, s32, f32, s32, s32, s32, s32)
ext 025D4ED0 void warpmjConstructActor(daWarpmj_c*)
ext 0246B6A4 s32 warpmjCheckSea(f32, f32)
ext 0246BA0C f32 warpmjGetSeaHeight(f32, f32)
ext 024F1478 f32 warpmjGetWaterHeight(cXyz*)
ext 025D674C void warpmjSetCullBox(daWarpmj_c*, f32, f32, f32, f32, f32, f32)
ext 028E93CC void warpmjTranslate(Mtx34*, f32, f32, f32)
ext 025A847C void* warpmjSpawnParticle(void*, u32, u32, cXyz*, u32, u32, u32, u32, s32, u32, u32, u32)
ext 02543F10 s32 warpmjGetEventIndex(void*, const char*, s32)
ext 0255FFF4 void warpmjInitTev(dKy_tevstr_c*, s32, s32)
ext 025B8B94 s32 warpmjCheckEventBit(void*, u32)
ext 02520460 s32 warpmjLoadResource(request_of_phase_process_class*, const char*)
ext 025D63E8 s32 warpmjEntryHeap(daWarpmj_c*, u32, u32)
ext 025204C8 void warpmjDeleteResource(request_of_phase_process_class*, const char*)
ext 025626A4 void warpmjSetLight(void*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void warpmjApplyLight(void*, J3DModel*, dKy_tevstr_c*)
ext 027F3F8C WarpmjMaterialBlock* warpmjGetMaterialBlock(J3DModelData*)
ext 0274D458 void warpmjCorrectColor(void*, void*, f32)
ext 027F9F0C be<f32>* warpmjGetUniform(be<u32>*, s32)
ext 025E7FC4 void warpmjEntryBtk(mDoExt_baseAnm*, J3DModelData*, f32)
ext 025E83FC void warpmjEntryBrk(mDoExt_baseAnm*, J3DModelData*, f32)
ext 025E86B8 void warpmjEntryBck(mDoExt_baseAnm*, J3DModelData*, f32)
ext 025E2DE0 void warpmjUpdateModel(J3DModel*, u32)
ext 025E742C u32 warpmjPlayAnimation(mDoExt_btkAnm*)
ext 0201ADE0 void warpmjSubtract(cXyz*, cXyz*, cXyz*)
ext 028E8DD0 f32 warpmjSquaredMagnitude(cXyz*)
ext 028F4384 f32 warpmjSqrt(f32)
ext 0254407C s32 warpmjCheckStart(void*, s32)
ext 025440C8 s32 warpmjCheckEnd(void*, s32)
ext 02587EFC void warpmjChangeScene(u32, s32)
ext 02542D88 s32 warpmjGetStaff(void*, const char*, u32, u32)
ext 02542EDC s32 warpmjGetAction(void*, s32, void*, s32, s32, s32)
ext 02543280 void warpmjEndCut(void*, s32)
ext 025447C8 s32 warpmjIsAdvance(void*, s32)
ext 025D7A58 void warpmjOrderEvent(daWarpmj_c*, s32, u32, u32, u32, u32)
ext 02526E70 WarpmjDemoActor* warpmjGetDemoActor(void*, u32)
ext 025DA884 void warpmjRemoveDrawTag(void*)
ext 025DF2B8 void* warpmjGetDrawQueue(daWarpmj_c*)
ext 025DA874 void warpmjQueueDrawTag(void*, void*)
ext 02520540 s32 warpmjGetReverb(s32)
ext 025E1A40 void warpmjStartLevelSound(u32, cXyz*, u32, s32)
ext 02543714 void warpmjSetGoal(void*, cXyz*)
ext 025E1988 void warpmjStartSound(u32)
ext 028F026C void warpmjRegisterGlobal(void*)
ext 028ED6F8 void warpmjInitHeaderByte(void*)
ext 028EAB2C void warpmjInitHeaderFlag(void*)
ext 0273AF40 void warpmjDeleteObject(void*)
ext 025D50BC void warpmjDestroyActor(daWarpmj_c*, s32)
ext 025200D4 WarpmjGameInfo* warpmjGetGameInfo()
ext 02555D0C void* warpmjGetEnvironment()
ext 0273AD10 daWarpmj_c* warpmjAllocateActor(u32)
str 10042944
str 10042954
str 10042974
str 10042980
str 1004298C
str 10042998
str 100429A8
str 100429D0
str 100429F4
str 10042A20
*/
