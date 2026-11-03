/* Sound driver event log and the remote speaker filter preset table, reconstructed from WWHD. */
#include "nw/snd/AudioEventLog.h"
using namespace nw::snd;
struct RemoteFilterTable { be<s16> coefficients[128][5]; };
static RemoteFilterTable sRemoteFilterPresets;
void* OSGetCurrentThread();

/* 0289D108 */
void nw::snd::remoteFilterCoefficients(s32 preset, be<s16>* first, be<s16>* second, be<s16>* third, be<s16>* fourth, be<s16>* fifth) {
    if (preset > 127) preset = 127;
    else if (preset < 0) preset = 0;
    const be<s16>* coefficients = sRemoteFilterPresets.coefficients[preset];
    *first = coefficients[0];
    *second = coefficients[1];
    *third = coefficients[2];
    *fourth = coefficients[3];
    *fifth = coefficients[4];
}

/* 0289D15C */
void AudioEventBuffer::append(u32 kind, u32 first, u32 second) {
    gabi::AutoFrame frame{0x0289D15Cu};
    s32 index = mCount;
    if (index >= Capacity) return;
    while (true) {
        // The index is the stored count, unchecked below 0: keep the guest address arithmetic.
        AudioEventRecord* record = gabi::at<AudioEventRecord>(gabi::ea(mRecords) + u32(index) * sizeof(AudioEventRecord));
        record->mKind = kind;
        record->mFirst = first;
        record->mSecond = second;
        record->mThread = OSGetCurrentThread();
        index = s32(u32(mCount) + 1u);
        mCount = index;
        if (index != Capacity - 1) return;
        // One record left: it marks the overflow.
        kind = OverflowKind;
        first = 0xFFFFFFFFu;
        second = 0xFFFFFFFFu;
    }
}

/* 0289D1D8 */
void AudioEventLog::append(u32 kind, u32 first, u32 second) {
    gabi::AutoFrame frame{0x0289D1D8u};
    AudioEventBuffer* writing = mWriting;
    if (writing) writing->append(kind, first, second);
}

/* 0289D1E8 */
void AudioEventBuffer::clear() { mCount = 0; }

/* 0289D1FC */
void AudioEventLog::clearReading() {
    gabi::AutoFrame frame{0x0289D1FCu};
    AudioEventBuffer* writing = mWriting;
    if (writing == &mBuffers[0]) mBuffers[1].clear();
    else if (writing == &mBuffers[1]) mBuffers[0].clear();
}

/* 0289D220 */
void AudioEventLog::switchBuffers() {
    AudioEventBuffer* writing = mWriting;
    if (writing == &mBuffers[0]) {
        mBuffers[1].mCount = 0;
        mWriting = &mBuffers[1];
    } else if (writing == &mBuffers[1]) {
        mWriting = &mBuffers[0];
        mBuffers[0].mCount = 0;
    }
}

/* WWHD glue
data sRemoteFilterPresets 10184F84
ext C0009BC8 void* OSGetCurrentThread()
*/
