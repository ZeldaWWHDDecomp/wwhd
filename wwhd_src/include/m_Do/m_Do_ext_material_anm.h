/* Material animation wrappers: tww names, HD resource-backed animator work.
 */
#pragma once
#include "bindings.h"
#include "gc_api/JSystem/JUTAssert.h"
namespace material_layout {
struct Descriptor {
    be<s32> mMaterialCount, mAnimationCount, mResourceCountA, mResourceCountB;
    be<u8> mEnabled, mStorageFlags;
    u8 _012[2];
    be<u32> mBufferA, mBufferB;
    u8 _01C[0x40 - 0x1C]; // HD: size query/init also write the trailing descriptor work.
};
WWHD_SIZE(Descriptor, 0x40);
struct Resource {
    u8 _000[0xC];
    union { be<u32> mColorFlags; be<u16> mPatternFlags; };
    u8 _010[4];
    be<u16> mColorCount, mPatternCount;
    be<s32> mCountA, mCountB;
};
struct AnimationVtable { u8 _000[0x14]; gfn getFrameMax; };
struct Animation {
    be<f32> mFrame;
    gptr<AnimationVtable> __vtbl;
    u8 _008[4];
    gptr<Resource> mpResource;
    s32 getFrameMax() { return gabi::call_ptr<s32>(__vtbl->getFrameMax.get(), this); }
};
struct FrameBlend {
    be<f32> mFrame, mStart, mEnd;
    u8 _00C[4];
    gfn mCallback;
    gptr<void> mCallbackArg;
    f32 evaluate(f32 frame) {
        return gabi::call_ptr<f32>(mCallback.get(), mCallbackArg.get(), frame, mStart.get(), mEnd.get());
    }
};
struct MaterialWork {
    enum : u32 { COLOR_VT = 0x1016D7E0, PATTERN_VT = 0x1016D860 };
    gptr<FrameBlend> mpFrame;
    u8 _004[0x34 - 4];
    be<u32> __vtbl;
    u8 _038[0x44 - 0x38];
    be<u32> field_0x44;
    u8 _048[0x58 - 0x48];
};
WWHD_SIZE(MaterialWork, 0x58);
struct Model {
    u8 _000[0x14];
    u8 mMaterialTable[0x38 - 0x14];
    gptr<MaterialWork> mpTextureAnimator;
    u8 _03C[4];
    gptr<MaterialWork> mpColorAnimator;
};
struct ModelData { u8 _000[0x24]; be<u16> mMaterialCount; };
struct mDoExt_baseAnm {
    J3DFrameCtrl mFrameCtrl; // HD: embedded instead of the GameCube frame-controller pointer.
    u32 initPlay(s32 length, s32 mode, s32 start, s32 end, u32 existing, f32 rate);
    u32 play();
};
struct mDoExt_bpkAnm : mDoExt_baseAnm {
    gptr<Animation> mpAnm;
    gptr<void> field_0x14;
    MaterialWork mAnimator;
    be<u16> mUpdateMaterialNum;
    u8 _072[2];
    u32 init(Model* model, Animation* animation, u32 play, s32 mode, s32 start, s32 end, u32 existing, f32 rate);
    void entry(Model* model, f32 frame);
};
struct mDoExt_btpAnm : mDoExt_baseAnm {
    gptr<Animation> mpAnm;
    gptr<void> field_0x14;
    be<u16> mUpdateMaterialNum;
    u8 _01A[2];
    MaterialWork mAnimator;
    u32 init(Model* model, Animation* animation, u32 play, s32 mode, s32 start, s32 end, u32 existing, f32 rate);
    void entry(Model* model, s32 frame);
    void calc(Model* model, f32 frame);
};
WWHD_OFFSET(mDoExt_bpkAnm, mAnimator, 0x18);
WWHD_OFFSET(mDoExt_bpkAnm, mUpdateMaterialNum, 0x70);
WWHD_OFFSET(mDoExt_btpAnm, mAnimator, 0x1C);
WWHD_SIZE(mDoExt_bpkAnm, 0x74);
WWHD_SIZE(mDoExt_btpAnm, 0x74);
}
u32 materialIsCurrentSolidHeap();
void materialFrameInit(J3DFrameCtrl* control, s32 end);
void materialFrameUpdate(J3DFrameCtrl* control);
void materialWorkConstruct(material_layout::MaterialWork* work);
material_layout::ModelData* materialGetModelData(material_layout::Model* model);
void* materialAllocate(u32 size, s32 alignment);
u32 materialColorBufferSize(material_layout::Descriptor* descriptor);
u32 materialPatternBufferSize(material_layout::Descriptor* descriptor);
void materialColorInit(material_layout::MaterialWork* work, material_layout::Descriptor* descriptor, void* buffer, u32 size);
void materialPatternInit(material_layout::MaterialWork* work, material_layout::Descriptor* descriptor, void* buffer, u32 size);
void materialColorSetResource(material_layout::MaterialWork* work, material_layout::Resource* resource);
void materialPatternSetResource(material_layout::MaterialWork* work, material_layout::Resource* resource);
void materialColorAttach(material_layout::MaterialWork* work, material_layout::ModelData* model);
void materialPatternAttach(material_layout::MaterialWork* work, material_layout::ModelData* model);
void materialColorCalc(material_layout::MaterialWork* work);
void materialPatternCalc(material_layout::MaterialWork* work);
void materialPatternApply(material_layout::MaterialWork* work, void* materialTable);
