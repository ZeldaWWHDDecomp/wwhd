/* A double-buffered log of sound driver events (kind, two values, calling thread), and the
 * remote speaker filter preset lookup, reconstructed from WWHD. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
struct AudioEventRecord { be<u32> mKind, mFirst, mSecond; gptr<void> mThread; };
WWHD_SIZE(AudioEventRecord, 0x10);
struct AudioEventBuffer {
    enum { Capacity = 64, OverflowKind = 2 };
    AudioEventRecord mRecords[Capacity];
    be<s32> mCount;
    void append(u32 kind, u32 first, u32 second);
    void clear();
};
WWHD_OFFSET(AudioEventBuffer, mCount, 0x400);
WWHD_SIZE(AudioEventBuffer, 0x404);
struct AudioEventLog {
    AudioEventBuffer mBuffers[2];
    gptr<AudioEventBuffer> mWriting;
    void append(u32 kind, u32 first, u32 second);
    void clearReading();
    void switchBuffers();
};
WWHD_OFFSET(AudioEventLog, mWriting, 0x808);
WWHD_SIZE(AudioEventLog, 0x80C);
void remoteFilterCoefficients(s32 preset, be<s16>* first, be<s16>* second, be<s16>* third, be<s16>* fourth, be<s16>* fifth);
} }
