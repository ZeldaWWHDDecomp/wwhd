/** m_Do_ext_material_anm.cpp — color and texture-pattern animation.
 * Based on tww m_Do/m_Do_ext.cpp.
 */
#include "m_Do/m_Do_ext_material_anm.h"
using material_layout::Descriptor;
using material_layout::Resource;
using material_layout::Animation;
using material_layout::Model;
using material_layout::ModelData;
using material_layout::MaterialWork;
using material_layout::FrameBlend;
static f32 materialZero;
static f32 materialFrameAdjustment;

/* 025E72D4 */
u32 material_layout::mDoExt_baseAnm::initPlay(s32 length, s32 mode, s32 start, s32 end, u32 existing, f32 rate) {
    if (!existing && !materialIsCurrentSolidHeap())
        JUTAssertion_showAssert("m_Do_ext.cpp", 0x1CA, "i_modify || isCurrentSolidHeap()");
    f32 frame = static_cast<f32>(start);
    mFrameCtrl.mStart = start;
    mFrameCtrl.mFrame = frame;
    materialFrameInit(&mFrameCtrl, end < 0 ? length : end);
    f32 zero = materialZero;
    mFrameCtrl.mRate = rate;
    mFrameCtrl.mAttribute = mode;
    if (rate < zero)
        frame = static_cast<f32>(mFrameCtrl.mEnd);
    mFrameCtrl.mFrame = frame;
    mFrameCtrl.mLoop = static_cast<s16>(gabi::ftoi(frame));
    return 1;
}

/* 025E742C */
u32 material_layout::mDoExt_baseAnm::play() {
    materialFrameUpdate(&mFrameCtrl);
    return (mFrameCtrl.mState & J3DFrameCtrl::STATE_STOP_E) || mFrameCtrl.mRate == materialZero;
}

/* 025E7480 */
material_layout::mDoExt_bpkAnm* mDoExt_bpkAnm_ct(material_layout::mDoExt_bpkAnm* self) {
    if (!self) {
        self = static_cast<material_layout::mDoExt_bpkAnm*>(operator_new(0x74));
        if (!self)
            return nullptr;
    }
    materialFrameInit(&self->mFrameCtrl, 0);
    self->mpAnm = nullptr;
    self->field_0x14 = nullptr;
    materialWorkConstruct(&self->mAnimator);
    self->mUpdateMaterialNum = 0;
    self->mAnimator.field_0x44 = 0;
    self->mAnimator.__vtbl = MaterialWork::COLOR_VT; // HD: color animator vtable.
    return self;
}

/* 025E7820 */
material_layout::mDoExt_btpAnm* mDoExt_btpAnm_ct(material_layout::mDoExt_btpAnm* self) {
    if (!self) {
        self = static_cast<material_layout::mDoExt_btpAnm*>(operator_new(0x74));
        if (!self)
            return nullptr;
    }
    materialFrameInit(&self->mFrameCtrl, 0);
    self->field_0x14 = nullptr;
    self->mUpdateMaterialNum = 0;
    self->mpAnm = nullptr;
    materialWorkConstruct(&self->mAnimator);
    self->mAnimator.field_0x44 = 0;
    self->mAnimator.__vtbl = MaterialWork::PATTERN_VT; // HD: texture-pattern animator vtable.
    return self;
}

/* 025E74FC */
u32 material_layout::mDoExt_bpkAnm::init(Model* model, Animation* animation, u32 play, s32 mode, s32 start, s32 end, u32 existing, f32 rate) {
    if (!animation) {
        mpAnm = nullptr;
        return 1;
    }
    mpAnm = animation;
    if (!existing) {
        // HD: reserve all 64 descriptor bytes; the size query and initializer write its tail.
        Descriptor descriptor;
        descriptor.mMaterialCount = -1;
        descriptor.mAnimationCount = -1;
        descriptor.mResourceCountA = -1;
        descriptor.mResourceCountB = -1;
        descriptor.mEnabled = 1;
        descriptor.mStorageFlags = 0;
        descriptor.mBufferA = 0;
        descriptor.mBufferB = 0;
        ModelData* data = materialGetModelData(model);
        s32 count = data->mMaterialCount;
        if (descriptor.mMaterialCount < count)
            descriptor.mMaterialCount = count;
        Resource* resource = animation->mpResource;
        count = resource->mColorCount;
        if (descriptor.mAnimationCount < count)
            descriptor.mAnimationCount = count;
        s32 first = resource->mCountA;
        if (descriptor.mResourceCountA < first)
            descriptor.mResourceCountA = first;
        s32 second = resource->mCountB;
        if (descriptor.mResourceCountB < second)
            descriptor.mResourceCountB = second;
        u32 flags = resource->mColorFlags;
        descriptor.mStorageFlags = descriptor.mStorageFlags | ((flags & 1) ^ 1);
        s32 size = resource->mCountA;
        if (size < 4)
            size = 4;
        descriptor.mResourceCountB = size;
        descriptor.mStorageFlags = 1;
        u32 bytes = materialColorBufferSize(&descriptor);
        void* buffer = materialAllocate(bytes, 4);
        if (!buffer)
            return 0;
        materialColorInit(&mAnimator, &descriptor, buffer, bytes);
    }
    materialColorSetResource(&mAnimator, animation->mpResource);
    ModelData* data = materialGetModelData(model);
    materialColorAttach(&mAnimator, data);
    if (play) {
        Animation* current = mpAnm;
        s32 length = current->getFrameMax();
        current = mpAnm; // HD: the virtual callback can replace the animation.
        Resource* resource = current->mpResource;
        u32 flags = resource->mColorFlags;
        f32 frame = static_cast<f32>(length);
        if (flags & 4)
            frame = gabi::fsubs_ppc(frame, materialFrameAdjustment);
        if (!initPlay(static_cast<s16>(gabi::ftoi(frame)), mode, start, end, existing, rate))
            return 0;
    }
    return 1;
}

/* 025E789C */
u32 material_layout::mDoExt_btpAnm::init(Model* model, Animation* animation, u32 play, s32 mode, s32 start, s32 end, u32 existing, f32 rate) {
    if (!animation) {
        mpAnm = nullptr;
        return 1;
    }
    mpAnm = animation;
    if (!existing) {
        // HD: reserve all 64 descriptor bytes; the size query and initializer write its tail.
        Descriptor descriptor;
        descriptor.mMaterialCount = -1;
        descriptor.mAnimationCount = -1;
        descriptor.mResourceCountA = -1;
        descriptor.mResourceCountB = -1;
        descriptor.mEnabled = 1;
        descriptor.mStorageFlags = 0;
        descriptor.mBufferA = 0;
        descriptor.mBufferB = 0;
        ModelData* data = materialGetModelData(model);
        s32 count = data->mMaterialCount;
        if (descriptor.mMaterialCount < count)
            descriptor.mMaterialCount = count;
        Resource* resource = animation->mpResource;
        count = resource->mPatternCount;
        if (descriptor.mAnimationCount < count)
            descriptor.mAnimationCount = count;
        s32 first = resource->mCountA;
        if (descriptor.mResourceCountA < first)
            descriptor.mResourceCountA = first;
        s32 second = resource->mCountB;
        if (descriptor.mResourceCountB < second)
            descriptor.mResourceCountB = second;
        u32 flags = resource->mPatternFlags;
        descriptor.mStorageFlags = descriptor.mStorageFlags | ((flags & 1) ^ 1);
        s32 size = resource->mCountA;
        if (size < 4)
            size = 4;
        descriptor.mResourceCountB = size;
        descriptor.mStorageFlags = 1;
        u32 bytes = materialPatternBufferSize(&descriptor);
        void* buffer = materialAllocate(bytes, 4);
        if (!buffer)
            return 0;
        materialPatternInit(&mAnimator, &descriptor, buffer, bytes);
    }
    materialPatternSetResource(&mAnimator, animation->mpResource);
    ModelData* data = materialGetModelData(model);
    materialPatternAttach(&mAnimator, data);
    if (play) {
        Animation* current = mpAnm;
        s32 length = current->getFrameMax();
        current = mpAnm; // HD: the virtual callback can replace the animation.
        Resource* resource = current->mpResource;
        u32 flags = resource->mPatternFlags;
        f32 frame = static_cast<f32>(length);
        if (flags & 4)
            frame = gabi::fsubs_ppc(frame, materialFrameAdjustment);
        if (!initPlay(static_cast<s16>(gabi::ftoi(frame)), mode, start, end, existing, rate))
            return 0;
    }
    return 1;
}

/* 025E779C */
void material_layout::mDoExt_bpkAnm::entry(Model* model, f32 frame) {
    Animation* animation = mpAnm;
    if (!animation)
        return;
    mFrameCtrl.mFrame = frame;
    animation->mFrame = frame;
    FrameBlend* blend = mAnimator.mpFrame;
    f32 result = blend->evaluate(frame);
    blend->mFrame = result;
    materialColorCalc(&mAnimator);
    model->mpColorAnimator = &mAnimator;
}

/* 025E7B3C */
void material_layout::mDoExt_btpAnm::entry(Model* model, s32 frameInt) {
    f32 frame = static_cast<f32>(frameInt);
    Animation* animation = mpAnm;
    if (!animation)
        return;
    mFrameCtrl.mFrame = frame;
    animation->mFrame = frame;
    FrameBlend* blend = mAnimator.mpFrame;
    f32 result = blend->evaluate(frame);
    blend->mFrame = result;
    materialPatternCalc(&mAnimator);
    model->mpTextureAnimator = &mAnimator;
}

/* 025E7BE4 */
void material_layout::mDoExt_btpAnm::calc(Model* model, f32 frame) {
    Animation* animation = mpAnm;
    if (!animation)
        return;
    mFrameCtrl.mFrame = frame;
    animation->mFrame = frame;
    FrameBlend* blend = mAnimator.mpFrame;
    f32 result = blend->evaluate(frame);
    blend->mFrame = result;
    materialPatternCalc(&mAnimator);
    materialPatternApply(&mAnimator, model->mMaterialTable);
}

/* WWHD glue (tools/verify/autoglue.py)
data materialZero 100586DC
data materialFrameAdjustment 100586D4
str 10058A2C
str 10058A3C
ext 025E2AF8 u32 materialIsCurrentSolidHeap()
ext 027F2BC0 void materialFrameInit(J3DFrameCtrl* control, s32 end)
ext 027F2FC4 void materialFrameUpdate(J3DFrameCtrl* control)
ext 027DA984 void materialWorkConstruct(material_layout::MaterialWork* work)
ext 027F3F8C material_layout::ModelData* materialGetModelData(material_layout::Model* model)
ext 025E3A34 void* materialAllocate(u32 size, s32 alignment)
ext 027DEFB8 u32 materialColorBufferSize(material_layout::Descriptor* descriptor)
ext 027E0AF0 u32 materialPatternBufferSize(material_layout::Descriptor* descriptor)
ext 027DEE5C void materialColorInit(material_layout::MaterialWork* work, material_layout::Descriptor* descriptor, void* buffer, u32 size)
ext 027E09B8 void materialPatternInit(material_layout::MaterialWork* work, material_layout::Descriptor* descriptor, void* buffer, u32 size)
ext 027DF7F4 void materialColorSetResource(material_layout::MaterialWork* work, material_layout::Resource* resource)
ext 027E12B4 void materialPatternSetResource(material_layout::MaterialWork* work, material_layout::Resource* resource)
ext 027DF240 void materialColorAttach(material_layout::MaterialWork* work, material_layout::ModelData* model)
ext 027E0C88 void materialPatternAttach(material_layout::MaterialWork* work, material_layout::ModelData* model)
ext 027DF40C void materialColorCalc(material_layout::MaterialWork* work)
ext 027E1038 void materialPatternCalc(material_layout::MaterialWork* work)
ext 027E1234 void materialPatternApply(material_layout::MaterialWork* work, void* materialTable)
*/
