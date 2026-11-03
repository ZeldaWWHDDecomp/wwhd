/* Sequence command views reconstructed from WWHD accesses. */
#pragma once
#include "nw/snd/SequenceArgumentReader.h"
#include "nw/snd/SoundArchive.h"
namespace nw { namespace snd {
struct SequenceCommandStackEntry {
    be<u8> mLoop; be<u8> mRemaining; u8 mPadding[2];
    gptr<const be<u8>> mPosition;
};
struct SequenceByteRamp { be<u8> mOrigin,mTarget; be<s16> mDuration,mElapsed; };
struct SequenceModulation {
    be<f32> mDepth,mSpeed;
    be<u32> mDelay;
    be<u8> mType,mRange,mScope,mPadding;
};
/* Refines the canonical track's opaque playback/parameter ranges at +0x100. */
struct SequenceCommandState_l {
    gptr<const be<u8>> mSequenceBase;
    SequenceCursor mCursor;
    be<u8> mCondition,mWaitForNotes,mNoteMode,mReuseVoice;
    SequenceCommandStackEntry mStack[10];
    be<u8> mStackDepth,mSynthFlag,mMuted,mReserved15F;
    be<s32> mWait;
    be<u8> mWaitForNoteEnd,mPortamento,mReleaseGuard,mReleasePriorityFixed;
    be<u32> mBank;
    be<f32> mNotePitchOffset;
    SequenceByteRamp mVolume,mSecondaryVolume,mPan,mSignedRamp,mBend;
    u8 mPadding18E[2];
    SequenceModulation mModulation[4];
    be<u8> mModulationRoute[4];
    be<u8> mVelocityScale,mNotePriorityBias,mNotePan,mReserved1D7;
    be<s8> mTranspose;
    be<u8> mNotePriority,mPortamentoKey,mPitchTime;
    be<u8> mEnvelope[4];
    be<u16> mEnvelopeDelay;
    be<u8> mBiquadType,mControlByte,mRoutingByte,mEffectByte,mMixByte,mReserved1E7;
    be<f32> mCenteredControl,mBiquadValue;
};
using SequenceCommandTrack_l=SequenceTrack;
/* The command view starts at sequenceBegin in the canonical SequenceTrack. */
inline SequenceCommandState_l* sequenceCommandState(SequenceTrack* track) {
    return reinterpret_cast<SequenceCommandState_l*>(&track->sequenceBegin);
}
struct SequenceCommandReader {
    gptr<const be<u32>> mVtable;
    u32 readCommand(SequenceCommandTrack_l* track,u32 emitNotes);
    void executeCommand(SequenceCommandTrack_l* track,u32 opcode,s32 first,s32 second);
    void emitNote(SequenceCommandTrack_l* track,u32 key,u32 velocity,s32 length,u32 mode);
    static u32 readTrackMask(const be<u8>* sequence,u32 offset,be<u32>* mask);
};
static_assert(sizeof(SequenceCommandState_l)==0xF0);
static_assert(offsetof(SequenceCommandState_l,mCursor)==4);
static_assert(offsetof(SequenceCommandState_l,mMuted)==0x5E);
static_assert(offsetof(SequenceCommandState_l,mWait)==0x60);
static_assert(offsetof(SequenceCommandState_l,mTranspose)==0xD8);
}}
