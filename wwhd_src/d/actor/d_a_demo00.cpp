/** Scripted demo models and lights, verified against the WWHD implementation.
 * The GameCube actor supplies the names and structure. HD uses SafeString resource
 * lookups, framebuffer textures, and an auxiliary animated model. Private source. */
#include "d/actor/d_a_demo00.h"
#include "bindings.h"
#include "d/d_demo.h"
#include "d/d_kankyo_demo.h"
#include "gc_api/tu.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1000D6D0,
    .safeStringAssure = 0x02122914,
    .gndChk = {0x1000D6F8, 0x1000D708, 0x1000D728, 0x1000D718},
};
static constexpr u32 l_actorVtable = 0x1000D9E4;
#include "gabi.h"
static s32 createHeapCallBack(daDemo00_c*);
void* demo00_lightConstructor(void*);
void demo00_stringCallback(void*);

template <class T> T* demoView(void* p) {
    return static_cast<T*>(p);
}

// HD: native adapters preserve the verified argument and full-register result widths.
void demoActorConstruct(void*);
void demoInitTev(void*, s8, u32);
void demoDeleteLight(void*);
void demoActorDestruct(void*, u32);
void demoDeleteHeap(void*);
void demoRegisterDestructor(void*);
void demoHeaderConstructA(void*);
void demoHeaderConstructB(void*);
void demoMatrixRotationZXY(void*, s16, s16, s16);
s32 demoEntrySolidHeap(void*, void*, u32);
void demoAnimationPlay(void*);
void* demoGetResourceByID(void*, void*, u16);
void* demoGetResourceByName(void*, void*, void*);
void demoAssert(const char*, s32, const char*);
void* demoBtpConstruct(void*);
s32 demoBtpInit(void*, void*, void*, u32, u32, s32, s32, u32, u32, f32);
void* demoBtkConstruct(void*);
s32 demoBtkInit(void*, void*, void*, u32, u32, s32, s32, u32, u32, f32);
void* demoBrkConstruct(void*);
s32 demoBrkInit(void*, void*, void*, u32, u32, s32, s32, u32, u32, f32);
void* demoModelCreate(void*, u32, u32);
void* demoGetModelMetadata(void*);
void demoModelSetFlags(void*, u32);
DemoMorfView* demoMorfConstruct(void*, void*, void*, void*, void*, s32, void*, s32, u32, void*, u32,
                                u32, f32);
void* demoGetTextureName(void*, u16);
void* demoGetFramebufferData(void*, u32);
void* demoImageConstruct(void*, void*);
void* demoInvisibleConstruct(void*);
s32 demoInvisibleCreate(void*, void*);
void demoGroundConstruct(void*);
void demoLightInit(void*, void*, u32, u8, u32);
void demoSettingTevStruct(void*, s32, void*, void*);
void demoSetLightTevColor(void*, void*, void*);
void demoBtpEntry(void*, void*, s16);
void demoBtkEntry(void*, void*, f32);
void demoBrkEntry(void*, void*, f32);
void demoPrepareTextureSlot(void*, u32);
void demoInvisibleMorfEntry(void*, void*);
void demoInvisibleModelEntry(void*, void*);
void demoMorfUpdateDL(void*);
void demoDrawAuxiliaryNative(void*);
void demoModelUpdateDL(void*, u32);
s32 demoSeaContains(f32, f32);
f32 demoSeaHeight(f32, f32);
void demoGetWaterColor(void*, void*);
void demoApplyExposure(void*, void*, f32);
void* demoMaterialColor(void*, s32);
void* demoGetActor(void*, u32);
void demoParseParameter(void*, void*);
void demoSetEventBit(void*, u16);
void demoSetItem(u8);
void demoStopQuake(void*, s32);
void demoStartShock(void*, s32, s32, void*);
void demoStartQuake(void*, s32, s32, void*);
void demoFadeIn(s8);
void demoFadeOut(s8, s32);
void demoSetFadeColor(void*);
void* demoFindActorByName(const char*, s32, s32);
void demoSetActorData(void*, u32, u32, u32, u32, u32, u32, u32);
void demoLightSetPosition(void*, void*);
void demoMorfSetAnimation(void*, void*, s32, s32, f32, f32, f32, f32);
f32 demoGroundCross(void*, void*);
u32 demoGroundSoundID(void*, void*);
void demoMorfPlay(void*, void*, u32, s32);
void* demoGetJointMetadata(void*);
s32 demoFindJoint(void*, void*);

static GXColor l_fadeWhite;
static GXColor l_fadeBlack;

static f32 l_one = 1.0f;
static f32 l_zero = 0.0f;
static f32 l_demoSeaHeightA = -3555.0f;
static f32 l_demoSeaHeightB = 672.0f;
static f32 l_maxPatternFrame = 3.0f;
static f32 l_auxiliaryYawBias = 1.5707963705062866f;
static f32 l_angleScale = 10430.3779296875f;
static f32 l_colorMaximum = 255.0f;
static f32 l_motionScale = 0.5f;
static f32 l_frameRate = 0.019999999552965164f;
static f32 l_frameLimit = 0.4000000059604645f;
static f32 l_animationEnd = -1.0f;
static f32 l_groundCheckHeight = 100.0f;
static f32 l_groundSoundDistance = 20.0f;
static f32 l_demoHeightAmplitude = 5.0f;
static f32 l_demo46OffsetX = -30.0f;
static f32 l_demo46OffsetZ = 30.0f;
static f32 l_demo44Height = -100.0f;
static f32 l_demo44Roll = 800.0f;
static f32 l_demo17OffsetY = 40.0f;
static f32 l_demo17OffsetZ = 8.0f;
static f32 l_demo17OffsetX = 10.0f;
static f32 l_demo17PlaySpeed = 2.0f;
static f32 l_headerPi = -3.1415927410125732f;
static f32 l_headerPiHalf = 3.1415927410125732f;
static s8 l_stayRoom;
static s16 l_demoPhase;
static u32 l_demoFrame;
static s16 l_monotone;
struct DemoLEventbitsStorage {
    be<u16> mValues[50];
};
WWHD_SIZE(DemoLEventbitsStorage, 100);
static DemoLEventbitsStorage l_eventBits = {
    {0xFFFF, 0x2A80, 0xFFFF, 0xFFFF, 0xFFFF, 0x2401, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
     0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x2110, 0xFFFF, 0xFFFF,
     0xFFFF, 0xFFFF, 0xFFFF, 0x2D01, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
     0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
     0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x3802}};
struct DemoLItemnumbersStorage {
    be<u8> mValues[10];
};
WWHD_SIZE(DemoLItemnumbersStorage, 10);
static DemoLItemnumbersStorage l_itemNumbers = {
    {0x39, 0x3A, 0x3E, 0x6A, 0x6B, 0x69, 0x30, 0x3B, 0x35, 0xFF}};
struct DemoLLightdataStorage {
    DemoLightDefinition mValues[3];
};
WWHD_SIZE(DemoLLightdataStorage, 24);
static DemoLLightdataStorage l_lightData;
static void* l_demoManager;
static void* l_resourceManager;
static void* l_graphicsSystem;
static void* l_saveInfo;
static u32 l_opaDrawBuffer;
static u32 l_xluDrawBuffer;
static Mtx34 l_matrix;
struct DemoLHeaderzeroStorage {
    be<u32> mValues[4];
};
WWHD_SIZE(DemoLHeaderzeroStorage, 16);
static DemoLHeaderzeroStorage l_headerZero;
static f32 l_headerPiValue;
static f32 l_headerPiHalfValue;
static u8 l_headerFlagA;
static u8 l_headerFlagB;
struct DemoLDestructorrecordaStorage {
    be<u32> mValues[3];
};
WWHD_SIZE(DemoLDestructorrecordaStorage, 12);
static DemoLDestructorrecordaStorage l_destructorRecordA;
struct DemoLDestructorrecordbStorage {
    be<u32> mValues[3];
};
WWHD_SIZE(DemoLDestructorrecordbStorage, 12);
static DemoLDestructorrecordbStorage l_destructorRecordB;
struct DemoLDestructorrecordcStorage {
    be<u32> mValues[3];
};
WWHD_SIZE(DemoLDestructorrecordcStorage, 12);
static DemoLDestructorrecordcStorage l_destructorRecordC;
static cXyz l_jointOffset;
struct DemoLArchivenameStorage {
    char mValues[8];
};
WWHD_SIZE(DemoLArchivenameStorage, 8);
static DemoLArchivenameStorage l_archiveName;

namespace {
void* ptr(u32 a) {
    return gabi::at<void>(a);
}
void* offset(void* p, u32 n) {
    return ptr(gabi::ea(p) + n);
}
// HD: compact PTMF virtual offsets and parameter streams have dynamic word locations.
be<u32>& word(void* p, u32 n) {
    return *gabi::at<be<u32>>(gabi::ea(p) + n);
}
void setAction(daDemo00_c* actor, u32 target) {
    actor->actionAdjustment = 0;
    actor->actionIndex = -1;
    actor->actionTarget = target;
}
} // namespace

/* 0212139C */
void daDemo00_resID_c::reset() {
    mBrkID = -1u;
    mPlightID = -1u;
    mAuxiliaryID = -1u;
    mBckID = -1u;
    mBtkID = -1u;
    mBtpID = -1u;
    mShapeID = -1u;
}

/* 021213C0 */
s32 daDemo00_c::create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this)) {
            demoActorConstruct(this);
            condition = actor_condition;
            __vtbl = l_actorVtable;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    s8 room = l_stayRoom;
    demoInitTev(&tevStr, room, 0xFFu);
    setAction(this, 0x02121624);
    mNextID.reset();
    mPreviousDrawMode = -1;
    return cPhs_COMPLEATE_e;
}

/* 02121460 */
void daDemo00_model_c::reset() {
    mID.reset();
    void* light = mpPlight;
    mpModel = nullptr;
    mpAuxiliary = nullptr;
    mpBtpAnm = nullptr;
    mpBtkAnm = nullptr;
    mpBrkAnm = nullptr;
    if (light) {
        demoDeleteLight(light);
        mpPlight = nullptr;
    }
}

/* 02122804 */
s32 daDemo00_c::actLeaving(void* demoActor) {
    DemoMorfView* morf = mModel.mpMorf;
    if (morf)
        reinterpret_cast<mDoExt_McaMorf*>(morf)->stopZelAnime();
    demoDeleteHeap(this);
    setAction(this, 0x02121624);
    return 1;
}

/* 02121394 */
s32 daDemo00_Delete(daDemo00_c* actor) {
    return 1;
}

/* 0212290C */
s32 daDemo00_IsDelete(daDemo00_c* actor) {
    return 1;
}

/* 0212153C */
void daDemo00_c::setBaseMtx() {
    void* matrix = &l_matrix;
    f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
    PSMTXTrans(static_cast<Mtx34*>(matrix), x, y, z);
    s16 ax = shape_angle.x, az = shape_angle.z, ay = shape_angle.y;
    demoMatrixRotationZXY(matrix, ax, ay, az);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = static_cast<Mtx34*>(matrix)->m[i / 4][i % 4];
    DemoModelView* model = mModel.mpModel;
    for (u32 i = 0; i < 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    model = mModel.mpModel;
    y = scale.y;
    x = scale.x;
    z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    model = mModel.mpModel;
    reinterpret_cast<J3DModel*>(model)->calc();
}

namespace {
struct DemoString {
    be<u32> string, vtable;
};
bool demoArchiveEquals(const char* literal) {
    gabi::Local<DemoString> left, right;
    right->vtable = gc_tu.safeString;
    right->string = gabi::ea(&l_archiveName);
    left->vtable = gc_tu.safeString;
    left->string = gabi::guest_addr(literal);
    demo00_stringCallback(left.get());
    u32 vtable = left->vtable;
    gabi::call_ptr((u32)demoView<DemoSafeStringVtable>(ptr(vtable))->mMethods[5], left.get());
    vtable = right->vtable;
    u32 target = demoView<DemoSafeStringVtable>(ptr(vtable))->mMethods[5];
    u32 a = left->string;
    gabi::call_ptr(target, right.get());
    u32 b = right->string;
    if (a == b)
        return true;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 ca = *gabi::at<be<u8>>(a + i);
        u8 cb = *gabi::at<be<u8>>(b + i);
        if (ca != cb)
            return false;
        if (!ca)
            return true;
    }
    return false;
}
void invokeAction(daDemo00_c* actor, void* demoActor) {
    s16 index = actor->actionIndex;
    s16 adjustment = actor->actionAdjustment;
    void* self = ptr(gabi::ea(actor) + adjustment);
    u32 target;
    if (index < 0)
        target = actor->actionTarget;
    else {
        s16 tableOffset = *gabi::at<be<s16>>(gabi::ea(actor) + 0x3B2);
        u32 table = word(self, tableOffset);
        target = word(ptr(table), (u32)(s32)index * 8 + 4);
    }
    gabi::call_ptr(target, self, demoActor);
}
} // namespace

/* 02121624 */
s32 daDemo00_c::actStandby(void* demoActor) {
    if ((u32)mNextID.mShapeID == -1u && (u32)mNextID.mPlightID == -1u)
        return 1;
    u32 auxiliary = mNextID.mAuxiliaryID, shape = mNextID.mShapeID, bck = mNextID.mBckID;
    mModel.mID.mShapeID = shape;
    mModel.mID.mBckID = bck;
    u32 btp = mNextID.mBtpID;
    mModel.mID.mAuxiliaryID = auxiliary;
    mModel.mID.mBtpID = btp;
    u32 brk = mNextID.mBrkID, btk = mNextID.mBtkID, plight = mNextID.mPlightID;
    mModel.mID.mBtkID = btk;
    mModel.mID.mBrkID = brk;
    mModel.mID.mPlightID = plight;
    if (!demoEntrySolidHeap(this, gabi::at<void>(gabi::guest_fn((const void*)createHeapCallBack)),
                            0x4000u))
        return 1;
    if ((void*)mModel.mpModel) {
        (this)->setBaseMtx();
        DemoModelView* model = mModel.mpModel;
        cullMtx = model ? gabi::ea(model) + 0xC8 : 0;
        demoView<dDemo_actor_c>(demoActor)->mModel = reinterpret_cast<J3DModel*>(model);
        DemoMorfView* morf = mModel.mpMorf;
        if (morf)
            demoView<dDemo_actor_c>(demoActor)->mAnmFrameMax = (f32)(s16)morf->mFrameCtrl.mEnd;
    }
    s8 argument = this->argument;
    setAction(this, 0x021219B8);
    bool enter = argument == 1 && demoArchiveEquals("Demo08");
    if (!enter) {
        if (argument == 1)
            argument = this->argument;
        enter = argument == 2 && demoArchiveEquals("Demo32");
    }
    if (enter)
        invokeAction(this, demoActor);
    return 1;
}

/* 021218E4 */
void daDemo00_c::adjustPatternFrame() {
    gabi::Local<cXyz> motion;
    cXyz_mi(&current.pos, motion.get(), &old.pos);
    DemoMorfView* morf = mModel.mpMorf;
    u32 state = morf->mFlags;
    void* direction = ptr((u32)gabi::ea(morf->mpJoint.get()) + 0x10);
    morf->mFlags = state | 8;
    PSVECAdd(motion.get(), static_cast<cXyz*>(direction), motion.get());
    f32 squared = PSVECSquareMag(motion.get());
    f32 length = std_sqrtf(squared);
    f32 bias = l_motionScale;
    f32 target = gabi::fmadds(length, bias, bias);
    f32 minimum = l_one;
    f32 frameTarget = minimum;
    if (!(target < minimum)) {
        f32 maximum = l_maxPatternFrame;
        frameTarget = target - maximum >= 0.0f ? maximum : target;
    }
    void* animation = mModel.mpAuxiliary;
    f32 rate = l_frameRate;
    f32 limit = l_frameLimit;
    gabi::Local<be<f32>> frame;
    *frame = demoView<DemoAuxiliaryView>(animation)->mFrameCtrl.mRate;
    cLib_addCalc(frame.get(), frameTarget, rate, limit, rate);
    f32 result = *frame;
    animation = mModel.mpAuxiliary;
    demoView<DemoAuxiliaryView>(animation)->mFrameCtrl.mRate = result;
    animation = mModel.mpAuxiliary;
    demoAnimationPlay(&demoView<DemoAuxiliaryView>(animation)->mFrameCtrl);
}

namespace {
u8 byte(u32 address) {
    return *gabi::at<be<u8>>(address);
}
bool equalChars(u32 a, u32 b, u32 maximum = 0x40001) {
    if (a == b)
        return true;
    for (u32 i = 0; i < maximum; ++i) {
        u8 ca = byte(a + i), cb = byte(b + i);
        if (ca != cb)
            return false;
        if (!ca)
            return true;
    }
    return false;
}
void callStringCallback(DemoString* string) {
    u32 table = string->vtable;
    gabi::call_ptr((u32)demoView<DemoSafeStringVtable>(ptr(table))->mMethods[5], string);
}
bool equalStrings(DemoString* left, DemoString* right, unsigned leftCalls) {
    for (unsigned i = 0; i < leftCalls; ++i)
        callStringCallback(left);
    u32 table = right->vtable;
    u32 target = demoView<DemoSafeStringVtable>(ptr(table))->mMethods[5];
    u32 a = left->string;
    gabi::call_ptr(target, right);
    return equalChars(a, right->string);
}
void initString(DemoString* string, u32 text) {
    string->vtable = gc_tu.safeString;
    string->string = text;
}
void initString(DemoString* string, const char* text) {
    initString(string, gabi::guest_addr(text));
}
void* demoResource(u16 id) {
    gabi::Local<DemoString> archive;
    archive->vtable = gc_tu.safeString;
    void* manager = l_resourceManager;
    archive->string = gabi::ea(&l_archiveName);
    return demoGetResourceByID(manager, archive.get(), id);
}
void* namedDemoResource(const char* archiveName, const char* resourceName) {
    gabi::Local<DemoString> archive, resource;
    initString(archive.get(), archiveName);
    initString(resource.get(), resourceName);
    void* manager = l_resourceManager;
    return demoGetResourceByName(manager, archive.get(), resource.get());
}
u32 animationMode(void* resource) {
    DemoAnimationVtable* table = demoView<DemoAnimationResource>(resource)->mpVtable;
    return gabi::call_ptr<u32>((u32)table->mMethods[3], resource);
}
u32 shiftLeft(u32 word, u32 amount) {
    return amount & 32 ? 0 : word << (amount & 31);
}
u32 shiftRight(u32 word, u32 amount) {
    return amount & 32 ? 0 : word >> (amount & 31);
}
struct DemoHeapTextureScratch {
    DemoTextureDescriptor descriptor;
    u8 reserved[0x160 - sizeof(DemoTextureDescriptor)];
};
} // namespace

/* 0211F35C */
s32 daDemo00_c::createHeap() {
    // The texture descriptor encodes its guest address into resource-relative offsets.
    // HD places it at entry SP - 0x160 (frame 0x178, descriptor at +0x18).
    gabi::Local<DemoHeapTextureScratch> textureScratch;
    if ((u32)mModel.mID.mShapeID != -1u) {
        void* data = demoResource((u16)(u32)mModel.mID.mShapeID);
        if (!data)
            demoAssert("d_a_demo00.cpp", 0x115u, "modelData != (0)");
        u32 modelFlags = 0x11000002;
        f32 speed = l_one;
        if ((u32)mModel.mID.mBtpID != -1u) {
            void* controller = demoBtpConstruct(nullptr);
            mModel.mpBtpAnm = demoView<J3DFrameCtrl>(controller);
            if (!controller)
                return 0;
            void* resource = demoResource((u16)(u32)mModel.mID.mBtpID);
            if (!resource)
                return 1;
            u32 mode = animationMode(resource);
            controller = mModel.mpBtpAnm;
            if (!demoBtpInit(controller, data, resource, 1u, mode, 0u, -1, 0u, 0u, speed))
                return 0;
            modelFlags = 0x15020002;
        }
        if ((u32)mModel.mID.mBtkID != -1u) {
            void* controller = demoBtkConstruct(nullptr);
            mModel.mpBtkAnm = demoView<J3DFrameCtrl>(controller);
            if (!controller)
                return 0;
            void* resource = demoResource((u16)(u32)mModel.mID.mBtkID);
            if (!resource)
                return 1;
            u32 mode = animationMode(resource);
            controller = mModel.mpBtkAnm;
            if (!demoBtkInit(controller, data, resource, 1u, mode, 0u, -1, 0u, 0u, speed))
                return 0;
            modelFlags |= ((u32)mModel.mID.mBtkID & 0x10000000) ? 0x1200 : 0x200;
        }
        if ((u32)mModel.mID.mBrkID != -1u) {
            void* controller = demoBrkConstruct(nullptr);
            mModel.mpBrkAnm = demoView<J3DFrameCtrl>(controller);
            if (!controller)
                return 0;
            void* resource = demoResource((u16)(u32)mModel.mID.mBrkID);
            if (!resource)
                return 1;
            u32 mode = animationMode(resource);
            controller = mModel.mpBrkAnm;
            if (!demoBrkInit(controller, data, resource, 1u, mode, 0u, -1, 0u, 0u, speed))
                return 0;
        }
        if ((u32)mModel.mID.mBckID == -1u) {
            mModel.mpMorf = nullptr;
            DemoModelView* model =
                demoView<DemoModelView>(demoModelCreate(data, 0x80000u, modelFlags));
            mModel.mpModel = demoView<DemoModelView>(model);
            if (!model)
                return 0;
            gabi::Local<DemoString> name, first, second, third;
            initString(first.get(), "blackfadebox");
            initString(second.get(), "black_plane");
            initString(third.get(), "mat_black_box");
            DemoMetadataView* metadata = demoView<DemoMetadataView>(demoGetModelMetadata(data));
            u32 relative = metadata->mNameOffset;
            initString(name.get(), relative ? gabi::ea(metadata) + 4 + relative : 0);
            bool special = equalStrings(first.get(), name.get(), 2);
            if (!special)
                special = equalStrings(second.get(), name.get(), 2);
            if (!special)
                special = equalStrings(third.get(), name.get(), 2);
            if (special) {
                model = mModel.mpModel;
                u32 flags = (u32)model->mFlags & ~1u;
                model->mFlags = flags;
                demoModelSetFlags(model, flags);
            }
        } else {
            void* resource = demoResource((u16)(u32)mModel.mID.mBckID);
            if (!resource)
                demoAssert("d_a_demo00.cpp", 0x1A9u, "anm != (0)");
            DemoMorfView* morf =
                demoMorfConstruct(nullptr, data, nullptr, nullptr, resource, -1, nullptr, -1, 1u,
                                  nullptr, 0x80000u, modelFlags, speed);
            mModel.mpMorf = demoView<DemoMorfView>(morf);
            if (!morf || !(u32)gabi::ea(morf->mpModel.get()))
                return 0;
            mModel.mpModel = demoView<DemoModelView>(morf->mpModel.get());
            gabi::Local<DemoString> modelName, expected;
            initString(expected.get(), "fn_comp");
            DemoMetadataView* metadata = demoView<DemoMetadataView>(demoGetModelMetadata(data));
            u32 relative = metadata->mNameOffset;
            initString(modelName.get(), relative ? gabi::ea(metadata) + 4 + relative : 0);
            bool auxiliary = equalStrings(expected.get(), modelName.get(), 2);
            if (auxiliary) {
                auxiliary = false;
                for (const char* text :
                     {"Demo07", "Demo18", "Demo19", "Demo20", "Demo24", "Demo26", "Demo33"}) {
                    if (demoArchiveEquals(text)) {
                        auxiliary = true;
                        break;
                    }
                }
            }
            if (auxiliary) {
                void* object = operator_new(0x78u);
                if (object)
                    demoBtkConstruct(&demoView<DemoAuxiliaryView>(object)->mFrameCtrl);
                mModel.mpAuxiliary = demoView<DemoAuxiliaryView>(object);
                if (!object)
                    return 0;
                void* auxiliaryData = namedDemoResource("Always", "ship_minamo00.bdl");
                object = mModel.mpAuxiliary;
                void* auxiliaryModel = demoModelCreate(auxiliaryData, 0u, 0x11020203u);
                demoView<DemoAuxiliaryView>(object)->mpModel =
                    demoView<DemoModelView>(auxiliaryModel);
                object = mModel.mpAuxiliary;
                if (!(u32)gabi::ea(demoView<DemoAuxiliaryView>(object)->mpModel.get()))
                    return 0;
                void* auxiliaryAnimation = namedDemoResource("Always", "ship_minamo00.btk");
                object = mModel.mpAuxiliary;
                if (!demoBtkInit(&demoView<DemoAuxiliaryView>(object)->mFrameCtrl, auxiliaryData,
                                 auxiliaryAnimation, 1u, 2u, 0u, -1, 0u, 0u, speed))
                    return 0;
            }
            DemoModelView* model = mModel.mpModel;
            void* modelData = model->mpModelData.get();
            DemoTextureView* textures =
                demoView<DemoTextureView>(demoView<DemoModelDataView>(modelData)->mpTextures.get());
            if (textures) {
                void* names = demoView<DemoModelDataView>(modelData)->mpTextureNames.get();
                if (names) {
                    for (u16 index = 0; index < (u16)textures->mCount; ++index) {
                        void* text = demoGetTextureName(names, index);
                        if (!text)
                            continue;
                        bool framebuffer =
                            equalChars(gabi::ea(text), gabi::guest_addr("B_dummy"), -1u);
                        if (!framebuffer &&
                            !equalChars(gabi::ea(text), gabi::guest_addr("cy_kankyo"), -1u))
                            continue;
                        model->mFramebufferTexture = 1;
                        if (!equalChars(gabi::ea(text), gabi::guest_addr("B_dummy"), -1u))
                            continue;
                        void* system = l_graphicsSystem;
                        void* framebufferData = demoGetFramebufferData(system, 6u);
                        void* image = operator_new(0xC0u);
                        image = demoImageConstruct(image, framebufferData);
                        DemoTextureDescriptor* descriptor = &textureScratch->descriptor;
                        // The image pointer is kept in descriptor word 8 (frame +0x38) and is
                        // copied with the descriptor; the final destination+0x20 store reloads it
                        // from there.
                        descriptor->words[8] = gabi::ea(image);
                        demoView<DemoDescriptorView>(descriptor)->mWidth =
                            (u16)(u32)demoView<DemoImageView>(image)->mWidth;
                        u32 height = demoView<DemoImageView>(image)->mHeight;
                        demoView<DemoDescriptorView>(descriptor)->mWrap = 0;
                        demoView<DemoDescriptorView>(descriptor)->mHeight = (u16)height;
                        void* destination =
                            ptr((u32)gabi::ea(textures->mpDescriptors.get()) + (u32)index * 36);
                        for (u32 i = 0; i < 9; ++i)
                            demoView<DemoTextureDescriptor>(destination)->words[i] =
                                (u32)descriptor->words[i];
                        destination =
                            ptr((u32)gabi::ea(textures->mpDescriptors.get()) + (u32)index * 36);
                        demoView<DemoDescriptorView>(destination)->mImageOffset =
                            (u32)demoView<DemoDescriptorView>(destination)->mImageOffset +
                            gabi::ea(descriptor) - gabi::ea(destination);
                        destination =
                            ptr((u32)gabi::ea(textures->mpDescriptors.get()) + (u32)index * 36);
                        demoView<DemoDescriptorView>(destination)->mPaletteOffset =
                            (u32)demoView<DemoDescriptorView>(destination)->mPaletteOffset +
                            gabi::ea(descriptor) - gabi::ea(destination);
                        destination =
                            ptr((u32)gabi::ea(textures->mpDescriptors.get()) + (u32)index * 36);
                        demoView<DemoDescriptorView>(destination)->mImage =
                            (u32)descriptor->words[8];
                        u32 a = textures->mMaskHigh, b = textures->mMaskLow;
                        if (index < 64) {
                            u32 mask = shiftLeft(a, index) | shiftRight(b, 32 - index) |
                                       shiftLeft(b, index + 32);
                            u32 low = textures->mFlagsLow, high = textures->mFlagsHigh;
                            textures->mFlagsHigh = high | mask;
                            textures->mFlagsLow = low | shiftLeft(b, index);
                        } else {
                            u32 n = index - 64;
                            u32 low = textures->mExtendedLow, high = textures->mExtendedHigh;
                            u32 mask =
                                shiftLeft(a, n) | shiftRight(b, 32 - n) | shiftLeft(b, n + 32);
                            textures->mExtendedLow = low | shiftLeft(b, n);
                            textures->mExtendedHigh = high | mask;
                        }
                    }
                }
            }
        }
        if ((u8)mDrawMode == DEMO_DRAW_INVISIBLE) {
            void* invisible = demoInvisibleConstruct(nullptr);
            mModel.mpInvisibleModel = invisible;
            if (!invisible)
                return 0;
            DemoModelView* model = mModel.mpModel;
            if (!demoInvisibleCreate(invisible, model))
                return 0;
        } else
            mModel.mpInvisibleModel = nullptr;
        daDemo00_bgc_c* background = demoView<daDemo00_bgc_c>(operator_new(0x58u));
        if (background) {
            demoGroundConstruct(background);
            background->mGndChk.mbObjThrough = 0;
            background->mGndChk.mbLinkThrough = 0;
            background->mGndChk.mbArrowThrough = 0;
            background->mGndChk.mbBombThrough = 0;
            background->mGndChk.mGrpPassChk = gabi::ea(background) + 0x4C;
            static_cast<dBgS_PolyPassChk&>(background->mGndChk).__vtbl = gc_tu.gndChk.v40;
            background->mGndChk.mbBoomerangThrough = 0;
            background->mGndChk.mbCamThrough = 0;
            background->mGndChk.mGrp = 1;
            static_cast<cBgS_PolyInfo&>(background->mGndChk).__vtbl = gc_tu.gndChk.v20;
            background->mGndChk.mbRopeThrough = 0;
            background->mGndChk.mPolyPassChk = gabi::ea(background) + 0x40;
            static_cast<cBgS_Chk&>(background->mGndChk).__vtbl = gc_tu.gndChk.v10;
            static_cast<dBgS_GrpPassChk&>(background->mGndChk).__vtbl = gc_tu.gndChk.v4C;
        }
        mModel.mBgc = demoView<daDemo00_bgc_c>(background);
        if (!background)
            return 0;
        background->mGndChk.mFlag = (u32)background->mGndChk.mFlag & ~2u;
    }
    if ((u32)mModel.mID.mPlightID != -1u) {
        void* light = demo00_lightConstructor(nullptr);
        mModel.mpPlight = demoView<DemoLightView>(light);
        if (!light)
            return 0;
        u32 delay = demoArchiveEquals("Demo24") ? 60 : 0;
        u32 id = mModel.mID.mPlightID;
        light = mModel.mpPlight;
        u8 type = l_lightData.mValues[id].mType;
        demoLightInit(light, &current.pos, id, type, delay);
    }
    return 1;
}

/* 02120244 */
static s32 createHeapCallBack(daDemo00_c* actor) {
    return (actor)->createHeap();
}

/* 0212070C */
s32 daDemo00_c::draw() {
    DemoModelView* model = mModel.mpModel;
    if (!model)
        return 1;
    void* environment = dKy_getEnvlight();
    demoSettingTevStruct(environment, 0, &current.pos, &tevStr);
    environment = dKy_getEnvlight();
    model = mModel.mpModel;
    demoSetLightTevColor(environment, model, &tevStr);

    if (mModel.mpBtpAnm) {
        bool skipPattern = false;
        if (demoArchiveEquals("Demo46")) {
            for (const char* literal : {"aj", "ji"}) {
                model = mModel.mpModel;
                DemoMetadataView* metadata = demoView<DemoMetadataView>(model->mpMetadata.get());
                u32 relative = metadata->mNameOffset;
                gabi::Local<DemoString> name, expected;
                initString(name, relative ? gabi::ea(metadata) + 4 + relative : 0);
                initString(expected, literal);
                demo00_stringCallback(expected);
                if (equalStrings(expected, name, 1)) {
                    skipPattern = true;
                    break;
                }
            }
        }
        if (!skipPattern) {
            void* controller = mModel.mpBtpAnm;
            s16 frame = (s16)gabi::ftoi((f32)demoView<J3DFrameCtrl>(controller)->mFrame);
            model = mModel.mpModel;
            demoBtpEntry(controller, model->mpModelData.get(), frame);
        }
    }
    if (mModel.mpBtkAnm) {
        void* controller = mModel.mpBtkAnm;
        model = mModel.mpModel;
        f32 frame = demoView<J3DFrameCtrl>(controller)->mFrame;
        demoBtkEntry(controller, model->mpModelData.get(), frame);
    }
    if (mModel.mpBrkAnm) {
        void* controller = mModel.mpBrkAnm;
        model = mModel.mpModel;
        f32 frame = demoView<J3DFrameCtrl>(controller)->mFrame;
        demoBrkEntry(controller, model->mpModelData.get(), frame);
    }
    model = mModel.mpModel;
    if (model->mFramebufferTexture) {
        for (u32 i = 0; i < (u16)model->mMaterialCount; ++i) {
            void* packet = ptr((u32)gabi::ea(model->mpMaterialPackets.get()) + i * 0x3C);
            for (u32 slot = 0; slot != 8; ++slot)
                demoPrepareTextureSlot(packet, slot);
            model = mModel.mpModel;
        }
    }
    void* invisible = mModel.mpInvisibleModel;
    if (invisible) {
        DemoMorfView* morf = mModel.mpMorf;
        if (morf)
            demoInvisibleMorfEntry(invisible, morf);
        else
            demoInvisibleModelEntry(invisible, model);
    } else {
        bool specialList = false;
        if (model->mFramebufferTexture) {
            gabi::Local<DemoString> expected, stage;
            initString(expected, "GTower");
            void* game = dComIfGp_get();
            initString(stage, gabi::ea(game) + 0x5134);
            specialList = equalStrings(expected, stage, 2);
        }
        auto selectLists = [](be<u32> DemoDrawListsView::* first,
                              be<u32> DemoDrawListsView::* second) {
            void* game = dComIfGp_get();
            l_opaDrawBuffer = demoView<DemoDrawListsView>(game)->*first;
            game = dComIfGp_get();
            l_xluDrawBuffer = demoView<DemoDrawListsView>(game)->*second;
        };
        if (specialList)
            selectLists(&DemoDrawListsView::mInvisibleOpa, &DemoDrawListsView::mInvisibleXlu);
        u8 mode = mDrawMode;
        if (mode == DEMO_DRAW_MASK_OFF)
            selectLists(&DemoDrawListsView::mMaskOffOpa, &DemoDrawListsView::mMaskOffXlu);
        else if (mode == DEMO_DRAW_P1)
            selectLists(&DemoDrawListsView::mP1Opa, &DemoDrawListsView::mP1Xlu);
        DemoMorfView* morf = mModel.mpMorf;
        if (morf) {
            demoMorfUpdateDL(morf);
            if (mModel.mpAuxiliary)
                demoDrawAuxiliaryNative(this);
        } else {
            model = mModel.mpModel;
            demoModelUpdateDL(model, 0);
        }
        model = mModel.mpModel;
        if (model->mFramebufferTexture || (u8)mDrawMode == DEMO_DRAW_MASK_OFF ||
            (u8)mDrawMode == DEMO_DRAW_P1)
            selectLists(&DemoDrawListsView::mOpa, &DemoDrawListsView::mXlu);
    }
    if (mModel.mpBtpAnm) {
        model = mModel.mpModel;
        demoView<DemoModelDataView>(model->mpModelData.get())->mBtpAnimation = 0;
    }
    if (mModel.mpBtkAnm) {
        model = mModel.mpModel;
        demoView<DemoModelDataView>(model->mpModelData.get())->mBtkAnimation = 0;
    }
    if (mModel.mpBrkAnm) {
        model = mModel.mpModel;
        demoView<DemoModelDataView>(model->mpModelData.get())->mBrkAnimation = 0;
    }
    return 1;
}

/* 02120248 */
void daDemo00_c::drawAuxiliary() {
    DemoMorfView* morf = mModel.mpMorf;
    morf->mFlags = (u32)morf->mFlags | 8;
    void* joint = morf->mpJoint.get();
    f32 height = l_zero;
    if (demoSeaContains((f32)current.pos.x, (f32)current.pos.z))
        height = demoSeaHeight((f32)current.pos.x, (f32)current.pos.z);
    if (demoArchiveEquals("Demo07"))
        height = l_demoSeaHeightA;
    else if (demoArchiveEquals("Demo26"))
        height = l_demoSeaHeightB;
    void* matrix = &l_matrix;
    PSMTXTrans(static_cast<Mtx34*>(matrix),
               (f32)((f32)current.pos.x + (f32)demoView<DemoJointView>(joint)->mTranslation.x),
               (f32)(height + (f32)l_maxPatternFrame),
               (f32)((f32)current.pos.z + (f32)demoView<DemoJointView>(joint)->mTranslation.z));
    f32 angle = (f32)((f32)demoView<DemoJointView>(joint)->mYaw + (f32)l_auxiliaryYawBias);
    s16 turn = (s16)gabi::ftoi((f32)(angle * (f32)l_angleScale));
    s16 yaw = shape_angle.y;
    mDoMtx_YrotM(static_cast<Mtx34*>(matrix), (s16)(yaw + turn));
    void* auxiliary = mModel.mpAuxiliary;
    DemoModelView* model =
        demoView<DemoModelView>(demoView<DemoAuxiliaryView>(auxiliary)->mpModel.get());
    f32 transform[12];
    for (u32 i = 0; i != 12; ++i)
        transform[i] = static_cast<Mtx34*>(matrix)->m[i / 4][i % 4];
    for (u32 i = 0; i != 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = transform[i];
    auxiliary = mModel.mpAuxiliary;
    model = demoView<DemoModelView>(demoView<DemoAuxiliaryView>(auxiliary)->mpModel.get());
    void* data = model->mpModelData.get();
    DemoMaterialView* material =
        demoView<DemoMaterialView>(demoView<DemoModelDataView>(data)->mpMaterials.get());
    gabi::Local<be<u32>> color;
    gabi::Local<be<f32>> ignored;
    demoGetWaterColor(&*color, &*ignored);
    f32 delta = (f32)(height - (f32)current.pos.y);
    s32 alpha = (s32)(255u - (u32)gabi::ftoi((f32)(delta + delta)));
    if (alpha < 0)
        alpha = 0;
    else if (alpha > 255)
        alpha = 255;
    struct SignedColor {
        be<s16> r, g, b, a;
    };
    gabi::Local<SignedColor> channels;
    channels->r = demoView<GXColor>(color.get())->r;
    channels->g = demoView<GXColor>(color.get())->g;
    channels->b = demoView<GXColor>(color.get())->b;
    channels->a = alpha;
    void* environment = dKy_getEnvlight();
    f32 exposure = demoView<DemoExposureView>(environment)->mExposure;
    void* colorBlock = material->mpColorBlock.get();
    DemoColorBlockVtable* table = demoView<DemoColorBlock>(colorBlock)->mpVtable;
    gabi::call_ptr((u32)table->mMethods[9], colorBlock, 1, &*channels);
    struct FloatColor {
        be<f32> r, g, b, a;
    };
    gabi::Local<FloatColor> normalized, adjusted;
    f32 maximum = l_colorMaximum;
    normalized->r = (f32)((s16)channels->r / maximum);
    normalized->g = (f32)((s16)channels->g / maximum);
    normalized->b = (f32)((s16)channels->b / maximum);
    normalized->a = (f32)((s16)channels->a / maximum);
    demoApplyExposure(&*adjusted, &*normalized, exposure);
    material->mFlags = (u32)material->mFlags | 0x20;
    void* destination = demoMaterialColor(&material->mFlags, 5);
    f32 red = adjusted->r, green = adjusted->g, blue = adjusted->b;
    demoView<FloatColor>(destination)->g = green;
    demoView<FloatColor>(destination)->b = blue;
    demoView<FloatColor>(destination)->r = red;
    demoView<FloatColor>(destination)->a = (f32)((s16)channels->a / maximum);
    auxiliary = mModel.mpAuxiliary;
    f32 frame = demoView<DemoAuxiliaryView>(auxiliary)->mFrameCtrl.mFrame;
    demoBtkEntry(&demoView<DemoAuxiliaryView>(auxiliary)->mFrameCtrl, data, frame);
    auxiliary = mModel.mpAuxiliary;
    demoModelUpdateDL(demoView<DemoAuxiliaryView>(auxiliary)->mpModel.get(), 0);
}

/* 02120CBC */
s32 daDemo00_c::execute() {
    u8 actorID = demoActorID;
    mGroundValid = 0;
    void* demoActor = nullptr;
    if (actorID && actorID <= 32) {
        void* manager = l_demoManager;
        if (!manager) {
            demoAssert("d_demo.h", 0x23A, "m_object != (0)");
            manager = l_demoManager;
        }
        demoActor = demoGetActor(manager, actorID);
    }
    if (!demoActor) {
        fopAcM_delete(this);
        return 1;
    }
    auto enabled = [demoActor](u16 bit) {
        return ((u16)demoView<dDemo_actor_c>(demoActor)->mFlags & bit) != 0;
    };
    if (enabled(dDemo_actor_c::ENABLE_SHAPE_e))
        mNextID.mShapeID = demoView<dDemo_actor_c>(demoActor)->mShapeId;
    if (enabled(dDemo_actor_c::ENABLE_ANM_e)) {
        u32 animation = demoView<dDemo_actor_c>(demoActor)->mAnmId;
        if ((s16)animation != 26 || !demoArchiveEquals("Demo19"))
            mNextID.mBckID = demoView<dDemo_actor_c>(demoActor)->mAnmId;
    }
    if (enabled(dDemo_actor_c::ENABLE_UNK_e)) {
        u8 oldMode = mDrawMode;
        u8 mode = (u8)(u32)demoView<dDemo_actor_c>(demoActor)->mPrm.mId;
        mDrawMode = mode;
        struct ParsedData {
            be<u32> words[5];
        };
        gabi::Local<be<u32>> parameter;
        gabi::Local<ParsedData> parsed;
        *parameter = gabi::ea(demoView<dDemo_actor_c>(demoActor)->mPrm.mData.get());
        demoParseParameter(&*parameter, &*parsed);
        u8 type = *reinterpret_cast<be<u8>*>(parsed.get());
        u32 begin = parsed->words[3], valid = parsed->words[4];
        u8 expectedType =
            (mode == DEMO_MONOTONE || mode == DEMO_FADE_WHITE || mode == DEMO_FADE_BLACK) ? 33
            : (mode == DEMO_EVENT_BIT || mode == DEMO_ITEM || mode == DEMO_VIBRATION)     ? 49
                                                                                          : 51;
        if (type && begin && type == expectedType && valid) {
            if (mode == DEMO_EVENT_BIT) {
                u8 index = byte(begin);
                if (index >= 50)
                    demoAssert("d_a_demo00.cpp", 0x4AA, "argID < (sizeof(l_eventBit)/sizeof(u16))");
                u16 event = l_eventBits.mValues[index];
                if (event != 0xFFFF) {
                    void* save = l_saveInfo;
                    demoSetEventBit(
                        gabi::at<dSv_event_c>(gabi::ea(save) +
                                              offsetof(DemoSaveView, mInfo.mSavedata.mEvent)),
                        event);
                }
            } else if (mode == DEMO_ITEM) {
                u8 index = byte(begin);
                if (index >= 10)
                    demoAssert("d_a_demo00.cpp", 0x4C4, "argID < (sizeof(l_itemNo)/sizeof(u8))");
                u8 item = l_itemNumbers.mValues[index];
                if (item != 255)
                    demoSetItem(item);
            } else if (mode == DEMO_MONOTONE) {
                l_monotone = (s8)byte(begin);
            } else if (mode == DEMO_VIBRATION) {
                u8 strength = byte(begin);
                f32 zero = l_zero, one = l_one;
                void* game = dComIfGp_get();
                if (strength == 255)
                    demoStopQuake(gabi::at<dVibration_c>(gabi::ea(game) +
                                                         offsetof(dComIfG_inf_c, play.mVibration)),
                                  1);
                else {
                    gabi::Local<cXyz> direction;
                    direction->x = zero;
                    direction->y = one;
                    direction->z = zero;
                    if (strength < 100)
                        demoStartShock(
                            gabi::at<dVibration_c>(gabi::ea(game) +
                                                   offsetof(dComIfG_inf_c, play.mVibration)),
                            strength, 1, &*direction);
                    else
                        demoStartQuake(
                            gabi::at<dVibration_c>(gabi::ea(game) +
                                                   offsetof(dComIfG_inf_c, play.mVibration)),
                            strength - 100, 1, &*direction);
                }
            } else if (mode == DEMO_FADE_WHITE || mode == DEMO_FADE_BLACK) {
                u32 end = begin + (u32)parsed->words[2];
                if (begin != end) {
                    s8 fade = byte(begin);
                    if ((u8)mDrawMode != oldMode || fade != (s8)mPreviousDrawMode) {
                        mPreviousDrawMode = fade;
                        s8 speed = begin + 1 == end ? 0 : (s8)byte(begin + 1);
                        if (!fade)
                            demoFadeIn(speed);
                        else
                            demoFadeOut(speed, 0);
                        GXColor* color =
                            (u8)mDrawMode == DEMO_FADE_WHITE ? &l_fadeWhite : &l_fadeBlack;
                        demoSetFadeColor(color);
                    }
                }
            } else {
                u32 end = begin + (u32)parsed->words[2] * 4;
                s32 selector = -1;
                for (u32 entry = begin; entry != end; entry += 4) {
                    u32 value = word(ptr(entry), 0);
                    if (selector < 0) {
                        selector = value;
                        continue;
                    }
                    switch (selector) {
                    case DEMO_BTP:
                        mNextID.mBtpID = value;
                        break;
                    case DEMO_BTK:
                        mNextID.mBtkID = value;
                        break;
                    case DEMO_PLIGHT_RESOURCE:
                        mNextID.mPlightID = value;
                        break;
                    case DEMO_BRK:
                        mNextID.mBrkID = value;
                        break;
                    case DEMO_BTK_LOOP:
                        mNextID.mBtkID = value | 0x10000000;
                        break;
                    case DEMO_BRK_LOOP:
                        mNextID.mBrkID = value | 0x10000000;
                        break;
                    }
                    selector = -1;
                }
            }
        }
    }
    invokeAction(this, demoActor);
    return 1;
}

/* 02121390 */
s32 daDemo00_Execute(daDemo00_c* actor) {
    return actor->execute();
}

namespace {
u16 demoEnableBits(void* demoActor) {
    return demoView<dDemo_actor_c>(demoActor)->mFlags;
}
void clearDemoFrameEnable(void* demoActor) {
    demoView<dDemo_actor_c>(demoActor)->mFlags =
        demoEnableBits(demoActor) & ~dDemo_actor_c::ENABLE_ANM_FRAME_e;
}

// HD: these scene-specific adjustments use the native actor path.
void adjustDemoPosition(daDemo00_c* actor, void* demoActor, f32 threshold, f32 amplitude) {
    s8 index = actor->argument;
    if (index == 10 && demoArchiveEquals("Demo46")) {
        actor->current.pos.x = (f32)((f32)actor->current.pos.x + l_demo46OffsetX);
        actor->current.pos.y = (f32)((f32)actor->current.pos.y + amplitude);
        actor->current.pos.z = (f32)((f32)actor->current.pos.z + l_demo46OffsetZ);
        actor->setBaseMtx();
        return;
    }
    index = actor->argument;
    if (index == 0 && (u16)(u32)actor->mModel.mID.mBckID == 3 && demoArchiveEquals("Demo44")) {
        s16 phase = l_demoPhase;
        phase = (s16)(phase + 0x320);
        l_demoPhase = phase;
        f32 cosine = cM_scos(phase);
        actor->current.pos.y = gabi::fmadds(amplitude, cosine, l_demo44Height);
        f32 sine = cM_ssin(phase);
        s16 roll = (s16)gabi::ftoi((f32)(l_demo44Roll * sine));
        actor->shape_angle.z = roll - 0x16A;
        clearDemoFrameEnable(demoActor);
        actor->setBaseMtx();
        return;
    }
    index = actor->argument;
    if ((u32)((s32)index - 1) < 4 && demoArchiveEquals("Demo17")) {
        index = actor->argument;
        u32 frame = l_demoFrame;
        if (index == 3 || index == 4) {
            if (frame - 120 < 0x38F) {
                actor->current.pos.y = (f32)((f32)actor->current.pos.y - l_demo17OffsetY);
                actor->current.pos.z = (f32)((f32)actor->current.pos.z + l_demo17OffsetZ);
                if ((s8)actor->argument == 3 && frame - 908 < 0x7B)
                    actor->current.pos.x = (f32)((f32)actor->current.pos.x - threshold);
            } else if (frame - 1620 < 0x5D) {
                const char* actorName = index == 3 ? "P1b" : "P1c";
                void* other = demoFindActorByName(actorName, 0, 0);
                if (other) {
                    DemoMorfView* morf =
                        demoView<DemoMorfView>(demoView<DemoOtherActor>(other)->mpMorf.get());
                    DemoModelView* model = demoView<DemoModelView>(morf->mpModel.get());
                    J3DMtxBuffer_hd* matrices = demoView<J3DMtxBuffer_hd>(model->mpMtxBuffer.get());
                    matrices->mFlags = (u16)matrices->mFlags | 0x10;
                    void* transforms = ptr(matrices->mpAnmMtx);
                    gabi::Local<cXyz> jointPosition, translated;
                    jointPosition->x = static_cast<Mtx34*>(transforms)[11].m[0][3];
                    jointPosition->y = static_cast<Mtx34*>(transforms)[11].m[1][3];
                    jointPosition->z = static_cast<Mtx34*>(transforms)[11].m[2][3];
                    cXyz_pl(&*jointPosition, &*translated, &l_jointOffset);
                    actor->current.pos = *translated;
                }
            }
        } else if (index == 2 && frame - 1044 < 0xA7) {
            actor->current.pos.x = (f32)((f32)actor->current.pos.x - l_demo17OffsetX);
        } else if (index == 1 && frame - 1620 < 0x5D) {
            clearDemoFrameEnable(demoActor);
            DemoMorfView* morf = actor->mModel.mpMorf;
            morf->mFrameCtrl.mRate = l_demo17PlaySpeed;
        }
    }
    actor->setBaseMtx();
}
} // namespace

/* 021219B8 */
s32 daDemo00_c::actPerformance(void* demoActor) {
    if ((u32)mModel.mID.mShapeID != (u32)mNextID.mShapeID ||
        (u32)mModel.mID.mPlightID != (u32)mNextID.mPlightID) {
        mModel.reset();
        setAction(this, 0x02122804);
        return 1;
    }
    if (!mModel.mpModel) {
        if (mModel.mpPlight) {
            demoSetActorData(this, 2, 0, 0, 0, 0, 0, 0);
            demoLightSetPosition(mModel.mpPlight.get(), &current.pos);
        }
        return 1;
    }
    f32 one = l_one;
    if (mModel.mpMorf && (u32)mModel.mID.mBckID != (u32)mNextID.mBckID) {
        void* animation = demoResource((u16)(u32)mNextID.mBckID);
        if (!animation)
            return 1;
        f32 zero = l_zero;
        f32 transition = demoEnableBits(demoActor) & 0x80
                             ? (f32)demoView<dDemo_actor_c>(demoActor)->mAnmTransition
                             : zero;
        demoMorfSetAnimation(mModel.mpMorf.get(), animation, -1, 0, transition, one, zero,
                             l_animationEnd);
        mModel.mID.mBckID = mNextID.mBckID;
        l_demoPhase = 0;
    }
    if ((u32)mModel.mID.mBtpID != (u32)mNextID.mBtpID) {
        void* animation = demoResource((u16)(u32)mNextID.mBtpID);
        if (!animation)
            return 1;
        DemoModelView* model = mModel.mpModel;
        u32 mode = animationMode(animation);
        demoBtpInit(mModel.mpBtpAnm.get(), model->mpModelData.get(), animation, 1, mode, 0, -1, 1,
                    0, one);
        mModel.mID.mBtpID = mNextID.mBtpID;
    }
    if ((u32)mModel.mID.mBtkID != (u32)mNextID.mBtkID) {
        if (demoArchiveEquals("Demo18") && (u32)mNextID.mBtkID == 0x1003C)
            mNextID.mBtkID = 0x10021;
        void* animation = demoResource((u16)(u32)mNextID.mBtkID);
        if (!animation)
            return 1;
        bool loop = ((u32)mModel.mID.mBtkID & 0x10000000) != 0;
        DemoModelView* model = mModel.mpModel;
        u32 mode = loop ? 2 : animationMode(animation);
        void* controller = mModel.mpBtkAnm;
        loop = ((u32)mModel.mID.mBtkID & 0x10000000) != 0;
        s16 frame = loop ? (s16)gabi::ftoi((f32)demoView<J3DFrameCtrl>(controller)->mFrame) : 0;
        demoBtkInit(controller, model->mpModelData.get(), animation, 1, mode, frame, -1, 1, 0, one);
        mModel.mID.mBtkID = mNextID.mBtkID;
    }
    if ((u32)mModel.mID.mBrkID != (u32)mNextID.mBrkID) {
        void* animation = demoResource((u16)(u32)mNextID.mBrkID);
        if (!animation)
            return 1;
        bool loop = ((u32)mModel.mID.mBrkID & 0x10000000) != 0;
        DemoModelView* model = mModel.mpModel;
        u32 mode = loop ? 2 : animationMode(animation);
        void* controller = mModel.mpBrkAnm;
        loop = ((u32)mModel.mID.mBrkID & 0x10000000) != 0;
        s16 frame = loop ? (s16)gabi::ftoi((f32)demoView<J3DFrameCtrl>(controller)->mFrame) : 0;
        demoBrkInit(controller, model->mpModelData.get(), animation, 1, mode, frame, -1, 1, 0, one);
        mModel.mID.mBrkID = mNextID.mBrkID;
    }
    f32 preservedX = old.pos.x, preservedY = old.pos.y, preservedZ = old.pos.z;
    demoSetActorData(this, 0x2A, 0, 0, 0, 0, 0, 0);
    daDemo00_bgc_c* background = mModel.mBgc;
    old.pos.y = preservedY;
    old.pos.x = preservedX;
    old.pos.z = preservedZ;
    if (background) {
        f32 x = current.pos.x;
        f32 y = (f32)((f32)current.pos.y + l_groundCheckHeight);
        f32 z = current.pos.z;
        background->mGndChk.m_pos.x = x;
        background->mGndChk.m_pos.y = y;
        background->mGndChk.m_pos.z = z;
        void* game = dComIfGp_get();
        background = mModel.mBgc;
        f32 ground = demoGroundCross(
            gabi::at<dBgS>(gabi::ea(game) + offsetof(dComIfG_inf_c, play.mBgS)), background);
        background->mGroundY = ground;
        mGroundValid = 1;
    }
    f32 threshold = l_groundSoundDistance, amplitude = l_demoHeightAmplitude;
    adjustDemoPosition(this, demoActor, threshold, amplitude);
    if (demoEnableBits(demoActor) & 0x40) {
        bool automatic = false;
        f32 frame = demoView<dDemo_actor_c>(demoActor)->mAnmFrame;
        if ((s8)argument == 1 && (u16)(u32)mModel.mID.mBckID == 31 && (u32)l_demoFrame < 0x398)
            automatic = demoArchiveEquals("Demo32");
        if (frame > one || automatic) {
            frame = (f32)(frame - one);
            DemoMorfView* morf = mModel.mpMorf;
            if (morf) {
                if (!automatic)
                    morf->mFrameCtrl.mFrame = (f32)(s16)gabi::ftoi(frame);
                u32 sound = 0;
                background = mModel.mBgc;
                if (background && (u8)mGroundValid &&
                    std::fabs((f32)((f32)background->mGroundY - (f32)current.pos.y)) < threshold) {
                    void* game = dComIfGp_get();
                    background = mModel.mBgc;
                    sound = demoGroundSoundID(
                        gabi::at<dBgS>(gabi::ea(game) + offsetof(dComIfG_inf_c, play.mBgS)),
                        background ? static_cast<cBgS_PolyInfo*>(&background->mGndChk) : nullptr);
                }
                s8 room = l_stayRoom;
                s32 reverb = dComIfGp_getReverb(room);
                demoMorfPlay(mModel.mpMorf.get(), &current.pos, sound, reverb);
                if (automatic)
                    frame = mModel.mpMorf->mFrameCtrl.mFrame;
            }
            void* controller = mModel.mpBtpAnm;
            if (controller) {
                demoView<J3DFrameCtrl>(controller)->mFrame = frame;
                controller = mModel.mpBtpAnm;
                demoAnimationPlay(controller);
            }
            controller = mModel.mpBtkAnm;
            if (controller) {
                if (!((u32)mModel.mID.mBtkID & 0x10000000)) {
                    demoView<J3DFrameCtrl>(controller)->mFrame = frame;
                    controller = mModel.mpBtkAnm;
                }
                demoAnimationPlay(controller);
            }
            controller = mModel.mpBrkAnm;
            if (controller) {
                if (!((u32)mModel.mID.mBrkID & 0x10000000)) {
                    demoView<J3DFrameCtrl>(controller)->mFrame = frame;
                    controller = mModel.mpBrkAnm;
                }
                demoAnimationPlay(controller);
            }
        } else {
            DemoMorfView* morf = mModel.mpMorf;
            if (morf)
                morf->mFrameCtrl.mFrame = (f32)(s16)gabi::ftoi(frame);
            void* controller = mModel.mpBtpAnm;
            if (controller)
                demoView<J3DFrameCtrl>(controller)->mFrame = frame;
            controller = mModel.mpBtkAnm;
            if (controller) {
                if ((u32)mModel.mID.mBtkID & 0x10000000)
                    demoAnimationPlay(controller);
                else
                    demoView<J3DFrameCtrl>(controller)->mFrame = frame;
            }
            controller = mModel.mpBrkAnm;
            if (controller) {
                if ((u32)mModel.mID.mBrkID & 0x10000000)
                    demoAnimationPlay(controller);
                else
                    demoView<J3DFrameCtrl>(controller)->mFrame = frame;
            }
        }
    } else {
        void* controller = mModel.mpMorf;
        if (controller)
            demoMorfPlay(controller, &current.pos, 0, 0);
        else if ((controller = mModel.mpBtpAnm))
            demoAnimationPlay(controller);
        else if ((controller = mModel.mpBtkAnm))
            demoAnimationPlay(controller);
        else if ((controller = mModel.mpBrkAnm))
            demoAnimationPlay(controller);
    }
    void* light = mModel.mpPlight;
    if (demoEnableBits(demoActor) & 4) {
        demoWords(&scale)->x = demoWords(&demoView<dDemo_actor_c>(demoActor)->mScale)->x;
        demoWords(&scale)->y = demoWords(&demoView<dDemo_actor_c>(demoActor)->mScale)->y;
        demoWords(&scale)->z = demoWords(&demoView<dDemo_actor_c>(demoActor)->mScale)->z;
    }
    if (light) {
        gabi::Local<cXyz> position;
        position->x = current.pos.x;
        position->y = current.pos.y;
        position->z = current.pos.z;
        void* definition = &l_lightData.mValues[(u32)mModel.mID.mPlightID];
        if (demoView<DemoLightDefinition>(definition)->mUseJoint == 1) {
            DemoModelView* model = mModel.mpModel;
            u32 name = gabi::ea(demoView<DemoLightDefinition>(definition)->mJointName.get());
            DemoMetadataView* metadata =
                demoView<DemoMetadataView>(demoGetJointMetadata(model->mpModelData.get()));
            u32 relative = metadata->mJointNamesOffset;
            void* names = relative ? offset(metadata, 0x10 + relative) : nullptr;
            s32 joint = demoFindJoint(names, ptr(name));
            model = mModel.mpModel;
            J3DMtxBuffer_hd* matrices = demoView<J3DMtxBuffer_hd>(model->mpMtxBuffer.get());
            u16 flags = matrices->mFlags;
            void* transform = ptr((u32)matrices->mpAnmMtx + (u32)joint * 48);
            matrices->mFlags = flags | 0x10;
            position->x = static_cast<Mtx34*>(transform)->m[0][3];
            position->y = static_cast<Mtx34*>(transform)->m[1][3];
            position->z = static_cast<Mtx34*>(transform)->m[2][3];
            light = mModel.mpPlight;
        }
        demoLightSetPosition(light, &*position);
    }
    if (mModel.mpAuxiliary)
        (this)->adjustPatternFrame();
    return 1;
}

// HD: compiler-generated constructors, deleting destructors and SafeString copies.

/* 0211F31C */
void* demo00_lightConstructor(void* light) {
    if (!light)
        light = operator_new(0x34u);
    if (light)
        demoView<DemoLightView>(light)->mPower = l_one;
    return light;
}

/* 021214C0 */
void demo00_destructor(daDemo00_c* actor, u32 flags) {
    if (actor) {
        void* heap = actor->heap;
        actor->__vtbl = l_actorVtable;
        if (heap) {
            DemoMorfView* morf = actor->mModel.mpMorf;
            if (morf)
                reinterpret_cast<mDoExt_McaMorf*>(morf)->stopZelAnime();
        }
        demoActorDestruct(actor, 0u);
        if (flags & 1)
            operator_delete(actor);
    }
}

/* 02122864 */
void demo00_staticInit() {
    l_headerZero.mValues[2] = 0;
    l_headerZero.mValues[0] = 0;
    l_headerZero.mValues[3] = 0;
    l_headerZero.mValues[1] = 0;
    demoRegisterDestructor(&l_destructorRecordA);
    l_headerPiValue = l_headerPi;
    l_headerPiHalfValue = l_headerPiHalf;
    demoHeaderConstructA(&l_headerFlagA);
    demoRegisterDestructor(&l_destructorRecordB);
    demoHeaderConstructB(&l_headerFlagB);
    demoRegisterDestructor(&l_destructorRecordC);
}

/* 021228F8 */
void demo00_cleanupDestructor(void* object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 02122914 */
void demo00_stringCallback(void* string) {}

// clang-format off
/* WWHD glue
data l_fadeWhite 101D5E94
data l_fadeBlack 101D5E9C
str 1000D6AC
str 1000D6B8 in 0212070C
str 1000D6C0
str 1000D6C3
str 1000D6C8
str 1000D738
str 1000D748
str 1000D764
str 1000D770
str 1000D778 in 0211F35C
str 1000D780 in 0211F35C
str 1000D788 in 0211F35C
str 1000D790
str 1000D798
str 1000D7A0 in 0211F35C
str 1000D7A8
str 1000D7B0
str 1000D7B8
str 1000D7C4
str 1000D7D4
str 1000D7E8
str 1000D7FC in 0211F35C
str 1000D80C
str 1000D820
str 1000D830
str 1000D860 in 02120248
str 1000D868 in 02120248
str 1000D870 in 02120CBC
str 1000D884
str 1000D8AC in 02120CBC
str 1000D8BC
str 1000D94C
str 1000D954 in 02121624
str 1000D998
str 1000D99C
str 1000D9A0 in 021219B8
str 1000D9A8 in 021219B8
str 1000D9B0
str 1000D9B8
str 1000D9C0 in 021219B8
ext 0200ECD4 f32 cLib_addCalc(be<f32>*,f32,f32,f32,f32)
data l_one 1000D6A8
data l_zero 1000D83C
data l_demoSeaHeightA 1000D840
data l_demoSeaHeightB 1000D844
data l_maxPatternFrame 1000D848
data l_auxiliaryYawBias 1000D84C
data l_angleScale 1000D850
data l_colorMaximum 1000D854
data l_motionScale 1000D95C
data l_frameRate 1000D960
data l_frameLimit 1000D964
data l_animationEnd 1000D968
data l_groundCheckHeight 1000D96C
data l_groundSoundDistance 1000D970
data l_demoHeightAmplitude 1000D974
data l_demo46OffsetX 1000D978
data l_demo46OffsetZ 1000D97C
data l_demo44Height 1000D980
data l_demo44Roll 1000D984
data l_demo17OffsetY 1000D988
data l_demo17OffsetZ 1000D98C
data l_demo17OffsetX 1000D990
data l_demo17PlaySpeed 1000D994
data l_headerPi 1000D9D8
data l_headerPiHalf 1000D9DC
data l_stayRoom 1047E6C8
data l_demoPhase 101B42F8
data l_demoFrame 101D600C
data l_monotone 101F4822
data l_eventBits 1000D8E8
data l_itemNumbers 1000D878
data l_lightData 101B42B8
data l_demoManager 101D5FFC
data l_resourceManager 101F4F28
data l_graphicsSystem 101F9968
data l_saveInfo 101F84DC
data l_opaDrawBuffer 104B4634
data l_xluDrawBuffer 104B4638
data l_matrix 1048D0CC
data l_headerZero 10463AEC
data l_headerPiValue 10463AE0
data l_headerPiHalfValue 10463AE4
data l_headerFlagA 10463AE8
data l_headerFlagB 10463AE9
data l_destructorRecordA 101B42FC
data l_destructorRecordB 101B4308
data l_destructorRecordC 101B4314
data l_jointOffset 1000D9C8
data l_archiveName 1047E6B8
ext 025D4ED0 void demoActorConstruct(void*)
ext 0255FFF4 void demoInitTev(void*,s8,u32)
ext 02563BF0 void demoDeleteLight(void*)
ext 025D50BC void demoActorDestruct(void*,u32)
ext 025D6134 void demoDeleteHeap(void*)
ext 028F026C void demoRegisterDestructor(void*)
ext 028ED6F8 void demoHeaderConstructA(void*)
ext 028EAB2C void demoHeaderConstructB(void*)
ext 025F19F8 void demoMatrixRotationZXY(void*,s16,s16,s16)
ext 025D63E8 s32 demoEntrySolidHeap(void*,void*,u32)
ext 025E742C void demoAnimationPlay(void*)
ext 026067F4 void* demoGetResourceByID(void*,void*,u16)
ext 02606900 void* demoGetResourceByName(void*,void*,void*)
ext 0273AA24 void demoAssert(const char*,s32,const char*)
ext 025E7820 void* demoBtpConstruct(void*)
ext 025E789C s32 demoBtpInit(void*,void*,void*,u32,u32,s32,s32,u32,u32,f32)
ext 025E7C6C void* demoBtkConstruct(void*)
ext 025E7CE0 s32 demoBtkInit(void*,void*,void*,u32,u32,s32,s32,u32,u32,f32)
ext 025E80D0 void* demoBrkConstruct(void*)
ext 025E8154 s32 demoBrkInit(void*,void*,void*,u32,u32,s32,s32,u32,u32,f32)
ext 025E38E0 void* demoModelCreate(void*,u32,u32)
ext 027F3F8C void* demoGetModelMetadata(void*)
ext 027F596C void demoModelSetFlags(void*,u32)
ext 025E4F64 DemoMorfView* demoMorfConstruct(void*,void*,void*,void*,void*,s32,void*,s32,u32,void*,u32,u32,f32)
ext 027ED1F0 void* demoGetTextureName(void*,u16)
ext 027F81A4 void* demoGetFramebufferData(void*,u32)
ext 02773680 void* demoImageConstruct(void*,void*)
ext 025E895C void* demoInvisibleConstruct(void*)
ext 025E8A48 s32 demoInvisibleCreate(void*,void*)
ext 02008E0C void demoGroundConstruct(void*)
ext 02563ABC void demoLightInit(void*,void*,u32,u8,u32)
ext 025626A4 void demoSettingTevStruct(void*,s32,void*,void*)
ext 02562F5C void demoSetLightTevColor(void*,void*,void*)
ext 025E7B3C void demoBtpEntry(void*,void*,s16)
ext 025E7FC4 void demoBtkEntry(void*,void*,f32)
ext 025E83FC void demoBrkEntry(void*,void*,f32)
ext 027FA7F8 void demoPrepareTextureSlot(void*,u32)
ext 025E8E58 void demoInvisibleMorfEntry(void*,void*)
ext 025E8DEC void demoInvisibleModelEntry(void*,void*)
ext 025E54D8 void demoMorfUpdateDL(void*)
ext 02120248 void demoDrawAuxiliaryNative(void*)
ext 025E2DE0 void demoModelUpdateDL(void*,u32)
ext 0246B6A4 s32 demoSeaContains(f32,f32)
ext 0246BA0C f32 demoSeaHeight(f32,f32)
ext 025602F0 void demoGetWaterColor(void*,void*)
ext 0274D458 void demoApplyExposure(void*,void*,f32)
ext 027F9F0C void* demoMaterialColor(void*,s32)
ext 02526E70 void* demoGetActor(void*,u32)
ext 0283CA7C void demoParseParameter(void*,void*)
ext 025B8B68 void demoSetEventBit(void*,u16)
ext 0254DA38 void demoSetItem(u8)
ext 025CB610 void demoStopQuake(void*,s32)
ext 025CB374 void demoStartShock(void*,s32,s32,void*)
ext 025CB408 void demoStartQuake(void*,s32,s32,void*)
ext 025F05E8 void demoFadeIn(s8)
ext 025F05D0 void demoFadeOut(s8,s32)
ext 025F05F8 void demoSetFadeColor(void*)
ext 025D9F38 void* demoFindActorByName(const char*,s32,s32)
ext 02527028 void demoSetActorData(void*,u32,u32,u32,u32,u32,u32,u32)
ext 02563C50 void demoLightSetPosition(void*,void*)
ext 025E4A98 void demoMorfSetAnimation(void*,void*,s32,s32,f32,f32,f32,f32)
ext 02008974 f32 demoGroundCross(void*,void*)
ext 024EECAC u32 demoGroundSoundID(void*,void*)
ext 025E535C void demoMorfPlay(void*,void*,u32,s32)
ext 027F68FC void* demoGetJointMetadata(void*)
ext 027DF9B0 s32 demoFindJoint(void*,void*)
*/
