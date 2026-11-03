// Own preset dispatch and mixed-radix weight preparation; image tables are not embedded.
#include "agl/DrawFormatPreparation.h"

void allocateDrawFormatFields(agl::UniformBuffer* buffer, s32 count, sead::RegistryHeap* heap);
void appendDrawFormatField(agl::UniformBuffer* buffer, u32 type, u32 count);

namespace agl {
static DrawFormatPresets_l sDrawFormatPresets;
static DrawVariantGroups_l sDrawVariantGroups;

/* 027CBCD0 */
void configureDrawFormat(UniformBuffer* buffer, u32 preset, u32 offset,
                         sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    DrawFormatPreset_l* format = &sDrawFormatPresets.mPresets[preset + offset];
    s32 count = format->mFieldCount;
    allocateDrawFormatFields(buffer, count, heap);
    if (count > 0) {
        DrawFormatField_l* field = format->mFields;
        for (s32 index = 0; index < count; ++index, ++field) {
            u32 fieldCount = field->mCount;
            u32 type = field->mType;
            appendDrawFormatField(buffer, type, fieldCount);
        }
    }
}

/* 027CBD54 */
void initializeDrawVariantWeights() {
    for (u32 groupIndex = 0; groupIndex < 4; ++groupIndex) {
        DrawVariantGroup_l* group = &sDrawVariantGroups.mGroups[groupIndex];
        s32 count = group->mDimensionCount;
        s32 index = s32(u32(count) - 1);
        u32 weight = 1;
        if (index >= 0) {
            u32 remaining = u32(count);
            do {
                be<u32>* weights = group->mWeights;
                weights[index] = weight;
                DrawVariantDimension_l* dimensions = group->mDimensions;
                u32 choices = dimensions[index].mChoiceCount;
                --index;
                weight *= choices;
            } while (--remaining != 0);
        }
    }
}

/* 027CBDB0 */
void agl::DrawProgramSelection_l::selectVariant(u32 preset, u32 choice, u32 enabled,
    u32 alternate, u32 mode, u32 option, u32 optionAlternate, u32 stackChoice,
    u32 firstByte, u32 secondByte, u32 thirdByte, u32 secondaryEnabled,
    u32 secondaryAlternate, u32 lastByte) {
    gabi::AutoFrame frame{gabi::current_function()};
    // The six byte-valued arguments occupy complete ABI stack words; only their low bytes are read.
    firstByte = u8(firstByte);
    secondByte = u8(secondByte);
    thirdByte = u8(thirdByte);
    secondaryEnabled = u8(secondaryEnabled);
    secondaryAlternate = u8(secondaryAlternate);
    lastByte = u8(lastByte);
    DrawVariantGroup_l* format = &sDrawVariantGroups.mGroups[preset];
    detail::ShaderHolder* holder = sShaderHolder;
    u32 shaderIndex = format->mShaderIndex;
    u32 groupCount = holder->mPrograms.mCount;
    ShaderProgram* group = nullptr;
    if (shaderIndex < groupCount) {
        gptr<ShaderProgram>* groups = holder->programSlots();
        group = groups[shaderIndex];
    }
    mProgram = group;
    u32 index = 0;
    if (preset == 0) {
        u32 secondaryMode = secondaryEnabled ? (secondaryAlternate ? 1u : 2u) : 0u;
        u32 drawMode = enabled ? (mode ? 3u : (alternate ? 2u : 1u)) : 0u;
        be<u32>* weights = format->mWeights;
        u32 thirdWeight = weights[0];
        u32 secondWeight = weights[1];
        u32 thirdTerm = thirdWeight * thirdByte;
        u32 secondaryWeight = weights[2];
        u32 secondTerm = secondWeight * secondByte;
        u32 secondaryTerm = secondaryWeight * secondaryMode;
        u32 choiceWeight = weights[3];
        index = thirdTerm + secondTerm;
        u32 choiceTerm = choiceWeight * choice;
        u32 modeWeight = weights[4];
        index += secondaryTerm;
        u32 modeTerm = modeWeight * drawMode;
        u32 lastWeight = weights[5];
        index += choiceTerm;
        u32 lastTerm = lastWeight * lastByte;
        index += modeTerm;
        index += lastTerm;
    } else if (preset == 1) {
        be<u32>* weights = format->mWeights;
        u32 stackWeight = weights[1];
        u32 secondWeight = weights[0];
        u32 secondaryMode = secondaryEnabled ? (secondaryAlternate ? 1u : 2u) : 0u;
        u32 secondTerm = secondWeight * secondByte;
        u32 secondaryWeight = weights[2];
        u32 stackTerm = stackWeight * stackChoice;
        u32 choiceWeight = weights[3];
        u32 secondaryTerm = secondaryWeight * secondaryMode;
        u32 enabledWeight = weights[4];
        index = secondTerm + stackTerm;
        u32 choiceTerm = choiceWeight * choice;
        index += secondaryTerm;
        u32 enabledTerm = enabledWeight * enabled;
        index += choiceTerm;
        index += enabledTerm;
    } else if (preset == 2) {
        u32 optionMode = option ? (optionAlternate ? 2u : 1u) : 0u;
        be<u32>* weights = format->mWeights;
        u32 optionWeight = weights[1];
        u32 firstWeight = weights[0];
        u32 secondaryMode = secondaryEnabled ? (secondaryAlternate ? 1u : 2u) : 0u;
        u32 firstTerm = firstWeight * firstByte;
        u32 secondWeight = weights[2];
        u32 optionTerm = optionWeight * optionMode;
        u32 stackWeight = weights[3];
        u32 secondTerm = secondWeight * secondByte;
        index = firstTerm + optionTerm;
        u32 secondaryWeight = weights[4];
        u32 stackTerm = stackWeight * stackChoice;
        u32 choiceWeight = weights[5];
        index += secondTerm;
        u32 secondaryTerm = secondaryWeight * secondaryMode;
        u32 enabledWeight = weights[6];
        index += stackTerm;
        u32 choiceTerm = choiceWeight * choice;
        index += secondaryTerm;
        u32 enabledTerm = enabledWeight * enabled;
        index += choiceTerm;
        index += enabledTerm;
    }
    ShaderVariantDomain* variants = &programSharedData(group)->variation;
    if (s32(index) <= 0) {
        mProgram = variants->root;
    } else {
        u32 count = variants->programCount;
        u32 programIndex = index - 1;
        ShaderProgram* programs = variants->programs;
        if (programIndex < count)
            programs += programIndex;
        mProgram = programs;
    }
}

/* 027CFED4 */
void agl::DrawProgramSelection_l::selectPair(u32 first, u32 second, u32 enabled,
                                           u32 alternate, u32 third, u32 fourth) {
    gabi::AutoFrame frame{gabi::current_function()};
    detail::ShaderHolder* holder = sShaderHolder;
    u32 shaderIndex = sDrawVariantGroups.mGroups[3].mShaderIndex;
    u32 groupCount = holder->mPrograms.mCount;
    ShaderProgram* group = nullptr;
    if (shaderIndex < groupCount) {
        gptr<ShaderProgram>* groups = holder->programSlots();
        group = groups[shaderIndex];
    }
    mProgram = group;
    be<u32>* weights = sDrawVariantGroups.mGroups[3].mWeights;
    u32 mode = enabled ? (alternate ? 1u : 2u) : 0u;
    u32 secondWeight = weights[0];
    u32 modeWeight = weights[2];
    u32 secondTerm = secondWeight * second;
    u32 firstWeight = weights[1];
    u32 thirdWeight = weights[3];
    u32 firstTerm = firstWeight * first;
    u32 fourthWeight = weights[4];
    u32 index = secondTerm + modeWeight * mode;
    u32 thirdTerm = thirdWeight * third;
    index += firstTerm;
    u32 fourthTerm = fourthWeight * fourth;
    index += thirdTerm;
    ShaderVariantDomain* variants = &programSharedData(group)->variation;
    index += fourthTerm;
    if (s32(index) <= 0) {
        mProgram = variants->root;
    } else {
        u32 count = variants->programCount;
        u32 programIndex = index - 1;
        ShaderProgram* programs = variants->programs;
        if (programIndex < count)
            programs += programIndex;
        mProgram = programs;
    }
}

/* 027CC250 */
void agl::DrawFormatPreparation_l::configurePresetA(UniformBuffer* buffer, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    configureDrawFormat(buffer, 0, 0, heap);
}

/* 027CCE34 */
void agl::DrawFormatPreparation_l::configurePresetB(UniformBuffer* buffer, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    configureDrawFormat(buffer, 1, 0, heap);
}

/* 027CE0AC */
void agl::DrawFormatPreparation_l::configurePresetC(UniformBuffer* buffer, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    configureDrawFormat(buffer, 2, 0, heap);
}

/* 027CF8A0 */
void agl::DrawFormatPreparation_l::configurePresetD(UniformBuffer* buffer, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    configureDrawFormat(buffer, 3, 0, heap);
}
}

/* WWHD glue
data sDrawFormatPresets 1016A9A8
data sDrawVariantGroups 101F962C
ext 027BECEC void allocateDrawFormatFields(agl::UniformBuffer* buffer, s32 count, sead::RegistryHeap* heap)
ext 027BED84 void appendDrawFormatField(agl::UniformBuffer* buffer, u32 type, u32 count)
*/
