/* Playback defaults using the canonical configuration layout. Matrix data stays in the image. */
#include "nw/snd/VoicePlaybackParameters.h"
using namespace nw::snd;
static const MixInput sMainMixDefaults;
static const MixInput sAuxiliaryMixDefaults;

/* inlined into VoicePlaybackParameters::initialize */
static void copyVoiceMixWords(void* destination,const MixInput* source) {
    be<u32>* output = (be<u32>*)destination;
    const be<u32>* input = (const be<u32>*)source;
    for (u32 i = 0; i < 24; ++i) output[i] = input[i];
}

/* 028A5C04 */
void VoicePlaybackParameters::initialize() {
    volume = 1.0f;
    pitch = 1.0f;
    copyVoiceMixWords(&mainMix,&sMainMixDefaults);
    copyVoiceMixWords(&gamepadMix,&sAuxiliaryMixDefaults);
    copyVoiceMixWords(&secondGamepadMix,&sAuxiliaryMixDefaults);
    be<f32>* sends = remoteAuxiliarySend;
    for (u32 i = 0; i < 4; ++i) {
        remoteMix[i] = 0.0f;
        sends[i] = 0.0f;
    }
    remoteEnabled = 0;
    remoteFilterEnabled = 0;
    lowPassCutoff = 0;
    biquadCoefficients[0] = 0;
    biquadCoefficients[2] = 0;
    biquadEnabled = 0;
    biquadCoefficients[3] = 0;
    interpolationMode = 0;
    remoteFilterCutoff = 0;
    biquadCoefficients[4] = 0;
    lowPassEnabled = 0;
    biquadCoefficients[1] = 0;
    gamepadSurround = 0;
}

/* WWHD glue
 data sMainMixDefaults 101870C8
 data sAuxiliaryMixDefaults 10187128
*/
