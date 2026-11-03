// Local WWHD parameter defaults and channel mapping.
#include "agl/EntryParameters.h"
#include "agl/EffectSurfaceViews.h"

static f32 sScalarDefault;
// Borrowed read-only views; canonical writer ownership remains outside this unit.
static f32 sSharedPairFirst;
static f32 sSharedPairSecond;
static f32 sPairDefault;

/* 027ACA18 */
static void resetScalarMode(agl::ScalarModeEntry* entry) {
    f32 scalar = sScalarDefault;
    entry->mMode.mValue = 2;
    entry->mScalar.mValue = scalar;
}
/* 027AD0BC */
static void resetScalarCount(agl::ScalarModeEntry* entry) {
    f32 scalar = sScalarDefault;
    entry->mMode.mValue = 2;
    entry->mScalar.mValue = scalar;
}
/* 027AD63C */
static void resetTripleSelector(agl::TripleSelectorEntry* entry) {
    entry->mSelectors[0].mValue = 4;
    entry->mSelectors[1].mValue = 1;
    entry->mSelectors[2].mValue = 9;
}
/* 027ADDD8 */
static void resetTriplePair(agl::TriplePairEntry* entry) {
    f32 first = sSharedPairFirst;
    entry->mPairs[0].mValue[0] = first;
    f32 second = sSharedPairSecond;
    entry->mPairs[1].mValue[0] = first;
    entry->mPairs[2].mValue[1] = second;
    entry->mPairs[2].mValue[0] = first;
    entry->mPairs[0].mValue[1] = second;
    entry->mPairs[1].mValue[1] = second;
}
/* 027AE390 */
static void resetDoublePair(agl::DoublePairEntry* entry) {
    f32 first = sSharedPairFirst;
    entry->mPairs[0].mValue[0] = first;
    f32 second = sSharedPairSecond;
    entry->mPairs[0].mValue[1] = second;
    f32 value = sPairDefault;
    entry->mPairs[1].mValue[0] = value;
    entry->mPairs[1].mValue[1] = value;
}
/* 027AE470 */
static void resetChannelSelector(agl::ChannelSelectorEntry* entry) {
    entry->mSelectors[0].mValue = 4;
    entry->mSelectors[1].mValue = 0;
    entry->mSelectors[3].mValue = 2;
    entry->mSelectors[4].mValue = 3;
    entry->mSelectors[2].mValue = 1;
}

struct SelectorTable5 { be<u8> mValues[5]; };
struct SelectorTable6 { be<u8> mValues[6]; };
static SelectorTable5 sFormatMap;
static SelectorTable6 sFirstChannelMap;
static SelectorTable6 sSecondChannelMap;
static SelectorTable6 sThirdChannelMap;
static SelectorTable6 sFourthChannelMap;

/* 027AE3B8 */
static u32 mapChannels(agl::ChannelSelectorEntry* entry, agl::EffectContext_l* context, u32 result) {
    u32 format = context->format;
    u32 first = 0, second = 1, third = 2, fourth = 3;
    u32 index = entry->mSelectors[0].mValue;
    if (index <= 4) format = sFormatMap.mValues[index];
    index = entry->mSelectors[1].mValue;
    u32 secondIndex = entry->mSelectors[2].mValue;
    if (index <= 5) first = sFirstChannelMap.mValues[index];
    if (secondIndex <= 5) second = sSecondChannelMap.mValues[secondIndex];
    index = entry->mSelectors[3].mValue;
    u32 fourthIndex = entry->mSelectors[4].mValue;
    if (index <= 5) third = sThirdChannelMap.mValues[index];
    if (fourthIndex <= 5) fourth = sFourthChannelMap.mValues[fourthIndex];
    u32 flags = context->sampler.dirty;
    context->sampler.componentMapping[3] = fourth;
    context->format = format;
    context->sampler.componentMapping[2] = third;
    context->sampler.dirty = flags | 1;
    context->sampler.componentMapping[1] = second;
    context->mappingChanged = 1;
    context->sampler.componentMapping[0] = first;
    context->sampler.customMapping = 1;
    return result;
}
u32 applyScalarMode(agl::ScalarModeEntry*, agl::EffectContext_l*, u32, u32);
u32 applyCountMode(agl::ScalarModeEntry*, agl::EffectContext_l*, u32, u32);
/* 027AC984 */
static u32 applySelectedMode(agl::ScalarModeEntry* entry, agl::EffectContext_l* destination, u32 initial) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 mode = entry->mMode.mValue;
    u32 result = initial;
    if (mode == 4) {
        result = applyScalarMode(entry, destination, 2, initial);
        mode = 2;
    } else if (mode == 3) {
        result = applyScalarMode(entry, destination, 1, initial);
        mode = 2;
    }
    return applyScalarMode(entry, destination, mode, result);
}
/* 027AD050 */
static u32 applyRepeatedCount(agl::ScalarModeEntry* entry, agl::EffectContext_l* destination, u32 initial) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = entry->mMode.mValue;
    u32 result = initial;
    while (count > 0) {
        result = applyCountMode(entry, destination, 1, result);
        --count;
    }
    return result;
}
u32 applySelectorEntry(agl::TripleSelectorEntry*, agl::EffectContext_l*, u32, u32, u32);
/* 027AD4D4 */
static u32 applyRepeatedSelector(agl::TripleSelectorEntry* entry, agl::EffectContext_l* destination, u32 initial) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 mode = 3;
    switch (u32(entry->mSelectors[2].mValue)) {
    case 3: mode = 0; break;
    case 5: mode = 1; break;
    case 7: mode = 2; break;
    case 9: mode = 3; break;
    case 11: mode = 4; break;
    case 13: mode = 5; break;
    }
    u32 result = initial;
    for (s32 index = 0; index < s32(entry->mSelectors[1].mValue); ++index) {
        u32 kind = entry->mSelectors[0].mValue;
        result = applySelectorEntry(entry, destination, kind, mode, result);
    }
    return result;
}
/* WWHD glue
 ext 027AD0D4 u32 applySelectorEntry(agl::TripleSelectorEntry*, agl::EffectContext_l*, u32, u32, u32)
 ext 027AC390 u32 applyScalarMode(agl::ScalarModeEntry*, agl::EffectContext_l*, u32, u32)
 ext 027ACA30 u32 applyCountMode(agl::ScalarModeEntry*, agl::EffectContext_l*, u32, u32)
 data sScalarDefault 10168874
 data sSharedPairFirst 104A0908
 data sSharedPairSecond 104A090C
 data sPairDefault 101689DC
 data sFormatMap 10168B54
 data sFirstChannelMap 10168B34
 data sSecondChannelMap 10168B3C
 data sThirdChannelMap 10168B44
 data sFourthChannelMap 10168B4C
*/
