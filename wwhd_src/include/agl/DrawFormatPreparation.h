// Uniform-format presets read by this unit; target buffer ownership remains with AGL#15.
#pragma once
#include "wwhd.h"
#include "sead/HeapRegistry.h"
#include "agl/ShaderHolder.h"
#include "agl/ShaderProgramActivation.h"
#include "agl/UniformBuffer.h"

namespace agl {
struct DrawFormatField_l {
    be<u32> mTag;
    be<u32> mType;
    be<u32> mCount;
};
struct DrawFormatPreset_l {
    be<u32> mTag;
    gptr<DrawFormatField_l> mFields;
    be<s32> mFieldCount;
};
struct DrawFormatPreparation_l {
    void configurePresetA(UniformBuffer* buffer, sead::RegistryHeap* heap);
    void configurePresetB(UniformBuffer* buffer, sead::RegistryHeap* heap);
    void configurePresetC(UniformBuffer* buffer, sead::RegistryHeap* heap);
    void configurePresetD(UniformBuffer* buffer, sead::RegistryHeap* heap);
};
void configureDrawFormat(UniformBuffer* buffer, u32 preset, u32 offset,
                         sead::RegistryHeap* heap);
struct DrawVariantDimension_l {
    be<u32> mTag;
    be<u32> mChoiceCount;
};
struct DrawVariantGroup_l {
    be<u32> mTag;
    be<s32> mDimensionCount;
    gptr<DrawVariantDimension_l> mDimensions;
    be<u32> mShaderIndex;
    gptr<be<u32>> mWeights;
};
// A shader variant selection over the library's ShaderHolder (sShaderHolder): the holder's program at the
// group's shader index, then that program's variant (ShaderProgramSharedData::variation) for the weighted index.
struct DrawProgramSelection_l {
    gptr<ShaderProgram> mProgram;
    void selectVariant(u32 preset, u32 choice, u32 enabled, u32 alternate,
                       u32 mode, u32 option, u32 optionAlternate, u32 stackChoice,
                       u32 firstByte, u32 secondByte, u32 thirdByte,
                       u32 secondaryEnabled, u32 secondaryAlternate, u32 lastByte);
    void selectPair(u32 first, u32 second, u32 enabled, u32 alternate,
                    u32 third, u32 fourth);
};
struct DrawFormatPresets_l { DrawFormatPreset_l mPresets[4]; };
struct DrawVariantGroups_l { DrawVariantGroup_l mGroups[4]; };
void initializeDrawVariantWeights();
}
WWHD_SIZE(agl::DrawFormatField_l, 12);
WWHD_OFFSET(agl::DrawFormatField_l, mType, 4);
WWHD_OFFSET(agl::DrawFormatField_l, mCount, 8);
WWHD_SIZE(agl::DrawFormatPreset_l, 12);
WWHD_OFFSET(agl::DrawFormatPreset_l, mFields, 4);
WWHD_OFFSET(agl::DrawFormatPreset_l, mFieldCount, 8);
WWHD_SIZE(agl::DrawVariantDimension_l, 8);
WWHD_SIZE(agl::DrawVariantGroup_l, 0x14);
WWHD_OFFSET(agl::DrawVariantGroup_l, mDimensionCount, 4);
WWHD_OFFSET(agl::DrawVariantGroup_l, mDimensions, 8);
WWHD_OFFSET(agl::DrawVariantGroup_l, mWeights, 0x10);

WWHD_SIZE(agl::DrawFormatPresets_l, 48);
WWHD_SIZE(agl::DrawVariantGroups_l, 80);

WWHD_SIZE(agl::DrawProgramSelection_l, 4);

