// JStudio object/adaptor extensions use the merged timeline's value and object prefixes.
#pragma once
#include "JSystem/JStudio/jstudio-control.h"
namespace JStudio {
using TVariableValue = jst_timeline::TVariableValue;
struct TObject;
struct TAdaptor : jst_timeline::TAdaptor {
    enum Operation : u32 { OP_VOID=1, OP_IMMEDIATE=2, OP_TIME=3, OP_FVR_NAME=0x10, OP_FVR_INDEX=0x12 };
    struct TSetVariableValue_immediate { be<u32> index; be<f32> value; };
    static void adaptor_setVariableValue_VOID_(TAdaptor*, TObject*, u32, const void*, u32);
    static void adaptor_setVariableValue_IMMEDIATE_(TAdaptor*, TObject*, u32, const void*, u32);
    static void adaptor_setVariableValue_TIME_(TAdaptor*, TObject*, u32, const void*, u32);
    static void adaptor_setVariableValue_FVR_NAME_(TAdaptor*, TObject*, u32, const void*, u32);
    static void adaptor_setVariableValue_FVR_INDEX_(TAdaptor*, TObject*, u32, const void*, u32);
    void adaptor_setVariableValue(TObject*, u32, u32, const void*, u32);
    void adaptor_setVariableValue_n(TObject*, const be<u32>*, u32, u32, const void*, u32);
    void adaptor_setVariableValue_immediate(const TSetVariableValue_immediate*);
    void adaptor_setVariableValue_Vec(const be<u32>*, const Vec&);
    void adaptor_getVariableValue_Vec(Vec*, const be<u32>*) const;
    struct Color { be<u8> components[4]; };
    void adaptor_setVariableValue_GXColor(const be<u32>*, const Color&);
    void adaptor_getVariableValue_GXColor(Color*, const be<u32>*) const;
    void adaptor_do_prepare(const TObject*);
    void adaptor_do_begin(const TObject*);
    void adaptor_do_end(const TObject*);
    void adaptor_do_update(const TObject*, u32);
    void adaptor_do_data(const TObject*, const void*, u32, const void*, u32);
};
struct TObject : jst_timeline::TObject {
    void do_begin();
    void do_end();
    void do_wait(u32);
    void do_data(const void*, u32, const void*, u32);
};
struct BoolOutput {
    gptr<void> __vtbl;
    gabi::ptmf<void (TAdaptor::*)(u32,const void*,u32)> mFunc;
};
WWHD_SIZE(BoolOutput,0xC); WWHD_OFFSET(BoolOutput,mFunc,4);
struct AdaptorMethod { be<u32> adjust; gfn target; };
struct AdaptorVTable { AdaptorMethod methods[40]; };
enum AdaptorMethodSlot : u32 { ADAPTOR_DESTROY=1, ADAPTOR_PREPARE=2, ADAPTOR_BEGIN=3, ADAPTOR_END=4, ADAPTOR_UPDATE=5, ADAPTOR_DATA=6 };
// The subclasses add no state to the timeline object itself.
struct TObject_actor : TObject { void do_paragraph(u32, const void*, u32); };
struct TObject_ambientLight : TObject { void do_paragraph(u32, const void*, u32); };
struct TObject_camera : TObject { void do_paragraph(u32, const void*, u32); };
struct TObject_fog : TObject { void do_paragraph(u32, const void*, u32); };
struct TObject_light : TObject { void do_paragraph(u32, const void*, u32); };
struct TObject_message : TObject { void do_paragraph(u32, const void*, u32); };
struct TObject_particle : TObject { void do_paragraph(u32, const void*, u32); };
struct TObject_sound : TObject { void do_paragraph(u32, const void*, u32); };
WWHD_SIZE(TAdaptor,0xC); WWHD_SIZE(TObject,0x38); WWHD_SIZE(AdaptorMethod,8);
}

// The shared default output object (104B605C), owned by jstudio-object.cpp; adaptors use it when a
// variable value has no output of its own.
extern jst_timeline::TOutput& sJStudioOutputNone;
// The NaN default value (101FCBC8) of the sound and particle adaptors' immediate tables; owned by
// JStudio_JAudio/object-sound.cpp.
extern be<f32>& sJStudioQuietNaN;
