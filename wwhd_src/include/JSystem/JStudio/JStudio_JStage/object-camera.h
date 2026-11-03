// Camera adaptor setup and output callback types, extending the merged prefix.
#pragma once
#include "JSystem/JStudio/JStudio_JStage/object-actor.h"
namespace JStudio_JStage {
struct CameraVTable : ActorVTable {
    u8 gapViewPosition[0x4C]; gfn getViewPosition;
    u8 gapViewTarget[0x1C]; gfn getViewTarget;
};
WWHD_OFFSET(CameraVTable, getViewPosition, 0xDC);
WWHD_OFFSET(CameraVTable, getViewTarget, 0xFC);
struct TAdaptor_camera : jst_adaptors::TAdaptor_camera {
    gptr<jst_adaptors::StageObject> mParent;
    be<u32> mParentNodeID;
    void adaptor_do_prepare(const jst_adaptors::TObject*);
    void adaptor_do_begin(const jst_adaptors::TObject*);
    void adaptor_do_end(const jst_adaptors::TObject*);
    void adaptor_do_data(const jst_adaptors::TObject*, const void*, u32, const void*, u32);
    void adaptor_do_PARENT(u32, const void*, u32);
    void adaptor_do_PARENT_NODE(u32, const void*, u32);
    void adaptor_do_PARENT_ENABLE(u32, const void*, u32);
};
struct CameraVariablePrefix {
    be<u32> mCount; gptr<void> __vtbl;
    jst_timeline::TVariableValue mValues[11];
    gptr<const StageSystem> mSystem;
};
struct CameraValueOutput : jst_timeline::TOutput {
    be<s32> mValueIndex;
    gabi::ptmf<void (jst_adaptors::StageObject::*)(f32)> mSetter;
    gabi::ptmf<f32 (jst_adaptors::StageObject::*)()> mGetter;
    void output(f32, jst_adaptors::TAdaptor*) const;
};
WWHD_SIZE(TAdaptor_camera, 0xF8);
WWHD_OFFSET(TAdaptor_camera, mParent, 0xF0);
WWHD_OFFSET(TAdaptor_camera, mParentNodeID, 0xF4);
WWHD_SIZE(CameraVariablePrefix, 0xE8);
WWHD_OFFSET(CameraVariablePrefix, mValues, 8);
WWHD_OFFSET(CameraVariablePrefix, mSystem, 0xE4);
WWHD_SIZE(CameraValueOutput, 0x18);
WWHD_OFFSET(CameraValueOutput, mSetter, 8);
WWHD_OFFSET(CameraValueOutput, mGetter, 0x10);
}
