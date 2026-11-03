// Sound adaptor additions over the merged adaptor view, following tww object-sound.cpp.
#pragma once
#include "JSystem/JStudio/JST_adaptors.h"
#include "JSystem/JStudio/JST_timeline.h"
namespace JStudio_JAudio {
struct TAdaptor_sound : jst_adaptors::TAdaptor_sound {
    struct TVVOOn_BEGIN_FADE_IN_ : jst_timeline::TOutput {};
    struct TVVOOn_END_FADE_OUT_ : jst_timeline::TOutput {};
    struct TVVOSetValue_ : jst_timeline::TOutput {
        be<s32> mValueIndex;
        gabi::ptmf<void (jst_adaptors::JAISound::*)(f32, u32)> mSetFunc;
        void output(f32, jst_adaptors::TAdaptor*) const;
    };
    void adaptor_do_prepare(const jst_adaptors::TObject*);
    void adaptor_do_SOUND(u32, const void*, u32);
    void adaptor_do_LOCATED(u32, const void*, u32);
};
struct SoundVariablePrefix {
    be<u32> mCount;
    gptr<void> __vtbl;
    jst_timeline::TVariableValue mValues[11];
};
WWHD_SIZE(TAdaptor_sound, 0x104);
WWHD_SIZE(TAdaptor_sound::TVVOSetValue_, 0x10);
WWHD_OFFSET(TAdaptor_sound::TVVOSetValue_, mValueIndex, 4);
WWHD_OFFSET(TAdaptor_sound::TVVOSetValue_, mSetFunc, 8);
WWHD_SIZE(SoundVariablePrefix, 0xE4);
WWHD_OFFSET(SoundVariablePrefix, mValues, 8);
}
