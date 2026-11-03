/* Flag-ranked optional sound parameter records, reconstructed from WWHD readers. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
struct EnvelopeBytes { be<u8> values[5]; };
struct SoundOptionalParameters {
    be<u32> mFlags,mValues[1];
    u32 byteValue() const;
    s32 signedByteValue() const;
    f32 scalarValue() const;
    void readRouting(be<u8>* control,be<u8>* outputs,s32 outputCapacity) const;
    const EnvelopeBytes* envelope() const;
    u32 alternateByteValue() const;
    u32 highByteValue() const;
    u32 upperByteValue() const;
};
struct TrackOptionalParameters {
    be<u32> mIdentity,mFlags,mValues[1];
    u32 byteValue() const;
    u32 alternateByteValue() const;
    u32 trailingByteValue() const;
    u32 highByteValue() const;
    f32 scalarValue() const;
    const EnvelopeBytes* envelope() const;
};
struct NoteOptionalParameters {
    be<u32> mIdentity,mFlags,mValues[1];
    u32 keyValue() const;
    u32 byteValue() const;
    u32 alternateByteValue() const;
    f32 scalarValue() const;
    u32 booleanValue() const;
    u32 highByteValue() const;
    u32 upperByteValue() const;
    const EnvelopeBytes* envelope() const;
};
struct NoteEnvelopeReference {
    be<u32> mReserved,mKind;
    EnvelopeBytes mEnvelope;
    const EnvelopeBytes* envelope() const;
};
void initializeSoundEnvelopeDefaults();
void initializeNoteEnvelopeDefaults();
WWHD_SIZE(EnvelopeBytes,5);
WWHD_OFFSET(SoundOptionalParameters,mValues,4);
WWHD_OFFSET(TrackOptionalParameters,mFlags,4);
WWHD_OFFSET(TrackOptionalParameters,mValues,8);
WWHD_OFFSET(NoteOptionalParameters,mFlags,4);
WWHD_OFFSET(NoteEnvelopeReference,mEnvelope,8);
} }
