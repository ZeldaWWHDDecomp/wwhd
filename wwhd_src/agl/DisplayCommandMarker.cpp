// Debug check (RenderQueueManager::drawView 027C1AD8 with manager flag 0x80): one random word of one random deferred
// command's display list is overwritten with 0xEEEEEEEE and flushed (no empty-list guard in the original).
#include "agl/DisplayFrameState.h"
#include "sead/Random.h"
#include "sead/ElementAt.h"
#include <bit>
using namespace agl::render;
static sead::GlobalRandom* sGlobalRandom;
static DisplayFloatConstant sRandomScale;
static DisplayFloatConstant sUnsignedThreshold;
static DisplayDoubleConstant sUnsignedBias;
static DisplayDoubleConstant sSignedBias;
void flushDataCache(void*,u32);
struct FrameMarkerScratch { be<u32> mHigh,mLow; };
static f64 markerEncoded(const FrameMarkerScratch* scratch) {
    return std::bit_cast<f64>(((u64)(u32)scratch->mHigh << 32) | (u32)scratch->mLow);
}
/* 027D7050 */
void agl::render::DisplayFrameState::markRandomDeferredWord() {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 scale = sRandomScale;
    u32 random = sGlobalRandom->getU32();
    gabi::FrameLocal<FrameMarkerScratch> scratch(8);
    scratch->mLow = random;
    scratch->mHigh = 0x43300000u;
    f64 encoded = markerEncoded(scratch.get());
    f64 unsignedBias = sUnsignedBias;
    s32 count = mDeferred.mCount;
    f64 randomWide = gabi::fsub_ppc(encoded,unsignedBias);
    f64 signedBias = sSignedBias;
    scratch->mLow = (u32)count ^ 0x80000000u;
    f64 encodedCount = markerEncoded(scratch.get());
    f32 randomFloat = (f32)randomWide;
    f64 countWide = gabi::fsub_ppc(encodedCount,signedBias);
    f32 fraction = gabi::fmuls_ppc(randomFloat,scale);
    f32 countFloat = (f32)countWide;
    f32 selected = gabi::fmuls_ppc(fraction,countFloat);
    u32 index = (u32)gabi::ftoi(selected);
    scratch->mHigh = index;
    index = scratch->mHigh;
    agl::gx2util::TimedCounter* command = sead::elementAt(deferredCommands(),index);
    be<u32>* data = static_cast<be<u32>*>(static_cast<void*>(recording(command)->mData));
    random = sGlobalRandom->getU32();
    scratch->mLow = random;
    u32 size = recording(command)->mUsed;
    scratch->mHigh = 0x43300000u;
    encoded = markerEncoded(scratch.get());
    u32 wordCount = size >> 2;
    unsignedBias = sUnsignedBias;
    scratch->mLow = wordCount;
    randomWide = gabi::fsub_ppc(encoded,unsignedBias);
    encodedCount = markerEncoded(scratch.get());
    randomFloat = (f32)randomWide;
    countWide = gabi::fsub_ppc(encodedCount,unsignedBias);
    fraction = gabi::fmuls_ppc(randomFloat,scale);
    countFloat = (f32)countWide;
    selected = gabi::fmuls_ppc(fraction,countFloat);
    f32 threshold = sUnsignedThreshold;
    u32 word;
    if (selected < threshold) {
        scratch->mHigh = (u32)gabi::ftoi(selected);
        word = scratch->mHigh;
    } else {
        f32 reduced = gabi::fsubs_ppc(selected,threshold);
        scratch->mHigh = (u32)gabi::ftoi(reduced);
        word = (u32)scratch->mHigh + 0x80000000u;
    }
    be<u32>* marked = &sead::elementAt(data,word);
    *marked = 0xEEEEEEEEu;
    flushDataCache(marked,4);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
/* WWHD glue
data sGlobalRandom 101F8B60
data sRandomScale 1016C32C
data sUnsignedThreshold 1016C2F8
data sUnsignedBias 1016C330
data sSignedBias 1016C2E8
*/
