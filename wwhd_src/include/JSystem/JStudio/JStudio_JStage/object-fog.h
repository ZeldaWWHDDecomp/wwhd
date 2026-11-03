// Fog adaptor initialization and value outputs over the merged prefix.
#pragma once
#include "JSystem/JStudio/JStudio_JStage/object-ambientlight.h"
namespace JStudio_JStage {
struct TAdaptor_fog : jst_adaptors::TAdaptor_fog {
    void adaptor_do_prepare(const jst_adaptors::TObject*);
    void adaptor_do_begin(const jst_adaptors::TObject*);
    void adaptor_do_end(const jst_adaptors::TObject*);
    void adaptor_do_data(const jst_adaptors::TObject*, const void*, u32, const void*, u32);
};
struct FogVariablePrefix {
    be<u32> mCount; gptr<void> __vtbl;
    jst_timeline::TVariableValue mValues[6];
    gptr<const StageSystem> mSystem;
};
struct FogValueOutput : jst_timeline::TOutput {
    be<s32> mValueIndex;
    gabi::ptmf<void (jst_adaptors::StageObject::*)(f32)> mSetter;
    gabi::ptmf<f32 (jst_adaptors::StageObject::*)()> mGetter;
    void output(f32, jst_adaptors::TAdaptor*) const;
};
struct FogOutputs { FogValueOutput values[3]; };
WWHD_SIZE(FogOutputs,0x48); WWHD_OFFSET(FogOutputs,values,0);
struct FogVTable {
    u8 prefix[0x24]; gfn getFlags; u8 gapFlags[4]; gfn setFlags;
    u8 gapColor[0x6C]; gfn getColor;
};
WWHD_SIZE(TAdaptor_fog, 0x8C);
WWHD_SIZE(FogVariablePrefix, 0x84);
WWHD_OFFSET(FogVariablePrefix, mValues, 8);
WWHD_OFFSET(FogVariablePrefix, mSystem, 0x80);
WWHD_SIZE(FogValueOutput, 0x18);
WWHD_OFFSET(FogVTable, getColor, 0x9C);
}

// Canonical output table owned by the fog-adaptor TU.
extern JStudio_JStage::FogOutputs& sJStudioFogOutputs;
