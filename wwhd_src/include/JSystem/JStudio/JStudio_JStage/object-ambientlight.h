// Ambient light adaptor, extending the merged prefix with its setup methods.
#pragma once
#include "JSystem/JStudio/JST_adaptors.h"
#include "JSystem/JStudio/JST_timeline.h"
#include "JSystem/JStudio/JStudio_JStage/stage-prefixes.h"
namespace JStudio_JStage {
struct TAdaptor_ambientLight : jst_adaptors::TAdaptor_ambientLight {
    void adaptor_do_prepare(const jst_adaptors::TObject*);
    void adaptor_do_begin(const jst_adaptors::TObject*);
    void adaptor_do_end(const jst_adaptors::TObject*);
    void adaptor_do_data(const jst_adaptors::TObject*, const void*, u32, const void*, u32);
};
struct AmbientVariablePrefix {
    be<u32> mCount; gptr<void> __vtbl;
    jst_timeline::TVariableValue mValues[4];
    gptr<const StageSystem> mSystem;
};
struct AmbientVTable {
    u8 prefix[0x24]; gfn getFlags; u8 gapFlags[4]; gfn setFlags;
    u8 gapColor[0x3C]; gfn getColor;
};
WWHD_SIZE(TAdaptor_ambientLight, 0x64);
WWHD_SIZE(AmbientVariablePrefix, 0x5C);
WWHD_OFFSET(AmbientVariablePrefix, mValues, 8);
WWHD_OFFSET(AmbientVariablePrefix, mSystem, 0x58);
WWHD_OFFSET(AmbientVTable, getColor, 0x6C);
}
