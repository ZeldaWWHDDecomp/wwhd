/** Moblin statue (Mozo), WWHD. Private verified source.
 * HD: expanded animation, actor and collision layouts retain their guest offsets.
 */
#include "d/actor/d_a_mozo.h"
#include "bindings.h"
using MozoByte = be<u8>;
static MozoByte l_headerAngle;
static MozoByte l_headerCircle;
static MozoWords3 l_headerDtor0;
static MozoWords3 l_headerDtor1;
static MozoWords3 l_headerDtor2;
static MozoCollisionSource cps_src;
static cXyz l_fireAim;
using MozoWord = be<u32>;
using MozoFloat = be<f32>;
static MozoWord l_resourceControl;
static MozoWord l_currentModel;
static MozoByte l_eventState;
static daMozo_HIO_c l_HIO;
static MozoMtx l_calcMtx;
static MozoMtx l_j3dMtx;
static MozoWords4 l_identityQuat;
static MozoAngles l_zeroAngles;
static cXyz l_defaultDirection;
static MozoSincosTable l_sincos;
static MozoFloat l_piLowConstant;
static MozoFloat l_piHighConstant;
static MozoFloat l_piLow;
static MozoFloat l_piHigh;
static MozoWord l_beamOriginGuard;
static MozoWord l_beamTargetGuard;
static MozoWord l_leftOriginGuard;
static MozoWord l_rightOriginGuard;
static MozoWord l_fireOriginGuard;
static MozoWord l_fireTargetGuard;
static MozoWord l_headerWord0;
static MozoWord l_headerWord1;
static MozoWord l_headerWord2;
static MozoWord l_headerWord3;
static cXyz l_beamOrigin;
static cXyz l_beamTarget;
static cXyz l_leftOrigin;
static cXyz l_rightOrigin;
static cXyz l_fireOrigin;
static cXyz l_fireTarget;

static void *mozoPtr(u32 address) { return gabi::at<u8>(address); }
static u32 mozoResource(s32 index, const char *archive) {
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::guest_str(archive);
    name->__vtbl = mozo_tu::safeString;
    gabi::Local<u8[16]> linkage;
    return mozoGetResource(mozoPtr((u32)(l_resourceControl)), name.get(), index);
}

static u32 mozoParticle(u32 position, u32 id, s32 layer) {
    u32 common = mozoGetPlay();
    u32 particles =
        (u32)((*reinterpret_cast<be<u32> *>(&gabi::at<MozoPlay>(common)->mpParticleControl)));
    return mozoParticleSet(mozoPtr(particles), layer, id, mozoPtr(position), 0, 0, 0xFF, 0, -1, 0,
                           0, 0);
}

static void mozoStopEmitter(u32 emitter) {
    gabi::at<MozoEmitter>(emitter)->mFrameCount = (s32)(-1);
    gabi::at<MozoEmitter>(emitter)->mStatus =
        (u32)((u32)(gabi::at<MozoEmitter>(emitter)->mStatus) | 1);
}

/* 021D6070 */
static void *daMozo_HIO_c__ct(void *self) {
    if (!self) {
        self = mozoAllocate(0x48);
        if (!self)
            return nullptr;
    }
    u32 base = gabi::ea(self);
    daMozo_HIO_c *hio = gabi::at<daMozo_HIO_c>(base);
    hio->mNo = (s8)(-1);
    hio->m08 = (s32)(-1);
    hio->mpBeamChild = (u32)(base + 0x10);
    hio->mpFireChild = (u32)(base + 0x24);
    hio->mBeamChild.m04 = (f32)(1000.0f);
    hio->mBeamChild.m08 = (f32)(1200.0f);
    hio->mBeamChild.m0C = (s16)(0x2000);
    hio->mBeamChild.m0E = (s16)(0x2800);
    hio->mBeamChild.m10 = (u8)(0);
    hio->mBeamChild.__vtbl = (u32)(mozo_tu::beamHio);
    hio->mFireChild.m04 = (f32)(1000.0f);
    hio->mFireChild.m08 = (f32)(1200.0f);
    hio->mFireChild.m0C = (s16)(0x2000);
    hio->mFireChild.m0E = (s16)(0x2800);
    hio->mFireChild.m10 = (u8)(0);
    hio->mFireChild.__vtbl = (u32)(mozo_tu::fireHio);
    hio->m3C = (f32)(0.0f);
    hio->m40 = (f32)(-300.0f);
    hio->m44 = (f32)(600.0f);
    hio->__vtbl = (u32)(mozo_tu::hio);
    return self;
}

/* 021D3DF0 */
static BOOL daMozo_nodeCallBack(void *node, s32 timing) {
    gabi::Local<be<f32>[12]> rotation;
    gabi::Local<u8[16]> linkage;
    u32 model = (u32)(l_currentModel);
    u32 actor = (u32)(gabi::at<MozoModel>(model)->mUserArea);
    u8 type = (u8)(gabi::at<daMozo_c>(actor)->field_0x376);
    if (type > MOZO_FIRE)
        return 1;
    u32 joint = mozoGetJoint(node);
    u32 jointOffset = (u16)(gabi::at<MozoJoint>(joint)->mJntNo) * 48;
    if (timing != 0)
        return 1;
    u32 origin, target;
    if (type == MOZO_BEAM) {
        origin = gabi::ea(&l_beamOrigin);
        target = gabi::ea(&l_beamTarget);
        if (!(u32)(l_beamOriginGuard)) {
            l_beamOriginGuard = (u32)(1);
            gabi::at<cXyz>(origin)->x = (f32)(60.0f);
            gabi::at<cXyz>(origin)->z = (f32)(0.0f);
            gabi::at<cXyz>(origin)->y = (f32)(-20.0f);
        }
        if (!(u32)(l_beamTargetGuard)) {
            l_beamTargetGuard = (u32)(1);
            gabi::at<cXyz>(target)->x = (f32)(1250.0f);
            gabi::at<cXyz>(target)->y = (f32)(-250.0f);
            gabi::at<cXyz>(target)->z = (f32)(0.0f);
        }
        if (!(u32)(l_leftOriginGuard)) {
            l_leftOrigin.y = (f32)(-20.0f);
            l_leftOrigin.z = (f32)(12.5f);
            l_leftOriginGuard = (u32)(1);
            l_leftOrigin.x = (f32)(60.0f);
        }
        if (!(u32)(l_rightOriginGuard)) {
            l_rightOrigin.y = (f32)(-20.0f);
            l_rightOrigin.z = (f32)(-12.5f);
            l_rightOriginGuard = (u32)(1);
            l_rightOrigin.x = (f32)(60.0f);
        }
    } else {
        origin = gabi::ea(&l_fireOrigin);
        target = gabi::ea(&l_fireTarget);
        if (!(u32)(l_fireOriginGuard)) {
            gabi::at<cXyz>(origin)->x = (f32)(0.0f);
            gabi::at<cXyz>(origin)->z = (f32)(0.0f);
            l_fireOriginGuard = (u32)(1);
            gabi::at<cXyz>(origin)->y = (f32)(0.0f);
        }
        if (!(u32)(l_fireTargetGuard)) {
            gabi::at<cXyz>(target)->y = (f32)(0.0f);
            l_fireTargetGuard = (u32)(1);
            gabi::at<cXyz>(target)->x = (f32)(5000.0f);
            gabi::at<cXyz>(target)->z = (f32)(0.0f);
        }
    }
    u32 matrices = (u32)(gabi::at<MozoModel>(model)->mpMatrices);
    u32 data = (u32)(gabi::at<MozoMatrices>(matrices)->mpAnimationMatrices);
    gabi::at<MozoMatrices>(matrices)->mFlags =
        (u16)((u16)(gabi::at<MozoMatrices>(matrices)->mFlags) | 0x10);
    mozoMtxCopy(mozoPtr(data + jointOffset), &l_calcMtx);
    mozoMtxMultVec(&l_calcMtx, mozoPtr(origin), &gabi::at<daMozo_c>(actor)->unrotatedOrigin);
    mozoMtxMultVec(&l_calcMtx, mozoPtr(target), &gabi::at<daMozo_c>(actor)->unrotatedTarget);
    mozoMtxCopy(&l_calcMtx, rotation.get());
    f32 x = (f32)((*rotation)[3]), z = (f32)((*rotation)[11]);
    (*rotation)[11] = (f32)(0.0f);
    f32 y = (f32)((*rotation)[7]);
    (*rotation)[7] = (f32)(0.0f);
    (*rotation)[3] = (f32)(0.0f);
    mozoMtxTranslation(&l_calcMtx, x, y, z);
    mozoMtxQuatM(&gabi::at<daMozo_c>(actor)->mQuatRotation);
    mozoMtxConcat(&l_calcMtx, rotation.get(), &l_calcMtx);
    if (type == MOZO_BEAM) {
        mozoMtxMultVec(&l_calcMtx, mozoPtr(origin), &gabi::at<daMozo_c>(actor)->beamOrigin);
        mozoMtxMultVec(&l_calcMtx, mozoPtr(target), &gabi::at<daMozo_c>(actor)->beamTarget);
        mozoMtxMultVec(&l_calcMtx, &l_leftOrigin, &gabi::at<daMozo_c>(actor)->leftOrigin);
        mozoMtxMultVec(&l_calcMtx, &l_rightOrigin, &gabi::at<daMozo_c>(actor)->rightOrigin);
    } else {
        mozoMtxMultVec(&l_calcMtx, mozoPtr(origin), &gabi::at<daMozo_c>(actor)->fireOrigin);
        mozoMtxMultVec(&l_calcMtx, mozoPtr(target), &gabi::at<daMozo_c>(actor)->fireTarget);
    }
    matrices = (u32)(gabi::at<MozoModel>(model)->mpMatrices);
    u16 flags = (u16)(gabi::at<MozoMatrices>(matrices)->mFlags);
    if (type == MOZO_BEAM)
        data = (u32)(gabi::at<MozoMatrices>(matrices)->mpAnimationMatrices);
    gabi::at<MozoMatrices>(matrices)->mFlags = (u16)(flags | 0x10);
    // HD: preserve the GHS floating loads/stores, including the fire-path matrix-pointer reload.
    constexpr u32 beamLoads[12] = {6, 3, 8, 10, 11, 5, 1, 4, 7, 0, 9, 2};
    constexpr u32 beamStores[12] = {11, 9, 10, 1, 0, 7, 4, 8, 6, 3, 2, 5};
    constexpr u32 fireLoads[12] = {1, 7, 4, 11, 10, 2, 0, 9, 3, 6, 8, 5};
    constexpr u32 fireStores[12] = {8, 11, 9, 4, 2, 3, 0, 6, 10, 5, 1, 7};
    f32 values[12];
    const u32 *loads = type == MOZO_BEAM ? beamLoads : fireLoads;
    const u32 *stores = type == MOZO_BEAM ? beamStores : fireStores;
    for (s32 i = 0; i < 12; ++i) {
        values[loads[i]] = (f32)(l_calcMtx.mElements[loads[i]]);
        if (type == MOZO_FIRE && i == 1)
            data = (u32)(gabi::at<MozoMatrices>(matrices)->mpAnimationMatrices);
    }
    for (s32 i = 0; i < 12; ++i)
        gabi::at<MozoMtx>(data + jointOffset)->mElements[stores[i]] = (f32)(values[stores[i]]);
    mozoMtxCopy(&l_calcMtx, &l_j3dMtx);
    return 1;
}

/* 021D4494 */
static BOOL CheckCreateHeap(daMozo_c *self) { return self->CreateHeap(); }

/* 021D42B4 */
BOOL daMozo_c::CreateHeap() {
    u32 data = mozoResource(9, "Mozo");
    u32 animation = mozoResource(6, "Mozo");
    u32 morf = mozoMorfConstruct(0, mozoPtr(data), 0, 0, mozoPtr(animation), 2, 1.0f, 0, -1, 0, 0,
                                 0, 0x11020203);
    this->mAnimMorf = gabi::at<mDoExt_McaMorf>(morf);
    u32 color = mozoResource(0xC, "Mozo");
    this->m_brk = color;
    if (!color)
        mozoAssertion(STR(mozo_tu::assertFile), 0x16A, STR(mozo_tu::brkAssertion));
    u32 texture = mozoResource(0xF, "Mozo");
    this->m_btk = texture;
    if (!texture)
        mozoAssertion(STR(mozo_tu::assertFile), 0x16D, STR(mozo_tu::btkAssertion));
    BOOL colorOk = mozoBrkInit(&this->mBrkAnm, mozoPtr(data), mozoPtr((u32)this->m_brk), 1, 0, 1.0f,
                               0, -1, 0, 0);
    BOOL textureOk = mozoBtkInit(&this->mBtkAnm, mozoPtr(data), mozoPtr((u32)this->m_btk), 1, 0,
                                 1.0f, 0, -1, 0, 0);
    if (!data)
        return 0;
    morf = gabi::ea(this->mAnimMorf.get());
    if (!morf || !(u32)((*reinterpret_cast<be<u32> *>(&gabi::at<mDoExt_McaMorf>(morf)->mpModel))) ||
        !colorOk || !textureOk)
        return 0;
    return 1;
}

/* 021D4654 */
void daMozo_c::set_mtx() {
    u32 model = (u32)((*reinterpret_cast<be<u32> *>(&this->mAnimMorf->mpModel)));
    f32 z = this->scale.z, x = this->scale.x, y = this->scale.y;
    gabi::at<MozoModel>(model)->mBaseScale.x = (f32)(x);
    gabi::at<MozoModel>(model)->mBaseScale.y = (f32)(y);
    gabi::at<MozoModel>(model)->mBaseScale.z = (f32)(z);
    mozoMtxTranslation(&l_calcMtx, (f32)this->current.pos.x, (f32)this->current.pos.y,
                       (f32)this->current.pos.z);
    mozoMtxYrotM(&l_calcMtx, (s16)this->current.angle.y);
    // HD: the base transform uses this verified load/store order.
    constexpr u32 loads[12] = {8, 1, 0, 3, 6, 7, 4, 2, 11, 9, 5, 10};
    constexpr u32 stores[12] = {5, 11, 0, 4, 1, 2, 3, 6, 7, 9, 8, 10};
    f32 values[12];
    for (s32 i = 0; i < 12; ++i)
        values[loads[i]] = (f32)(l_calcMtx.mElements[loads[i]]);
    for (s32 i = 0; i < 12; ++i)
        gabi::at<MozoModel>(model)->mBaseTRMtx[stores[i]] = (f32)(values[stores[i]]);
}

/* 021D4F14 */
void daMozo_c::anime_proc() {
    mozoMorfPlay(this->mAnimMorf.get(), 0, 0, 0);
    mozoAnimationPlay(&this->mBrkAnm);
    mozoAnimationPlay(&this->mBtkAnm);
    u32 morf = gabi::ea(this->mAnimMorf.get());
    if ((f32)(gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame) < 24.0f) {
        s32 reverb = mozoGetReverb((s8)this->current.roomNo);
        mozoSoundStart(0x6179, mozoPtr(gabi::ea(this) + 0x37C), 0, reverb);
        morf = gabi::ea(this->mAnimMorf.get());
    }
    if (mozoFrameCheck(&gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl, 35.0f)) {
        u32 soundPosition = gabi::ea(this) + 0x37C;
        if ((f32)(this->mAnimMorf->mFrameCtrl.mRate) > 0.0f) {
            s32 reverb = mozoGetReverb((s8)this->current.roomNo);
            mozoSoundStart(0x697A, mozoPtr(soundPosition), 0, reverb);
        } else {
            mozoSoundStop(mozoPtr(soundPosition), 0x697A);
            s32 reverb = mozoGetReverb((s8)this->current.roomNo);
            mozoSoundStart(0x697B, mozoPtr(soundPosition), 0, reverb);
        }
    }
}

/* 021D4AC0 */
void daMozo_c::wait_proc_init() {
    setAnm(MOZO_ANM_WAIT, 0.0f);
    this->mCurrentProc.i = -1;
    this->mCurrentProc.f = MOZO_WAIT;
    this->mCurrentProc.d = 0;
}

/* 021D53D8 */
void daMozo_c::wait_proc() {
    u32 common = mozoGetPlay();
    u32 player = (u32)((*reinterpret_cast<be<u32> *>(&gabi::at<MozoPlay>(common)->mpPlayer)));
    if (!checkRange(0))
        return;
    u32 table = (u32)(gabi::at<MozoPlayer>(player)->__vtbl);
    u32 target = (u32)(gabi::at<MozoPlayerVtable>(table)->mpCheckStatus);
    if (gabi::call_ptr<BOOL>(target, mozoPtr(player)))
        return;
    u8 type = this->field_0x376;
    if (type == MOZO_BEAM || type == MOZO_BEAM_ALT)
        search_beam_proc_init();
    else if (type == MOZO_FIRE)
        search_fire_proc_init();
    if ((u8)(l_eventState) == 0)
        this->requestEvent = 1;
}

/* 021D52C4 */
void daMozo_c::search_beam_proc_init() {
    setAnm(MOZO_ANM_SEARCH, 0.0f);
    this->mBtkAnm.mFrameCtrl.mFrame = 0.0f;
    this->mCurrentProc.i = -1;
    this->mCurrentProc.d = 0;
    this->mBtkAnm.mFrameCtrl.mRate = (f32)(1.0f);
    this->mBrkAnm.frame.mRate = (f32)(1.0f);
    this->mBrkAnm.frame.mFrame = (f32)(0.0f);
    this->mCurrentProc.f = MOZO_SEARCH_BEAM;
}

/* 021D551C */
void daMozo_c::search_beam_proc() {
    gabi::Local<cXyz> towardPlayer, forward, beam, horizontal;
    gabi::Local<be<f32>[4]> rotation;
    gabi::Local<be<s16>[3]> angles;
    gabi::Local<u8[16]> linkage;
    u32 common = mozoGetPlay();
    f32 frame = (f32)(this->mAnimMorf->mFrameCtrl.mFrame);
    u32 player = (u32)((*reinterpret_cast<be<u32> *>(&gabi::at<MozoPlay>(common)->mpPlayer)));
    if (frame > 30.0f) {
        mozoVectorDifference(&gabi::at<MozoPlayer>(player)->current.pos, towardPlayer.get(),
                             &this->unrotatedOrigin);
        mozoVectorDifference(&this->unrotatedTarget, forward.get(), &this->unrotatedOrigin);
        mozoQuatFromVectors(rotation.get(), forward.get(), towardPlayer.get());
        mozoQuatSlerp(&this->mQuatRotation, rotation.get(), &this->mQuatRotation, 0.2f);
    }
    anime_proc();
    u32 morf = gabi::ea(this->mAnimMorf.get());
    f32 end = (f32)(s16)(gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mEnd);
    f32 finalFrame = gabi::fsubs_ppc(end, 1.0f);
    if ((f32)(gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame) > finalFrame) {
        u32 left = getBeamActor(this->leftChild);
        u32 right = getBeamActor(this->rightChild);
        if (left && right) {
            mozoVectorDifference(&this->beamTarget, beam.get(), &this->beamOrigin);
            for (s32 i = 0; i < 3; ++i)
                reinterpret_cast<be<u16> *>(angles.get())[i] =
                    (u16)((u16)(l_zeroAngles.mAngles[i]));
            s32 yaw = mozoAtan2((f32)beam->x, (f32)beam->z);
            horizontal->x = beam->x;
            horizontal->z = beam->z;
            horizontal->y = 0.0f;
            f32 square = mozoVectorSquare(horizontal.get());
            f32 length = mozoSqrt(square);
            f32 down = -(f32)beam->y;
            s32 pitch = mozoAtan2(down, length);
            if (!(u32)(gabi::at<MozoBeam>(left)->mpEmitter))
                gabi::at<MozoBeam>(left)->mpEmitter = (u32)(mozoParticle(left + 0x314, 0x8121, 0));
            s16 active = (s16)(gabi::at<MozoBeam>(left)->mBeamState);
            f32 maximum = 5.0f;
            gabi::at<MozoBeam>(left)->mBeamTimer = (f32)(0.0f);
            if (active == 0) {
                f32 scale = (f32)(gabi::at<MozoBeam>(left)->mBeamScale);
                if (scale < maximum)
                    gabi::at<MozoBeam>(left)->mBeamScale = (f32)(gabi::fadds_ppc(scale, 1.0f));
                else {
                    gabi::at<MozoBeam>(left)->mBeamTimer = (f32)(0.0f);
                    gabi::at<MozoBeam>(left)->mBeamState = (s16)(1);
                    gabi::at<MozoBeam>(left)->mBeamScale = (f32)(maximum);
                }
            } else
                gabi::at<MozoBeam>(left)->mBeamScale = (f32)(maximum);
            u32 x = (u32)((*reinterpret_cast<be<u32> *>(&this->leftOrigin.x)));
            (*reinterpret_cast<be<u32> *>(&gabi::at<MozoBeam>(left)->current.pos.x)) = (u32)(x);
            u32 y = (u32)((*reinterpret_cast<be<u32> *>(&this->leftOrigin.y)));
            (*reinterpret_cast<be<u32> *>(&gabi::at<MozoBeam>(left)->current.pos.y)) = (u32)(y);
            u32 z = (u32)((*reinterpret_cast<be<u32> *>(&this->leftOrigin.z)));
            (*reinterpret_cast<be<s16> *>(&gabi::at<MozoBeam>(left)->current.angle.x)) =
                (s16)(pitch);
            (*reinterpret_cast<be<u32> *>(&gabi::at<MozoBeam>(left)->current.pos.z)) = (u32)(z);
            (*reinterpret_cast<be<s16> *>(&gabi::at<MozoBeam>(left)->current.angle.y)) = (s16)(yaw);
            s16 roll = (s16)((*angles)[2]);
            gabi::at<MozoBeam>(left)->mThickness = (f32)(15.0f);
            (*reinterpret_cast<be<s16> *>(&gabi::at<MozoBeam>(left)->current.angle.z)) =
                (s16)(roll);
            if (!(u32)(gabi::at<MozoBeam>(right)->mpEmitter))
                gabi::at<MozoBeam>(right)->mpEmitter =
                    (u32)(mozoParticle(right + 0x314, 0x8121, 0));
            active = (s16)(gabi::at<MozoBeam>(right)->mBeamState);
            gabi::at<MozoBeam>(right)->mBeamTimer = (f32)(0.0f);
            if (active == 0) {
                f32 scale = (f32)(gabi::at<MozoBeam>(right)->mBeamScale);
                if (scale < maximum) {
                    maximum = gabi::fadds_ppc(scale, 1.0f);
                    gabi::at<MozoBeam>(right)->mBeamScale = (f32)(maximum);
                } else {
                    gabi::at<MozoBeam>(right)->mBeamTimer = (f32)(0.0f);
                    gabi::at<MozoBeam>(right)->mBeamState = (s16)(1);
                    gabi::at<MozoBeam>(right)->mBeamScale = (f32)(maximum);
                }
            } else
                gabi::at<MozoBeam>(right)->mBeamScale = (f32)(maximum);
            x = (u32)((*reinterpret_cast<be<u32> *>(&this->rightOrigin.x)));
            (*reinterpret_cast<be<u32> *>(&gabi::at<MozoBeam>(right)->current.pos.x)) = (u32)(x);
            y = (u32)((*reinterpret_cast<be<u32> *>(&this->rightOrigin.y)));
            (*reinterpret_cast<be<u32> *>(&gabi::at<MozoBeam>(right)->current.pos.y)) = (u32)(y);
            z = (u32)((*reinterpret_cast<be<u32> *>(&this->rightOrigin.z)));
            (*reinterpret_cast<be<s16> *>(&gabi::at<MozoBeam>(right)->current.angle.x)) =
                (s16)(pitch);
            (*reinterpret_cast<be<u32> *>(&gabi::at<MozoBeam>(right)->current.pos.z)) = (u32)(z);
            (*reinterpret_cast<be<s16> *>(&gabi::at<MozoBeam>(right)->current.angle.y)) =
                (s16)(yaw);
            roll = (s16)((*angles)[2]);
            gabi::at<MozoBeam>(right)->mThickness = (f32)(15.0f);
            (*reinterpret_cast<be<s16> *>(&gabi::at<MozoBeam>(right)->current.angle.z)) =
                (s16)(roll);
        }
    }
    if (!checkRange(1))
        towait_proc_init();
}

/* 021D534C */
void daMozo_c::search_fire_proc_init() {
    setAnm(MOZO_ANM_SEARCH, 0.0f);
    this->mBtkAnm.mFrameCtrl.mFrame = (f32)(0.0f);
    this->mBrkAnm.frame.mFrame = (f32)(0.0f);
    this->mCurrentProc.i = -1;
    this->mBrkAnm.frame.mRate = (f32)(1.0f);
    this->mCurrentProc.d = 0;
    this->mCurrentProc.f = MOZO_SEARCH_FIRE;
    this->fireTimer = 0;
    this->mBtkAnm.mFrameCtrl.mRate = (f32)(1.0f);
}

/* 021D593C */
void daMozo_c::search_fire_proc() {
    gabi::Local<cXyz> toward, forward, direction, endpoint;
    gabi::Local<be<f32>[4]> rotation;
    gabi::Local<u8[16]> linkage;
    u32 base = gabi::ea(this);
    mozoGetPlay();
    f32 frame = (f32)(this->mAnimMorf->mFrameCtrl.mFrame);
    bool rotate = frame > 30.0f;
    if (!rotate) {
        s32 animation = this->animation;
        rotate = animation == MOZO_ANM_FIRE_REVERSE || animation == MOZO_ANM_FIRE;
    }
    if (rotate) {
        mozoMtxYrotS(&l_calcMtx, (s16)this->current.angle.y);
        mozoMtxMultVec(&l_calcMtx, &l_fireAim, toward.get());
        mozoVectorAdd(toward.get(), &this->current.pos, toward.get());
        mozoVectorSubtract(toward.get(), &this->unrotatedOrigin, toward.get());
        mozoVectorDifference(&this->unrotatedTarget, forward.get(), &this->unrotatedOrigin);
        mozoQuatFromVectors(rotation.get(), forward.get(), toward.get());
        mozoQuatSlerp(&this->mQuatRotation, rotation.get(), &this->mQuatRotation, 0.2f);
    }
    anime_proc();
    s32 animation = this->animation;
    if (animation == MOZO_ANM_SEARCH || animation == MOZO_ANM_FIRE) {
        u32 morf = gabi::ea(this->mAnimMorf.get());
        f32 threshold =
            gabi::fsubs_ppc((f32)(s16)(gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mEnd), 50.0f);
        if (!((f32)(gabi::at<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame) > threshold))
            return;
        if (!(u32)this->fireEmitter)
            this->fireEmitter = mozoParticle(base + 0x314, 0x81A7, 0);
        if (!(u32)this->smokeEmitter)
            this->smokeEmitter = mozoParticle(base + 0x314, 0x81A8, 2);
        u32 model = (u32)((*reinterpret_cast<be<u32> *>(&this->mAnimMorf->mpModel)));
        u32 matrices = (u32)(gabi::at<MozoModel>(model)->mpMatrices);
        u16 flags = (u16)(gabi::at<MozoMatrices>(matrices)->mFlags);
        u32 data = (u32)(gabi::at<MozoMatrices>(matrices)->mpAnimationMatrices);
        gabi::at<MozoMatrices>(matrices)->mFlags = (u16)(flags | 0x10);
        mozoMtxCopy(gabi::at<MozoMtx>(data) + 2, &l_calcMtx);
        mozoMtxXYZrotM(&l_calcMtx, 0x640, 0x4000, 0);
        mozoMtxTranslateM(0.0f, 50.0f, 52.0f);
        u32 emitter = this->fireEmitter;
        if (emitter)
            mozoEmitterMatrix(&l_calcMtx, &gabi::at<MozoEmitter>(emitter)->mGlobalRotation,
                              &gabi::at<MozoEmitter>(emitter)->mGlobalTranslation);
        emitter = this->smokeEmitter;
        if (emitter)
            mozoEmitterMatrix(&l_calcMtx, &gabi::at<MozoEmitter>(emitter)->mGlobalRotation,
                              &gabi::at<MozoEmitter>(emitter)->mGlobalTranslation);
        mozoVectorDifference(&this->fireTarget, direction.get(), &this->fireOrigin);
        f32 x, y, z;
        if (mozoVectorNormalize(direction.get())) {
            y = direction->y;
            z = direction->z;
            x = direction->x;
        } else {
            x = (f32)((*reinterpret_cast<be<f32> *>(&l_defaultDirection)));
            y = (f32)(l_defaultDirection.y);
            direction->x = x;
            z = (f32)(l_defaultDirection.z);
            direction->y = y;
            direction->z = z;
        }
        f32 time = (f32)(s16)this->fireTimer;
        f32 length = gabi::fmuls_ppc(time, 45.0f);
        f32 lengthDifference = gabi::fsubs_ppc(length, 600.0f);
        f32 radius = gabi::fmuls_ppc(time, 3.0f);
        f32 radiusDifference = gabi::fsubs_ppc(radius, 80.0f);
        endpoint->x = x;
        if (lengthDifference >= 0.0f)
            length = 600.0f;
        endpoint->y = y;
        if (radiusDifference >= 0.0f)
            radius = 80.0f;
        endpoint->z = z;
        mozoVectorScale(endpoint.get(), endpoint.get(), length);
        mozoVectorAdd(endpoint.get(), &this->fireOrigin, endpoint.get());
        mozoCapsuleSetEnds(&this->mCps.mGeometry, &this->fireOrigin, endpoint.get());
        u32 dx = (u32)((*reinterpret_cast<be<u32> *>(&direction->x)));
        u32 dy = (u32)((*reinterpret_cast<be<u32> *>(&direction->y)));
        (*reinterpret_cast<be<u32> *>(&this->mCps.mGObjAt.mVec.x)) = (u32)(dx);
        u32 dz = (u32)((*reinterpret_cast<be<u32> *>(&direction->z)));
        this->mCps.mGeometry.mRadius = (f32)(radius);
        (*reinterpret_cast<be<u32> *>(&this->mCps.mGObjAt.mVec.y)) = (u32)(dy);
        (*reinterpret_cast<be<u32> *>(&this->mCps.mGObjAt.mVec.z)) = (u32)(dz);
        u32 common = mozoGetPlay();
        mozoCollisionSet(&gabi::at<MozoPlay>(common)->mCollision, &this->mCps);
        u32 ex = (u32)((*reinterpret_cast<be<u32> *>(&endpoint->x)));
        u32 ey = (u32)((*reinterpret_cast<be<u32> *>(&endpoint->y)));
        (*reinterpret_cast<be<u32> *>(&this->soundPosition.x)) = (u32)(ex);
        u32 ez = (u32)((*reinterpret_cast<be<u32> *>(&endpoint->z)));
        (*reinterpret_cast<be<u32> *>(&this->soundPosition.y)) = (u32)(ey);
        (*reinterpret_cast<be<u32> *>(&this->soundPosition.z)) = (u32)(ez);
        mozoLevelSound(0x7033, &this->soundPosition);
        s16 timer = this->fireTimer;
        this->fireTimer = (s16)(timer + 1);
        if (timer > 60) {
            setAnm(MOZO_ANM_FIRE_REVERSE, 0.0f);
            emitter = this->fireEmitter;
            if (emitter) {
                gabi::at<MozoEmitter>(emitter)->mRate = (f32)(0.0f);
                emitter = this->fireEmitter;
                mozoStopEmitter(emitter);
                this->fireEmitter = 0;
            }
            emitter = this->smokeEmitter;
            if (emitter) {
                gabi::at<MozoEmitter>(emitter)->mRate = (f32)(0.0f);
                emitter = this->smokeEmitter;
                mozoStopEmitter(emitter);
                this->smokeEmitter = 0;
            }
            towait_proc_init();
        }
    } else if (animation == MOZO_ANM_FIRE_REVERSE &&
               (f32)(this->mAnimMorf->mFrameCtrl.mFrame) < 30.0f) {
        setAnm(MOZO_ANM_FIRE, 8.0f);
        this->fireTimer = 0;
    }
}

/* 021D54A8 */
void daMozo_c::towait_proc_init() {
    setAnm(MOZO_ANM_TO_WAIT, 0.0f);
    this->mAnimMorf->mFrameCtrl.mRate = (f32)(-0.5f);
    this->mBrkAnm.frame.mRate = (f32)(-1.0f);
    this->mCurrentProc.d = 0;
    this->mCurrentProc.f = MOZO_TO_WAIT;
    this->mCurrentProc.i = -1;
    this->mBtkAnm.mFrameCtrl.mRate = (f32)(-1.0f);
}

/* 021D5E08 */
void daMozo_c::towait_proc() {
    u32 left = getBeamActor(this->leftChild);
    u32 right = getBeamActor(this->rightChild);
    mozoQuatSlerp(&this->mQuatRotation, &l_identityQuat, &this->mQuatRotation, 0.05f);
    anime_proc();
    u8 type = this->field_0x376;
    bool finished = false;
    if (type == MOZO_BEAM) {
        if (!left || !right)
            return;
        u32 emitter = (u32)(gabi::at<MozoBeam>(left)->mpEmitter);
        if (emitter) {
            mozoStopEmitter(emitter);
            gabi::at<MozoBeam>(left)->mpEmitter = (u32)(0);
        }
        s16 state = (s16)(gabi::at<MozoBeam>(left)->mBeamState);
        if (state == 1) {
            f32 scale = (f32)(gabi::at<MozoBeam>(left)->mBeamScale);
            if (scale < 5.0f)
                gabi::at<MozoBeam>(left)->mBeamScale = (f32)(gabi::fadds_ppc(scale, 1.0f));
            f32 timer = (f32)(gabi::at<MozoBeam>(left)->mBeamTimer);
            if (timer < 4.0f)
                gabi::at<MozoBeam>(left)->mBeamTimer = (f32)(gabi::fadds_ppc(timer, 1.0f));
            else {
                gabi::at<MozoBeam>(left)->mBeamScale = (f32)(0.0f);
                gabi::at<MozoBeam>(left)->mBeamState = (s16)(0);
                gabi::at<MozoBeam>(left)->mBeamTimer = (f32)(0.0f);
                finished = true;
            }
        } else {
            gabi::at<MozoBeam>(left)->mBeamScale = (f32)(0.0f);
            gabi::at<MozoBeam>(left)->mBeamTimer = (f32)(0.0f);
            finished = true;
        }
        emitter = (u32)(gabi::at<MozoBeam>(right)->mpEmitter);
        if (emitter) {
            mozoStopEmitter(emitter);
            gabi::at<MozoBeam>(right)->mpEmitter = (u32)(0);
        }
        state = (s16)(gabi::at<MozoBeam>(right)->mBeamState);
        if (state == 1) {
            f32 scale = (f32)(gabi::at<MozoBeam>(right)->mBeamScale);
            if (scale < 5.0f)
                gabi::at<MozoBeam>(right)->mBeamScale = (f32)(gabi::fadds_ppc(scale, 1.0f));
            f32 timer = (f32)(gabi::at<MozoBeam>(right)->mBeamTimer);
            if (timer < 4.0f) {
                gabi::at<MozoBeam>(right)->mBeamTimer = (f32)(gabi::fadds_ppc(timer, 1.0f));
                return;
            }
            gabi::at<MozoBeam>(right)->mBeamTimer = (f32)(0.0f);
            gabi::at<MozoBeam>(right)->mBeamScale = (f32)(0.0f);
            gabi::at<MozoBeam>(right)->mBeamState = (s16)(0);
        } else {
            gabi::at<MozoBeam>(right)->mBeamScale = (f32)(0.0f);
            gabi::at<MozoBeam>(right)->mBeamTimer = (f32)(0.0f);
        }
        if (!finished)
            return;
    } else if (type != MOZO_FIRE)
        return;
    if (!((f32)((*reinterpret_cast<be<f32> *>(&this->mQuatRotation[3]))) > 0.99f))
        return;
    if (!((f32)(this->mAnimMorf->mFrameCtrl.mFrame) < 25.0f))
        return;
    if (!((f32)(this->mBrkAnm.frame.mFrame) < 1.0f))
        return;
    if (!((f32)(this->mBtkAnm.mFrameCtrl.mFrame) < 1.0f))
        return;
    wait_proc_init();
}

/* 021D5030 */
BOOL daMozo_c::checkRange(s32 expanded) {
    gabi::Local<cXyz> difference, horizontal, facing;
    gabi::Local<u8[16]> linkage;
    u32 common = mozoGetPlay();
    u32 player = (u32)((*reinterpret_cast<be<u32> *>(&gabi::at<MozoPlay>(common)->mpPlayer)));
    if (expanded == 0 && (f32)(gabi::at<MozoPlayer>(player)->mStatueRangeState) < 0.0f)
        return 0;
    mozoVectorDifference(&gabi::at<MozoPlayer>(player)->current.pos, difference.get(),
                         &this->current.pos);
    horizontal->x = difference->x;
    horizontal->y = 0.0f;
    horizontal->z = difference->z;
    f32 square = mozoVectorSquare(horizontal.get());
    f32 distance = mozoSqrt(square);
    if ((f32)difference->y > -280.0f)
        return 0;
    u32 hio = (u32) reinterpret_cast<be<u32> *>(&l_HIO.mpBeamChild)[(u8)this->field_0x376];
    u32 angle = (u16)((*reinterpret_cast<be<u16> *>(&this->current.angle.y)));
    f32 maximum = (expanded ? (f32)gabi::at<daMozo_childHIO_c>(hio)->m08
                            : (f32)gabi::at<daMozo_childHIO_c>(hio)->m04);
    s32 angleMaximum = (expanded ? (s16)gabi::at<daMozo_childHIO_c>(hio)->m0E
                                 : (s16)gabi::at<daMozo_childHIO_c>(hio)->m0C);
    u32 trig = gabi::ea(&l_sincos.mEntries[angle >> 3]);
    facing->y = 0.0f;
    facing->x = (f32)(gabi::at<MozoSincos>(trig)->mSin);
    facing->z = (f32)(gabi::at<MozoSincos>(trig)->mCos);
    f32 forward = mozoVectorDot(difference.get(), facing.get());
    common = mozoGetPlay();
    u32 other = (u32)((*reinterpret_cast<be<u32> *>(&gabi::at<MozoPlay>(common)->mpPlayer)));
    s32 toward = mozoSearchAngle(this, mozoPtr(other));
    s32 separation = mozoDistanceAngle(toward, (s16)this->current.angle.y);
    return separation < angleMaximum && distance < maximum && forward > 200.0f;
}

/* 021D483C */
void daMozo_c::setAnm(u32 animation, f32 blend) {
    this->animation = animation;
    if (animation > 4)
        return;
    u32 resource = mozoResource(6, "Mozo");
    f32 speed = 0.0f, start = 0.0f, end = -1.0f;
    switch (animation) {
    case 1:
        speed = 1.0f;
        break;
    case 2:
        speed = -0.25f;
        start = 24.0f;
        end = 36.0f;
        break;
    case 3:
        speed = 1.0f;
        start = 25.0f;
        break;
    case 4:
        speed = -0.25f;
        start = 32.0f;
        end = 36.0f;
        break;
    default:
        break;
    }
    mozoMorfSetAnm(this->mAnimMorf.get(), mozoPtr(resource), 0, blend, speed, start, end, 0);
}

/* 021D4B14 */
s32 daMozo_c::CreateInit() {
    gabi::Local<cXyz> childScale;
    gabi::Local<u8[16]> linkage;
    u32 base = gabi::ea(this);
    u32 model = (u32)((*reinterpret_cast<be<u32> *>(&this->mAnimMorf->mpModel)));
    u8 parameter = (u8)(reinterpret_cast<be<u8> *>(&this->mParameters)[3]);
    u32 data = (u32)(gabi::at<MozoModel>(model)->mpModelData);
    if (parameter == 0xFF)
        parameter = 0;
    if (parameter > 2)
        parameter = 2;
    u32 morf = gabi::ea(this->mAnimMorf.get());
    this->field_0x376 = parameter;
    model = (u32)((*reinterpret_cast<be<u32> *>(&gabi::at<mDoExt_McaMorf>(morf)->mpModel)));
    gabi::at<MozoModel>(model)->mUserArea = (u32)(base);
    u16 index = 0;
    u32 joints = mozoGetJointTree(mozoPtr(data));
    if ((u16)(gabi::at<MozoJointTree>(joints)->mJointNum) > 0) {
        do {
            index = (u16)(index + 1);
            joints = mozoGetJointTree(mozoPtr(data));
            if (index >= (u16)(gabi::at<MozoJointTree>(joints)->mJointNum))
                break;
            if (index == 2) {
                u32 count = (u32)(gabi::at<MozoModelData>(data)->mJointCapacity);
                u32 joint = (u32)(gabi::at<MozoModelData>(data)->mpJoints);
                if (count > 2)
                    joint += 0x38;
                gabi::at<MozoJoint>(joint)->mpCallback =
                    gabi::guest_fn(reinterpret_cast<const void *>(daMozo_nodeCallBack));
            }
        } while (true);
    }
    for (s32 i = 0; i < 4; ++i)
        reinterpret_cast<be<u32> *>(&this->mQuatRotation)[i] =
            (u32)((u32)(l_identityQuat.mWords[i]));
    mozoCullMin(this, -1000.0f, -1000.0f, -1000.0f);
    mozoCullMax(this, 1000.0f, 1000.0f, 1000.0f);
    model = (u32)((*reinterpret_cast<be<u32> *>(&this->mAnimMorf->mpModel)));
    u32 y = (u32)((*reinterpret_cast<be<u32> *>(&this->current.pos.y))),
        x = (u32)((*reinterpret_cast<be<u32> *>(&this->current.pos.x)));
    (*reinterpret_cast<be<u32> *>(&this->leftOrigin.y)) = (u32)(y);
    (*reinterpret_cast<be<u32> *>(&this->rightOrigin.x)) = (u32)(x);
    this->cullMtx = (u32)(model ? model + 0xC8 : 0);
    u32 z = (u32)((*reinterpret_cast<be<u32> *>(&this->current.pos.z)));
    u8 type = this->field_0x376;
    (*reinterpret_cast<be<u32> *>(&this->rightOrigin.z)) = (u32)(z);
    (*reinterpret_cast<be<u32> *>(&this->leftOrigin.z)) = (u32)(z);
    (*reinterpret_cast<be<u32> *>(&this->rightOrigin.y)) = (u32)(y);
    (*reinterpret_cast<be<u32> *>(&this->leftOrigin.x)) = (u32)(x);
    if (type == MOZO_BEAM) {
        s32 room = (s8)(this->tevStr.mRoomNo);
        u32 actorId = (u32)(this->mBsPcId);
        childScale->y = 1.5f;
        childScale->z = 20.0f;
        childScale->x = 1.5f;
        u32 child =
            mozoCreateChild(0xE8, actorId, 0, &this->leftOrigin, room, 0, childScale.get(), -1, 0);
        room = (s8)(this->tevStr.mRoomNo);
        this->leftChild = child;
        actorId = (u32)(this->mBsPcId);
        this->rightChild = mozoCreateChild(0xE8, actorId, 0x30000000, &this->rightOrigin, room, 0,
                                           childScale.get(), -1, 0);
    } else {
        mozoStatusInit(&this->mStts, 0xFF, 0xFF, this);
        mozoColliderSet(&this->mCps, &cps_src);
        (*reinterpret_cast<be<u32> *>(&this->mCps.mStts)) = (u32)(base + 0x558);
        mozoCapsuleSetEnds(&this->mCps.mGeometry, &this->current.pos, &this->current.pos);
        this->mCps.mGeometry.mRadius = (f32)(25.0f);
    }
    set_mtx();
    mozoMorfCalc(this->mAnimMorf.get());
    wait_proc_init();
    if ((s8)(l_HIO.mNo) < 0)
        *reinterpret_cast<be<u8> *>(&l_HIO.mNo) =
            (u8)(mozoCreateHioChild(STR(mozo_tu::hioName), &l_HIO));
    return 4;
}

/* 021D4DB8 */
s32 daMozo_c::_create() {
    u32 flags = (u32)(this->actor_condition);
    if (!(flags & 8)) {
        // HD: the inlined GHS constructor accepts a null actor.
        if (this) {
            mozoActorConstruct(this);
            this->__vtbl = mozo_tu::actor;
            mozoBrkConstruct(&this->mBrkAnm);
            mozoBtkConstruct(&this->mBtkAnm);
            mozoStatusConstruct(&this->mStts);
            mozoGlobalStatusConstruct(&this->mStts.__vtbl_gstts);
            this->mStts.__vtbl = (u32)(mozo_tu::status);
            this->mStts.__vtbl_gstts = (u32)(mozo_tu::globalStatus);
            mozoColliderConstruct(&this->mCps);
            this->mCps.mShapeVtable = (u32)(mozo_tu::shapeBase);
            this->mCps.mBoundsVtable = (u32)(mozo_tu::bounds);
            mozoCapsuleGeometryConstruct(&this->mCps.mGeometry);
            flags = (u32)(this->actor_condition);
            this->mCps.mShapeVtable = (u32)(mozo_tu::shape);
            this->mCps.__vtbl_hitinf = (u32)(mozo_tu::hitInfo);
            this->mCps.mGeometry.__vtbl = (u32)(mozo_tu::capsule);
        }
        this->actor_condition = (u32)(flags | 8);
    }
    s32 phase = mozoResLoad(&this->mPhs, STR(mozo_tu::loadArchive));
    if (phase == 4) {
        if (!mozoEntryHeap(this,
                           mozoPtr(gabi::guest_fn(reinterpret_cast<const void *>(CheckCreateHeap))),
                           0x1AA0))
            return 5;
        phase = CreateInit();
        _execute();
    }
    return phase;
}

/* 021D47D8 */
BOOL daMozo_c::_delete() {
    mozoResDelete(&this->mPhs, STR(mozo_tu::deleteArchive));
    mozoSoundDelete(&this->soundPosition);
    s32 child = (s8)(l_HIO.mNo);
    if (child >= 0) {
        mozoDeleteHioChild(child);
        l_HIO.mNo = (s8)(-1);
    }
    return 1;
}

/* 021D524C */
u32 daMozo_c::getBeamActor(s32 actorId) {
    gabi::Local<s32> id;
    *reinterpret_cast<be<s32> *>(id.get()) = (s32)(actorId);
    gabi::Local<u8[16]> linkage;
    u32 actor = 0;
    if (actorId != -1)
        actor = mozoSearchActor(mozoPtr(mozo_tu::searchByIdJudge), id.get());
    if (actor && mozoIsActor(mozoPtr(actor)) && (s16)(gabi::at<daMozo_c>(actor)->mProfName) == 0xE8)
        return actor;
    return 0;
}

/* 021D4538 */
void daMozo_c::event_move() {
    u8 state = (u8)(l_eventState);
    if (state == 2)
        return;
    if ((u16)(this->eventInfo.mCommand) == 2) {
        this->requestEvent = 0;
        l_eventState = (u8)(1);
        u32 common = mozoGetPlay();
        if (mozoEventEnd(&gabi::at<MozoPlay>(common)->mEventManager, STR(mozo_tu::eventName))) {
            common = mozoGetPlay();
            gabi::at<MozoPlay>(common)->mEventFlags =
                (u16)((u16)(gabi::at<MozoPlay>(common)->mEventFlags) | 8);
            l_eventState = (u8)(2);
            return;
        }
        state = (u8)(l_eventState);
    } else if (state == 1) {
        u32 common = mozoGetPlay();
        if (mozoEventEnd(&gabi::at<MozoPlay>(common)->mEventManager, STR(mozo_tu::eventName))) {
            common = mozoGetPlay();
            gabi::at<MozoPlay>(common)->mEventFlags =
                (u16)((u16)(gabi::at<MozoPlay>(common)->mEventFlags) | 8);
            l_eventState = (u8)(2);
            return;
        }
        state = (u8)(l_eventState);
    }
    if (state == 0 && (u8)this->requestEvent == 1) {
        mozoOrderEvent(this, STR(mozo_tu::eventName), 1, 0xFFFF);
        this->eventInfo.mCondition = (u16)((u16)(this->eventInfo.mCondition) | 2);
    }
}

/* 021D4734 */
BOOL daMozo_c::_execute() {
    mozoMorfCalc(this->mAnimMorf.get());
    s32 index = this->mCurrentProc.i;
    s32 adjustment = (s16)this->mCurrentProc.d;
    u32 object = gabi::ea(this) + adjustment;
    u32 target;
    if (index < 0)
        target = this->mCurrentProc.f;
    else {
        s32 tableOffset = (s16)(reinterpret_cast<be<s16> *>(&this->mCurrentProc.f)[1]);
        u32 entry = (u32)(*gabi::at<be<u32>>(object + tableOffset)) + index * 8;
        target = (u32)gabi::at<MozoProcedure>(entry)->target;
        gabi::cpu->r[7] = entry; // HD: these volatile registers are live at the original bctrl.
        gabi::cpu->r[9] = tableOffset;
    }
    gabi::cpu->r[8] = adjustment;
    gabi::cpu->r[10] = target;
    gabi::call_ptr(target, mozoPtr(object));
    event_move();
    set_mtx();
    return 1;
}

/* 021D4498 */
BOOL daMozo_c::_draw() {
    mozoGetPlay();
    u32 model = (u32)((*reinterpret_cast<be<u32> *>(&this->mAnimMorf->mpModel)));
    u32 data = (u32)(gabi::at<MozoModel>(model)->mpModelData);
    u32 light = mozoGetEnvironment();
    mozoSettingTev(mozoPtr(light), 1, &this->current.pos, &this->tevStr);
    u32 morf = gabi::ea(this->mAnimMorf.get());
    light = mozoGetEnvironment();
    model = (u32)((*reinterpret_cast<be<u32> *>(&gabi::at<mDoExt_McaMorf>(morf)->mpModel)));
    mozoLightTev(mozoPtr(light), mozoPtr(model), &this->tevStr);
    mozoBrkEntry(&this->mBrkAnm, mozoPtr(data), (f32)this->mBrkAnm.frame.mFrame);
    mozoBtkEntry(&this->mBtkAnm, mozoPtr(data), (f32)this->mBtkAnm.mFrameCtrl.mFrame);
    mozoMorfEntry(this->mAnimMorf.get());
    return 1;
}

/* 021D4534 */
static BOOL daMozo_Draw(daMozo_c *self) { return self->_draw(); }

/* 021D47CC */
static BOOL daMozo_Execute(daMozo_c *self) { return self->_execute(); }

/* 021D47D0 */
static BOOL daMozo_IsDelete(daMozo_c *self) { return 1; }

/* 021D4838 */
static BOOL daMozo_Delete(daMozo_c *self) { return self->_delete(); }

/* 021D4F10 */
static s32 daMozo_Create(daMozo_c *self) { return self->_create(); }

/* 021D614C */
static void __sinit_d_a_mozo_cpp() {
    l_headerWord2 = (u32)(0);
    l_headerWord0 = (u32)(0);
    l_headerWord3 = (u32)(0);
    l_headerWord1 = (u32)(0);
    mozoRegisterGlobal(&l_headerDtor0);
    l_piLow = (f32)((f32)(l_piLowConstant));
    l_piHigh = (f32)((f32)(l_piHighConstant));
    mozoAngleConstruct(&l_headerAngle);
    mozoRegisterGlobal(&l_headerDtor1);
    mozoCircleConstruct(&l_headerCircle);
    mozoRegisterGlobal(&l_headerDtor2);
    daMozo_HIO_c__ct(&l_HIO);
}

/* 021D61EC */
static void MozoSafeString_dtor(void *self, s32 flags) {
    if (self && (flags & 1))
        mozoDeallocate(self);
}

/* 021D6200 */
static void daMozo_c_dtor(daMozo_c *self, s32 flags) {
    if (!self)
        return;
    mozoColliderDestroy(&self->mCps, 2);
    mozoStatusDestroy(&self->mStts, 2);
    mozoActorDestroy(self, 0);
    if (flags & 1)
        mozoDeallocate(self);
}

/* 021D626C */
static void MozoSafeString_assureTerminationImpl(void *self) {}

/* WWHD glue (tools/verify/autoglue.py): Mozo translation unit
str 100152E0 "Mozo" in 021D42B4
str 10015344 "Mozo" in 021D483C
data l_headerAngle 10465788
data l_headerCircle 10465789
data l_headerDtor0 101BAC90
data l_headerDtor1 101BAC9C
data l_headerDtor2 101BACA8
data cps_src 101BAC44
data l_fireAim 1046581C
data l_resourceControl 101F4F28
data l_currentModel 104B462C
data l_eventState 101BACB4
data l_HIO 104657E4
data l_calcMtx 1048D0CC
data l_j3dMtx 104B4868
data l_identityQuat 101E9C38
data l_zeroAngles 101FFB14
data l_defaultDirection 101FFBA8
data l_sincos 104A44F8
data l_piLowConstant 100153C8
data l_piHighConstant 100153CC
data l_piLow 10465768
data l_piHigh 1046576C
data l_beamOriginGuard 10465770
data l_beamTargetGuard 10465774
data l_leftOriginGuard 10465778
data l_rightOriginGuard 1046577C
data l_fireOriginGuard 10465780
data l_fireTargetGuard 10465784
data l_headerWord0 1046578C
data l_headerWord1 10465790
data l_headerWord2 10465794
data l_headerWord3 10465798
data l_beamOrigin 1046579C
data l_beamTarget 104657A8
data l_leftOrigin 104657B4
data l_rightOrigin 104657C0
data l_fireOrigin 104657CC
data l_fireTarget 104657D8
ext 0200BD2C void mozoStatusConstruct(void*)
ext 0200E240 void mozoCollisionSet(void*,void*)
ext 0200FAAC s32 mozoDistanceAngle(s32,s32)
ext 02018150 void mozoCapsuleGeometryConstruct(void*)
ext 02018808 void mozoCapsuleSetEnds(void*,void*,void*)
ext 020195B0 s32 mozoAtan2(f32,f32)
ext 0201ADE0 void mozoVectorDifference(void*,void*,void*)
ext 0201B47C BOOL mozoVectorNormalize(void*)
ext 023127F8 void mozoQuatFromVectors(void*,void*,void*)
ext 02515860 void mozoStatusDestroy(void*,s32)
ext 02515980 void mozoColliderDestroy(void*,s32)
ext 02515DA0 void mozoGlobalStatusConstruct(void*)
ext 02515F14 void mozoStatusInit(void*,s32,s32,void*)
ext 02515FB8 void mozoColliderConstruct(void*)
ext 025164C0 void mozoColliderSet(void*,void*)
ext 025200D4 u32 mozoGetPlay()
ext 02520460 s32 mozoResLoad(void*,const char*)
ext 025204C8 void mozoResDelete(void*,const char*)
ext 02520540 s32 mozoGetReverb(s32)
ext 0254457C BOOL mozoEventEnd(void*,const char*)
ext 02555D0C u32 mozoGetEnvironment()
ext 025626A4 void mozoSettingTev(void*,s32,void*,void*)
ext 02562F5C void mozoLightTev(void*,void*,void*)
ext 025A847C u32 mozoParticleSet(void*,s32,u32,void*,s32,s32,s32,s32,s32,s32,s32,s32)
ext 025D4604 BOOL mozoIsActor(void*)
ext 025D4ED0 void mozoActorConstruct(void*)
ext 025D50BC void mozoActorDestroy(void*,s32)
ext 025D5218 u32 mozoSearchActor(void*,void*)
ext 025D5A20 u32 mozoCreateChild(s32,u32,u32,void*,s32,s32,void*,s32,s32)
ext 025D63E8 BOOL mozoEntryHeap(void*,void*,u32)
ext 025D672C void mozoCullMin(void*,f32,f32,f32)
ext 025D673C void mozoCullMax(void*,f32,f32,f32)
ext 025D6894 s32 mozoSearchAngle(void*,void*)
ext 025D77DC void mozoOrderEvent(void*,const char*,s32,u32)
ext 025E19CC void mozoLevelSound(u32,void*)
ext 025E1A40 void mozoSoundStart(u32,void*,s32,s32)
ext 025E1B0C void mozoSoundStop(void*,u32)
ext 025E1B34 void mozoSoundDelete(void*)
ext 025E4A98 void mozoMorfSetAnm(void*,void*,s32,f32,f32,f32,f32,s32)
ext 025E4F64 u32 mozoMorfConstruct(s32,void*,s32,s32,void*,s32,f32,s32,s32,s32,s32,s32,u32)
ext 025E535C void mozoMorfPlay(void*,s32,s32,s32)
ext 025E5590 void mozoMorfEntry(void*)
ext 025E55A0 void mozoMorfCalc(void*)
ext 025E742C void mozoAnimationPlay(void*)
ext 025E7C6C void mozoBtkConstruct(void*)
ext 025E7CE0 BOOL mozoBtkInit(void*,void*,void*,s32,s32,f32,s32,s32,s32,s32)
ext 025E7FC4 void mozoBtkEntry(void*,void*,f32)
ext 025E80D0 void mozoBrkConstruct(void*)
ext 025E8154 BOOL mozoBrkInit(void*,void*,void*,s32,s32,f32,s32,s32,s32,s32)
ext 025E83FC void mozoBrkEntry(void*,void*,f32)
ext 025F0A10 u32 mozoCreateHioChild(const char*,void*)
ext 025F0A18 void mozoDeleteHioChild(s32)
ext 025F1884 void mozoMtxYrotS(void*,s32)
ext 025F19F8 void mozoMtxXYZrotM(void*,s32,s32,s32)
ext 025F1C28 void mozoMtxYrotM(void*,s32)
ext 025F24E0 void mozoMtxTranslateM(f32,f32,f32)
ext 025F25CC void mozoMtxQuatM(void*)
ext 026066C4 u32 mozoGetResource(void*,void*,s32)
ext 0273AA24 void mozoAssertion(const char*,s32,const char*)
ext 0273AD10 void* mozoAllocate(u32)
ext 0273AF40 void mozoDeallocate(void*)
ext 027F2BF8 BOOL mozoFrameCheck(void*,f32)
ext 027F3F94 u32 mozoGetJointTree(void*)
ext 027F7878 u32 mozoGetJoint(void*)
ext 028249B0 void mozoEmitterMatrix(void*,void*,void*)
ext 028E8D88 void mozoVectorAdd(void*,void*,void*)
ext 028E8DAC void mozoVectorSubtract(void*,void*,void*)
ext 028E8DD0 f32 mozoVectorSquare(void*)
ext 028E8E64 void mozoVectorScale(void*,void*,f32)
ext 028E8F44 f32 mozoVectorDot(void*,void*)
ext 028E8F64 void mozoMtxMultVec(void*,void*,void*)
ext 028E90D4 void mozoMtxCopy(void*,void*)
ext 028E9108 void mozoMtxConcat(void*,void*,void*)
ext 028E93CC void mozoMtxTranslation(void*,f32,f32,f32)
ext 028E9BC0 void mozoQuatSlerp(void*,void*,void*,f32)
ext 028EAB2C void mozoCircleConstruct(void*)
ext 028ED6F8 void mozoAngleConstruct(void*)
ext 028F026C void mozoRegisterGlobal(void*)
ext 028F4384 f32 mozoSqrt(f32)
*/
