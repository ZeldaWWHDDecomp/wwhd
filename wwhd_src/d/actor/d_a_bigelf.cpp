// Great Fairy actor, ported with the verified HD behaviour.
#include "d/actor/d_a_bigelf.h"

const gc_tu_data gc_tu = {.safeString = 0x1000912C};
static constexpr u32 l_actorVtable = 0x10009144;
static Mtx34 l_mtxStack;
static Mtx34 l_jointMatrixCopy;
// HD: these accesses copy floating-point storage through integer registers, preserving SNaNs.
static be<u32>& bigelf_word(be<f32>& value) { return *gabi::at<be<u32>>(gabi::ea(&value)); }
template <typename T> static T* bigelf_view(u32 address) { return gabi::at<T>(address); }
static f32 l_zero = 0.0f;
static f32 l_handForward = 20.0f;
static f32 l_handEye = -20.0f;
static f32 l_one = 1.0f;
static f32 l_greenColor = 2.0f;
static f32 l_blueColor = 3.0f;
static f32 l_fairyHeight = 100.0f;
static f32 l_spawnHeight = 30.0f;
static f32 l_attentionHeight = 50.0f;
static f32 l_lightRate = 0.25f;
static f32 l_lightStep = 80.0f;
static f32 l_lightColor = 255.0f;
static f32 l_animationRate = 0.1f;
static f32 l_blendMinimum = 0.05f;
static f32 l_blendStep = 0.01f;
static f32 l_morphFrames = 8.0f;
static f32 l_animationEnd = -1.0f;
static f32 l_huntDistance = 900.0f;
static f32 l_shipStartDistance = 800.0f;
static f32 l_appearHeight = 70.0f;
static f32 l_exitHeight = 250.0f;
static f32 l_fogEnd = 2000.0f;
static f32 l_linkLightRadius = 300.0f;
static f32 l_linkForward = 400.0f;
static f32 l_fairyMoveRate = 0.2f;
static f32 l_appearSoundFrame = 87.0f;
static f32 l_exitFogTarget = 10.0f;
static f32 l_disappearStretch = 5.0f;
static f32 l_flowerBeginFrame = 116.0f;
static f32 l_flowerEndFrame = 173.0f;
static f32 l_flowerCreateFrame = 167.0f;
static f32 l_dimBeginFrame = 16.0f;
static f32 l_lightForward = 60.0f;
static f32 l_dimSoundFrame = 126.0f;
static f32 l_chimeSoundFrame = 154.0f;
static f32 l_dimEndFrame = 166.0f;
static f32 l_actorLightScale = 0.7f;
static f32 l_actorLightBase = 0.3f;
static f32 l_backgroundLightScale = 0.6f;
static f32 l_backgroundLightBase = 0.4f;
static f32 l_octOffsetX = -820.0f;
static f32 l_octOffsetZ = 1340.0f;
static f32 l_shipExitDistance = 1570.0f;
static f32 l_minimumAngle = -3.1415927410125732f;
static f32 l_maximumAngle = 3.1415927410125732f;
// HD: numeric initializers let autoglue check the mapped archive index table.
static s32 l_bckIndex[4] = {8, 6, 7, 6};
static u16 l_eventFlags[7] = {12320, 12304, 12296, 12292, 12290, 12289, 12672};
static u16 l_changeEffects[4] = {33615, 33617, 33619, 33621};
static u16 l_releaseEffects[4] = {33616, 33618, 33620, 33622};
static u16 l_appearEffects[4] = {33607, 33609, 33611, 33613};
static u16 l_flashEffects[4] = {33608, 33610, 33612, 33614};
static BigelfGlobalWord l_saveInfo;
static BigelfGlobalWord l_resourceManager;
static BigelfGlobalWord l_messageManager;
static BigelfGlobalWord l_messageHandle;
static BigelfGlobalWord l_currentModel;
static BigelfGlobalWord l_currentMatrix;

static BigelfHeaderStatics l_headerStatics;
static BigelfDestructorRecord l_primaryDestructor;
static BigelfDestructorRecord l_colorDestructor;
static BigelfDestructorRecord l_vectorDestructor;

static char* l_actionNames[] = {"WAIT",     "FA1",      "APPEAR",  "TALK",
                                "EXIT",     "FL_DEMO",  "FL_LINK", "FL_DELETE",
                                "FL_DM_BF", "FL_DM_MD", "FL_DM_AF"};

// HD: GHS stores the method address explicitly; retain the eight-byte virtual descriptor path.
static constexpr u32 l_waitActionTarget = 0x02097C98;

/* inlined into daBigelf_c::_execute */
static dComIfG_inf_c* gameInfo() { return bigelf_view<dComIfG_inf_c>(bigelf_getGameInfo()); }
/* inlined into daBigelf_c::talk */
// HD: the save-data pointer names its 32-byte header; dSv_info_c starts immediately after it.
static dSv_info_c* save() { return bigelf_view<dSv_info_c>((u32)l_saveInfo.value + 0x20); }
/* inlined into daBigelf_c::CreateHeap */
static void* bytes(u32 address) { return gabi::at<u8>(address); }

/* inlined into daBigelf_c::hunt */
static u32 findActor(u32 id) { return gabi::ea(fopAcM_SearchByID(id)); }

/* inlined into daBigelf_c::demoInitAppear */
static u32 particle(u16 effect, cXyz* position, csXyz* angle = nullptr, cXyz* scale = nullptr) {
    u32 manager = gabi::ea(gameInfo()->play.mParticle.get());
    return bigelf_setParticle(bytes(manager), 0, effect, position, angle, scale, 255, 0, -1, 0, 0,
                              0);
}
/* inlined into daBigelf_c::demoInitAppear */
static void sound(daBigelf_c* actor, u32 id) {
    s32 reverb = bigelf_getReverb((s32)(s8)actor->current.roomNo);
    bigelf_startSound(id, &actor->eyePos, 0, reverb);
}
/* inlined into daBigelf_c::demoProc */
static void cutEnd(daBigelf_c* actor) {
    s32 staff = actor->mStaffId;
    bigelf_endCut(&gameInfo()->play.mEvtManager, staff);
}
/* inlined into daBigelf_c::demoInitAppear */
static u16 colorEffect(daBigelf_c* actor, const be<u16>* table) {
    return table[(u32)(s32)(s8)actor->mColorType];
}
/* inlined into daBigelf_c::demoInitExit */
static void retireEmitter(u32 emitter) {
    u32 flags = bigelf_view<BigelfEmitterView>(emitter)->mFlags;
    bigelf_view<BigelfEmitterView>(emitter)->mMaxFrame = -1;
    bigelf_view<BigelfEmitterView>(emitter)->mFlags = flags | 1;
}

/* inlined into daBigelf_c::_execute */
static void copyMatrix(u32 source, u32 destination) {
    J3DModel::mtx_assign(bigelf_view<Mtx34>(destination), bigelf_view<Mtx34>(source));
}

/* inlined into daBigelf_c::nodeCallBack */
static void commitJointMatrix(u32 model, u16 joint) {
    bigelf_copyMatrix(bytes(l_currentMatrix.value), &l_jointMatrixCopy);
    u32 buffer = gabi::ea(bigelf_view<J3DModel>(model)->mpMtxBuffer.get());
    u16 flags = bigelf_view<J3DMtxBuffer_hd>(buffer)->mFlags;
    u32 current = l_currentMatrix.value;
    bigelf_view<J3DMtxBuffer_hd>(buffer)->mFlags = flags | 0x10;
    u32 matrices = bigelf_view<J3DMtxBuffer_hd>(buffer)->mpAnmMtx;
    copyMatrix(current, gabi::ea(&bigelf_view<Mtx34>(matrices)[joint]));
}

/* inlined into daBigelf_c::CreateHeap */
static u32 resource(gabi::Local<SafeString>& name, s32 id) {
    name->mStringTop = gabi::guest_str("bigelf");
    name->__vtbl = gc_tu.safeString;
    return bigelf_getResource(bytes(l_resourceManager.value), name.get(), id);
}
/* inlined into daBigelf_c::CreateHeap */
static void assertion(u32 line, const char* condition) {
    bigelf_assertion("d_a_bigelf.cpp", line, condition);
}
/* inlined into daBigelf_c::CreateHeap */
static s8 namedJoint(u32 data, const char* name) {
    u32 table = bigelf_getJointNames(bytes(data));
    u32 offset = bigelf_view<BigelfNameTable>(table)->mNamesOffset;
    u32 names = offset ? gabi::ea(&bigelf_view<BigelfNameTable>(table)->mNamesOffset) + offset : 0;
    return (s8)bigelf_getJointIndex(bytes(names), name);
}

/* inlined into daBigelf_c::init */
static void invokeAction(daBigelf_c* actor) { (actor->*(actor->mCurrentStateFunc))(nullptr); }

/* 02097B7C */
void daBigelf_c::oct_delete() {

    gabi::Local<cXyz> offset, position;
    u32 oct = findActor(this->mOctID);
    dComIfG_inf_c* p = gameInfo();
    u16 flags = this->mStateBits;
    u32 ship = gabi::ea(p->play.mpPlayerPtr[2].get());
    if (!(flags & FLAG_OCT_EXIT_PENDING)) {
        return;
    }
    s16 timer = this->mOctTimer;
    if (timer > 0) {
        this->mOctTimer = (s16)(timer - 1);
        return;
    }
    if (oct) {
        if (ship) {
            f32 zero = l_zero, distance = l_shipExitDistance;
            offset->y = zero;
            offset->z = distance;
            offset->x = zero;
            bigelf_absolutePosition(bytes(oct), offset.get(), position.get());
            position->y = bigelf_view<fopAc_ac_c>(ship)->current.pos.y;
            u32 yaw = bigelf_targetAngleY(&this->current.pos, position.get());
            bigelf_startShipPosition(bytes(ship), position.get(), (s32)(s16)(yaw + 0x4000));
        }
        bigelf_deleteActor(bytes(oct));
        flags = this->mStateBits;
    }
    this->mStateBits = flags & ~FLAG_OCT_EXIT_PENDING;
}

/* 020945CC */
BOOL daBigelf_c::nodeCallBack(void* node) {

    gabi::Local<cXyz> offset, position;
    u32 model = l_currentModel.value;
    u32 joint = bigelf_getJoint(node);
    u16 index = bigelf_view<BigelfJointData>(joint)->mJntNo;
    u32 buffer = gabi::ea(bigelf_view<J3DModel>(model)->mpMtxBuffer.get());
    u16 flags = bigelf_view<J3DMtxBuffer_hd>(buffer)->mFlags;
    u32 matrices = bigelf_view<J3DMtxBuffer_hd>(buffer)->mpAnmMtx;
    bigelf_view<J3DMtxBuffer_hd>(buffer)->mFlags = flags | 0x10;
    u32 matrix = l_currentMatrix.value;
    bigelf_copyMatrix(bytes(gabi::ea(&bigelf_view<Mtx34>(matrices)[index])), bytes(matrix));
    s32 head = this->mJntCtrl.mHeadJntNum;
    f32 zero = l_zero;
    if ((u32)index == (u32)head) {
        s32 target = this->mAnmID == 0 ? (s32)(s16)this->mJntCtrl.mAngles[0][0] : 0;
        bigelf_cLib_addCalcAngleS(&this->mHeadTurn, target, 8, 0x400, 0x100);
        s32 yaw = this->mHeadTurn;
        bigelf_rotateZ(bytes(l_currentMatrix.value), (s32)(s16)(0u - (u32)yaw));
        offset->x = zero;
        offset->y = zero;
        offset->z = zero;
        bigelf_matrixPosition(offset.get(), position.get());
        f32 x = position->x, y = position->y, z = position->z;
        this->mCurrentPos.x = x;
        this->mCurrentPos.z = z;
        this->mCurrentPos.y = y;
        offset->x = l_handForward;
        offset->z = zero;
        offset->y = l_handEye;
        bigelf_matrixPosition(offset.get(), position.get());
        z = position->z;
        y = position->y;
        x = position->x;
        u8 count = this->mAttnSetCount;
        this->mUnkPos.x = x;
        this->mUnkPos.y = y;
        this->mUnkPos.z = z;
        if (count != 255) {
            this->mAttnSetCount = (u8)(count + 1);
        }
    } else if ((u32)index != (u32)(s32)(s8)this->mJntCtrl.mBackboneJntNum &&
               (u32)index == (u32)(s32)(s8)this->mHandRBJointIndex) {
        offset->x = zero;
        offset->y = zero;
        offset->z = zero;
        bigelf_matrixPosition(offset.get(), &this->mHandPos);
    }
    commitJointMatrix(model, index);
    return 1;
}

/* 0209487C */
static BOOL nodeCallBack_Bigelf(void* node, s32 phase) {

    if (!phase) {
        u32 model = l_currentModel.value;
        u32 actor = bigelf_view<J3DModel>(model)->mUserArea;
        bigelf_getJoint(node);
        if (actor) {
            bigelf_view<daBigelf_c>(actor)->nodeCallBack(node);
        }
    }
    return 1;
}

/* 020965DC */
void daBigelf_c::lightInit(cXyz* position) {

    u32 x = bigelf_word(position->x);
    bigelf_word(this->mLightInfluence.mPos.x) = x;
    u32 y = bigelf_word(position->y);
    u8 registered = this->mIsLightShining;
    bigelf_word(this->mLightInfluence.mPos.y) = y;
    u32 z = bigelf_word(position->z);
    bigelf_word(this->mLightInfluencePos.x) = x;
    bigelf_word(this->mLightInfluence.mPos.z) = z;
    bigelf_word(this->mLightInfluencePos.y) = y;
    bigelf_word(this->mLightInfluencePos.z) = z;
    if (registered) {
        return;
    }
    z = bigelf_word(this->mLightInfluence.mPos.z);
    y = bigelf_word(this->mLightInfluence.mPos.y);
    f32 zero = l_zero;
    this->mLightInfluenceColor.b = 255;
    this->mLightInfluence.mPower = zero;
    this->mLightInfluenceFluctuation = zero;
    this->mLightInfluence.mFluctuation = zero;
    this->mLightInfluencePower = zero;
    bigelf_word(this->mLightInfluencePos.z) = z;
    this->mLightInfluenceColor.r = 255;
    this->mLightInfluence.mColorB = 255;
    this->mIsLightShining = 1;
    this->mLightInfluenceColor.g = 255;
    this->mLightInfluence.mColorG = 255;
    this->mLightInfluence.mColorR = 255;
    x = bigelf_word(this->mLightInfluence.mPos.x);
    bigelf_word(this->mLightInfluencePos.y) = y;
    bigelf_word(this->mLightInfluencePos.x) = x;
    bigelf_setPriorityLight(&this->mLightInfluence);
}

/* 02096ECC */
void daBigelf_c::lightEnd() {

    if (this->mIsLightShining) {
        this->mIsLightShining = 0;
        dKy_plight_cut(&this->mLightInfluence);
    }
}

/* 020965D0 */
void daBigelf_c::lightProc() { this->mLightState = 0; }

/* 02096770 */
void daBigelf_c::darkInit() {

    if (this->getType() != 6) {
        this->mDarkActive = 1;
    }
}

/* 0209641C */
void daBigelf_c::darkEnd() {

    f32 one = l_one;
    this->mDarkActive = 0;
    bigelf_setActColorRatio(one);
    bigelf_setBackgroundColorRatio(one);
    bigelf_setVrboxColorRatio(one);
}

/* 0209744C */
void daBigelf_c::darkProc() {

    if (this->mDarkActive) {
        f32 target = this->m3A4;
        f32 maximum = l_one, rate = l_animationRate;
        bigelf_cLib_addCalc2(&this->m3A0, target, rate, maximum);
        f32 gain = this->m3A0;
        f32 scale = l_actorLightScale, base = l_actorLightBase;
        bigelf_setActColorRatio(gabi::fmadds(scale, gain, base));
        gain = this->m3A0;
        scale = l_backgroundLightScale;
        base = l_backgroundLightBase;
        f32 factor = gabi::fmadds(scale, gain, base);
        bigelf_setBackgroundColorRatio(factor);
        bigelf_setVrboxColorRatio(factor);
    }
}

/* 0209675C */
void daBigelf_c::demoInitFlDelete() {

    this->mDemoCounter = 0;
    this->mLightState = 255;
}

/* 02096EEC */
u32 daBigelf_c::demoProcFlDelete() {

    s16 count = (s16)((u32)(s32)(s16)this->mDemoCounter + 1);
    this->mDemoCounter = count;
    if (count < 27) {
        return 1;
    }
    if (count == 27) {
        dComIfG_inf_c* p = gameInfo();
        u32 player = gabi::ea(p->play.mpPlayerPtr[0].get());
        u16 effect = colorEffect(this, l_releaseEffects);
        particle(effect, &bigelf_view<fopAc_ac_c>(player)->current.pos);
        count = this->mDemoCounter;
        this->m3A4 = l_one;
        if (count < 27) {
            return 1;
        }
    }
    u32 emitter = this->mpLinkEmitter;
    f32 zero = l_zero;
    if (emitter) {
        u8 alpha = this->mLightState;
        if (alpha > 10) {
            alpha = (u8)(alpha - 10);
            emitter = this->mpLinkEmitter;
            this->mLightState = alpha;
            bigelf_view<BigelfEmitterView>(emitter)->mGlobalPrmColor.a = alpha;
            return 1;
        }
        emitter = this->mpLinkEmitter;
        this->mLightState = 0;
        bigelf_view<BigelfEmitterView>(emitter)->mGlobalPrmColor.a = 0;
        emitter = this->mpLinkEmitter;
        retireEmitter(emitter);
        this->mpLinkEmitter = 0;
        this->mLightRadius = zero;
        return 1;
    }
    f32 power = this->mLightInfluence.mPower;
    if (power > zero) {
        return 1;
    }
    this->lightEnd();
    cutEnd(this);
    u16 flags = this->mStateBits;
    if (flags & FLAG_CUT_PENDING) {
        this->mStateBits = flags & ~FLAG_CUT_PENDING;
        if (this->getType() == 6) {
            dComIfG_inf_c* p = gameInfo();
            s16 count = p->play.mItemMaxMagicCount;
            p->play.mItemMaxMagicCount = (u16)((u32)(s32)count + 32);
            p = gameInfo();
            count = p->play.mItemMagicCount;
            p->play.mItemMagicCount = (u16)((u32)(s32)count + 32);
        }
    }
    return 1;
}

/* 0209666C */
void daBigelf_c::demoInitFlLink() {

    gabi::Local<cXyz> offset, position;
    u32 player = gabi::ea(gameInfo()->play.mpPlayerPtr[0].get());
    f32 zero = l_zero, radius = l_linkForward, height = l_linkLightRadius;
    offset->z = radius;
    offset->x = zero;
    offset->y = height;
    bigelf_absolutePosition(bytes(player), offset.get(), position.get());
    u16 effect = colorEffect(this, l_changeEffects);
    this->mpLinkEmitter = particle(effect, position.get(), &this->shape_angle);
    this->lightInit(position.get());
    this->mLightRadius = radius;
    this->demoInitWait();
}

/* 02096EA8 */
u32 daBigelf_c::demoProcFlLink() {
    this->demoProcWait();
    return 1;
}

/* 020967E8 */
void daBigelf_c::demoInitFlDmAf() {

    gabi::Local<cXyz> position;
    f32 y = this->mHandPos.y, x = this->mHandPos.x, height = l_handForward;
    position->x = x;
    position->y = (f32)((f64)y + (f64)height);
    f32 z = this->mHandPos.z;
    u16 effect = colorEffect(this, l_changeEffects);
    position->z = z;
    this->mpChangeEmitter = particle(effect, position.get(), &this->shape_angle);
    bigelf_startSystemSound(0x7853);
}

/* 02097380 */
u32 daBigelf_c::demoProcFlDmAf() {

    if (this->mAnmFinished || !((u16)this->mStateBits & FLAG_CUT_PENDING)) {
        cutEnd(this);
        this->mStateBits = (u16)this->mStateBits & ~FLAG_CUT_PENDING;
        u32 emitter = this->mpChangeEmitter;
        if (emitter) {
            retireEmitter(emitter);
            this->mpChangeEmitter = 0;
        }
    } else {
        f32 low = l_flowerBeginFrame,
            frame = bigelf_view<mDoExt_McaMorf>((u32)gabi::ea(this->mpBckAnimator.get()))
                        ->mFrameCtrl.mFrame;
        if (!(frame < low)) {
            f32 high = l_flowerEndFrame;
            if (!(frame > high)) {
                this->mStateBits = (u16)this->mStateBits | 8u;
            }
        }
    }
    return 1;
}

/* 020967A4 */
void daBigelf_c::demoInitFlDmMd() {

    this->darkInit();
    u16 flags = this->mStateBits;
    f32 one = l_one;
    this->m3A0 = one;
    this->m3A4 = one;
    this->mStateBits =
        (flags & ~FLAG_LIGHT_CREATED) | (FLAG_DIM_SOUND_PENDING | FLAG_CHIME_SOUND_PENDING);
}

/* 02097130 */
u32 daBigelf_c::demoProcFlDmMd() {

    gabi::Local<cXyz> position;
    u16 flags = this->mStateBits;
    f32 frame =
        bigelf_view<mDoExt_McaMorf>((u32)gabi::ea(this->mpBckAnimator.get()))->mFrameCtrl.mFrame;
    if (!(flags & FLAG_CUT_PENDING)) {
        cutEnd(this);
        return 1;
    }
    f32 begin = l_dimBeginFrame, lightFrame = l_flowerBeginFrame;
    if (!(frame < begin)) {
        if (frame < lightFrame) {
            f32 difference = (f32)((f64)lightFrame - (f64)frame);
            f32 rate = l_blendStep;
            flags = this->mStateBits;
            this->m3A4 = (f32)((f64)difference * (f64)rate);
        } else {
            f32 zero = l_zero;
            flags = this->mStateBits;
            this->m3A4 = zero;
        }
    }
    if (!(flags & FLAG_LIGHT_CREATED) && !(frame < lightFrame)) {
        u16 yaw = (u16)this->shape_angle.y;
        f32 z = this->mHandPos.z, distance = l_lightForward, x = this->mHandPos.x;

        this->mStateBits = flags | FLAG_LIGHT_CREATED;
        f32 sine = cM_ssin(yaw), y = this->mHandPos.y, cosine = cM_scos(yaw);
        position->y = y;
        position->x = gabi::fmadds(sine, distance, x);
        position->z = gabi::fmadds(cosine, distance, z);
        this->lightInit(position.get());
        f32 radius = l_linkLightRadius;
        s32 room = (s8)this->current.roomNo;
        this->mLightRadius = radius;
        s32 reverb = bigelf_getReverb(room);
        bigelf_startSound(0x6A2F, &this->eyePos, 0, reverb);
        flags = this->mStateBits;
    }
    if (flags & FLAG_DIM_SOUND_PENDING) {
        f32 threshold = l_dimSoundFrame;
        if (!(frame < threshold)) {
            s32 room = (s8)this->current.roomNo;
            this->mStateBits = flags & ~FLAG_DIM_SOUND_PENDING;
            s32 reverb = bigelf_getReverb(room);
            bigelf_startSound(0x492B, &this->eyePos, 0, reverb);
            flags = this->mStateBits;
        }
    }
    if (flags & FLAG_CHIME_SOUND_PENDING) {
        f32 threshold = l_chimeSoundFrame;
        if (!(frame < threshold)) {
            s32 room = (s8)this->current.roomNo;
            this->mStateBits = flags & ~FLAG_CHIME_SOUND_PENDING;
            s32 reverb = bigelf_getReverb(room);
            bigelf_startSound(0x492C, &this->eyePos, 0, reverb);
        }
    }
    f32 threshold = l_dimEndFrame;
    if (!(frame < threshold)) {
        cutEnd(this);
        this->mStateBits = (u16)this->mStateBits & ~FLAG_CUT_PENDING;
    }
    if (!(frame < lightFrame)) {
        f32 end = l_flowerEndFrame;
        if (!(frame > end)) {
            this->mStateBits = (u16)this->mStateBits | FLAG_FLOWER_VISIBLE;
        }
    }
    return 1;
}

/* 0209709C */
u32 daBigelf_c::demoProcFlDmBf() {

    if (!((u16)this->mStateBits & FLAG_CUT_PENDING)) {
        cutEnd(this);
        return 1;
    }
    if (this->mAnmFinished) {
        this->setAnm(2);
        cutEnd(this);
        this->mStateBits = (u16)this->mStateBits & ~FLAG_CUT_PENDING;
    }
    return 1;
}

/* 02096CA8 */
u32 daBigelf_c::demoProcFlDemo() {

    gabi::Local<cXyz> position;
    if (this->mAnmFinished) {
        u16 flags = this->mStateBits;
        if (flags & FLAG_CUT_PENDING) {
            s32 animation = this->mAnmID;
            if (animation == 0) {
                this->setAnm(2);
            } else if (animation == 2) {
                this->mStateBits = flags & ~FLAG_CUT_PENDING;
                cutEnd(this);
            } else {
                return 1;
            }
        } else {
            cutEnd(this);
        }
    }
    if ((s8)this->mAnmID != 2) {
        return 1;
    }
    f32 low = l_flowerBeginFrame,
        frame = bigelf_view<mDoExt_McaMorf>((u32)gabi::ea(this->mpBckAnimator.get()))
                    ->mFrameCtrl.mFrame;
    u8 state = this->mLightState;
    if (!(frame < low)) {
        f32 high = l_flowerEndFrame;
        if (!(frame > high)) {
            this->mStateBits = (u16)this->mStateBits | FLAG_FLOWER_VISIBLE;
        }
    }
    if (state == 0) {
        f32 y = this->mHandPos.y, height = l_handForward, threshold = l_flowerCreateFrame;
        f32 x = this->mHandPos.x, z = this->mHandPos.z;
        position->y = (f32)((f64)y + (f64)height);
        position->z = z;
        position->x = x;
        if (frame < threshold) {
            return 1;
        }
        this->mLightState = state + 1;
        u16 effect = colorEffect(this, l_changeEffects);
        this->mpChangeEmitter = particle(effect, position.get(), &this->shape_angle);
        bigelf_startSystemSound(0x7853);
    } else if (state == 1 && !((u16)this->mStateBits & FLAG_CUT_PENDING) && this->mpChangeEmitter) {
        u32 emitter = this->mpChangeEmitter;
        this->mLightState = state + 1;
        retireEmitter(emitter);
        this->mpChangeEmitter = 0;
    }
    return 1;
}

/* 02096478 */
void daBigelf_c::demoInitExit() {

    gabi::Local<cXyz> position, scale;
    u32 emitter = this->mpAppearEmitter;
    if (emitter) {
        retireEmitter(emitter);
        this->mpAppearEmitter = 0;
    }
    u16 flags = this->mStateBits;
    f32 y = this->current.pos.y, height = l_exitHeight, one = l_one;
    this->mDemoCounter = 0;
    f32 x = this->current.pos.x;
    this->mDisappearTarget = one;
    scale->y = one;
    position->x = x;
    position->y = (f32)((f64)y + (f64)height);
    scale->x = one;
    this->mDisappearScale = one;
    scale->z = one;
    this->mStateBits = flags | FLAG_DISAPPEARING;
    f32 z = this->current.pos.z;
    this->mHeightOffset = height;
    position->z = z;
    particle(0x272, position.get(), nullptr, scale.get());
    s32 room = (s8)this->current.roomNo;
    f32 zero = l_zero;
    this->tevStr.mFogColor.r = 255;
    this->tevStr.mFogStartZ = zero;
    this->tevStr.mFogColor.b = 255;
    f32 power = l_fogEnd;
    this->tevStr.mFogColor.g = 255;
    this->tevStr.mFogEndZ = power;
    s32 reverb = bigelf_getReverb(room);
    bigelf_startSound(0x594B, &this->eyePos, 0, reverb);
    sound(this, 0x492D);
    this->darkEnd();
}

/* 02096B20 */
u32 daBigelf_c::demoProcExit() {

    f32 ten = l_exitFogTarget;
    s16 timer = this->mDemoCounter;
    f32 one = l_one, maximum = l_lightStep;
    this->mDemoCounter = (s16)((u32)(s32)timer + 1);
    bigelf_cLib_addCalc2(&this->tevStr.mFogEndZ, ten, one, maximum);
    timer = this->mDemoCounter;
    f32 rate = l_animationRate;
    if (timer < 70) {
        f32 step = l_blendStep;
        bigelf_cLib_addCalc0(&this->mDisappearScale, rate, step);
        bigelf_cLib_addCalc0(&this->mDisappearTarget, rate, step);
        return 1;
    }
    if (timer == 70) {
        sound(this, 0x5905);
        if (this->getType() == 6) {
            bigelf_stopBgm(45);
        }
    }
    f32 target = l_disappearStretch;
    bigelf_cLib_addCalc2(&this->mDisappearTarget, target, rate, one);
    bigelf_cLib_addCalc0(&this->mDisappearScale, rate, l_blendMinimum);
    if ((s16)this->mDemoCounter >= 90) {
        cutEnd(this);
        this->mStateBits = ((u16)this->mStateBits | FLAG_HIDDEN) & 0xFFEF;
        if (this->getType() != 6) {
            this->makeFa1S();
        }
        this->m3A8 = one;
    }
    return 1;
}

/* 02096328 */
void daBigelf_c::demoInitTalk() {

    this->talkInit();
    s32 staff = this->mStaffId;
    u32 message = bigelf_getSubstance(&gameInfo()->play.mEvtManager, staff, "MsgNo", 3);
    if (!message) {
        bigelf_assertion("d_a_bigelf.cpp", 0x333, "a_intP");
    }
    u32 id = *bigelf_view<be<u32>>(message);
    this->mCurrentMessageId = id;
    if (id == 12010) {
        f32 life = (f32)save()->mSavedata.mPlayer.mPlayerStatusA.mMaxLife;
        dComIfG_inf_c* p = gameInfo();
        f32 old = p->play.mItemLifeCount;
        p->play.mItemLifeCount = (f32)((f64)old + (f64)life);
        u8 count = save()->mSavedata.mPlayer.mPlayerStatusA.mMaxMagic;
        p = gameInfo();
        s16 previous = p->play.mItemMagicCount;
        p->play.mItemMagicCount = (u16)((u32)(s32)previous + count);
    }
}

/* 02096ACC */
u32 daBigelf_c::demoProcTalk() {

    u32 status = this->talk();
    if (status == 0x12 || status == 0xFE) {
        s32 staff = this->mStaffId;
        bigelf_endCut(&gameInfo()->play.mEvtManager, staff);
    }
    return 1;
}

/* 0209611C */
void daBigelf_c::demoInitAppear() {

    gabi::Local<cXyz> position, direction;
    u32 fairy = findActor(this->mpFairyActor);
    if (fairy) {
        dEvt_control_c* event = &gameInfo()->play.mEvtCtrl;
        u32 process = bigelf_getEventProcessId(event, this);
        event->mPtTalk = process;
        bigelf_deleteActor(bytes(fairy));
    }
    f32 x = this->current.pos.x, y = this->current.pos.y;
    f32 height = l_appearHeight, z = this->current.pos.z;
    position->x = x;
    position->z = z;
    position->y = (f32)((f64)y + (f64)height);
    dComIfG_inf_c* p = gameInfo();
    f32 zero = l_zero, one = l_one;
    direction->z = zero;
    direction->y = one;
    direction->x = zero;
    bigelf_startShock(&p->play.mVibration, 5, -33, direction.get());
    u16 effect = colorEffect(this, l_appearEffects);
    particle(effect, position.get(), nullptr, &this->scale);
    effect = colorEffect(this, l_flashEffects);
    particle(effect, position.get(), nullptr, &this->scale);
    if (this->getType() == 6) {
        this->mAttentionDelay = 15;
    }
    this->m3A8 = zero;
    this->mLightState = 15;
    if (this->getType() == 6) {
        sound(this, 0x594D);
        bigelf_startBgm(0x80000053u);
    }
    this->mStateBits = (u16)this->mStateBits | FLAG_APPEAR_SOUND_PENDING;
}

/* 02096988 */
u32 daBigelf_c::demoProcAppear() {

    u8 timer = this->mLightState;
    if (timer) {
        timer = (u8)(timer - 1);
        this->mLightState = timer;
        if (timer) {
            return 1;
        }
        this->mStateBits = (u16)this->mStateBits & ~FLAG_HIDDEN;
        this->setAnm(1);
        if (!this->mpAppearEmitter) {
            this->mpAppearEmitter = particle(0x8346, &this->current.pos);
        }
        return 1;
    }
    u16 flags = this->mStateBits;
    if (flags & FLAG_APPEAR_SOUND_PENDING) {
        f32 frame = bigelf_view<mDoExt_McaMorf>((u32)gabi::ea(this->mpBckAnimator.get()))
                        ->mFrameCtrl.mFrame,
            threshold = l_appearSoundFrame;
        if (!(frame < threshold)) {
            s32 room = (s8)this->current.roomNo;
            this->mStateBits = flags & ~FLAG_APPEAR_SOUND_PENDING;
            s32 reverb = bigelf_getReverb(room);
            bigelf_startSound(0x492A, &this->eyePos, 0, reverb);
        }
    }
    if (this->mAnmFinished) {
        this->setAnm(0);
        cutEnd(this);
    }
    f32 scale = this->scale.x;
    this->scale.z = scale;
    this->scale.y = scale;
    return 1;
}

/* 020960A4 */
void daBigelf_c::demoInitFa1() {

    u32 fairy = findActor(this->mpFairyActor);
    if (fairy) {
        bigelf_changeFairy(bytes(fairy));
        s32 reverb = bigelf_getReverb((s32)(s8)this->current.roomNo);
        bigelf_startSound(0x594A, &this->eyePos, 0, reverb);
    }
}

/* 020968F4 */
u32 daBigelf_c::demoProcFa1() {

    u32 fairy = findActor(this->mpFairyActor);
    if (fairy) {
        f32 y = this->current.pos.y;
        f32 height = l_appearHeight, rate = l_fairyMoveRate;
        f32 target = (f32)((f64)y + (f64)height), max = l_fairyHeight;
        bigelf_cLib_addCalc2(&bigelf_view<fopAc_ac_c>(fairy)->current.pos.y, target, rate, max);
    }
    s32 staff = this->mStaffId;
    bigelf_endCut(&gameInfo()->play.mEvtManager, staff);
    return 1;
}

/* 02096034 */
void daBigelf_c::demoInitWait() {

    s32 staff = this->mStaffId;
    u32 timer = bigelf_getSubstance(&gameInfo()->play.mEvtManager, staff, "Timer", 3);
    this->mDemoCounter = timer ? bigelf_view<be<s16>>(timer)[1] : 0;
    this->setAnm(0);
}

/* 0209689C */
u32 daBigelf_c::demoProcWait() {

    s16 count = this->mDemoCounter;
    if (count > 0) {
        this->mDemoCounter = (s16)(count - 1);
    } else {
        s32 staff = this->mStaffId;
        bigelf_endCut(&gameInfo()->play.mEvtManager, staff);
    }
    return 0;
}

/* 02095F60 */
void daBigelf_c::demoInitCom() {

    u16 flags = this->mStateBits;
    s32 staff = this->mStaffId;
    this->mStateBits = flags | FLAG_CUT_PENDING;
    if (!bigelf_getSubstance(&gameInfo()->play.mEvtManager, staff, "Ship", 3)) {
        return;
    }
    u32 ship = gabi::ea(gameInfo()->play.mpPlayerPtr[2].get());
    if (ship) {
        gabi::Local<cXyz> offset, position;
        f32 zero = l_zero, distance = l_shipStartDistance;
        offset->y = zero;
        offset->z = distance;
        offset->x = zero;
        bigelf_absolutePosition(this, offset.get(), position.get());
        position->y = bigelf_view<fopAc_ac_c>(ship)->current.pos.y;
        u32 yaw = bigelf_targetAngleY(&this->current.pos, position.get());
        bigelf_startShipPosition(bytes(ship), position.get(), (s32)(s16)(yaw + 0x4000u));
    }
    this->mStateBits = (u16)this->mStateBits | FLAG_FACE_FIXED;
}

/* 020974F8 */
void daBigelf_c::demoProcCom() { this->darkProc(); }

/* 02095F14 */
s32 daBigelf_c::getNowEventAction() {

    s32 staff = this->mStaffId;
    return bigelf_getActionIndex(&gameInfo()->play.mEvtManager, staff, l_actionNames, 11, 0, 1);
}

/* 020974FC */
void daBigelf_c::demoProc() {

    s32 action = this->getNowEventAction(), staff = this->mStaffId;
    if (bigelf_isAdvance(&gameInfo()->play.mEvtManager, staff)) {
        this->demoInitCom();
        switch (action) {
        case ACT_WAIT:
            this->demoInitWait();
            break;
        case ACT_FA1:
            this->demoInitFa1();
            break;
        case ACT_APPEAR:
            this->demoInitAppear();
            break;
        case ACT_TALK:
            this->demoInitTalk();
            break;
        case ACT_EXIT:
            this->demoInitExit();
            break;
        case ACT_FL_DEMO:
            this->lightProc();
            break;
        case ACT_FL_LINK:
            this->demoInitFlLink();
            break;
        case ACT_FL_DELETE:
            this->demoInitFlDelete();
            break;
        case ACT_FL_DM_MD:
            this->demoInitFlDmMd();
            break;
        case ACT_FL_DM_AF:
            this->demoInitFlDmAf();
            break;
        }
    }
    switch (action) {
    case ACT_WAIT:
        this->demoProcWait();
        break;
    case ACT_FA1:
        this->demoProcFa1();
        break;
    case ACT_APPEAR:
        this->demoProcAppear();
        break;
    case ACT_TALK:
        this->demoProcTalk();
        break;
    case ACT_EXIT:
        this->demoProcExit();
        break;
    case ACT_FL_DEMO:
        this->demoProcFlDemo();
        break;
    case ACT_FL_LINK:
        this->demoProcFlLink();
        break;
    case ACT_FL_DELETE:
        this->demoProcFlDelete();
        break;
    case ACT_FL_DM_BF:
        this->demoProcFlDmBf();
        break;
    case ACT_FL_DM_MD:
        this->demoProcFlDmMd();
        break;
    case ACT_FL_DM_AF:
        this->demoProcFlDmAf();
        break;
    default:
        cutEnd(this);
        break;
    }
    this->demoProcCom();
}

/* 020948E0 */
u32 daBigelf_c::getType() { return (u8)(u32)this->mParameters; }

/* 020958E0 */
u32 daBigelf_c::getSwbit() { return ((u32)this->mParameters >> 8) & 255; }

/* 020958EC */
u32 daBigelf_c::getSwbit2() { return ((u32)this->mParameters >> 16) & 255; }

/* 02094ED4 */
u32 daBigelf_c::getEventFlag() {

    u32 type = this->getType();
    return type <= 6 ? l_eventFlags[type] : 0;
}

/* 02094F20 */
void daBigelf_c::makeFa1S() {

    gabi::Local<cXyz> position;
    gabi::Local<csXyz> angle;
    position->x = this->current.pos.x;
    position->y = (f32)((f64)(f32)this->current.pos.y + (f64)(f32)this->mHeightOffset);
    position->z = this->current.pos.z;
    angle->x = this->current.angle.x;
    angle->y = this->current.angle.y;
    angle->z = this->current.angle.z;
    for (u32 i = 0; i < 10; ++i) {
        bigelf_createActor(fpcNm_NPC_FA1_e, 4, position.get(), (s32)(s8)this->current.roomNo,
                           angle.get(), 0, -1, 0);
        angle->y = (s16)((u32)(s32)(s16)angle->y + 10000u);
    }
}

/* 02094FC8 */
void daBigelf_c::makeFa1() {

    gabi::Local<cXyz> position;
    gabi::Local<csXyz> angle;
    angle->x = this->current.angle.x;
    angle->y = this->current.angle.y;
    angle->z = this->current.angle.z;
    position->x = this->current.pos.x;
    position->y = (f32)((f64)(f32)this->current.pos.y + (f64)l_fairyHeight);
    position->z = this->current.pos.z;
    this->mpFairyActor = bigelf_createActor(fpcNm_NPC_FA1_e, 6, position.get(),
                                            (s32)(s8)this->current.roomNo, angle.get(), 0, -1, 0);
}

/* 020958F8 */
void daBigelf_c::setAnm(s32 animation) {

    f32 speed = l_one, morph = l_morphFrames;
    f32 end = l_animationEnd, start = l_zero;
    s32 mode = -1;
    if ((u32)animation >= 1 && (u32)animation < 3) {
        morph = start;
    } else if (animation == 3) {
        speed = end;
        end = l_fairyHeight;
        mode = 3;
    }
    s32 current = (s8)this->mAnmID;
    if (current == 1) {
        morph = start;
    }
    if ((u32)animation == (u32)current || animation == -1) {
        return;
    }
    this->mPrevFrame = start;
    this->mAnmID = (s8)animation;
    this->mAnmFinished = 0;
    s32 resource = l_bckIndex[(u32)(s32)(s8)animation];
    u32 manager = l_resourceManager.value;
    gabi::Local<SafeString> name;
    name->__vtbl = gc_tu.safeString;
    name->mStringTop = gabi::guest_str("bigelf");
    u32 data = bigelf_getResource(bytes(manager), name.get(), resource);
    u32 animator = gabi::ea(this->mpBckAnimator.get());
    bigelf_setMorfAnimation(bytes(animator), bytes(data), mode, 0, morph, speed, start, end);
}

/* 02095A88 */
void daBigelf_c::setAnmStatus() { this->setAnm(0); }

/* 02095A90 */
u32 daBigelf_c::next_msgStatus(be<u32>* message) {

    u32 id = *message;
    if (id == 12008) {
        *message = 12009;
        u32 type = this->getType();
        u8 item;
        if (type <= 1) {
            item = save()->mSavedata.mPlayer.mPlayerStatusA.mWalletSize ? dItemNo_MAX_RUPEE_UP2_e
                                                                        : dItemNo_MAX_RUPEE_UP1_e;
        } else if (type <= 3) {
            item = save()->mSavedata.mPlayer.mItemMax.mItemMax2.mBombNum > 30
                       ? dItemNo_MAX_BOMB_UP2_e
                       : dItemNo_MAX_BOMB_UP1_e;
        } else if (type <= 5) {
            item = save()->mSavedata.mPlayer.mItemMax.mItemMax2.mArrowNum > 30
                       ? dItemNo_MAX_ARROW_UP2_e
                       : dItemNo_MAX_ARROW_UP1_e;
        } else {
            item = dItemNo_RED_RUPEE_e;
        }
        this->mGivenItem = item;
        gameInfo()->play.mEvtCtrl.mGetItemNo = item;
        return 15;
    }
    if (id == 12011 || id == 12012 || id == 12015 || id == 12016) {
        *message = id + 1;
        return 15;
    }
    if (id == 12017) {
        *message = 12014;
        return 15;
    }
    return 16;
}

/* 02095BC8 */
u32 daBigelf_c::getMsg() {

    u32 message = this->mCurrentMessageId;
    if (message == 12014 && save()->mSavedata.mPlayer.mPlayerItem.mItems[12] == 39) {
        message = 12015;
        this->mCurrentMessageId = message;
    }
    return message;
}

/* 02095BF8 */
void daBigelf_c::talkInit() {

    this->mTalkPhase = 0;
    this->mMsgState = 255;
}

/* 02095C0C */
u32 daBigelf_c::talk() {

    u32 manager = l_messageManager.value;
    s32 phase = (s8)this->mTalkPhase;
    if (!phase) {
        l_messageHandle.value = fpcM_ERROR_PROCESS_ID_e;
        this->mCurrentMsgNo = this->getMsg();
        this->mTalkPhase = 1;
        return 255;
    }
    if (phase == -1) {
        return 255;
    }
    if (l_messageHandle.value == fpcM_ERROR_PROCESS_ID_e) {
        u32 message = this->mCurrentMsgNo;
        u32 handle = bigelf_setMessage(bytes(manager), message, &this->eyePos);
        l_messageHandle.value = handle;
        return 255;
    }
    if (!((u16)this->mStateBits & FLAG_TALK_CONTROL)) {
        gameInfo();
        phase = (s8)this->mTalkPhase;
    }
    if (phase == 1) {
        this->mTalkPhase = 2;
        return 255;
    }
    if (phase != 2) {
        return 255;
    }
    u16 status = (u16)bigelf_getMessageStatus(bytes(manager));
    if (status == 14) {
        u32 next = this->next_msgStatus(&this->mCurrentMsgNo);
        bigelf_setMessageStatus(bytes(manager), next);
        if (bigelf_getMessageStatus(bytes(manager)) == 15) {
            u32 message = this->mCurrentMsgNo;
            bigelf_setMessage(bytes(manager), message, 0);
        }
    } else if (status == 18) {
        bigelf_setMessageStatus(bytes(manager), 19);
        this->mTalkPhase = -1;
    }
    return status;
}

/* 02095058 */
BOOL daBigelf_c::init() {

    if (!this->mInitialized) {
        s16 dispatch = this->mCurrentStateFunc.i;
        bool current = dispatch == -1 && (s16)this->mCurrentStateFunc.d == 0 &&
                       (u32)this->mCurrentStateFunc.f == l_waitActionTarget;
        if (!current) {
            if (dispatch != 0) {
                this->mActionPhase = -1;
                invokeAction(this);
            }
            this->mActionPhase = 0;
            this->mCurrentStateFunc.f = l_waitActionTarget;
            this->mCurrentStateFunc.i = -1;
            this->mCurrentStateFunc.d = 0;
            (this->*(this->mCurrentStateFunc))(nullptr);
        }
    }
    f32 x = this->current.pos.x, y = this->home.pos.y, height = l_spawnHeight;
    this->mpFairyActor = fpcM_ERROR_PROCESS_ID_e;
    y = (f32)((f64)y + (f64)height);
    f32 eyeHeight = l_fairyHeight;
    this->attention_info.position.x = x;
    this->eyePos.x = x;
    this->current.pos.y = y;
    f32 eyeY = (f32)((f64)y + (f64)eyeHeight), z = this->current.pos.z,
        attentionHeight = l_attentionHeight;
    this->eyePos.y = eyeY;
    f32 attentionY = (f32)((f64)eyeY + (f64)attentionHeight);
    this->mUnkPos.y = eyeY;
    this->mCurrentPos.y = eyeY;
    this->attention_info.position.y = attentionY;
    this->mUnkPos.x = x;
    this->attention_info.position.z = z;
    this->eyePos.z = z;
    this->mUnkPos.z = z;
    this->mCurrentPos.x = x;
    this->mCurrentPos.z = z;
    if (this->getType() != 6) {
        u32 flag = this->getEventFlag();
        if (bigelf_isEventBit(&save()->mSavedata.mEvent, flag)) {
            this->makeFa1S();
        } else {
            this->makeFa1();
        }
    }
    this->mStateBits = (u16)this->mStateBits | FLAG_HIDDEN;
    return 1;
}

/* 02095D70 */
void daBigelf_c::setAttention(BOOL force) {

    s16 delay = this->mAttentionDelay;
    if (delay > 0) {
        this->mAttentionDelay = (s16)(delay - 1);
        return;
    }
    if (!force && (u8)this->mAttnSetCount >= 2) {
        return;
    }
    f32 x = this->mCurrentPos.x, y = this->mCurrentPos.y;
    f32 height = l_attentionHeight;
    this->attention_info.position.x = x;
    y = (f32)((f64)y + (f64)height);
    f32 z = this->mCurrentPos.z, eyeZ = this->mUnkPos.z;
    this->attention_info.position.y = y;
    this->attention_info.position.z = z;
    this->eyePos.z = eyeZ;
    f32 eyeX = this->mUnkPos.x, eyeY = this->mUnkPos.y;
    this->eyePos.x = eyeX;
    this->eyePos.y = eyeY;
}

/* 02095DDC */
void daBigelf_c::lookBack() {

    dComIfG_inf_c* p = gameInfo();
    u16 flags = this->mStateBits;
    u32 player = gabi::ea(p->play.mpPlayerPtr[0].get());
    if (!(flags & FLAG_FACE_FIXED)) {
        s16 yaw = bigelf_fopAcM_searchActorAngleY(this, bytes(player));
        this->shape_angle.y = yaw;
        this->current.angle.y = yaw;
    }
}

/* 02095E28 */
u32 daBigelf_c::hunt() {

    u32 fairy = findActor(this->mpFairyActor);
    u32 player = gabi::ea(gameInfo()->play.mpPlayerPtr[0].get());
    if (!fairy) {
        this->mState = STATE_DONE;
        return 0;
    }
    f32 distance = bigelf_fopAcM_searchActorDistanceXZ(this, bytes(player));
    f32 limit = l_huntDistance;
    if (distance < limit) {
        this->mState = STATE_READY;
        u32 event = bigelf_getEventIndex(&gameInfo()->play.mEvtManager, "BIGELF_ARRIVAL", 255);
        this->eventID = (s16)event;
        bigelf_orderOtherEvent(this, event, 255, 0xFFFF, 0, 1);
    }
    return 1;
}

/* 02097B10 */
BOOL daBigelf_c::oct_search() {

    u32 oct = bigelf_searchActorName("Daiocta", 0, 0);
    if (oct) {
        this->mOctID = bigelf_view<fopAc_ac_c>(oct)->mBsPcId;
        this->mState = STATE_OCT;
        this->mOctTimer = 10;
    }
    return 1;
}

/* 02097930 */
u32 daBigelf_c::oct() {

    gabi::Local<cXyz> origin, offset;
    u32 oct = findActor(this->mOctID);
    if (oct) {
        u32 x = bigelf_word(bigelf_view<fopAc_ac_c>(oct)->current.pos.x);
        bigelf_word(this->current.pos.x) = x;
        u32 y = bigelf_word(bigelf_view<fopAc_ac_c>(oct)->current.pos.y);
        f32 homeY = this->home.pos.y;
        bigelf_word(this->current.pos.y) = y;
        u32 z = bigelf_word(bigelf_view<fopAc_ac_c>(oct)->current.pos.z);
        bigelf_word(this->eyePos.x) = x;
        bigelf_word(this->current.pos.z) = z;
        f32 currentZ = this->current.pos.z, currentX = this->current.pos.x;
        this->current.pos.y = homeY;
        y = bigelf_word(this->current.pos.y);
        this->attention_info.position.x = currentX;
        bigelf_word(this->eyePos.y) = y;
        this->attention_info.position.z = currentZ;
        bigelf_word(this->eyePos.z) = z;
        this->attention_info.position.y = homeY;
        s32 yaw = bigelf_view<fopAc_ac_c>(oct)->shape_angle.y;
        f32 offsetX = l_octOffsetX, offsetZ = l_octOffsetZ, zero = l_zero;
        offset->z = offsetZ;
        origin->x = zero;
        origin->y = zero;
        origin->z = zero;
        this->current.angle.y = (s16)yaw;
        offset->x = offsetX;
        offset->y = zero;
        u32 extra = bigelf_targetAngleY(origin.get(), offset.get());
        u16 flags = this->mStateBits;
        this->shape_angle.y = (s16)((u32)yaw + extra);
        this->current.angle.y = (s16)((u32)yaw + extra);
        this->mStateBits = flags | FLAG_FACE_FIXED;
    } else {
        this->mStateBits = (u16)this->mStateBits & ~FLAG_FACE_FIXED;
    }
    u32 bit = this->getSwbit();
    if (!bigelf_isSwitch(save(), bit, (s32)(s8)this->current.roomNo)) {
        return 1;
    }
    s16 timer = this->mOctTimer;
    if (timer > 0) {
        this->mOctTimer = (s16)(timer - 1);
        return 1;
    }
    u32 player = gabi::ea(gameInfo()->play.mpPlayerPtr[0].get());
    this->mState = STATE_READY;
    u32 event = bigelf_getEventIndex(&gameInfo()->play.mEvtManager, "BIGELF_ARRIVAL2", 255);
    this->eventID = (s16)event;
    bigelf_orderChangeEvent(this, bytes(player), event, 0, 0xFFFF);
    u16 flags = this->mStateBits;
    this->mOctTimer = 30;
    this->mStateBits = flags | FLAG_OCT_EXIT_PENDING;
    return 1;
}

/* 020977AC */
u32 daBigelf_c::ready0() {

    findActor(this->mpFairyActor);
    if (this->eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        this->mState = STATE_EVENT;
        u32 staff = bigelf_getStaffId(&gameInfo()->play.mEvtManager, "BigElf", 0, 0);
        this->mStaffId = staff;
        this->m3A8 = l_one;
        this->demoProc();
    } else {
        bigelf_orderOtherEvent(this, (s32)(s16)this->eventID, 255, 0xFFFF, 0, 1);
    }
    return 1;
}

/* 02097868 */
u32 daBigelf_c::event0() {

    s32 id = (s16)this->eventID;
    if (bigelf_eventEndCheck(&gameInfo()->play.mEvtManager, id)) {
        u32 flag = this->getEventFlag();
        bigelf_onEventBit(&save()->mSavedata.mEvent, flag);
        this->mState = STATE_DONE;
        dComIfG_inf_c* p = gameInfo();
        u16 flags = p->play.mEvtCtrl.mEventFlag;
        p->play.mEvtCtrl.mEventFlag = flags | 8;
        if (this->getType() == 6) {
            u32 bit = this->getSwbit2();
            if (bit != 255) {
                bigelf_onSwitch(save(), bit, (s32)(s8)this->current.roomNo);
            }
        }
    } else {
        this->demoProc();
    }
    return 1;
}

/* 02097C98 */
BOOL daBigelf_c::wait_action(void* argument) {

    s32 phase = this->mActionPhase;
    if (!phase) {
        u32 flag = this->getEventFlag();
        u32 done = bigelf_isEventBit(&save()->mSavedata.mEvent, flag);
        if (done) {
            this->mState = STATE_DONE;
        } else if (this->getType() == 6) {
            u32 bit = this->getSwbit();
            if (bigelf_isSwitch(save(), bit, (s32)(s8)this->current.roomNo)) {
                bit = this->getSwbit2();
                if (bit != 255) {
                    bigelf_onSwitch(save(), bit, (s32)(s8)this->current.roomNo);
                }
                this->mState = STATE_DONE;
            } else {
                this->mState = STATE_SEARCH_OCT;
            }
        } else {
            this->mState = STATE_HUNT;
        }
        this->setAnmStatus();
        this->mActionPhase = (s8)((u8)this->mActionPhase + 1);
        return 1;
    }
    if (phase == -1) {
        return 1;
    }
    s32 state = (s8)this->mState;
    u32 attention = 0;
    switch (state) {
    case STATE_HUNT:
        attention = this->hunt();
        break;
    case STATE_READY:
        attention = this->ready0();
        break;
    case STATE_EVENT:
        attention = this->event0();
        break;
    case STATE_DONE:
        this->lookBack();
        this->setAttention(1);
        break;
    case STATE_SEARCH_OCT:
        attention = this->oct_search();
        break;
    case STATE_OCT:
        attention = this->oct();
        break;
    }
    if (state != 3) {
        this->lookBack();
        this->setAttention(attention);
    }
    if ((u16)this->mStateBits & FLAG_HIDDEN) {
        u32 z = bigelf_word(this->current.pos.z), y = bigelf_word(this->current.pos.y);
        bigelf_word(this->attention_info.position.z) = z;
        bigelf_word(this->eyePos.y) = y;
        u32 x = bigelf_word(this->current.pos.x);
        bigelf_word(this->attention_info.position.y) = y;
        bigelf_word(this->eyePos.x) = x;
        bigelf_word(this->eyePos.z) = z;
        bigelf_word(this->attention_info.position.x) = x;
    }
    this->oct_delete();
    return 1;
}

/* 02095738 */
BOOL daBigelf_c::_draw() {

    u32 morf = gabi::ea(this->mpBckAnimator.get()), flower = gabi::ea(this->mpFlowerModel.get()),
        model = gabi::ea(bigelf_view<mDoExt_McaMorf>(morf)->mpModel.get());
    u16 flags = this->mStateBits;
    u32 data = gabi::ea(bigelf_view<J3DModel>(model)->mModelData.get()),
        flowerData = gabi::ea(bigelf_view<J3DModel>(flower)->mModelData.get());
    if (flags & FLAG_HIDDEN) {
        return 0;
    }
    if (!(flags & FLAG_DISAPPEARING)) {
        u32 environment = bigelf_getEnvironment();
        bigelf_settingTev(bytes(environment), 0, &this->current.pos, &this->tevStr);
    }
    u32 environment = bigelf_getEnvironment();
    bigelf_setLightTev(bytes(environment), bytes(model), &this->tevStr);
    environment = bigelf_getEnvironment();
    bigelf_setLightTev(bytes(environment), bytes((u32)gabi::ea(this->mpFlowerModel.get())),
                       &this->tevStr);
    f32 frame = this->mBrkAnimator.mFrameCtrl.mFrame;
    bigelf_mDoExt_brkAnm_entry(&this->mBrkAnimator, bytes(data), frame);
    frame = this->mBtkAnimator.mFrameCtrl.mFrame;
    bigelf_mDoExt_btkAnm_entry(&this->mBtkAnimator, bytes(data), frame);
    bigelf_updateMorf(bytes((u32)gabi::ea(this->mpBckAnimator.get())));
    if ((u16)this->mStateBits & FLAG_FLOWER_VISIBLE) {
        frame = this->mFlowerBrkAnimator.mFrameCtrl.mFrame;
        bigelf_mDoExt_brkAnm_entry(&this->mFlowerBrkAnimator, bytes(flowerData), frame);
        s32 hand = this->mHandRBJointIndex;
        u32 buffer = gabi::ea(bigelf_view<J3DModel>(model)->mpMtxBuffer.get()),
            matrices = bigelf_view<J3DMtxBuffer_hd>(buffer)->mpAnmMtx;
        u16 state = bigelf_view<J3DMtxBuffer_hd>(buffer)->mFlags;
        flower = gabi::ea(this->mpFlowerModel.get());
        bigelf_view<J3DMtxBuffer_hd>(buffer)->mFlags = state | 0x10;
        copyMatrix(gabi::ea(&bigelf_view<Mtx34>(matrices)[(u32)hand]),
                   gabi::ea(&bigelf_view<J3DModel>(flower)->mBaseTransformMtx));
        bigelf_updateModel(bytes((u32)gabi::ea(this->mpFlowerModel.get())), 0);
    }
    return 1;
}

/* 020953C4 */
BOOL daBigelf_c::_execute() {

    bigelf_dNpc_JntCtrl_setParam(&this->mJntCtrl, 0, 0, 0, 0, 4000, 9000, -2000, -4000, 0x1000);
    u16 flags = this->mStateBits;
    if (!(flags & FLAG_DISAPPEARING)) {
        u32 done =
            bigelf_playMorf(bytes((u32)gabi::ea(this->mpBckAnimator.get())), &this->eyePos, 0, 0);
        u32 morf = gabi::ea(this->mpBckAnimator.get());
        f32 previous = this->mPrevFrame;
        this->mAnmFinished = (s8)done;
        f32 frame = bigelf_view<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame;
        if (frame < previous && (s8)this->mAnmID != 3) {
            morf = gabi::ea(this->mpBckAnimator.get());
            this->mAnmFinished = 1;
        }
        frame = bigelf_view<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame;
        flags = this->mStateBits;
        this->mPrevFrame = frame;
    }
    // HD: state flags are cleared after the descriptor's first two words are loaded.
    s16 adjustment = this->mCurrentStateFunc.d;
    s16 dispatch = this->mCurrentStateFunc.i;
    this->mStateBits = flags & ~FLAG_FLOWER_VISIBLE;
    // The guest descriptor is stable across that store; retain the typed GHS virtual path.
    (this->*(this->mCurrentStateFunc))(nullptr);
    s16 yaw = this->current.angle.y;
    f32 z = this->current.pos.z, x = this->current.pos.x, y = this->current.pos.y;
    u32 morf = gabi::ea(this->mpBckAnimator.get());
    s8 room = this->current.roomNo;
    this->shape_angle.y = yaw;
    this->tevStr.mRoomNo = room;
    u32 model = gabi::ea(bigelf_view<mDoExt_McaMorf>(morf)->mpModel.get());
    bigelf_translateMatrix(&l_mtxStack, x, y, z);
    bigelf_rotateY(&l_mtxStack, (s32)(s16)this->current.angle.y);
    f32 sy = this->scale.y, sx = this->scale.x, sz = this->scale.z;
    bigelf_view<J3DModel>(model)->mBaseScale.x = sx;
    bigelf_view<J3DModel>(model)->mBaseScale.y = sy;
    bigelf_view<J3DModel>(model)->mBaseScale.z = sz;
    flags = this->mStateBits;
    f32 zero = l_zero;
    if (flags & FLAG_DISAPPEARING) {
        bigelf_translate(zero, (f32)this->mHeightOffset, zero);
        f32 scale = this->mDisappearScale, height = this->mDisappearTarget;
        bigelf_scale(scale, height, scale);
        height = this->mHeightOffset;
        bigelf_translate(zero, -(f64)height, zero);
    }
    copyMatrix(gabi::ea(&l_mtxStack), gabi::ea(&bigelf_view<J3DModel>(model)->mBaseTransformMtx));
    bigelf_calculateMorf(bytes((u32)gabi::ea(this->mpBckAnimator.get())));
    bigelf_mDoExt_baseAnm_play(&this->mBtkAnimator);
    f32 radius = this->mLightRadius, one = l_one, rate = l_lightRate, maximum = l_lightStep;
    bigelf_cLib_addCalc(&this->mLightInfluence.mPower, radius, rate, maximum, one);
    radius = this->mLightRadius;
    if (radius > zero) {
        f32 power = this->mLightInfluence.mPower;
        f32 fraction = (f32)((f64)power / (f64)radius), color = l_lightColor;
        f32 r = (f32)((f64)color * (f64)fraction);
        f32 secondRadius = this->mLightRadius;
        fraction = (f32)((f64)power / (f64)secondRadius);
        f32 gb = (f32)((f64)color * (f64)fraction);
        this->mLightInfluence.mColorR = (u8)gabi::ftoi(r);
        this->mLightInfluence.mColorG = (u8)gabi::ftoi(gb);
        this->mLightInfluence.mColorB = (u8)gabi::ftoi(gb);
    }
    if (this->getType() != 6) {
        bigelf_customColorSet(4, 0, (f32)this->m3AC);
    }
    f32 speed = this->m3A8, minimum = l_blendMinimum, step = l_blendStep, factor = l_animationRate;
    bigelf_cLib_addCalc(&this->m3AC, speed, factor, minimum, step);
    return 1;
}

/* 02095374 */
BOOL daBigelf_c::_delete() {

    bigelf_deleteResource(&this->mPhaseProcReq, "bigelf");
    u32 animator = gabi::ea(this->mpBckAnimator.get());
    if (animator) {
        bigelf_stopAnimation(bytes(animator));
    }
    return 1;
}

/* 02094E4C */
static BOOL CheckCreateHeap(daBigelf_c* actor) { return actor->CreateHeap(); }

/* 0209524C */
s32 daBigelf_c::_create() {

    u32 condition = this->actor_condition;
    if (!(condition & 8)) {
        if (this) {
            bigelf_actor__ct(this);
            condition = this->actor_condition;
        }
        this->actor_condition = condition | 8;
    }
    s32 phase = bigelf_loadResource(&this->mPhaseProcReq, "bigelf");
    if (phase == cPhs_COMPLEATE_e) {
        if (this->mProcName != fpcNm_BIGELF_e) {
            return cPhs_ERROR_e;
        }
        this->mInitialized = 0;
        if (!bigelf_entrySolidHeap(this, CheckCreateHeap, 0xB7B0)) {
            this->mpBckAnimator = nullptr;
            return cPhs_ERROR_e;
        }
        u32 model = gabi::ea(
            bigelf_view<mDoExt_McaMorf>((u32)gabi::ea(this->mpBckAnimator.get()))->mpModel.get());
        this->cullMtx = model ? gabi::ea(&bigelf_view<J3DModel>(model)->mBaseTransformMtx) : 0;
        if (!this->init()) {
            this->mpBckAnimator = nullptr;
            return cPhs_ERROR_e;
        }
    }
    return phase;
}

/* 020948EC */
BOOL daBigelf_c::CreateHeap() {

    gabi::Local<SafeString> name[6];
    u32 data = resource(name[0], dRes_INDEX_BIGELF_BDL_BIGELF_e);
    if (!data) {
        assertion(0x7F9, "modelData");
    }
    u32 animation = resource(name[1], dRes_INDEX_BIGELF_BCK_IDLE_e);
    f32 one = l_one;
    u32 morf = bigelf_mDoExt_McaMorf_construct(0, bytes(data), 0, 0, bytes(animation), 2, 0, -1, 1,
                                               0, 0x80000u, 0x11000222u, one);
    this->mpBckAnimator = bigelf_view<mDoExt_McaMorf>(morf);
    if (!morf || !gabi::ea(bigelf_view<mDoExt_McaMorf>(morf)->mpModel.get())) {
        return 0;
    }
    animation = resource(name[2], dRes_INDEX_BIGELF_BRK_BIGELF_e);
    if (!bigelf_mDoExt_brkAnm_init(&this->mBrkAnimator, bytes(data), bytes(animation), 1, 0, 0, -1,
                                   0, 0, one)) {
        return 0;
    }
    animation = resource(name[3], dRes_INDEX_BIGELF_BTK_BIGELF_e);
    if (!bigelf_mDoExt_btkAnm_init(&this->mBtkAnimator, bytes(data), bytes(animation), 1, 2, 0, -1,
                                   0, 0, one)) {
        return 0;
    }
    f32 z = this->current.pos.z;
    morf = gabi::ea(this->mpBckAnimator.get());
    f32 x = this->current.pos.x, y = this->current.pos.y;
    u32 model = gabi::ea(bigelf_view<mDoExt_McaMorf>(morf)->mpModel.get());
    bigelf_translateMatrix(&l_mtxStack, x, y, z);
    bigelf_rotateY(&l_mtxStack, (s32)(s16)this->current.angle.y);
    copyMatrix(gabi::ea(&l_mtxStack), gabi::ea(&bigelf_view<J3DModel>(model)->mBaseTransformMtx));
    bigelf_calculateMorf(bytes((u32)gabi::ea(this->mpBckAnimator.get())));
    s8 head = namedJoint(data, "head");
    this->mJntCtrl.mHeadJntNum = head;
    if (head < 0) {
        assertion(0x84F, "m_jnt.getHeadJntNum() >= 0");
    }
    s8 backbone = namedJoint(data, "backbone");
    head = this->mJntCtrl.mHeadJntNum;
    this->mJntCtrl.mBackboneJntNum = backbone;
    if (head < 0) {
        assertion(0x858, "m_jnt.getHeadJntNum() >= 0");
        backbone = this->mJntCtrl.mBackboneJntNum;
    }
    if (backbone < 0) {
        assertion(0x859, "m_jnt.getBackboneJntNum() >= 0");
    }
    s8 hand = namedJoint(data, "handRB");
    this->mHandRBJointIndex = hand;
    if (hand < 0) {
        assertion(0x861, "m_fl_jnt >= 0");
    }
    u16 index = 0;
    u32 joints = bigelf_getJointTree(bytes(data));
    while (index < bigelf_view<BigelfJointTree>(joints)->mJointNum) {
        if ((u32)index == (u32)(s32)(s8)this->mJntCtrl.mHeadJntNum ||
            (u32)index == (u32)(s32)(s8)this->mJntCtrl.mBackboneJntNum ||
            (u32)index == (u32)(s32)(s8)this->mHandRBJointIndex) {
            u32 model =
                gabi::ea(bigelf_view<mDoExt_McaMorf>((u32)gabi::ea(this->mpBckAnimator.get()))
                             ->mpModel.get());
            u32 modelData = gabi::ea(bigelf_view<J3DModel>(model)->mModelData.get()),
                count = bigelf_view<J3DModelData>(modelData)->mJointNum;
            u32 array = bigelf_view<J3DModelData>(modelData)->mJointNodes;
            if (index < count) {
                array = gabi::ea(&bigelf_view<BigelfJointNodeView>(array)[index]);
            }
            bigelf_view<BigelfJointNodeView>(array)->setCallBack(
                reinterpret_cast<J3DJointCallBack>(nodeCallBack_Bigelf));
        }
        index = (u16)(index + 1);
        joints = bigelf_getJointTree(bytes(data));
    }
    model = gabi::ea(
        bigelf_view<mDoExt_McaMorf>((u32)gabi::ea(this->mpBckAnimator.get()))->mpModel.get());
    bigelf_view<J3DModel>(model)->mUserArea = gabi::ea(this);
    u32 flowerData = resource(name[4], dRes_INDEX_BIGELF_BDL_FLOWER_e);
    if (!flowerData) {
        assertion(0x876, "flModelData");
    }
    this->mpFlowerModel =
        bigelf_view<J3DModel>(bigelf_createModel(bytes(flowerData), 0x80000u, 0x01000000u));
    if (!gabi::ea(this->mpFlowerModel.get())) {
        return 0;
    }
    animation = resource(name[5], dRes_INDEX_BIGELF_BRK_FLOWER_e);
    if (!bigelf_mDoExt_brkAnm_init(&this->mFlowerBrkAnimator, bytes(flowerData), bytes(animation),
                                   1, 0, 0, -1, 0, 0, one)) {
        return 0;
    }
    this->mColorType = 0;
    u32 type = this->getType();
    if (type >= 2 && type <= 6) {
        f32 frame;
        if (type <= 3) {
            frame = one;
        } else if (type < 6) {
            frame = l_greenColor;
        } else {
            frame = l_blueColor;
        }
        if (type <= 3) {
            this->mBrkAnimator.mFrameCtrl.mFrame = frame;
            this->mFlowerBrkAnimator.mFrameCtrl.mFrame = frame;
            this->mColorType = 1;
        } else {
            this->mColorType = type < 6 ? 2 : 3;
            this->mBrkAnimator.mFrameCtrl.mFrame = frame;
            this->mFlowerBrkAnimator.mFrameCtrl.mFrame = frame;
        }
    }
    f32 zero = l_zero;
    this->m3AC = one;
    this->mLightRadius = zero;
    this->m3A8 = one;
    return 1;
}

/* 02095370 */
static s32 daBigelf_Create(daBigelf_c* actor) { return actor->_create(); }

/* 020953C0 */
static BOOL daBigelf_Delete(daBigelf_c* actor) { return actor->_delete(); }

/* 02095734 */
static BOOL daBigelf_Execute(daBigelf_c* actor) { return actor->_execute(); }

/* 020958D4 */
static BOOL daBigelf_Draw(daBigelf_c* actor) { return actor->_draw(); }

/* 020958D8 */
static BOOL daBigelf_IsDelete(daBigelf_c* actor) { return 1; }

// HD: compiler-generated and template code of this translation unit.

/* 02094E50 */
static daBigelf_c* bigelf_actor__ct(daBigelf_c* actor) {

    if (!actor) {
        actor = bigelf_allocateActor(0x644);
        if (!actor) {
            return nullptr;
        }
    }
    bigelf_constructActor(actor);
    actor->__vtbl = l_actorVtable;
    u32 address = gabi::ea(actor);
    mDoExt_brkAnm::ct(&actor->mBrkAnimator);
    mDoExt_btkAnm::ct(&actor->mBtkAnimator);
    mDoExt_brkAnm::ct(&actor->mFlowerBrkAnimator);
    bigelf_constructJointController(&actor->mJntCtrl);
    f32 value = l_one;
    actor->mLightInfluenceTargetIntensity = value;
    actor->mLightInfluence.mHD20 = value;
    return actor;
}

/* 02097FD8 */
static void bigelf_SafeString__dt(void* object, u32 flags) {

    if (object && (flags & 1)) {
        bigelf_deleteObject(object);
    }
}

/* 02097FEC */
static void bigelf_actor__dt(daBigelf_c* actor, u32 flags) {

    if (actor) {
        bigelf_destroyActor(actor, 0);
        if (flags & 1) {
            bigelf_deleteObject(actor);
        }
    }
}

/* 02098040 */
static void bigelf_SafeString_assureTerminationImpl_() {}

/* 02097F44 */
static void __sinit_d_a_bigelf_cpp() {

    l_headerStatics.mZero[2] = 0;
    l_headerStatics.mZero[0] = 0;
    l_headerStatics.mZero[3] = 0;
    l_headerStatics.mZero[1] = 0;
    bigelf_registerGlobal(&l_primaryDestructor);
    l_headerStatics.mMinimumAngle = l_minimumAngle;
    l_headerStatics.mMaximumAngle = l_maximumAngle;
    bigelf_constructStaticColor(&l_headerStatics.mColorConstructor);
    bigelf_registerGlobal(&l_colorDestructor);
    bigelf_constructStaticVector(&l_headerStatics.mVectorConstructor);
    bigelf_registerGlobal(&l_vectorDestructor);
}

// clang-format off
/* WWHD glue
data l_mtxStack 1048D0CC
data l_jointMatrixCopy 104B4868
data l_actionNames 101912FC
str 10009390 "Daiocta"
str 10009380 "BIGELF_ARRIVAL2"
str 10009370 "BigElf"
str 10009310 "d_a_bigelf.cpp" in 02096328
str 10009308 "a_intP"
str 10009300 "MsgNo"
str 100092EC "Timer"
str 100092E4 "Ship"
str 1000926C "BIGELF_ARRIVAL"
str 10009260 "bigelf" in 020958F8
str 10009238 "bigelf" in 02095374
str 10009230 "bigelf" in 0209524C
str 100091F4 "m_jnt.getBackboneJntNum() >= 0"
str 100091E8 "backbone"
str 100091CC "m_jnt.getHeadJntNum() >= 0"
str 100091BC "d_a_bigelf.cpp"
str 100091AC "m_fl_jnt >= 0"
str 100091A0 "modelData"
str 10009194 "flModelData"
str 1000918C "handRB"
str 10009184 "head"
str 1000917C "bigelf"
data l_headerStatics 104622E4
data l_vectorDestructor 10191350
data l_colorDestructor 10191344
data l_primaryDestructor 10191338
data l_zero 10009164
data l_handForward 10009168
data l_handEye 1000916C
data l_one 10009170
data l_greenColor 10009174
data l_blueColor 10009178
data l_fairyHeight 10009224
data l_spawnHeight 10009228
data l_attentionHeight 1000922C
data l_lightRate 10009240
data l_lightStep 10009244
data l_lightColor 10009248
data l_animationRate 1000924C
data l_blendMinimum 10009250
data l_blendStep 10009254
data l_morphFrames 10009258
data l_animationEnd 1000925C
data l_huntDistance 10009268
data l_shipStartDistance 100092E0
data l_appearHeight 100092F4
data l_exitHeight 10009320
data l_fogEnd 10009324
data l_linkLightRadius 10009328
data l_linkForward 1000932C
data l_fairyMoveRate 10009330
data l_appearSoundFrame 10009334
data l_exitFogTarget 10009338
data l_disappearStretch 1000933C
data l_flowerBeginFrame 10009340
data l_flowerEndFrame 10009344
data l_flowerCreateFrame 10009348
data l_dimBeginFrame 1000934C
data l_lightForward 10009350
data l_dimSoundFrame 10009354
data l_chimeSoundFrame 10009358
data l_dimEndFrame 1000935C
data l_actorLightScale 10009360
data l_actorLightBase 10009364
data l_backgroundLightScale 10009368
data l_backgroundLightBase 1000936C
data l_octOffsetX 10009378
data l_octOffsetZ 1000937C
data l_shipExitDistance 10009398
data l_minimumAngle 100093A0
data l_maximumAngle 100093A4
data l_bckIndex 10009154
data l_eventFlags 10009214
data l_changeEffects 10009118
data l_releaseEffects 10009120
data l_appearEffects 10191328
data l_flashEffects 10191330
data l_saveInfo 101F84DC
data l_resourceManager 101F4F28
data l_messageManager 101F4B5C
data l_messageHandle 104622E0
data l_currentModel 104B462C
data l_currentMatrix 1018C7B0
ext 0200ED84 void bigelf_cLib_addCalc2(void* value,f32 target,f32 scale,f32 maximum)
ext 025D6894 s16 bigelf_fopAcM_searchActorAngleY(fopAc_ac_c* actor,void* other)
ext 025D6958 f32 bigelf_fopAcM_searchActorDistanceXZ(fopAc_ac_c* actor,void* other)
ext 0200EDC8 void bigelf_cLib_addCalc0(be<f32>* value,f32 scale,f32 maximum)
ext 0200F378 void bigelf_cLib_addCalcAngleS(be<s16>* value,s32 target,s32 scale,s32 maximum,s32 minimum)
ext 025E4F64 u32 bigelf_mDoExt_McaMorf_construct(u32 self,void* model,u32 callback1,u32 callback2,void* animation,s32 mode,s32 start,s32 end,s32 modify,u32 sound,u32 modelFlags,u32 differenceFlags,f32 speed)
ext 025E8154 BOOL bigelf_mDoExt_brkAnm_init(mDoExt_brkAnm* animator,void* model,void* animation,s32 play,s32 mode,s32 start,s32 end,s32 modify,s32 entry,f32 speed)
ext 025E7CE0 BOOL bigelf_mDoExt_btkAnm_init(mDoExt_btkAnm* animator,void* model,void* animation,s32 play,s32 mode,s32 start,s32 end,s32 modify,s32 entry,f32 speed)
ext 0259E08C void bigelf_dNpc_JntCtrl_setParam(dNpc_JntCtrl_c* controller,u32 a,u32 b,u32 c,u32 d,s32 headX,s32 headY,s32 backboneX,s32 backboneY,s32 step)
ext 025E742C void bigelf_mDoExt_baseAnm_play(mDoExt_baseAnm* animator)
ext 0200ECD4 void bigelf_cLib_addCalc(be<f32>* value,f32 target,f32 scale,f32 maximum,f32 minimum)
ext 025E83FC void bigelf_mDoExt_brkAnm_entry(mDoExt_brkAnm* animator,void* model,f32 frame)
ext 025E7FC4 void bigelf_mDoExt_btkAnm_entry(mDoExt_btkAnm* animator,void* model,f32 frame)
ext 025D5834 u32 bigelf_createActor(s32 name,u32 parameters,cXyz* position,s32 room,csXyz* angle,u32 scale,s32 parent,u32 append)
ext 0273AD10 daBigelf_c* bigelf_allocateActor(u32 size)
ext 025D4ED0 void bigelf_constructActor(fopAc_ac_c* actor)
ext 0273AF40 void bigelf_deleteObject(void* object)
ext 025D50BC void bigelf_destroyActor(fopAc_ac_c* actor,u32 flags)
ext 025200D4 u32 bigelf_getGameInfo()
ext 02542EDC s32 bigelf_getActionIndex(void* manager,s32 staff,void* names,s32 count,s32 mode,s32 flag)
ext 0254487C u32 bigelf_getSubstance(void* manager,s32 staff,const char* name,s32 type)
ext 02543280 void bigelf_endCut(void* manager,s32 staff)
ext 025D5218 u32 bigelf_judgeActor(void* judge,be<u32>* argument)
ext 02234CB8 void bigelf_changeFairy(void* fairy)
ext 02520540 s32 bigelf_getReverb(s32 room)
ext 025E1A40 void bigelf_startSound(u32 sound,cXyz* position,u32 argument,s32 reverb)
ext 02543F10 u32 bigelf_getEventIndex(void* manager,const char* name,s32 mapToolId)
ext 025D7A58 void bigelf_orderOtherEvent(fopAc_ac_c* actor,s32 event,u32 mapToolId,u32 flags,u32 priority,u32 mode)
ext 025204C8 void bigelf_deleteResource(request_of_phase_process_class* phase,const char* archive)
ext 025E563C void bigelf_stopAnimation(void* animator)
ext 025D9F38 u32 bigelf_searchActorName(const char* name,u32 argument,u32 flags)
ext 026066C4 u32 bigelf_getResource(void* manager,void* name,s32 index)
ext 025E4A98 void bigelf_setMorfAnimation(void* animator,void* animation,s32 mode,u32 argument,f32 morph,f32 speed,f32 start,f32 end)
ext 025F7DB0 u32 bigelf_setMessage(void* manager,u32 message,cXyz* position)
ext 025F795C u32 bigelf_getMessageStatus(void* manager)
ext 025F74D0 void bigelf_setMessageStatus(void* manager,u32 status)
ext 025A847C u32 bigelf_setParticle(void* manager,u32 group,u16 effect,cXyz* position,csXyz* angle,cXyz* scale,u32 alpha,u32 callback,s32 room,u32 primary,u32 environment,u32 extra)
ext 025DA124 void bigelf_absolutePosition(void* actor,cXyz* offset,cXyz* position)
ext 0200F93C u32 bigelf_targetAngleY(cXyz* source,cXyz* target)
ext 024832E0 void bigelf_startShipPosition(void* ship,cXyz* position,s32 angle)
ext 0253F124 u32 bigelf_getEventProcessId(void* control,fopAc_ac_c* actor)
ext 025D57E0 void bigelf_deleteActor(void* actor)
ext 025CB374 void bigelf_startShock(void* vibration,u32 type,s32 duration,cXyz* direction)
ext 025E18EC void bigelf_startBgm(u32 sound)
ext 0273AA24 void bigelf_assertion(const char* file,u32 line,const char* condition)
ext 025E1988 void bigelf_startSystemSound(u32 sound)
ext 025E1904 void bigelf_stopBgm(u32 duration)
ext 025447C8 BOOL bigelf_isAdvance(void* manager,s32 staff)
ext 02542D88 u32 bigelf_getStaffId(void* manager,const char* name,u32 actor,u32 argument)
ext 025440C8 BOOL bigelf_eventEndCheck(void* manager,s32 event)
ext 025B8B68 void bigelf_onEventBit(void* events,u32 bit)
ext 025B9E38 void bigelf_onSwitch(void* memory,u32 bit,s32 room)
ext 025BA0C0 BOOL bigelf_isSwitch(void* memory,u32 bit,s32 room)
ext 025D7970 void bigelf_orderChangeEvent(fopAc_ac_c* actor,void* player,u32 event,u32 priority,u32 flags)
ext 025B8B94 u32 bigelf_isEventBit(void* events,u32 bit)
ext 028F026C void bigelf_registerGlobal(void* descriptor)
ext 028ED6F8 void bigelf_constructStaticColor(void* color)
ext 028EAB2C void bigelf_constructStaticVector(void* vector)
ext 028E90D4 void bigelf_copyMatrix(void* source,void* destination)
ext 027F7878 u32 bigelf_getJoint(void* node)
ext 025F1C5C void bigelf_rotateZ(void* matrix,s32 angle)
ext 0200FCD8 void bigelf_matrixPosition(cXyz* source,cXyz* destination)
ext 027F68FC u32 bigelf_getJointNames(void* data)
ext 027DF9B0 u32 bigelf_getJointIndex(void* names,const char* name)
ext 028E93CC void bigelf_translateMatrix(void* matrix,f32 x,f32 y,f32 z)
ext 025F1C28 void bigelf_rotateY(void* matrix,s32 angle)
ext 025E55A0 void bigelf_calculateMorf(void* animator)
ext 027F3F94 u32 bigelf_getJointTree(void* data)
ext 025E38E0 u32 bigelf_createModel(void* data,u32 modelFlags,u32 differenceFlags)
ext 02520460 s32 bigelf_loadResource(request_of_phase_process_class* phase,const char* archive)
ext 025D63E8 BOOL bigelf_entrySolidHeap(fopAc_ac_c* actor,BigelfHeapCallback callback,u32 size)
ext 025E535C u32 bigelf_playMorf(void* animator,cXyz* position,u32 sound,u32 reverb)
ext 025F24E0 void bigelf_translate(f32 x,f64 y,f32 z)
ext 025F2518 void bigelf_scale(f32 x,f32 y,f32 z)
ext 0255FDA4 void bigelf_customColorSet(s32 source,s32 destination,f32 blend)
ext 02555D0C u32 bigelf_getEnvironment()
ext 025626A4 void bigelf_settingTev(void* environment,u32 light,cXyz* position,dKy_tevstr_c* tev)
ext 02562F5C void bigelf_setLightTev(void* environment,void* model,dKy_tevstr_c* tev)
ext 025E5580 void bigelf_updateMorf(void* animator)
ext 025E2DE0 void bigelf_updateModel(void* model,u32 flags)
ext 0255A3D8 void bigelf_setActColorRatio(f32 value)
ext 0255A418 void bigelf_setBackgroundColorRatio(f32 value)
ext 0255A51C void bigelf_setVrboxColorRatio(f32 value)
ext 0255A2B8 void bigelf_setPriorityLight(LIGHT_INFLUENCE *light)
ext 0259DAA0 void bigelf_constructJointController(dNpc_JntCtrl_c *controller)
*/
// clang-format on
