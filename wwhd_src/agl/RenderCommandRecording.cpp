// Render command recording: a command record takes the next pool index (atomic increment), records into the
// current frame's slice of its ring from the ring's fill offset, and on end advances the offset by the aligned
// recorded size. Both ends are timed and run under the graphics access manager's local lock.
#include "agl/RenderCommandPool.h"
#include "sead/ElementAt.h"
#include "agl/RenderRecording.h"
#include "agl/GraphicsAccess.h"
using namespace agl::render;
typedef gptr<agl::GraphicsAccessManager> CommandAccessManagerPointer;
static CommandAccessManagerPointer sCommandAccessManager;
Pair32 systemTime();
static const u32 kMaxCommandBytes = 0x400000;
// A command record starts with the display-list recording its constructor builds first (027DA694 -> 027C05F4).
// The tick count of OSGetSystemTime: high word in r3, low word in r4.
static void recordTime(agl::gx2util::TickValue* tick) {
    Pair32 now = systemTime();
    tick->mHigh = now.r3;
    tick->mLow = now.r4;
}
static agl::DisplayListRecording* recordingOf(agl::gx2util::TimedCounter* command) {
    return reinterpret_cast<agl::DisplayListRecording*>(command);
}
/* 027D54F8 */
u32 agl::render::RenderCommandPool::beginCommand(const sead::SafeText* name,u32 request,u32 ring) {
    gabi::AutoFrame frame{gabi::current_function()};
    sCommandAccessManager->acquireLocal();
    u32 counter = gabi::ea(&mNextCommand);
    u32 index;
    do { index = gabi::load_reserved(counter); } while (!gabi::store_conditional(counter,index + 1u));
    gx2util::TimedCounter* command = mCommands;
    if (index < (u32)mCommandCapacity) command += index;
    recordingOf(command)->clear();
    be<u8>* text = name->mText;
    command->mActive = ring;
    recordingOf(command)->mName.mText = text;
    command->mCount = request;
    command->mRate = 0;
    recordTime(&command->mStart);
    RenderCommandRing* target = &mRings[ring];
    u32 frameIndex = (u8)mCurrentFrame;
    RenderCommandSlice* slice = target->mSlices;
    u32 offset = target->mOffset;
    if (frameIndex < (u32)target->mCapacity) slice += frameIndex;
    u32 remaining = (u32)slice->mSize - offset;
    if ((s32)remaining > (s32)kMaxCommandBytes) remaining = kMaxCommandBytes;
    recordingOf(command)->setStorage(static_cast<u8*>(static_cast<void*>(slice->mData)) + offset,remaining);
    recordingOf(command)->begin(0);
    target->mCurrentCommand = command;
    return index;
}
/* 027D55F8 */
agl::gx2util::TimedCounter* agl::render::RenderCommandPool::endCommand(u32 index,u32 ring) {
    gabi::AutoFrame frame{gabi::current_function()};
    gx2util::TimedCounter* command = mCommands;
    if (index < (u32)mCommandCapacity) command = &sead::elementAt<gx2util::TimedCounter>(gabi::ea(command),index); /* guest EA arithmetic: wraps at 32 bits as on the console */
    RenderCommandRing* target = &sead::elementAt<RenderCommandRing>(gabi::ea(mRings),ring);
    u32 used = recordingOf(command)->end(0);
    u32 offset = (u32)target->mOffset + used;
    offset += agl::RecordingPolicy::alignment() - 1u;
    offset &= ~(agl::RecordingPolicy::alignment() - 1u);
    target->mOffset = offset;
    recordTime(&command->mEnd);
    target->mCurrentCommand = nullptr;
    sCommandAccessManager->releaseLocal();
    return command;
}
/* WWHD glue
data sCommandAccessManager 101F8E04
ext 027B736C void agl::GraphicsAccessManager::acquireLocal()
ext 027B737C void agl::GraphicsAccessManager::releaseLocal()
ext 027C05C8 void agl::DisplayListRecording::clear()
ext 027C05A0 void agl::DisplayListRecording::setStorage(void* data,u32 capacity)
ext 027C073C u32 agl::DisplayListRecording::begin(u32 index)
ext 027C077C u32 agl::DisplayListRecording::end(u32 index)
ext C0009C98 Pair32 systemTime()
*/
