// WWHD's sound-handle prefix and its translation unit's text helpers.
// Layouts reconstructed from the local instructions; no external layouts used.
#pragma once
#include "sead/FileDevice.h"
#include "nw/snd/BasicSound.h"
namespace sead {
// The owning sound layer is not merged yet; this is only its observed handle prefix.
struct AudioHandle_l {
    gptr<nw::snd::BasicSound> mSound;
    void stop(s32 fadeFrames);
    u32 hasSound() const;
    u32 soundId() const;
};
namespace audio_handle_text {
struct Text : SafeText {
    void destroy(u32 flags);
    void assureTerminationImpl();
};
struct TextBuffer : SafeTextBuffer {
    void destroy(u32 flags);
    void assureTerminationImpl();
};
}
}
WWHD_OFFSET(sead::AudioHandle_l,mSound,0);
WWHD_SIZE(sead::AudioHandle_l,4);
