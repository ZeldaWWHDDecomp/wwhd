// Light adaptor lifecycle and direction outputs over the merged adaptor prefix.
#pragma once
#include "JSystem/JStudio/JST_adaptors.h"
#include "JSystem/JStudio/JST_timeline.h"
#include "JSystem/JStudio/jstudio-control.h"
namespace JStudio_JStage {
struct TAdaptor_light : jst_adaptors::TAdaptor_light {
    void adaptor_do_prepare(const jst_adaptors::TObject*);
    void adaptor_do_begin(const jst_adaptors::TObject*);
    void adaptor_do_end(const jst_adaptors::TObject*);
    void adaptor_do_data(const jst_adaptors::TObject*,const void*,u32,const void*,u32);
    void adaptor_do_FACULTY(u32,const be<u32>*,u32);
    void adaptor_do_ENABLE(u32,const be<u32>*,u32);
};
struct LightVariablePrefix {
    be<u32> mCount; gptr<void> __vtbl;
    jst_timeline::TVariableValue mValues[13]; gptr<const void> mSystem;
};
struct LightDirectionOutput : jst_timeline::TOutput {
    be<s32> mValueIndex; be<u32> mDirection;
    void output(f32,jst_adaptors::TAdaptor*) const;
};
struct LightVtable {
    u8 prefix[0x24]; gfn getFlags; u8 gapFlags[4]; gfn setFlags;
    u8 gapType[0x44]; gfn setLightType; u8 gapPosition[4]; gfn getPosition;
    u8 gapColor[0xC]; gfn getColor; u8 gapDirection[0x2C]; gfn getDirection;
};
struct LightBeginFrame {
    jst_adaptors::Color color,packed;
    jst_adaptors::Vec source[2],target,transformed[2];
};
WWHD_SIZE(TAdaptor_light,0x11C); WWHD_SIZE(LightVariablePrefix,0x110); WWHD_OFFSET(LightVariablePrefix,mValues,8); WWHD_OFFSET(LightVariablePrefix,mSystem,0x10C);
WWHD_SIZE(LightDirectionOutput,12); WWHD_OFFSET(LightDirectionOutput,mDirection,8);
WWHD_OFFSET(LightVtable,getPosition,0x7C); WWHD_OFFSET(LightVtable,getColor,0x8C); WWHD_OFFSET(LightVtable,getDirection,0xBC);
WWHD_SIZE(LightBeginFrame,0x44);
}
