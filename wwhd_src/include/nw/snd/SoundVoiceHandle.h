/* Sound voice ownership facade using the merged hardware/software voice layouts. */
#pragma once
#include "wwhd.h"
#include "nw/snd/VoicePlaybackParameters.h"
#include "nw/snd/FilterPresets.h"
#include "nw/snd/SoftwareVoice.h"
namespace nw { namespace snd {
struct SecondarySoundVoicePool { gptr<SoftwareVoiceManager> value; };
WWHD_SIZE(SecondarySoundVoicePool, 4);
struct SoundVoiceHandle {
    be<u32> priority;
    be<u32> state;
    VoicePlaybackParameters parameters;
    be<u32> sampleFormat;          // copied to the voices by setState(0) (028A5F9C)
    be<u32> sampleRate;
    be<u16> adpcmCoefficients[16];
    u8 reserved[0x1C];
    gptr<HardwareVoice> primary;
    gptr<SoftwareVoice> secondary;
    static SoundVoiceHandle* construct(SoundVoiceHandle* handle);
    static void destroy(SoundVoiceHandle* handle,u32 flags);
    static void setSecondaryPool(SoftwareVoiceManager* pool);
    static void primaryReleased(void* unused, SoundVoiceHandle* handle);
    static void secondaryReleased(void* unused, SoundVoiceHandle* handle);
    void reset(u32 value);
    u32 acquire(u32 value, u32 kind);
    void release();
    void setState(u32 value);
    void appendRequest(VoiceRequest* request);
    void setSendLevels(const be<u32>* values);
    void setLowPass(u32 enabled, u32 value);
    void setBiquad(u32 enabled, const FilterCoefficients* coefficients);
    void setRemoteFilter(u32 enabled, u32 value);
    void synchronizeParameters();
    u32 isActive();
    void setPriority(u32 value);
};
WWHD_OFFSET(SoundVoiceHandle, state, 4);
WWHD_OFFSET(SoundVoiceHandle, parameters, 8);
WWHD_OFFSET(SoundVoiceHandle, sampleFormat, 0x168);
WWHD_OFFSET(SoundVoiceHandle, sampleRate, 0x16C);
WWHD_OFFSET(SoundVoiceHandle, adpcmCoefficients, 0x170);
WWHD_OFFSET(SoundVoiceHandle, primary, 0x1AC);
WWHD_OFFSET(SoundVoiceHandle, secondary, 0x1B0);
WWHD_SIZE(SoundVoiceHandle, 0x1B4);
} }
