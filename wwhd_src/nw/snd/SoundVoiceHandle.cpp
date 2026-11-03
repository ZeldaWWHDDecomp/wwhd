/* Sound voice handle lifetime and forwarding. From WWHD's instructions only. */
#include "nw/snd/SoundVoiceHandle.h"
using namespace nw::snd;

void* allocateSoundVoiceHandle(u32 size);
void deleteSoundVoiceHandle(void* object);
static SecondarySoundVoicePool sSecondarySoundVoicePool;

/* 028A5CF8 */
SoundVoiceHandle* SoundVoiceHandle::construct(SoundVoiceHandle* handle) {
    gabi::AutoFrame frame{0x028A5CF8};
    if (!handle) handle = (SoundVoiceHandle*)allocateSoundVoiceHandle(sizeof(SoundVoiceHandle));
    if (handle) {
        be<f32>* sends = handle->parameters.remoteAuxiliarySend;
        for (u32 i = 0; i < 4; ++i) {
            handle->parameters.remoteMix[i] = 0.0f;
            sends[i] = 0.0f;
        }
        handle->parameters.remoteEnabled = 0;
        handle->secondary = nullptr;
        handle->primary = nullptr;
    }
    return handle;
}

/* 028A5D5C */
void SoundVoiceHandle::destroy(SoundVoiceHandle* handle,u32 flags) {
    gabi::AutoFrame frame{0x028A5D5C};
    if (handle && (flags & 1)) deleteSoundVoiceHandle(handle);
}

/* 028A5D70 */
void SoundVoiceHandle::setSecondaryPool(SoftwareVoiceManager* pool) {
    sSecondarySoundVoicePool.value = pool;
}

/* 028A5D7C */
void SoundVoiceHandle::primaryReleased(void* unused, SoundVoiceHandle* handle) {
    handle->primary = nullptr;
}

/* 028A5D88 */
void SoundVoiceHandle::secondaryReleased(void* unused, SoundVoiceHandle* handle) {
    handle->secondary = nullptr;
}

/* 028A5D94 */
void SoundVoiceHandle::reset(u32 value) {
    gabi::AutoFrame frame{0x028A5D94};
    priority = value;
    state = 1;
    parameters.initialize();
    sampleFormat = 1;
    sampleRate = 32000;
}

/* 028A5DE4 */
u32 SoundVoiceHandle::acquire(u32 value, u32 kind) {
    gabi::AutoFrame frame{0x028A5DE4};
    if (kind < 3) {
        HardwareVoice* voice = HardwareVoice::acquire(value,
            gabi::guest_fn((const void*)&primaryReleased), gabi::ea(this));
        if (!voice) return 0;
        // This call site stores the handle context in the canonical status word.
        voice->status = gabi::ea(this);
        voice->mixerSelect = kind;
        primary = voice;
        reset(value);
        return 1;
    }
    if (kind != 3) return 0;
    SoftwareVoiceManager* pool = sSecondarySoundVoicePool.value;
    if (pool) {
        SoftwareVoice* voice = pool->acquire(value,
            gabi::guest_fn((const void*)&secondaryReleased), gabi::ea(this));
        if (!voice) return 0;
        secondary = voice;
    }
    reset(value);
    return 1;
}

/* 028A5F00 */
void SoundVoiceHandle::release() {
    gabi::AutoFrame frame{0x028A5F00};
    HardwareVoice* voice = primary;
    state = 1;
    if (voice) {
        voice->release();
        primary = nullptr;
    }
    SoftwareVoice* auxiliary = secondary;
    if (auxiliary) {
        auxiliary->release();
        secondary = nullptr;
    }
}

/* 028A5F64 */
u32 SoundVoiceHandle::isActive() {
    HardwareVoice* voice = primary;
    if (voice) return voice->voice != nullptr;
    SoftwareVoice* auxiliary = secondary;
    if (auxiliary) return auxiliary->linked;
    return 0;
}

/* 028A5F9C */
void SoundVoiceHandle::setState(u32 value) {
    gabi::AutoFrame frame{0x028A5F9C};
    HardwareVoice* voice = primary;
    state = value;
    if (voice) {
        voice->playbackMode = value;
        if (value == 0) {
            primary->sampleRate = sampleRate;
            primary->sampleFormat = sampleFormat;
            HardwareVoice* destination = primary;
            be<u16>* coefficients = (be<u16>*)destination->adpcmCoefficients;
            for (u32 i = 0; i < 16; ++i) coefficients[i] = adpcmCoefficients[i];
        } else if (value == 1) {
            primary->clearRequests();
        }
    }
    SoftwareVoice* auxiliary = secondary;
    if (auxiliary) {
        auxiliary->playbackMode = value;
        if (value == 0) {
            secondary->sampleRate = sampleRate;
            secondary->sampleFormat = sampleFormat;
            SoftwareVoice* destination = secondary;
            be<u16>* coefficients = (be<u16>*)destination->adpcmCoefficients;
            for (u32 i = 0; i < 16; ++i) coefficients[i] = adpcmCoefficients[i];
        } else if (value == 1) {
            auxiliary->clearRequests();
        }
    }
}

/* 028A60EC */
void SoundVoiceHandle::appendRequest(VoiceRequest* request) {
    gabi::AutoFrame frame{0x028A60EC};
    HardwareVoice* voice = primary;
    if (voice) {
        voice->appendRequest(request);
        return;
    }
    SoftwareVoice* auxiliary = secondary;
    if (auxiliary) auxiliary->appendRequest(request);
}

/* 028A6110 */
void SoundVoiceHandle::setPriority(u32 value) {
    gabi::AutoFrame frame{0x028A6110};
    HardwareVoice* voice = primary;
    priority = value;
    if (voice) voice->setPriority(value);
    SoftwareVoice* auxiliary = secondary;
    if (auxiliary) auxiliary->setPriority(value);
}

/* 028A6170 */
void SoundVoiceHandle::setSendLevels(const be<u32>* values) {
    be<u32>* destination = (be<u32>*)parameters.remoteMix;
    for (u32 i = 0; i < 9; ++i) destination[i] = values[i];
}

/* 028A61A0 */
void SoundVoiceHandle::setLowPass(u32 enabled, u32 value) {
    parameters.lowPassEnabled = enabled;
    if (enabled) parameters.lowPassCutoff = value;
}

/* 028A61B4 */
void SoundVoiceHandle::setBiquad(u32 enabled, const FilterCoefficients* coefficients) {
    parameters.biquadEnabled = enabled;
    if (!enabled) return;
    parameters.biquadCoefficients[0] = coefficients->feedforward[0];
    parameters.biquadCoefficients[1] = coefficients->feedforward[1];
    parameters.biquadCoefficients[2] = coefficients->feedforward[2];
    parameters.biquadCoefficients[3] = coefficients->feedback[0];
    parameters.biquadCoefficients[4] = coefficients->feedback[1];
}

/* 028A61EC */
void SoundVoiceHandle::setRemoteFilter(u32 enabled, u32 value) {
    parameters.remoteFilterEnabled = enabled;
    if (enabled) parameters.remoteFilterCutoff = value;
}

/* 028A6200 */
void SoundVoiceHandle::synchronizeParameters() {
    HardwareVoice* voice = primary;
    be<u32>* output = nullptr;
    if (voice) output = (be<u32>*)voice->streamState;
    else {
        SoftwareVoice* auxiliary = secondary;
        if (!auxiliary) return;
        output = (be<u32>*)&auxiliary->mixLevels;
    }
    const be<u32>* input = (const be<u32>*)&parameters;
    for (u32 i = 0; i < 88; ++i) output[i] = input[i];
}

/* WWHD glue
 ext 028A6644 nw::snd::HardwareVoice* nw::snd::HardwareVoice::acquire(u32,u32,u32)
 ext 028A9DE0 nw::snd::SoftwareVoice* nw::snd::SoftwareVoiceManager::acquire(u32,u32,u32)
 ext 028A648C void nw::snd::HardwareVoice::clearRequests()
 ext 028A8C60 void nw::snd::SoftwareVoice::clearRequests()
 ext 028A6750 void nw::snd::HardwareVoice::appendRequest(nw::snd::VoiceRequest*)
 ext 028A8D48 void nw::snd::SoftwareVoice::appendRequest(nw::snd::VoiceRequest*)
 data sSecondarySoundVoicePool 101FCB3C
 ext 0273AD10 void* allocateSoundVoiceHandle(u32)
 ext 0273AF40 void deleteSoundVoiceHandle(void*)
 ext 028A6708 void nw::snd::HardwareVoice::release()
 ext 028A8D44 void nw::snd::SoftwareVoice::release()
 ext 028A67D8 void nw::snd::HardwareVoice::setPriority(u32)
 ext 028A8EB0 void nw::snd::SoftwareVoice::setPriority(u32)
*/
