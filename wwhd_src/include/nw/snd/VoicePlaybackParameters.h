/* Complete storage for the canonical hardware/software voice configuration prefix. */
#pragma once
#include "nw/snd/HardwareVoice.h"
namespace nw { namespace snd {
// The shared prefix ends at interpolationMode; the copied configuration includes two tail bytes.
struct VoicePlaybackParameters : VoiceParameters {
    u8 padding[2];
    void initialize();
};
WWHD_OFFSET(VoicePlaybackParameters, mainMix, 8);
WWHD_OFFSET(VoicePlaybackParameters, remoteMix, 0x128);
WWHD_OFFSET(VoicePlaybackParameters, remoteEnabled, 0x148);
WWHD_OFFSET(VoicePlaybackParameters, lowPassEnabled, 0x14C);
WWHD_OFFSET(VoicePlaybackParameters, biquadCoefficients, 0x150);
WWHD_OFFSET(VoicePlaybackParameters, lowPassCutoff, 0x15A);
WWHD_OFFSET(VoicePlaybackParameters, remoteFilterCutoff, 0x15C);
WWHD_SIZE(VoicePlaybackParameters, 0x160);
} }
