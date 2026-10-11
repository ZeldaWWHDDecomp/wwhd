#include "m_Do/m_Do_ext_more_anm.h"

static f32 l_one;
static u32 l_backendVtable;

// HD: resource descriptors and backend allocation replace the GameCube material arrays.
static bool prepareBackend(HdAnmBackend* backend, HdAnmModel* model, HdAnmObject* animation) {
    gabi::Local<HdAnmDescriptor> local;
    HdAnmDescriptor* descriptor = local.get();
    descriptor->mModelCount = -1;
    descriptor->mMaterialCount = -1;
    descriptor->mFirstCount = -1;
    descriptor->mSecondCount = -1;
    descriptor->mEnabled = 1;
    descriptor->mFlags = 0;
    descriptor->mBufferSize = 0;
    descriptor->mBufferCount = 0;
    HdAnmModelData* data = hdAnmGetModelData(model);
    s32 count = data->mMaterialCount;
    if (descriptor->mModelCount < count) descriptor->mModelCount = count;
    HdAnmResource* resource = animation->mResource;
    count = resource->mMaterialCount;
    if (descriptor->mMaterialCount < count) descriptor->mMaterialCount = count;
    s32 first = resource->mFirstCount;
    if (descriptor->mFirstCount < first) descriptor->mFirstCount = first;
    s32 second = resource->mSecondCount;
    if (descriptor->mSecondCount < second) descriptor->mSecondCount = second;
    u32 flags = resource->mFlags;
    descriptor->mFlags = (u8)(descriptor->mFlags | ((flags & HdAnmResource_Enabled) ^ HdAnmResource_Enabled));
    s32 size = resource->mFirstCount;
    if (size < 4) size = 4;
    descriptor->mSecondCount = size;
    descriptor->mFlags = 1;
    u32 bytes = hdAnmDescriptorSize(descriptor);
    void* buffer = hdAnmBufferAllocate(bytes, 4);
    if (buffer == NULL) return false;
    hdAnmBackendInit(backend, descriptor, buffer, bytes);
    return true;
}

static u32 initPlayback(mDoExt_baseAnm* self, gptr<HdAnmObject>& slot, bool skeletal,
                        s32 mode, s32 start, s32 end, u32 existing, f32 rate) {
    HdAnmObject* animation = slot;
    s32 length = gabi::call_ptr<s32>(animation->mVtable[HdAnm_LengthSlot].get(), animation);
    // HD: the virtual callback may replace the animation; reload the stored pointer.
    animation = slot;
    HdAnmResource* resource = skeletal ? animation->mBckResource.get() : animation->mResource.get();
    u32 flags = resource->mFlags;
    f32 frame = (f32)length;
    if (flags & HdAnmResource_ExcludeLast) frame = gabi::fsubs_ppc(frame, l_one);
    return hdAnmInitPlay(self, (s32)(s16)gabi::ftoi(frame), mode, start, end, existing, rate) ? 1 : 0;
}

static void calculateController(HdAnmController* controller, f32 frame) {
    f32 result = gabi::call_ptr<f32>(controller->mCalc.get(), controller->mCalcData.get(), frame,
                                   (f32)controller->mStart, (f32)controller->mEnd);
    controller->mFrame = result;
}

/* 025E7C6C */
// HD: the constructor ABI accepts a nullable allocation destination.
static HdBtkAnm* HdBtkAnm__ct(HdBtkAnm* self) {
    if (self == NULL) {
        self = static_cast<HdBtkAnm*>(hdAnmAllocate(sizeof(HdBtkAnm)));
        if (self == NULL) return NULL;
    }
    hdAnmFrameConstruct(self, 0);
    hdAnmBackendConstruct(&self->mBackend);
    self->mBackend.mBuffer = 0;
    self->mBackend.__vtbl = &l_backendVtable;
    self->mpTexMtxAnm = 0;
    self->mUpdateMaterialNum = 0;
    self->mpAnm = NULL;
    return self;
}

/* 025E7CE0 */
u32 HdBtkAnm::init(HdAnmModel* model, HdAnmObject* animation, u32 play, s32 mode,
                     s32 start, s32 end, u32 existing, f32 rate) {
    if (animation == NULL) {
        mpAnm = NULL;
        return 1;
    }
    if (model == NULL) {
        hdAnmAssert("btk source", 0x40F, "btk model");
        return 0;
    }
    mpAnm = animation;
    if (!existing && !prepareBackend(&mBackend, model, animation)) return 0;
    hdAnmSetResource(&mBackend, animation->mResource);
    HdAnmModelData* data = hdAnmGetModelData(model);
    hdAnmBindModel(&mBackend, data);
    if (play && !initPlayback(this, mpAnm, false, mode, start, end, existing, rate)) return 0;
    return 1;
}

/* 025E7FC4 */
void HdBtkAnm::entry(HdAnmModel* model, f32 frame) {
    HdAnmObject* animation = mpAnm;
    if (animation == NULL) return;
    mFrameCtrl.mFrame = frame;
    animation->mFrame = frame;
    calculateController(mBackend.mController, frame);
    hdAnmBackendCalc(&mBackend);
    model->mBtk = &mBackend;
}

/* 025E8048 */
void HdBtkAnm::calc(HdAnmModel* model, f32 frame) {
    HdAnmObject* animation = mpAnm;
    if (animation == NULL) return;
    mFrameCtrl.mFrame = frame;
    animation->mFrame = frame;
    calculateController(mBackend.mController, frame);
    hdAnmBackendCalc(&mBackend);
    hdAnmBackendApply(&mBackend, &model->mMaterialTarget);
}

/* 025E80D0 */
static HdBrkAnm* HdBrkAnm__ct(HdBrkAnm* self) {
    if (self == NULL) {
        self = static_cast<HdBrkAnm*>(hdAnmAllocate(sizeof(HdBrkAnm)));
        if (self == NULL) return NULL;
    }
    hdAnmFrameConstruct(self, 0);
    self->mpAnm = NULL;
    self->mCRegUpdateMaterialNum = 0;
    self->mpKRegAnm = 0;
    self->mKRegUpdateMaterialNum = 0;
    self->mpCRegAnm = 0;
    hdAnmBackendConstruct(&self->mBackend);
    self->mBackend.mBuffer = 0;
    self->mBackend.__vtbl = &l_backendVtable;
    return self;
}

/* 025E8154 */
u32 HdBrkAnm::init(HdAnmModel* model, HdAnmObject* animation, u32 play, s32 mode,
                     s32 start, s32 end, u32 existing, f32 rate) {
    if (animation == NULL) {
        mpAnm = NULL;
        return 1;
    }
    mpAnm = animation;
    if (model == NULL) {
        return 0;
    }
    if (!existing && !prepareBackend(&mBackend, model, animation)) return 0;
    hdAnmSetResource(&mBackend, animation->mResource);
    HdAnmModelData* data = hdAnmGetModelData(model);
    hdAnmBindModel(&mBackend, data);
    if (play && !initPlayback(this, mpAnm, false, mode, start, end, existing, rate)) return 0;
    return 1;
}

/* 025E83FC */
void HdBrkAnm::entry(HdAnmModel* model, f32 frame) {
    HdAnmObject* animation = mpAnm;
    if (animation == NULL) return;
    mFrameCtrl.mFrame = frame;
    animation->mFrame = frame;
    calculateController(mBackend.mController, frame);
    hdAnmBackendCalc(&mBackend);
    model->mBrk = &mBackend;
}

/* 025E8480 */
void HdBrkAnm::calc(HdAnmModel* model, f32 frame) {
    HdAnmObject* animation = mpAnm;
    if (animation == NULL) return;
    mFrameCtrl.mFrame = frame;
    animation->mFrame = frame;
    calculateController(mBackend.mController, frame);
    hdAnmBackendCalc(&mBackend);
    hdAnmBackendApply(&mBackend, &model->mMaterialTarget);
}

/* 025E8508 */
u32 HdBckAnm::init(HdAnmModel* model, HdAnmObject* animation, u32 play, s32 mode,
                  s32 start, s32 end, u32 existing, f32 rate) {
    if (!existing && !hdAnmIsCurrentSolidHeap()) hdAnmAssert("bck source", 0x61B, "solid heap");
    if (model == NULL || animation == NULL) {
        hdAnmAssert("bck source", 0x61C, "bck model and animation");
        return 0;
    }
    mpAnm = animation;
    if (!existing) hdBckBackendInit(&mBackend, model, animation);
    else hdBckBackendSet(&mBackend, animation);
    if (!play) return 1;
    if (mpAnm == NULL) return 0;
    return initPlayback(this, mpAnm, true, mode, start, end, existing, rate);
}

/* 025E86B8 */
void HdBckAnm::entry(HdAnmModel* model, f32 frame) {
    // HD: skeletal binding updates its backend controller without changing the wrapper frame.
    calculateController(mBackend.mController, frame);
    model->mJointBinding->mBck = &mBackend;
}

/* 025E871C */
void HdBckAnm::changeBckOnly(HdAnmObject* animation) {
    mpAnm = animation;
    hdBckBackendSet(&mBackend, animation);
}

static void setDrawMode(u32 mode) {
    gabi::Local<HdAnmDrawState> local;
    HdAnmDrawState* state = local.get();
    hdAnmDrawStateConstruct(state);
    u32 flags = state->mMode;
    flags = (flags & ~HdAnm_ModeLowMask) + mode;
    flags = (flags & ~HdAnm_ModeHighMask) + HdAnm_ModeHighValue;
    state->mMode = flags;
    state->mFlags = HdAnm_DrawFlags;
    hdAnmDrawStateApply(state);
}

/* 025E8728 */
static void mDoExt_setAnmDrawModeEight() { setDrawMode(HdAnm_ModeEight); }

/* 025E8774 */
static void mDoExt_setAnmDrawModeSeven() { setDrawMode(HdAnm_ModeSeven); }

/* WWHD glue (tools/verify/autoglue.py): HD animation backend ABI.
data l_one 100586D4
data l_backendVtable 1016D7E0
str 10058A60 "btk source" renamed
str 10058A70 "btk model" renamed
str 10058A84 "bck source" renamed
str 10058A94 "solid heap" renamed
str 10058AB8 "bck model and animation" renamed
ext 0273AD10 void* hdAnmAllocate(u32)
ext 027F2BC0 void hdAnmFrameConstruct(mDoExt_baseAnm*, u32)
ext 027DA984 void hdAnmBackendConstruct(HdAnmBackend*)
ext 027F3F8C HdAnmModelData* hdAnmGetModelData(HdAnmModel*)
ext 027DEFB8 u32 hdAnmDescriptorSize(HdAnmDescriptor*)
ext 025E3A34 void* hdAnmBufferAllocate(u32, u32)
ext 027DEE5C void hdAnmBackendInit(HdAnmBackend*, HdAnmDescriptor*, void*, u32)
ext 027DF7F4 void hdAnmSetResource(HdAnmBackend*, HdAnmResource*)
ext 027DF240 void hdAnmBindModel(HdAnmBackend*, HdAnmModelData*)
ext 025E72D4 u32 hdAnmInitPlay(mDoExt_baseAnm*, s32, s32, s32, s32, u32, f32)
ext 027DF40C void hdAnmBackendCalc(HdAnmBackend*)
ext 027DF774 void hdAnmBackendApply(HdAnmBackend*, void*)
ext 025E2AF8 u32 hdAnmIsCurrentSolidHeap()
ext 027F36FC void hdBckBackendInit(HdBckBackend*, HdAnmModel*, HdAnmObject*)
ext 027F363C void hdBckBackendSet(HdBckBackend*, HdAnmObject*)
ext 0273AA24 void hdAnmAssert(const char*, u32, const char*)
ext 02750250 void hdAnmDrawStateConstruct(HdAnmDrawState*)
ext 02750370 void hdAnmDrawStateApply(HdAnmDrawState*)
*/
